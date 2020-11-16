#ifndef HX711_h
#define HX711_h
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "delay.h"

#define HX711_LIB_VERSION  "0.2.0"

void HX_begin(void);
void reset(void);
bool is_ready(void);
void wait_ready(uint32_t ms);
bool wait_ready_retry(uint8_t retries, uint32_t ms);
bool wait_ready_timeout(uint32_t timeout, uint32_t ms);
long read(void);
long readAV(uint8_t times);
long get_value(uint8_t times);
float get_units(uint8_t times);
void tare(void);
void set_scale(float scale);
void power_down(void);
void power_up(void);
#endif /* HX711_h */
