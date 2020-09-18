#include "core.h"
#include "common.h"

char* run_read_cmd(int addr)
{
    FILE* fp = NULL; 
    char* value = NULL; 
    int BUFSZ = 100; 
    char buf[BUFSZ]; 
    char command[150]; 
 
    sprintf(command, "devmem 0x%x", addr);

    if((fp = popen(command,"r")) == NULL) 
    { 
        return NULL;
    } 
    if((fgets(buf,BUFSZ,fp))!= NULL) 
    {

		value = (char*)malloc(strlen(buf)+1);
        memcpy(value,buf,strlen(buf)+1); 
    }
    
    pclose(fp); 
    
    fp=NULL; 
    
	return value;
}

void init_reg_info(reg_info_t** handler, zlog_category_t* log_handler){
    *handler = (reg_info_t*)xzalloc(sizeof(reg_info_t));
    reg_info_t* reg_info = *handler;
	reg_info->base = 0;
	reg_info->cnt  = 0;
	reg_info->read_expect = 0;
	if(reg_info->c){
		free(reg_info->c);
	}
    reg_info->c = NULL;
	reg_info->log_handler = log_handler;
}

void reset_reg_info(reg_info_t* reg_info){
	reg_info->base = 0;
	reg_info->cnt  = 0;
	reg_info->read_expect = 0;
	if(reg_info->c){
		free(reg_info->c);
	}
	reg_info->c = NULL;
}

int process_reg_json(reg_info_t* reg_info, char* buf){

	char cmd[64];
	int ret = 0;

	cJSON * root = NULL;
    cJSON * item = NULL;
	cJSON * array_item = NULL;
    root = cJSON_Parse(buf);

	if(cJSON_HasObjectItem(root,"base") == 0){
		printf("invalid reg json !\n");
		return -1;
	}
	item = cJSON_GetObjectItem(root , "base");
	reg_info->base = bb_strtoull(item->valuestring, NULL, 16);

	// read return item
	array_item = cJSON_GetObjectItem(root , "op_cmd");
	int op_cnt = cJSON_GetArraySize(array_item);
	//zlog_info(reg_info->log_handler, "reg op_cnt = %d \n", op_cnt);
	reg_info->c = (struct reg_op_cmd_s*)xzalloc(sizeof(struct reg_op_cmd_s) * op_cnt);
	reg_info->cnt = op_cnt;

	array_item = cJSON_GetObjectItem(root , "return");

	for(int i=0;i<op_cnt;i++){
		cJSON * pSub = cJSON_GetArrayItem(array_item, i);
		if(cJSON_HasObjectItem(pSub,"ret") == 1){
			item = cJSON_GetObjectItem(pSub,"ret");
			reg_info->c[i].expect = get_item_val(item->valuestring);
			reg_info->read_expect = 1;
		}
	}


	// read op_cmd item
	array_item = cJSON_GetObjectItem(root , "op_cmd");
	op_cnt = cJSON_GetArraySize(array_item);

	for(int i=0;i<op_cnt;i++){
		cJSON * pSub = cJSON_GetArrayItem(array_item, i);

		if(cJSON_HasObjectItem(pSub,"offset") == 1){
			item = cJSON_GetObjectItem(pSub,"offset");
			reg_info->c[i].offset = bb_strtoull(item->valuestring, NULL, 16);
		}


		if(cJSON_HasObjectItem(pSub,"cmd") == 1){
			item = cJSON_GetObjectItem(pSub,"cmd");
			reg_info->c[i].cmd = bb_strtoull(item->valuestring, NULL, 10);
		}

		if(cJSON_HasObjectItem(pSub,"val") == 1){
			item = cJSON_GetObjectItem(pSub,"val");
			reg_info->c[i].value = bb_strtoull(item->valuestring, NULL, 16);
		}

		if(cJSON_HasObjectItem(pSub,"waite_time") == 1){
			item = cJSON_GetObjectItem(pSub,"waite_time");
			reg_info->c[i].waite_time = bb_strtoull(item->valuestring, NULL, 10);
		}

		//devmem 0x43c20100 32 0xcf000fff

		int addr = reg_info->c[i].offset + reg_info->base;

		if(reg_info->c[i].cmd){
			sprintf(cmd, "devmem 0x%x 32 0x%x", addr, reg_info->c[i].value);
			system(cmd);
			//printf("write: %s \n", cmd);
			if(reg_info->c[i].waite_time !=0){
				//printf("usleep(%d)\n",reg_info->c[i].waite_time);
				usleep(reg_info->c[i].waite_time);
			}

		}else{
			char* str_value = run_read_cmd(addr);
			if(str_value){
				//zlog_info(reg_info->log_handler,"read: devmem 0x%x --- val = %s\n", addr, str_value);
				if(reg_info->read_expect){
					int read_val = get_item_val(str_value);
					if(read_val != reg_info->c[i].expect){
						free(str_value);
						printf("reg : read != expect expect = 0x%x , read = 0x%x\n", reg_info->c[i].expect, read_val);
						zlog_info(reg_info->log_handler, "reg : read != expect expect = 0x%x , read = 0x%x\n", reg_info->c[i].expect, read_val);
						ret = -1;
						goto exit;
					}else{
						//zlog_info(reg_info->log_handler, "reg : read == expect expect = 0x%x , read = 0x%x\n", reg_info->c[i].expect, read_val);
					}
				}
				free(str_value);
			}
		}

	}

exit:
	cJSON_Delete(root);
	free(reg_info->c);
    reg_info->c = NULL;

	return ret;
}

int reg_tool(char* jsonBuf, reg_info_t *reg_handler){
	reset_reg_info(reg_handler);
    int ret = process_reg_json(reg_handler, jsonBuf);
	free(jsonBuf);
    return ret;
}




