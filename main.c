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

#include <stdio.h>
#include <uartstdio.h>


#include <uart.h>
#include <config.h>
#include <mpu6050.h>
#include <timer.h>
#include <systick.h>
#include <led.h>


int main(){
	ROM_FPULazyStackingEnable();
	ROM_FPUEnable();
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|
		SYSCTL_OSC_MAIN);
	
	led_init();
	systick_init();
	uart_init();
	printf("message\n");
	
	config_init();
	if(!mpu6050_init()){
		printf("mpu6050 not found\n");
		while(true);
	}
	pwm_input_init();
	pwm_output_init();
	
	
	while(true)
	{
		unsigned int * input;
		input = pwm_input_get();
		pwm_output_set(input[2],input[2],input[2],input[2]);
	}
}
