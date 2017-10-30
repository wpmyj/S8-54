#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"


/** @defgroup DataBuffer
 *  @{
 */


extern uint8 *dataIN[NumChannels];      ///< Считанные данные первого канала.
extern uint8 *dataOUT[NumChannels];     ///< Данные к выводу первого канала.

#define IN(ch)  (dataIN[ch])
#define IN_A    IN(A)
#define IN_B    IN(B)
#define OUT(ch) (dataOUT[ch])
#define OUT_A   OUT(A)
#define OUT_B   OUT(B)


extern void *extraMEM;      // Это специальный указатель. Используется для выделения памяти переменным, которые не нужны всё время выполения программы,
                            // но нужны болеее чем в одной функции. Перед использованием с помощью вызова malloc() выделяется необходимое количество
                            // памяти, которая затем освобождается вызвом free()
#define MALLOC_EXTRAMEM(NameStruct, name)   extraMEM = malloc(sizeof(NameStruct));    \
                                            NameStruct *name = (NameStruct*)extraMEM
#define ACCESS_EXTRAMEM(NameStruct, name)   NameStruct *name = (NameStruct*)extraMEM
#define FREE_EXTRAMEM()                     free(extraMEM);


/** @}
 */
