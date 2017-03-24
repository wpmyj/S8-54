#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "FPGA/fpgaTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    uint timeMS : 32;    // Время в миллисекундах от старта системы. Т.к. структура заполняется во время сохранения данных в хранилище, то timeMS == 0 означает, что полный сигнал в режиме поточеного вывода ещё не считан
    uint hours : 5;
    uint minutes : 6;
    uint seconds : 6;
    uint year : 7;
    uint month : 4;
    uint day : 5;
} PackedTime;

typedef struct
{
    uint8*      addr;               // Адрес данных во внешнем ОЗУ
    uint16      rShift[2];
    uint16      trigLev[2];
    int16       tShift;             // Смещение по времени
    uint8       range[2];           // Масштаб по напряжению обоих каналов.
    uint        tBase : 5;          // Масштаб по времени
    uint        enableA : 1;        // Включён ли канал A
    uint        enableB : 1;        // Включен ли канал B
    uint        coupleA : 2;        // Режим канала по входу
    uint        coupleB : 2;
    uint        peackDet : 2;       // Включен ли пиковый детектор
    uint        inverseA : 1;
    uint        inverseB : 1;
    uint        multiplierA : 1;
    uint        multiplierB : 1;
    uint        indexLength : 3;    // Сколько байт в канале (при включённом пиковом детекторе байт в два раза больше, чем точек)
    PackedTime  time;
} DataSettings;


#define INDEXLENGTH(ds)         ((ds)->indexLength)

#define TRIGLEV(ds, ch)         ((ds)->trigLev[ch])
#define TRIGLEV_A(ds)           (TRIGLEV(ds, A))
#define TRIGLEV_B(ds)           (TRIGLEV(ds, B))

#define ENABLED_A(ds)           ((bool)(ds)->enableA)
#define ENABLED_B(ds)           ((bool)(ds)->enableB)
#define ENABLED(ds, ch)         ((ch == A) ? ENABLED_A(ds) : ENABLED_B(ds))

#define Lval_ENABLED_A(ds)      ((ds)->enableA)
#define Lval_ENABLED_B(ds)      ((ds)->enableB)
#define Lval_ENABLED(ds, ch)    ((ch == A) ? Lval_ENABLED_A(ds) : Lval_ENABLED_B(ds))

#define INVERSE_A(ds)           ((ds)->inverseA)
#define INVERSE_B(ds)           ((ds)->inverseB)
#define INVERSE(ds, ch)         ((ch == A) ? INVERSE_A(ds) : INVERSE_B(ds))

#define TSHIFT(ds)              ((ds)->tShift)
#define TBASE(ds)               ((TBase)(ds)->tBase)
#define Lval_TBASE(ds)          ((ds)->tBase)

#define COUPLE_A(ds)            ((ModeCouple)(ds)->coupleA)
#define COUPLE_B(ds)            ((ModeCouple)(ds)->coupleB)
#define COUPLE(ds, ch)          ((ch == A) ? COUPLE_A(ds) : COUPLE_B(ds))

#define Lval_COUPLE_A(ds)       ((ds)->coupleA)
#define Lval_COUPLE_B(ds)       ((ds)->coupleB)

#define DIVIDER_A(ds)           ((Divider)(ds)->multiplierA)
#define DIVIDER_B(ds)           ((Divider)(ds)->multiplierB)
#define DIVIDER(ds, ch)         ((ch == A) ? DIVIDER_A(ds) : DIVIDER_B(ds))

#define Lval_DIVIDER_A(ds)      ((ds)->multiplierA)
#define Lval_DIVIDER_B(ds)      ((ds)->multiplierB)

#define RANGE(ds, ch)           ((Range)(ds)->range[ch])
#define RANGE_A(ds)             (RANGE(ds, A))
#define RANGE_B(ds)             (RANGE(ds, B))

#define Lval_RANGE(ds, ch)      ((ds)->range[ch])
#define Lval_RANGE_A(ds)        (Lval_RANGE(ds, A))
#define Lval_RANGE_B(ds)        (Lval_RANGE(ds, B))

#define RSHIFT(ds, ch)          ((ds)->rShift[ch])
#define RSHIFT_A(ds)            (RSHIFT(ds, A))
#define RSHIFT_B(ds)            (RSHIFT(ds, B))

#define PEACKDET(ds)            ((bool)(ds)->peackDet)
#define Lval_PEACKDET(ds)       ((ds)->peackDet)

#define TIME_DAY(ds)            ((ds)->time.day)
#define TIME_HOURS(ds)          ((ds)->time.hours)
#define TIME_MINUTES(ds)        ((ds)->time.minutes)
#define TIME_SECONDS(ds)        ((ds)->time.seconds)
#define TIME_MONTH(ds)          ((ds)->time.month)
#define TIME_YEAR(ds)           ((ds)->time.year)
#define TIME_MS(ds)             ((ds)->time.timeMS)

#define ADDRESS_DATA(ds)        ((ds)->addr)


int NumBytesInChannel(const DataSettings *ds);  // Возвращает количество байт на канал
int NumBytesInData(const DataSettings *ds);     // Возвращает количество байт в обоих каналах
int NumPointsInChannel(const DataSettings *ds); // Возвращает количество точек на канал

// Возвращает 0, если канал выключен
uint8 *AddressChannel(DataSettings *ds, Channel ch);


typedef enum
{
    StateOSCI_Start,
    StateOSCI_DrawLoPart        // Пишем надписи в нижней части экрана
} StateOSCI;

extern StateOSCI gState;    // Для описания текущего состояния прибора - например, сбор данных, вывод на дисплей текущего сигнала или из памяти

#define NUM_DATAS 999
extern DataSettings gDatas[NUM_DATAS];                  // Используется только в DataStorage
extern uint8 gDataAve[NumChannels][FPGA_MAX_POINTS];    // Используется только в DataStorage

extern int gAddNStop;
extern void *extraMEM;      // Это специальный указатель. Используется для выделения памяти переменным, которые не нужны всё время выполения программы,
                            // но нужны болеее чем в одной функции. Перед использованием с помощью вызова malloc() выделяется необходимое количество
                            // памяти, которая затем освобождается вызвом free()



#define INVERSE_CH(ch) GetInverse(ch)

bool GetInverse(Channel ch);
