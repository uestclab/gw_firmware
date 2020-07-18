#include "common.h"

zlog_category_t * initLog(const char* path, char* app_name){
	int rc;
	zlog_category_t *zlog_handler = NULL;

	rc = zlog_init(path);

	if (rc) {
		printf("init serverLog failed\n");
		return NULL;
	}

	zlog_handler = zlog_get_category(app_name);

	if (!zlog_handler) {
		printf("get cat fail\n");

		zlog_fini();
		return NULL;
	}

	return zlog_handler;
}

void closeLog(){
	zlog_fini();
}

void test(g_handler_para* g_handler){
    zlog_info(g_handler->log_handler, "cmd line : -l %s , -f %s \n", g_handler->g_args->log_file, g_handler->g_args->conf_file);

}

int main(int argc,char** argv)
{

    g_args_para* g_args = (g_args_para*)malloc(sizeof(g_args_para));
    if(g_args == NULL){
 		fprintf (stdout, "error : g_args == NULL \n");
        return 0;       
    }

	int ret = get_cmd_line(argc, argv, g_args);
	if(-EPERM == ret){
		fprintf (stdout, "get_cmd_line error : %s \n", g_args->prog_name);
        return 0;
	}



	zlog_category_t *zlog_handler = initLog(g_args->log_file,g_args->prog_name);
	zlog_info(zlog_handler,"******************** start initdev process ********************************\n");
	zlog_info(zlog_handler,"this version built time is:[%s  %s]\n",__DATE__,__TIME__);

    g_handler_para* g_handler = (g_handler_para*)malloc(sizeof(g_handler_para));
    g_handler->g_args = g_args;

    g_handler->log_handler = zlog_handler;
	/* msg_queue */
	g_msg_queue_para* g_msg_queue = createMsgQueue();
	if(g_msg_queue == NULL){
		zlog_info(zlog_handler,"No msg_queue created \n");
		return 0;
	}
    g_handler->g_msg_queue = g_msg_queue;

	// /* reg dev */
	// g_RegDev_para* g_RegDev = NULL;
	// int state = initRegdev(&g_RegDev, zlog_handler);
	// if(state != 0 ){
	// 	zlog_info(zlog_handler,"initRegdev create failed !");
	// 	return 0;
	// }

	/* Timer handler */
	event_timer_t* g_timer = NULL;
    int state = ngx_event_timer_init(&g_timer);
    if(state != 0){
        zlog_info(zlog_handler, "ngx_event_timer_init error , stat = %d \n", state);
        return 0;
    }
    g_handler->g_timer = g_timer;

	/* ThreadPool handler */
	ThreadPool* g_threadpool = NULL;
	createThreadPool(64, 20, &g_threadpool);
    g_handler->g_threadpool = g_threadpool;

	// /* server thread */
	// g_server_para* g_server = NULL;
	// state = CreateServerThread(&g_server, g_threadpool, g_msg_queue, zlog_handler);
	// if(state == -1 || g_server == NULL){
	// 	zlog_info(zlog_handler,"No server thread created \n");
	// 	return 0;
	// }

    test(g_handler);

	eventLoop(g_handler);

	closeLog();

    return 0;
}