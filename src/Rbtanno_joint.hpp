
/*
 * Rbtanno_joint.hpp
 *
 *  Created on: May 10, 2022
 *      Author: arios
 */



#include <stdint.h>
#include <math.h>
#include <map>
#include <stdio.h>
#include <sstream>

//#include "communication-layer.cpp"

#define AXI_ADDRESS 0x40000000
#define LOWER_VELOCITY 0x00FFFF
#define ACC_DEC_LIMIT_PERCENTAGE 5
#define HOME_VELOCITY 0x000FA0


// Mapping of joint base register and actual position AXI register
static std::map<uint32_t, uint32_t> pos_AXI_reg_offset = {{0, 1}, {6, 2}, {12, 3}, {18, 4}, {24, 5}, {30, 6}};



class Rbtanno_joint
{
private:
	// Joint registers to control the motor movements
	uint8_t config_reg_addr;
	uint8_t spike_ref_reg_addr;
	uint8_t isi_reg_addr;
	uint8_t position_reg_addr;
	uint8_t acc_dec_factor_reg_addr;
	uint8_t acc_dec_limit_reg_addr;
	//Logger *pLogger = NULL;
	std::string msg;
	
	// Memory pointer to write and read the information from/to the app and the FPGA controller using the AXI4
	int32_t *axi;

	// Joint auto configuration
	bool auto_config;

	/*
	 * Configure the acceleration/deceleration ramps using HOME_VELOCITY as the lowest velocity and ACC_DEC_LIMIT_PERCENTAGE
	 * as the limits point for the ramps.
	 * @param num_spikes: Number of spikes to be fired to perform the movement.
	 * @param velocity: Movement velocity encoded in the ISI (Inter-Spike-Inteval) in velocity*10ns.
	 */
	void configure_acc_dec_ramps_auto(int32_t num_spikes, uint32_t velocity);

	/*
	 * Get the joint status value
	 * @return: The value of the status register of the joint
	 */
	uint32_t get_status();

public:
	/*
	 * Constructor
	 * @param base_address: The configuration register for the joint
	 * @param axi_address: Memory address where the AXI4 module is mapped in memory.
	 */
	Rbtanno_joint(uint8_t base_address, int32_t *axi_address);
	Rbtanno_joint();
	~Rbtanno_joint();
	/*
	 * Perform a movement for an specific number of spikes using a specific velocity
	 * @param num_spikes: Number of spikes to be sent to the motor
	 * @param velocity: Movement velocity encoded in the ISI (Inter-Spike-Inteval) in velocity*10ns
	 */
	void move(int32_t num_spikes, uint32_t velocity);
	/*
	 * Stop the current joint movement.
	 */
	void stop();
	/*
	 * Move the joint to the home position (actual_position == 0)
	 */
	void search_home();
	/*
	 * Return the joint actual position
	 */
	int32_t get_position();
	/*
	 * Configure the joint initial position.
	 * @param position: Absolute position (in spikes) to be set as initial joint position.
	 */
	void configure_initial_position(int32_t position);

	/*
	 * Configure the joint defining acceleration/deceleration ramps and the limit of spikes for the ramps.
	 * @param acc_dec_factor: Increasing/decreasing factor for the ISI value. With this factor we can change the acceleration/deceleration ramps.
	 * @param acc_dec_limit: Limit number of spikes used to define the acceleration/deceleration ramps.
	 */
	void configure_acc_dec_ramps(uint32_t acc_dec_factor, uint32_t acc_dec_limit);

	/*
	 * Check if the actual movement is finish
	 * @return: True if the joint is moving. False if the joint is not moving.
	 */
	bool is_moving();
};