#include "Panel.h"
#include "Controls.h"
#include "Display/Display.h"
//#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Hardware/Timer.h"
#include "Hardware/Hardware.h"
#include "Hardware/FSMC.h"

#include <string.h>

#include "PanelFunctions.c"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_DATA            20

#define LED_CHANA_ENABLE    129
#define LED_CHANA_DISABLE   1
#define LED_CHANB_ENABLE    130
#define LED_CHANB_DISABLE   2
#define LED_TRIG_ENABLE     131
#define LED_TRIG_DISABLE    3
#define POWER_OFF           4

static uint16 firstPos = 0;
static uint16 lastPos = 0;
static uint16 dataTransmitted[MAX_DATA] = {0x00};
volatile static bool isRunning = true;
static PanelButton pressedButton = B_Empty;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


bool Panel_ProcessingCommandFromPIC(uint16 command)
{
    if (command != 0)
    {
        PanelButton prButton = ButtonIsPress(command);
        if (prButton)
        {
            pressedButton = prButton;
        }
        else
        {
            pressedButton = B_Empty;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_TransmitData(uint16 data)
{
    if(lastPos == MAX_DATA)
    {
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
    
    HAL_SPI_Init(&handleSPI);

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
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_DeInit(void)
{
    HAL_NVIC_DisableIRQ(SPI1_IRQn);
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Panel_EnableLEDRegSet(bool enable)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
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
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* handleSPI)
{
    if (!Panel_ProcessingCommandFromPIC(dataSPIfromPanel))
    {
        HAL_SPI_DeInit(handleSPI);
        HAL_SPI_Init(handleSPI);
    }

    SPI1->DR = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// This interrupt call soft NSS for spi (see Hardware::SPIforPanel.c::
// PanelInit() and HAL_GPIO_EXTI_Callback())
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// See Hardware::SPIforPanel.c::HAL_SPI_RxCpltCallback()
void SPI1_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&handleSPI);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
PanelButton Panel_PressedButton(void)
{
    return pressedButton;
}
