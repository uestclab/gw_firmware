#ifndef __MON_CONTROL_H__
#define	__MON_CONTROL_H__

#include "core.h"

// typedef enum peripheral_gpio{
//     RF_IN_PLACE_0 = 969,
//     RF_IN_PLACE_1 = 970,
//     RF_IN_PLACE_2 = 971,
//     SWITCH_DET_N  = 992,
// }peripheral_gpio;

int init_rf_monitor();
int peripheral_check(g_handler_para* g_handler);

// typedef enum mon_gpio{
//     MON_PG_ZYNQ = 974,
// 	MON_PG_CLK  = 975,
//     MON_PG_ADC  = 976,
//     MON_PG_DAC  = 977,
//     MON_PG_5V   = 978,

//     MON_HMC_GPIO1 = 996,
//     MON_HMC_GPIO2 = 997,
// }mon_gpio;

#define	MAX_POLL	32

struct gpio_poll_s{
    zlog_category_t* log_handler;
	int cnt;
	struct pollfd fds[MAX_POLL];
    int gpio_no_idx[MAX_POLL];
};

int init_monitor(zlog_category_t* log_handler);
void start_monitor(g_handler_para* g_handler, ThreadPool* g_threadpool);

#endif