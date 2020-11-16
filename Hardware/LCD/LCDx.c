#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "String.h"
#include "ASC.c"

#define oneWire_pin_1 HT_GPIOD
#define oneWire_pin_2 GPIO_PIN_1
//#define FALSE 0
//#define TRUE  1
#define DS18B20_NUM		4
#define LCD_RS		GPIO_PIN_0
#define LCD_RES		GPIO_PIN_14
#define LCD_CS	 	GPIO_PIN_0
#define LCD_SDA	 	GPIO_PIN_2
#define LCD_SCLK 	GPIO_PIN_5
#define LCD_PORT1	HT_GPIOC

char LCDLine[4][17];

unsigned char ROM_NO[8];
char LastDiscrepancy;  
char LastFamilyDiscrepancy; 
bool LastDeviceFlag;   
unsigned char crc8;

int A,B,C,D,E,F,G,H,I,J,k;
unsigned char docrc8(unsigned char value);
void Init_LCD(void);
void LCD_SendCommand(char cmd);
void LCD_SendData(char data);
void LCD_ClearScreen(void);
void LCD_PrintLine(int line , char *str);
void LCD_PrintWord(int line , int row ,char word);
//delay(1ms)
void delay(u32 nCount){
  vu32 i;
  for (i = 0; i < 1* nCount; i++){
		__NOP();
  }
}

void SetSpeed(int speed)
{
	 double x=speed;
		// Standard Speed
		 A = 5 * x;
     B = 61 * x;
     C = 60 * x;
     D = 8 * x;
     E = 13 * x;
     F = 42 * x;
     G = 0;
     H = 479 *x;
     I = 55 * x;
     J = 240 * x;		
		 k = 240*x;
}

u8 OWTouchReset(void)
{
		u8 result;
		
		GPIO_DirectionConfig(oneWire_pin_1, oneWire_pin_2, GPIO_DIR_OUT);
		
		
		
		delay(G);//0
		GPIO_WriteOutBits(oneWire_pin_1, oneWire_pin_2, RESET); // Drives DQ low
		delay(H);//480
		GPIO_WriteOutBits(oneWire_pin_1, oneWire_pin_2, SET); // Releases the bus
		delay(I);//70
	
	
		GPIO_DirectionConfig(oneWire_pin_1, oneWire_pin_2, GPIO_DIR_IN);
		result = GPIO_ReadInBit(oneWire_pin_1, oneWire_pin_2);
	
		delay(J);
		
		GPIO_DirectionConfig(oneWire_pin_1, oneWire_pin_2, GPIO_DIR_OUT);
		delay(k);
		
		
		return result;
	
}

void OWWriteBit(u8 data)
{
		
		GPIO_DirectionConfig(oneWire_pin_1, oneWire_pin_2, GPIO_DIR_OUT);
		GPIO_WriteOutBits(oneWire_pin_1,oneWire_pin_2,RESET);
		delay(A);
		GPIO_WriteOutBits(oneWire_pin_1,oneWire_pin_2,data);
		delay(B-D);
		GPIO_WriteOutBits(oneWire_pin_1,oneWire_pin_2,SET);
		delay(D);
}

void OWWriteByte(u8 data)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		OWWriteBit(data & 0x01);
		data >>= 1;
	}
}

int OWReadBit(void)
{
	int buff;
	
	GPIO_DirectionConfig(oneWire_pin_1, oneWire_pin_2, GPIO_DIR_OUT);
	GPIO_WriteOutBits(oneWire_pin_1,oneWire_pin_2,RESET);
	delay(A);
	GPIO_WriteOutBits(oneWire_pin_1, oneWire_pin_2, SET);
	delay(A+10);
	
	GPIO_DirectionConfig(oneWire_pin_1, oneWire_pin_2, GPIO_DIR_IN);
	buff = GPIO_ReadInBit(oneWire_pin_1,oneWire_pin_2)& 0x01;
	delay(B-D);
	return buff;
}

int OWReadByte(void)
{
	int data = 0x00,i;
	for(i=0;i<8;i++)
	{
		data >>= 1;
		 if (OWReadBit())
            data |= 0x80;
	}
	return data;
}

u8 OWSearch(void)
{
   char id_bit_number;   
   char last_zero, rom_byte_number, search_result;
   u8 id_bit, cmp_id_bit,search_direction; 
   unsigned char rom_byte_mask ; 
   id_bit_number = 1;
   last_zero = 0;
   rom_byte_number = 0;
   rom_byte_mask = 1;
   search_result = 0;
   crc8 = 0;

   if (!LastDeviceFlag)  
   {
      if (OWTouchReset())    
      {
         LastDiscrepancy = 0;  
         LastDeviceFlag = FALSE;
         LastFamilyDiscrepancy = 0;
         return 0;    
      }
      OWWriteByte(0xF0);   
   delay(A);//30
      do        
      {
         id_bit = OWReadBit();   
         cmp_id_bit = OWReadBit();
         if (id_bit  && cmp_id_bit)
            break;
         else       
         {
           
            if (id_bit != cmp_id_bit)
               search_direction = id_bit; 
            else
            {
              
               if (id_bit_number < LastDiscrepancy)
                  search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
   
      else
               
                  search_direction = (id_bit_number == LastDiscrepancy);
   
               if (search_direction == 0)
               {
                  last_zero = id_bit_number;
                  
                  if (last_zero < 9)
                     LastFamilyDiscrepancy = last_zero;
               }
            }
            
            if (search_direction == 1)
               ROM_NO[rom_byte_number] |= rom_byte_mask;
            else
               ROM_NO[rom_byte_number] &= ~rom_byte_mask;
            OWWriteBit(search_direction);
            id_bit_number++;
            rom_byte_mask <<= 1;
            if (rom_byte_mask == 0)
            {
                docrc8(ROM_NO[rom_byte_number]);  
                rom_byte_number++;
                rom_byte_mask = 1;
            }
         }
      }
      while(rom_byte_number < 8); 
 
      if (!((id_bit_number < 65) || (crc8 != 0)))
      {
        LastDiscrepancy = last_zero;
         
         if (LastDiscrepancy == 0)
            LastDeviceFlag = TRUE;  
        
         search_result = 1; 
      }
   }
   if (!search_result || !ROM_NO[0])
   {
      LastDiscrepancy = 0;
      LastDeviceFlag = FALSE;
      LastFamilyDiscrepancy = 0;
      search_result = 0;
   }
   return search_result;
}
 
unsigned char docrc8(unsigned char value)
{
   crc8 = dscrc_table[crc8 ^ value];  
   return crc8;
}
u8 OWFirst(void)
{
   LastDiscrepancy = 0;
   LastDeviceFlag = FALSE;
   LastFamilyDiscrepancy = 0;
   return OWSearch();
}
u8 OWNext(void)
{
   return OWSearch();
}

s16 ReadDesignateTemper(u8 pID[8])
{
	u8 a;
	s16 data;
	int x[9];
	int temp=0x00;
	while(OWTouchReset());
	OWWriteByte(0xCC);
	OWWriteByte(0x44);
	while(OWTouchReset());
	OWWriteByte(0x55);
	for(data = 0;data < 8;data ++)
	{
	   OWWriteByte(pID[data]);
	}
	delay(50);
	OWWriteByte(0xBE);	
	
	for(a=0;a<9;a++)
		{
			x[a]=OWReadByte();
		}
		x[0]=x[0]>>4;
		x[1]=x[1]<<4;
		temp=x[0]+x[1];
		return temp;
} 

void Init_LCD(void){
	
	// Reset
	GPIO_WriteOutBits(HT_GPIOA, LCD_RES, RESET);
	delay(240);
	GPIO_WriteOutBits(HT_GPIOA, LCD_RES, SET);
	delay(240);
	
	//Reset Command
	LCD_SendCommand(0xE2);
	delay(30);
	
	//
	LCD_SendCommand(0x2C);
	delay(30);
	LCD_SendCommand(0x2E);
	delay(30);
	LCD_SendCommand(0x2F);
	delay(30);

	LCD_SendCommand(0x24);
	LCD_SendCommand(0x81);
	LCD_SendCommand(0x0F);
	LCD_SendCommand(0xA2);
	LCD_SendCommand(0xC0);
	LCD_SendCommand(0xA1);
	LCD_SendCommand(0x40);
	LCD_SendCommand(0xAF);	
}

void LCD_SendCommand(char cmd)
{
	char i = 0;
	GPIO_WriteOutBits(HT_GPIOB, LCD_CS, RESET);
	GPIO_WriteOutBits(HT_GPIOC, LCD_RS, RESET);
	for(i = 0 ; i < 8 ; i++)
	{
		GPIO_WriteOutBits(HT_GPIOB, LCD_SCLK, RESET);
		
		if(cmd & 0x80)
				GPIO_WriteOutBits(HT_GPIOB, LCD_SDA, SET);
		else
				GPIO_WriteOutBits(HT_GPIOB, LCD_SDA, RESET);
		delay(25);
		
		GPIO_WriteOutBits(HT_GPIOB, LCD_SCLK, SET);
		delay(25);
		cmd <<= 1;
	}
}

void LCD_SendData(char data)
{
	char i = 0;
	GPIO_WriteOutBits(HT_GPIOB, LCD_CS, RESET);
	GPIO_WriteOutBits(HT_GPIOC, LCD_RS, SET);
	for(i = 0 ; i < 8 ; i++){
		GPIO_WriteOutBits(HT_GPIOB, LCD_SCLK, RESET);	
		if(data & 0x80)
			GPIO_WriteOutBits(HT_GPIOB, LCD_SDA, SET);	
		else
				GPIO_WriteOutBits(HT_GPIOB, LCD_SDA, RESET);
		GPIO_WriteOutBits(HT_GPIOB, LCD_SCLK, SET);
		data <<= 1;
	}// end for 
}

void LCD_ClearScreen(void)
{
	unsigned char i,j = 0;
	for(i = 0 ; i < 9 ; i++)
	{
		GPIO_WriteOutBits(HT_GPIOB, LCD_CS, RESET);
		LCD_SendCommand(0xB0+i);
		LCD_SendCommand(0x10);
		LCD_SendCommand(0x00);
		for(j = 0 ; j < 132 ; j++)
			LCD_SendData(0x00);
	} // end for
}

void LCD_PrintLine(int line , char *str){
	int i;
	for(i = 0 ; i < 17 ; i++)						LCD_PrintWord(line , i , ' ');
	for(i = 0 ; i < strlen(str) ; i++)	LCD_PrintWord(line , i , str[i]);
}

void LCD_PrintWord(int line , int row ,char word)
{
	int  i , j , k , l;
	char temp;
	temp = LCDLine[line-1][row];
	k = (word - 32) * 16;
	l = 4 + (row*8 + 1);
	for(i = 0 ; i < 2 ; i++)
	{
		if(temp == word) break;
		LCDLine[line-1][row] = word;
		GPIO_WriteOutBits(HT_GPIOB, LCD_CS, RESET);
		LCD_SendCommand(0xB0 + ((line - 1) * 2) + i);
		LCD_SendCommand(0x10 + (l/16));
		LCD_SendCommand(0x00 + (l%16));
		for(j = 0 ; j < 8 ; j++)
		{
			LCD_SendData(Ascii_2[k+(i*8)+j]);
		}// end for
	}// end for
}

unsigned char reR(u8 i){
	return ROM_NO[i];
}

void LCD_INIT(void){
	USART_InitTypeDef USART_InitStructure;
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  HTCFG_INPUT_KEY1_CLK(CKCUClock)  = 1;
	HTCFG_INPUT_WAKE_CLK(CKCUClock)  = 1;
  CKCUClock.Bit.AFIO       = 1;
	COM1_CLK(CKCUClock)  = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	GPIO_DirectionConfig(HT_GPIOD,GPIO_PIN_2,GPIO_DIR_OUT);
	GPIO_PullResistorConfig(oneWire_pin_1,oneWire_pin_2,GPIO_PR_UP);
	GPIO_InputConfig(oneWire_pin_1,oneWire_pin_2,ENABLE);
	GPIO_DirectionConfig(HT_GPIOC, LCD_RS, GPIO_DIR_OUT);
  GPIO_DirectionConfig(HT_GPIOA, LCD_RES, GPIO_DIR_OUT);
  GPIO_DirectionConfig(HT_GPIOB, LCD_CS, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOB, LCD_SDA, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOB, LCD_SCLK, GPIO_DIR_OUT);

  AFIO_GPxConfig(COM1_TX_GPIO_ID, COM1_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(COM1_RX_GPIO_ID, COM1_RX_AFIO_PIN, AFIO_FUN_USART_UART);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
  USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
  USART_InitStructure.USART_Parity = USART_PARITY_NO;
  USART_InitStructure.USART_Mode = USART_MODE_NORMAL;

  USART_Init(COM1_PORT, &USART_InitStructure);
  USART_TxCmd(COM1_PORT, ENABLE);
  USART_RxCmd(COM1_PORT, ENABLE);
}
