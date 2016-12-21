#include "Command.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------
void CommandZ80_Init(CommandZ80 *command)
{
    command->bad = false;
    command->address = 0;
    command->numTackts = 0;
    command->tackts = 0;
    VectorUINT8_Init(&command->opCodes);
    command->mnemonic = 0;
    command->transcript = 0;
    command->flags = 0;
    command->comment = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
bool CommandZ80_OperatorLess(CommandZ80 *this, CommandZ80 *another)
{
    return this->address < another->address;
}
