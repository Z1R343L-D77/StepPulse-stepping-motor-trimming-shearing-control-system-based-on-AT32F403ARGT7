/**
 * @file app_step_fifo.c
 * @author {Z1R343L} (Z1R343L@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "stdint.h"
#include "app_step_fifo.h"
#include "app_control.h"

/* 飞剪触发队列 */
#define STEP2_QUEUE_SIZE  8                        /* 队列大小 8 即任务最大值*/
uint32_t step2_trigger_queue[STEP2_QUEUE_SIZE];
volatile uint8_t step2_q_head = 0;
volatile uint8_t step2_q_tail = 0;
volatile uint8_t step2_q_count = 0;

/**
 * @brief  向飞剪触发队列中推送一个脉冲值
 * @param pulse 要推送的脉冲值
 * @return uint8_t 推送成功返回 1，否则返回 0
 */
uint8_t step2_queue_push(uint32_t pulse)
{
    if(step2_q_count >= STEP2_QUEUE_SIZE)
    {
        return 0;
    }
    step2_trigger_queue[step2_q_tail] = pulse;
    step2_q_tail++;
    if(step2_q_tail >= STEP2_QUEUE_SIZE)
    {
        step2_q_tail = 0;
    }
    step2_q_count++;
    return 1;
}

/**
 * @brief  从飞剪触发队列中弹出一个脉冲值 即返回金属触发当时的脉冲值
 * @param pulse 指向存储弹出脉冲值的变量的指针
 * @return uint8_t 弹出成功返回 1，否则返回 0
 */
uint8_t step2_queue_pop(uint32_t *pulse)
{
    if(step2_q_count == 0)
    {
        return 0;
    }

    *pulse = step2_trigger_queue[step2_q_head];
    step2_q_head++;
    if(step2_q_head >= STEP2_QUEUE_SIZE)
    {
        step2_q_head = 0;
    }
    step2_q_count--;
    return 1;
}

/**
 * @brief  查看飞剪触发队列的头元素
 * @param pulse 指向存储头元素脉冲值的变量的指针
 * @return uint8_t 查看成功返回 1，否则返回 0
 * @note 先看最早那一件是不是已经到了飞剪启动时机，没到就别把它拿出来。
 */
uint8_t step2_queue_peek(uint32_t *pulse)
{
    if(step2_q_count == 0)
    {
        return 0;
    }
    *pulse = step2_trigger_queue[step2_q_head];
    return 1;
}

/* 追剪触发 FIFO */
#define STEP3_QUEUE_SIZE  8

uint32_t step3_trigger_queue[STEP3_QUEUE_SIZE];
volatile uint8_t step3_q_head = 0;
volatile uint8_t step3_q_tail = 0;
volatile uint8_t step3_q_count = 0;

/* 当前正在处理的产品触发时刻 */
uint32_t step3_cur_trigger_pulse = 0;

/* 下一件产品触发时刻 */
uint32_t step3_next_trigger_pulse = 0;

uint8_t step3_queue_push(uint32_t pulse)
{
    if(step3_q_count >= STEP3_QUEUE_SIZE)
    {
        return 0;
    }

    step3_trigger_queue[step3_q_tail] = pulse;
    step3_q_tail++;
    if(step3_q_tail >= STEP3_QUEUE_SIZE)
    {
        step3_q_tail = 0;
    }
    step3_q_count++;
    return 1;
}

uint8_t step3_queue_peek(uint32_t *pulse)
{
    if(step3_q_count == 0)
    {
        return 0;
    }

    *pulse = step3_trigger_queue[step3_q_head];
    return 1;
}

uint8_t step3_queue_pop(uint32_t *pulse)
{
    if(step3_q_count == 0)
    {
        return 0;
    }

    *pulse = step3_trigger_queue[step3_q_head];
    step3_q_head++;
    if(step3_q_head >= STEP3_QUEUE_SIZE)
    {
        step3_q_head = 0;
    }
    step3_q_count--;
    return 1;
}


/*
 * @brief  初始化飞剪触发队列
 * @param  None
 * @retval None
 * @note   此函数用于初始化飞剪触发队列，将头、尾指针和计数器重置为 0
 */
void step_fifo_init(void)
{
    step2_q_head = 0;
    step2_q_tail = 0;
    step2_q_count = 0;

    step3_q_head = 0;
    step3_q_tail = 0;
    step3_q_count = 0;

    step3_cur_trigger_pulse = 0;
    step3_next_trigger_pulse = 0;
}

