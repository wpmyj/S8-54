#pragma once


#include "definesZ80.h"


extern uint8 *RAMZ80;

#define RAM16(address) (*((uint16*)&RAMZ80[address]))
#define RAM8(address) (*(&RAMZ80[address]))
