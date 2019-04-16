#include <stdint.h>
#include <stdbool.h>
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

#include <led.h>
#include <systick.h>
#include <mpu6050.h>

enum status system_status = on_init;
//----------- init systick ------
void systick_init(void)
{
	SysTickPeriodSet(1000000);
	IntMasterEnable();
	SysTickIntEnable();
	SysTickEnable();
}


//------- set system status -------
void status_set(enum status s)
{
	system_status = s;
}
//------- get system status -------
enum status status_get(void)
{
	return system_status;
}
// ----- systick int handler-------
// declared in startup.s file 
void SysTickIntHandler()
{
	static unsigned int count = 0;
	count++;
	led_run();
	if(system_status == on_init){
		// do nothing,
		// it should be switch to ready in main.c
		// if initialization spend to much time 
		// switch to failure
		if(count > 5000){
			system_status = failure;
		}
		return;
	}
	// update attitude
	attitude_update();
	if(system_status == ready){
		//TODO: 
		// check pwm_input status and
		// switch to unlocked
		
		//TODO:update IMU data
		
	}
	
}
