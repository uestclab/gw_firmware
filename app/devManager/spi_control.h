#ifndef _SPI_CONTROL_INCLUDED_
#define _SPI_CONTROL_INCLUDED_


#pragma pack(1)

struct spi_op_cmd_s_{
	unsigned char cmd; //0--read  1--write
	
	//unsigned char width; //1--byte  2--word  4--dword
	int			  instruction_data;
	 
	int			  waite_time; //waite time  for next write or read. unit us

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


struct spi_op_cmd_s{
	int item;
	struct spi_op_cmd_s_ *c;
};


int ct_parse2global_spi(void *dev, const char *jbuf, struct spi_op_cmd_s *clk_cmd);

int spi_cmd_process(const char *conf_file);


#endif /* _SPI_CONTROL_INCLUDED_ */