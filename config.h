#ifndef __CONFIG_H_
#define __CONFIG_H_

//----config struct------
struct config
{
	unsigned short int pitch_center_shift;
	unsigned short int roll_center_shift;
	unsigned short int yaw_center_shift;
	unsigned short int throttle_minimum_shift;
};

//----- functions -------
void config_init(void);
void config_write(void);
struct config * config_get(void);
#endif