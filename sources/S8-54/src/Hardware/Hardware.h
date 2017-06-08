#pragma once
#include "defines.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @defgroup Hardware
 *  @brief Аппаратные ресурсы
 *  @{
 */

/** @defgroup Priorities
 *  @brief Приоритеты аппаратных модулей
 *  @{
 */

#define PRIORITY_SYS_TICK           0, 0
#define PRIORITY_FPGA_ADC           1, 0
#define PRIORITY_FPGA_P2P           1, 1
#define PRIORITY_TIMER_TIM6         2, 0
#define PRIORITY_PANEL_EXTI9_5      3, 0
#define PRIORITY_PANEL_SPI1         4, 0
#define PRIORITY_SOUND_DMA1_STREAM5 5, 0
#define PRIORITY_FLASHDRIVE_OTG     6, 0
#define PRIORITY_VCP_OTG            7, 0
#define PRIORITY_RAM_DMA2_STREAM0   8, 0

/** @}
 */

/// Инициализация аппаратных ресурсов
void Hardware_Init(void);
/// Возвращает контрольную сумму содержимого трёх секторов (5, 6, 7), в которых хранится основная программа.
uint Hardware_CalculateCRC32(void);

// #define HARDWARE_ERROR HardwareErrorHandler(__FILE__, __FUNCTION__, __LINE__);
// void HardwareErrorHandler(const char *file, const char *function, int line) {};

/** @}
 */
