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

#include <mpu6050.h>

#define Byte16(ByteH, ByteL)  \
		((unsigned short int)((((uint16_t)(ByteH))<<8) | ((uint16_t)(ByteL))))
		
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

bool mpu6050_init(void)
{
	i2c_init();
	unsigned char result = mpu6050_read_register(0x75);
	if(result!=0x68){
		return false;
	}
	mpu6050_write_register(0x6B,0x80);	//reset mpu6050
	SysCtlDelay(10 * 1000 * 1000);		//wait some seconds 
	mpu6050_write_register(0x6B,0x00);
	//check write result
	if(mpu6050_read_register(0x6B)!=0x00)
		return false;
	mpu6050_write_register(0x68,0x03);	//reboot sensors
	SysCtlDelay(10 * 1000 * 1000);		//wait some seconds 
	mpu6050_write_register(0x6B,0x01);	//set clock source
	mpu6050_write_register(0x6c,0x00);	//enable gyro and accel	
	mpu6050_write_register(0x1a,0x03);	//enable low pass filter
	mpu6050_write_register(0x19,0x00);	//sampling 1khz
	mpu6050_write_register(0x1b,0x18);	//set gyro range
	mpu6050_write_register(0x1c,0x10);	//set accel range
}

void mpu6050_get_value(unsigned char * buff)
{
	unsigned int addr = 0x3B;
	for(int i = 0;i<14;i++){
		buff[i] = mpu6050_read_register(addr);
		addr++;
	}
}

struct RawValue acc_filter(struct RawValue input)
{
	#define FILTER_LENGTH 10
	static unsigned char filter_cnt = 0;
	static struct RawValue acc_buf[FILTER_LENGTH];
	
	acc_buf[filter_cnt].x = input.x;
	acc_buf[filter_cnt].y = input.y;
	acc_buf[filter_cnt].z = input.z;
	
	//sum and average
	unsigned int x=0,y=0,z=0;
	for(int i = 0;i<FILTER_LENGTH;i++){
		x += acc_buf[i].x;
		y += acc_buf[i].y;
		z += acc_buf[i].z;
	}
	struct RawValue avg;
	avg.x = x / FILTER_LENGTH;
	avg.y = y / FILTER_LENGTH;
	avg.z = z / FILTER_LENGTH;
	return avg;
}

float invSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}


#define GYRO_GR	0.0010653f

#define sampleFreq	1000.0f			// sample frequency in Hz
#define twoKpDef	(2.0f * 0.5f)	// 2 * proportional gain
#define twoKiDef	(2.0f * 0.0f)	// 2 * integral gain


//---------------------------------------------------------------------------------------------------
// Variable definitions

volatile float twoKp = twoKpDef;											// 2 * proportional gain (Kp)
volatile float twoKi = twoKiDef;											// 2 * integral gain (Ki)
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;					// quaternion of sensor frame relative to auxiliary frame
volatile float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;	// integral error terms scaled by Ki

struct Quaternion MahonyAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az) {
	float recipNorm;
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;        

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		halfvx = q1 * q3 - q0 * q2;
		halfvy = q0 * q1 + q2 * q3;
		halfvz = q0 * q0 - 0.5f + q3 * q3;
	
		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		// Compute and apply integral feedback if enabled
		if(twoKi > 0.0f) {
			integralFBx += twoKi * halfex * (1.0f / sampleFreq);	// integral error scaled by Ki
			integralFBy += twoKi * halfey * (1.0f / sampleFreq);
			integralFBz += twoKi * halfez * (1.0f / sampleFreq);
			gx += integralFBx;	// apply integral feedback
			gy += integralFBy;
			gz += integralFBz;
		}
		else {
			integralFBx = 0.0f;	// prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gx += twoKp * halfex;
		gy += twoKp * halfey;
		gz += twoKp * halfez;
	}
	
	// Integrate rate of change of quaternion
	gx *= (0.5f * (1.0f / sampleFreq));		// pre-multiply common factors
	gy *= (0.5f * (1.0f / sampleFreq));
	gz *= (0.5f * (1.0f / sampleFreq));
	qa = q0;
	qb = q1;
	qc = q2;
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx); 
	
	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
	
	//return result
	struct Quaternion result;
	result.q0 = q0;
	result.q1 = q1;
	result.q2 = q2;
	result.q3 = q3;
	return result;
}

struct Euler quaternion_to_euler(struct Quaternion q)
{
	const float PI = 3.1415926;
	const double Epsilon = 0.0009765625f;
    const double Threshold = 0.5f - Epsilon;
	
	double TEST = q.q0 * q.q2 - q.q1 * q.q3;
	
	struct Euler result;
	
	if(TEST < -Threshold || TEST > Threshold){
		int sign = TEST>0?1:-1;
		result.z = -1 * sign * (double)atan2(q.q1,q.q0);
		result.y = sign * (PI / 2.0);
		result.x = 0;
	}else{
		result.x = atan2(2 * (q.q2 * q.q3 + q.q0 * q.q1),q.q0*q.q0 - q.q1 *q.q1 -q.q2*q.q2 + q.q3*q.q3);
		result.y = asin(-2 * (q.q1 * q.q3 - q.q0* q.q2));
		result.z = atan2(2 * (q.q1 * q.q2 + q.q0*q.q3),q.q0*q.q0 + q.q1*q.q1 - q.q2*q.q2 - q.q3*q.q3);
	}
	return result;
}

struct Euler attitude_get(void){
	unsigned char buff[14];
	mpu6050_get_value(buff);
	unsigned short int acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z;
	struct RawValue accel,gyro;
	accel.x	= Byte16(buff[0],buff[1]);
	accel.y = Byte16(buff[2],buff[3]);
	accel.z = Byte16(buff[4],buff[5]);
	
	gyro.x = Byte16(buff[8],buff[9]);
	gyro.y = Byte16(buff[10],buff[11]);
	gyro.z = Byte16(buff[12],buff[13]);
	
	accel = acc_filter(accel);
	struct Quaternion q =  MahonyAHRSupdateIMU(accel.x,accel.y,accel.z,gyro.x,gyro.y,gyro.z);
	struct Euler attitude = quaternion_to_euler(q);
	return attitude;
}