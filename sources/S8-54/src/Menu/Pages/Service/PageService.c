// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "PageServiceMath.h"
#include "ServiceEthernet.h"
#include "ServiceInformation.h"
#include "ServiceTime.h"
#include "ServiceSound.h"
#include "Display/Display.h"
#include "FPGA/FPGA.h"
#include "Panel/Panel.h"



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;
extern void Func_Start(int key);          // 1 - нажатие, 1 - отпускание


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const  Button bResetSettings;                        ///< СЕРВИС - Сброс настроек
        void  OnPress_ResetSettings(void);
static void      Draw_ResetSettings(void);
static const  Button bAutoSearch;                           ///< СЕРВИС - Поиск сигнала
static void   OnPress_AutoSearch(void);
static const   Page ppCalibrator;                           ///< СЕРВИС - КАЛИБРАТОР
static const  Choice cCalibrator_Calibrator;                ///< СЕРВИС - КАЛИБРАТОР - Калибратор
static void OnChanged_Calibrator_Calibrator(bool active);
static const  Button bCalibrator_Calibrate;                 ///< СЕРВИС - КАЛИБРАТОР - Калибровать
static bool  IsActive_Calibrator_Calibrate(void);
static void   OnPress_Calibrator_Calibrate(void);
static const  Choice cRecorder;
static void OnChanged_Recorder(bool active);
static const   Page ppFFT;                                  ///< СЕРВИС - СПЕКТР
static bool  IsActive_FFT(void);
static void   OnPress_FFT(void);
static const  Choice cFFT_View;                             ///< СЕРВИС - СПЕКТР - Отображение
static const  Choice cFFT_Scale;                            ///< СЕРВИС - СПЕКТР - Шкала
static const  Choice cFFT_Source;                           ///< СЕРВИС - СПЕКТР - Источник
static const  Choice cFFT_Window;                           ///< СЕРВИС - СПЕКТР - Окно
static const  Page pppFFT_Cursors;                          ///< СЕРВИС - СПЕКТР - КУРСОРЫ
static bool  IsActive_FFT_Cursors(void);
static void  OnRegSet_FFT_Cursors(int angle);
static const SButton bFFT_Cursors_Exit;                     ///< СЕРВИС - СПЕКТР - КУРСОРЫ - Выход
static void   OnPress_FFT_Cursors_Exit(void);
static const SButton bFFT_Cursors_Source;                   ///< СЕРВИС - СПЕКТР - КУРСОРЫ - Источник
static void   OnPress_FFT_Cursors_Source(void);
static void      Draw_FFT_Cursors_Source(int x, int y);


static const  Choice cLanguage;


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
        (void*)&bResetSettings,     // СЕРВИС - Сброс настроек
        (void*)&bAutoSearch,        // СЕРВИС - Поиск сигнала
        (void*)&ppCalibrator,       // СЕРВИС - КАЛИБРАТОР
        (void*)&cRecorder,          // СЕРВИС - Регистратор
        (void*)&ppFFT,              // СЕРВИС - СПЕКТР
        (void*)&mspMathFunction,    // СЕРВИС - ФУНКЦИЯ
        (void*)&mspEthernet,        // СЕРВИС - ETHERNET
        (void*)&mspSound,           // СЕРВИС - ЗВУК
        (void*)&mspTime,            // СЕРВИС - ВРЕМЯ
        (void*)&cLanguage,          // СЕРВИС - Язык
        (void*)&mspInformation      // СЕРВИС - ИНФОРМАЦИЯ
    }
};

// СЕРВИС - Сброс настроек ---------------------------------------------------------------------------------------------------------------------------
static const Button bResetSettings =
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
    Display_SetDrawMode(DrawMode_Hand, Draw_ResetSettings);

    if (Panel_WaitPressingButton() == B_Start)
    {
        Settings_Load(true);
    }

    Display_SetDrawMode(DrawMode_Auto, 0);
    Panel_Enable();
    Func_Start(1);
}

static void Draw_ResetSettings(void)
{
    Painter_BeginScene(gColorBack);

    Painter_DrawTextInRectWithTransfersC(30, 110, 300, 200, "Подтвердите сброс настроек нажатием кнопки ПУСК/СТОП.\n"
                                                         "Нажмите любую другую кнопку, если сброс не нужен.", gColorFill);

    Painter_EndScene();
}

// СЕРВИС - Поиск сигнала ----------------------------------------------------------------------------------------------------------------------------
static const Button bAutoSearch =
{
    Item_Button, &pService, 0,
    {
        "Поиск сигнала", "Find signal",
        "Устанавливает оптимальные установки осциллографа для сигнала в канале 1",
        "Sets optimal settings for the oscilloscope signal on channel 1"
    },
    OnPress_AutoSearch
};

static void OnPress_AutoSearch(void)
{
    gBF.FPGAneedAutoFind = 1;
};


// СЕРВИС - КАЛИБРАТОР ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppCalibrator =
{
    Item_Page, &pService, 0,
    {
        "КАЛИБРАТОР", "CALIBRATOR",
        "Управлением калибратором и калибровка осциллографа",
        "Control of the calibrator and calibration of an oscillograph"
    },
    Page_ServiceCalibrator,
    {
        (void*)&cCalibrator_Calibrator,     // СЕРВИС - КАЛИБРАТОР - Калибратор
        (void*)&bCalibrator_Calibrate       // СЕРВИС - КАЛИБРАТОР - Калибровать
    }
};


// СЕРВИС - КАЛИБРАТОР - Калибратор ------------------------------------------------------------------------------------------------------------------
static const Choice cCalibrator_Calibrator =
{
    Item_Choice, &ppCalibrator, 0,
    {
        "Калибратор",   "Calibrator",
        "Режим работы калибратора",
        "Mode of operation of the calibrator"
    },
    {
        {"Перем",       "DC"},
        {"+4V",         "+4V"},
        {"0V",          "0V"}
    },
    (int8*)&CALIBRATOR_MODE, OnChanged_Calibrator_Calibrator
};

static void OnChanged_Calibrator_Calibrator(bool active)
{
    FPGA_SetCalibratorMode(CALIBRATOR_MODE);
}

// СЕРВИС - КАЛИБРАТОР - Калибровать -----------------------------------------------------------------------------------------------------------------
static const Button bCalibrator_Calibrate =
{
    Item_Button, &ppCalibrator, IsActive_Calibrator_Calibrate,
    {
        "Калибровать", "Calibrate",
        "Запуск процедуры калибровки",
        "Running the calibration procedure"
    },
    OnPress_Calibrator_Calibrate, EmptyFuncVII
};

static bool IsActive_Calibrator_Calibrate(void)
{
    return !(SET_CALIBR_MODE_A == CalibrationMode_Disable && CALIBR_MODE_B == CalibrationMode_Disable);
}

static void OnPress_Calibrator_Calibrate(void)
{
    gStateFPGA.needCalibration = true;
}

// СЕРВИС - Регистратор ------------------------------------------------------------------------------------------------------------------------------
static const Choice cRecorder =
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
    (int8*)&RECORDER_MODE, OnChanged_Recorder
};

static void OnChanged_Recorder(bool active)
{
    FPGA_EnableRecorderMode(RECORDER_MODE);
}


// СЕРВИС - СПЕКТР ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppFFT =
{
    Item_Page, &pService, IsActive_FFT,
    {
        "СПЕКТР", "SPECTRUM",
        "Отображение спектра входного сигнала",
        "Mapping the input signal spectrum"
    },
    Page_MathFFT,
    {
        (void*)&cFFT_View,      // СЕРВИС - СПЕКТР - Отображение
        (void*)&cFFT_Scale,     // СЕРВИС - СПЕКТР - Шкала
        (void*)&cFFT_Source,    // СЕРВИС - СПЕКТР - Источник
        (void*)&cFFT_Window,    // СЕРВИС - СПЕКТР - Окно 
        (void*)&pppFFT_Cursors, // СЕРВИС - СПЕКТР - КУРСОРЫ
        (void*)&mcFFTrange
    },
    OnPress_FFT
};

static bool IsActive_FFT(void)
{
    return !FUNC_ENABLED;
}

static void OnPress_FFT(void)
{
    if (!IsActive_FFT())
    {
        Display_ShowWarning(ImpossibleEnableFFT);
    }
}

// СЕРВИС - СПЕКТР - Отображение ---------------------------------------------------------------------------------------------------------------------
static const Choice cFFT_View =
{
    Item_Choice, &ppFFT, 0,
    {
        "Отображение", "Display",
        "Включает и выключает отображение спектра",
        "Enables or disables the display of the spectrum"
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&FFT_ENABLED
};


// СЕРВИС - СПЕКТР - Шкала ---------------------------------------------------------------------------------------------------------------------------
static const Choice cFFT_Scale =
{
    Item_Choice, &ppFFT, 0,
    {
        "Шкала",        "Scale",
        "Задаёт масштаб вывода спектра - линейный или логарифмический",
        "Sets the scale of the output spectrum - linear or logarithmic"
    },
    {
        {"Логарифм",    "Log"},
        {"Линейная",    "Linear"}
    },
    (int8*)&SCALE_FFT
};

// СЕРВИС - СПЕКТР - Источник ------------------------------------------------------------------------------------------------------------------------
static const Choice cFFT_Source =
{
    Item_Choice, &ppFFT, 0,
    {
        "Источник",     "Source",
        "Выбор источника для расчёта спектра",
        "Selecting the source for the calculation of the spectrum"
    },
    {
        {"Канал 1",     "Channel 1"},
        {"Канал 2",     "Channel 2"},
        {"Канал 1 + 2", "Channel 1 + 2"}
    },
    (int8*)&SOURCE_FFT
};

// СЕРВИС - СПЕКТР - Окно ----------------------------------------------------------------------------------------------------------------------------
static const Choice cFFT_Window =
{
    Item_Choice, &ppFFT, 0,
    {
        "Окно",         "Window",
        "Задаёт окно для расчёта спектра",
        "Sets the window to calculate the spectrum"
    },
    {
        {"Прямоугольн", "Rectangle"},
        {"Хэмминга",    "Hamming"},
        {"Блэкмена",    "Blackman"},
        {"Ханна",       "Hann"}
    },
    (int8*)&WINDOW_FFT
};

// СЕРВИС - СПЕКТР - КУРСОРЫ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page pppFFT_Cursors =
{
    Item_Page, &ppFFT, IsActive_FFT_Cursors,
    {
        "КУРСОРЫ", "CURSORS",
        "Включает курсоры для измерения параметров спектра",
        "Includes cursors to measure the parameters of the spectrum"
    },
    Page_SB_MathCursorsFFT,
    {
        (void*)&bFFT_Cursors_Exit,      // СЕРВИС - СПЕКТР - КУРСОРЫ - Выход
        (void*)&bFFT_Cursors_Source,    // СЕРВИС - СПЕКТР - КУРСОРЫ - Источник
        (void*)0,
        (void*)0,
        (void*)0,
        (void*)0
    },
    0, 0, OnRegSet_FFT_Cursors
};

static bool IsActive_FFT_Cursors(void)
{
    return FFT_ENABLED;
}

static void OnRegSet_FFT_Cursors(int angle)
{
    POS_MATH_CUR(MATH_CURRENT_CUR) += (uint8)angle;
    Sound_RegulatorShiftRotate();
}


// СЕРВИС - СПЕКТР - КУРСОРЫ - Выход -----------------------------------------------------------------------------------------------------------------
static const SButton bFFT_Cursors_Exit =
{
    Item_SmallButton, &pppFFT_Cursors,
    COMMON_BEGIN_SB_EXIT,
    OnPress_FFT_Cursors_Exit,
    DrawSB_Exit
};

static void OnPress_FFT_Cursors_Exit(void)
{
    Display_RemoveAddDrawFunction();
}

// СЕРВИС - СПЕКТР - КУРСОРЫ - Источник --------------------------------------------------------------------------------------------------------------
static const SButton bFFT_Cursors_Source =
{
    Item_SmallButton, &pppFFT_Cursors, 0,
    {
        "Источник", "Source",
        "Выбор источника для расчёта спектра",
        "Source choice for calculation of a range"
    },
    OnPress_FFT_Cursors_Source,
    Draw_FFT_Cursors_Source
};

static void OnPress_FFT_Cursors_Source(void)
{
    MATH_CURRENT_CUR = (MATH_CURRENT_CUR + 1) % 2;
}

static void Draw_FFT_Cursors_Source(int x, int y)
{
    Painter_DrawText(x + 7, y + 5, MATH_CURRENT_CUR_IS_0 ? "1" : "2");
}






























// СЕРВИС - Язык -------------------------------------------------------------------------------------------------------------------------------------
static const Choice cLanguage =
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
