#include "VectorCommandZ80.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------
void VectorCommandZ80_Init(VectorCommandZ80 *vector)
{
    vector->numElements = 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void VectorCommandZ80_PushBack(VectorCommandZ80 *vector, CommandZ80 command)
{

}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int VectorCommandZ80_Size(VectorCommandZ80 *vector)
{
    return vector->numElements;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
CommandZ80 VectorCommandZ80_GetElement(VectorCommandZ80 *vector, int position)
{
    CommandZ80 command;
    CommandZ80_Init(&command);
    return command;
}
