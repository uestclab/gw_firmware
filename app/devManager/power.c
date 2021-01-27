#include "common.h"
#include "power.h"

int init_power(zlog_category_t* log_handler){

    if(gpio_open(POWER_EN_PTC, 1) < 0){
        return -1;
    }

    if(gpio_set_val(POWER_EN_PTC, 0) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 1 \n", POWER_EN_PTC);
        return -1;
    }

    return 0;
}

int start_heat_device(zlog_category_t* log_handler){
    if(gpio_set_val(POWER_EN_PTC, 1) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 1 \n", POWER_EN_PTC);
        return -1;
    }
    return 0;
}

int stop_heat_device(zlog_category_t* log_handler){
    if(gpio_set_val(POWER_EN_PTC, 0) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 1 \n", POWER_EN_PTC);
        return -1;
    }
    return 0;
}

int process_power_down_signal(zlog_category_t* log_handler){

    int8_t read_val = -1;
    if(gpio_get_val(POWER_PFO, &read_val) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 0 \n", POWER_PFO);
        return -1;
    }

    if(read_val == 1){
        zlog_info(log_handler, "power is not down \n");
        return 0;
    }

    // do something when power down
    // ...

    return 0;
}

int process_capgd_singal(zlog_category_t* log_handler){

    int8_t read_val = -1;
    if(gpio_get_val(POWER_CAPGD, &read_val) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 0 \n", POWER_CAPGD);
        return -1;
    }

    if(read_val == 1){
        zlog_info(log_handler, "capacitance is not wrong. \n");
        return 0;
    }

    // do something when capacitance
    // ...

    return 0;
}

int process_smb_alert_singal(zlog_category_t* log_handler){

    int8_t read_val = -1;
    if(gpio_get_val(POWER_SMB_ALERT, &read_val) < 0){
        zlog_info(log_handler, "write failed : gpio_no = %d , val = 0 \n", POWER_SMB_ALERT);
        return -1;
    }

    if(read_val == 1){
        zlog_info(log_handler, "smb alert is not. \n");
        return 0;
    }

    // do something when smb alert
    // read i2c ltc3350

    return 0;
}
