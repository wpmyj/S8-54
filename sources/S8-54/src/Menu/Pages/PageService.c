#include "defines.h"
#include "Hardware/RTC.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Menu/Menu.h"
#include "defines.h"
#include "Menu/MenuFunctions.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Panel/Panel.h"
#include "Display/Colors.h"
#include "PageServiceMath.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;
extern void FuncBtnStart(int key);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Button mbResetSettings;
        void OnPress_ResetSettings(void);
static void  FuncDraw_ResetSettings(void);
static const Button mbAutoSearch;
static void  OnPress_AutoSearch(void);
static const Choice mcRecorder;
static void  OnChange_Recorder(bool active);
static const Choice mcLanguage;

static const Page mspCalibrator;
static const Choice mcCalibrator;
static void  OnChange_Calibrator(bool active);
static const Button mbCalibrator_Calibrate;
static bool  IsActive_Calibrator_Calibrate(void);
static void  OnPress_Calibrator_Calibrate(void);

static const Page mspEthernet;
static const Page mspSound;
static const Page mspTime;
static const Time mtTime;
static const Governor mgTimeCorrection;
static void  OnChange_Time_Correction(void);
static const Page mspInformation;


// СЕРВИС ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpService =
{
    Item_Page, &mainPage,
    {
        "СЕРВИС", "SERVICE"
    },
    {
        "Дополнительные настройки, калибровка, поиск сигнала, математические функции",
        "Additional settings, calibration, signal search, mathematical functions"
    },
    EmptyFuncBV, Page_Service,
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
    Item_Button, &mpService,
    {
        "Сброс настроек", "Reset settings"
    },
    {
        "Сброс настроек на настройки по умолчанию",
        "Reset to default settings"
    },
    EmptyFuncBV, OnPress_ResetSettings, EmptyFuncVII
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
    FuncBtnStart(1);
}

static void FuncDraw_ResetSettings(void)
{
    Painter_BeginScene(gColorBack);

    Painter_DrawTextInRectWithTransfersC(30, 110, 300, 200, "Подтвердите сброс настроек нажатием кнопки ПУСК/СТОП.\n"
                                         "Нажмите любую другую кнопку, если сброс не нужен.", gColorFill);

    Painter_EndScene();
}

// СЕРВИС - Поиск сигнала ----------------------------------------------------------------------------------------------------------------------------
static const Button mbAutoSearch =
{
    Item_Button, &mpService,
    {
        "Поиск сигнала", "Find signal"
    },
    {
        "Устанавливает оптимальные установки осциллографа для сигнала в канале 1",
        "Sets optimal settings for the oscilloscope signal on channel 1"
    },
    EmptyFuncBV, OnPress_AutoSearch
};

static void OnPress_AutoSearch(void)
{
    FPGA_StartAutoFind();
};

// СЕРВИС - Регистратор ------------------------------------------------------------------------------------------------------------------------------
static const Choice mcRecorder =
{
    Item_Choice, &mpService,
    {
        "Регистратор", "Recorder"
    },
    {
        "Включает/выключает режим регистратора. Этот режим доступен на развёртках 50 мс/дел и более медленных.",
        "Turn on/off recorder mode. This mode is available for scanning 20ms/div and slower."
    },
    EmptyFuncBV,
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
    Item_Choice, &mpService, {"Язык", "Language"},
    {
        "Позволяет выбрать язык меню",
        "Allows you to select the menu language"
    },
    EmptyFuncBV,
    {
        {"Русский",     "Russian"},
        {"Английский",  "English"}
    },
    (int8*)&set.common.lang
};


// ВРЕМЯ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspTime =
{
    Item_Page, &mpService,
    {
        "ВРЕМЯ", "TIME"
    },
    {
        "Установка и настройка времени",
        "Set and setup time"
    },
    EmptyFuncBV, Page_ServiceTime,
    {
        (void*)&mtTime,             // СЕРВИС - ВРЕМЯ - Время
        (void*)&mgTimeCorrection    // CЕРВИС - ВРЕМЯ - Коррекция
    }
};

// СЕРВИС - ВРЕМЯ - Время ----------------------------------------------------------------------------------------------------------------------------
static int8 dServicetime = 0;
static int8 hours = 0, minutes = 0, secondes = 0, year = 0, month = 0, day = 0;
static const Time mtTime =
{
    Item_Time, &mspTime,    
    {
        "Время", "Time"
    },
    {
        "Установка текущего времени.\nПорядок работы:\n"
        "Нажать на элемент меню \"Время\". Откроется меню установки текущего времени. Короткими нажатиями кнопки на цифровой клавиатуре, соответсвующей "
        "элементу управления \"Время\", выделить часы, минуты, секунды, год, месяц, или число. Выделенный элемент обозначается мигающей областью. "
        "Вращением ручки УСТАНОВКА установить необходимое значение. Затем выделить пункт \"Сохранить\", нажать и удреживать более 0.5 сек кнопку на панели "
        "управления. Меню установки текущего временя закроется с сохранением нового текущего времени. Нажатие длительное удержание кнопки на любом другом элементе "
        "приведёт к закрытию меню установки текущего вре    мени без сохранения нового текущего времени",

        "Setting the current time. \nPoryadok work:\n"
        "Click on the menu item \"Time\".The menu set the current time.By briefly pressing the button on the numeric keypad of conformity "
        "Control \"Time\", highlight the hours, minutes, seconds, year, month, or a number.The selected item is indicated by a flashing area. "
        "Turn the setting knob to set the desired value. Then highlight \"Save\", press and udrezhivat more than 0.5 seconds, the button on the panel "
        "Control. Menu Setting the current time will be closed to the conservation of the new current time. Pressing a button on the prolonged retention of any other element "
        "will lead to the closure of the current time setting menu without saving the new current time"
    },
    EmptyFuncBV,
    &dServicetime, &hours, &minutes, &secondes, &month, &day, &year
};


// СЕРВИС - Время - Коррекция ------------------------------------------------------------------------------------------------------------------------
static const Governor mgTimeCorrection =
{
    Item_Governor, &mspTime,
    {
        "Коррекция", "Correction"
    },
    {
        "Установка корректирующего коэффициента для компенсации хода времени",
        "Setting correction factor to compensate for time travel"
    },
    EmptyFuncBV,
    &setNR.correctionTime, -63, 63, OnChange_Time_Correction  
};

static void OnChange_Time_Correction(void)
{
    RTC_SetCorrection((int8)setNR.correctionTime);
}


// СЕРВИС - ИНФОРМАЦИЯ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspInformation =
{
    Item_Page, &mpService,
    {
        "ИНФОРМАЦИЯ", "INFORMATION"
    },
    {
        "Показывает информацию о приборе",
        "Displays information about the device"
    },
    EmptyFuncBV
};


// СЕРВИС - КАЛИБРАТОР ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspCalibrator =
{
    Item_Page, &mpService,
    {
        "КАЛИБРАТОР", "CALIBRATOR"
    },
    {
        "Управлением калибратором и калибровка осциллографа",
        "Control of the calibrator and calibration of an oscillograph"
    },
    EmptyFuncBV, Page_ServiceCalibrator,
    {
        (void*)&mcCalibrator,           // СЕРВИС - КАЛИБРАТОР - Калибратор
        (void*)&mbCalibrator_Calibrate  // СЕРВИС - КАЛИБРАТОР - Калибровать
    }
};

// СЕРВИС - КАЛИБРАТОР - Калибратор ------------------------------------------------------------------------------------------------------------------
static const Choice mcCalibrator =
{
    Item_Choice, &mspCalibrator,
    {
        "Калибратор", "Calibrator"
    },
    {
        "Режим работы калибратора",
        "Mode of operation of the calibrator"
    },
    EmptyFuncBV,
    {
        {"Перем", "DC"},
        {"+4V", "+4V"},
        {"0V", "0V"}
    },
    (int8*)&set.service.calibrator, OnChange_Calibrator
};

static void OnChange_Calibrator(bool active)
{
    FPGA_SetCalibratorMode(set.service.calibrator);
}

// СЕРВИС - КАЛИБРАТОР - Калибровать -----------------------------------------------------------------------------------------------------------------
static const Button mbCalibrator_Calibrate =
{
    Item_Button, &mspCalibrator,
    {
        "Калибровать", "Calibrate"
    },
    {
        "Запуск процедуры калибровки",
        "Running the calibration procedure"
    },
    IsActive_Calibrator_Calibrate, OnPress_Calibrator_Calibrate, EmptyFuncVII
};

static bool IsActive_Calibrator_Calibrate(void)
{
    return !(CALIBR_MODE_A == CalibrationMode_Disable && CALIBR_MODE_B == CalibrationMode_Disable);
}

static void OnPress_Calibrator_Calibrate(void)
{
    gStateFPGA.needCalibration = true;
}
















































//------------------------------------------------------------------------------------------------------------------------------------------------------
void OnPressPrevSettings(void)
{

}


//------------------------------------------------------------------------------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
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


// СЕРВИС //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mpService;

// СЕРВИС - Откат настроек
const Button mbServicePreviousSettings =
{
    Item_Button, &mpService,
    {
        "Откат настроек", "Return settings"
    },
    {
        "Возвращает настройки осциллографа, которые были в момент нажатия \"Поиск сигнала\"",
        "Returns the oscilloscope settings that were in when you press \"Searching for signal\""
    },
    EmptyFuncBV, OnPressPrevSettings, EmptyFuncVII
};

// СЕРВИС - КАЛИБРАТОР ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mspCalibrator;

extern const Page mspFreqMeter;

// СЕРВИС - ЗВУК //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mspSound;


//------------------------------------------------------------------------------------------------------------------------------------------------------
extern const Page mspEthernet;


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncOfChangedEthernetSettings(bool active)
{
    Display_ShowWarning(NeedRebootDevice);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcServEthEnable =
{
    Item_Choice, &mspEthernet, 
    {"Ethernet",    "Ethernet"},
    {
        "Чтобы задействовать ethernet, выберите \"Включено\" и выключите прибор.\n"
        "Чтобы отключить ethernet, выберите \"Отключено\" и выключите прибор."
        ,
        "To involve ethernet, choose \"Included\" and switch off the device.\n"
        "To disconnect ethernet, choose \"Disconnected\" and switch off the device."
    },
    EmptyFuncBV,
    {
        {"Включено",    "Included"},
        {"Отключено",   "Disconnected"}
    },
    (int8*)&set.eth.enable, FuncOfChangedEthernetSettings
};


static const IPaddress ipAddress =
{
    Item_IP, &mspEthernet,
    {"IP адрес", "IP-address"},
    {
        "Установка IP адреса",
        "Set of IP-address"
    },
    EmptyFuncBV,
    &set.eth.ip0, &set.eth.ip1, &set.eth.ip2, &set.eth.ip3,
    FuncOfChangedEthernetSettings,
    &set.eth.port
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
static const IPaddress ipNetMask =
{
    Item_IP, &mspEthernet,
    {"Маска подсети", "Network mask"},
    {
        "Установка маски подсети",
        "Set of network mask"
    },
    EmptyFuncBV,
    &set.eth.mask0, &set.eth.mask1, &set.eth.mask2, &set.eth.mask3,
    FuncOfChangedEthernetSettings
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
static const IPaddress ipGateway =
{
    Item_IP, &mspEthernet,
    {"Шлюз", "Gateway"},
    {
        "Установка адреса основного шлюза",
        "Set of gateway address"
    },
    EmptyFuncBV,
    &set.eth.gw0, &set.eth.gw1, &set.eth.gw2, &set.eth.gw3,
    FuncOfChangedEthernetSettings
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
static const MACaddress macMAC =
{
    Item_MAC, &mspEthernet,
    {"Физ адрес", "MAC-address"},
    {
        "Установка физического адреса",
        "Set of MAC-address"
    },
    EmptyFuncBV,
    &set.eth.mac0, &set.eth.mac1, &set.eth.mac2, &set.eth.mac3, &set.eth.mac4, &set.eth.mac5,
    FuncOfChangedEthernetSettings
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
// СЕРВИС - ЗВУК - Звук
const Page mspSound;

const Choice mcServiceSoundEnable =
{
    Item_Choice, &mspSound, {"Звук", "Sound"},
    {
        "Включение/выключение звука",
        "Inclusion/switching off of a sound"
    },
    EmptyFuncBV,
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.service.soundEnable
};


//---------------------------------------------------------------------------------------------------------------------------------------------------
const Governor mgServiceSoundVolume =
{
    Item_Governor, &mspSound,
    {
        "Громкость", "Volume"
    },
    {
        "Установка громкости звука",
        "Set the volume"
    },
    EmptyFuncBV,
    &set.service.soundVolume, 0, 100, EmptyFuncVV
};


// СЕРВИС - ЗВУК /////////////////////////////////////////////////////////////////////////////////////////////
const Page mspSound =
{
    Item_Page, &mpService,
    {
        "ЗВУК", "SOUND"
    },
    {
        "В этом меню можно настроить громкость звука",
        "In this menu, you can adjust the volume"
    },
    EmptyFuncBV, Page_ServiceSound,
    {
        (void*)&mcServiceSoundEnable,
        (void*)&mgServiceSoundVolume
    }

};


// СЕРВИС - ETHERNET  /////////////////////////////////////////////////////////////////////////////////////////
const Page mspEthernet = 
{
    Item_Page, &mpService,
    {
        "ETHERNET", "ETHERNET"
    },
    {
        "Настройки ethernet",
        "Settings of ethernet"
    },
    EmptyFuncBV, Page_ServiceEthernet,
    {
        (void*)&mcServEthEnable,
        (void*)&ipAddress,
        (void*)&ipNetMask,
        (void*)&ipGateway,
        (void*)&macMAC
    }
};
