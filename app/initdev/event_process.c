#include "common.h"

/* typedef void (*event_handler_pt)(ngx_event_t *ev); */
void checkTaskToTimer(ngx_event_t *ev){
    g_msg_queue_para* g_msg_queue = (g_msg_queue_para*)(ev->data);
    postMsg(MSG_TIMEOUT, NULL, 0, NULL, 0, g_msg_queue);
    free(ev);
}

void timeout_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    zlog_info(g_handler->log_handler, "timeout fun \n");
    addTimeOutWorkToTimer(g_handler->g_msg_queue, checkTaskToTimer, 30000, g_handler->g_timer);
}

void reload_conf_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    zlog_info(g_handler->log_handler, "reload_conf_fun fun \n");
}

void work_idle_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    zlog_info(g_handler->log_handler, "work_idle_fun fun \n");
    run_action_by_step(g_handler, g_handler->g_threadpool);
}

void completed_fun(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler){
    zlog_info(g_handler->log_handler, "completed_fun fun \n");
}

msg_fun_st msg_flow[] = 
{ 
    {MSG_TIMEOUT, timeout_fun}, 
    {MSG_CLI, reload_conf_fun}, 
    {MSG_CONF_CHANGE, reload_conf_fun},
    {MSG_WORK_IDLE, work_idle_fun},
    {MSG_NO_WORK_LEFT, completed_fun}
};

void parseEventJson(char* event_buf, int buf_len){
	cJSON * root = NULL;
    cJSON * item = NULL;
    root = cJSON_Parse(event_buf);
    if(cJSON_HasObjectItem(root,"localIp") == 1){
        item = cJSON_GetObjectItem(root,"localIp");
    }
    if(cJSON_HasObjectItem(root,"currentTime") == 1){
        item = cJSON_GetObjectItem(root,"currentTime");
    }
    cJSON_Delete(root);
}

int processMessage_table_drive(struct msg_st* msgData, g_handler_para* g_handler) 
{ 
    int type_num = sizeof(msg_flow) / sizeof(msg_fun_st); 
    int i = 0;
    for (i = 0; i < type_num; i++) 
    { 
        if (msg_flow[i].frame_type == msgData->msg_type) 
        { 
            msg_flow[i].fun_ptr(msgData->msg_type, msgData->msg_json, msgData->msg_len, NULL, 0, g_handler); 
            return 0; 
        } 
    }
    zlog_error(g_handler->log_handler, "unknow error frame type !!! ");
    return -1;
}

void 
eventLoop(g_handler_para* g_handler){

    run_action_by_step(g_handler, g_handler->g_threadpool);
    addTimeOutWorkToTimer(g_handler->g_msg_queue, checkTaskToTimer, 5000, g_handler->g_timer);
	while(1){
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
	if(msg_type == MSG_CONF_CHANGE){
		level = -1;
	}
	postMsgQueue(data,level,g_msg_queue);
}