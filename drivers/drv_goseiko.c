/**
 * @file drv_goseiko.c
 * @author {Z1R343L} (Z1R343L@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "stdio.h" 
#include "drv_goseiko.h"

/* 全局变量加static限制作用域，禁止裸全局暴露 */
static Goseiko_Control_t goseiko_x1;
static Goseiko_Control_t goseiko_x2;
static Goseiko_Control_t goseiko_x3;

/* 消抖阈值：连续3次相同状态才确认（3*5ms=15ms） */
#define DEBOUNCE_THRESHOLD  3U

/**
 * @brief  传感器初始化
 * @note   消除魔法数字，字段语义清晰
 * @retval None
 */
void drv_goseiko_init(void)
{   
    /* X1传感器初始化 */
    goseiko_x1.port = GPIOA;
    goseiko_x1.pin = GPIO_PINS_6;
    goseiko_x1.current_state = GOSEIKO_NO_METAL;
    goseiko_x1.last_raw_state = GOSEIKO_NO_METAL;
    goseiko_x1.debounce_cnt = 0U;
    goseiko_x1.debounce_threshold = DEBOUNCE_THRESHOLD;
    
    /* X2传感器初始化 */
    goseiko_x2.port = GPIOC;
    goseiko_x2.pin = GPIO_PINS_9;
    goseiko_x2.current_state = GOSEIKO_NO_METAL;
    goseiko_x2.last_raw_state = GOSEIKO_NO_METAL;
    goseiko_x2.debounce_cnt = 0U;
    goseiko_x2.debounce_threshold = DEBOUNCE_THRESHOLD;
    
    /* X3传感器初始化 */
    goseiko_x3.port = GPIOA;
    goseiko_x3.pin = GPIO_PINS_11;
    goseiko_x3.current_state = GOSEIKO_NO_METAL;
    goseiko_x3.last_raw_state = GOSEIKO_NO_METAL;
    goseiko_x3.debounce_cnt = 0U;
    goseiko_x3.debounce_threshold = DEBOUNCE_THRESHOLD;
}

/**
 * @brief  读取传感器原始状态（无消抖）
 * @note   PNP型传感器：低电平=检测到金属，高电平=无金属
 * @param  goseiko_id: 传感器ID（GOSEIKO_X1/X2/X3）
 * @retval Goseiko_State_t: GOSEIKO_NO_METAL或GOSEIKO_DETECTED
 */
Goseiko_State_t drv_goseiko_read_raw(Goseiko_ID_t goseiko_id)
{
    Goseiko_Control_t* p_goseiko = NULL;
    
    /* 根据ID选择传感器 */
    switch(goseiko_id) {
        case GOSEIKO_X1:
            p_goseiko = &goseiko_x1;
            break;
        case GOSEIKO_X2:
            p_goseiko = &goseiko_x2;
            break;
        case GOSEIKO_X3:
            p_goseiko = &goseiko_x3;
            break;
        default:
            return GOSEIKO_NO_METAL;
    }
    
    /* 空指针检查 */
    if (p_goseiko == NULL) {
        return GOSEIKO_NO_METAL;
    }
    
    /* PNP型：低电平(RESET)=检测到金属 */
    if (GOSEIKO_READ(p_goseiko) == RESET) {
        return GOSEIKO_DETECTED;
    }
    return GOSEIKO_NO_METAL;
}

/**
 * @brief  传感器状态机（带消抖）
 * @note   空指针检查、消除语法错误、逻辑无歧义
 * @param  p_goseiko: 传感器控制结构体指针
 * @param  goseiko_id: 传感器ID
 * @retval None
 */
static void goseiko_control_work(Goseiko_Control_t *p_goseiko, Goseiko_ID_t goseiko_id)
{
    Goseiko_State_t raw_state;
    
    /* 空指针检查（必加） */
    if (p_goseiko == NULL) {
        return;
    }
    
    /* 读取原始状态 */
    raw_state = drv_goseiko_read_raw(goseiko_id);
    
    /* 状态变化检测 */
    if (raw_state != p_goseiko->last_raw_state) {
        /* 状态发生变化，重置计数器 */
        p_goseiko->debounce_cnt = 1U;
        p_goseiko->last_raw_state = raw_state;
    } else {
        /* 状态保持不变，递增计数器 */
        if (p_goseiko->debounce_cnt < p_goseiko->debounce_threshold) {
            p_goseiko->debounce_cnt++;
        }
        
        /* 达到消抖阈值，更新稳定状态 */
        if (p_goseiko->debounce_cnt >= p_goseiko->debounce_threshold) {
            p_goseiko->current_state = raw_state;
        }
    }
}

/**
 * @brief  获取传感器稳定状态
 * @note   返回经过消抖处理的稳定状态
 * @param  goseiko_id: 传感器ID（GOSEIKO_X1/X2/X3）
 * @retval uint8_t: 0=无金属，1=检测到金属
 */
uint8_t drv_goseiko_get_state(Goseiko_ID_t goseiko_id)
{
    switch(goseiko_id) {
        case GOSEIKO_X1:
            return goseiko_x1.current_state;
        case GOSEIKO_X2:
            return goseiko_x2.current_state;
        case GOSEIKO_X3:
            return goseiko_x3.current_state;
        default:
            return GOSEIKO_NO_METAL;
    }
}

/**
 * @brief  传感器任务处理函数（2ms调用）
 * @note   函数体简洁，仅调用核心逻辑
 * @retval None
 */
void task_goseiko_scan_proc(void)
{
    goseiko_control_work(&goseiko_x1, GOSEIKO_X1);
    goseiko_control_work(&goseiko_x2, GOSEIKO_X2);
    goseiko_control_work(&goseiko_x3, GOSEIKO_X3);
}

