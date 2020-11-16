#include "ht32.h"
#define IN1 GPIO_PIN_7
#define IN2 GPIO_PIN_9
#define IN3 GPIO_PIN_3
#define IN4 GPIO_PIN_10
#define rIn GPIO_PIN_1

bool f=FALSE;
TM_TimeBaseInitTypeDef MCTM_TimeBaseInitStructure;
TM_OutputInitTypeDef MCTM_OutputInitStructure;
MCTM_CHBRKCTRInitTypeDef MCTM_CHBRKCTRInitStructure;

void motor_init(void)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
	CKCUClock.Bit.PA		=	1;
	CKCUClock.Bit.PC		=	1;
  CKCUClock.Bit.AFIO	= 1;
  CKCUClock.Bit.MCTM0	= 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Configure MCTM Channel 0 output pin 	*/
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_15, AFIO_FUN_MCTM_GPTM);//0:B15 to A1
	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_4, AFIO_FUN_MCTM_GPTM);//b:B4 to GND
	AFIO_GPxConfig(GPIO_PA, rIn, AFIO_FUN_GPIO); //A1 to B15
	GPIO_DirectionConfig(HT_GPIOA, rIn, GPIO_DIR_IN);
	GPIO_PullResistorConfig(HT_GPIOA, rIn, GPIO_PR_DISABLE);
	GPIO_InputConfig(HT_GPIOA, rIn, ENABLE);
	
	AFIO_GPxConfig(GPIO_PC, GPIO_PIN_14, AFIO_FUN_GPIO);
	GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_14, GPIO_DIR_OUT);
	//GPIO_WriteOutBits(HT_GPIOC,GPIO_PIN_14,RESET);
	
	AFIO_GPxConfig(GPIO_PA, IN1, AFIO_FUN_GPIO);
	GPIO_DirectionConfig(HT_GPIOA, IN1, GPIO_DIR_OUT);
	AFIO_GPxConfig(GPIO_PA, IN2, AFIO_FUN_GPIO);
	GPIO_DirectionConfig(HT_GPIOA, IN2, GPIO_DIR_OUT);
	AFIO_GPxConfig(GPIO_PA, IN3, AFIO_FUN_GPIO);
	GPIO_DirectionConfig(HT_GPIOA, IN3, GPIO_DIR_OUT);
	AFIO_GPxConfig(GPIO_PA, IN4, AFIO_FUN_GPIO);
	GPIO_DirectionConfig(HT_GPIOA, IN4, GPIO_DIR_OUT);
	
  /* MCTM Time Base configuration                                                                           */
  MCTM_TimeBaseInitStructure.CounterReload = (48000000/2000) - 1;
  MCTM_TimeBaseInitStructure.Prescaler = 20;//10ms
  MCTM_TimeBaseInitStructure.RepetitionCounter = 0;
  MCTM_TimeBaseInitStructure.CounterMode = TM_CNT_MODE_UP;
  MCTM_TimeBaseInitStructure.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;
  TM_TimeBaseInit(HT_MCTM0, &MCTM_TimeBaseInitStructure);
	
	//0
  MCTM_OutputInitStructure.Channel = TM_CH_0;
  MCTM_OutputInitStructure.OutputMode = TM_OM_PWM1;
  MCTM_OutputInitStructure.Control = TM_CHCTL_ENABLE;
  MCTM_OutputInitStructure.ControlN = TM_CHCTL_ENABLE;
  MCTM_OutputInitStructure.Polarity = TM_CHP_NONINVERTED;
  MCTM_OutputInitStructure.PolarityN = TM_CHP_NONINVERTED;
  MCTM_OutputInitStructure.IdleState = MCTM_OIS_LOW;
  MCTM_OutputInitStructure.IdleStateN = MCTM_OIS_HIGH;
  MCTM_OutputInitStructure.Compare = (48000000/2000) * 1/2;
  MCTM_OutputInitStructure.AsymmetricCompare = 0;
  TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
	
  //b
  MCTM_CHBRKCTRInitStructure.OSSRState = MCTM_OSSR_STATE_ENABLE;
  MCTM_CHBRKCTRInitStructure.OSSIState = MCTM_OSSI_STATE_ENABLE;
  MCTM_CHBRKCTRInitStructure.LockLevel = MCTM_LOCK_LEVEL_2;
  MCTM_CHBRKCTRInitStructure.Break0 = MCTM_BREAK_ENABLE;
  MCTM_CHBRKCTRInitStructure.Break0Polarity = MCTM_BREAK_POLARITY_HIGH;
  MCTM_CHBRKCTRInitStructure.AutomaticOutput = MCTM_CHAOE_ENABLE;
  MCTM_CHBRKCTRInitStructure.DeadTime = (72);
  MCTM_CHBRKCTRInitStructure.BreakFilter = 0;
  MCTM_CHBRKCTRConfig(HT_MCTM0, &MCTM_CHBRKCTRInitStructure);
  /* MCTM counter enable                                                                                    */
  TM_Cmd(HT_MCTM0, ENABLE);
  /* MCTM Channel Main Output enable                                                                        */
  MCTM_CHMOECmd(HT_MCTM0, ENABLE);
}
void motor_start(int ii)
{
	//GPIO_WriteOutBits(HT_GPIOC,GPIO_PIN_14,RESET);
	if(ii==0)
	{
		GPIO_WriteOutBits(HT_GPIOA,IN4,RESET);
		GPIO_WriteOutBits(HT_GPIOA,IN1,SET);
	}
	else if(ii==1)
	{
		GPIO_WriteOutBits(HT_GPIOA,IN1,RESET);
		GPIO_WriteOutBits(HT_GPIOA,IN2,SET);
	}
	else if(ii==2)
	{
		GPIO_WriteOutBits(HT_GPIOA,IN2,RESET);
		GPIO_WriteOutBits(HT_GPIOA,IN3,SET);
	}
	else if(ii==3)
	{
		GPIO_WriteOutBits(HT_GPIOA,IN3,RESET);
		GPIO_WriteOutBits(HT_GPIOA,IN4,SET);
	}
}
void motor_end(void)
{
	GPIO_WriteOutBits(HT_GPIOA,IN1,RESET);
	GPIO_WriteOutBits(HT_GPIOA,IN2,RESET);
	GPIO_WriteOutBits(HT_GPIOA,IN3,RESET);
	GPIO_WriteOutBits(HT_GPIOA,IN4,RESET);
}
