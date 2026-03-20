/**
  ******************************************************************************
  * @file           : drv_gpio.h
  * @author         : 19816
  * @brief          : GPIO驱动（LED控制）头文件
  * @attention      : 符合MISRA-C 2012规范，适配AT32F402
  * @date           : 2025/1/20
  ******************************************************************************
  */

#ifndef DRV_GPIO_H
#define DRV_GPIO_H

/* 头文件包含 - 仅包含必要头文件，避免冗余 */
#include "stdint.h"
#include "at32f403a_407_gpio.h"
/* 宏必须全大写，参数加括号，避免优先级问题 */
#define GPIO_TYPEDEF      gpio_type  /* 适配AT32 GPIO类型别名 */
#define GPIO_ON(_LIGHT)   do { gpio_bits_write((_LIGHT)->port, (_LIGHT)->pin, TRUE); } while(0)
#define GPIO_OFF(_LIGHT)  do { gpio_bits_write((_LIGHT)->port, (_LIGHT)->pin, FALSE); } while(0)

/* 枚举必须typedef，显式指定类型，常量全大写 */
typedef uint8_t GPIO_Level_t;           /* GPIO电平类型 */  
#define GPIO_LOW    ((GPIO_Level_t)0U)  /* 低电平（无符号后缀U） */
#define GPIO_HIGH   ((GPIO_Level_t)1U)  /* 高电平 */

/* 结构体禁止下划线开头，合理对齐，字段命名清晰 */
typedef struct {
    GPIO_TYPEDEF* port;               /* GPIO端口（如GPIOF） */
    uint32_t pin;                     /* GPIO引脚（如GPIO_PINS_4） */
    uint32_t period;                  /* 闪烁周期（5ms为单位） */
    uint16_t total_times;             /* 预设闪烁总次数（替换原times，语义更清晰） */
    uint16_t cycle_cnt;               /* 单周期计数器（替换原cnt） */
    uint16_t completed_times;         /* 已完成闪烁次数（替换原times_cnt） */
    uint8_t reset_flag;               /* 复位标志（1=复位，0=正常） */
    uint8_t finish_flag;              /* 完成标志（1=结束，0=运行） */
    float duty_cycle;                 /* 占空比（点亮时间百分比，替换原light_on_percent） */
    GPIO_Level_t active_level;        /* 点亮有效电平（GPIO_LOW/GPIO_HIGH） */
} GPIO_Control_t;  /* 规范命名：前缀+功能+_t */

/* 函数原型必须完整，参数/返回值类型显式 */
void drv_gpio_init(void);  /* 初始化GPIO（LED控制结构体） */ 
void led1_setup(uint32_t period, float duty_cycle, uint16_t total_times);
void task_led_proc(void);

#endif // DRV_GPIO_H
