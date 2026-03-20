/**
 * @file bsp_goseiko.c
 * @author {Z1R343L} (Z1R343L@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "stdio.h"
#include "bsp_goseiko.h"
#include "at32f403a_407_crm.h"
#include "app_control.h"
#include "drv_gpio.h"

/* 回调函数声明 */
static inline void x1_metal_falling_handler(void);   /*< X1 下降沿（有金属 → 无金属） */
static inline void x2_metal_rising_handler(void);    /*< X2 上升沿（无金属 → 有金属） */
static inline void x3_metal_falling_handler(void);   /*< X3 下降沿（有金属 → 无金属） */

/* 传感器配置表 */
static BSP_Goseiko_t goseiko_list[] = {
    {
        .id             = GOSEIKO_X1,
        .last_state     = 0U,
        .on_metal_leave = x1_metal_falling_handler,   /*< X1 只用下降沿 */
        .on_metal_enter = NULL
    },
    {
        .id             = GOSEIKO_X2,
        .last_state     = 1U,
        .on_metal_leave = NULL,
        .on_metal_enter = x2_metal_rising_handler     /*< X2 只用上升沿（追轴到位） */
    },
    {
        .id             = GOSEIKO_X3,
        .last_state     = 0U,
        .on_metal_leave = x3_metal_falling_handler,
        .on_metal_enter = NULL     					  /*< X3 只用上升沿（追轴到位） */
    }
};

#define GOSEIKO_COUNT  (sizeof(goseiko_list) / sizeof(goseiko_list[0]))

/**
 * @brief  X1 下降沿处理（主轴金属通过）
 */
static inline void x1_metal_falling_handler(void)
{
    led1_setup(100U, 0.5F, 1U);
    on_sensor1_trigger();          /*< 启动追赶 */
}

/**
 * @brief  X2 上升沿处理（追轴金属到位）
 */
static inline void x2_metal_rising_handler(void)
{
    on_sensor2_trigger();          /*< 停止同步 + 复位 */   
}
/**
 * @brief  X3 下降沿处理（飞轴金属到位）
 */
static inline void x3_metal_falling_handler(void)
{
	on_sensor3_trigger();
}

/**
 * @brief  单个传感器边沿检测（同时支持上升沿 + 下降沿）
 * @note   DRV 层已消抖，这里只判断边沿
 */
static void goseiko_edge_detect(BSP_Goseiko_t* p_goseiko)
{
    if (p_goseiko == NULL) return;

    uint8_t current_state = drv_goseiko_get_state(p_goseiko->id);

    /* 下降沿（1→0） */
    if ((p_goseiko->last_state == 1U) && (current_state == 0U)) {
        if (p_goseiko->on_metal_leave != NULL) {
            p_goseiko->on_metal_leave();
        }
    }

    /* 上升沿（0→1） */
    if ((p_goseiko->last_state == 0U) && (current_state == 1U)) {
        if (p_goseiko->on_metal_enter != NULL) {
            p_goseiko->on_metal_enter();
        }
    }
    p_goseiko->last_state = current_state;
}

/**
 * @brief  传感器业务逻辑处理（2ms 调用一次）
 */
void bsp_goseiko_process(void)
{
    uint8_t i;

    task_goseiko_scan_proc();          /*< DRV 消抖 */  

    for (i = 0U; i < GOSEIKO_COUNT; i++) {
        goseiko_edge_detect(&goseiko_list[i]);
    }
}

/**
 * @brief  传感器硬件初始化
 */
void bsp_goseiko_init(void) 
{ 
    gpio_init_type gpio_init_struct;
    uint8_t i;

    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);

    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_pins = GPIO_PINS_6 | GPIO_PINS_11;
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_pins = GPIO_PINS_9;
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOC, &gpio_init_struct);

    drv_goseiko_init();

    for (i = 0U; i < GOSEIKO_COUNT; i++) {
        goseiko_list[i].last_state = 0U;
    }
}
