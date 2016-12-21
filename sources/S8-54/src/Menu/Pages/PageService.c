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


extern Page mainPage;
extern void FuncBtnStart(int key);


#ifdef _MS_VS
#pragma warning(push)
#pragma warning(disable:4132)
#endif

static const Button mbResetSettings;
static const Button mbAutoSearch;
static const Choice mcRecorder;
static const Choice mcLanguage;

static const Page mspCalibrator;
static const Choice mcCalibrator;
static const Button mbCalibrator_Calibrate;

static const Page mspServiceMath;
static const Page mspServiceEthernet;
static const Page mspServiceSound;
static const Page mspTime;
static const Time mtTime;
static const Governor mgTimeCorrection;
static void OnChange_Time_Correction(void);

#ifdef _MS_VS
#pragma warning(pop)
#endif

       void OnPress_ResetSettings(void);
static void OnPress_AutoSearch(void);
static void FuncDraw_ResetSettings(void);
static void OnTimerDraw_ResetSettings(void);
static void OnChange_Recorder(bool active);
static void OnChange_Calibrator(bool active);
static bool IsActive_Calibrator_Calibrate(void);
static void OnPress_Calibrator_Calibrate(void);


// ������ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpService =
{
    Item_Page, &mainPage,
    {
        "������", "SERVICE"
    },
    {
        "�������������� ���������, ����������, ����� �������, �������������� �������",
        "Additional settings, calibration, signal search, mathematical functions"
    },
    EmptyFuncBV, Page_Service,
    {
        (void*)&mbResetSettings,            // ������ -> ����� ��������
        (void*)&mbAutoSearch,               // ������ -> ����� �������
        (void*)&mspCalibrator,              // ������ -> ����������
        (void*)&mspServiceMath,
        (void*)&mspServiceEthernet,
        (void*)&mspServiceSound,
        (void*)&mspTime,                    // ������ -> �����
        (void*)&mcRecorder,                 // ������ -> �����������
        (void*)&mcLanguage                  // ������ -> ����
    }
};

// ������ -> ����� �������� --------------------------------------------------------------------------------------------------------------------------------
static const Button mbResetSettings =
{
    Item_Button, &mpService,
    {
        "����� ��������", "Reset settings"
    },
    {
        "����� �������� �� ��������� �� ���������",
        "Reset to default settings"
    },
    EmptyFuncBV, OnPress_ResetSettings, EmptyFuncVII
};

void OnPress_ResetSettings(void)
{
    Panel_Disable();
    Display_SetDrawMode(DrawMode_Hand, FuncDraw_ResetSettings);
    Timer_Enable(kTimerDrawHandFunction, 100, OnTimerDraw_ResetSettings);

    if (Panel_WaitPressingButton() == B_Start)
    {
        Settings_Load(true);
    }

    Timer_Disable(kTimerDrawHandFunction);
    Display_SetDrawMode(DrawMode_Auto, 0);
    Panel_Enable();
    FuncBtnStart(1);
}

static void FuncDraw_ResetSettings(void)
{
    Painter_BeginScene(gColorBack);

    Painter_DrawTextInRectWithTransfersC(30, 110, 300, 200, "����������� ����� �������� �������� ������ ����/����.\n"
                                         "������� ����� ������ ������, ���� ����� �� �����.", gColorFill);

    Painter_EndScene();
}

static void OnTimerDraw_ResetSettings(void)
{
    Display_Update();
}

// ������ -> ����� ������� ----------------------------------------------------------------------------------------------------------------------------------------
static const Button mbAutoSearch =
{
    Item_Button, &mpService,
    {
        "����� �������", "Find signal"
    },
    {
        "������������� ����������� ��������� ������������ ��� ������� � ������ 1",
        "Sets optimal settings for the oscilloscope signal on channel 1"
    },
    EmptyFuncBV, OnPress_AutoSearch
};

static void OnPress_AutoSearch(void)
{
    FPGA_StartAutoFind();
};

// ������ -> ����������� ---------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcRecorder =
{
    Item_Choice, &mpService,
    {
        "�����������", "Recorder"
    },
    {
        "��������/��������� ����� ������������. ���� ����� �������� �� ��������� 50 ��/��� � ����� ���������.",
        "Turn on/off recorder mode. This mode is available for scanning 20ms/div and slower."
    },
    EmptyFuncBV,
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.service.recorder, OnChange_Recorder, EmptyFuncVII
};

static void OnChange_Recorder(bool active)
{
    FPGA_EnableRecorderMode(set.service.recorder);
}

// ������ -> ���� -------------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcLanguage =
{
    Item_Choice, &mpService, {"����", "Language"},
    {
        "��������� ������� ���� ����",
        "Allows you to select the menu language"
    },
    EmptyFuncBV,
    {
        {"�������",     "Russian"},
        {"����������",  "English"}
    },
    (int8*)&set.common.lang, EmptyFuncVB, EmptyFuncVII
};


// ����� ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspTime =
{
    Item_Page, &mpService,
    {
        "�����", "TIME"
    },
    {
        "��������� � ��������� �������",
        "Set and setup time"
    },
    EmptyFuncBV, Page_ServiceTime,
    {
        (void*)&mtTime,             // ������ -> ����� -> �����
        (void*)&mgTimeCorrection    // C����� -> ����� -> ���������
    }
};

// ������ -> ����� -> ����� --------------------------------------------------------------------------------------------------------------------------------------------------------
static int8 dServicetime = 0;
static int8 hours = 0, minutes = 0, secondes = 0, year = 0, month = 0, day = 0;
static const Time mtTime =
{
    Item_Time, &mspTime,    
    {
        "�����", "Time"
    },
    {
        "��������� �������� �������.\n������� ������:\n"
        "������ �� ������� ���� \"�����\". ��������� ���� ��������� �������� �������. ��������� ��������� ������ �� �������� ����������, �������������� "
        "�������� ���������� \"�����\", �������� ����, ������, �������, ���, �����, ��� �����. ���������� ������� ������������ �������� ��������. "
        "��������� ����� ��������� ���������� ����������� ��������. ����� �������� ����� \"���������\", ������ � ���������� ����� 0.5 ��� ������ �� ������ "
        "����������. ���� ��������� �������� ������� ��������� � ����������� ������ �������� �������. ������� ���������� ��������� ������ �� ����� ������ �������� "
        "������� � �������� ���� ��������� �������� ���    ���� ��� ���������� ������ �������� �������",

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


// ������ -> ����� -> ��������� ---------------------------------------------------------------------------------------------------------------------------------------------------
static const Governor mgTimeCorrection =
{
    Item_Governor, &mspTime,
    {
        "���������", "Correction"
    },
    {
        "��������� ��������������� ������������ ��� ����������� ���� �������",
        "Setting correction factor to compensate for time travel"
    },
    EmptyFuncBV,
    &set.nr.correctionTime, -63, 63, OnChange_Time_Correction  
};

static void OnChange_Time_Correction(void)
{
    RTC_SetCorrection((int8)set.nr.correctionTime);
}


// ������ -> ���������� /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspCalibrator =
{
    Item_Page, &mpService,
    {
        "����������", "CALIBRATOR"
    },
    {
        "����������� ������������ � ���������� ������������",
        "Control of the calibrator and calibration of an oscillograph"
    },
    EmptyFuncBV, Page_ServiceCalibrator,
    {
        (void*)&mcCalibrator,           // ������ -> ���������� -> ����������
        (void*)&mbCalibrator_Calibrate  // ������ -> ���������� -> �����������
    }
};

// ������ -> ���������� -> ���������� -------------------------------------------------------------------------------------------------------------------------------
static const Choice mcCalibrator =
{
    Item_Choice, &mspCalibrator,
    {
        "����������", "Calibrator"
    },
    {
        "����� ������ �����������",
        "Mode of operation of the calibrator"
    },
    EmptyFuncBV,
    {
        {"�����", "DC"},
        {"+4V", "+4V"},
        {"0V", "0V"}
    },
    (int8*)&set.service.calibrator, OnChange_Calibrator, EmptyFuncVII
};

static void OnChange_Calibrator(bool active)
{
    FPGA_SetCalibratorMode(set.service.calibrator);
}

// ������ -> ���������� -> ����������� -------------------------------------------------------------------------------------------------------------------------------
static const Button mbCalibrator_Calibrate =
{
    Item_Button, &mspCalibrator,
    {
        "�����������", "Calibrate"
    },
    {
        "������ ��������� ����������",
        "Running the calibration procedure"
    },
    IsActive_Calibrator_Calibrate, OnPress_Calibrator_Calibrate, EmptyFuncVII
};

static bool IsActive_Calibrator_Calibrate(void)
{
    return !(set.chan[A].calibrationMode == CalibrationMode_Disable && set.chan[B].calibrationMode == CalibrationMode_Disable);
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
    uint16 newColor1 = Painter_ReduceBrightness(set.display.colors[COLOR_MENU_TITLE], 0.50f);
    uint16 newColor2 = Painter_ReduceBrightness(set.display.colors[COLOR_MENU_TITLE], 1.50f);
    set.display.colors[COLOR_MENU_TITLE_DARK] = newColor1;
    set.display.colors[COLOR_MENU_TITLE_BRIGHT] = newColor2;
    Color_Log(COLOR_MENU_TITLE);
    Color_Log(COLOR_MENU_TITLE_DARK);
    Color_Log(COLOR_MENU_TITLE_BRIGHT);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void OnChangedColorMenu2Item(void)
{
    uint16 newColor1 = Painter_ReduceBrightness(set.display.colors[COLOR_MENU_ITEM], 0.50f);
    uint16 newColor2 = Painter_ReduceBrightness(set.display.colors[COLOR_MENU_ITEM], 1.50f);
    set.display.colors[COLOR_MENU_ITEM_DARK] = newColor1;
    set.display.colors[COLOR_MENU_ITEM_BRIGHT] = newColor2;
    Color_Log(COLOR_MENU_ITEM);
    Color_Log(COLOR_MENU_ITEM_DARK);
    Color_Log(COLOR_MENU_ITEM_BRIGHT);
}


// ������ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mpService;

// ������ - ����� ��������
const Button mbServicePreviousSettings =
{
    Item_Button, &mpService,
    {
        "����� ��������", "Return settings"
    },
    {
        "���������� ��������� ������������, ������� ���� � ������ ������� \"����� �������\"",
        "Returns the oscilloscope settings that were in when you press \"Searching for signal\""
    },
    EmptyFuncBV, OnPressPrevSettings, EmptyFuncVII
};

// ������ - ���������� ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mspCalibrator;

extern const Page mspFreqMeter;

// ������ - ���� //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mspServiceSound;


//------------------------------------------------------------------------------------------------------------------------------------------------------
#include "PageServiceMath.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------
extern const Page mspServiceEthernet;


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncOfChangedEthernetSettings(bool active)
{
    Display_ShowWarning(NeedRebootDevice);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcServEthEnable =
{
    Item_Choice, &mspServiceEthernet, 
    {"Ethernet",    "Ethernet"},
    {
        "����� ������������� ethernet, �������� \"��������\" � ��������� ������.\n"
        "����� ��������� ethernet, �������� \"���������\" � ��������� ������."
        ,
        "To involve ethernet, choose \"Included\" and switch off the device.\n"
        "To disconnect ethernet, choose \"Disconnected\" and switch off the device."
    },
    EmptyFuncBV,
    {
        {"��������",    "Included"},
        {"���������",   "Disconnected"}
    },
    (int8*)&set.eth.enable, FuncOfChangedEthernetSettings, EmptyFuncVII
};


static const IPaddress ipAddress =
{
    Item_IP, &mspServiceEthernet,
    {"IP �����", "IP-address"},
    {
        "��������� IP ������",
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
    Item_IP, &mspServiceEthernet,
    {"����� �������", "Network mask"},
    {
        "��������� ����� �������",
        "Set of network mask"
    },
    EmptyFuncBV,
    &set.eth.mask0, &set.eth.mask1, &set.eth.mask2, &set.eth.mask3,
    FuncOfChangedEthernetSettings
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
static const IPaddress ipGateway =
{
    Item_IP, &mspServiceEthernet,
    {"����", "Gateway"},
    {
        "��������� ������ ��������� �����",
        "Set of gateway address"
    },
    EmptyFuncBV,
    &set.eth.gw0, &set.eth.gw1, &set.eth.gw2, &set.eth.gw3,
    FuncOfChangedEthernetSettings
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
static const MACaddress macMAC =
{
    Item_MAC, &mspServiceEthernet,
    {"��� �����", "MAC-address"},
    {
        "��������� ����������� ������",
        "Set of MAC-address"
    },
    EmptyFuncBV,
    &set.eth.mac0, &set.eth.mac1, &set.eth.mac2, &set.eth.mac3, &set.eth.mac4, &set.eth.mac5,
    FuncOfChangedEthernetSettings
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
// ������ - ���� - ����
const Page mspServiceSound;

const Choice mcServiceSoundEnable =
{
    Item_Choice, &mspServiceSound, {"����", "Sound"},
    {
        "���������/���������� �����",
        "Inclusion/switching off of a sound"
    },
    EmptyFuncBV,
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.service.soundEnable, EmptyFuncVB, EmptyFuncVII
};


//---------------------------------------------------------------------------------------------------------------------------------------------------
const Governor mgServiceSoundVolume =
{
    Item_Governor, &mspServiceSound,
    {
        "���������", "Volume"
    },
    {
        "��������� ��������� �����",
        "Set the volume"
    },
    EmptyFuncBV,
    &set.service.soundVolume, 0, 100, EmptyFuncVV
};


// ������ - ���� /////////////////////////////////////////////////////////////////////////////////////////////
const Page mspServiceSound =
{
    Item_Page, &mpService,
    {
        "����", "SOUND"
    },
    {
        "� ���� ���� ����� ��������� ��������� �����",
        "In this menu, you can adjust the volume"
    },
    EmptyFuncBV, Page_ServiceSound,
    {
        (void*)&mcServiceSoundEnable,
        (void*)&mgServiceSoundVolume
    }

};


// ������ - ETHERNET  /////////////////////////////////////////////////////////////////////////////////////////
const Page mspServiceEthernet = 
{
    Item_Page, &mpService,
    {
        "ETHERNET", "ETHERNET"
    },
    {
        "��������� ethernet",
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
