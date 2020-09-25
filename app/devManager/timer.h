#ifndef TIMER_H
#define TIMER_H

#include "core.h"


//typedef void (*event_handler_pt)(ngx_event_t *ev);

void addTimeOutWorkToTimer(void* data, event_handler_pt cb_fun, int cnt_ms, event_timer_t* g_timer);

#endif//TIMER_H
