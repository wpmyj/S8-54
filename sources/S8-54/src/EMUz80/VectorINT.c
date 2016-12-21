#include "VectorINT.h"

#include <stdlib.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VectorINT_Init(VectorINT *vector)
{
    vector->numElements = 0;
    vector->buffer = 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void VectorINT_PushBack(VectorINT *vector, int value)
{
    int *temp = malloc((vector->numElements + 1) * sizeof(int));
    for (int i = 0; i < vector->numElements; i++)
    {
        temp[i] = vector->buffer[i];
    }
    temp[vector->numElements] = value;
    free(vector->buffer);
    vector->buffer = temp;
    vector->numElements++;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int VectorINT_Size(VectorINT *vector)
{
    return vector->numElements;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int VectorINT_GetElement(VectorINT *vector, int position)
{
    if (position < vector->numElements)
    {
        return vector->buffer[position];
    }

    return 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void VectorINT_Erase(VectorINT *vecotr, int position)
{

}
