#include "core.h"

uint16_t run_i2cget_cmd(int addr, int reg, int w_flag)
{
    FILE* fp = NULL; 
    char* value = NULL; 
    int BUFSZ = 100; 
    char buf[BUFSZ]; 
    char command[150]; 
 
	if(w_flag){
		sprintf(command, "i2cget -f -y 0 0x%x 0x%x w", addr, reg);
	}else{
		sprintf(command, "i2cget -f -y 0 0x%x 0x%x", addr, reg);
	}
	//printf("command : %s\n",command); 

    if((fp = popen(command,"r")) == NULL) 
    { 
        return 0;
    } 
    if((fgets(buf,BUFSZ,fp))!= NULL) 
    {
		value = (char*)malloc(strlen(buf)+1);
        memcpy(value,buf,strlen(buf)+1);
		printf("get value : %s", value); 
    }
    
    pclose(fp); 
    
    fp=NULL;

	uint16_t ret = bb_strtoull(value, NULL, 16);
	return ret;

}

void run_i2cset_cmd(int addr, int reg, uint8_t value)
{
    char command[128];
    sprintf(command, "i2cset -f -y 0 0x%x 0x%x 0x%x", addr, reg, value);
	system(command);
}

// ad7417 地址选择
void run_i2cset_swiich_cmd(int addr, int switch_value)
{
    char command[128];
    sprintf(command, "i2cset -f -y 0 0x%x 0x%x", addr, switch_value);
	system(command);
}


/* ----  get operation ---- */

// 整板温度
double get_board_temper(){

// i2cset -y -f 0 0x28 0x00 (地址选择)
// i2cset -y -f 0 0x28 0x01 0x00
//    #等待1ms
// i2cget -y -f 0 0x28 0x00  w   #连续输出两个字节
	double temper = 0;
	run_i2cset_swiich_cmd(0x28,0x00);
	run_i2cset_cmd(0x28,0x01,0x00);
	usleep(1000);

	uint8_t reg = 0x00;
	uint16_t val = run_i2cget_cmd(0x28,reg,1);

	printf("temper : 0x%x\n", val);

	uint16_t place0 = val & (0xff);
	uint16_t place1 = (val & (0xff00)) >> 8;

	uint16_t tmp = (place0 << 2) | (place1 >> 6);
	tmp = tmp & 0x3ff;
	int msb = tmp & 0x2ff;
	if(msb){ // Negative
		temper = (tmp & 0x1ff - 512) * 1.0 / 4;
	}else{
		temper = (tmp) * 1.0 / 4;
	}

    return temper;
}
//int process_reg_json(reg_info_t* reg_info, char* buf)
int enoughHeat(char *jsonBuf, zlog_category_t* log_handler) {
	int ret = -1;
	cJSON * root = NULL;
    cJSON * item = NULL;
    root = cJSON_Parse(jsonBuf);

    item = cJSON_GetObjectItem(root,"temper");
    int expect_temp_i = bb_strtoull(item->valuestring, NULL, 10);
    item = cJSON_GetObjectItem(root,"wait_time_s");
    int expect_waite_time = bb_strtoull(item->valuestring, NULL, 10);
	cJSON_Delete(root);
    free(jsonBuf);

    double expect_temper = expect_temp_i * 1.0;
    double curr_temper = get_board_temper();
    double new_temper  = 0;
    if(curr_temper > expect_temper){
        ret = 0;
    }else{
        start_heat_device(log_handler);
        sleep(expect_waite_time);
        stop_heat_device(log_handler);
        new_temper = get_board_temper();
        if(new_temper > expect_temper){
            ret = 0;
        }
    }

    zlog_info(log_handler, "heat_proc --- ret : %d, board_temper = %f, expect_temper = %f, expect_waite_time(s) = %d, new_temper = %f\n", 
                    ret, curr_temper, expect_temper, expect_waite_time, new_temper);

    return ret;
}