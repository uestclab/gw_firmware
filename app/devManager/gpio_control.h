#ifndef __GPIO_CONTROL_H__
#define	__GPIO_CONTROL_H__

#pragma pack(1)
struct gpio_op_cmd_s{
	char name[128];
	int  id;
	int intr;
	int act;
	int gpio_no;
	int dir;
	int val;
#define	UNSUPPORT	-1
	int excep_val;
	int waite_time;//us
};
#pragma pack()

typedef struct gpio_info_t{
	struct list_head headNode;
	int item_num;
}gpio_info_t;

typedef struct gpio_node_t{
	struct list_head list;
	int fd;
	int read_expect;
	struct gpio_op_cmd_s cmd;
}gpio_node_t;

void init_gpio_info(gpio_info_t** handler);
void reset_gpio_info(gpio_info_t* handler);

int gpio_tool(char* jsonBuf, gpio_info_t *gpio_handler);

#endif