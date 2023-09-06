
/*
 * Rbtanno.hpp
 *
 *  Created on: May 10, 2022
 *      Author: arios
 */


#include <map>
#include <string>
#include <fcntl.h>
#include <sys/mman.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include "Rbtanno_joint.hpp"
#include "mosquitto.h"

#include "communication-layer.cpp"

using json = nlohmann::json;
static std::map<std::string, int> joint_base_addresses = { { "J1", 0 }, { "J2",
		6 }, { "J3", 12 }, { "J4", 18 }, { "J5", 24 }, { "J6", 30 } };


class Rbtanno {
private:
	int32_t *axi_device;
	json config_json;
	json trajectory_json;
	std::map<std::string, Rbtanno_joint> joints;
	std::string config_file_path;
	//Logger *pLogger = NULL;
	std::string msg;

	/*
	 * Save the current robot state that means all joints actual position.
	 */
	void save_joints_positions();

public:
	/*
	 * Constructor
	 * @param axi_address: Base AXI address where the AXI4 device is mapped. This device is the interface
	 * between the software and the robot controller deployed on the FPGA.
	 * @param config_file_path: String path of the json file used to configure de robot joints and where
	 * the trayectories are described.
	 */
	Rbtanno(uint32_t axi_address, std::string config_file_path);
	~Rbtanno();

	/*
	 * Configure all the joints which name is passed in a list.
	 * @param joint_list: Vector of string that contains the joint names to be configured
	 */
	void config_joints(std::vector<std::string> joint_list);



	/*
	 * Execute a movement received by command.
	 * @return: 0 if no errors. != Error code
	 */
	Point MvToPoint_Class(Point Punto);

	/*
	 * Execute all trajectories received by command.
	 * @return: 0 if no errors. != Error code
	 */
	Trajectory Trajectory_Class(Trajectory Trayectoria);


	/*
	 * Load trajectory file.
	 * @param: Trajectory filepath name
	*/
	void load_trajectories(std::string traj_file_path);

	/*
	 * Search home for all joints
	 * 
	*/
	void home(void); 
};