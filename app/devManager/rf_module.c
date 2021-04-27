#include "common.h"
#include "rf_module.h"

char *path_11_tx = "/run/media/mmcblk1p1/etc/GW50_lmx2594_Tx_config_V1_PFD200M_FREQ14G125_step1.json";
char *path_11_rx = "/run/media/mmcblk1p1/etc/GW50_lmx2594_Rx_config_V1_PFD200M_FREQ12G333_step1.json";
char *path_10_tx = "/run/media/mmcblk1p1/etc/GW50_lmx2594_Tx_config_V1_PFD200M_FREQ12G4583_step1.json";
char *path_10_rx = "/run/media/mmcblk1p1/etc/GW50_lmx2594_Rx_config_V1_PFD200M_FREQ14G_step1.json";


int get_rf_gpio_value(int rf_gpio){
    int8_t read_val = -1;
    if(gpio_get_val(rf_gpio, &read_val) < 0 ){
        printf("read rf_gpio failed !\n");
        return -1;
    }
    return read_val;
}

int rf_module_init(){
    if(gpio_open(RF_IN_PLL_LOCK_0, 0) < 0){
		return -1;
	}
    if(gpio_open(RF_IN_PLL_LOCK_1, 0) < 0){
		return -1;
	}
    return 0;
}


int config_freq(g_handler_para* g_handler){
    int gpio970val = get_rf_gpio_value(RF_IN_PLACE_1);
    int gpio971val = get_rf_gpio_value(RF_IN_PLACE_2);

    char *tx_path = NULL;
    char *rx_path = NULL;

    if(gpio971val == 1 && gpio970val == 1){
        tx_path = path_11_tx;
        rx_path = path_11_rx;
    }else if(gpio971val == 1 && gpio970val == 0){
        tx_path = path_10_tx;
        rx_path = path_10_rx;
    }

    /* enable tx channel */
    char *jsonBuf = get_json_buf(tx_path);
    if(spi_tool(jsonBuf, g_handler->g_tool->spi_handler) != 0){
        return -1;
    }

    // check pll gpio
    if(get_rf_gpio_value(RF_IN_PLL_LOCK_0) == 1){
        system("spi_tool /dev/spidev32766.1	0x2C1FA4");
        zlog_info(g_handler->log_handler, "enable rf tx channel\n");
    }else{
        zlog_error(g_handler->log_handler, "rf Tx pll is not locked \n");
        return -2;
    }

    /* enable rx channel */
    jsonBuf = get_json_buf(rx_path);
    if(spi_tool(jsonBuf, g_handler->g_tool->spi_handler) != 0){
        return -1;
    }

    // check pll gpio
    if(get_rf_gpio_value(RF_IN_PLL_LOCK_1) == 1){
        system("spi_tool /dev/spidev32766.2	0x2C1FA4");
        zlog_info(g_handler->log_handler, "enable rf rx channel\n");
    }else{
        zlog_error(g_handler->log_handler, "rf Rx pll is not locked \n");
        return -2;
    }

}