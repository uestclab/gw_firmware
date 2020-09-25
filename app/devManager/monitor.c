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

/* -------- monitor gpio ----------- */

struct gpio_poll_s g_poll_list = {
	.cnt = 0,
};

static inline int fds_add(struct gpio_poll_s *pfds)
{

	int i = 0;
	int found = -EMFILE;

	for(i=0; i< MAX_POLL; i++){
		if(-1 == pfds->fds[i].fd){
			found = i;
			break;
		}
	}

	return found;
}

void gpio_layer_init(zlog_category_t* log_handler)
{
	int ret = 0;
	int i = 0;
	
	g_poll_list.cnt = 0;
    g_poll_list.log_handler = log_handler;

	for(i=0; i< MAX_POLL; i++){
		g_poll_list.fds[i].fd = -1;
        g_poll_list.gpio_no_idx[i] = -1;
	}
}

int init_mon_gpio(int num, int edge){
    int ret = 0;
    int fd = -1;
    
    if(gpio_open(num, 0) < 0){
        ret = -1;
        goto exit;
    }

    if(gpio_set_intr(num, edge, &fd)<0){
        ret = -1;
        goto exit;      
    }

    ret = fds_add(&g_poll_list);
	if(ret < 0){
		goto exit;
	}else{
        struct gpio_poll_s *pfds = &g_poll_list;
       	pfds->fds[ret].fd  = fd;
        pfds->fds[ret].events  = POLLPRI;
        pfds->fds[ret].revents = 0;
        pfds->gpio_no_idx[ret] = num;
        
        pfds->cnt++;
        zlog_info(pfds->log_handler, "gpio_no = %d , fd = %d , idx = %d \n", num, fd, ret); 
    }


    exit:
        return (ret>0)?0:ret;

}

int init_monitor(zlog_category_t* log_handler){

    gpio_layer_init(log_handler);

    if(init_mon_gpio(MON_PG_ZYNQ, BOTH) < 0){
        return -1;
    }

    if(init_mon_gpio(MON_PG_CLK, BOTH) < 0){
        return -1;
    }

    if(init_mon_gpio(MON_PG_ADC, BOTH) < 0){
        return -1;
    }

    if(init_mon_gpio(MON_PG_DAC, BOTH) < 0){
        return -1;
    }

    if(init_mon_gpio(MON_PG_5V, BOTH) < 0){
        return -1;
    }

    if(init_mon_gpio(MON_HMC_GPIO1, BOTH) < 0){
        return -1;
    }

    if(init_mon_gpio(MON_HMC_GPIO2, BOTH) < 0){
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

int gpio_poll_waite(int to_ms, g_handler_para* g_handler)
{
	int ret = 0, rc; 
	int i, found = 0;
	int8_t val = -1;
    zlog_info(g_handler->log_handler, "new gpio_poll_waite.... \n");
	rc = poll(g_poll_list.fds,MAX_POLL,to_ms);
	if(rc > 0 ){
		for(i=0; i<MAX_POLL; i++ ){
			if(g_poll_list.fds[i].revents & POLLPRI){
                zlog_info(g_handler->log_handler,"revents = 0x%x \n", g_poll_list.fds[i].revents);
				g_poll_list.fds[i].revents = 0;
				if(lseek(g_poll_list.fds[i].fd,0,SEEK_SET) == -1){
                    ret = -1;
                    goto exit;
                }

				if(read(g_poll_list.fds[i].fd, &val,sizeof(val)) == -1){
                    ret = -1;
                    goto exit;
                }
				found++;
                int gpio_no = g_poll_list.gpio_no_idx[i];
                val = val - 48; // "1" ascii
                zlog_info(g_handler->log_handler, "gpio_no = %d , g_poll_list.fds[%d].fd = %d, val = %d \n", 
                         gpio_no, i, g_poll_list.fds[i].fd, val);
                if(val == 0){
                    postMsg(MSG_MON_GPIO_EXCEPTION,NULL,gpio_no,NULL,val,g_handler->g_msg_queue);
                }
				if(found == rc){
					break;
				}
			}
		}
	}

	exit:
	return (ret == 0)?0:ret;
}

void* monitor_work_thread(void* args){
	g_handler_para* g_handler = (g_handler_para*)args;
    g_args_para* g_args = g_handler->g_args;
    zlog_info(g_handler->log_handler, "start monitor work \n");
    int ret = 0;
    do{
        ret = gpio_poll_waite(10000, g_handler); //wakeup after 10s
        if(ret){
            break;
        }
    }while(1);

	postMsg(MSG_EXIT_MON_GPIO,NULL,0,NULL,0,g_handler->g_msg_queue);
	return NULL;
    
}

// add to monitor
void start_monitor(g_handler_para* g_handler, ThreadPool* g_threadpool){
    AddWorker(monitor_work_thread,(void*)g_handler,g_threadpool);
}
