#ifndef __TIMER_H_
#define __TIMER_H_

struct pwm_input
{
	unsigned int channel1;
	unsigned int channel2;
	unsigned int channel3;
	unsigned int channel4;
	unsigned int channel5;
	unsigned int channel6;
};

void pwm_input_init(void);
struct pwm_input * pwm_input_get(void);

#endif
