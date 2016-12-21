#pragma once


typedef struct
{
    int *buffer;
    int numElements;
} VectorINT;


void VectorINT_Init(VectorINT *vector);
void VectorINT_PushBack(VectorINT *vector, int value);
int VectorINT_Size(VectorINT *vector);
int VectorINT_GetElement(VectorINT *vector, int position);
void VectorINT_Erase(VectorINT *vecotr, int position);
