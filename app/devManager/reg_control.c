#include "core.h"
#include "cf_ops.h"

#include "common.h"


static int get_json_data_reg(const char *jbuf, void* dev,  struct reg_op_cmd_s *clk_cmd, int use)
{
	int ret, i;
	parse_type* root, *child, *iter;

	int base_addr = 0;
	int map_size = 0x10000; //64K

	struct reg_op_cmd_s_ *pop_cmd;
	
	regdev_close(dev);
	if(clk_cmd->c){
		xfree(clk_cmd->c);
		clk_cmd->item = 0;
	}
	

	root = parse_config_file(jbuf);
	if(NULL == root){
		// log_error("parse_config_file(%s) fail!",__func__);
		ret = -EINVAL;
		goto exit;
	}

	ret = get_item_val(root, "base", &base_addr);
	if(ret){ 
		// log_error("get base_addr fail!");
	}
	ret = get_item_val(root, "map_size", &map_size);
	if(ret){ 
		// log_debug("get map_size fail! use defult: 64K");
	}

	ret = get_array_size(root, "op_cmd");
	if(ret<0){
		// log_error("get op_cmd size fail!");
		goto exit;
	}

	clk_cmd->item = ret;

	clk_cmd->c = (struct reg_op_cmd_s_*)xzalloc(clk_cmd->item * sizeof(struct reg_op_cmd_s_));
	if(NULL == clk_cmd->c){
		// log_error("op_cmd malloc fail!");
		ret = -ENOMEM;;
		goto exit;
	}
	
	child = get_obj_item(root, "op_cmd");
	if(NULL == child){
		// log_error("get_obj_item(op_cmd) fail!");
		ret = -EINVAL;;
		goto exit;
	}

	ret = 0;

	i = 0;
	get_ArrayForEach(iter, child){
		pop_cmd = clk_cmd->c + i;

		ret = get_item_str_val(iter, "name", pop_cmd->name);
		if(ret < 0){
			pop_cmd->name[0] = '\0';
			// log_warn("can't get \"name\".index(%d), cat(\"name\")",i+1);
		}

		ret = get_item_val(iter, "id", &(pop_cmd->id));
		if(ret < 0){
			// log_warn("can't get \"id\".index(%d), cat(\"id\")",i+1);
			pop_cmd->id = -1;
		}

		ret = get_item_val(iter, "offset",  (int*)&(pop_cmd->reg));
		if(ret < 0){
			// log_error("get offset fail! index(%d)",i+1);
			goto exit;
		}

		ret = get_item_val(iter, "cmd", (int*)&(pop_cmd->cmd));
		if(ret < 0){
			// log_error("get cmd fail! index(%d)",i+1);
			goto exit;
		}

		ret = get_item_val(iter, "val",  (int*)&(pop_cmd->val));
		if(ret < 0){
			// log_debug("get val fail! index(%d)",i+1);
			if(pop_cmd->cmd){
				// log_error("get val fail! index(%d)",i+1);
				goto exit;
			}
		}

		ret = get_item_val(iter, "waite_time", (int*)&(pop_cmd->waite_time));
		if(ret < 0){
			pop_cmd->waite_time = 0;
			//log_debug("get cmd fail! index(%d)",i+1);
		}
		
		ret = get_item_val(iter, "ifcon1", (int*)&(pop_cmd->ifcon1));
		if(ret < 0){
			pop_cmd->ifcon1 = 0;
			//log_debug("get ifcon1 fail! index(%d)",i+1);
		}

		ret = get_item_val(iter, "id", &(pop_cmd->id));
		if(ret < 0){
			pop_cmd->ifcon1_flag = 0;
			//log_debug("get ifcon1_flag fail! index(%d)",i+1);
		}

		ret = get_item_val(iter, "ifcon1_mask", (int*)&(pop_cmd->ifcon1_mask));
		if(ret < 0){
			pop_cmd->ifcon1_mask = 0;
			//log_debug("get ifcon1_mask fail! index(%d)",i+1);
		}

		//ret = get_array_val(i, "ifcon2", child, (int*)&(pop_cmd->ifcon2));
		ret = get_item_val(iter, "ifcon2", (int*)&(pop_cmd->ifcon2));
		if(ret < 0){
			pop_cmd->ifcon2 = 0;
			//log_debug("get ifcon2 fail! index(%d)",i+1);
		}

		ret = get_item_val(iter, "ifcon2_flag", (int*)&(pop_cmd->ifcon2_flag));
		if(ret < 0){
			pop_cmd->ifcon2_flag = 0;
			//log_debug("get ifcon2_flag fail! index(%d)",i+1);
		}

		ret = get_item_val(iter, "ifcon2_mask", (int*)&(pop_cmd->ifcon2_mask));
		if(ret < 0){
			pop_cmd->ifcon2_mask = 0;
			//log_debug("get ifcon2_mask fail! index(%d)",i+1);
		}

		ret = get_item_val(iter, "waite_ifcon", (int*)&(pop_cmd->waite_time_ifcon));
		if(ret < 0){
			pop_cmd->waite_time_ifcon = 0;
			//log_debug("get waite_ifcon fail! index(%d)",i+1);
		}

		ret = 0;
		i++;
	}

	regdev_set_para(dev, base_addr, map_size);
exit:
	parse_config_del(root);

	if((ret < 0) && (clk_cmd->c)){
		clk_cmd->item = 0;
		xfree(clk_cmd->c);
	}

	return ret;
}

int ct_parse2global_reg(void *dev, const char *jbuf, struct reg_op_cmd_s *clk_cmd)
{
	return get_json_data_reg(jbuf, dev, clk_cmd, CT_DEV);
}