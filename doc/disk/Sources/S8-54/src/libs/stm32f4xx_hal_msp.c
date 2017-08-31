// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Hardware/Hardware.h"
#include <stm32f4xx_hal.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef oscIS;
    RCC_PeriphCLKInitTypeDef periphClkIS;

    __PWR_CLK_ENABLE();

    HAL_PWR_EnableBkUpAccess();

    oscIS.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    oscIS.PLL.PLLState = RCC_PLL_NONE;
    oscIS.LSEState = RCC_LSE_ON;
    oscIS.LSIState = RCC_LSI_OFF;

    if (HAL_RCC_OscConfig(&oscIS) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    periphClkIS.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    periphClkIS.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&periphClkIS) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    __HAL_RCC_RTC_ENABLE();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
    __HAL_RCC_RTC_DISABLE();

    HAL_PWR_DisableBkUpAccess();

    __PWR_CLK_DISABLE();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{

}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{

}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    __HAL_RCC_FMC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitTypeDef isGPIOB =
    {
        GPIO_PIN_7,         // NL1
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FAST,
        GPIO_AF12_FMC
    };
    HAL_GPIO_Init(GPIOD, &isGPIOB);

    GPIO_InitTypeDef isGPIOD =
    {
        GPIO_PIN_0 |        // D2
        GPIO_PIN_1 |        // D3
        GPIO_PIN_4 |        // NOE/RD
        GPIO_PIN_5 |        // NWE/WR
        GPIO_PIN_7 |        // NE1
        GPIO_PIN_8 |        // D13
        GPIO_PIN_9 |        // D14
        GPIO_PIN_10 |       // D15
        GPIO_PIN_11 |       // A16
        GPIO_PIN_12 |       // A17
        GPIO_PIN_13 |       // A18
        GPIO_PIN_14 |       // D0
        GPIO_PIN_15,        // D1
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FAST,
        GPIO_AF12_FMC
    };
    HAL_GPIO_Init(GPIOD, &isGPIOD);

    GPIO_InitTypeDef isGPIOE =
    {
        GPIO_PIN_3 |        // A19
        GPIO_PIN_4 |        // A20
        GPIO_PIN_7 |        // D4
        GPIO_PIN_8 |        // D5
        GPIO_PIN_9 |        // D6
        GPIO_PIN_10 |       // D7
        GPIO_PIN_11 |       // D8
        GPIO_PIN_12 |       // D9
        GPIO_PIN_13 |       // D10
        GPIO_PIN_14 |       // D11
        GPIO_PIN_15,        // D12
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FAST,
        GPIO_AF12_FMC
    };
    HAL_GPIO_Init(GPIOE, &isGPIOE);

    GPIO_InitTypeDef isGPIOF =
    {
        GPIO_PIN_0 |        // A0
        GPIO_PIN_1 |        // A1
        GPIO_PIN_2 |        // A2
        GPIO_PIN_3 |        // A3
        GPIO_PIN_4 |        // A4
        GPIO_PIN_5 |        // A5
        GPIO_PIN_12 |       // A6
        GPIO_PIN_13 |       // A7
        GPIO_PIN_14 |       // A8
        GPIO_PIN_15,        // A9
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FAST,
        GPIO_AF12_FMC
    };
    HAL_GPIO_Init(GPIOF, &isGPIOF);

    GPIO_InitTypeDef isGPIOG =
    {
        GPIO_PIN_0 |        // A10
        GPIO_PIN_1 |        // A11
        GPIO_PIN_2 |        // A12
        GPIO_PIN_3 |        // A13
        GPIO_PIN_4 |        // A14
        GPIO_PIN_5,         // A15
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FAST,
        GPIO_AF12_FMC
    };
    HAL_GPIO_Init(GPIOG, &isGPIOG);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef *hsram)
{
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 ||
                    GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{
    GPIO_InitTypeDef structGPIO =
    {
        GPIO_PIN_4,
        GPIO_MODE_ANALOG,
        GPIO_NOPULL
    };

    HAL_GPIO_Init(GPIOA, &structGPIO);

    static DMA_HandleTypeDef hdmaDAC1 =
    {
        DMA1_Stream5,
        {
            DMA_CHANNEL_7,
            DMA_MEMORY_TO_PERIPH,
            DMA_PINC_DISABLE,
            DMA_MINC_ENABLE,
            DMA_PDATAALIGN_BYTE,
            DMA_MDATAALIGN_BYTE,
            DMA_CIRCULAR,
            DMA_PRIORITY_HIGH,
            DMA_FIFOMODE_DISABLE,
            DMA_FIFO_THRESHOLD_HALFFULL,
            DMA_MBURST_SINGLE,
            DMA_PBURST_SINGLE
        }
    };

    HAL_DMA_Init(&hdmaDAC1);

    __HAL_LINKDMA(hdac, DMA_Handle1, hdmaDAC1);

    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, PRIORITY_SOUND_DMA1_STREAM5);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac)
{

}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Инициализация флешки
void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
    GPIO_InitTypeDef isGPIO =
    {
        0,
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FAST
    };

    /*
    104 - PA12 - D+
    103 - PA11 - D-
    101 - PA9  - VBUS
    */

    __GPIOA_CLK_ENABLE();
    __USB_OTG_FS_CLK_ENABLE();
    __SYSCFG_CLK_ENABLE();

    isGPIO.Speed = GPIO_SPEED_HIGH;
    isGPIO.Pin = GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_12;
    isGPIO.Alternate = GPIO_AF10_OTG_FS;

    HAL_GPIO_Init(GPIOA, &isGPIO);

    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    HAL_NVIC_SetPriority(OTG_FS_IRQn, PRIORITY_FLASHDRIVE_OTG);

    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// Деинициализация флешки
void HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd)
{
    __USB_OTG_FS_CLK_DISABLE();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Инициализаця VCP
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
    __GPIOB_CLK_ENABLE();
    __USB_OTG_HS_CLK_ENABLE();
    __SYSCFG_CLK_ENABLE();

    GPIO_InitTypeDef  isGPIO =
    {
        GPIO_PIN_14 | GPIO_PIN_15,
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        GPIO_SPEED_HIGH,
        GPIO_AF12_OTG_HS_FS,
    };

    HAL_GPIO_Init(GPIOB, &isGPIO);

    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();

    HAL_NVIC_SetPriority(OTG_HS_IRQn, PRIORITY_VCP_OTG);

    HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
    __USB_OTG_HS_CLK_DISABLE();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Инициализация Ethernet
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIOs clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE(); 

/* Ethernet pins configuration ************************************************/
  /*
        MDIO -----> PA2
        MDC ------> PC1
        RXD2 -----> PB0
        RXD3 -----> PB1
        TX_CLK ---> PC3
        TXD2 -----> PC2
        TXD3 -----> PB8
        RX_CLK ---> PA1
        RX_DV ----> PA7
        RXD0 -----> PC4
        RXD1 -----> PC5
        TX_EN ----> PB11
        TXD0 -----> PG13
        TXD1 -----> PG14
        RX_ER ----> PB10
        CRS ------> PA0
        COL ------> PA3
  */
  
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL; 
  GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
  GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_11;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.Pin =  GPIO_PIN_13 | GPIO_PIN_14;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

   __HAL_RCC_ETH_CLK_ENABLE();
  
  if(heth->Init.MediaInterface == ETH_MEDIA_INTERFACE_MII)
  {
      HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
  }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// Деиницилизация Ethernet
void HAL_ETH_MspDeInit(ETH_HandleTypeDef *heth)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc)
{
    __HAL_RCC_CRC_CLK_ENABLE();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc)
{
    __HAL_RCC_CRC_FORCE_RESET();
    __HAL_RCC_CRC_RELEASE_RESET();
}
