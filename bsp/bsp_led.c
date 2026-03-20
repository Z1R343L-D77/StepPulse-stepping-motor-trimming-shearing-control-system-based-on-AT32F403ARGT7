/**
 * @file bsp_led.c
 * @author {Z1R343L} (Z1R343L@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-09
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "drv_gpio.h"
#include "bsp_led.h"
#include "at32f403a_407_gpio.h"


/**
  * @brief  点亮 LED
  * @param  none
  * @retval none
  */
//static void led_on(void)
//{
//	gpio_bits_reset(GPIOB, GPIO_PINS_3);
//}

/**
  * @brief  熄灭 LED
  * @param  none
  * @retval none
  */
static void led_off(void)
{
  gpio_bits_set(GPIOB, GPIO_PINS_3);
}

/**
  * @brief  翻转 LED 状态
  * @param  none
  * @retval none
  */
// static void led_toggle(void)
// {
//  gpio_bits_toggle(GPIOB, GPIO_PINS_3);
// }

/**
  * @brief  初始化 LED (PB3)
  * @param  none
  * @retval none
  */
void bsp_led_init(void)
{
  gpio_init_type gpio_init_struct;
  
  /* 使能 GPIOB 和 IOMUX 时钟 */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
  
  /* 禁用 JTAG，释放 PB3 (JTDO) */
  gpio_pin_remap_config(SWJTAG_GMUX_010, TRUE);
  
  /* 配置 PB3 为推挽输出 */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_PINS_3;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);
  
  /* 初始状态：LED 熄灭（高电平）*/
  led_off();
  
  /* 调用DRV层初始化 */
  drv_gpio_init();
}











