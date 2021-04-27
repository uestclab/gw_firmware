#include "core.h"

typedef struct stat_node_s{
	struct list_head next;
	int item;
	int item_exp;
	char dst[128];
	int  stat;
	struct stat_ret_s *ret_exp;
	struct stat_ret_s *ret;
}stat_node_s;

typedef struct excel_node_s {
	struct list_head next;
	pid_t pid;	
	int  seq;
	char  *name;
	int  action_type;
	char command[1]; // '\0'
}excel_node_s;


typedef struct run_node_s {
	struct list_head next;
	
    int     seq_num;
	int     seq; // normal : 0, if check point , used to re-configure start position
	char    *dst;
	char    *con_file;
	char    *st_file_list;
	char    **st_file_tk;//[32]
	int     st_file_tk_no;
	int     st_to; // normal : 0 , if check failed and  non zero , return to seq check point

	struct list_head stat_list;
	
	void *priv;
}run_node_s;

int start_parse(g_handler_para* g_handler);
void run_action_by_step(g_handler_para* g_handler, ThreadPool* g_threadpool);
