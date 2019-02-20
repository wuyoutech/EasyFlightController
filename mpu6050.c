#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
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

#define Byte16(Type, ByteH, ByteL)  \
		((Type)((((uint16_t)(ByteH))<<8) | ((uint16_t)(ByteL))))
		
#define MPU6050_ADDR 0x68

void i2c_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);
	
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE,GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTB_BASE,GPIO_PIN_3);
	I2CMasterInitExpClk(I2C0_BASE,SysCtlClockGet(),true);
}

unsigned char mpu6050_read_register(unsigned char addr)
{
	I2CMasterSlaveAddrSet(I2C0_BASE,MPU6050_ADDR,false);
	
	I2CMasterDataPut(I2C0_BASE,addr);
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_SEND);
	while(I2CMasterBusy(I2C0_BASE));
	
	I2CMasterSlaveAddrSet(I2C0_BASE,MPU6050_ADDR,true);
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);
	while(I2CMasterBusy(I2C0_BASE));
	
	unsigned char i2c_read = I2CMasterDataGet(I2C0_BASE);
	return i2c_read;
}

void mpu6050_write_register(unsigned char addr,unsigned char data)
{
	I2CMasterSlaveAddrSet(I2C0_BASE,MPU6050_ADDR,false);
	
	I2CMasterDataPut(I2C0_BASE,addr);
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C0_BASE));
	
	I2CMasterDataPut(I2C0_BASE,data);
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_BURST_SEND_CONT);
	while(I2CMasterBusy(I2C0_BASE));
	
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(I2CMasterBusy(I2C0_BASE));
}
