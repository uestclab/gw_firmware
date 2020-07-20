#include "core.h"


typedef enum msg_event{
	/* test msg type */
    MSG_CLI = 1,
	MSG_MONITOR,
	MSG_TIMEOUT,
	MSG_CONF_CHANGE,
	MSG_WORK_IDLE,
	MSG_NO_WORK_LEFT,
}msg_event;

// 表驱动
typedef void (*PROC_MSG_FUN)(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler);
typedef struct __msg_fun_st 
{ 
    const long int frame_type;//消息类型 
    PROC_MSG_FUN fun_ptr;//函数指针 
}msg_fun_st;



#include "event_process.h"
#include "timer.h"
#include "cmd_line.h"
#include "parse_mon.h"

