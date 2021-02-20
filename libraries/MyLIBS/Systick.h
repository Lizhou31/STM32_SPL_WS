#ifndef __SYSTICK_H
#define __SYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f10x.h>

#define MAX_DELAY  0xFFFFFFFFU /* 32-bit */

void SysTickInit(void);
void IncTicks(void);
uint64_t GetTicks(void);

#ifdef __cplusplus
}
#endif

#endif