#ifndef Keypad_h
#define Keypad_h
#include "ht32.h"

void Keypad_init(void);
int scan(void);
int scanKeys(void);
int getKeys(void);
void reKeys(void);

#endif
