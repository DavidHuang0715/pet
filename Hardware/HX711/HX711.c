#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "delay.h"
#define DataP GPIO_PIN_2
#define ClockP GPIO_PIN_5

uint8_t  _gain = 128;
long     _offset = 0;
float    _scale = 1;
uint32_t _lastRead = 0;
float    _price = 0;
uint8_t m = 1;

	union
	{
		long value;
		uint8_t data[4];
	} v;

void HX_begin(void)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};

	CKCUClock.Bit.PC = 1;
	CKCUClock.Bit.AFIO = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	AFIO_GPxConfig(GPIO_PC, ClockP, AFIO_MODE_DEFAULT);
	//GPIO_WriteOutBits(HT_GPIOC,ClockP,SET);
	GPIO_DirectionConfig(HT_GPIOC, ClockP, GPIO_DIR_OUT);
	
	AFIO_GPxConfig(GPIO_PC, DataP, AFIO_MODE_DEFAULT);
	GPIO_DirectionConfig(HT_GPIOC, DataP, GPIO_DIR_IN);
	GPIO_InputConfig(HT_GPIOC, DataP, ENABLE);
}
void reset(void)
{
  _offset = 0;
  _scale = 1;
  _gain = 128;
}

bool is_ready(void)
{
	bool i;
	if(GPIO_ReadInBit(HT_GPIOC,DataP) == SET){
		i=TRUE;
	}
	else
		i=FALSE;
  return i;
}
void wait_ready(u16 ms) 
{
  while (!is_ready())
  {
    delay_ms(ms);
  }
}

bool wait_ready_retry(uint8_t retries, u16 ms) 
{
  while (retries--)
  {
    if (is_ready()) return TRUE;
    delay_ms(ms);
  }
  return FALSE;
}

bool wait_ready_timeout(uint32_t timeout, u16 ms)
{
  //uint32_t start = millis();
	uint32_t start=0;
  while (start < timeout) 
  {
    if (is_ready()) return TRUE;
    delay_ms(ms);
		start++;
  }
  return FALSE;
}

uint8_t shiftIn(void)
{
	uint8_t s;
	uint8_t i;
	s=0x00;
	for (i = 0; i < 8; ++i) {
		GPIO_WriteOutBits(HT_GPIOC,ClockP,RESET);
		s |= (GPIO_ReadInBit(HT_GPIOC,DataP)<< (7 - i));
		GPIO_WriteOutBits(HT_GPIOC,ClockP,SET);
	}
	return s;
}

long read(void) 
{
	uint8_t i;
  // this waiting takes most time...
	while (GPIO_ReadInBit(HT_GPIOC,DataP) == RESET){break;}
  //*noInterrupts();
	v.value=0;
  // Pulse the clock pin 24 times to read the data.
  v.data[2] = shiftIn();
  v.data[1] = shiftIn();
  v.data[0] = shiftIn();

  // TABLE 3 page 4 datasheet
  // only default verified, so other values not supported yet
	m=1;
  if (_gain == 64) m = 3;
  if (_gain == 32) m = 2;
	
  for(i=0;i<m ;i++)
  {
		GPIO_WriteOutBits(HT_GPIOC,ClockP,RESET);
		GPIO_WriteOutBits(HT_GPIOC,ClockP,SET);
  }

  //*interrupts();

  // SIGN extend
  if (v.data[2] & 0x80) {
		v.data[3] = 0xFF;
	}
	else{
		v.data[3] = 0x00;
	}
//printf("\r\n%dh",v.data[2]);
//printf("\r\n%d",v.data[2]&0x80);
  //*_lastRead = millis();
	v.value = ( (v.data[3]) << 24
			| (v.data[2]) << 16
			| (v.data[1]) << 8
			| (v.data[0]) );
  return v.value;
}

long readAV(uint8_t times) 
{
  long sum;
	uint8_t i;
	sum=0;
  for (i = 0; i < times; i++) 
  {
    sum += read();
		//printf("\r\n%f",sum*1.0);
    delay_ms(1);
  }
  return (sum / times);
}

long get_value(uint8_t times) 
{
	return (readAV(times)-_offset); 
}
float get_units(uint8_t times)
{
  return (get_value(times)/_scale);
}

void tare(void)
{
	_offset = readAV(1);
	//printf("\r\nt:%f",_offset*1.0);
}

void set_scale(float scale)  
{ 
	_scale = scale; 
}

void power_down(void) 
{
  GPIO_WriteOutBits(HT_GPIOC,ClockP,SET);
  GPIO_WriteOutBits(HT_GPIOC,ClockP,RESET);
}

void power_up(void) 
{
  GPIO_WriteOutBits(HT_GPIOC,ClockP,SET);
}
