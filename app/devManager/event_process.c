#include "common.h"
#include "led.h"
#include "monitor.h"

void show_msg_info_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    ////zlog_info(g_handler->log_handler, "show_msg_info_fun fun \n");
}

/* typedef void (*event_handler_pt)(ngx_event_t *ev); */
void checkTaskToTimer(ngx_event_t *ev){
    g_msg_queue_para* g_msg_queue = (g_msg_queue_para*)(ev->data);
    postMsg(MSG_TIMEOUT, NULL, 0, NULL, 0, g_msg_queue);
    free(ev);
}

void timeout_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    //zlog_info(g_handler->log_handler, "timeout fun \n");
    addTimeOutWorkToTimer(g_handler->g_msg_queue, checkTaskToTimer, 1800000, g_handler->g_timer);
}

void init_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    //zlog_info(g_handler->log_handler, "init_fun fun \n");
}

void work_idle_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    //zlog_info(g_handler->log_handler, " ---------------- EVENT : MSG_MONTAB_WORK_IDLE: \n");
    run_action_by_step(g_handler, g_handler->g_threadpool);
}

void completed_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    zlog_info(g_handler->log_handler, "completed_fun fun \n");
    led_green(g_handler->log_handler);
    start_monitor(g_handler, g_handler->g_threadpool);
    system("sh /run/media/mmcblk1p1/script/completed.sh");
    // sleep(40);
    //system("reboot");
}

void montab_fault_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    zlog_info(g_handler->log_handler, " ---------------- EVENT : MSG_MONTAB_PROCESS_FAULT: \n");
    zlog_info(g_handler->log_handler, "restart confige process : --- ret = %d \n", buf_len);
    led_blue(g_handler->log_handler);
    if(buf_len == -100){
        printf("204B halt !!!!!! \n");
        zlog_info(g_handler->log_handler, "204B halt !!!!!! \n");
    }else if(buf_len == -101){
        printf("dac halt !!!!!! \n");
        zlog_info(g_handler->log_handler, "dac halt !!!!!! \n");
    }

    // sleep(5);
    // g_handler->g_args->control_run_num = 0;
    run_action_by_step(g_handler, g_handler->g_threadpool);
}

void exit_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    //zlog_info(g_handler->log_handler, "exit_fun fun \n");
    g_handler->g_args->exit_code = 1;
}

void test_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    zlog_info(g_handler->log_handler, "test_fun fun \n");
    //system("sh /run/media/mmcblk1p1/script/looptest.sh");
    //zlog_info(g_handler->log_handler, "system call fun \n");
}

void run_rsdk_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    zlog_info(g_handler->log_handler, "run_rsdk_fun fun \n");
    system("sh /run/media/mmcblk1p1/script/run_rsdk.sh");
    zlog_info(g_handler->log_handler, "end run_rsdk_fun call fun \n");
}

void mon_gpio_exception_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    int gpio_no = buf_len;
    int gpio_value = tmp_data_len;
    led_blue(g_handler->log_handler);
    zlog_info(g_handler->log_handler, "mon_gpio_exception_fun fun ---- gpio_no = %d , gpio_value = %d \n", gpio_no, gpio_value);
}

void mon_exit_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    zlog_info(g_handler->log_handler, "mon_exit_fun fun \n");
}

msg_fun_st msg_flow[] = 
{
    {MSG_RUN_RSDK, run_rsdk_fun},
    {MSG_TEST, test_fun},
    {MSG_TIMEOUT, timeout_fun},
    {MSG_SHOW_WORKQUEUE, show_msg_info_fun},
    {MSG_CLI, init_fun}, 
    {MSG_INIT_COMPLETED, init_fun},
    {MSG_MONTAB_WORK_IDLE, work_idle_fun},
    {MSG_NO_MONTAB_WORK_LEFT, completed_fun},
    {MSG_MONTAB_PROCESS_FAULT,montab_fault_fun},
    {MSG_EXIT,exit_fun},
    {MSG_EXIT_MON_GPIO, mon_exit_fun},
    {MSG_MON_GPIO_EXCEPTION, mon_gpio_exception_fun}
};

int processMessage_table_drive(struct msg_st* msgData, g_handler_para* g_handler) 
{ 
    int type_num = sizeof(msg_flow) / sizeof(msg_fun_st); 
    int i = 0;
    for (i = 0; i < type_num; i++) 
    { 
        if (msg_flow[i].frame_type == msgData->msg_type) 
        { 
            msg_flow[i].fun_ptr(msgData->msg_type, msgData->msg_json, msgData->msg_len, NULL, msgData->tmp_data_len, g_handler); 
            return 0; 
        } 
    }
    zlog_error(g_handler->log_handler, "unknow error frame type !!! ");
    return -1;
}

void 
eventLoop(g_handler_para* g_handler){

    led_blue(g_handler->log_handler);
    run_action_by_step(g_handler, g_handler->g_threadpool);
    addTimeOutWorkToTimer(g_handler->g_msg_queue, checkTaskToTimer, 5000, g_handler->g_timer);
	while(!g_handler->g_args->exit_code){
		struct msg_st* getData = getMsgQueue(g_handler->g_msg_queue);
		if(getData == NULL){
			zlog_info(g_handler->log_handler," getMsgQueue : getData == NULL \n");
			continue;
		}

        int ret = processMessage_table_drive(getData, g_handler);

		free(getData);
	}// end while(1)
}

void postMsg(long int msg_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_msg_queue_para* g_msg_queue){
	struct msg_st* data = (struct msg_st*)malloc(sizeof(struct msg_st));
	data->msg_type = msg_type;
	data->msg_number = msg_type;

	data->tmp_data = tmp_data;
	data->tmp_data_len = tmp_data_len; // just used for constellation IQ data transfer

	data->msg_len = buf_len;
	if(buf != NULL && buf_len != 0)
		memcpy(data->msg_json,buf,buf_len);
	
	int level = 0;
	if(msg_type == MSG_TIMEOUT){
		level = -1;
	}
	postMsgQueue(data,level,g_msg_queue);
}