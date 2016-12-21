#pragma once


#include "Command.h"


typedef struct
{
    int numElements;
} VectorCommandZ80;


void VectorCommandZ80_Init(VectorCommandZ80 *vector);
void VectorCommandZ80_PushBack(VectorCommandZ80 *vector, CommandZ80 command);
int VectorCommandZ80_Size(VectorCommandZ80 *vector);
CommandZ80 VectorCommandZ80_GetElement(VectorCommandZ80 *vector, int position);
