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
#include <string.h>
//----- string split function ----------
int str_split(char * str,char ** split_result,char spliter)
{
	split_result[0] = str;
	int a = 1;
	for(int i = 0;;i++){
		// if end to NULL
		// jump out loop
		if(*(str+i) == 0)
			break;
		// if character equal to spliter
		// set result pointer
		// and end symbol
		if(*(str+i) == spliter){
			split_result[a] = str+i+1;
			a++;
			*(str+i) = 0;
		}
	}
	// return numbers of fragment
	return a;
}
//--------- init uart module -----------------
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
char recv_buffer[256];
//---------commnad input and execute in here --------
void uart_run(void)
{
	// get message from uart
	// and execute command
	scanf("%s",recv_buffer);
	// split to "command" and "paramenter"
	char command[256] = {0};
	char paramenter[256] = {0};
	// find '(' in buffer
	char * paramenter_start = (char *)memchr(recv_buffer,'(',strlen(recv_buffer));
	if(paramenter_start != NULL){
		// if find '(' successful
		memcpy(command,recv_buffer,paramenter_start-recv_buffer);
	}else{
		// entire message is command
		memcpy(command,recv_buffer,strlen(recv_buffer));
	}
	// check synatex
	char * paramenter_end = (char *)memchr(recv_buffer,')',strlen(recv_buffer));
	if(paramenter_end == NULL){
		printf("synatex error:%s\n",recv_buffer);
		return;
	}
	// copy paramenter
	memcpy(paramenter,paramenter_start + 1,paramenter_end - paramenter_start -1);
	//split paramenter
	char * paramenters[10];	//support 10 paramenter max
	int paramenter_number = str_split(paramenter,result,',');
}
