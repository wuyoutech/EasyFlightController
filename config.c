#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"
#include "driverlib/eeprom.h"
#include "driverlib/pwm.h"

#include <config.h>

//---config file address--
unsigned int * config_address;
//---config file size-----
unsigned short int config_size;
//---  crc32 table -------
static unsigned int table[256];

static unsigned int bitrev(unsigned int input,unsigned char bw)
{
	unsigned int var = 0;
	for(int i = 0;i < bw;i++){
		if(input & 0x01){
			var |= 1<<(bw-i-1);
		}
		input >>= 1;
	}
	return var;
}
//---- calculate crc32 ----
unsigned int crc32(void * input, int len)
{
	unsigned int crc = 0xFFFFFFFF;
	unsigned char index;
	unsigned char * pch = (unsigned char * )input;
	for(int i = 0; i< len;i++){
		index = (unsigned char )(crc^*pch);
		crc = (crc>>8)^table[index];
		pch++;
	}
	return crc;
}
//---- init crc32 table ---
void crc32_init(unsigned int poly)
{
	unsigned int c;
	poly = bitrev(poly,32);
	for(int i = 0;i<256;i++){
		c = i;
		for(int j = 0;j<8;j++){
			if(c & 1)
				c = poly^(c>>1);
			else
				c = c>>1;
		}
		table[i] = c;
	}
}
//------- init eeprom -----
void eeprom_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
	EEPROMInit();
}
//- write config to eeprom -
void config_write(void)
{
	EEPROMProgram(config_address,0x0000,config_size);	//write config
	unsigned int crc_result = crc32(config_address,config_size);	//calculate crc32
	EEPROMProgram(&crc_result,config_size,0x04);		//write crc32 result
}
//- get config file -------
struct config * config_get(void)
{
	return (struct config *)config_address;
}
// -----reset config ------
void config_reset(void)
{
	struct config *  config = (struct config *)config_address;
	//factory settings value
	config -> pitch_center_shift = 0;
	config -> roll_center_shift = 0;
	config -> yaw_center_shift = 0;
	config -> throttle_minimum_shift = 0;
	
	config -> pitch_channel_number = 0;
	config -> roll_channel_number = 1;
	config -> yaw_channel_number = 2;
	config -> throttle_channel_number = 3;
	
	config -> unlock_range_minimum = 1000;
	config -> unlock_range_maximum = 1000;

	config -> euler_range_max = 30;
	
	config_write();
}
//----- init config module ---
void config_init(void)
{
	crc32_init(0x4c11db7);	//init crc32 table
	eeprom_init();			//init eeprom
	
	//make sure config file size % 4 = 0
	if(sizeof(struct config) % 4 > 0){
		config_size = (sizeof(struct config) / 4 + 1) * 4;	
	}else{
		config_size = sizeof(struct config);
	}
	config_address = malloc(config_size);
	//read config from eeprom
	EEPROMRead(config_address,0x0000,config_size);
	//check crc result
	unsigned int crc_saved;
	EEPROMRead(&crc_saved,config_size,0x04);
	//calculate config file crc
	unsigned int crc_result;
	crc_result = crc32(config_address,config_size);
	if(crc_result != crc_saved){
		config_reset();
	}
}
