#ifndef __POWER_CONTROL_H__
#define	__POWER_CONTROL_H__

#include "core.h"

int init_power(zlog_category_t* log_handler);

int start_heat_device(zlog_category_t* log_handler);
int stop_heat_device(zlog_category_t* log_handler);

int process_power_down_signal(zlog_category_t* log_handler);
int process_capgd_singal(zlog_category_t* log_handler);
int process_smb_alert_singal(zlog_category_t* log_handler);

#endif