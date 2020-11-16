#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "stdio.h"

u32 ScanUnsignedDecimal(void);

void RTC_INIT(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.BKP = 1;
	CKCUClock.Bit.USART0=1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  #if (!LIBCFG_NO_PWRCU_TEST_REG)
    if (PWRCU_CheckReadyAccessed() != PWRCU_OK)
    {
      while (1);
    }
  #endif
  HT32F_DVB_LEDInit(HT_LED1);
  RETARGET_Configuration();
  NVIC_EnableIRQ(RTC_IRQn);
}
void RTC_Configuration(void)
{
  /* Reset Backup Domain                                                                                    */
  PWRCU_DeInit();
  /* Configure Low Speed External clock (LSE)                                                               */
  RTC_LSESMConfig(RTC_LSESM_FAST);
  RTC_LSECmd(ENABLE);
  while (CKCU_GetClockReadyStatus(CKCU_FLAG_LSERDY) == RESET);
  /* Configure the RTC                                                                                      */
  RTC_ClockSourceConfig(RTC_SRC_LSE);
  RTC_IntConfig(RTC_INT_CSEC, ENABLE);
  RTC_SetPrescaler(RTC_RPRE_32768);
}

u32 ScanUnsignedDecimal(void)
{
  u8 bChar = 0;
  u32 wNbr = 0;
  while (1)
  {
    bChar = getchar();
		//printf("%u",bChar);
    if ((bChar >= '0') && (bChar <= '9'))
    {
      wNbr = (wNbr * 10) + (bChar - '0');
		//printf("%u",wNbr);
    }
    else
    {
      break;
    }

  }
  return wNbr;
}

void RTC_IRQHandler(void)
{
  extern vu32 gwTimeDisplay;
  u8 bFlags;

  bFlags = RTC_GetFlagStatus();
  if (bFlags & 0x1)
  {
    /* Enable time update */
    gwTimeDisplay = 1;
  }

  /* Toggle LED1 each 1s */
  HT32F_DVB_LEDToggle(HT_LED1);
}

char* TDL(u32 wTimeVar)
{
	char sh[2],sm[2],ss[2],*S;
  u32 THH = 0, TMM = 0, TSS = 0;
  wTimeVar %= 86400;
  THH = wTimeVar/3600;
  TMM = (wTimeVar % 3600)/60;
  TSS = (wTimeVar % 3600)%60;
	sprintf(sh, "%02u", (unsigned int)THH);
	sprintf(sm, "%02u", (unsigned int)TMM);
	sprintf(ss, "%02u", (unsigned int)TSS);
	sprintf(S, "%s:%s:%s",sh,sm,ss);
	return S;
}

u32 Time_Set(u32 Tmp_HH,u32 Tmp_MM)
{
  u32 Tmp;
	Tmp=Tmp_HH * 3600 + Tmp_MM * 60;
	return Tmp;
}
