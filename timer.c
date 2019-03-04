#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"

#include <timer.h>
#include <config.h>

//------ array for saving raw data ------
unsigned int pwm_input_raw[6];
//--- array for saving calibrated data --
unsigned int pwm_input_calibrated[6];

//-----   interrupt handler -------
void timer0a_int_handler(void)
{
	TimerIntClear(WTIMER0_BASE,TimerIntStatus(WTIMER0_BASE,TIMER_CAPA_EVENT));
	
	static unsigned int lasttime = 0;
	unsigned int nowtime = TimerValueGet(WTIMER0_BASE,TIMER_A);
	
	unsigned int count = 0;
	if(nowtime > lasttime){
		count = nowtime - lasttime;
	}else{
		count = nowtime - lasttime + 0xffff;
	}
	lasttime = nowtime;
	//TODO:UPDATE TIME
	if(count < 120000){
		pwm_input_raw[0] = count/50;
	}
}

void timer0b_int_handler(void)
{
	TimerIntClear(WTIMER0_BASE,TimerIntStatus(WTIMER0_BASE,TIMER_CAPB_EVENT));
	
	static unsigned int lasttime = 0;
	unsigned int nowtime = TimerValueGet(WTIMER0_BASE,TIMER_B);
	
	unsigned int count = 0;
	if(nowtime > lasttime){
		count = nowtime - lasttime;
	}else{
		count = nowtime - lasttime + 0xffff;
	}
	lasttime = nowtime;
	if(count < 120000){
		pwm_input_raw[1] = count / 50;
	}
}
void timer1a_int_handler(void)
{
	TimerIntClear(WTIMER1_BASE,TimerIntStatus(WTIMER1_BASE,TIMER_CAPA_EVENT));
	
	static unsigned int lasttime = 0;
	unsigned int nowtime = TimerValueGet(WTIMER1_BASE,TIMER_A);
	
	unsigned int count = 0;
	if(nowtime > lasttime){
		count = nowtime - lasttime;
	}else{
		count = nowtime - lasttime + 0xFFFF;
	}
	lasttime = nowtime;
	if(count < 120000){
		pwm_input_raw[2] = count / 50;
	}
}
void timer1b_int_handler(void)
{
	TimerIntClear(WTIMER1_BASE,TimerIntStatus(WTIMER1_BASE,TIMER_CAPB_EVENT));
	
	static unsigned int lasttime = 0;
	unsigned int nowtime  = TimerValueGet(WTIMER1_BASE,TIMER_B);
	
	unsigned int count = 0;
	if(nowtime > lasttime){
		count = nowtime - lasttime;
	}else{
		count = nowtime - lasttime + 0xffff;
	}
	lasttime = nowtime;
	if(count < 12000){
		pwm_input_raw[3] = count / 50;
	}
}
void timer2a_int_handler(void)
{
	TimerIntClear(WTIMER2_BASE,TimerIntStatus(WTIMER2_BASE,TIMER_CAPA_EVENT));
	
	static unsigned int lasttime = 0;
	unsigned int nowtime = TimerValueGet(WTIMER2_BASE,TIMER_A);
	
	unsigned int count = 0;
	if(nowtime > lasttime){
		count = nowtime - lasttime;
	}else{
		count = nowtime - lasttime + 0xffff;
	}
	lasttime = nowtime;
	if(count < 12000){
		pwm_input_raw[4] = count / 50;
	}
}
void timer2b_int_handler(void)
{
	TimerIntClear(WTIMER2_BASE,TimerIntStatus(WTIMER2_BASE,TIMER_CAPB_EVENT));
	
	static unsigned int lasttime = 0;
	unsigned int nowtime = TimerValueGet(WTIMER2_BASE,TIMER_B);
	
	unsigned int count = 0;
	if(nowtime > lasttime){
		count = nowtime - lasttime;
	}else{
		count = nowtime - lasttime + 0xffff;
	}
	lasttime = nowtime;
	if(count <12000){
		pwm_input_raw[5] = count / 50;
	}
}
// ------ initialize timers ----------
void pwm_input_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER2);
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
	
	GPIOPinTypeTimer(GPIO_PORTC_BASE,GPIO_PIN_4);	//set pin type to timer
	GPIOPinTypeTimer(GPIO_PORTC_BASE,GPIO_PIN_5);
	GPIOPinTypeTimer(GPIO_PORTC_BASE,GPIO_PIN_6);
	GPIOPinTypeTimer(GPIO_PORTC_BASE,GPIO_PIN_7);
	GPIOPinTypeTimer(GPIO_PORTD_BASE,GPIO_PIN_0);
	GPIOPinTypeTimer(GPIO_PORTD_BASE,GPIO_PIN_1);
	
	GPIOPinConfigure(GPIO_PC4_WT0CCP0);
	GPIOPinConfigure(GPIO_PC5_WT0CCP1);
	GPIOPinConfigure(GPIO_PC6_WT1CCP0);
	GPIOPinConfigure(GPIO_PC7_WT1CCP1);
	GPIOPinConfigure(GPIO_PD0_WT2CCP0);
	GPIOPinConfigure(GPIO_PD1_WT2CCP1);
	
	GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_4,
					GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);	//wake up pull
	GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_5,
					GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_6,
					GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_7,
					GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTD_BASE,GPIO_PIN_0,
					GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTD_BASE,GPIO_PIN_1,
					GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
					
	TimerConfigure(WTIMER0_BASE,TIMER_CFG_SPLIT_PAIR			//set timer work mode
				   |TIMER_CFG_A_CAP_TIME_UP
				   |TIMER_CFG_B_CAP_TIME_UP);
	TimerConfigure(WTIMER1_BASE,TIMER_CFG_SPLIT_PAIR
					|TIMER_CFG_A_CAP_TIME_UP
					|TIMER_CFG_B_CAP_TIME_UP);
	TimerConfigure(WTIMER2_BASE,TIMER_CFG_SPLIT_PAIR
					|TIMER_CFG_A_CAP_TIME_UP
					|TIMER_CFG_B_CAP_TIME_UP);
		
	TimerControlEvent(WTIMER0_BASE,TIMER_A,TIMER_EVENT_BOTH_EDGES);	//set to both edges interrupt
	TimerControlEvent(WTIMER0_BASE,TIMER_B,TIMER_EVENT_BOTH_EDGES);
	TimerControlEvent(WTIMER1_BASE,TIMER_A,TIMER_EVENT_BOTH_EDGES);
	TimerControlEvent(WTIMER1_BASE,TIMER_B,TIMER_EVENT_BOTH_EDGES);
	TimerControlEvent(WTIMER2_BASE,TIMER_A,TIMER_EVENT_BOTH_EDGES);
	TimerControlEvent(WTIMER2_BASE,TIMER_B,TIMER_EVENT_BOTH_EDGES);
					
	TimerLoadSet(WTIMER0_BASE,TIMER_A,0xFFFFFFFF);					//set loader to max value
	TimerLoadSet(WTIMER0_BASE,TIMER_B,0xFFFFFFFF);
	TimerLoadSet(WTIMER1_BASE,TIMER_A,0xFFFFFFFF);
	TimerLoadSet(WTIMER1_BASE,TIMER_B,0xFFFFFFFF);
	TimerLoadSet(WTIMER2_BASE,TIMER_A,0xFFFFFFFF);
	TimerLoadSet(WTIMER2_BASE,TIMER_B,0xFFFFFFFF);
	
	TimerIntRegister(WTIMER0_BASE,TIMER_A,timer0a_int_handler);		//regist interrupt handler
	TimerIntRegister(WTIMER0_BASE,TIMER_B,timer0b_int_handler);
	TimerIntRegister(WTIMER1_BASE,TIMER_A,timer1a_int_handler);
	TimerIntRegister(WTIMER1_BASE,TIMER_B,timer1b_int_handler);
	TimerIntRegister(WTIMER2_BASE,TIMER_A,timer2a_int_handler);
	TimerIntRegister(WTIMER2_BASE,TIMER_B,timer2b_int_handler);
	
	IntMasterEnable();												//enable master interrupt
	TimerIntEnable(WTIMER0_BASE,TIMER_CAPA_EVENT);
	TimerIntEnable(WTIMER0_BASE,TIMER_CAPB_EVENT);
	TimerIntEnable(WTIMER1_BASE,TIMER_CAPA_EVENT);
	TimerIntEnable(WTIMER1_BASE,TIMER_CAPB_EVENT);
	TimerIntEnable(WTIMER2_BASE,TIMER_CAPA_EVENT);
	TimerIntEnable(WTIMER2_BASE,TIMER_CAPB_EVENT);
	
	IntEnable(INT_WTIMER0A);
	IntEnable(INT_WTIMER0B);
	IntEnable(INT_WTIMER1A);
	IntEnable(INT_WTIMER1B);
	IntEnable(INT_WTIMER2A);
	IntEnable(INT_WTIMER2B);
	
	TimerEnable(WTIMER0_BASE,TIMER_A);								//enable timer
	TimerEnable(WTIMER0_BASE,TIMER_B);
	TimerEnable(WTIMER1_BASE,TIMER_A);
	TimerEnable(WTIMER1_BASE,TIMER_B);
	TimerEnable(WTIMER2_BASE,TIMER_A);
	TimerEnable(WTIMER2_BASE,TIMER_B);
}

//------- get pwm input value --------
unsigned int * pwm_input_get(void)
{
	struct config * config = config_get();
	pwm_input_calibrated[config -> pitch_channel_number] = 
		pwm_input_raw[config -> pitch_channel_number] - config -> pitch_center_shift;
	
	pwm_input_calibrated[config -> roll_channel_number] = 
		pwm_input_raw[config -> roll_channel_number] - config -> roll_center_shift;
	
	pwm_input_calibrated[config -> yaw_channel_number] = 
		pwm_input_raw[config -> yaw_channel_number] - config -> yaw_center_shift;
	
	pwm_input_calibrated[config -> throttle_channel_number] = 
		pwm_input_raw[config -> throttle_channel_number] - config -> throttle_minimum_shift;

	pwm_input_calibrated[4] = pwm_input_raw[4];
	pwm_input_calibrated[5] = pwm_input_raw[5];
	
	return pwm_input_calibrated;
}
void pwm_calibration(void)
{
	struct config * config = config_get();
	config -> pitch_center_shift = pwm_input_raw[config -> pitch_channel_number];
	config -> roll_center_shift = pwm_input_raw[config -> roll_channel_number];
	config -> yaw_center_shift = pwm_input_raw[config -> yaw_center_shift];
	config -> throttle_minimum_shift = pwm_input_raw[config -> throttle_channel_number];
	config_write();
}
/********************************************************************/
//pwm output
void pwm_output_init()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER5);
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
	// ----- unlock Pin -----
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR)  |= GPIO_PIN_7;
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0x00;
	
	GPIOPinConfigure(GPIO_PD2_WT3CCP0);
	GPIOPinConfigure(GPIO_PD3_WT3CCP1);
	GPIOPinConfigure(GPIO_PD6_WT5CCP0);
	GPIOPinConfigure(GPIO_PD7_WT5CCP1);
	
	GPIOPinTypeTimer(GPIO_PORTD_BASE,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7);
	
	TimerConfigure(WTIMER3_BASE,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
	TimerConfigure(WTIMER5_BASE,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
	
	#define max_count 200000
	
	TimerLoadSet(WTIMER3_BASE,TIMER_A,max_count);
	TimerLoadSet(WTIMER3_BASE,TIMER_B,max_count);
	TimerLoadSet(WTIMER5_BASE,TIMER_A,max_count);
	TimerLoadSet(WTIMER5_BASE,TIMER_B,max_count);
	
	TimerMatchSet(WTIMER3_BASE,TIMER_A,max_count - 800*50);
	TimerMatchSet(WTIMER3_BASE,TIMER_B,max_count - 800*50);
	TimerMatchSet(WTIMER5_BASE,TIMER_A,max_count - 800*50);
	TimerMatchSet(WTIMER5_BASE,TIMER_B,max_count - 800*50);
	
	TimerEnable(WTIMER3_BASE,TIMER_A);
	TimerEnable(WTIMER3_BASE,TIMER_B);
	TimerEnable(WTIMER5_BASE,TIMER_A);
	TimerEnable(WTIMER5_BASE,TIMER_B);
}
//-------- pwm output setter -------
void pwm_output_set(unsigned int a,unsigned int b,unsigned int c,unsigned int d)	
{
	TimerMatchSet(WTIMER3_BASE,TIMER_A,max_count - a*50);
	TimerMatchSet(WTIMER3_BASE,TIMER_B,max_count - b*50);
	TimerMatchSet(WTIMER5_BASE,TIMER_A,max_count - c*50);
	TimerMatchSet(WTIMER5_BASE,TIMER_B,max_count - d*50);
}
