#pragma once
#include "Hardware/RTC.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspTime;


// ВРЕМЯ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int8 dServicetime = 0;
static int8 hours = 0, minutes = 0, secondes = 0, year = 0, month = 0, day = 0;
static const Time mtTime =
{
    Item_Time, &mspTime, 0,
    {
        "Время", "Time"
        ,
        "Установка текущего времени.\nПорядок работы:\n"
        "Нажать на элемент меню \"Время\". Откроется меню установки текущего времени. Короткими нажатиями кнопки на цифровой клавиатуре, соответсвующей "
        "элементу управления \"Время\", выделить часы, минуты, секунды, год, месяц, или число. Выделенный элемент обозначается мигающей областью. "
        "Вращением ручки УСТАНОВКА установить необходимое значение. Затем выделить пункт \"Сохранить\", нажать и удреживать более 0.5 сек кнопку на панели "
        "управления. Меню установки текущего временя закроется с сохранением нового текущего времени. Нажатие длительное удержание кнопки на любом другом элементе "
        "приведёт к закрытию меню установки текущего вре    мени без сохранения нового текущего времени"
        ,
        "Setting the current time. \nPoryadok work:\n"
        "Click on the menu item \"Time\".The menu set the current time.By briefly pressing the button on the numeric keypad of conformity "
        "Control \"Time\", highlight the hours, minutes, seconds, year, month, or a number.The selected item is indicated by a flashing area. "
        "Turn the setting knob to set the desired value. Then highlight \"Save\", press and udrezhivat more than 0.5 seconds, the button on the panel "
        "Control. Menu Setting the current time will be closed to the conservation of the new current time. Pressing a button on the prolonged retention of any other element "
        "will lead to the closure of the current time setting menu without saving the new current time"
    },
    &dServicetime, &hours, &minutes, &secondes, &month, &day, &year
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChange_Time_Correction(void)
{
    RTC_SetCorrection((int8)NRST_CORRECTION_TIME);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Governor mgTimeCorrection =
{
    Item_Governor, &mspTime, 0,
    {
        "Коррекция", "Correction",
        "Установка корректирующего коэффициента для компенсации хода времени",
        "Setting correction factor to compensate for time travel"
    },
    &NRST_CORRECTION_TIME, -63, 63, OnChange_Time_Correction
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Page mspTime =
{
    Item_Page, &mpService, 0,
    {
        "ВРЕМЯ", "TIME",
        "Установка и настройка времени",
        "Set and setup time"
    },
    Page_ServiceTime,
    {
        (void*)&mtTime,             // СЕРВИС - ВРЕМЯ - Время
        (void*)&mgTimeCorrection    // CЕРВИС - ВРЕМЯ - Коррекция
    }
};
