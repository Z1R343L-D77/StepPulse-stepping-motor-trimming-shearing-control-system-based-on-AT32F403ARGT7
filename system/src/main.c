/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *
  * Copyright (c) 2025, Artery Technology, All rights reserved.
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  * 
  * author: Z1R343L
  **************************************************************************
  */
#include "at32f403a_407_clock.h"
#include "at32_system_delay.h"

#include "drv_gpio.h"
#include "drv_goseiko.h"
#include "drv_step.h"

#include "bsp_time.h"
#include "bsp_led.h"
#include "bsp_goseiko.h"
#include "bsp_step.h"
#include "bsp_mPulse.h"

#include "app_scheduler.h"
#include "app_control.h"

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();
  bsp_led_init();
  bsp_goseiko_init();
  bsp_mPulse_init();
  //***** step start*******//
  bsp_step_init();
  Step_Init(&step2,TMR2,TMR_SELECT_CHANNEL_2,GPIOB,GPIO_PINS_1,500,8000,500);
  Step_Init(&step3,TMR5,TMR_SELECT_CHANNEL_3,GPIOA,GPIO_PINS_3,500,8000,500);
  /* 一号传感器到二号传感器间距是7050个脉冲 */
  step_move_start_pwm(&step2, 6400,DIR_LEFT,Decelerate_USE);
  step_move_start_pwm(&step3, 32000,DIR_LEFT,Decelerate_USE);
  bsp_timer_step_main_init();/* 主轴4khz */
  Chase_System_Init();
  //***** step end*******//
  bsp_timer_init();
  scheduler_init();
  while(1)
  {
    scheduler_run();
    os_step_move_scan();
    os_step_move_ctrl();
  }
}
