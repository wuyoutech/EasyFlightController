#ifndef __SYSTICK_H_
#define __SYSTICK_H_
// -------- struct declartion -------
enum status
{
	on_init,
	ready,
	unlocked,
	failure
};

// -------- function definition -----
void systick_init(void);
void status_set(enum status s);
enum status status_get(void);
#endif
