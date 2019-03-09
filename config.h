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

	unsigned int euler_range_max;

	//PID parameter
	float pr_angle_kp,     pr_angle_ki,     pr_angle_kd;       //Pitch and roll, angle pid.
	float pr_velocity_kp,  pr_velocity_ki,  pr_velocity_kd;    //Pitch and roll, angle velocity pid.
	float yaw_angle_kp,    yaw_angle_ki,    yaw_angle_kd; 	   //Yaw, angle pid.
	float yaw_velocity_kp, yaw_velocity_ki, yaw_velocity_kd;   //Yaw, angle velocity pid.
};

//----- functions -------
void config_init(void);
void config_write(void);
struct config * config_get(void);
#endif
