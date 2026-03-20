/**
 * @file app_control.c
 * @author {Z1R343L} (Z1R343L@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "stdio.h"
#include "app_control.h"
#include "bsp_step.h"
#include "drv_step.h"
#include "app_step_fifo.h"

stepTypedef step2, step3;
step3_state_t step3_state = STEP3_IDLE;
step2_state_t step2_state = STEP2_IDLE;

volatile uint8_t step2_start;
volatile uint8_t step3_start;

/* 因为主轴是 3kHz，即每 1ms 走 3 个脉冲 */
uint32_t Master_Pulse_Counter = 0;
uint32_t s1_trigger_pulse_val = 0;
uint32_t step3_return_Counter = 0;
uint8_t  next_item_pending = 0;

/**
 * @brief  追剪系统初始化
 * @param  None
 * @retval None
 * @note   此函数用于初始化步进电机 3 和 2 的追轴系统，确保它们在 homing 状态下运行
 */
void Chase_System_Init(void)
{
    step3_state = STEP3_HOMING;
    step3_start = 0;

    step2_state = STEP2_HOMING_RESET;
    step2_start = 0;

    step_fifo_init();
}

/**
 * @brief 1号传感器触发中断服务函数
 * @note  金属传感器触发，启动追剪和飞剪
 */
void on_sensor1_trigger(void)
{
    if(!(step3_start && step2_start)) return;

    s1_trigger_pulse_val = Master_Pulse_Counter;

    if(step3_state == STEP3_IDLE)
    {
        Step_Init(&step3, TMR5, TMR_SELECT_CHANNEL_3, GPIOA, GPIO_PINS_3, MAIN_STEP_SPEED, MAIN_STEP_SPEED, 0);
        step_move_start_pwm(&step3, RESET_STEPS, DIR_LEFT, Decelerate_NOUSE);
        step3_state = STEP3_SYNC;
        next_item_pending = 0;
    }
    else if(step3_state == STEP3_RESET)
    {
        next_item_pending = 1;
        step3.state = Stop;
        step3_state = STEP3_CATCH;
        Step_Abort(&step3);
        Step_DMA_IRQHandler(&step3);
    }

    /* 飞剪连续触发入队 */
    step2_queue_push(Master_Pulse_Counter);
}

/**
 * @brief 2号传感器触发中断服务函数 
 * @param  None
 */
void on_sensor2_trigger(void)
{
    if(step3_state == STEP3_HOMING)
    {
        step3.state = Stop;
        Step_Abort(&step3);
        Step_DMA_IRQHandler(&step3);
    }
    else if(step3_state == STEP3_SYNC)
    {
        step3.state = Stop;
        Step_Abort(&step3);
        Step_DMA_IRQHandler(&step3);
        step3_return_Counter = Master_Pulse_Counter;
    }
}

/**
 * @brief 3号传感器触发中断服务函数 
 * @param  None
 */
void on_sensor3_trigger(void)
{
    if(step2_state == STEP2_HOMING_RESET)
    {
        step2.state = Stop;
        Step_Abort(&step2);
        Step_DMA_IRQHandler(&step2);
    }
    else if(step2_state == STEP2_SYNC)
    {
        step2.state = Stop;
        Step_Abort(&step2);
        Step_DMA_IRQHandler(&step2);
    }
}

/**
 * @brief  追剪 3 号步进电机 中断服务函数
 * @param  None
 * @retval None
 * @note   此函数在 DMA 脉冲发送结束时触发时调用，用于处理步进电机 3 的运动控制 
 */
void step3_move_isr(void)
{
    switch(step3_state)
    {
        case STEP3_IDLE:
        break;
        case STEP3_HOMING:
            Step_Init(&step3, TMR5, TMR_SELECT_CHANNEL_3, GPIOA, GPIO_PINS_3, STEP3_SPEED, STEP3_SPEED, 100);
            step_move_start_pwm(&step3, RESET_STEPS, DIR_RIGHT, Decelerate_USE);
            step3_state = STEP3_HOMING_RESET;
        break;
        case STEP3_HOMING_RESET:
            step3_start = 1;
            step3_state = STEP3_IDLE;
        break;
        case STEP3_CATCH:
            if(next_item_pending)
            {
                next_item_pending = 0;
                uint32_t master_offset = (Master_Pulse_Counter - s1_trigger_pulse_val);
                uint32_t slaver_offset = (uint32_t)((Master_Pulse_Counter - step3_return_Counter) * (STEP3_SPEED / (float)MAIN_STEP_SPEED));
                uint32_t return_dist = RESET_STEPS - master_offset - slaver_offset;
                return_dist = (uint32_t)((uint64_t)return_dist * STEP3_SPEED / (STEP3_SPEED + MAIN_STEP_SPEED));

                if(return_dist > RESET_STEPS)
                {
                    return_dist = RESET_STEPS;
                }

                Step_Init(&step3, TMR5, TMR_SELECT_CHANNEL_3, GPIOA, GPIO_PINS_3, STEP3_SPEED, STEP3_SPEED, 0);
                step_move_start_pwm(&step3, return_dist, DIR_RIGHT, Decelerate_NOUSE);
                step3_state = STEP3_CATCH;
            }
            else
            {
                Step_Init(&step3, TMR5, TMR_SELECT_CHANNEL_3, GPIOA, GPIO_PINS_3, MAIN_STEP_SPEED, MAIN_STEP_SPEED, 0);
                step_move_start_pwm(&step3, RESET_STEPS, DIR_LEFT, Decelerate_NOUSE);
                step3_state = STEP3_SYNC;
            }
        break;
        case STEP3_SYNC:
            Step_Init(&step3, TMR5, TMR_SELECT_CHANNEL_3, GPIOA, GPIO_PINS_3, STEP3_SPEED, STEP3_SPEED, 100);
            step_move_start_pwm(&step3, RESET_STEPS, DIR_RIGHT, Decelerate_USE);
            step3_state = STEP3_RESET;
        break;
        case STEP3_RESET:
            step3_state = STEP3_IDLE;
        break;
    }
}

/**
 * @brief  飞剪 2号 步进电机 中断服务函数
 * @param  None
 * @retval None
 * @note   此函数在 DMA 脉冲发送结束时触发时调用，用于处理步进电机 3 的运动控制 
 */
void step2_move_isr(void)
{
    switch(step2_state)
    {
        case STEP2_IDLE:
        break;
        case STEP2_HOMING_RESET:
            step2_start = 1;
            step2_state = STEP2_IDLE;
        break;
        case STEP2_CATCH:
            Step_Init(&step2, TMR2, TMR_SELECT_CHANNEL_2, GPIOB, GPIO_PINS_1, MAIN_STEP_SPEED, MAIN_STEP_SPEED, 0);
            step_move_start_pwm(&step2, 3500, DIR_RIGHT, Decelerate_USE);
            step2_state = STEP2_SYNC;
        break;
        case STEP2_SYNC:
            step2_start = 1;
            step2_state = STEP2_IDLE;
        break;
    }
}

/**
 * @brief  步进电机扫描任务函数
 * @param  None
 * @retval None
 * @note   此函数在主循环中调用，用于扫描步进电机的脉冲缓冲区
 */
void os_step_move_scan(void)
{
    if(step2.flag)
    {
        if(Step_IsBuffRdy(&step2))
        {
            tmr_pwm_start_dma(step2.tmr, step2.channel, (uint16_t *)Step_GetCurBuffer(&step2), Step_BuffUsedLength(&step2));
            Step_BufferUsed(&step2);
        }
        Step_BuffFill(&step2);
        step2.flag = 0;
    }

    if(step3.flag)
    {
        if(Step_IsBuffRdy(&step3))
        {
            tmr_pwm_start_dma(step3.tmr, step3.channel, (uint16_t *)Step_GetCurBuffer(&step3), Step_BuffUsedLength(&step3));
            Step_BufferUsed(&step3);
        }
        Step_BuffFill(&step3);
        step3.flag = 0;
    }
}

/**
 * @brief 启动步进电机 PWM 运行
 * @param hstep 步进电机控制句柄指针
 * @param stepToGo 要移动的步数
 * @param dir 方向    0: 正向  1: 反向
 * @param useDec 是否使用减速    0: 不使用  1: 使用
 */
void step_move_start_pwm(stepTypedef *hstep, uint32_t stepToGo, uint8_t dir, uint8_t useDec)
{
    Step_Prefill(hstep, stepToGo, dir, useDec);

    if(hstep == &step2)
    {
        step2.flag = 1;
    }
    else if(hstep == &step3)
    {
        step3.flag = 1;
    }
}

/**
 * @brief  步进电机控制任务函数
 * @param  None
 * @retval None
 * @note   此函数在主循环中调用，用于控制步进电机的运动
 */
void os_step_move_ctrl(void)
{
    /* 飞剪空闲时，检查队列头产品是否到了启动窗口 */
    if(step2_state == STEP2_IDLE)
    {
        uint32_t trig_pulse_val = 0;

        /* 查看队列头元素，获取最早入队的触发脉冲值 */
        if(step2_queue_peek(&trig_pulse_val))
        {
            /* 判断当前主脉冲计数与触发脉冲值的差值是否达到等待脉冲数 */
            if((uint32_t)(Master_Pulse_Counter - trig_pulse_val) >= WAIT_PULSE)
            {
                /* 到达启动时机，弹出该脉冲值并启动飞剪 */
                step2_queue_pop(&trig_pulse_val);
                Step_Init(&step2, TMR2, TMR_SELECT_CHANNEL_2, GPIOB, GPIO_PINS_1, STEP2_SPEED, STEP2_SPEED, 0);
                step_move_start_pwm(&step2, 3800, DIR_LEFT, Decelerate_USE);
                step2_state = STEP2_CATCH;
            }
        }
    }
}
