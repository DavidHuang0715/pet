#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "String.h"
#include "LCDx.h"
#include "HX711.h"
#include "delay.h"
#include "ASC.c"
#include "RTC.h"
#include "Keypad.h"
#include "motor.h"
#include "stdio.h"

#define DS18B20_NUM		4
#define S1 "1:Set time"
#define S2 "2:Set interval"
#define S3 "3:Set feed"
#define StartAddress    (1024 * 4)
#define EndAddress      (1024 * 8)
#define TEST_DATA       (250)

float x=0,fg=500,xx=0;
u8 num=0,ii=0,rslt;
bool ba=TRUE;
int yearz,monthz,dayz,hr,min,ihr,imin;
char chi[16] = "00:00";
char ch0[16] = "";
char ch1[16] = S1;
char ch2[16] = S2;
char ch3[16] = S3;
vu32 gwTimeDisplay = 0;
const float scale_factor = 9*189/4;
u8 array[DS18B20_NUM][8];
s16 DS18B20_ReadDesignateTemper(u8 pID[8]);

void FLASH_Erase(void);
void FLASH_Program(u32 uData);
void FLASH_Check(void);
	
int main(void)
{
	int keyi[17] = {-7,1,2,3,-1,4,5,6,-2,7,8,9,-3,-5,0,-6,-4};
	int key0,ikey,fi;
	int ch=6,chh,fgg,ii,jj;
	u32 wT=0;
	int st=0;
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
	CKCUClock.Bit.PA	=	1;
  CKCUClock.Bit.AFIO	= 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
	AFIO_GPxConfig(GPIO_PA, GPIO_PIN_2, AFIO_FUN_GPIO);
	GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_2, GPIO_DIR_OUT);
	yearz=0;monthz=0;dayz=0;key0=0;ikey=0;hr=0;min=0;ihr=0;imin=0;
	chh=ch;fgg=(int)fg;
	Keypad_init();
	reKeys();
	LCD_INIT();
  RTC_INIT();
	RTC_Configuration();
  RTC_Cmd(ENABLE);
	SetSpeed(6);
	rslt = OWFirst();
	num=1;
	for(ii=0;ii<8;ii++)
	{
		array[0][ii]=reR(ii);
	}
	while (rslt)
	{
    rslt = OWNext();
		for(ii=0;ii<8;ii++)
		{
			array[num][ii]=reR(ii);
		}		 
		num++;
	}
	num=num-1;
	Init_LCD();
	LCD_ClearScreen();
	RETARGET_Configuration();
	HX_begin();
	power_down();  
	delay_ms(1000);
	power_up();
	set_scale(scale_factor);
	tare();
	get_units(2);
	power_down();
	motor_init();
	while(1)
	{
		rslt = OWFirst();
		num=1;
		for(ii=0;ii<8;ii++)
		{
			array[0][ii]=reR(ii);
		}
		while(rslt)
		{
			rslt = OWNext();
			for(ii=0;ii<8;ii++)
			{
				array[num][ii]=reR(ii);
			}	 
			num++;
		}
		key0=getKeys();
		if(ikey==0){
			if(keyi[key0]>-1)
			{
				ikey=keyi[key0];
			}
			if (gwTimeDisplay == 1)
			{
				sprintf(ch0, "Time:%s", TDL(RTC_GetCounter()+wT));
				if(strncmp(ch0, "Time:00:00:00", 13) == 0){
					dayz++;
					if((((monthz<8&&monthz%2==1)||(monthz>7&&monthz%2==0))&&dayz>31)
						||(((monthz<8&&monthz%2==0)||(monthz>7&&monthz%2==1))&&dayz>30)){
						dayz=1;
						monthz++;
					}
					if(monthz>12){
						yearz++;
						monthz=1;
					}
				}
				gwTimeDisplay = 0;
			}
			if(strncmp(TDL(RTC_GetCounter()+wT), chi, 5) == 0) {
				/*LCD_ClearScreen();
				power_up();
				x=get_units(1);
				xx=x;
				power_down();
				while(x<fg){
					GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_2,SET);
					power_up();
					x=get_units(1);
					power_down();
					for(jj=0;jj<25;jj++){
						for(ii=0;ii<4;ii++){
							motor_start(ii);
							delay_ms(5);
						}
						GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_2,RESET);
					}
				}
				GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_2,RESET);
				motor_end();
				printf("%d %d %d %d %d 0 %d %d!",yearz,monthz,dayz,ihr,imin,(int)x,(int)xx);
				fi=getchar();
				if(fi==0x2d){
					FLASH_Program(yearz);
					FLASH_Program(monthz);
					FLASH_Program(dayz);
					FLASH_Program(ihr);
					FLASH_Program(imin);
					FLASH_Program(x);
					FLASH_Program(xx);
				}
				else if(fi==0x2b){
					FLASH_Check();
				}
				ihr+=ch;
				if(ihr>23){
					ihr-=24;
				}
				sprintf(chi, "%02d:%02d",ihr,imin);*/
			}
			else{
				LCD_PrintLine(1,ch0);
				sprintf(ch1, S1);
				LCD_PrintLine(2,ch1);
				sprintf(ch2, S2);
				LCD_PrintLine(3,ch2);
				sprintf(ch3, S3);
				LCD_PrintLine(4,ch3);
			}
		}
		else if(ikey==1){
			/*LCD_ClearScreen();
			if(keyi[key0]>-1)
			{
				if(st==0){
					yearz=yearz*10+keyi[key0];
				}
				else if(st==1){
					if((monthz<3)&&(keyi[key0]<2))
						monthz=monthz*10+keyi[key0];
					else
						monthz=keyi[key0];
				}
				else if(st==2){
					if(keyi[key0]==3)
						if((monthz<8&&monthz%2==1)||(monthz>7&&monthz%2==0))
							dayz=31;
						else
							dayz=30;
					else if(keyi[key0]<3){
						dayz=dayz*10+keyi[key0];
					}
					else
						dayz=keyi[key0];
				}
				else if(st==3){
					if((hr<2)||(hr<3&&keyi[key0]<4))
						hr=hr*10+keyi[key0];
					else
						hr=keyi[key0];
				}
				else if(st==4){
					if(min<6)
						min=min*10+keyi[key0];
					else
						min=keyi[key0];
				}
			}
			else if(keyi[key0]==-5){
				if(st==0){
					RTC_Configuration();
					RTC_Cmd(ENABLE);
					hr=0;
					min=0;
					ikey=0;
				}
				else if(st>0){
					st--;
				}
			}
			else if(keyi[key0]==-6){
				if(st<4){
					st++;
				}
				else if(st==4){
					wT=Time_Set(hr,min);
					RTC_Configuration();
					RTC_Cmd(ENABLE);
					st=0;
					hr=0;
					min=0;
					ikey=0;
				}
			}
			if(st==0){
				sprintf(ch0, "set yearz:%d",yearz);
			}
			else if(st==1){
				sprintf(ch0, "set monthz:%d",monthz);
			}
			else if(st==2){
				sprintf(ch0, "set dayz:%d",dayz);
			}
			else if(st==3){
				sprintf(ch0, "set hour:%02d",hr);
			}
			else if(st==4){
				sprintf(ch0, "set minute:%02d",min);
			}
			LCD_PrintLine(1,ch0);
			delay_ms(50);*/
		}
		else if(ikey==2){
			LCD_ClearScreen();
			if(keyi[key0]>-1)
			{
				if(st==0){
					chh=keyi[key0];
				}
				else if(st==1){
					if((ihr<2)||(ihr<3&&keyi[key0]<4))
						ihr=ihr*10+keyi[key0];
					else
						ihr=keyi[key0];
				}
				else if(st==2){
					if(imin<6)
						imin=imin*10+keyi[key0];
					else
						imin=keyi[key0];
				}
			}
			else if(keyi[key0]==-5){
				if(st==0){
					chh=ch;
					ikey=0;
				}
				else if(st==1){
					st=0;
				}
				else if(st==2){
					st=1;
				}
			}
			else if(keyi[key0]==-6){
				if(st==0){
					st=1;
				}
				else if(st==1){
					st=2;
				}
				else if(st==2){
					ch=chh;
					sprintf(chi, "%02d:%02d",ihr,imin);
					st=0;
					ikey=0;
				}
			}
			if(st==0){
				sprintf(ch0, "set interval:%d",chh);
			}
			else if(st==1){
				sprintf(ch0, "set hour:%02d",ihr);
			}
			else if(st==2){
				sprintf(ch0, "set minute:%02d",imin);
			}
			LCD_PrintLine(1,ch0);
			delay_ms(50);
		}
		else if(ikey==3){
			LCD_ClearScreen();
			if(keyi[key0]>-1)
			{
				if(fgg<500)
					fgg=fgg*10+keyi[key0];
				else if(fgg==500)
					fgg=keyi[key0];
				if(fgg>500)
					fgg=500;
			}
			else if(keyi[key0]==-5){
				ikey=0;
			}
			else if(keyi[key0]==-6){
				fg=(float)fgg;
				fgg=ch;
				ikey=0;
			}
			sprintf(ch0, "set feed:%d",fgg);
			LCD_PrintLine(1,ch0);
			delay_ms(50);
		}
	}
}

void FLASH_Erase(void)
{
  u32 Addr,e=0;
  FLASH_State FLASHState;
  for (Addr = StartAddress; Addr < EndAddress; Addr += FLASH_PAGE_SIZE)
  {
    FLASHState=FLASH_ErasePage(Addr);
		//printf("e:%u\n",e++);
  }
}

void FLASH_Program(u32 uData)
{
	u32 uAddr;
  FLASH_State FLASHState;
	uAddr=StartAddress;
	while(rw(uAddr)!=0xFFFFFFFF){
		uAddr+=4;
	}
	//printf("P %u:%u\n",uAddr,uData);
	FLASHState=FLASH_ProgramWordData(uAddr, uData);
}

void FLASH_Check(void)
{
  u32 uAddr;
	int ff,fff[7];
  FLASH_State FLASHState;
	uAddr=StartAddress;
	while(rw(uAddr)!=0xFFFFFFFF){
		for(ff=0;ff<7;ff++){
			fff[ff]=rw(uAddr);
			uAddr+=4;
		}
		printf("%d %d %d %d %d 0 %d %d!",fff[0],fff[1],fff[2],fff[3],fff[4],fff[5],fff[6]);
	}
	FLASH_Erase();
} 
