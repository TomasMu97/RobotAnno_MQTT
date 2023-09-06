/*
 * Rbtanno_joint.cpp
 *
 *  Created on: May 10, 2022
 *      Author: arios
 */


#include "Rbtanno_joint.hpp"



Rbtanno_joint::Rbtanno_joint(uint8_t base_address, int32_t *axi_address) {
	this->config_reg_addr = base_address;
	this->spike_ref_reg_addr = this->config_reg_addr + 1;
	this->isi_reg_addr = this->config_reg_addr + 2;
	this->position_reg_addr = this->config_reg_addr + 3;
	this->acc_dec_factor_reg_addr = this->config_reg_addr + 4;
	this->acc_dec_limit_reg_addr = this->config_reg_addr + 5;
	this->axi = axi_address;
	this->auto_config = true;

	//this->msg = "Created new joint with 0x " + std::to_string(this->config_reg_addr) + ".";
	//std::cout << msg << std::endl;
}

Rbtanno_joint::Rbtanno_joint() {

}

Rbtanno_joint::~Rbtanno_joint() {

}

void Rbtanno_joint::move(int32_t num_spikes, uint32_t velocity) {
	//The num_spikes sign determines the movement direction.
	//std::cout << "Entering movement" << std::endl;
	this->axi[0] = (0x00000000 | (0x00FFFFFF & num_spikes))
			| (this->spike_ref_reg_addr << 24);
	// Set the velocity
	this->axi[0] = (0x00000000 | (0x00FFFFFF & velocity))
			| (this->isi_reg_addr << 24);
	
	if (this->auto_config) {
		// Set the acc/dec ramps
		//this->configure_acc_dec_ramps_auto(num_spikes, velocity);
		this->configure_acc_dec_ramps(2000,2000);
	}
	
	// Start the movement
	this->axi[0] = 0x00000001 | (this->config_reg_addr << 24);

	// Enable joint auto configuration
	this->auto_config = true;
}

void Rbtanno_joint::stop() {
	this->axi[0] = 0x00000000 | (this->config_reg_addr << 24);
}

void Rbtanno_joint::search_home() {
	if (this->get_position() != 0) {
		int32_t mov_to_home = (-1) * this->get_position();
		this->move(mov_to_home, HOME_VELOCITY);
	}
}

int32_t Rbtanno_joint::get_position() {
	// Get the status gegister value where the joint position is saved from bit 23 - 0.
	uint32_t status = this->get_status();
	// This joint_pos is actually a 24 bit number. So let see the number sign and extend it to 32 bit integer.
	int32_t joint_pos = ((0x00800000 & status) != 0) ?
					(0xFF000000 | status) : (0x00FFFFFF & status);
	return joint_pos;
}

void Rbtanno_joint::configure_initial_position(int32_t position) {
	// Set the position as initial position in the hardware controller
	this->axi[0] = (0x00000000 | (0x00FFFFFF & position))
			| (this->position_reg_addr << 24);
}

/*
Tal y como está planteado esto, si el movimiento es extenso, la rampa de aceleración tarda mucho en completarse.
*/
void Rbtanno_joint::configure_acc_dec_ramps_auto(int32_t num_spikes,
		uint32_t velocity) {

	// Calculate the acc/dec limit
	// acc_dec_limit = num_spikes * 0.05
	// acc_dec_limit(1000) = 50
	// acc_dec_limit(100000) = 5000
	uint32_t acc_dec_limit = ceil(abs(num_spikes) * ACC_DEC_LIMIT_PERCENTAGE / 100.0);

	// Set the acc/dec factor
	//acc_dec_factor = (65535 - velocity) / (num_spikes*0.05) //POR QUE?
	//acc_dec_factor(1000,3000) = 1251
	//acc_dec_factor(100000,3000) = 13
	uint32_t acc_dec_factor = ceil((LOWER_VELOCITY - velocity) / (float) acc_dec_limit);

	this->axi[0] = (0x00000000 | (0x00FFFFFF & acc_dec_factor)) | (this->acc_dec_factor_reg_addr << 24);
	//this->axi[0] = (0x00000000 | (0x00FFFFFF & acc_dec_factor)) | (this->acc_dec_factor_reg_addr << 24);
	// Set the acce/dec limits
	this->axi[0] = (0x00000000 | (0x00FFFFFF & acc_dec_limit))  | (this->acc_dec_limit_reg_addr << 24);
}


void Rbtanno_joint::configure_acc_dec_ramps(uint32_t acc_dec_factor,
		uint32_t acc_dec_limit) {
	// Set the acc/dec factor = RETARDO EN TICKS DE RELOJ
	this->axi[0] = (0x00000000 | (0x00FFFFFF & acc_dec_factor))
			| (this->acc_dec_factor_reg_addr << 24);
	// Set the acce/dec limits = NUMERO DE STEPS QUE DURA LA ACELERACION
	this->axi[0] = (0x00000000 | (0x00FFFFFF & acc_dec_limit))
			| (this->acc_dec_limit_reg_addr << 24);
	// Disable joint auto configuration
	this->auto_config = false;
}

uint32_t Rbtanno_joint::get_status(){
	// Get the status register value
	uint32_t status = this->axi[AXI_ADDRESS
						+ pos_AXI_reg_offset[this->config_reg_addr]];
	return status;
}

bool Rbtanno_joint::is_moving(){
	// Get the status register value where the moving bit state is mapped on bit 24.
	uint32_t status = this->get_status();
	//this->msg = "Status " + std::to_string(status) + ".";
	//LOG_DEBUG(this->msg);
	bool moving = ((0x01000000 & status) == 0) ? true : false;
	return moving;
}
