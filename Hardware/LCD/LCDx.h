#ifndef _LCDX_H
#define _LCDX_H 
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "String.h"
//#include "ASC.c"

void delay(u32 nCount);
void SetSpeed(int speed);    
u8 OWTouchReset(void);
void OWWriteBit(u8 data);
void OWWriteByte(u8 data);
int OWReadBit(void);
int OWReadByte(void);
unsigned char docrc8(unsigned char value);
u8 OWFirst(void);
u8 OWNext(void);
s16 ReadDesignateTemper(u8 pID[8]);
void Init_LCD(void);
void LCD_SendCommand(char cmd);
void LCD_SendData(char data);
void LCD_ClearScreen(void);
void LCD_PrintLine(int line , char *str);
void LCD_PrintWord(int line , int row ,char word);
unsigned char reR(u8 i);
void LCD_INIT(void);
#endif
