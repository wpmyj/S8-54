#pragma once


#include "defines.h"


typedef struct
{
    int numElements;
} VectorUINT8;


void VectorUINT8_Init(VectorUINT8 *vector);
void VectorUINT8_PushBack(VectorUINT8 *vector, uint8 value);
