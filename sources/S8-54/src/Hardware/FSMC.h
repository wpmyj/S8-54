 #pragma once
#include "defines.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Hardware
 *  @{
 *  @defgroup FSMC
 *  @brief Работа с внешними устройствами по шине FSMC - OЗУ, Дисплей, Альтера
 *  @{
 */


#define ADDR_BANK           0x60000000

#define ADDR_CDISPLAY               ((uint8*)(ADDR_BANK + 0x080000))    // Здесь 8-битный низкоскоростной мультиплексный режим
#define ADDR_FPGA                   ((uint8*)(ADDR_BANK + 0x300000))    // Здесь 16-битный высокоскоростной немульиплексный режим
#define ADDR_RAM                    ((uint8*)(ADDR_BANK + 0x200000))    // Здесь 16-битный высокоскоростной немульиплексный режим


typedef enum
{
    ModeFSMC_None,
    ModeFSMC_Display,
    ModeFSMC_FPGA,
    ModeFSMC_RAM
} ModeFSMC;


void FSMC_Init(void);

void FSMC_SetMode(ModeFSMC mode);

// Функция используется для обращения к шине из прерываний - если чтение/запись происходит во время инициализации, возникает ошибка
bool FSMC_InSetStateMode(void);

// Функция используется в паре с предыдущей. Устанавливает функцию, которая должа быть выполнена после установки режима на шине
void FSMC_SetFuncitonAfterSetMode(pFuncBV func);
void FSMC_RemoveFunctionAfterSetMode(void);

// DEPTRECATED Восстанавливает режим, который был установлен перед вызовом FSMC_SetMode()
void FSMC_RestoreMode(void);
    
ModeFSMC FSMC_GetMode(void);

#define FSMC_READ(address) (*(address))
#define FSMC_WRITE(address, data) (*(address) = data)

#define FSMC_SET_MODE(mode)                     \
    ModeFSMC modePrev = FSMC_GetMode();         \
    bool needRestoreMode = mode != modePrev;    \
    if(needRestoreMode)                         \
    {                                           \
        FSMC_SetMode(mode);                     \
    }

#define FSMC_RESTORE_MODE()                     \
    if(needRestoreMode)                         \
    {                                           \
        FSMC_SetMode(modePrev);                 \
    }

/** @}  @}
 */
