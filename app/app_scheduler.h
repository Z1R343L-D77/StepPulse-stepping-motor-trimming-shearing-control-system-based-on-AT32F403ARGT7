/**
  ******************************************************************************
  * @file           : scheduler.h
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/20
  ******************************************************************************
  */

#ifndef INC_002_G_SCHEDULER_H
#define INC_002_G_SCHEDULER_H

#include "stdint.h"

/* 以下为文件改动部分，更具实际芯片文件GPIO包改动 */ 
#include "at32f403a_407_int.h"

extern uint32_t now_time;
void scheduler_init(void);
void scheduler_run(void);


#endif //INC_002_G_SCHEDULER_H
