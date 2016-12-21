#pragma once


#include "Decoder.h"


// —труктура описывает границы секции, полностью декомпилированной и непрерывной
// в start хранитс€ адрес первого байта первой команды, в end - адрес последнего байта последней команды, как правило - 
// команды перехода. ≈сли сразу после этой команды перехода размещаетс€ друга€ комнда, еЄ адрес будет размещЄн в следующей
// секции Section.
typedef struct
{
    uint16 start;
    uint16 end;
} Section;

#define NUM_SECTIONS (1024 * 8)

void DataBase_Init(Section sections[NUM_SECTIONS]);
void DataBase_GetData(uint16 address, OutStruct params[20]);
