
/*
 * Copyright (C) uestclab
 * Copyright (C) Gw, Inc.
 */


#ifndef _GW_CORE_H_INCLUDED_
#define _GW_CORE_H_INCLUDED_

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

#include "cJSON.h"
#include "gw_macros_util.h"
#include "zlog.h"
#include "msg_queue.h"
#include "ThreadPool.h"
#include "event_timer.h"
#include "gw_utility.h"


typedef struct g_args_para{
	char*   prog_name;
	char*   conf_file;
	char*   log_file;
}g_args_para;

typedef struct g_handler_para{
    zlog_category_t*    log_handler;
    g_msg_queue_para*   g_msg_queue;
    ThreadPool*         g_threadpool;
    event_timer_t*      g_timer;
    g_args_para*        g_args;
}g_handler_para;



#endif /* _GW_CORE_H_INCLUDED_ */
