#include "Panel.h"
#include "Controls.h"
#include "Log.h"
#include "Display/Display.h"
#include "FPGA/FPGA.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Debug.h"
#include "Hardware/Timer.h"
#include "Hardware/Sound.h"
#include "Hardware/Hardware.h"
#include "Hardware/FSMC.h"

#include <string.h>

#include "PanelFunctions.c"


extern void OnPress_ResetSettings(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_DATA            20

#define LED_CHAN0_ENABLE    129
#define LED_CHAN0_DISABLE   1
#define LED_CHAN1_ENABLE    130
#define LED_CHAN1_DISABLE   2
#define LED_TRIG_ENABLE     131
#define LED_TRIG_DISABLE    3
#define POWER_OFF           4


static PanelButton pressedKey = B_Empty;
static volatile PanelButton pressedButton = B_Empty;         // Это используется для отслеживания нажатой кнопки при отключенной панели
static uint16 dataTransmitted[MAX_DATA] = {0x00};
static uint16 firstPos = 0;
static uint16 lastPos = 0;


// В этих переменных сохраняем значения в прерывании
static PanelButton releaseButton = B_Empty;
static PanelButton pressButton = B_Empty;
static Regulator regLeft = R_Empty;
static Regulator regRight = R_Empty;
static int numReg = 0;                              // Число поворотов ручки
static Regulator regPress = R_Empty;
static Regulator regRelease = R_Empty;
static PanelCommand recvCommand = C_None;

static int allRecData = 0;
static bool isRunning = true;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    void(*funcOnKey)(int);  // Функция вызывается при нажатии(1) / отпускании(-1) кнопки
    void(*funcLongPressure)(void);  // Функция выывается при длительном удержании кнопки
} StructButton;


static const StructButton funcButton[B_NumButtons] =
{
    {0, 0},
    {EFB,           ChannelA_Long}, // B_ChannelA
    {EFB,           EmptyFuncVV},   // B_Service
    {EFB,           ChannelB_Long}, // B_ChannelB
    {EFB,           EmptyFuncVV},   // B_Display
    {EFB,           TimeLong},      // B_Time
    {EFB,           EmptyFuncVV},   // B_Memory
    {EFB,           BtnRegTrig},// B_Sinchro
    {FuncBtnStart,  EmptyFuncVV},   // B_Start
    {EFB,           EmptyFuncVV},   // B_Cursors
    {EFB,           EmptyFuncVV},   // B_Measures
    {FuncBtnPower,  EmptyFuncVV},   // B_Power
    {EFB,           Help_Long},     // B_Help
    {EFB,           Menu_Long},     // B_Menu
    {EFB,           F1_Long},       // B_F1
    {EFB,           F2_Long},       // B_F2
    {EFB,           F3_Long},       // B_F2
    {EFB,           F4_Long},       // B_F3
    {EFB,           F5_Long}        // B_F4
};


typedef struct
{
    void(*rotate)(int delta);   // Эта функция вызывается при повороте ручки
    void(*press)(int delta);    // Функция вызывается при нажатии/отпускании ручки
    void(*longPress)(void);     // Эта функция вызывается при длительном нажатии ручки
} StructReg;

static const StructReg funculatorReg[] =
{
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {FuncRangeA,    EFB,                EmptyFuncVV},   // R_RangeA
    {FuncRShiftA,   FuncBtnRegChannelA, EmptyFuncVV},   // R_RShiftA
    {FuncRangeB,    EFB,                EmptyFuncVV},   // R_RangeB
    {FuncRShiftB,   FuncBtnRegChannelB, EmptyFuncVV},   // R_RShiftB
    {FuncTBase,     EFB,                EmptyFuncVV},   // R_TBase
    {FuncTShift,    FuncBtnRegTime,     EmptyFuncVV},   // R_TShift
    {FuncTrigLev,   FuncBtnRegTrig,     EmptyFuncVV},   // R_TrigLev
    {FuncRegSet,    FuncBtnRegSet,      EmptyFuncVV},   // R_Set
    // WARN временно продублировано
    {FuncRangeA,    EFB, EmptyFuncVV},
    {FuncRShiftA,   FuncBtnRegChannelA, EmptyFuncVV},
    {FuncRangeB,    EFB,                EmptyFuncVV},   // R_RangeB
    {FuncRShiftB,   FuncBtnRegChannelB, EmptyFuncVV},
    {FuncTBase,     EFB,                EmptyFuncVV},   // R_TBase
    {FuncTShift,    FuncBtnRegTime,     EmptyFuncVV},   // R_TShift
    {FuncTrigLev,   FuncBtnRegTrig,     EmptyFuncVV},   // R_TrigLev
    {FuncRegSet,    FuncBtnRegSet,      EmptyFuncVV}    // R_Set
};

uint16 RotateRegRight(Regulator reg);
uint16 ButtonPress(PanelButton button);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static PanelButton ButtonIsRelease(uint16 command)
{
    PanelButton button = B_Empty;

    static uint timePrevReleaseButton = 0;

    if(command < B_NumButtons && command > B_Empty)
    {
        if(gTimerMS - timePrevReleaseButton > 100)
        {
            button = (PanelButton)command;
            timePrevReleaseButton = gTimerMS;
        }
    }

    return button;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static PanelButton ButtonIsPress(uint16 command)
{
    PanelButton button = B_Empty;

    static uint timePrevPressButton = 0;

    if (command < (B_NumButtons | 0x80) && command > (B_Empty | 0x80))
    {
        if(gTimerMS - timePrevPressButton > 100)
        {
            button = (PanelButton)(command & 0x7f);
            timePrevPressButton = gTimerMS;
        }
    }

    return button;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static Regulator RegulatorPress(uint16 command)
{
    if (command >= (0x1c | 0x80) && command <= (0x23 | 0x80))
    {
        return (Regulator)(command & 0x7f);
    }
    return R_Empty;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static Regulator RegulatorRelease(uint16 command)
{
    if (command >= 0x1c && command <= 0x23)
    {
        return (Regulator)command;
    }
    return R_Empty;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static PanelCommand ReceiveCommand(uint16 command)
{
    if (command == C_Reset)
    {
        return C_Reset;
    }
    return C_None;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static Regulator RegulatorLeft(uint16 command)
{
    if(command >= 20 && command <= 27)
    {
        return (Regulator)command;
    }
    return R_Empty;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static Regulator RegulatorRight(uint16 command)
{
    if(command >= (20 | 0x80)  && command <= (27 | 0x80))
    {
        return (Regulator)(command & 0x7f);
    }
    return R_Empty;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void OnTimerPressedKey(void)
{
    if(pressedKey != B_Empty)
    {
        void (*func)(void) = funcButton[pressedKey].funcLongPressure;
        Menu_ReleaseButton(pressedKey);
        if(func != 0)
        {
            func();
        }
        pressedKey = B_Empty;
    }
    //Timer_Disable(kPressKey);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool Panel_ProcessingCommandFromPIC(uint16 command)
{
    if (command != 0)
    {
        gBF.panelControlReceive = 1;

        allRecData++;

        PanelButton relButton = ButtonIsRelease(command);
        if (relButton)
        {
            releaseButton = relButton;
        }
        else
        {
            PanelButton prButton = ButtonIsPress(command);
            if (prButton)
            {
                pressButton = prButton;
                pressedButton = prButton;
            }
            else
            {
                Regulator rLeft = RegulatorLeft(command);
                if (rLeft)
                {
                    regLeft = rLeft;
                    numReg++;
                }
                else
                {
                    Regulator rRight = RegulatorRight(command);
                    if (rRight)
                    {
                        regRight = rRight;
                        numReg++;
                    }
                    else
                    {
                        Regulator rPress = RegulatorPress(command);
                        if (rPress)
                        {
                            regPress = rPress;
                        }
                        else
                        {
                            Regulator rRelease = RegulatorRelease(command);
                            if (rRelease)
                            {
                                regRelease = rRelease;
                            }
                            else
                            {
                                PanelCommand com = ReceiveCommand(command);
                                if (com)
                                {
                                    recvCommand = com;
                                }
                                else
                                {
                                    if (Settings_DebugModeEnable())
                                    {
                                        static int errRecData = 0;
                                        errRecData++;
                                        float percent = (float)errRecData / allRecData * 100.0f;
                                        const int SIZE = 100;
                                        char buffer[SIZE];
                                        buffer[0] = 0;
                                        snprintf(buffer, SIZE, "%5.3f", percent);
                                        strcat(buffer, "%");
                                        LOG_ERROR("Ошибок SPI - %s %d/%d, command = %d", buffer, errRecData, allRecData, (int)command);
                                    }
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void ProcessingCommand(void)
{
    if (recvCommand == C_Reset)
    {
        Settings_Load(true);
        recvCommand = C_None;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_Update(void)
{
    if (pressButton)
    {
        pressedButton = pressButton;
    }

    if (isRunning)
    {
        if (releaseButton)
        {
            Menu_ReleaseButton(releaseButton);
            funcButton[releaseButton].funcOnKey(-1);
            if (pressedKey != B_Empty)
            {
                Menu_ShortPressureButton(releaseButton);
                pressedKey = B_Empty;
            }
            Timer_Disable(kPressKey);
        }
        else if (pressButton)
        {
            funcButton[pressButton].funcOnKey(1);
            Menu_PressButton(pressButton);
            pressedKey = pressButton;
            //Timer_Enable(kPressKey, 500, OnTimerPressedKey);
            Timer_SetAndStartOnce(kPressKey, OnTimerPressedKey, 500);
        }
        else if (regLeft)
        {
            for (int i = 0; i < numReg; i++)
            {
                funculatorReg[regLeft].rotate(-1);
            }
        }
        else if (regRight)
        {
            for (int i = 0; i < numReg; i++)
            {
                funculatorReg[regRight].rotate(1);
            }
        }
        else if (regPress)
        {
            Sound_ButtonPress();
            funculatorReg[regPress].press(1);
            regPress = R_Empty;
        }
        else if (regRelease)
        {
            Sound_ButtonRelease();
            funculatorReg[regRelease].press(-1);
            regRelease = R_Empty;
        }
        else
        {
            ProcessingCommand();
        }
    }

    pressButton = B_Empty;
    releaseButton = B_Empty;
    regLeft = R_Empty;
    regRight = R_Empty;
    numReg = 0;

    gBF.panelControlReceive = 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_EnableLEDChannelA(bool enable)
{
    Panel_TransmitData(enable ? LED_CHAN0_ENABLE : LED_CHAN0_DISABLE);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_EnableLEDChannelB(bool enable)
{
    Panel_TransmitData(enable ? LED_CHAN1_ENABLE : LED_CHAN1_DISABLE);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_EnableLEDTrig(bool enable)
{
    static bool enabled = false;

    if (enable != enabled)
    {
        enabled = enable;
        Panel_TransmitData(enable ? LED_TRIG_ENABLE : LED_TRIG_DISABLE);
        Display_EnableTrigLabel(enable);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_TransmitData(uint16 data)
{
    if(lastPos == MAX_DATA)
    {
        LOG_ERROR("Не могу послать в панель - мало места");
    }
    else
    {
        if (data == 0x04)
        {
            uint time = gTimerMS;
            while (gTimerMS - time < 200) {};
        }
        dataTransmitted[lastPos++] = data;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
uint16 Panel_NextData(void)
{
    if (lastPos > 0)
    {
        uint16 data = dataTransmitted[firstPos++];
        if(firstPos == lastPos)
        {
            firstPos = lastPos = 0;
        }
        return data;
    }
    return 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_Disable(void)
{
    isRunning = false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_Enable(void)
{
    isRunning = true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
/*
    SPI1
    56  - PG0 - программный NSS 
    41  - PA5 - SCK
    42  - PA6 - MISO
    135 - PB5 - MOSI
*/


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __SPI1_CLK_ENABLE();

    GPIO_InitTypeDef isGPIOA_B =
    {
        GPIO_PIN_5 | GPIO_PIN_6,    // GPIO_Pin
        GPIO_MODE_AF_PP,            // GPIO_Mode
        GPIO_PULLDOWN,
        GPIO_SPEED_FAST,            // GPIO_Speed
        GPIO_AF5_SPI1
    };
    HAL_GPIO_Init(GPIOA, &isGPIOA_B);
    
    isGPIOA_B.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOB, &isGPIOA_B);
    
    if (HAL_SPI_Init(&handleSPI) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    HAL_NVIC_SetPriority(SPI1_IRQn, PRIORITY_PANEL_SPI1);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);

    // Теперь настроим программный NSS (PB6).

    GPIO_InitTypeDef isGPIOG =
    {
        GPIO_PIN_6,                 // GPIO_Pin
        GPIO_MODE_IT_RISING,        // GPIO_Mode
        GPIO_NOPULL
    };      
    HAL_GPIO_Init(GPIOB, &isGPIOG);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, PRIORITY_PANEL_EXTI9_5);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    // Лампочка установка
    isGPIOG.Pin = GPIO_PIN_12;
    isGPIOG.Mode = GPIO_MODE_OUTPUT_PP;
    isGPIOG.Speed = GPIO_SPEED_HIGH;
    isGPIOG.Alternate = GPIO_AF0_MCO;
    HAL_GPIO_Init(GPIOG, &isGPIOG);

    Panel_EnableLEDRegSet(false);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_EnableLEDRegSet(bool enable)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
PanelButton Panel_WaitPressingButton(void)
{
    pressedButton = B_Empty;
    while (pressedButton == B_Empty) {};
    return pressedButton;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static uint8 dataSPIfromPanel;


//------------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    // Прерывание на SPI от панели управления
    if (pin == GPIO_PIN_6)
    {
        HAL_SPI_Receive_IT(&handleSPI, &dataSPIfromPanel, 1);
    }

    // Прерывание на чтение точки поточечного вывода
    if (pin == GPIO_PIN_2)
    {
        FPGA_ReadPoint();
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* handleSPI)
{
    if (!Panel_ProcessingCommandFromPIC(dataSPIfromPanel))
    {
        HAL_SPI_DeInit(handleSPI);
        HAL_SPI_Init(handleSPI);
    }

    SPI1->DR = Panel_NextData();
}
