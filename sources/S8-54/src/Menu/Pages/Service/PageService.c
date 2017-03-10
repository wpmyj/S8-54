#include "defines.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Display/Colors.h"
#include "FPGA/FPGA.h"
#include "Hardware/Timer.h"
#include "Menu/Menu.h"
#include "Menu/MenuDrawing.h"
#include "Menu/MenuFunctions.h"
#include "PageServiceMath.h"
#include "Panel/Panel.h"
#include "Settings/Settings.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "ServiceInformation.h"
#include "ServiceTime.h"
#include "ServiceCalibrator.h"


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

static const Page mspEthernet;
static const Page mspSound;


// СЕРВИС ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpService =
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
    Item_Button, &mpService, 0,
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
    Item_Button, &mpService, 0,
    {
        "Поиск сигнала", "Find signal",
        "Устанавливает оптимальные установки осциллографа для сигнала в канале 1",
        "Sets optimal settings for the oscilloscope signal on channel 1"
    },
    OnPress_AutoSearch
};

static void OnPress_AutoSearch(void)
{
    FPGA_StartAutoFind();
};

// СЕРВИС - Регистратор ------------------------------------------------------------------------------------------------------------------------------
static const Choice mcRecorder =
{
    Item_Choice, &mpService, 0,
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
    Item_Choice, &mpService, 0,
    {
        "Язык",         "Language",
        "Позволяет выбрать язык меню",
        "Allows you to select the menu language"
    },
    {
        {"Русский",     "Russian"},
        {"Английский",  "English"}
    },
    (int8*)&set.common.lang
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
extern const Page mpService;

// СЕРВИС - Откат настроек
const Button mbServicePreviousSettings =
{
    Item_Button, &mpService, 0,
    {
        "Откат настроек", "Return settings",
        "Возвращает настройки осциллографа, которые были в момент нажатия \"Поиск сигнала\"",
        "Returns the oscilloscope settings that were in when you press \"Searching for signal\""
    },
    OnPressPrevSettings
};

extern const Page mspCalibrator;
extern const Page mspFreqMeter;
extern const Page mspSound;
extern const Page mspEthernet;

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncOfChangedEthernetSettings(bool active)
{
    Display_ShowWarning(NeedRebootDevice);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcServEthEnable =
{
    Item_Choice, &mspEthernet, 0,
    {
        "Ethernet",    "Ethernet"
        ,
        "Чтобы задействовать ethernet, выберите \"Включено\" и выключите прибор.\n"
        "Чтобы отключить ethernet, выберите \"Отключено\" и выключите прибор."
        ,
        "To involve ethernet, choose \"Included\" and switch off the device.\n"
        "To disconnect ethernet, choose \"Disconnected\" and switch off the device."
    },
    {
        {"Включено",    "Included"},
        {"Отключено",   "Disconnected"}
    },
    (int8*)&set.eth.enable, FuncOfChangedEthernetSettings
};


static const IPaddress ipAddress =
{
    Item_IP, &mspEthernet, 0,
    {
        "IP адрес", "IP-address",
        "Установка IP адреса",
        "Set of IP-address"
    },
    &set.eth.ip0, &set.eth.ip1, &set.eth.ip2, &set.eth.ip3,
    FuncOfChangedEthernetSettings,
    &set.eth.port
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const IPaddress ipNetMask =
{
    Item_IP, &mspEthernet, 0,
    {
        "Маска подсети", "Network mask",
        "Установка маски подсети",
        "Set of network mask"
    },
    &set.eth.mask0, &set.eth.mask1, &set.eth.mask2, &set.eth.mask3,
    FuncOfChangedEthernetSettings
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const IPaddress ipGateway =
{
    Item_IP, &mspEthernet, 0,
    {
        "Шлюз", "Gateway",
        "Установка адреса основного шлюза",
        "Set of gateway address"
    },
    &set.eth.gw0, &set.eth.gw1, &set.eth.gw2, &set.eth.gw3,
    FuncOfChangedEthernetSettings
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const MACaddress macMAC =
{
    Item_MAC, &mspEthernet, 0,
    {
        "Физ адрес", "MAC-address",
        "Установка физического адреса",
        "Set of MAC-address"
    },
    &set.eth.mac0, &set.eth.mac1, &set.eth.mac2, &set.eth.mac3, &set.eth.mac4, &set.eth.mac5,
    FuncOfChangedEthernetSettings
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
// СЕРВИС - ЗВУК - Звук
const Page mspSound;

const Choice mcServiceSoundEnable =
{
    Item_Choice, &mspSound, 0,
    {
        "Звук", "Sound",
        "Включение/выключение звука",
        "Inclusion/switching off of a sound"
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.service.soundEnable
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const Governor mgServiceSoundVolume =
{
    Item_Governor, &mspSound, 0,
    {
        "Громкость", "Volume",
        "Установка громкости звука",
        "Set the volume"
    },
    &set.service.soundVolume, 0, 100, EmptyFuncVV
};

// СЕРВИС - ЗВУК /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspSound =
{
    Item_Page, &mpService, 0,
    {
        "ЗВУК", "SOUND",
        "В этом меню можно настроить громкость звука",
        "In this menu, you can adjust the volume"
    },
    Page_ServiceSound,
    {
        (void*)&mcServiceSoundEnable,
        (void*)&mgServiceSoundVolume
    }

};

// СЕРВИС - ETHERNET  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspEthernet = 
{
    Item_Page, &mpService, 0,
    {
        "ETHERNET", "ETHERNET",
        "Настройки ethernet",
        "Settings of ethernet"
    },
    Page_ServiceEthernet,
    {
        (void*)&mcServEthEnable,
        (void*)&ipAddress,
        (void*)&ipNetMask,
        (void*)&ipGateway,
        (void*)&macMAC
    }
};
