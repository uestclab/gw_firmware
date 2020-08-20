#include "core.h"
#include "common.h"

#include <linux/spi/spidev.h>
#include "cJSON.h"
#include "read_file.h"
#include "str2digit.h"
#include "spi_control.h"

uint8_t default_rx[BUFF_LEN] = {0, };
uint8_t padd_tx[BUFF_LEN] = {0, };

void init_spi_info(spi_info_t** handler, zlog_category_t* log_handler){

	*handler = (spi_info_t*)xzalloc(sizeof(spi_info_t));

	spi_info_t* spi_handler = *handler;

	spi_handler->spidev = NULL;
	spi_handler->fd  = -1;
	spi_handler->spimode = 0;
	spi_handler->spimaxclk = 0x7A120;
	spi_handler->spibpw = 8;
	spi_handler->cnt = 0;
	spi_handler->c = NULL;
	spi_handler->log_handler = log_handler;
}

void reset_spi_info(spi_info_t* handler){
	handler->spidev = NULL;
	handler->fd  = -1;
	handler->spimode = 0;
	handler->spimaxclk = 0x7A120;
	handler->spibpw = 8;
	handler->cnt = 0;
	if(handler->c){
		free(handler->c);
	}
	handler->c = NULL;
}

static void pabort(const char *s)
{
	perror(s);
	abort();
}

int init_spidev(spi_info_t* spi_handler){

	char *device = spi_handler->spidev;

	uint32_t mode = spi_handler->spimode;
	uint8_t bits = spi_handler->spibpw;
	uint32_t speed = spi_handler->spimaxclk;	

	int ret = 0;
	int fd;

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	spi_handler->fd = fd;

	mode = mode | SPI_MODE_0;
	//printf("spi mode: 0x%x\n", mode);
	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	// printf("spi mode: 0x%x\n", mode);
	// printf("bits per word: %d\n", bits);
	// printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
}

void close_spidev(spi_info_t* spi_handler){
	close(spi_handler->fd);
	if(spi_handler->spidev){
		free(spi_handler->spidev);
		spi_handler->spidev = NULL;
	}
	if(spi_handler->c){
		free(spi_handler->c);
		spi_handler->c = NULL;
	}
}

static void transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len, uint32_t speed, uint8_t bits)
{
	int ret;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

}

int parse_spidev(char* buf, spi_info_t* spi_handler){
	cJSON * root = NULL;
    cJSON * item = NULL;
    root = cJSON_Parse(buf);

	if(cJSON_HasObjectItem(root,"spidev") == 0){
		printf("invalid spi json !\n");
		return -1;
	}
	item = cJSON_GetObjectItem(root , "spidev");
	spi_handler->spidev = (char*)xzalloc(strlen(item->valuestring)+1);
	memcpy(spi_handler->spidev, item->valuestring, strlen(item->valuestring)+1);

	if(cJSON_HasObjectItem(root,"spimode") == 1){
		item = cJSON_GetObjectItem(root , "spimode");
		spi_handler->spimode = bb_strtoull(item->valuestring, NULL, 16);
	}
	if(cJSON_HasObjectItem(root,"spimaxclk") == 1){
		item = cJSON_GetObjectItem(root , "spimaxclk");
		spi_handler->spimaxclk = bb_strtoull(item->valuestring, NULL, 16);
	}
	if(cJSON_HasObjectItem(root,"spibpw") == 1){
		item = cJSON_GetObjectItem(root , "spibpw");
		spi_handler->spibpw = bb_strtoull(item->valuestring, NULL, 16);
	}

	cJSON * array_item = NULL;

	array_item = cJSON_GetObjectItem(root , "op_cmd");

	int op_cnt = cJSON_GetArraySize(array_item);
	spi_handler->cnt = op_cnt;
	zlog_info(spi_handler->log_handler, "spi op_cnt = %d \n", op_cnt);

	spi_handler->c = (struct spi_op_cmd_s*)xzalloc(sizeof(struct spi_op_cmd_s) * op_cnt);

	for(int i=0;i<op_cnt;i++){
		cJSON * pSub = cJSON_GetArrayItem(array_item, i);
		item = cJSON_GetObjectItem(pSub,"instruction_data");
		int instuction;
		instuction = bb_strtoull(item->valuestring, NULL, 16);
		spi_handler->c[i].instuction = htonl(instuction) >> 8;

		item = cJSON_GetObjectItem(pSub,"cmd");
		spi_handler->c[i].cmd = bb_strtoull(item->valuestring, NULL, 16);

		if(cJSON_HasObjectItem(pSub,"ifcon1") == 1){
			item = cJSON_GetObjectItem(pSub , "ifcon1");
			spi_handler->c[i].ifcon1 = bb_strtoull(item->valuestring, NULL, 16);
		}

		if(cJSON_HasObjectItem(pSub,"ifcon_flag1") == 1){
			item = cJSON_GetObjectItem(pSub , "ifcon_flag1");
			spi_handler->c[i].ifcon_flag1 = bb_strtoull(item->valuestring, NULL, 16);
		}

		if(cJSON_HasObjectItem(pSub,"ifcon_mask1") == 1){
			item = cJSON_GetObjectItem(pSub , "ifcon_mask1");
			spi_handler->c[i].ifcon_mask1 = bb_strtoull(item->valuestring, NULL, 16);
		}

		if(cJSON_HasObjectItem(pSub,"ifcon2") == 1){
			item = cJSON_GetObjectItem(pSub , "ifcon2");
			spi_handler->c[i].ifcon2 = bb_strtoull(item->valuestring, NULL, 16);
		}

		if(cJSON_HasObjectItem(pSub,"ifcon_flag2") == 1){
			item = cJSON_GetObjectItem(pSub , "ifcon_flag2");
			spi_handler->c[i].ifcon_flag2 = bb_strtoull(item->valuestring, NULL, 16);
		}

		if(cJSON_HasObjectItem(pSub,"ifcon_mask2") == 1){
			item = cJSON_GetObjectItem(pSub , "ifcon_mask2");
			spi_handler->c[i].ifcon_mask2 = bb_strtoull(item->valuestring, NULL, 16);
		}

		if(spi_handler->c[i].ifcon_flag1 || spi_handler->c[i].ifcon_flag2){
				spi_handler->c[i].waite_time_ifcon = 1000000;
				spi_handler->c[i].waite_time = 0;
		}

		if(cJSON_HasObjectItem(pSub,"waite_time_ifcon") == 1){
			item = cJSON_GetObjectItem(pSub , "waite_time_ifcon");
			spi_handler->c[i].waite_time_ifcon = bb_strtoull(item->valuestring, NULL, 16);
		}

		if(cJSON_HasObjectItem(pSub,"waite_time") == 1){
			item = cJSON_GetObjectItem(pSub , "waite_time");
			spi_handler->c[i].waite_time = get_item_val(item->valuestring);
		}

	}
	
	cJSON_Delete(root);
	return 0;
}

int process_spi_cmd(spi_info_t* spi_handler){
	uint32_t instuction;
	int con1 = 1;
	int con2 = 1;
	struct timeval start_time;
	int op_cnt = spi_handler->cnt;

	int state = 0;

	for(int i=0;i<op_cnt;i++){
		if(spi_handler->c[i].cmd){
			instuction = spi_handler->c[i].instuction;
			transfer(spi_handler->fd, (char*)(&instuction), default_rx, 3, spi_handler->spimaxclk, spi_handler->spibpw);
			//printf("instruction_data : 0x%x \n", htonl(instuction)>>8);
			if(spi_handler->c[i].waite_time){
				usleep(spi_handler->c[i].waite_time);
			}	
		}else{
			con1 = 1;
			con2 = 1;
			get_usec(&start_time);
			for(;;){
				instuction = spi_handler->c[i].instuction;
				transfer(spi_handler->fd, (char*)(&instuction), default_rx, 3, spi_handler->spimaxclk, spi_handler->spibpw);
				//printf("for: instruction_data 0x%x\n", htonl(instuction)>>8);
				char ret_str[16];
				snprintf(ret_str, ARRAY_SIZE(ret_str), "0x%x%x%x", default_rx[0], default_rx[1], default_rx[2]);
				//printf("read cmd return value : %s \n", ret_str);
				if(spi_handler->c[i].ifcon_flag1){
					con1 = (((default_rx[2] & spi_handler->c[i].ifcon_mask1) == spi_handler->c[i].ifcon1) ) ;
				}

				if(spi_handler->c[i].ifcon_flag2){
					con2 = (((default_rx[2] & spi_handler->c[i].ifcon_mask2) == spi_handler->c[i].ifcon2) ) ;
					//printf("con2 = %d --- ret : 0x%x , mask : 0x%x , ifcon2 : 0x%x\n", con2, 
					// default_rx[2], spi_handler->c[i].ifcon_mask2, spi_handler->c[i].ifcon2);
				}

				if(spi_handler->c[i].waite_time){
					usleep(spi_handler->c[i].waite_time);
				}

				if(con1 && con2){
					break;
				}else{
					if(time_expire(start_time, spi_handler->c[i].waite_time_ifcon)){
						//printf("read state error -- instruction_data 0x%x\n", htonl(instuction)>>8);
						state = 1;
						break;
					}
				}
			}
		}
		if(state){
			return -1;
		}
	}

	return 0;
}

int spi_tool(char* jsonBuf, spi_info_t *spi_handler){
	reset_spi_info(spi_handler);
	if(parse_spidev(jsonBuf,spi_handler) != 0){
		free(jsonBuf);
		return -1;
	}

	/* spi init */
	int ret = init_spidev(spi_handler);

	if(process_spi_cmd(spi_handler) != 0){
		free(jsonBuf);
		close_spidev(spi_handler);
		return -1;
	}
	free(jsonBuf);
	close_spidev(spi_handler);
	return 0;
}