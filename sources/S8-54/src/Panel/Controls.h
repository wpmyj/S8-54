#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Panel
 *  @{
 */

/// Идентификаторы кнопок.
typedef enum
{
    B_Empty     = 0x00,     ///< кнопка не нажата
    B_Channel1  = 0x01,     ///< КАНАЛ 1
    B_Service   = 0x02,     ///< СЕРВИС
    B_Channel2  = 0x03,     ///< КАНАЛ 2
    B_Display   = 0x04,     ///< ДИСПЛЕЙ
    B_Time      = 0x05,     ///< РАЗВ
    B_Memory    = 0x06,     ///< ПАМЯТЬ
    B_Trig      = 0x07,     ///< СИНХР
    B_Start     = 0x08,     ///< ПУСК/СТАРТ
    B_Cursors   = 0x09,     ///< КУРСОРЫ
    B_Measures  = 0x0a,     ///< ИЗМЕР
    B_Power     = 0x0b,     ///< ПИТАНИЕ
    B_Help      = 0x0c,     ///< ПОМОЩЬ
    B_Menu      = 0x0d,     ///< МЕНЮ
    B_F1        = 0x0e,     ///< 1
    B_F2        = 0x0f,     ///< 2
    B_F3        = 0x10,     ///< 3
    B_F4        = 0x11,     ///< 4
    B_F5        = 0x12,     ///< 5
    B_NumButtons            ///< общее количество кнопок
} PanelButton;

/// Идентификаторы регуляторов.
typedef enum
{                               //               нажатие - отпускание
    R_Empty         = 0x00,
    R_Range1        = 0x14,     ///< ВОЛЬТ/ДЕЛ кан.1  0x9c - 0x1c
    R_RShift1       = 0x15,     ///<                  0x9d - 0x1d
    R_Range2        = 0x16,     ///< ВОЛЬТ/ДЕЛ кан.2  0x9e - 0x1e
    R_RShift2       = 0x17,     ///<                  0x9f - 0x1f
    R_TBase         = 0x18,     ///< ВРЕМЯ/ДЕЛ        0xa0 - 0x20
    R_TShift        = 0x19,     ///<                  0xa1 - 0x21
    R_TrigLev       = 0x1a,     ///< УРОВЕНЬ          0xa2 - 0x22
    R_Set           = 0x1b,     ///< УСТАНОВКА        0xa3 - 0x23
    R_Range1_BTN    = 0x1c,     ///< Плавное изменение масштаба по напряжению
    R_RShift1_BTN   = 0x1d,     ///< Установка нуля
    R_Range2_BTN    = 0x1e,     ///< Плавное изменение масштаба по наряжению
    R_RShift2_BTN   = 0x1f,     ///< Установка нуля
    R_TBase_BTN     = 0x20,     ///< Включение режима растяжки
    R_TShift_BTN    = 0x21,     ///< Установка нуля
    R_TrigLev_BTN   = 0x22,     ///< Установка нуля синхронизации или автоматической синхронизации
    R_Set_BTN       = 0x23
} Regulator;

/// Команды, принимаемые от панели
typedef enum
{
    C_None = 0,
    C_Reset =  0x24     ///< Команда сброса. Формируется одновременным нажатием нескольких кнопок на клавиатуре (КУРСОРЫ+ДИСПЛЕЙ+ПАМЯТЬ+F4)
} PanelCommand;

/// Направления вращения регуляторов.
typedef enum
{
    Direction_Left,     ///< вращение регулятора влево
    Direction_Right,    ///< вращение регулятора вправо
    Direction_None      ///< вращение отсутствует
} RegulatorDirection;

/// Число ручек
#define NUM_REGULATORS (R_Set - R_Range1 + 1)

/// Возвращает имя кнопки
const char* NameButton(PanelButton button);
/// Возвращает имя ручки
const char* NameRegulator(Regulator regulator);

/** @}
 */
