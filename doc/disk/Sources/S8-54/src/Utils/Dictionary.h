#pragma once
#include "Settings/SettingsService.h"


typedef enum
{
    DModel,
    DSoftware,
    DVersion,
    DInformation,
    DTrigLev,
    DTrig,
    DMode,
    D1ch,
    D2ch,
    DBalanceChA,
    DBalanceChB,
    DDetectFlashDrive,
    DSaveFirmware,
    DStoredInMemory,
    DM,
    DABS,
    DREL,
    DDis,
    DShift,
    D10uV,
    D10mV,
    D10V,
    D10kV,
    Dns,
    Dus,
    Dms,
    Ds,
    DMHz,
    DkHz,
    DHz,
    DNumWords
} DictWord;

#define DICT(word) (dictWords[word][LANG])

extern const char * const dictWords[DNumWords][2];
