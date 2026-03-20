/**
 * @file drv_gpio.c
 * @author {Z1R343L} (Z1R343L@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "drv_gpio.h"
#include "string.h"

/* 全局变量加static限制作用域，禁止裸全局暴露 */
static GPIO_Control_t led1;\

/**
 * @brief  GPIO初始化（LED控制结构体初始化）
 * @note   消除魔法数字，字段语义清晰
 * @retval None
 */
void drv_gpio_init(void)
{
    /* LED1初始化 - 保留原有参数，字段名规范化 */
    led1.port = GPIOB;
    led1.pin = GPIO_PINS_3;
    led1.active_level = GPIO_LOW;
    led1.period = 20U;                /* 周期20ms，占空比50% */
    led1.duty_cycle = 0.5f;
    led1.total_times = 5U;
    led1.reset_flag = 1U;
}

/**
 * @brief  GPIO控制状态机（5ms调用一次）
 * @note   空指针检查、消除语法错误、逻辑无歧义
 * @param  p_ctrl: GPIO控制结构体指针（MISRA：指针参数加p_前缀）
 * @retval None
 */
static void gpio_control_work(GPIO_Control_t *p_ctrl)
{
    /* 空指针检查 */
    if (p_ctrl == NULL) 
    {
        return;
    }

    /* 复位逻辑 */
    if (p_ctrl->reset_flag == 1U) 
    {
        p_ctrl->reset_flag = 0U;
        p_ctrl->cycle_cnt = 0U;
        p_ctrl->completed_times = 0U;
        p_ctrl->finish_flag = 0U;
    }

    /* 闪烁完成控制 */
    if (p_ctrl->completed_times >= p_ctrl->total_times) 
    {
        p_ctrl->finish_flag = 1U;
        /* MISRA：补充分号，逻辑对齐，消除注释混乱 */
        if (p_ctrl->active_level == GPIO_HIGH) 
        {
            GPIO_OFF(p_ctrl);  /* 高电平点亮 → 完成后关闭 */
        } else 
        {
            GPIO_ON(p_ctrl);   /* 低电平点亮 → 完成后关闭 */
        }
        return;
    }

    /* 周期计数（5ms递增） */
    p_ctrl->cycle_cnt++;

    /* 点亮区间判断 - MISRA：括号包裹表达式，避免优先级问题 */
    if (p_ctrl->cycle_cnt <= (uint16_t)(p_ctrl->period * p_ctrl->duty_cycle)) 
    {
        if (p_ctrl->active_level == GPIO_HIGH) 
        {
            GPIO_ON(p_ctrl);   /* 高电平点亮 */
        } else {
            GPIO_OFF(p_ctrl);  /* 低电平点亮 */
        }
    }
    /* 熄灭区间判断 */
    else if (p_ctrl->cycle_cnt < p_ctrl->period) 
    {
        if (p_ctrl->active_level == GPIO_HIGH) 
        {
            GPIO_OFF(p_ctrl);  /* 高电平点亮 → 熄灭 */
        } else 
        {
            GPIO_ON(p_ctrl);   /* 低电平点亮 → 熄灭 */  
        }
    }
    /* 单周期完成，重置计数 */
    else 
    {
        p_ctrl->cycle_cnt = 0U;
        p_ctrl->completed_times++;
    }
}

/**
 * @brief  LED1参数动态配置
 * @note   参数命名语义化，无隐式转换
 * @param  period: 闪烁周期（5ms单位）
 * @param  duty_cycle: 占空比（0~1）
 * @param  total_times: 闪烁总次数
 * @retval None
 */
void led1_setup(uint32_t period, float duty_cycle, uint16_t total_times)
{
    led1.period = period;
    led1.duty_cycle = duty_cycle;
    led1.total_times = total_times;
    led1.reset_flag = 1U;
}


/**
 * @brief  LED任务处理函数（5ms调用）
 * @note   None
 * @param  None
 * @retval None
 */
void task_led_proc(void)
{
    gpio_control_work(&led1);
}

