#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

void led_init(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
	
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_2);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_3);
}

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
