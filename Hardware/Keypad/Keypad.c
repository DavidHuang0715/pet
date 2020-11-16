#include "ht32.h"
#include "delay.h"
//R:in C:out
#define p8 GPIO_PIN_3//R0
#define p7 GPIO_PIN_1//R1
#define p6 GPIO_PIN_15//R2
#define p5 GPIO_PIN_7//R3
#define p4 GPIO_PIN_8//C0
#define p3 GPIO_PIN_9//C1
#define p2 GPIO_PIN_0//C2
#define p1 GPIO_PIN_3//C3
#define GP8 GPIO_PB
#define GP7 GPIO_PB
#define GP6 GPIO_PA
#define GP5 GPIO_PB
#define GP4 GPIO_PC
#define GP3 GPIO_PC
#define GP2 GPIO_PA
#define GP1 GPIO_PC
#define HT8 HT_GPIOB
#define HT7 HT_GPIOB
#define HT6 HT_GPIOA
#define HT5 HT_GPIOB
#define HT4 HT_GPIOC
#define HT3 HT_GPIOC
#define HT2 HT_GPIOA
#define HT1 HT_GPIOC

int kcode,bk=0;
bool s=TRUE;

void Keypad_init(void)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
	
	CKCUClock.Bit.PA		=	1;
	CKCUClock.Bit.PB		=	1;
	CKCUClock.Bit.PC		=	1;
  CKCUClock.Bit.AFIO	= 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	AFIO_GPxConfig(GP8, p8, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GP7, p7, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GP6, p6, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GP5, p5, AFIO_FUN_GPIO);
	
	GPIO_DirectionConfig(HT8, p8, GPIO_DIR_IN);
	GPIO_DirectionConfig(HT7, p7, GPIO_DIR_IN);
	GPIO_DirectionConfig(HT6, p6, GPIO_DIR_IN);
	GPIO_DirectionConfig(HT5, p5, GPIO_DIR_IN);
	
	GPIO_PullResistorConfig(HT8, p8, GPIO_PR_UP);
	GPIO_PullResistorConfig(HT7, p7, GPIO_PR_UP);
	GPIO_PullResistorConfig(HT6, p6, GPIO_PR_UP);
	GPIO_PullResistorConfig(HT5, p5, GPIO_PR_UP);
	
	GPIO_InputConfig(HT8, p8, ENABLE); 	
	GPIO_InputConfig(HT7, p7, ENABLE); 
	GPIO_InputConfig(HT6, p6, ENABLE); 
	GPIO_InputConfig(HT5, p5, ENABLE); 
	
	AFIO_GPxConfig(GP4, p4, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GP3, p3, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GP2, p2, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GP1, p1, AFIO_FUN_GPIO);
	
	GPIO_DirectionConfig(HT4, p4, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT3, p3, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT2, p2, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT1, p1, GPIO_DIR_OUT);
	
	GPIO_WriteOutBits(HT4,p4,SET);
	GPIO_WriteOutBits(HT4,p3,SET);
	GPIO_WriteOutBits(HT4,p2,SET);
	GPIO_WriteOutBits(HT4,p1,SET);
}

int scan()
{
		FlagStatus f=SET;
		f=GPIO_ReadInBit(HT8,p8);
		if(f==RESET){return 1;}
		f=GPIO_ReadInBit(HT7,p7);
		if(f==RESET){return 5;}
		f=GPIO_ReadInBit(HT6,p6);
		if(f==RESET){return 9;}
		f=GPIO_ReadInBit(HT5,p5);
		if(f==RESET){return 13;}
		return 0;
}

int scanKeys(void)
{
	int k=0;
	GPIO_WriteOutBits(HT4,p4,RESET);
	k=scan();
	GPIO_WriteOutBits(HT4,p4,SET);
	if(k!=0){return k;}
	GPIO_WriteOutBits(HT3,p3,RESET);
	k=scan();
	GPIO_WriteOutBits(HT3,p3,SET);
	if(k!=0){return k+1;}
	GPIO_WriteOutBits(HT2,p2,RESET);
	k=scan();
	GPIO_WriteOutBits(HT2,p2,SET);
	if(k!=0){return k+2;}
	GPIO_WriteOutBits(HT1,p1,RESET);
	k=scan();
	GPIO_WriteOutBits(HT1,p1,SET);
	if(k!=0){return k+3;}
	return 0;
}

int getKeys(void)
{
	kcode=scanKeys();
	if(kcode==0){s=TRUE;}
	if(s){
		if(kcode==bk){return 0;}
		else{
			s=FALSE;
			bk=kcode;
			return kcode;
		}
	}
	delay_ms(100);
	return 0;
}

void reKeys(void)
{
	s=TRUE;
	GPIO_WriteOutBits(HT4,p4,SET);
	GPIO_WriteOutBits(HT3,p3,SET);
	GPIO_WriteOutBits(HT2,p2,SET);
	GPIO_WriteOutBits(HT1,p1,SET);
	kcode=0;
}
