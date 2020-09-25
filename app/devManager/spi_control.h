#ifndef _SPI_CONTROL_INCLUDED_
#define _SPI_CONTROL_INCLUDED_

#define BUFF_LEN 256


// static uint32_t mode;
// static uint8_t bits = 8;
// static uint32_t speed = 0x7A120;


struct spi_op_cmd_s{
	unsigned char cmd; //0--read  1--write
	uint32_t instuction;

	int			  ifcon1;
	unsigned char ifcon_flag1; 
	uint32_t	  ifcon_mask1;

	int			  ifcon2;
	unsigned char ifcon_flag2; 
	uint32_t	  ifcon_mask2;

	int			  waite_time_ifcon;
	int           waite_time; // us
};

typedef struct spi_info_t{
	zlog_category_t* log_handler;
	char *spidev;
	int fd;
	uint32_t spimode;
	uint32_t spimaxclk; //speed
	uint8_t spibpw; // bits
	int cnt;
	struct spi_op_cmd_s *c;
}spi_info_t;

void init_spi_info(spi_info_t** handler, zlog_category_t* log_handler);

int spi_tool(char* jsonBuf, spi_info_t *spi_handler);

#endif /* _SPI_CONTROL_INCLUDED_ */