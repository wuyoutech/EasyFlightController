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

#include <config.h>

// ------ function for convert string to int -----
int atoi(const char * str)
{
	int value = 0;
	bool b_plus = true;
	switch(*str){
		case '+':
			str++;
			break;
		case '-':
			b_plus = false;
			str++;
			break;
		default:
			break;
	}
	while('\0' != *str){
		if(*str >= '0' && *str <= '9'){
			value = (value * 10)+(*str - '0');
			str++;
		}else{
			return 0;
		}
	}
	if(!b_plus)
		value = -value;
	return value;
}

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
	//waiting for GPIOA enabled
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));	
	

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
	gets(recv_buffer);
	// IMPORTANT:REMOVE CR '\r' FIRST
	for(int i = 0;i<strlen(recv_buffer);i++){
		if(recv_buffer[i] == '\r')
			recv_buffer[i] = '\0';
	}
	// split to "command" and "paramenter"
	char command[256] = {0};
	char paramenter[256] = {0};
	int paramenter_number;
	char * paramenters[10];	//support 10 paramenter max
	// find '(' in buffer
	char * paramenter_start = (char *)memchr(recv_buffer,'(',strlen(recv_buffer));
	char * paramenter_end = (char *)memchr(recv_buffer,')',strlen(recv_buffer));
	if(paramenter_start != NULL && paramenter_end != NULL){
		// if find '(' successful
		memcpy(command,recv_buffer,paramenter_start-recv_buffer);
		// copy paramenter
		memcpy(paramenter,paramenter_start + 1,paramenter_end - paramenter_start -1);
		//split paramenter
		paramenter_number= str_split(paramenter,paramenters,',');
	}else{
		// entire message is command
		memcpy(command,recv_buffer,strlen(recv_buffer));
		paramenter_number = 0;
	}
	
	// match command
	if(strcmp(command,"help") == 0){
		printf("support command list:\n");
		printf("set input channel(pitch,roll,yaw,throttle)\n");
	}else if(strcmp(command,"set input channel") == 0){
		// check paramenter number
		if(paramenter_number != 4){
			printf("synatex error: %d paramenter found ,require 4\n",paramenter_number);
			return;
		}
		int paramenter_number[4];
		// convert input
		for(int i=0;i<4;i++){
			paramenter_number[i] = atoi(paramenters[i]);
		}
		struct config * config = config_get();
		config -> pitch_channel_number = paramenter_number[0];
		config -> roll_channel_number  = paramenter_number[1];
		config -> yaw_channel_number = paramenter_number[2];
		config -> throttle_channel_number = paramenter_number [3];
		// save config
		config_write();
		//report result
		printf("pitch:%d,roll:%d,yaw:%d,throttle:%d\n",
			config -> pitch_channel_number,
			config -> roll_channel_number,
			config -> yaw_channel_number,
			config -> throttle_channel_number);
	}else if(strcmp(command,"calibration imu")){
		printf("command not support now\n");
	}else{
		printf("command not support, send 'help' for more infomation\n");
	}
}
