// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "PageServiceMath.h"
#include "ServiceCalibrator.h"
#include "ServiceEthernet.h"
#include "ServiceInformation.h"
#include "ServiceTime.h"
#include "ServiceSound.h"
#include "Display/Display.h"
#include "FPGA/FPGA.h"
#include "Panel/Panel.h"



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;
extern void Func_Start(int key);          // 1 - нажатие, -1 - отпускание


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Button mbResetSettings;
        void OnPress_ResetSettings(void);
static void  FuncDraw_ResetSettings(void);
static const Choice mcRecorder;
static void  OnChange_Recorder(bool active);
static const Choice mcLanguage;


// СЕРВИС - Поиск сигнала ----------------------------------------------------------------------------------------------------------------------------
static void OnPress_AutoSearch(void)
{
    gBF.FPGAneedAutoFind = 1;
};

static const Button mbAutoSearch =
{
    Item_Button, &pService, 0,
    {
        "Поиск сигнала", "Find signal",
        "Устанавливает оптимальные установки осциллографа для сигнала в канале 1",
        "Sets optimal settings for the oscilloscope signal on channel 1"
    },
    OnPress_AutoSearch
};

// СЕРВИС ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page pService =
{
    Item_Page, &mainPage, 0,
    {
        "СЕРВИС", "SERVICE",
        "Дополнительные настройки, калибровка, поиск сигнала, математические функции",
        "Additional settings, calibration, signal search, mathematical functions"
    },
    Page_Service,
    {
        (void*)&mbResetSettings,    // СЕРВИС - Сброс настроек
        (void*)&mbAutoSearch,       // СЕРВИС - Поиск сигнала
        (void*)&mspCalibrator,      // СЕРВИС - КАЛИБРАТОР
        (void*)&mcRecorder,         // СЕРВИС - Регистратор
        (void*)&mspFFT,             // СЕРВИС - СПЕКТР
        (void*)&mspMathFunction,    // СЕРВИС - ФУНКЦИЯ
        (void*)&mspEthernet,        // СЕРВИС - ETHERNET
        (void*)&mspSound,           // СЕРВИС - ЗВУК
        (void*)&mspTime,            // СЕРВИС - ВРЕМЯ
        (void*)&mcLanguage,         // СЕРВИС - Язык
        (void*)&mspInformation      // СЕРВИС - ИНФОРМАЦИЯ
    }
};

// СЕРВИС - Сброс настроек ---------------------------------------------------------------------------------------------------------------------------
static const Button mbResetSettings =
{
    Item_Button, &pService, 0,
    {
        "Сброс настроек", "Reset settings",
        "Сброс настроек на настройки по умолчанию",
        "Reset to default settings"
    },
    OnPress_ResetSettings
};

void OnPress_ResetSettings(void)
{
    Panel_Disable();
    Display_SetDrawMode(DrawMode_Hand, FuncDraw_ResetSettings);

    if (Panel_WaitPressingButton() == B_Start)
    {
        Settings_Load(true);
    }

    Display_SetDrawMode(DrawMode_Auto, 0);
    Panel_Enable();
    Func_Start(1);
}

static void FuncDraw_ResetSettings(void)
{
    Painter_BeginScene(gColorBack);

    Painter_DrawTextInRectWithTransfersC(30, 110, 300, 200, "Подтвердите сброс настроек нажатием кнопки ПУСК/СТОП.\n"
                                         "Нажмите любую другую кнопку, если сброс не нужен.", gColorFill);

    Painter_EndScene();
}

// СЕРВИС - Регистратор ------------------------------------------------------------------------------------------------------------------------------
static const Choice mcRecorder =
{
    Item_Choice, &pService, 0,
    {
        "Регистратор", "Recorder",
        "Включает/выключает режим регистратора. Этот режим доступен на развёртках 50 мс/дел и более медленных.",
        "Turn on/off recorder mode. This mode is available for scanning 20ms/div and slower."
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&RECORDER_MODE, OnChange_Recorder
};

static void OnChange_Recorder(bool active)
{
    FPGA_EnableRecorderMode(RECORDER_MODE);
}

// СЕРВИС - Язык -------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcLanguage =
{
    Item_Choice, &pService, 0,
    {
        "Язык",         "Language",
        "Позволяет выбрать язык меню",
        "Allows you to select the menu language"
    },
    {
        {"Русский",     "Russian"},
        {"Английский",  "English"}
    },
    (int8*)&LANG
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnPressPrevSettings(void)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnChangedColorMenuTitle(void)
{
    uint16 newColor1 = Painter_ReduceBrightness(COLOR(COLOR_MENU_TITLE), 0.50f);
    uint16 newColor2 = Painter_ReduceBrightness(COLOR(COLOR_MENU_TITLE), 1.50f);
    COLOR(COLOR_MENU_TITLE_DARK) = newColor1;
    COLOR(COLOR_MENU_TITLE_BRIGHT) = newColor2;
    Color_Log(COLOR_MENU_TITLE);
    Color_Log(COLOR_MENU_TITLE_DARK);
    Color_Log(COLOR_MENU_TITLE_BRIGHT);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnChangedColorMenu2Item(void)
{
    uint16 newColor1 = Painter_ReduceBrightness(COLOR(COLOR_MENU_ITEM), 0.50f);
    uint16 newColor2 = Painter_ReduceBrightness(COLOR(COLOR_MENU_ITEM), 1.50f);
    COLOR(COLOR_MENU_ITEM_DARK) = newColor1;
    COLOR(COLOR_MENU_ITEM_BRIGHT) = newColor2;
    Color_Log(COLOR_MENU_ITEM);
    Color_Log(COLOR_MENU_ITEM_DARK);
    Color_Log(COLOR_MENU_ITEM_BRIGHT);
}


// СЕРВИС ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page pService;

// СЕРВИС - Откат настроек
const Button mbServicePreviousSettings =
{
    Item_Button, &pService, 0,
    {
        "Откат настроек", "Return settings",
        "Возвращает настройки осциллографа, которые были в момент нажатия \"Поиск сигнала\"",
        "Returns the oscilloscope settings that were in when you press \"Searching for signal\""
    },
    OnPressPrevSettings
};
