/**
  ******************************************************************************
  * @file           : drv_goseiko.h
  * @author         : 19816
  * @brief          : 电感式金属传感器驱动头文件
  * @attention      : 符合MISRA-C 2012规范，适配AT32F403A
  * @date           : 2025/1/20
  ******************************************************************************
  */

#ifndef DRV_GOSEIKO_H
#define DRV_GOSEIKO_H

/* 头文件包含 - 仅包含必要头文件，避免冗余 */
#include "stdint.h"
#include "at32f403a_407_gpio.h"

/* 宏必须全大写，参数加括号，避免优先级问题 */
#define GPIO_TYPEDEF      gpio_type
#define GOSEIKO_READ(_S)  gpio_input_data_bit_read((_S)->port, (_S)->pin)

/* 枚举必须typedef，显式指定类型，常量全大写 */
typedef uint8_t Goseiko_State_t;
#define GOSEIKO_NO_METAL   ((Goseiko_State_t)0U)  /* 无金属 */
#define GOSEIKO_DETECTED   ((Goseiko_State_t)1U)  /* 检测到金属 */

/* 枚举必须typedef，显式指定类型，常量全大写 */
typedef uint8_t Goseiko_ID_t;
#define GOSEIKO_X1         ((Goseiko_ID_t)0U)
#define GOSEIKO_X2         ((Goseiko_ID_t)1U)
#define GOSEIKO_X3         ((Goseiko_ID_t)2U)
#define GOSEIKO_MAX        ((Goseiko_ID_t)3U)

/* 结构体禁止下划线开头，合理对齐，字段命名清晰 */
typedef struct {
    GPIO_TYPEDEF* port;               /* GPIO端口（如GPIOA） */
    uint32_t pin;                     /* GPIO引脚（如GPIO_PINS_4） */
    uint8_t current_state;            /* 当前稳定状态（0=无金属，1=有金属） */
    uint8_t last_raw_state;           /* 上次原始读取状态 */
    uint8_t debounce_cnt;             /* 消抖计数器 */
    uint8_t debounce_threshold;       /* 消抖阈值（连续相同次数） */  
} Goseiko_Control_t;

/* 函数原型必须完整，参数/返回值类型显式 */
void drv_goseiko_init(void);
Goseiko_State_t drv_goseiko_read_raw(Goseiko_ID_t goseiko_id);
uint8_t drv_goseiko_get_state(Goseiko_ID_t goseiko_id);
void task_goseiko_scan_proc(void);

#endif // DRV_GOSEIKO_H

