#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "Display/Display.h"
#include "FPGA/DataStorage.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Hardware
 *  @{
 *  @defgroup FLASH
 *  @brief Работа с внутренним ППЗУ
 *  @{
 */

#define MAX_NUM_SAVED_WAVES 23  ///< Число сохраняемых во внутреннем ППЗУ измерений. Пока ограничено количеством квадратиков, которые можно вывести в одну линию внизу сетки.

void FLASH_LoadSettings(void);                      // Если onlyNonReset == true, загружаются только несбрасываемые настройки
void FLASH_SaveSettings(void);

void FLASH_GetDataInfo(bool existData[MAX_NUM_SAVED_WAVES]);     // Если даннные есть, соответствующий элемент массива равен true
bool FLASH_ExistData(int num);
void FLASH_SaveData(int num, DataSettings *ds, uint8 *dataA, uint8 *dataB);
bool FLASH_GetData(int num, DataSettings **ds, uint8 **dataA, uint8 **dataB);
void FLASH_DeleteData(int num);
void FLASH_DeleteAllData(void);

/** @} @}
 */
