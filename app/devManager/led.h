#ifndef __LED_CONTROL_H__
#define	__LED_CONTROL_H__

#include "core.h"

int init_led(zlog_category_t* log_handler);
int led_red();  // device init state, only high control
int led_blue(zlog_category_t* log_handler); // init start , but not complete and successful, low control
int led_green(zlog_category_t* log_handler);// work correct and completed , low control

#endif