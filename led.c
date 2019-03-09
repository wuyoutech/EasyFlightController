#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include <systick.h>

void led_init(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
	
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_2);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_3);
}
//------- set led status function  -----
void led_status_set(bool red,bool green,bool blue){
	if(red)
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0xff);
	else
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0x00);
	if(blue)
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0xff);
	else
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0x00);
	if(green)
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0xff);
	else
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0x00);
}

//function running in SysTick Timer
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
				led_status = status_get();
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
				led_status = status_get();
			}
			break;
		}
		case unlocked:{
			if(count == 0){
				led_status_set(false,true,false);
			}else if(count == 70){
				led_status_set(false,false,false);
			}else if(count == 140){
				led_status_set(false,true,false);
			}else if(count == 210){
				led_status_set(false,false,false);
			}
			count++;
			if(led_status != status_get()){
				count = 0;
				led_status = status_get();
			}
			if(count == 700){
				count = 0;
			}
			break;
		}
		case failure:{
			if(count == 0){
				led_status_set(true,false,false);
			}else if(count == 30){
				led_status_set(false,false,false);
			}else if(count == 100){
				led_status_set(false,true,false);
			}else if(count == 130){
				led_status_set(false,false,false);
			}
			count++;
			if(count == 200){
				led_status_set(false,false,false);
				count = 0;
				led_status = status_get();
			}
			break;
		}
	}
}
//ass