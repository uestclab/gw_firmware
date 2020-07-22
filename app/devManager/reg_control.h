#ifndef _REG_CONTROL_INCLUDED_
#define _REG_CONTROL_INCLUDED_


#pragma pack(1)
struct reg_op_cmd_s_{
	char name[64];
	int id;
	int reg;
	int cmd;
	int val;
	int	waite_time; //waite time  for next write or read. unit us

	/*   ͨ����������������ϣ���ʱ Ҫ��ĳЩλΪ0��1�����
		1����Ҫ��Ĵ���31bit�������Ž�������������:
		   ifcon_flag1 = 1;  (((val & ifcon_mask1) == ifcon) ) ������
		   ifcon_mask1 = 0x80_00_00_00;
		   ifcon1 = 0;
	       2����Ҫ��Ĵ���7bit��1����Ž�������������:
	         ifcon_flag2 = 1; (((val & ifcon_mask2) == ifcon_mask2) ) ������
	         ifcon_mask2 = 0x00_00_00_80;
	         ifcon2 = ifcon_mas2;
	*/
	int			  ifcon1;
	unsigned char ifcon1_flag; // 1-- use ifcon1
	int			  ifcon1_mask;

	int			  ifcon2;
	unsigned char ifcon2_flag; // 1-- use ifcon2 
	int			  ifcon2_mask;

	int			  waite_time_ifcon; //waite time  for finish read. unit s
};
#pragma pack()


struct reg_op_cmd_s{
	int item;
	struct reg_op_cmd_s_ *c;
};


int ct_parse2global_reg(void *dev, const char *jbuf, struct reg_op_cmd_s *clk_cmd);


#endif /* _REG_CONTROL_INCLUDED_ */