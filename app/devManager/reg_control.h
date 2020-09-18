#ifndef _REG_CONTROL_INCLUDED_
#define _REG_CONTROL_INCLUDED_

struct reg_op_cmd_s{
	unsigned char cmd; //0--read  1--write
	int value;
	int offset;
	int waite_time; // us
	int expect;
};

typedef struct reg_info_t{
	zlog_category_t* log_handler;
	int base;
	int cnt;
	int read_expect;
	struct reg_op_cmd_s *c;
}reg_info_t;

void init_reg_info(reg_info_t** handler, zlog_category_t* log_handler);
int reg_tool(char* jsonBuf, reg_info_t *reg_handler);

char* run_read_cmd(int addr);

#endif /* _REG_CONTROL_INCLUDED_ */