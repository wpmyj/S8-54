#pragma once 
#include "defines.h"
#include "Panel/Controls.h"
#include "Settings/SettingsTypes.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @defgroup Panel 
 *  @brief Работа с панелью управления
 *  @{
 */

class Panel
{
public:
    /// Инициализация аппартаного обеспечения
    void Init(void);
    /// Функцию нужно вызывать каждый кадр для обработки полученных в данном кадре событий клавиатуры
    void Update(void);
    /// Включить/выключить лампочку одного из каналов
    void EnableLEDChannel(Channel ch, bool enable);
    /// Включить/выключить лампочку УСТАНОВКА
    void EnableLEDRegSet(bool enable);
    /// Включить/выключить лампочку СИНХР
    void EnableLEDTrig(bool enable);
    /// Передача данных в МК клавиатуры
    void TransmitData(uint16 data);
    /// Отлючить панель управления
    /** Когда панель отключена, то происходит лишь обновление состояния переменной releasedButton */
    void Disable(void);
    /// Включить панель управления
    void Enable(void);
    /// Обработать полученную команду
    bool ProcessingCommandFromPIC(uint16 command);
    /// Возвращает следующие данные, подготовленные для отправки в панель, из буфера
    uint16 NextData(void);
    /// Ожидание нажатия клавиши
    PanelButton WaitPressingButton(void);
    /// Отлкючает прибор, если была нажата кнопка ПИТАНИЕ
    void DisableIfNessessary(void);
};


extern Panel panel;


/**  @}
 */
