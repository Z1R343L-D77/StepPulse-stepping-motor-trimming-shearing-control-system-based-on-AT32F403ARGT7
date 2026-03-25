/**
 * @file app_control.h
 * @author {Z1R343L} (Z1R343L@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef __APP_CONTROL_H__
#define __APP_CONTROL_H__

#include "stdint.h"
#include "drv_step.h"
/* =============== 参数定义（可直接修改） ====================== */
#define MAIN_STEP_SPEED (4000)     /* 主轴速度,修改需要修改bsp_time.c的主轴计数器 */
#define WAIT_PULSE      (13000)    /* 飞剪触发等待脉冲 */
#define STEP3_SPEED			(20000)	
#define STEP2_SPEED			(15000)	
/* =============== 以下不可修改 ====================== */
#define SYNC_STEPS      (7000)     /* 同步跟随步数 */
#define RESET_STEPS     (7000)     /* 复位步数 */
#define DIR_RIGHT       (1U)
#define DIR_LEFT        (0U)

// 追轴状态机
typedef enum {
  STEP3_IDLE = 0,
  STEP3_HOMING,        /* 上电初始化一直正向转 */
  STEP3_HOMING_RESET,  /* homing 后返回初始位置 */
  STEP3_CATCH,
  STEP3_SYNC,
  STEP3_RESET         
} step3_state_t;

// 飞轴状态机
typedef enum {
  STEP2_IDLE = 0,
  STEP2_HOMING_RESET,  /* 上电 homing 后返回初始位置 */
  STEP2_CATCH,         /* 加速追 */
  STEP2_SYNC,          /* 同步飞剪段 */ 
} step2_state_t;

extern volatile uint8_t step3_start;

extern uint32_t Master_Pulse_Counter; 
extern uint32_t s1_trigger_pulse_val;
extern uint8_t  next_item_pending;

// ====================== 对外接口 ======================
void Chase_System_Init(void);          /* 在 main 的 step 初始化后调用 */
void on_sensor1_trigger(void);         /* 1号传感器（主轴金属件通过）中断里调用 */
void on_sensor2_trigger(void);         /* 2号传感器（追轴到位）中断里调用 */
void on_sensor3_trigger(void);				 /* 3号传感器（飞轴到位）中断里调用 */

void step3_move_isr(void);
void step2_move_isr(void);

void os_step_move_scan(void);
void os_step_move_ctrl(void);

void step_move_start_pwm(stepTypedef *hstep, uint32_t stepToGo, uint8_t dir, uint8_t useDec); 

#endif
