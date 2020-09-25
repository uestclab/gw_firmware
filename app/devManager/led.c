#include "common.h"
#include "led.h"

int init_led(zlog_category_t* log_handler){

    if(gpio_open(LED_RED, 1) < 0){
        return -1;
    }

    if(gpio_open(LED_BLUE, 1) < 0){
        return -1;
    }

    if(gpio_open(LED_GREEN, 1) < 0){
        return -1;
    }

    if(gpio_set_val(LED_BLUE, 1) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 1 \n", LED_BLUE);
        return -1;
    }

    if(gpio_set_val(LED_GREEN, 1) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 1 \n", LED_GREEN);
        return -1;
    }

    return 0;
}

// device init state, only high control
int led_red(){
    ;
}

// init start , but not complete and successful, low control
int led_blue(zlog_category_t* log_handler){

    if(gpio_set_val(LED_RED, 0) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 0 \n", LED_RED);
        return -1;
    }

    if(gpio_set_val(LED_BLUE, 0) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 0 \n", LED_BLUE);
        return -1;
    }

    if(gpio_set_val(LED_GREEN, 1) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 1 \n", LED_GREEN);
        return -1;
    }

    return 0;
}

// work correct and completed , low control
int led_green(zlog_category_t* log_handler){
    if(gpio_set_val(LED_RED, 0) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 0 \n", LED_RED);
        return -1;
    }

    if(gpio_set_val(LED_BLUE, 1) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 1 \n", LED_BLUE);
        return -1;
    }

    if(gpio_set_val(LED_GREEN, 0) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 0 \n", LED_GREEN);
        return -1;
    }
    
    return 0;
}