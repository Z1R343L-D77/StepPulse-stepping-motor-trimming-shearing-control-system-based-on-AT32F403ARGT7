/**
 * @file bsp_time.c
 * @author {Z1R343L} (Z1R343L@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "bsp_time.h"
#include "at32f403a_407_clock.h"
#include "app_control.h"
#include "bsp_step.h"

volatile uint32_t systick_ms = 0;

/**
  * @brief  初始化 TMR1 为 1ms 定时器
  * @param  none
  * @retval none
  */
void bsp_timer_init(void)
{
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  
  /* 244MHz / 244 = 1MHz, 1MHz / 1000 = 1kHz (精确 1ms) */
  tmr_base_init(TMR1, 999, 243);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
  
  nvic_irq_enable(TMR1_OVF_TMR10_IRQn, 0, 0);
  tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);
  tmr_counter_enable(TMR1, TRUE);
}

/**
  * @brief  获取系统滴答计数
  * @param  none
  * @retval 当前毫秒数
  */
uint32_t bsp_get_tick(void)
{
  return systick_ms;
}


/**
  * @brief  TMR1 溢出中断处理函数
  * @param  none
  * @retval none
  */
void TMR1_OVF_TMR10_IRQHandler(void)
{
  if(tmr_flag_get(TMR1, TMR_OVF_FLAG) != RESET)
  {
    systick_ms++;
    Master_Pulse_Counter += 4;
    tmr_flag_clear(TMR1, TMR_OVF_FLAG);
  }
}


/**
 * @brief  初始化 TMR8 为 3kHz 恒定脉冲（中断翻转 PA0）
 * @param  none
 * @retval none
 * @note   实现 PA0 输出精确 3kHz 方波（50% 占空比）
 */
void bsp_timer_step_main_init(void)
{
  gpio_init_type gpio_init_struct;
  
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins          = GPIO_PINS_0;
  gpio_init_struct.gpio_mode          = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type      = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull          = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);
  
  crm_periph_clock_enable(CRM_TMR8_PERIPH_CLOCK, TRUE);
  
  tmr_base_init(TMR8, 999, 29);          // 8kHz 溢出中断
  tmr_cnt_dir_set(TMR8, TMR_COUNT_UP);
  
  nvic_irq_enable(TMR8_OVF_TMR13_IRQn, 0, 1);   
  tmr_interrupt_enable(TMR8, TMR_OVF_INT, TRUE);
  
  tmr_counter_enable(TMR8, TRUE);
}

/**
 * @brief  TMR8 溢出中断服务函数（自动翻转 PA0）
 * @note   每 8kHz 中断一次翻转一次 → 精确 4kHz 方波
 */
void TMR8_OVF_TMR13_IRQHandler(void)
{
  if (tmr_flag_get(TMR8, TMR_OVF_FLAG) != RESET)
  {
    tmr_flag_clear(TMR8, TMR_OVF_FLAG);
    
    /* 翻转 PA0（实现方波） */
    gpio_bits_toggle(GPIOA, GPIO_PINS_0);
  }
}
