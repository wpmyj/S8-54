#include "defines.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Display/Colors.h"
#include "FPGA/FPGA.h"
#include "Hardware/RTC.h"
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

// ������ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpService =
{
    Item_Page, &mainPage, 0,
    {
        "������", "SERVICE",
        "�������������� ���������, ����������, ����� �������, �������������� �������",
        "Additional settings, calibration, signal search, mathematical functions"
    },
    Page_Service,
    {
        (void*)&mbResetSettings,    // ������ - ����� ��������
        (void*)&mbAutoSearch,       // ������ - ����� �������
        (void*)&mspCalibrator,      // ������ - ����������
        (void*)&mcRecorder,         // ������ - �����������
        (void*)&mspFFT,             // ������ - ������
        (void*)&mspMathFunction,    // ������ - �������
        (void*)&mspEthernet,        // ������ - ETHERNET
        (void*)&mspSound,           // ������ - ����
        (void*)&mspTime,            // ������ - �����
        (void*)&mcLanguage,         // ������ - ����
        (void*)&mspInformation      // ������ - ����������
    }
};

// ������ - ����� �������� ---------------------------------------------------------------------------------------------------------------------------
static const Button mbResetSettings =
{
    Item_Button, &mpService, 0,
    {
        "����� ��������", "Reset settings",
        "����� �������� �� ��������� �� ���������",
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

    Painter_DrawTextInRectWithTransfersC(30, 110, 300, 200, "����������� ����� �������� �������� ������ ����/����.\n"
                                         "������� ����� ������ ������, ���� ����� �� �����.", gColorFill);

    Painter_EndScene();
}

// ������ - ����� ������� ----------------------------------------------------------------------------------------------------------------------------
static const Button mbAutoSearch =
{
    Item_Button, &mpService, 0,
    {
        "����� �������", "Find signal",
        "������������� ����������� ��������� ������������ ��� ������� � ������ 1",
        "Sets optimal settings for the oscilloscope signal on channel 1"
    },
    OnPress_AutoSearch
};

static void OnPress_AutoSearch(void)
{
    FPGA_StartAutoFind();
};

// ������ - ����������� ------------------------------------------------------------------------------------------------------------------------------
static const Choice mcRecorder =
{
    Item_Choice, &mpService, 0,
    {
        "�����������", "Recorder",
        "��������/��������� ����� ������������. ���� ����� �������� �� ��������� 50 ��/��� � ����� ���������.",
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

// ������ - ���� -------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcLanguage =
{
    Item_Choice, &mpService, 0,
    {
        "����",         "Language",
        "��������� ������� ���� ����",
        "Allows you to select the menu language"
    },
    {
        {"�������",     "Russian"},
        {"����������",  "English"}
    },
    (int8*)&set.common.lang
};

// ����� /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspTime =
{
    Item_Page, &mpService, 0,
    {
        "�����", "TIME",
        "��������� � ��������� �������",
        "Set and setup time"
    },
    Page_ServiceTime,
    {
        (void*)&mtTime,             // ������ - ����� - �����
        (void*)&mgTimeCorrection    // C����� - ����� - ���������
    }
};

// ������ - ����� - ����� ----------------------------------------------------------------------------------------------------------------------------
static int8 dServicetime = 0;
static int8 hours = 0, minutes = 0, secondes = 0, year = 0, month = 0, day = 0;
static const Time mtTime =
{
    Item_Time, &mspTime, 0,
    {
        "�����", "Time"
        ,
        "��������� �������� �������.\n������� ������:\n"
        "������ �� ������� ���� \"�����\". ��������� ���� ��������� �������� �������. ��������� ��������� ������ �� �������� ����������, �������������� "
        "�������� ���������� \"�����\", �������� ����, ������, �������, ���, �����, ��� �����. ���������� ������� ������������ �������� ��������. "
        "��������� ����� ��������� ���������� ����������� ��������. ����� �������� ����� \"���������\", ������ � ���������� ����� 0.5 ��� ������ �� ������ "
        "����������. ���� ��������� �������� ������� ��������� � ����������� ������ �������� �������. ������� ���������� ��������� ������ �� ����� ������ �������� "
        "������� � �������� ���� ��������� �������� ���    ���� ��� ���������� ������ �������� �������"
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

// ������ - ����� - ��������� ------------------------------------------------------------------------------------------------------------------------
static const Governor mgTimeCorrection =
{
    Item_Governor, &mspTime, 0,
    {
        "���������", "Correction",
        "��������� ��������������� ������������ ��� ����������� ���� �������",
        "Setting correction factor to compensate for time travel"
    },
    &setNR.correctionTime, -63, 63, OnChange_Time_Correction  
};

static void OnChange_Time_Correction(void)
{
    RTC_SetCorrection((int8)setNR.correctionTime);
}

// ������ - ���������� ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspCalibrator =
{
    Item_Page, &mpService, 0,
    {
        "����������", "CALIBRATOR",
        "����������� ������������ � ���������� ������������",
        "Control of the calibrator and calibration of an oscillograph"
    },
    Page_ServiceCalibrator,
    {
        (void*)&mcCalibrator,           // ������ - ���������� - ����������
        (void*)&mbCalibrator_Calibrate  // ������ - ���������� - �����������
    }
};

// ������ - ���������� - ���������� ------------------------------------------------------------------------------------------------------------------
static const Choice mcCalibrator =
{
    Item_Choice, &mspCalibrator, 0,
    {
        "����������",   "Calibrator",
        "����� ������ �����������",
        "Mode of operation of the calibrator"
    },
    {
        {"�����",       "DC"},
        {"+4V",         "+4V"},
        {"0V",          "0V"}
    },
    (int8*)&set.service.calibrator, OnChange_Calibrator
};

static void OnChange_Calibrator(bool active)
{
    FPGA_SetCalibratorMode(set.service.calibrator);
}

// ������ - ���������� - ����������� -----------------------------------------------------------------------------------------------------------------
static const Button mbCalibrator_Calibrate =
{
    Item_Button, &mspCalibrator, IsActive_Calibrator_Calibrate,
    {
        "�����������", "Calibrate",
        "������ ��������� ����������",
        "Running the calibration procedure"
    },
    OnPress_Calibrator_Calibrate, EmptyFuncVII
};

static bool IsActive_Calibrator_Calibrate(void)
{
    return !(CALIBR_MODE_A == CalibrationMode_Disable && CALIBR_MODE_B == CalibrationMode_Disable);
}

static void OnPress_Calibrator_Calibrate(void)
{
    gStateFPGA.needCalibration = true;
}

















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


// ������ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mpService;

// ������ - ����� ��������
const Button mbServicePreviousSettings =
{
    Item_Button, &mpService, 0,
    {
        "����� ��������", "Return settings",
        "���������� ��������� ������������, ������� ���� � ������ ������� \"����� �������\"",
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
        "����� ������������� ethernet, �������� \"��������\" � ��������� ������.\n"
        "����� ��������� ethernet, �������� \"���������\" � ��������� ������."
        ,
        "To involve ethernet, choose \"Included\" and switch off the device.\n"
        "To disconnect ethernet, choose \"Disconnected\" and switch off the device."
    },
    {
        {"��������",    "Included"},
        {"���������",   "Disconnected"}
    },
    (int8*)&set.eth.enable, FuncOfChangedEthernetSettings
};


static const IPaddress ipAddress =
{
    Item_IP, &mspEthernet, 0,
    {
        "IP �����", "IP-address",
        "��������� IP ������",
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
        "����� �������", "Network mask",
        "��������� ����� �������",
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
        "����", "Gateway",
        "��������� ������ ��������� �����",
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
        "��� �����", "MAC-address",
        "��������� ����������� ������",
        "Set of MAC-address"
    },
    &set.eth.mac0, &set.eth.mac1, &set.eth.mac2, &set.eth.mac3, &set.eth.mac4, &set.eth.mac5,
    FuncOfChangedEthernetSettings
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
// ������ - ���� - ����
const Page mspSound;

const Choice mcServiceSoundEnable =
{
    Item_Choice, &mspSound, 0,
    {
        "����", "Sound",
        "���������/���������� �����",
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
        "���������", "Volume",
        "��������� ��������� �����",
        "Set the volume"
    },
    &set.service.soundVolume, 0, 100, EmptyFuncVV
};

// ������ - ���� /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspSound =
{
    Item_Page, &mpService, 0,
    {
        "����", "SOUND",
        "� ���� ���� ����� ��������� ��������� �����",
        "In this menu, you can adjust the volume"
    },
    Page_ServiceSound,
    {
        (void*)&mcServiceSoundEnable,
        (void*)&mgServiceSoundVolume
    }

};

// ������ - ETHERNET  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspEthernet = 
{
    Item_Page, &mpService, 0,
    {
        "ETHERNET", "ETHERNET",
        "��������� ethernet",
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