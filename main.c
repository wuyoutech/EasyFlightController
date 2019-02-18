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

void UART_Init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);	//enable uart0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);	//enable gpioA using for uart
	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));	//waiting for gpioA ready
	
	GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);
	
	UARTClockSourceSet(UART0_BASE,UART_CLOCK_PIOSC);
	UARTStdioConfig(0,115200,16*1000*1000);
}

int fputc(int ch, FILE *f){
	UARTCharPut(UART0_BASE,(unsigned char)ch);
	return ch;
}

int main(){
	ROM_FPULazyStackingEnable();
	ROM_FPUEnable();
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|
		SYSCTL_OSC_MAIN);
	UART_Init();
	printf("message\n");
	while(true);
}
