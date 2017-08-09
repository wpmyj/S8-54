// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Log.h"
#include "Panel.h"
#include "PanelFunctions.h"
#include "Display/PainterDataNew.h"
#include "FPGA/FPGA.h"
#include "Hardware/Hardware.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Menu/Menu.h"
#include "Utils/GlobalFunctions.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void OnPress_ResetSettings(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_DATA            20

#define LED_CHAN0_ENABLE    129
#define LED_CHAN0_DISABLE   1
#define LED_CHAN1_ENABLE    130
#define LED_CHAN1_DISABLE   2
#define LED_TRIG_ENABLE     131
#define LED_TRIG_DISABLE    3
#define POWER_OFF           4


static PanelButton pressedKey = B_Empty;
static volatile PanelButton pressedButton = B_Empty;    ///< Используется для отслеживания нажатой кнопки при отключенной панели.
static uint16 dataTransmitted[MAX_DATA] = {0x00};
static uint16 firstPos = 0;
static uint16 lastPos = 0;


// В этих переменных сохраняем значения в прерывании
static PanelButton releaseButton = B_Empty;
static PanelButton pressButton = B_Empty;
static PanelRegulator regLeft = R_Empty;
static PanelRegulator regRight = R_Empty;
static int numReg = 0;                                  ///< Число поворотов ручки.
static PanelRegulator regPress = R_Empty;
static PanelRegulator regRelease = R_Empty;
static PanelCommand recvCommand = C_None;

static int allRecData = 0;
static bool isRunning = true;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    void(*funcOnKey)(int);          ///< Функция вызывается при нажатии(1) / отпускании(-1) кнопки.
    void(*funcLongPressure)(void);  ///< Функция выывается при длительном удержании кнопки.
} StructButton;


static const StructButton funcButton[B_NumButtons] =
{
    {0, 0},
    {EFB,           Long_ChannelA}, ///< B_Channel1
    {EFB,           EmptyFuncVV},   ///< B_Service
    {EFB,           Long_ChannelB}, ///< B_Channel2
    {EFB,           EmptyFuncVV},   ///< B_Display
    {EFB,           Long_Time},     ///< B_Time
    {EFB,           EmptyFuncVV},   ///< B_Memory
    {EFB,           Long_Trig},     ///< B_Sinchro
    {Func_Start,    Long_Start},    ///< B_Start
    {EFB,           EmptyFuncVV},   ///< B_Cursors
    {EFB,           EmptyFuncVV},   ///< B_Measures
    {Func_Power,    EmptyFuncVV},   ///< B_Power
    {EFB,           Long_Help},     ///< B_Help
    {EFB,           Long_Menu},     ///< B_Menu
    {EFB,           F1_Long},       ///< B_F1
    {EFB,           F2_Long},       ///< B_F2
    {EFB,           F3_Long},       ///< B_F2
    {EFB,           F4_Long},       ///< B_F3
    {EFB,           F5_Long}        ///< B_F4
};


typedef struct
{
    void(*rotate)(int delta);       ///< Эта функция вызывается при повороте ручки.
    void(*press)(int delta);        ///< Функция вызывается при нажатии/отпускании ручки.
    void(*longPress)(void);         ///< Эта функция вызывается при длительном нажатии ручки.
} StructReg;

/** @todo Убрать дублирование*/
static const StructReg funculatorReg[] =
{
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {FuncRange1,    EFB,                EmptyFuncVV},   ///< R_Range1
    {FuncRShift1,   FuncBtnRegChannel1, EmptyFuncVV},   ///< R_RShift1
    {FuncRange2,    EFB,                EmptyFuncVV},   ///< R_Range2
    {FuncRShift2,   FuncBtnRegChannel2, EmptyFuncVV},   ///< R_RShift2
    {FuncTBase,     EFB,                EmptyFuncVV},   ///< R_TBase
    {FuncTShift,    FuncBtnRegTime,     EmptyFuncVV},   ///< R_TShift
    {FuncTrigLev,   FuncBtnRegTrig,     EmptyFuncVV},   ///< R_TrigLev
    {FuncRegSet,    FuncBtnRegSet,      EmptyFuncVV},   ///< R_Set
    {FuncRange1,    EFB,                EmptyFuncVV},   ///< R_Range1
    {FuncRShift1,   FuncBtnRegChannel1, EmptyFuncVV},   ///< R_RShift1
    {FuncRange2,    EFB,                EmptyFuncVV},   ///< R_Range2
    {FuncRShift2,   FuncBtnRegChannel2, EmptyFuncVV},   ///< R_RShift2
    {FuncTBase,     EFB,                EmptyFuncVV},   ///< R_TBase
    {FuncTShift,    FuncBtnRegTime,     EmptyFuncVV},   ///< R_TShift
    {FuncTrigLev,   FuncBtnRegTrig,     EmptyFuncVV},   ///< R_TrigLev
    {FuncRegSet,    FuncBtnRegSet,      EmptyFuncVV}    ///< R_Set
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static PanelButton ButtonIsRelease(uint16 command)
{
    PanelButton button = B_Empty;

    static uint timePrevReleaseButton = 0;

    if(command < B_NumButtons && command > B_Empty)
    {
        if(gTimeMS - timePrevReleaseButton > 100)
        {
            button = (PanelButton)command;
            timePrevReleaseButton = gTimeMS;
        }
    }

    return button;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static PanelButton ButtonIsPress(uint16 command)
{
    PanelButton button = B_Empty;

    static uint timePrevPressButton = 0;

    if (command < (B_NumButtons | 0x80) && command > (B_Empty | 0x80))
    {
        if(gTimeMS - timePrevPressButton > 100)
        {
            button = (PanelButton)(command & 0x7f);
            timePrevPressButton = gTimeMS;
        }
    }

    return button;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static PanelRegulator RegulatorPress(uint16 command)
{
    if (command >= (0x1c | 0x80) && command <= (0x23 | 0x80))
    {
        return (PanelRegulator)(command & 0x7f);
    }
    return R_Empty;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static PanelRegulator RegulatorRelease(uint16 command)
{
    if (command >= 0x1c && command <= 0x23)
    {
        return (PanelRegulator)command;
    }
    return R_Empty;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static PanelCommand ReceiveCommand(uint16 command)
{
    if (command == C_Reset)
    {
        return C_Reset;
    }
    return C_None;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static PanelRegulator RegulatorLeft(uint16 command)
{
    if(command >= 20 && command <= 27)
    {
        return (PanelRegulator)command;
    }
    return R_Empty;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static PanelRegulator RegulatorRight(uint16 command)
{
    if(command >= (20 | 0x80)  && command <= (27 | 0x80))
    {
        return (PanelRegulator)(command & 0x7f);
    }
    return R_Empty;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnTimerPressedKey(void)
{
    if(pressedKey != B_Empty)
    {
        void (*func)(void) = funcButton[pressedKey].funcLongPressure;
        if(func)
        {
            func();
        }
        else
        {
            Menu_ReleaseButton(pressedKey);
        }
        pressedKey = B_Empty;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Panel_ProcessingCommandFromPIC(uint16 command)
{
    if (command != 0)
    {
        PainterDataNew_InterruptDrawing();

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
                PanelRegulator rLeft = RegulatorLeft(command);
                if (rLeft)
                {
                    regLeft = rLeft;
                    numReg++;
                }
                else
                {
                    PanelRegulator rRight = RegulatorRight(command);
                    if (rRight)
                    {
                        regRight = rRight;
                        numReg++;
                    }
                    else
                    {
                        PanelRegulator rPress = RegulatorPress(command);
                        if (rPress)
                        {
                            regPress = rPress;
                        }
                        else
                        {
                            PanelRegulator rRelease = RegulatorRelease(command);
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
                                        LOG_ERROR_TRACE("Ошибок SPI - %s %d/%d, command = %d", buffer, errRecData, allRecData, (int)command);
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ProcessingCommand(void)
{
    if (recvCommand == C_Reset)
    {
        Settings_Load(true);
        recvCommand = C_None;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_EnableLEDChannelA(bool enable)
{
    Panel_TransmitData(enable ? LED_CHAN0_ENABLE : LED_CHAN0_DISABLE);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_TransmitData(uint16 data)
{
    if(lastPos == MAX_DATA)
    {
        LOG_ERROR_TRACE("Не могу послать в панель - мало места");
    }
    else
    {
        if (data == 0x04) //-V112
        {
            uint time = gTimeMS;
            while (gTimeMS - time < 200) {};
        }
        dataTransmitted[lastPos++] = data;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_Disable(void)
{
    isRunning = false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_Enable(void)
{
    isRunning = true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
/*
    SPI1
    56  - PG0 - программный NSS 
    41  - PA5 - SCK
    42  - PA6 - MISO
    135 - PB5 - MOSI
*/


//----------------------------------------------------------------------------------------------------------------------------------------------------
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_EnableLEDRegSet(bool enable)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
PanelButton Panel_WaitPressingButton(void)
{
    pressedButton = B_Empty;
    while (pressedButton == B_Empty) {};
    return pressedButton;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_DisableIfNessessary(void)
{
    if (NEED_DISABLE_POWER)
    {
        Settings_Save();
        Log_DisconnectLoggerUSB();
        Panel_TransmitData(0x04);
        while (1)
        {
        };
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint8 dataSPIfromPanel;


//----------------------------------------------------------------------------------------------------------------------------------------------------
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* handleSPI)
{
    if (!Panel_ProcessingCommandFromPIC(dataSPIfromPanel))
    {
        HAL_SPI_DeInit(handleSPI);
        HAL_SPI_Init(handleSPI);
    }

    SPI1->DR = Panel_NextData();
}


