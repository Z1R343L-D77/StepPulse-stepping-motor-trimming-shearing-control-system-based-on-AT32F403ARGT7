/**
 * @file bsp_mPulse.c
 * @author {Z1R343L} (Z1R343L@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-10
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "at32f403a_407.h"
#include "bsp_mPulse.h"
/**
 * @brief 初始化板载脉冲输出 由于需要mPulse，->PB5->TIM3_CH_2，所以需要初始化TMR3_CH2
 * @param none
 * @retval none
 */
 void bsp_mPulse_init(void)
 {
    gpio_init_type gpio_init_struct;
    tmr_output_config_type tmr_oc_init;

    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);

    gpio_pin_remap_config(TMR3_GMUX_0010, TRUE);   /* PB5 -> TMR3_CH2  */ 

    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_pins = GPIO_PINS_5;
    gpio_init(GPIOB, &gpio_init_struct);

    tmr_base_init(TMR3, 1199, 0);   /* 240MHz/(1199+1) = 200kHz */
    tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);

    tmr_output_default_para_init(&tmr_oc_init);
    tmr_oc_init.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
    tmr_oc_init.oc_output_state = TRUE;
    tmr_oc_init.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
    tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_2, &tmr_oc_init);

    tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 600); /* 50% = 600/1200*/   

    tmr_counter_enable(TMR3, TRUE);
}
 
