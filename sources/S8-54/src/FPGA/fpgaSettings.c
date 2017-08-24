// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Log.h"
#include "Data/Data.h"
#include "Display/Display.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGAtypes.h"
#include "Hardware/FSMC.h"
#include "Hardware/Hardware.h"
#include "Hardware/Timer.h"
#include "Panel/Panel.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"


/*
    Ограничение полосы в канале (CS3 - канал А, CS4 - канал B)
        Полн.пол.   20МГц   100МГц  200МГц  350МГц  650МГц  750МГц
    D6     0          1       0       1       0       1       0 
    D7     0          0       1       1       0       0       1
    D8     0          0       0       0       1       1       1

    Входы канлов - CS2

            Канал 1     Канал 2
            D2  D3      D5  D6
    DC      0   0       0   0
    AC      1   0       1   0
    GND     0   1       0   1
    50 Om   1   1       1   1

    Синхронизация - CS2
        Кан.1   Кан.2   Внешн.        Фильтр    ПС  НЧ  ВЧ  АС
    D14  0       0        1             D8      0   0   1   1
    D15  0       1        0             D9      0   1   0   1
*/


static const uint8 masksRange[RangeSize] =
{         //  543210
    BINARY_U8(010011),  ///< Range_2mV
    BINARY_U8(010111),  ///< Range_5mV
    BINARY_U8(011010),  ///< Range_10mV
    BINARY_U8(000011),  ///< Range_20mV
    BINARY_U8(000111),  ///< Range_50mV
    BINARY_U8(001010),  ///< Range_100mV
    BINARY_U8(010110),  ///< Range_200mV
    BINARY_U8(011010),  ///< Range_500mV
    BINARY_U8(000011),  ///< Range_1V
    BINARY_U8(000110),  ///< Range_2V
    BINARY_U8(001010)   ///< Range_5V
};


/// Добавочные смещения по времени для разверёток режима рандомизатора.
static int16 timeCompensation[TBaseSize] = {550, 275, 120, 55, 25, 9, 4, 1};

typedef struct
{
    uint8 maskNorm;         ///< Маска. Требуется для записи в аппаратную часть при выключенном режиме пикового детектора.
    uint8 maskPeackDet;     ///< Маска. Требуется для записи в аппаратную часть при включенном режиме пикового детектора.
} TBaseMaskStruct;


static const TBaseMaskStruct masksTBase[TBaseSize] =
{
    {BINARY_U8(00000000), BINARY_U8(00000000)}, ///< TBase_1ns
    {BINARY_U8(00000000), BINARY_U8(00000000)}, ///< TBase_2ns
    {BINARY_U8(00000000), BINARY_U8(00000000)}, ///< TBase_5ns
    {BINARY_U8(00000000), BINARY_U8(00000000)}, ///< TBase_10ns
    {BINARY_U8(00000000), BINARY_U8(00000000)}, ///< TBase_20ns
    {BINARY_U8(00000000), BINARY_U8(00000000)}, ///< TBase_50ns
    {BINARY_U8(00100001), BINARY_U8(00100001)}, ///< TBase_100ns
    {BINARY_U8(00100010), BINARY_U8(00100010)}, ///< TBase_200ns
    {BINARY_U8(00100011), BINARY_U8(00100010)}, ///< TBase_500ns
    {BINARY_U8(01000101), BINARY_U8(00100011)}, ///< TBase_1us
    {BINARY_U8(01000110), BINARY_U8(01000101)}, ///< TBase_2us
    {BINARY_U8(01000111), BINARY_U8(01000110)}, ///< TBase_5us
    {BINARY_U8(01001001), BINARY_U8(01000111)}, ///< TBase_10us
    {BINARY_U8(01001010), BINARY_U8(01001001)}, ///< TBase_20us
    {BINARY_U8(01001011), BINARY_U8(01001010)}, ///< TBase_50us
    {BINARY_U8(01001101), BINARY_U8(01001011)}, ///< TBase_100us
    {BINARY_U8(01001110), BINARY_U8(01001101)}, ///< TBase_200us
    {BINARY_U8(01001111), BINARY_U8(01001110)}, ///< TBase_500us
    {BINARY_U8(01010001), BINARY_U8(01001111)}, ///< TBase_1ms
    {BINARY_U8(01010010), BINARY_U8(01010001)}, ///< TBase_2ms
    {BINARY_U8(01010011), BINARY_U8(01010010)}, ///< TBase_5ms
    {BINARY_U8(01010101), BINARY_U8(01010011)}, ///< TBase_10ms
    {BINARY_U8(01010110), BINARY_U8(01010101)}, ///< TBase_20ms
    {BINARY_U8(01010111), BINARY_U8(01010110)}, ///< TBase_50ms
    {BINARY_U8(01011001), BINARY_U8(01010111)}, ///< TBase_100ms
    {BINARY_U8(01011010), BINARY_U8(01011001)}, ///< TBase_200ms
    {BINARY_U8(01011011), BINARY_U8(01011010)}, ///< TBase_500ms
    {BINARY_U8(01011101), BINARY_U8(01011011)}, ///< TBase_1s
    {BINARY_U8(01011110), BINARY_U8(01011101)}, ///< TBase_2s
    {BINARY_U8(01011111), BINARY_U8(01011110)}, ///< TBase_5s
    {BINARY_U8(01111111), BINARY_U8(01011111)}  ///< TBase_10s
};


uint16 gPost = 1024;
int16 gPred = 1024;
int gAddNStop = 0;


static void LoadTrigLev(void);
/// \todo временный костыль. При изменении tShift нужно временно останавливать альтеру, а при изменении развёртки не нужно
static void SetTShift(int tShift, bool needFPGApause);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void LoadTBase(void)
{
    TBase tBase = SET_TBASE;
    uint8 mask = SET_PEAKDET ? masksTBase[tBase].maskPeackDet : masksTBase[tBase].maskNorm;
    FPGA_Write(RecordFPGA, WR_RAZV, mask, true);
    TIME_COMPENSATION = timeCompensation[tBase];
}

//static const int16 addShift[][5][3] =
//{   // TPos     Left Center Right
//    /* 281   */ {{-40, -20, 0},  {10,  5,   0},  {0,  0,  0},  {0,  0,  0},  {0,  0,  0}},
//    //               1ns             2ns           5ns           10ns          20ns
//    /* 512   */ {{-6,  -28, 0},  {-6,  -3,  0},  {-6, -8,  0}, {-1, -3, 0},  {-1, 2,  0}},
//    /* 1024  */ {{-12, -6,  0},  {-12, -6,  0},  {-2, -6, 0},  {-2, -1, 0},  {-2, -1, 0}},
//    /* 2048  */ {{-24, -12, 0},  {-24, -12, 0},  {-4  -2, 0},  {-4, -2, 0},  {-1, -2, 0}},
//    /* 4096  */ {{-48, -24, 0},  {-23, -24, 0},  {-8, -4, 0},  {-3, -4, 0},  {0,  -1, 0}},
//    /* 8192  */ {{-46, -48, 0},  {-21, -23, 0},  {-6, -8, 0},  {-1, -3, 0},  {-1, 0,  0}},
//    /* 16384 */ {{-29, -46, 0},  {-17, -21, 0},  {-2, -6, 0},  {-2, -1, 0},  {-2, -1, 0}},
//    /* 32768 */ {{0,   0,   0},  {0,   0,   0},  {0,  0,  0},  {0,  0,  0},  {0,  0,  0}}
//};

int addShiftForFPGA = 0;

//----------------------------------------------------------------------------------------------------------------------------------------------------
void LoadTShift(void)
{
    TBase tBase = SET_TBASE;
    int tShift = SET_TSHIFT - sTime_TShiftMin() + timeCompensation[tBase];

    gPost = (uint16)tShift;

    if(IN_RANDOM_MODE)
    {
        extern const int Kr[];
        int k = 0;
        if (TPOS_IS_LEFT)
        {
            k = SET_POINTS_IN_CHANNEL % Kr[tBase];
        }
        else if (TPOS_IS_CENTER)
        {
            k = (SET_POINTS_IN_CHANNEL / 2) % Kr[tBase];
        }

        gPost = (uint16)((2 * gPost - k) / Kr[tBase]);

        addShiftForFPGA = (SET_TSHIFT * 2) % Kr[tBase];
        if (addShiftForFPGA < 0)
        {
            addShiftForFPGA = Kr[tBase] + addShiftForFPGA;
        }
        gPred = ~(PRETRIGGERED);
    }
    else
    {
        gPred = (int16)SET_BYTES_IN_CHANNEL / 2 - (int16)gPost;

        if (gPred < 0)
        {
            gPred = 0;
        }
        gPred = ~(gPred + 3);
    }

    if (tShift < 0)
    {
        gPost = 0;
        gAddNStop = -tShift;
    }
    else
    {
        gAddNStop = 0;
    }

    gPost = ~(gPost + 1);                   // Здесь просто для записи в железо дополняем

    if(!FPGA_IN_PROCESS_OF_READ)
    {
        if (SET_TBASE > 8)
        {
            ++gPost;
            --gPred;
        }
        FPGA_Write(RecordFPGA, WR_POST, gPost, true);
        FPGA_Write(RecordFPGA, WR_PRED, gPred, true);
    }
}


static const float divR[RangeSize] = {50.0f, 20.0f, 10.0f, 5.0f, 2.0f, 1.0f, 25.0f, 10.0f, 5.0f, 2.5f, 1.0f};


//---------------------------------------------------------------------------------------------------------------------------------------------------
// Рассчитывает смещение канала отностиельно нуля
static int CalculateDeltaRShift(Channel ch)
{
    Range range = SET_RANGE(ch);
    ModeCouple mode = SET_COUPLE(ch);
    static const int index[3] = {0, 1, 1};

    int addRShift = -(SET_INVERSE(ch) ? -1 : 1) * (int)NRST_RSHIFT_ADD(ch, range, index[mode]);
    int addRShiftFull = addRShift * (RSHIFT_IN_CELL / 20);

    uint16 rShiftRel = (uint16)((int)SET_RSHIFT(ch) + addRShiftFull);

    int dShift = (int)((RShiftZero - rShiftRel) / divR[SET_RANGE(ch)]);
    rShiftRel = (uint16)(RShiftZero + dShift);

    int delta = -(rShiftRel - RShiftZero);
    if (SET_INVERSE(ch))
    {
        delta = -delta;
    }
    rShiftRel = (uint16)(delta + RShiftZero);

    int retValue = RShiftMax + RShiftMin - rShiftRel - RShiftZero;

    return retValue;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void LoadRShift(Channel ch)
{
    // AD5064   : bits 24...27 - 0011b - command bits - Write to and update DAC channel
    //          : bits 20...23 - DAC channel - 0 - DACA, 1 - DACB
    //          : bits 04...19 - data
    static const uint mask[2] = {0x3000000, 0x3100000};

    int rShift = RShiftZero + CalculateDeltaRShift(ch);

    FPGA_Write(RecordDAC, ch == A ? dacRShiftA : dacRShiftB, mask[ch] | (rShift << 4), true);

    if (TRIG_INPUT_LPF || TRIG_INPUT_FULL)
    {
        LoadTrigLev();  // На некоторых настройках входа синхронизации требуется и коррекция уровня синхронизации
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void LoadTrigLev(void)
{
    uint data = 0x3200000;
    uint trigLev = SET_TRIGLEV(TRIGSOURCE);

    trigLev = TrigLevMax + TrigLevMin - trigLev;

    if ((TRIG_INPUT_LPF || TRIG_INPUT_FULL) && !TRIGSOURCE_EXT)
    {
        int delta = (CalculateDeltaRShift((Channel)TRIGSOURCE) * divR[SET_RANGE(TRIGSOURCE)]);
        trigLev = (int)trigLev + delta;
        if (trigLev < TrigLevMin)
        {
            trigLev = TrigLevMin;
        }
        else if (trigLev > TrigLevMax)
        {
            trigLev = TrigLevMax;
        }
    }
    
    data |= trigLev << 4;

    FPGA_Write(RecordDAC, dacTrigLev, data, true);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void WriteChipSelect1(void)
{

}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void WriteChipSelect2(void)
{
    uint data = 0;


    if(SET_RESISTANCE_A == Resistance_50Om)
    {
        data |= (3 << 2);
    }
    else if (!RECORDER_MODE)
    {               //                     D2        D3
        const uint maskCoupleA[3] = {0, (1 << 2), (1 << 3)};
        data |= maskCoupleA[SET_COUPLE_A];
    }

    if(SET_RESISTANCE_B == Resistance_50Om)
    {
        data |= (3 << 5);
    }
    else
    {
        const uint maskCoupleB[3] = {0, (1 << 5), (1 << 6)};
        data |= maskCoupleB[SET_COUPLE_B];
    }

    if(SET_RANGE_A >= Range_200mV)
    {
        data |= (1 << 1);
    }

    if(SET_RANGE_B >= Range_200mV)
    {
        data |= (1 << 4);
    }

    if (RECORDER_MODE)  // В режмие регистратора страхуемся и делаем запуск от внешней синхронизации (чтобы случайно не отключилось считывание точек);
    {
        data |= (1 << 14);
    }
    else
    {
        const uint maskSource[3] = {0, (1 << 15), (1 << 14)};

        data |= maskSource[TRIGSOURCE];
    }

    const uint trigInput[4] = {(1 << 9), (1 << 8) + (1 << 9), 0, (1 << 8)};

    data |= trigInput[TRIG_INPUT];

    FPGA_Write(RecordAnalog, CS2, data, true);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint PrepareChannel(Channel ch)
{
    uint data = 0;

    data |= masksRange[SET_RANGE(ch)];

    const uint maskField[] =
    {
        0,                      // full
        1 << 6,                 // 20
        1 << 7,                 // 100
        (1 << 6) + (1 << 7),    // 200
        1 << 8,                 // 350
        (1 << 6) + (1 << 8),    // 650
        (1 << 7) + (1 << 8)     // 750
    };

    if (MODE_EMS || SET_RANGE_2mV(ch) || SET_BANDWIDTH_20MHz(ch))
    {
        data |= maskField[Bandwidth_20MHz];
    }
    else
    {
        data |= maskField[BANDWIDTH_DEBUG(ch)];
    }

    return data;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void WriteChipSelect3(void)
{
    FPGA_Write(RecordAnalog, CS3, PrepareChannel(A), true);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void WriteChipSelect4(void)
{
    FPGA_Write(RecordAnalog, CS4, PrepareChannel(B), true);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void PrepareAndWriteDataToAnalogSPI(uint16 *addrAnalog)
{
    static pFuncVV funcs[4] = {WriteChipSelect1, WriteChipSelect2, WriteChipSelect3, WriteChipSelect4};
    funcs[(int)addrAnalog](); //-V205
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void LoadRange(Channel ch)
{
    PrepareAndWriteDataToAnalogSPI(ch == A ? CS3 : CS4);
    PrepareAndWriteDataToAnalogSPI(CS2);

    LoadRShift(ch);
    if(ch == (Channel)TRIGSOURCE)
    {
        LoadTrigLev();
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void LoadTrigPolarity(void)
{
    FPGA_Write(RecordFPGA, WR_TRIG, TRIG_POLARITY_FRONT ? 0x01 : 0x00, true);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Загрузить регистр WR_UPR (пиковый детектор и калибратор).
void LoadRegUPR(void)
{
    uint16 data = 0;
    if(IN_RANDOM_MODE)
    {
        SetBit(data, 0);
    }

    const uint16 mask[3] =
    {
        (1 << UPR_BIT_CALIBRATOR_AC_DC),
        (1 << UPR_BIT_CALIBRATOR_VOLTAGE),
        (0)
    };

    data |= mask[CALIBRATOR_MODE];

    if (RECORDER_MODE)
    {
        data |= (1 << UPR_BIT_RECORDER);
    }

    if (SET_PEAKDET_EN)
    {
        data |= (1 << UPR_BIT_PEAKDET);
    }

    FPGA_Write(RecordFPGA, WR_UPR, data, false);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_LoadSettings(void) 
{
    LoadTBase();
    LoadTShift();
    LoadRange(A);
    LoadRShift(A);
    LoadRange(B);
    LoadRShift(B);
    LoadTrigLev();
    LoadTrigPolarity();
    LoadRegUPR();
    LoadTrigLev();
    PrepareAndWriteDataToAnalogSPI(CS1);
    PrepareAndWriteDataToAnalogSPI(CS2);
    PrepareAndWriteDataToAnalogSPI(CS3);
    PrepareAndWriteDataToAnalogSPI(CS4);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetResistance(Channel ch, Resistance resistance)
{
    SET_RESISTANCE(ch) = resistance;
    PrepareAndWriteDataToAnalogSPI(CS2);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetBandwidth(Channel ch)
{
    PrepareAndWriteDataToAnalogSPI(ch == A ? CS3 : CS4);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetRange(Channel ch, Range range)
{
    if (!SET_ENABLED(ch))
    {
        return;
    }
    if (range < RangeSize && (int)range >= 0)
    {
        float rShiftAbs = RSHIFT_2_ABS(SET_RSHIFT(ch), SET_RANGE(ch));
        float trigLevAbs = RSHIFT_2_ABS(SET_TRIGLEV(ch), SET_RANGE(ch));
        sChannel_SetRange(ch, range);
        if (LINKING_RSHIFT == LinkingRShift_Voltage)
        {
            SET_RSHIFT(ch) = (int16)Math_RShift2Rel(rShiftAbs, range);
            SET_TRIGLEV(ch) = (int16)Math_RShift2Rel(trigLevAbs, range);
        }
        LoadRange(ch);
        LoadTrigLev();
    }
    else
    {
        Display_ShowWarning(ch == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTBase(TBase tBase)
{
    if (!SET_ENABLED_A && !SET_ENABLED_B)
    {
        return;
    }
    if (tBase < TBaseSize && (int)tBase >= 0)
    {
        float tShiftAbsOld = TSHIFT_2_ABS(SET_TSHIFT, SET_TBASE);
        sTime_SetTBase(tBase);
        LoadTBase();
        SetTShift((int)TSHIFT_2_REL(tShiftAbsOld, SET_TBASE), false);
        NEED_FINISH_DRAW = 1;
    }
    else
    {
        Display_ShowWarning(LimitSweep_Time);
    }
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_TBaseDecrease(void)
{
    if (SET_PEAKDET && SET_TBASE <= MIN_TBASE_PEC_DEAT)
    {
        Display_ShowWarning(LimitSweep_Time);
        Display_ShowWarning(EnabledPeakDet);
        return;
    }

    if ((int)SET_TBASE > 0)
    {
        if (RECORDER_MODE && SET_TBASE == MIN_TBASE_P2P)
        {
            Display_ShowWarning(TooFastScanForRecorder);
        }
        else
        {
            TBase base = (TBase)((int)SET_TBASE - 1);
            FPGA_SetTBase(base);
        }
    }
    else
    {
        Display_ShowWarning(LimitSweep_Time);
    }

    if (SET_TBASE == TBase_20ns &&      // Если перешли в режим эквавалентного времени
        !START_MODE_SINGLE)             // И не находимся в режиме однократного запуска
    {
        SAMPLE_TYPE = SAMPLE_TYPE_IS_OLD;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_TBaseIncrease(void)
{
    if (SET_TBASE < (TBaseSize - 1))
    {
        TBase base = (TBase)(SET_TBASE + 1); //-V2006
        FPGA_SetTBase(base);
    }
    else
    {
        Display_ShowWarning(LimitSweep_Time);
    }
    
    if (SET_TBASE == TBase_50ns &&      // Если перешли в режим реального времени
        !START_MODE_SINGLE)             // И не находимся в режиме однократного запуска
    {
        SAMPLE_TYPE = SampleType_Real;       // И установим реальный, потому что в реальном режиме эквивалентный глупо смотрится
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetRShift(Channel ch, uint16 rShift)
{
    if (!SET_ENABLED(ch))
    {
        return;
    }
    Display_ChangedRShiftMarkers(true);

    if (rShift > RShiftMax || rShift < RShiftMin)
    {
        Display_ShowWarning(ch == A ? LimitChan1_RShift : LimitChan2_RShift);
    }

    LIMITATION(rShift, RShiftMin, RShiftMax);

    uint16 oldRShift = SET_RSHIFT(ch);
    SET_RSHIFT(ch) = rShift;
    LoadRShift(ch);
    Display_RotateRShift(ch);

    if (oldRShift != rShift)
    {
        FPGA_TemporaryPause();
    }
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTrigLev(TrigSource ch, uint16 trigLev)
{
    Display_ChangedRShiftMarkers(true);
    if (trigLev < TrigLevMin || trigLev > TrigLevMax)
    {
        Display_ShowWarning(LimitSweep_Level);
    }
    LIMITATION(trigLev, TrigLevMin, TrigLevMax);

    Display_RotateTrigLev();

    if (SET_TRIGLEV(ch) != trigLev)
    {
        SET_TRIGLEV(ch) = trigLev;
        LoadTrigLev();
    }
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_ChangePostValue(int delta)
{
	/*
    int tempPost = (int)postValue + Math_Sign(delta);

    if (tempPost < 0)
    {
        postValue = 0;
    }
    else if (tempPost > 16383)
    {
        postValue = 16383;
    }
    else
    {
        postValue = tempPost;
    }
	*/
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTShift(int tShift)
{
    SetTShift(tShift, true);
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SetTShift(int tShift, bool needFPGApause)
{
    if (!SET_ENABLED_A && !SET_ENABLED_B)
    {
        return;
    }

    if (tShift < sTime_TShiftMin() || tShift > TShiftMax)
    {
        LIMITATION(tShift, sTime_TShiftMin(), TShiftMax);
        Display_ShowWarning(LimitSweep_TShift);
    }

    int16 oldTShift = SET_TSHIFT;

    sTime_SetTShift((int16)tShift);
    LoadTShift();       /// \todo temp for s8-54
    NEED_FINISH_DRAW = 1;

    if (needFPGApause && tShift != oldTShift)
    {
        FPGA_TemporaryPause();
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetDeltaTShift(int16 shift)
{
    timeCompensation[SET_TBASE] = shift;
    LoadTShift();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetPeackDetMode(PeakDetMode peackDetMode)
{
    SET_PEAKDET = peackDetMode;
    LoadRegUPR();
    LoadTBase();
    LoadTShift();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetCalibratorMode(CalibratorMode calibratorMode)
{
    CALIBRATOR_MODE = calibratorMode;
    LoadRegUPR();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_EnableRecorderMode(bool enable)
{
    RECORDER_MODE = enable;

    if (RECORDER_MODE)
    {
        if (SET_TBASE < TBase_100ms)
        {
            FPGA_SetTBase(TBase_100ms);
        }
    }

    FPGA_SetModeCouple(A, SET_COUPLE_A);
    FPGA_SetModeCouple(B, SET_COUPLE_B);
    
    LoadRegUPR();

    FPGA_SetTrigSource(TRIGSOURCE);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *FPGA_GetTShiftString(int16 tShiftRel, char buffer[20])
{
    float tShiftVal = TSHIFT_2_ABS(tShiftRel, SET_TBASE);
    return Time2String(tShiftVal, true, buffer);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA_RangeIncrease(Channel ch)
{
    bool retValue = false;
    if (SET_RANGE(ch) < RangeSize - 1)
    {
        FPGA_SetRange(ch, (Range)(SET_RANGE(ch) + 1)); //-V2006
        retValue = true;
    }
    else
    {
       Display_ShowWarning(ch == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
    NEED_FINISH_DRAW = 1;
    return retValue;
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA_RangeDecrease(Channel ch)
{
    bool retValue = false;
    if (SET_RANGE(ch) > 0)
    {
        FPGA_SetRange(ch, (Range)(SET_RANGE(ch) - 1)); //-V2006
        retValue = true;
    }
    else
    {
        Display_ShowWarning(ch == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
    NEED_FINISH_DRAW = 1;
    return retValue;
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTrigSource(TrigSource trigSource)
{
    TRIGSOURCE = trigSource;
    PrepareAndWriteDataToAnalogSPI(CS2);
    if (!TRIGSOURCE_EXT)
    {
        FPGA_SetTrigLev(TRIGSOURCE, SET_TRIGLEV(TRIGSOURCE));
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTrigPolarity(TrigPolarity polarity)
{
    TRIG_POLARITY = polarity;
    LoadTrigPolarity();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTrigInput(TrigInput trigInput)
{
    TRIG_INPUT = trigInput;
    PrepareAndWriteDataToAnalogSPI(CS2);
    LoadTrigLev();      // На некотрых насройках входа требуется коррекция уровня синхронизации
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetModeCouple(Channel ch, ModeCouple modeCoupe)
{
    SET_COUPLE(ch) = modeCoupe;
    PrepareAndWriteDataToAnalogSPI(CS2);
    FPGA_SetRShift(ch, SET_RSHIFT(ch));
}
