#pragma once


#include "definesZ80.h"
#include "Decoder.h"


extern OutStruct *out;


#define TACKTS      out->tackts
#define FLAGS       out->flags
#define COMMENT     out->comment
#define MNEMONIC    out->mnemonic
#define TRANSCRIPT  out->transcript

void AddAddress(uint address);
void AddOpcode(uint8 code);
