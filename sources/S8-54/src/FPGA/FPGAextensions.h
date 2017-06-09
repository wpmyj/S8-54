#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup FPGA
 *  @{
 *  @defgroup FPGA_Extensions FPGA Extensions
 *  @brief Дополнительные функции работы с альтерой
 *  @{
 */

/** @defgroup AutoFind Автопоиск
  * @brief Автоматический поиск сигнала
  * @{
  */

 /// Запуск процесса поиска сигнала
void  FPGA_AutoFind(void);

/** @}
  */

/// Запуск функции калибровки
void  FPGA_ProcedureCalibration(void);
/// Провести процедуру балансировки
void  FPGA_BalanceChannel(Channel ch);

bool  FreqMeter_Init(void);

void  FreqMeter_Draw(int x, int y);
/// Получить значение частоты для вывода в нижней части экрана
float FreqMeter_GetFreq(void);
/// Функция вызывается из FPGA
void  FreqMeter_Update(uint16 flag);

/** @}  @}
 */
