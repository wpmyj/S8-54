#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspCalibrator;


// СЕРВИС - КАЛИБРАТОР ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnChange_Calibrator(bool active)
{
    FPGA_SetCalibratorMode(set.service.calibrator);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_Calibrator_Calibrate(void)
{
    return !(CALIBR_MODE_A == CalibrationMode_Disable && CALIBR_MODE_B == CalibrationMode_Disable);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Calibrator_Calibrate(void)
{
    gStateFPGA.needCalibration = true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcCalibrator =
{
    Item_Choice, &mspCalibrator, 0,
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
    (int8*)&set.service.calibrator, OnChange_Calibrator
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Button mbCalibrator_Calibrate =
{
    Item_Button, &mspCalibrator, IsActive_Calibrator_Calibrate,
    {
        "Калибровать", "Calibrate",
        "Запуск процедуры калибровки",
        "Running the calibration procedure"
    },
    OnPress_Calibrator_Calibrate, EmptyFuncVII
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Page mspCalibrator =
{
    Item_Page, &mpService, 0,
    {
        "КАЛИБРАТОР", "CALIBRATOR",
        "Управлением калибратором и калибровка осциллографа",
        "Control of the calibrator and calibration of an oscillograph"
    },
    Page_ServiceCalibrator,
    {
        (void*)&mcCalibrator,           // СЕРВИС - КАЛИБРАТОР - Калибратор
        (void*)&mbCalibrator_Calibrate  // СЕРВИС - КАЛИБРАТОР - Калибровать
    }
};
