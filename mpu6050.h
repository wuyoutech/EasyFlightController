#ifndef __MPU6050_H_
#define __MPU6050_H_

struct Euler
{
	float x;
	float y;
	float z;
};

struct RawValue
{
	unsigned short int x;
	unsigned short int y;
	unsigned short int z;
};

struct Quaternion
{
	float q0;
	float q1;
	float q2;
	float q3;
};

struct Euler attitude_get(void);
struct Euler target_euler_get(void);

bool mpu6050_init(void);

#endif
