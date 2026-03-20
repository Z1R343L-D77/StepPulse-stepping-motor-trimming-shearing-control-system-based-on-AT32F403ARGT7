/**
 * @file system_it.h
 * @author {Z1R343L} (Z1R343L@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-09
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef __SYSTEM_IT_H__
#define __SYSTEM_IT_H__

#include "stdint.h"

#define SYSTEM_TICK systick_ms


#include "at32f403a_407.h"

extern volatile uint32_t systick_ms;

void bsp_timer_init(void);
uint32_t bsp_get_tick(void);


void bsp_timer_step_main_init(void);



#endif
