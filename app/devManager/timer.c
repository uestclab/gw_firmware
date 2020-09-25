#include "core.h"

void addTimeOutWorkToTimer(void* data, event_handler_pt cb_fun, int cnt_ms, event_timer_t* g_timer){
    ngx_event_t *ev = (ngx_event_t*)malloc(sizeof(ngx_event_t));
    ev->handler = cb_fun;
    ev->data = data;
    add_event_timer(ev, cnt_ms, g_timer); // 60s
}
