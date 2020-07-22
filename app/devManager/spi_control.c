#include "core.h"
#include "cf_ops.h"

#include "common.h"


static int get_json_data_spi(const char *jbuf, void* dev,  struct spi_op_cmd_s *clk_cmd, int use)
{
#ifdef	_DEBUG_LOG_
	printf("json spi : \n %s \n", jbuf);
#endif
    printf("json spi : \n %s \n", jbuf);
	int ret, i;
	parse_type* root, *child, *iter;

	char spidev[256];
	int spimode;
	int spimaxclk;
	int spibpw;

	struct spi_op_cmd_s_ *pop_cmd;

	if(clk_cmd->c){
		xfree(clk_cmd->c);
		clk_cmd->c = NULL;
		clk_cmd->item = 0;
	}

	root = parse_config_file(jbuf);
	if(NULL == root){
		printf("parse_config_file(%s) fail!",__func__);
		ret = -EINVAL;
		goto exit;
	}

	ret = get_item_str_val(root, "spidev", spidev);
	if(ret){ 
#ifdef	_DEBUG_LOG_
        printf("get spidev fail!");
#endif
	}

	ret = get_item_val(root, "spimode", &spimode);
	if(ret){
#ifdef	_DEBUG_LOG_ 
		printf("get spimode fail!");
#endif
	}

	ret = get_item_val(root, "spimaxclk", &spimaxclk);
	if(ret){
#ifdef	_DEBUG_LOG_
		printf("get spimaxclk fail!");
#endif
    }

	ret = get_item_val(root, "spibpw", &spibpw);
	if(ret){
#ifdef	_DEBUG_LOG_
		printf("get spibpw fail!");
#endif
    }

	spidev_set_para(dev, spimode, spimaxclk, spibpw, spidev, use);

	ret = get_array_size(root, "op_cmd");
	if(ret<0){
		printf("get op_cmd size fail!");
		goto exit;
	}

	clk_cmd->item = ret;

	clk_cmd->c = (struct spi_op_cmd_s_*)xzalloc(clk_cmd->item * sizeof(struct spi_op_cmd_s_));
	if(NULL == clk_cmd->c){
		printf("op_cmd malloc fail!");
		ret = -ENOMEM;;
		goto exit;
	}
	
	child = get_obj_item(root, "op_cmd");
	if(NULL == child){
		printf("get_obj_item(op_cmd) fail!");
		ret = -EINVAL;;
		goto exit;
	}

	ret = 0;

	i = 0;
	get_ArrayForEach(iter, child){
		pop_cmd = clk_cmd->c + i;
		ret = get_item_val(iter, "cmd", (int*)&(pop_cmd->cmd));
		if(ret < 0){
			printf("get op_cmd cmd fail! index(%d)",i+1);
			goto exit;
		}
		ret = get_item_val(iter, "instruction_data", &(pop_cmd->instruction_data));
		if(ret < 0){
			printf("get op_cmd instruction_data fail! index(%d)",i+1);
			goto exit;
		}
		ret = get_item_val(iter, "waite_time", &(pop_cmd->waite_time));
		if(ret < 0){
#ifdef	_DEBUG_LOG_
			printf("get op_cmd waite_time fail! index(%d)",i+1);
#endif
        }
		
		ret = get_item_val(iter, "ifcon1", &(pop_cmd->ifcon1));
		if(ret < 0){
#ifdef	_DEBUG_LOG_
			printf("get op_cmd ifcon1 fail! index(%d)",i+1);
#endif
        }

		ret = get_item_val(iter, "ifcon1_flag", (int*)&(pop_cmd->ifcon1_flag));
		if(ret < 0){
#ifdef	_DEBUG_LOG_
			printf("get op_cmd ifcon1_flag fail! index(%d)",i+1);
#endif
        }
		
		ret = get_item_val(iter, "ifcon1_mask", &(pop_cmd->ifcon1_mask));
		if(ret < 0){
#ifdef	_DEBUG_LOG_
			printf("get op_cmd ifcon1_mask fail! index(%d)",i+1);
#endif
        }

		ret = get_item_val(iter, "ifcon2", &(pop_cmd->ifcon2));
		if(ret < 0){
#ifdef	_DEBUG_LOG_
			printf("get op_cmd ifcon2 fail! index(%d)",i+1);
#endif
        }

		ret = get_item_val(iter, "ifcon2_flag", (int*)&(pop_cmd->ifcon2_flag));
		if(ret < 0){
#ifdef	_DEBUG_LOG_
			printf("get op_cmd ifcon2_flag fail! index(%d)",i+1);
#endif
        }

		ret = get_item_val(iter, "ifcon2_mask", &(pop_cmd->ifcon2_mask));
		if(ret < 0){
#ifdef	_DEBUG_LOG_
			printf("get op_cmd ifcon2_mask fail! index(%d)",i+1);
#endif
        }

		ret = get_item_val(iter, "waite_time_ifcon", &(pop_cmd->waite_time_ifcon));
		if(ret < 0){
			if(pop_cmd->ifcon1_flag || pop_cmd->ifcon2_flag){
				pop_cmd->waite_time_ifcon = 1000000;
#ifdef	_DEBUG_LOG_
				printf("get op_cmd waite_time_ifcon fail! index(%d). use default(%d us)",i+1,pop_cmd->waite_time_ifcon);
#endif
			}else{
#ifdef	_DEBUG_LOG_
				printf("get op_cmd ifcon_mask2 fail! index(%d)",i+1);
#endif
			}
		}
		
		pop_cmd->instruction_data = htonl(pop_cmd->instruction_data) >> 8;
		
		ret = 0;
		i++;
	}
			
exit:
	parse_config_del(root);
	if((ret < 0) && (clk_cmd->c)){
		xfree(clk_cmd->c);
	}

	return ret;
}


int ct_parse2global_spi(void *dev, const char *jbuf, struct spi_op_cmd_s *clk_cmd){
	return get_json_data_spi(jbuf, dev, clk_cmd, CT_DEV);
}

void printf_cmd(struct spi_op_cmd_s *use_cmd){
    printf("item = %d \n", use_cmd->item);
    int i = 0;
    for(i=0;i<use_cmd->item;i++){
        printf("cmd : %u, instruction_data : %x, waite_time : %d, ifcon1 : %d , ifcon1_flag : %u, ifcon1_mask : %d \n",
                use_cmd->c[i].cmd,  use_cmd->c[i].instruction_data,  use_cmd->c[i].waite_time,  use_cmd->c[i].ifcon1, 
                use_cmd->c[i].ifcon1_flag,  use_cmd->c[i].ifcon1_mask);
        printf("waite_time_ifcon : %d, ifcon2 : %d , ifcon2_flag : %u, ifcon2_mask : %d \n",
                use_cmd->c[i].waite_time_ifcon,  use_cmd->c[i].ifcon2, use_cmd->c[i].ifcon2_flag,  use_cmd->c[i].ifcon2_mask);
    }
}

int spi_cmd_process(const char *conf_file){
    void *spidev;
    struct spi_op_cmd_s	use_cmd;
    use_cmd.c = NULL;
    use_cmd.item = 0;

    char *file_buf;

    spidev_init(&(spidev));

    int ret = read_config_file(conf_file, &file_buf);

    ret = ct_parse2global_spi(spidev, file_buf, &use_cmd);

    printf("ct_parse2global_spi \n");

    ret = spidev_open(spidev,CT_DEV);
    
    if(ret < 0){
		printf("spidev_open err!(devname:%s)",spidev_get_devname(spidev));
        goto exit;
	}else{

        printf_cmd(&use_cmd);

		// ret = send_to_spidev(use_cmd, (void*)args, h_jstr);
		// if(ret < 0){
		// 	printf("send_to_spidev err!(devname:%s)",spidev_get_devname(spidev));
		// }
	}

    spidev_close(spidev);

exit:

	return ret;
}



