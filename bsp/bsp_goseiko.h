/**
  ******************************************************************************
  * @file           : bsp_goseiko.h
  * @author         : 19816
  * @brief          : 电感式金属传感器BSP层头文件
  * @attention      : 硬件引脚配置+业务逻辑控制层
  * @date           : 2025/1/20
  ******************************************************************************
  */

#ifndef BSP_GOSEIKO_H
#define BSP_GOSEIKO_H

#include "stdint.h"
#include "drv_goseiko.h"

/* BSP层传感器控制结构体 */
typedef struct {
    Goseiko_ID_t id;                   // 传感器ID
    uint8_t last_state;                // 上次状态
    void (*on_metal_leave)(void);      // 金属离开回调函数
    void (*on_metal_enter)(void);      // 金属进入回调函数
} BSP_Goseiko_t;

void bsp_goseiko_init(void);
void bsp_goseiko_process(void);

#endif // BSP_GOSEIKO_H
