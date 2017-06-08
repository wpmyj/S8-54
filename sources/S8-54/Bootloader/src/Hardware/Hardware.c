#include "defines.h"

#include "Display/Display.h"

#include "FlashDrive/FlashDrive.h"

#include "Hardware.h"
#include "Hardware/FLASH.h"
#include "Hardware/FSMC.h"
#include "Hardware/Timer.h"
#include "Panel/Panel.h"
#include "Utils/GlobalFunctions.h"

#include <stm32f437xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_dac.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig (&RCC_OscInitStruct);
    
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK |RCC_CLOCKTYPE_HCLK |RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Hardware_Init(void)
{
    HAL_Init();

    SystemClock_Config();

    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();
    __DMA1_CLK_ENABLE();        // Для DAC1 (бикалка)
    
    __TIM7_CLK_ENABLE();        // Для DAC1 (бикалка)
    __DAC_CLK_ENABLE();         // Для бикалки
    __PWR_CLK_ENABLE();

    __SYSCFG_CLK_ENABLE();

    HAL_NVIC_SetPriority(SysTick_IRQn, PRIORITY_SYS_TICK);

    // Timer  /////////////////////////////////////////////////////////////////
    //RCC_PCLK1Config(RCC_HCLK_Div1);

    // Таймер для мс
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, PRIORITY_TIMER_TIM6);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

    Timer_Init();
  
    Panel_Init();
      
    FSMC_Init();
    
    FSMC_SetMode(ModeFSMC_FPGA);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OTG_FS_IRQHandler(void)
{
    HAL_HCD_IRQHandler(&handleHCD);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void SysTick_Handler(void)
{
    HAL_IncTick();
}
