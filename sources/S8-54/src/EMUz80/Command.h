#pragma once


#include "defines.h"
#include "VectorUINT8.h"
#include "String.h"


typedef struct
{
    bool bad;
    int address;
    VectorUINT8 opCodes;
    char* mnemonic;
    int numTackts;
    char* transcript;
    char* flags;
    char* comment;
    int tackts;
} CommandZ80;

void CommandZ80_Init(CommandZ80 *comand);
bool CommandZ80_OperatorLess(CommandZ80 *first, CommandZ80 *another);
