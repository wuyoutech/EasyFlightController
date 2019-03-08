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

enum status system_status = on_init;
//----------- init systick ------
void systick_init(void)
{
	SysTickPeriodSet(100000);
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
	led_run();
	if(system_status == on_init){
		// do nothing,
		// it should be switch to ready in main.c
		// if initialization spend to much time 
		// switch to failure
		if(count > 5000){
			system_status = failure;
		}
	}else if(system_status == ready){
		//TODO: 
		// check pwm_input status and
		// switch to unlocked
		
		//TODO:update IMU data
		
	}
	count++;
	if(count > 3000){
		system_status = unlocked;
	}
	if(count >15000){
		system_status = failure;
	}
}
