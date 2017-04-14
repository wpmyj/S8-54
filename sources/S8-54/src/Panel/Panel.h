#pragma once 
#include "defines.h"
#include "Panel/Controls.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @defgroup Panel 
 *  @brief Работа с панелью управления
 *  @{
 */

/// Инициализация аппартаного обеспечения
void    Panel_Init(void);
/// Функцию нужно вызывать каждый кадр для обработки полученных в данном кадре событий клавиатуры
void    Panel_Update(void);
/// Включить/выключить лампочку УСТАНОВКА
void    Panel_EnableLEDRegSet(bool enable);
/// Включить/выключить лампочку КАНАЛ1
void    Panel_EnableLEDChannelA(bool enable);
/// Включить/выключить лампочку КАНАЛ2
void    Panel_EnableLEDChannelB(bool enable);
/// Включить/выключить лампочку СИНХР
void    Panel_EnableLEDTrig(bool enable);
/// Передача данных в МК клавиатуры
void    Panel_TransmitData(uint16 data);
/// Отлючить панель управления
/** Когда панель отключена, то происходит лишь обновление состояния переменной pressedButton */
void    Panel_Disable(void);
/// Включить панель управления
void    Panel_Enable(void);
/// Обработать полученную команду
bool    Panel_ProcessingCommandFromPIC(uint16 command);
/// Возвращает следующие данные, подготовленные для отправки в панель, из буфера
uint16  Panel_NextData(void);
/// Ожидание нажатия клавиши
PanelButton   Panel_WaitPressingButton(void);

/**  @}
 */
