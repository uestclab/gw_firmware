#include "common.h"
#include "monitor.h"


/* ------ rf gpio --------------- */
int init_rf_monitor(){
    if(gpio_open(RF_IN_PLACE_0, 0) < 0){
        return -1;
    }

    if(gpio_open(RF_IN_PLACE_1, 0) < 0){
        return -1;
    }

    if(gpio_open(RF_IN_PLACE_2, 0) < 0){
        return -1;
    }
    return 0;
}

int check_rf_exist(){
    int8_t read_val = -1;
    if(gpio_get_val(RF_IN_PLACE_0, &read_val) < 0 ){
        printf("read RF_IN_PLACE_0 failed !\n");
        return -1;
    }
    return read_val;
}

/* ------ switch gpio -------------- */
int init_switch_monitor(){
    if(gpio_open(SWITCH_DET_N, 0) < 0){
        return -1;
    }

    return 0;
}

int check_switch_exist(){
    int8_t read_val = -1;
    if(gpio_get_val(SWITCH_DET_N, &read_val) < 0 ){
        printf("read SWITCH_DET_N failed !\n");
        return -1;
    }
    return read_val;
}

int peripheral_check(g_handler_para* g_handler){
    g_handler->g_peripheral = (g_peripheral_para*)malloc(sizeof(g_peripheral_para));
    if(g_handler->g_peripheral == NULL){
		return -1;
    }

    int rf_on = check_rf_exist();
    if(rf_on == 1 ){
        zlog_info(g_handler->log_handler, " No rf device !");
    }
    g_handler->g_peripheral->rf_on = rf_on;
    int switch_on = check_switch_exist();
    if(switch_on ==1 ){
        zlog_info(g_handler->log_handler, " No switch device !");
    }
    g_handler->g_peripheral->switch_on = switch_on;

    return 0;
}


struct gpio_poll_s g_poll_list = {
	.cnt = 0,
};

void gpio_layer_init(void)
{
	int ret = 0;
	int i = 0;
	
	g_poll_list.cnt = 0;

	for(i=0; i< MAX_POLL; i++){
		g_poll_list.fds[i].fd = -1;
	}
}

// int init_gpio(int num, int edge){
//     if(gpio_open(num, 0) < 0){
//         return -1;
//     }

//     if(gpio_set_intr(num, edge, &(pdev->fd));
// 	if(ret < 0){
// 		goto exit;
// 	}

// }

int init_monitor(zlog_category_t* log_handler){

    gpio_layer_init();

    if(gpio_open(MON_PG_ZYNQ, 0) < 0){
        return -1;
    }

    if(gpio_open(MON_PG_CLK, 0) < 0){
        return -1;
    }

    if(gpio_open(MON_PG_ADC, 0) < 0){
        return -1;
    }

    if(gpio_open(MON_PG_DAC, 0) < 0){
        return -1;
    }

    if(gpio_open(MON_PG_5V, 0) < 0){
        return -1;
    }

    if(gpio_open(MON_HMC_GPIO1, 0) < 0){
        return -1;
    }

    if(gpio_open(MON_HMC_GPIO2, 0) < 0){
        return -1;
    }

    if(init_rf_monitor() < 0){
        return -1;
    }

    if(init_switch_monitor() <0){
        return -1;
    }

    return 0;
}

void* monitor_work_thread(void* args){
	g_handler_para* g_handler = (g_handler_para*)args;
    g_args_para* g_args = g_handler->g_args;

	// postMsg(MSG_NO_MONTAB_WORK_LEFT,NULL,0,NULL,0,g_handler->g_msg_queue);
	return NULL;
    
}

// add to monitor
void start_monitor(g_handler_para* g_handler, ThreadPool* g_threadpool){
    AddWorker(monitor_work_thread,(void*)g_handler,g_threadpool);
}