/*
    1. parse montab
    2. producer role
*/
#include "common.h"
#include "parse_helper.h"
#include "rf_module.h"

// for test
zlog_category_t*    log_handler = NULL;

int parse_excel(parser_t *parser, void *args)
{
	int ret = 0;
	char *tokens[4];
	int len_tk[5], action_types;
	excel_node_s *new_act = NULL;
    g_handler_para* g_handler = ( g_handler_para*)args;
    g_args_para*    g_args = g_handler->g_args;
	
	while(ret = config_read(parser, tokens, ARRAY_SIZE(tokens), "#:[") > 0){
		len_tk[0] = strlen(tokens[0]); //seq
		len_tk[1] = strlen(tokens[1]); //name
		len_tk[2] = strlen(tokens[2]); //action_type
		len_tk[3] = strlen(tokens[3]); //process
		
		len_tk[4] = len_tk[0] + len_tk[1] + len_tk[2] + len_tk[3];
		if(len_tk[4]){
			new_act = (excel_node_s *)xzalloc(sizeof(excel_node_s) + len_tk[4]);
			new_act->seq =  bb_strtoull(tokens[0], NULL, 0);//atoi(tokens[0]);

			new_act->name = (char *)xmalloc(len_tk[1] + 1);
			if(new_act->name){
				strcpy((void*)new_act->name, tokens[1]);
			}else{
				ret = -ENOMEM;
				goto exit;
			}

			new_act->action_type = strcmp(tokens[2], "respawn")? \
									(strcmp(tokens[2], "waite")? ONCE: WAITE)\
									: RESPAWN;
			strcpy(new_act->command, tokens[3]);
			
			list_add_tail(&(new_act->next), &(g_args->excel_list));
		}
	
	}
exit:
	return ret;
}

int parse_run(parser_t *parser, void *args)
{
    int cnt = 0;
	int ret = 0, i;
	char *tokens[5];
	int len_tk[6], action_types;
	run_node_s *new_act = NULL;
    g_handler_para* g_handler = ( g_handler_para*)args;
    g_args_para*    g_args = g_handler->g_args;
	struct stat_node_s *new_stat;
	int is_empty;
	while(ret = config_read(parser, tokens, ARRAY_SIZE(tokens), "#:[") > 0){
		len_tk[0] = strlen(tokens[0]); //seq
		len_tk[1] = strlen(tokens[1]); //dst
		len_tk[2] = strlen(tokens[2]); //conf_file
		len_tk[3] = strlen(tokens[3]); //st_file
		len_tk[4] = strlen(tokens[4]); //st_timeout
		
		len_tk[5] = len_tk[0] + len_tk[1] + len_tk[2] + len_tk[3] + len_tk[4];
		if(len_tk[5]){
			new_act = (run_node_s*)xzalloc(sizeof(run_node_s) + len_tk[5]);
			new_act->seq =  bb_strtoull(tokens[0], NULL, 0);//atoi(tokens[0]);

			new_act->dst = (char *)xmalloc(len_tk[1] + 1);
			if(new_act->dst){
				strcpy((void*)new_act->dst, tokens[1]);
			}else{
				ret = -ENOMEM;
				goto exit;
			}
			
			new_act->st_to = bb_strtoull(tokens[4], NULL, 0);
			
			is_empty = input_str_is_empty(tokens[2]);
			if(!is_empty){
				new_act->con_file=  (char *)xmalloc(len_tk[2] + 1);
				if(new_act->con_file){
					strcpy((void*)new_act->con_file, tokens[2]);
				}else{
					ret = -ENOMEM;
					goto exit;
				}
			}
			is_empty = input_str_is_empty(tokens[3]);
			INIT_LIST_HEAD(&(new_act->stat_list));
			if(!is_empty){
				new_act->st_file_list = (char *)xzalloc(len_tk[3] + 1);
				if(new_act->st_file_list){
					strcpy((void*)new_act->st_file_list, tokens[3]);
                    // zlog_info(log_handler, "new_act->st_file_list : %s \n", new_act->st_file_list);
					ret = st_file_list_parse(new_act->st_file_list, &(new_act->st_file_tk), &(new_act->st_file_tk_no));
					if(ret < 0){
						goto exit;
					}
					//zlog_info(log_handler, "new_act->st_file_tk_no = %d \n", new_act->st_file_tk_no);
					for(i = 0; i < new_act->st_file_tk_no; i++){
						new_stat = (stat_node_s*)xzalloc(sizeof(stat_node_s));
						if(NULL == new_stat){
							ret = -ENOMEM;
							goto exit;
						}
						ret = st_file_parse(new_act->st_file_tk[i], new_stat);
						if(ret < 0){
							goto exit;
						}
						list_add_tail(&(new_stat->next), &(new_act->stat_list));
					}
				}else{
					ret = -ENOMEM;
					goto exit;
				}
			}
            new_act->seq_num = cnt;
            cnt++;
			list_add_tail(&(new_act->next), &(g_args->run_list));
		}
	
	}
exit:
	return ret;
}

int parse_inittab(parser_t **parser, g_handler_para* g_handler)
{
	char *token[4];
    int ret = -1;
    if(g_handler->g_args->conf_file != NULL){
	    *parser = config_open2(g_handler->g_args->conf_file, fopen_for_read);

        if(*parser != NULL){
            if(found_title(*parser, "[excel]") == 0){
                ret = parse_excel(*parser, g_handler);
            }

            if(check_run_title(*parser, "[run]") == 0){
                ret = parse_run(*parser, g_handler);
            }

            return ret;
        }
    }
    return -1;
}

void display(g_handler_para* g_handler){
	zlog_info(g_handler->log_handler,"  ---------------- display () --------------------------\n");
	g_args_para* g_args = g_handler->g_args;
    zlog_info(g_handler->log_handler,"  ---------------- run () --------------------------\n");
    run_node_s* pnode_run = NULL;
	list_for_each_entry(pnode_run, &g_args->run_list, next) {
		if(pnode_run->dst != NULL){    
			zlog_info(g_handler->log_handler," control_seq_num : %d -- dst: %s, conf: %s, st_file: %s, st_to: %d \n" , 
                pnode_run->seq_num, pnode_run->dst, pnode_run->con_file, pnode_run->st_file_list, pnode_run->st_to);
		}
	}

	zlog_info(g_handler->log_handler,"  ---------------- end display () ----------------------\n");
}

/* --------------------------------------------------------------------------------------------------------------- */

int check_state(char* json_path, g_handler_para* g_handler){
	char *jsonBuf = get_json_buf(json_path);
	char *dst = NULL;
	cJSON * root = NULL;
    cJSON * item = NULL;
    root = cJSON_Parse(jsonBuf);
	cJSON * array_item = NULL;
	if(cJSON_HasObjectItem(root,"dst") == 1){
		item = cJSON_GetObjectItem(root , "dst");
		dst = malloc(32);
		memcpy(dst, item->valuestring, strlen(item->valuestring)+1);
	}
	cJSON_Delete(root);
	int ret = 0;
	if(dst != NULL){
		if(0 == strcmp(dst, "gpio")){
			ret = gpio_tool(jsonBuf, g_handler->g_tool->gpio_handler);
		}else if(0 == strcmp(dst, "adc") || 0 == strcmp(dst, "dac") || 0 == strcmp(dst, "hmc") || 0 == strcmp(dst, "lmx")){
			ret = spi_tool(jsonBuf, g_handler->g_tool->spi_handler);
			if(ret !=0){
				zlog_info(g_handler->log_handler, "spi : %s \n", json_path);
				ret = -101;
			}
		}else if(0 == strcmp(dst, "reg")){
			ret = reg_tool(jsonBuf, g_handler->g_tool->reg_handler);
			if(ret != 0){
				zlog_info(g_handler->log_handler, "reg : %s \n", json_path);
				ret = -100; // for loop test
			}
		}
		return ret;
	}else{
		free(jsonBuf);
		return -1;
	}
}


int shell_proc(const char *dst, void* cmd, g_handler_para* g_handler){
	run_node_s* run_cmd = (run_node_s*)cmd;
	if(0 == strcmp(run_cmd->con_file, "exit")){
		printf("shell proc exit \n");
		postMsg(MSG_EXIT,NULL,0,NULL,0,g_handler->g_msg_queue);
	}else if(0 == strcmp(run_cmd->con_file, "completed")){
		postMsg(MSG_TEST,NULL,0,NULL,0,g_handler->g_msg_queue);
	}else if(0 == strcmp(run_cmd->con_file, "runrsdk")){
		postMsg(MSG_RUN_RSDK,NULL,0,NULL,0,g_handler->g_msg_queue);
	}else{
		system(run_cmd->con_file);
	}
	return 0;
}

int spi_proc(const char *dst, void* cmd, g_handler_para* g_handler){
	run_node_s* run_cmd = (run_node_s*)cmd;
	//zlog_info(log_handler, "spi_tool : %s\n", run_cmd->con_file);
	if(run_cmd->con_file){
		char *jsonBuf = get_json_buf(run_cmd->con_file);
		if(spi_tool(jsonBuf, g_handler->g_tool->spi_handler) != 0){
			return -1;
		}
	}
	if(run_cmd->st_file_tk_no){
		//zlog_info(log_handler, "spi : check_state --- ****** \n");
		if(check_state(run_cmd->st_file_list, g_handler) != 0){
			return -101;
		}
	}

	return 0;
}

int gpio_proc(const char *dst, void* cmd, g_handler_para* g_handler){
	run_node_s* run_cmd = (run_node_s*)cmd;
	//zlog_info(log_handler, "gpio_tool : %s\n", run_cmd->con_file);
	if(run_cmd->con_file){
		char* jsonBuf = get_json_buf(run_cmd->con_file);
		if(gpio_tool(jsonBuf, g_handler->g_tool->gpio_handler) != 0){
			return -1;
		}
	}

	if(run_cmd->st_file_tk_no){
		//zlog_info(log_handler, "gpio : check_state --- ****** \n");
		if(check_state(run_cmd->st_file_list, g_handler) != 0){
			return -3;
		}
	}

	return 0;
}

int reg_proc(const char *dst, void* cmd, g_handler_para* g_handler){
	run_node_s* run_cmd = (run_node_s*)cmd;
	//zlog_info(log_handler, "reg_tool : %s\n", run_cmd->con_file);
	if(run_cmd->con_file){
		char* jsonBuf = get_json_buf(run_cmd->con_file);
		if(reg_tool(jsonBuf,g_handler->g_tool->reg_handler) != 0){
			return -1;
		}
	}

	if(run_cmd->st_file_tk_no){
		//zlog_info(log_handler, "reg : check_state --- ****** \n");
		int ret = check_state(run_cmd->st_file_list, g_handler);
		if(ret  != 0){
			if(ret == -100){
				return ret;
			}
			return -4;
		}
	}

	return 0;
}

int rf_fun(const char *dst, void* cmd, g_handler_para* g_handler){
	run_node_s* run_cmd = (run_node_s*)cmd;
	zlog_info(log_handler, "rf_fun --- dst : %s\n", dst);

	if(g_handler->g_peripheral->rf_on == 1){
		return 0;
	}

	return 0;
}

int rf_freq_func(const char *dst, void* cmd, g_handler_para* g_handler){
	run_node_s* run_cmd = (run_node_s*)cmd;
	zlog_info(g_handler->log_handler, "rf_freq_func --- dst : %s\n", dst);

	if(g_handler->g_peripheral->rf_on == 1){
		return 0;
	}

	int ret = config_freq(g_handler);

	return ret;
}

int mon_proc(const char *dst, void* cmd, g_handler_para* g_handler){
	run_node_s* run_cmd = (run_node_s*)cmd;
	return 0;
}

dst_fun_st dst_flow[] = {
	{"shell", shell_proc},
	{"adc",spi_proc},
	{"dac",spi_proc},
	{"hmc",spi_proc},
	{"lmx",spi_proc},
	{"reg",reg_proc},
	{"rf_freq",rf_freq_func},
	{"rf",rf_fun},
	{"gpio",gpio_proc},
	{"mon",mon_proc},
};

// gpio , adc ,dac, hmc, lmx, reg, rf, shell
int process_dispatch(run_node_s* pnode, g_handler_para* g_handler){
	int type_num = sizeof(dst_flow) / sizeof(dst_fun_st);
	int i = 0;
	int ret = -1;
	for(i = 0; i < type_num; i++){
		if(0 == strcmp(pnode->dst, dst_flow[i].msg_dst)){
			ret = dst_flow[i].fun_ptr(pnode->dst,pnode,g_handler);
			break;
		}
	}
	return ret;
}

void* montab_work_thread(void* args){
	g_handler_para* g_handler = (g_handler_para*)args;
    g_args_para* g_args = g_handler->g_args;
	int ret = 0;
    int control_num = g_args->control_run_num;
    run_node_s* pnode = NULL;
	list_for_each_entry(pnode, &g_args->run_list, next) {
		if(pnode->seq_num == control_num){    
			zlog_info(g_handler->log_handler," control_num : %d ---- dst: %s, conf: %s, st_file: %s, st_to: %d \n" , 
                control_num, pnode->dst, pnode->con_file, pnode->st_file_list, pnode->st_to);

			// process
			ret = process_dispatch(pnode,g_handler);
			if(ret  == 0){
				g_args->control_run_num ++;
				postMsg(MSG_MONTAB_WORK_IDLE,NULL,0,NULL,0,g_handler->g_msg_queue);
				return NULL;
			}else{
				postMsg(MSG_MONTAB_PROCESS_FAULT,NULL,ret,NULL,0,g_handler->g_msg_queue);
				return NULL;
			}
		}
	}

	postMsg(MSG_NO_MONTAB_WORK_LEFT,NULL,0,NULL,0,g_handler->g_msg_queue);
	return NULL;
    
}

int start_parse(g_handler_para* g_handler){
	log_handler = g_handler->log_handler;
    parser_t *parser;
    if(parse_inittab(&parser, g_handler) == -1){
        return -1;
    }
    //display(g_handler);
    return 0;
}

// step by control_run_num
void run_action_by_step(g_handler_para* g_handler, ThreadPool* g_threadpool){
    AddWorker(montab_work_thread,(void*)g_handler,g_threadpool);
}