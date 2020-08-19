#include "common.h"
#include "cJSON.h"

void init_gpio_info(gpio_info_t** handler){
	*handler = (gpio_info_t*)xzalloc(sizeof(gpio_info_t));
	gpio_info_t* gpio_handler = *handler;
	gpio_handler->item_num = 0;
	INIT_LIST_HEAD(&(gpio_handler->headNode));
}

void reset_gpio_info(gpio_info_t* handler){
	int cnt = 0;
    struct list_head *pos, *n;
    gpio_node_t* pnode = NULL;
    list_for_each_safe(pos, n, &handler->headNode){
        pnode = list_entry(pos, struct gpio_node_t, list);
        list_del(pos);
        free(pnode);
        cnt++;
    }

    if(cnt != handler->item_num){
        printf("reset_gpio_info error cnt !\n");
    }

	INIT_LIST_HEAD(&(handler->headNode));
    handler->item_num = 0;
}

int parse_gpio(char* buf, gpio_info_t* handler){
	cJSON * root = NULL;
    cJSON * item = NULL;
    root = cJSON_Parse(buf);

	cJSON * array_item = NULL;

	array_item = cJSON_GetObjectItem(root , "op_cmd");

	int op_cnt = cJSON_GetArraySize(array_item);
	handler->item_num = op_cnt;
	//printf("op_cnt = %d \n", op_cnt);

	for(int i=0;i<op_cnt;i++){
		gpio_node_t* node = (gpio_node_t*)xzalloc(sizeof(gpio_node_t));
		cJSON * pSub = cJSON_GetArrayItem(array_item, i);
		item = cJSON_GetObjectItem(pSub,"gpio_no");
		node->cmd.gpio_no = get_item_val(item->valuestring);

		if(cJSON_HasObjectItem(pSub,"name") == 1){
			item = cJSON_GetObjectItem(pSub , "name");
			memcpy(node->cmd.name, item->valuestring, strlen(item->valuestring)+1);
		}

		if(cJSON_HasObjectItem(pSub,"id") == 1){
			item = cJSON_GetObjectItem(pSub , "id");
			node->cmd.id = get_item_val(item->valuestring);
		}

		if(cJSON_HasObjectItem(pSub,"intr") == 1){
			item = cJSON_GetObjectItem(pSub , "intr");
			node->cmd.intr = get_item_val(item->valuestring);
		}

		if(cJSON_HasObjectItem(pSub,"dir") == 1){
			item = cJSON_GetObjectItem(pSub , "dir");
			node->cmd.dir = get_item_val(item->valuestring);
		}

		if(cJSON_HasObjectItem(pSub,"val") == 1){
			item = cJSON_GetObjectItem(pSub , "val");
			node->cmd.val = get_item_val(item->valuestring);
		}

		if(cJSON_HasObjectItem(pSub,"exp") == 1){
			item = cJSON_GetObjectItem(pSub , "exp");
			node->cmd.excep_val = get_item_val(item->valuestring);
			node->read_expect = 1;
		}

		if(cJSON_HasObjectItem(pSub,"sleep") == 1){
			item = cJSON_GetObjectItem(pSub , "sleep");
			node->cmd.waite_time = get_item_val(item->valuestring);
		}

		list_add_tail(&(node->list), &(handler->headNode));
	}
	
	cJSON_Delete(root);
	return 0;
}

int process_gpio_cmd(gpio_info_t* handler){

	int ret = 0;
	struct list_head *pos = NULL;
	gpio_node_t* tmp_node = NULL;
	int cnt = handler->item_num;
	struct gpio_op_cmd_s* tmp_cmd = NULL; 
	list_for_each(pos, &(handler->headNode)){
		tmp_node = list_entry(pos, struct gpio_node_t, list);
		tmp_cmd = &(tmp_node->cmd);

		if(gpio_open(tmp_cmd->gpio_no, tmp_cmd->dir) < 0){
			return -1;
		}

		if(GPIO_DIR_OUT == tmp_cmd->dir){
			// write
			if(gpio_set_val(tmp_cmd->gpio_no, tmp_cmd->val) < 0){
				printf("write failed : gpio_no = %d , val = %d \n", tmp_cmd->gpio_no, tmp_cmd->val);
			}else{
				printf("write success : gpio_no = %d , val = %d \n", tmp_cmd->gpio_no, tmp_cmd->val);
			}

			if(tmp_cmd->waite_time){
				usleep(tmp_cmd->waite_time);
			}

		}else{
			// read
			int8_t read_val = -1;
			if(gpio_get_val(tmp_cmd->gpio_no, &read_val) < 0 ){
				printf("read failed !\n");
                return -1;
			}else{
				if(tmp_node->read_expect){
					if(tmp_cmd->excep_val == read_val){
						printf("read == expect: gpio_no = %d , read_val = %d, expect = %d \n", 
							tmp_cmd->gpio_no, read_val, tmp_cmd->excep_val);
					}else{
						printf("read != expect: gpio_no = %d , read_val = %d, expect = %d \n", 
							tmp_cmd->gpio_no, read_val, tmp_cmd->excep_val);
                        return -1;						
					}
				}else{
					printf("read : gpio_no = %d , read_val = %d \n", tmp_cmd->gpio_no, read_val);
				}
			}
		}

	}

	return 0;
}


int gpio_tool(char* jsonBuf, gpio_info_t *gpio_handler){
	if(parse_gpio(jsonBuf,gpio_handler) == -1){
        free(jsonBuf);
		return -1;
	}

	int ret = process_gpio_cmd(gpio_handler);
    free(jsonBuf);
	return ret;
}