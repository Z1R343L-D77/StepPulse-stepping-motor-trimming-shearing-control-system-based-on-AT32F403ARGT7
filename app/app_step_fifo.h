/**
 * @file app_step_fifo.h
 * @author {Z1R343L} (Z1R343L@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef __APP_STEP_FIFO_H__
#define __APP_STEP_FIFO_H__ 

void step_fifo_init(void);

uint8_t step2_queue_peek(uint32_t *pulse);
uint8_t step2_queue_pop(uint32_t *pulse);
uint8_t step2_queue_push(uint32_t pulse);

#endif
