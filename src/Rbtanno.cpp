/*
 * Rbtanno.cpp
 *
 *  Created on: May 10, 2022
 *      Author: arios
 */

#include "Rbtanno.hpp"

#include <stdint.h>
#include <math.h>
#include <map>
#include <stdio.h>
#include <sstream>
#include <string>
#include <fcntl.h>
#include <sys/mman.h>
#include <fstream>
#include <iostream>

#define AXI_ADDRESS 0x40000000
#define LOWER_VELOCITY 0x00FFFF
#define ACC_DEC_LIMIT_PERCENTAGE 5
#define HOME_VELOCITY 0x000FA0


Rbtanno::Rbtanno(uint32_t axi_address, std::string config_file_path)
{
	//pLogger = Logger::getInstance();
	// Open the memory map and create a pointer to the AXI device
	int32_t mem_pointer = open("/dev/mem", O_RDWR | O_SYNC);
	axi_device = (int32_t *)mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, mem_pointer, axi_address);
	//LOG_DEBUG("Memory map openned and Axi device mapped.");
	//this->msg = "Opening configuration file " + config_file_path + ".";
	//LOG_DEBUG(this->msg);
	// Open the configuration file
	this->config_file_path = config_file_path;
	std::ifstream config_file(this->config_file_path, std::ios::in);
	// Parse with json library
	this->config_json = json::parse(config_file);
	config_file.close();
}

Rbtanno::~Rbtanno(){};

void Rbtanno::config_joints(std::vector<std::string> joint_list)
{
	// Take all the joint initial positions saved in the config file
	json joint_initial_positions = this->config_json["Joint_initial_positions"];
	// Create the instances for the joints specified in the joint_list
	for (auto j_idx : joint_list)
	{
		this->joints.emplace(j_idx, Rbtanno_joint(joint_base_addresses[j_idx], this->axi_device));
		this->joints[j_idx].configure_initial_position(joint_initial_positions[j_idx]);
	}
}


Point Rbtanno::MvToPoint_Class(Point Punto)
{
	// No error (TODO??)
	//uint8_t error_code = 0; 
	Point result = Punto;

	int num_motors = Punto.coordinates.size()-1;
	//Target will save the coordinates of target point. It has dimension num_motors.
	int32_t Target[num_motors];
	//Velocity_isi will store the velocity value sent as a coordinate, in last position.
	uint32_t velocity_isi = Punto.coordinates[num_motors];
	result.coordinates[num_motors]=double(velocity_isi);

	//Now we store the reference in target
	for (int motor_idx = 0; motor_idx < num_motors; motor_idx++){
		Target[motor_idx]=angle_to_ref(motor_idx+1,Punto.coordinates[motor_idx]);
	} 

	//Now, for every joint, we declare next_step_value as the difference between Target and current coordinates
	//then, we move each joint
	int jointdex =0;
	for (auto &j : this->joints)
	{
		int32_t next_step_value = Target[jointdex]-j.second.get_position();
		//int32_t next_step_value = Target[jointdex];  //In case of incremental references, i'll leave this one line
		if (next_step_value!=0) j.second.move(next_step_value, velocity_isi);
		//result.coordinates[jointdex]=ref_to_angle(jointdex+1,j.second.get_position());
		jointdex ++;
	}

	// Wait until all joints have finished their movements.
	for (auto &j : this->joints)
	{
		while (j.second.is_moving())
			;
	}

	// Save robot state
	this->save_joints_positions();
	//std::cout << result.to_json() << std::endl;

	jointdex =0;
	for (auto &j : this->joints)
	{
		result.coordinates[jointdex]=ref_to_angle(jointdex+1,j.second.get_position());
		jointdex ++;
	}

	return result;
}

Trajectory Rbtanno::Trajectory_Class(Trajectory Trayectoria)
{
	//uint8_t error_code = 0; // No error
	Trajectory Output = Trayectoria;
	Point Punto_objetivo;
	int num_steps = Trayectoria.points.size();

	for (int step_idx = 0; step_idx < num_steps; step_idx++){
		Punto_objetivo=Trayectoria.points[step_idx];
		Output.points[step_idx] = this->MvToPoint_Class(Punto_objetivo);
	} 
	return Output;
}

void Rbtanno::save_joints_positions()
{
	// Save the current robot position in the json file
	json joint_initial_positions = this->config_json["Joint_initial_positions"];
	for (auto &j : this->joints)
	{
		joint_initial_positions[j.first] = j.second.get_position();
	}
	std::ofstream config_file_out(this->config_file_path, std::ios::out);
	this->config_json["Joint_initial_positions"] = joint_initial_positions;
	config_file_out << this->config_json << std::endl;
	config_file_out.close();
}

void Rbtanno::load_trajectories(std::string traj_file_path)
{
	// Open the trajectory file
	std::ifstream trajectory_file(traj_file_path, std::ios::in);
	// Parse with json library
	this->trajectory_json = json::parse(trajectory_file);
	trajectory_file.close();
}

void Rbtanno::home(void)
{
	// Search home for each joints
	for (auto &j : this->joints)
	{
		j.second.search_home();
	}
	// Wait until all joints are at the home position
	for (auto &j : this->joints)
	{
		while (j.second.is_moving())
			;
	}
	// Save robot state
	this->save_joints_positions();
}
