#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"


/** @defgroup DataBuffer
 *  @{
 */


extern uint8 *dataIN[NumChannels];      ///< Считанные данные первого канала.
extern uint8 *dataOUT[NumChannels];     ///< Данные к выводу первого канала.

#define IN_A    dataIN[A]
#define IN_B    dataIN[B]
#define OUT_A   dataOUT[A]
#define OUT_B   dataOUT[B]


extern void *extraMEM;      // Это специальный указатель. Используется для выделения памяти переменным, которые не нужны всё время выполения программы,
                            // но нужны болеее чем в одной функции. Перед использованием с помощью вызова malloc() выделяется необходимое количество
                            // памяти, которая затем освобождается вызвом free()
#define MALLOC_EXTRAMEM(NameStruct, name)   extraMEM = malloc(sizeof(NameStruct));    \
                                            NameStruct *name = (NameStruct*)extraMEM
#define ACCESS_EXTRAMEM(NameStruct, name)   NameStruct *name = (NameStruct*)extraMEM
#define FREE_EXTRAMEM()                     free(extraMEM);


/** @}
 */
