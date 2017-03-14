#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    uint timeMS;    // Время в миллисекундах от старта системы. Т.к. структура заполняется во время сохранения данных в хранилище, то timeMS == 0 означает, что полный сигнал в режиме поточеного вывода ещё не считан
    uint hours : 5;
    uint minutes : 6;
    uint seconds : 6;
    uint year : 7;
    uint month : 4;
    uint day : 5;
} PackedTime;

typedef struct
{
    uint16      rShift[2];
    uint16      trigLev[2];
    int16       tShift;                 // Смещение по времени
    uint8       range[2];               // Масштаб по напряжению обоих каналов.
    uint8*      addrData;               // Адрес данных во внешнем ОЗУ
    uint        tBase : 5;    // Масштаб по времени
    uint        enableB : 1;    // Включен ли канал B
    uint        indexLength : 3;    // Сколько байт в канале (при включённом пиковом детекторе байт в два раза больше, чем точек)
    uint        modeCoupleA : 2;    // Режим канала по входу
    uint        modeCoupleB : 2;
    uint        peackDet : 2;    // Включен ли пиковый детектор
    uint        enableA : 1;    // Включён ли канал A
    uint        inverseA : 1;
    uint        inverseB : 1;
    uint        multiplierA : 1;
    uint        multiplierB : 1;
    PackedTime  time;
} DataSettings;

int NumBytesInChannel(const DataSettings *ds);  // Возвращает количество байт на канал
int NumBytesInData(const DataSettings *ds);     // Возвращает количество байт в обоих каналах
int NumPointsInChannel(const DataSettings *ds); // Возвращает количество точек на канал

// Возвращает 0, если канал выключен
uint8 *AddressChannel(DataSettings *ds, Channel ch);
