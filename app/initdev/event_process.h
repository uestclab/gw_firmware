#ifndef EVENT_PROCESS_H
#define EVENT_PROCESS_H

#include "core.h"

void 
eventLoop(g_handler_para* g_handler);

void postMsg(long int msg_type, char *buf, int buf_len, void* tmp_data, int tmp_data_len, g_msg_queue_para* g_msg_queue);

#endif//EVENT_PROCESS_H


