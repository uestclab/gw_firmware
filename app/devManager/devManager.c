#include "common.h"
#include "cmd_line.h"
#include "led.h"
#include "power.h"
#include "monitor.h"

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

int init_args_para(g_args_para** g_args){
	*g_args = (g_args_para*)malloc(sizeof(g_args_para));
    if(*g_args == NULL){
		return -1;
    }

	(*g_args)->control_run_num = 0;
	INIT_LIST_HEAD(&((*g_args)->excel_list));
	INIT_LIST_HEAD(&((*g_args)->run_list));

	(*g_args)->exit_code = 0;

	return 0;
}

int main(int argc,char** argv)
{
	g_args_para* g_args = NULL;
	if(init_args_para(&g_args) != 0){
		fprintf (stdout, "init_args_para error \n");
        return 0;	
	}

	int ret = get_cmd_line(argc, argv, g_args);
	if(-EPERM == ret){
		fprintf (stdout, "get_cmd_line error : %s \n", g_args->prog_name);
        return 0;
	}



	zlog_category_t *zlog_handler = initLog(g_args->log_file,g_args->prog_name);
	zlog_info(zlog_handler,"******************** start devManager process ********************************\n");
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

#ifdef ZYNQ
	/* monitor init , not start */
	if(init_monitor(zlog_handler) < 0){
		zlog_info(zlog_handler, "init monitor failure !\n");
		return 0;
	}

	/* Peripheral device check */
	if(peripheral_check(g_handler) < 0){
		zlog_info(zlog_handler, "init peripheral_check failure !\n");
		return 0;
	}

	/* led init */
	if(init_led(zlog_handler) < 0){
		zlog_info(zlog_handler, "init led failure !\n");
		return 0;
	}

	/* power init */
	if(init_power(zlog_handler) < 0){
		zlog_info(zlog_handler, "init power module failure !\n");
		return 0;
	}
#else

#endif

	/* init tools */
	g_handler->g_tool = (g_tool_para*)malloc(sizeof(g_tool_para));
	init_spi_info(&(g_handler->g_tool->spi_handler), zlog_handler);
	init_gpio_info(&(g_handler->g_tool->gpio_handler), zlog_handler);
	init_reg_info(&(g_handler->g_tool->reg_handler), zlog_handler);

	test(g_handler);

	if(start_parse(g_handler) !=0){
        zlog_info(zlog_handler, "start_parse error !!");
        return 0;		
	}

	eventLoop(g_handler);

	closeLog();

    return 0;
}
