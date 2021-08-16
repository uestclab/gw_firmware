#ifndef __I2C_CONTROL_H__
#define	__I2C_CONTROL_H__

double get_board_temper();
int enoughHeat(char *jsonBuf, zlog_category_t* log_handler);

#endif