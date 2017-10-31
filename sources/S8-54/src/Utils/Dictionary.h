#pragma once
#include "Settings/SettingsService.h"


typedef enum
{
    DModel,             // "Модель"
    DSoftware,          // "Программное обеспечение :"
    DVersion,           // "версия %s"
    DInformation,       // "ИНФОРМАЦИЯ"
    DTrigLev,           // "Ур синхр = "
    DTrig,              // "СИ"
    DMode,              // "режим"
    D1ch,               // "1к"
    D2ch,               // "2к"
    DBalanceChA,        // "Балансировка канала 1"
    DBalanceChB,        // "Балансировка канала 2"
    DDetectFlashDrive,  // "Обнаружено запоминающее устройство"
    DSaveFirmware,      // "Сохраняю прошивку"
    DStoredInMemory,    // "Записываю в память"
    DM,                 // "М"
    DABS,               // "АБС"
    DREL,               // "ОТН"
    DDis,               // "Вых"
    DShift,             // "См"
    D10uV,              // "\х10мкВ"
    D10mV,              // "\х10мВ"
    D10V,               // "\х10В"
    D10kV,              // "\х10кВ"
    Dns,                // "нс"
    Dus,                // "мкс"
    Dms,                // "мс"
    Ds,                 // "с"
    DMHz,               // "МГц"
    DkHz,               // "кГц"
    DHz,                // "Гц"
    DDeleteFromMemory,  // "Удаляю сохранённые данные"
    DNumWords
} DictWord;

#define DICT(word) (dictWords[word][LANG])

extern const char * const dictWords[DNumWords][2];
