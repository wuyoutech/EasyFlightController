#ifndef __CONFIG_H_
#define __CONFIG_H_

//----config struct------
struct config
{
	//----- channel mapping -----
	unsigned char pitch_channel_number;
	unsigned char roll_channel_number;
	unsigned char yaw_channel_number;
	unsigned char throttle_channel_number;
	//----- input calibration ---
	short int pitch_center_shift;
	short int roll_center_shift;
	short int yaw_center_shift;
	short int throttle_minimum_shift;
	//------- unlock range -------
	unsigned short int unlock_range_minimum;
	unsigned short int unlock_range_maximum;
	
	unsigned int euler_range_max;
};

//----- functions -------
void config_init(void);
void config_write(void);
struct config * config_get(void);
void config_reset(void);
#endif
