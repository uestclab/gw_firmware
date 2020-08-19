
/*
 * Copyright (C) uestclab
 * Copyright (C) Gw, Inc.
 */


#ifndef _GW_CORE_H_INCLUDED_
#define _GW_CORE_H_INCLUDED_


#ifndef	_GNU_SOURCE
#define		_GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <pthread.h> 
#include <limits.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <sys/time.h>
#include <dirent.h>
#include <poll.h>

#include "small_func.h"
#include "str2digit.h"
#include "list.h"

#include "cJSON.h"
#include "gw_macros_util.h"
#include "zlog.h"
#include "msg_queue.h"
#include "ThreadPool.h"
#include "event_timer.h"
#include "gw_utility.h"

#include "spi_control.h"
#include "gpio_control.h"


typedef struct g_tool_para{
	spi_info_t *spi_handler;
	gpio_info_t *gpio_handler;
}g_tool_para;

typedef struct g_args_para{
	int     exit_code;
	int     control_run_num;
	char*   prog_name;
	char*   conf_file;
	char*   log_file;
    struct list_head excel_list;
	struct list_head run_list;
}g_args_para;

typedef struct g_handler_para{
    zlog_category_t*    log_handler;
    g_msg_queue_para*   g_msg_queue;
    ThreadPool*         g_threadpool;
    event_timer_t*      g_timer;
    g_args_para*        g_args;
	g_tool_para*        g_tool;
}g_handler_para;


static inline void* xmalloc(size_t size)
{
	void *ptr = malloc(size);
	return ptr;
}

static inline void* xzalloc(size_t size)
{
	void *ptr = malloc(size);
	memset(ptr, 0, size);
	return ptr;
}

static inline void* xrealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	return ptr;
}

#define	xfree(ptr) do{ \
	if(ptr){ \
		free(ptr); \
		ptr = NULL; \
	} \
}while(0)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define	RESPAWN		0x1
#define ONCE        0x2
#define WAITE       0x3

static inline void get_usec(struct timeval *time)
{
	gettimeofday(time, NULL);

}

static inline int time_expire(struct timeval start, int expire_us)
{
	struct timeval now;
	int sec;
	int diff_usec;
	
	gettimeofday(&now, NULL);

	sec = now.tv_sec - start.tv_sec;
	
	diff_usec = sec * 1000000 + now.tv_usec - start.tv_usec;

	return ((diff_usec - expire_us)>=0)?1:0;
}

#endif /* _GW_CORE_H_INCLUDED_ */



