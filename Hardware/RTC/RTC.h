#ifndef _RTC_H
#define _RTC_H
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"

void RTC_INIT(void);
void RTC_Configuration(void);
u32 ScanUnsignedDecimal(void);
void RTC_IRQHandler(void);
char* TDL(u32 wTimeVar);
u32 Time_Set(u32 Tmp_HH,u32 Tmp_MM);

#endif
