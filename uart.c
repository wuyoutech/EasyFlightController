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
#include <uartstdio.h>
#include <stdio.h>

void uart_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));	//waiting for GPIOA enabled

	GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);
	UARTClockSourceSet(UART0_BASE,UART_CLOCK_PIOSC);
	UARTStdioConfig(0,115200,16*1000*1000);
}
//------ rewrite fputc for printf funciton -----
int fputc(int ch, FILE *f)
{
	UARTCharPut(UART0_BASE,(unsigned char)ch);
	return ch;
}
//------ rewrite fgetc for scnaf function ------
int fgetc(FILE *f)
{
	unsigned int recv = UARTCharGet(UART0_BASE);
	return recv;
}
//-------- receive data from uart saving here -------
char recv_buffer[255];
//---------commnad input and execute in here --------
void uart_run(void)
{
	// TODO: get message from uart
	// and execute command
	scanf("%s",recv_buffer);
	printf("%s",recv_buffer);
}