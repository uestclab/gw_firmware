#include "common.h"
#include "rf_module.h"

#ifdef ZYNQ
char *path_11_tx = "/run/media/mmcblk1p1/etc/GW50_lmx2594_Tx_config_V1_PFD200M_FREQ14G125_step1.json";
char *path_11_rx = "/run/media/mmcblk1p1/etc/GW50_lmx2594_Rx_config_V1_PFD200M_FREQ12G4583_step1.json";
char *path_10_tx = "/run/media/mmcblk1p1/etc/GW50_lmx2594_Tx_config_V1_PFD200M_FREQ12G333_step1.json";
char *path_10_rx = "/run/media/mmcblk1p1/etc/GW50_lmx2594_Rx_config_V1_PFD200M_FREQ14G_step1.json";
#endif

int get_rf_gpio_value(int rf_gpio){
    int8_t read_val = -1;
    if(gpio_get_val(rf_gpio, &read_val) < 0 ){
        printf("read rf_gpio failed !\n");
        return -1;
    }
    return read_val;
}

int rf_module_init(){
    // if(gpio_open(RF_IN_PLL_LOCK_0, 0) < 0){
	// 	return -1;
	// }
    // if(gpio_open(RF_IN_PLL_LOCK_1, 0) < 0){
	// 	return -1;
	// }
    return 0;
}


int config_freq(char* E1_E2_jsonBuf, g_handler_para* g_handler){
    // int gpio970val = get_rf_gpio_value(RF_IN_PLACE_1);
    // int gpio971val = get_rf_gpio_value(RF_IN_PLACE_2);

    // char *tx_path = NULL;
    // char *rx_path = NULL;

    // if(gpio971val == 1 && gpio970val == 1){
    //     tx_path = path_11_tx;
    //     rx_path = path_11_rx;
    // }else if(gpio971val == 1 && gpio970val == 0){
    //     tx_path = path_10_tx;
    //     rx_path = path_10_rx;
    // }

    int ret = 0;
    char *tx_path = NULL;
    char *rx_path = NULL;

    cJSON * root = NULL;
    cJSON * item = NULL;
    root = cJSON_Parse(E1_E2_jsonBuf);

    if(cJSON_HasObjectItem(root,"tx_path") == 0){
        zlog_error(g_handler->log_handler, "invalid spi json !\n");
        free(E1_E2_jsonBuf);
        return -1;
    }
    if(cJSON_HasObjectItem(root,"rx_path") == 0){
        zlog_error(g_handler->log_handler, "invalid spi json !\n");
        free(E1_E2_jsonBuf);
        return -1;
    }
    item = cJSON_GetObjectItem(root , "tx_path");
    tx_path = (char*)xzalloc(strlen(item->valuestring)+1);
    memcpy(tx_path, item->valuestring, strlen(item->valuestring)+1);
    item = cJSON_GetObjectItem(root , "rx_path");
    rx_path = (char*)xzalloc(strlen(item->valuestring)+1);
    memcpy(rx_path, item->valuestring, strlen(item->valuestring)+1);
    free(E1_E2_jsonBuf);
    cJSON_Delete(root);

    zlog_info(g_handler->log_handler, "tx_path = %s \n", tx_path);
    zlog_info(g_handler->log_handler, "rx_path = %s \n", rx_path);

    /* enable tx channel */
    char *jsonBuf = get_json_buf(tx_path); 
    if(spi_tool(jsonBuf, g_handler->g_tool->spi_handler) != 0){
        zlog_error(g_handler->log_handler, "rf Tx pll is not locked \n");
        ret = -1;
        goto exit;
    }
    system("spi_tool /dev/spidev32766.1	0x2C16A4");
    zlog_info(g_handler->log_handler, "enable rf tx channel\n");

    /* enable rx channel */
    jsonBuf = get_json_buf(rx_path);
    if(spi_tool(jsonBuf, g_handler->g_tool->spi_handler) != 0){
        zlog_error(g_handler->log_handler, "rf Rx pll is not locked \n");
        ret = -1;
        goto exit;
    }
    system("spi_tool /dev/spidev32766.2	0x2C16A4");
    zlog_info(g_handler->log_handler, "enable rf rx channel\n");

exit:
    free(tx_path);
    free(rx_path);
    return ret;

}