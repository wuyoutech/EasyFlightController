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

struct TargetVal
{
	unsigned int pitchVal;
	unsigned int rollVal;
	unsigned int yawVal;
}

struct Euler attitude_get(void);
struct TargetVal attitude_to_target_value(struct Euler euler);

bool mpu6050_init(void);

#endif
