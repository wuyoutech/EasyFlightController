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

#include "led.h"

enum status
{
	on_init,
	ready,
	unlocked,
	failure
};

enum status system_status = on_init;

void systick_init(void)
{
	SysTickPeriodSet(100000);
	IntMasterEnable();
	SysTickIntEnable();
	SysTickEnable();
}



void status_change(enum status s)
{
	system_status = s;
}

void led_run(void)
{
	static enum status led_status = on_init;
	static unsigned int count = 0;
	switch(led_status){
		case on_init:{
			if(count == 0){
				led_status_set(true,false,false);
			}else if(count == 70){
				led_status_set(false,true,false);
			}else if(count == 140){
				led_status_set(false,false,true);
			}
			count++;
			
			if(count == 210){
				count = 0;
				led_status = system_status;
			}
			break;
		}
		case ready:{
			static int status = 0;
			if(status > 2){
				if(count == 0){
					led_status_set(false,false,true);
				}else if(count == 70){
					led_status_set(false,false,false);
				}
				count++;
				if(count == 1000){
					count = 0;
				}
			}else if(status == 0){
				led_status_set(false,false,false);
				count++;
				if(count == 500){
					count = 0;
					status++;
				}
			}else{
				if(count == 0){
					led_status_set(false,false,true);
				}else if(count == 70){
					led_status_set(false,false,false);
				}
				count++;
				if(count == 140){
					count=0;
					status++;
				}
			}
			break;
		}
		case unlocked:
			break;
		case failure:
			break;
	}
}

void SysTickIntHandler()
{
	led_run();
	
	
	static int count = 0;
	count++;
	if(count > 3000){
		system_status = ready;
	}
}
