
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
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <pthread.h> 
#include <limits.h>
#include <sys/stat.h>   
#include <unistd.h>
#include <fcntl.h>

#include "small_func.h"
#include "list.h"

#include "cJSON.h"
#include "gw_macros_util.h"
#include "zlog.h"
#include "msg_queue.h"
#include "ThreadPool.h"
#include "event_timer.h"
#include "gw_utility.h"


typedef struct g_args_para{
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

#endif /* _GW_CORE_H_INCLUDED_ */
