#ifndef __TIMER_H_
#define __TIMER_H_

void pwm_input_init(void);
unsigned int * pwm_input_get(void);
unsigned int * pwm_input_get_raw(void);
void pwm_calibration(void);

void pwm_output_init(void);
void pwm_output_set(unsigned int a,unsigned int b,unsigned int c,unsigned int d);


#endif
