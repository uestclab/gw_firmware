#include "core.h"

#include "small_func.h"
#include "read_file.h"

#include "spi_control.h"
#include "reg_control.h"

#include <regdev/regdev_common.h>
#include <gpio/gpio_ops.h>
#include <spidev/spi_common.h>
#include <i2cdev/i2c_common.h>

typedef enum msg_event{
	/* test msg type */
    MSG_CLI = 1,
	MSG_SHOW_WORKQUEUE,
	MSG_TIMEOUT,
	MSG_INIT_COMPLETED,
	MSG_MONTAB_WORK_IDLE,
	MSG_NO_MONTAB_WORK_LEFT,
	MSG_MONTAB_PROCESS_FAULT,
	MSG_EXIT,
	MSG_TEST,
	MSG_RUN_RSDK,
	MSG_EXIT_MON_GPIO,
	MSG_MON_GPIO_EXCEPTION,
}msg_event;

// 表驱动
typedef void (*PROC_MSG_FUN)(long int frame_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_handler_para* g_handler);
typedef struct __msg_fun_st 
{ 
    const long int frame_type;//消息类型 
    PROC_MSG_FUN fun_ptr;//函数指针 
}msg_fun_st;


typedef int (*SIP_MSG_FUN)(const char *dst, void* cmd, g_handler_para* g_handler);
typedef struct __dst_fun_st{
	const char *msg_dst;
	SIP_MSG_FUN fun_ptr;
}dst_fun_st;



#include "event_process.h"
#include "timer.h"
#include "parse_mon.h"

