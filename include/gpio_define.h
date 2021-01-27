#ifndef __GPIO_DEFINE_H__
#define	__GPIO_DEFINE_H__

typedef enum rf_state_gpio{
    RF_IN_PLL_LOCK_0 = 967, // tx pll lock : 1 -- locked
    RF_IN_PLL_LOCK_1 = 968, // rx pll lock : 1 -- locked
}rf_state_gpio;

typedef enum peripheral_gpio{
    RF_IN_PLACE_0 = 969,
    RF_IN_PLACE_1 = 970,
    RF_IN_PLACE_2 = 971,
    SWITCH_DET_N  = 992,
}peripheral_gpio;

typedef enum mon_gpio{
    MON_PG_ZYNQ = 974,
	MON_PG_CLK  = 975,
    MON_PG_ADC  = 976,
    MON_PG_DAC  = 977,
    MON_PG_5V   = 978,

    MON_HMC_GPIO1 = 996,
    MON_HMC_GPIO2 = 997,
}mon_gpio;

typedef enum led_gpio{
    LED_RED = 986,
	LED_GREEN = 987,
    LED_BLUE = 988,
}led_gpio;

typedef enum power_gpio{
    POWER_PFO = 1014,
    POWER_CAPGD = 1015,
    POWER_EN_PTC = 1016,
    POWER_SMB_ALERT = 1017,
}power_gpio;

#endif