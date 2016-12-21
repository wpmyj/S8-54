
/*!*****************************************************************************
 @file		Siluet_Control.c
 @author	Козлячков В.К.
 @date		2012.10
 @version	V0.5 
 @brief		Initial & Control Functions (MCU, SFM, PL)
 @details	Функции инициализации и управления 
 @note		Hi-Level & Low-Level Functions
 *******************************************************************************
 @verbatim 
 MCU - функции микроконтроллера
 XRAM - функции работы с внешней памятью
 SFM - функции работы с последовательной флэш-памятью
 PL - функции работы с ПЛИС
 FPС - функции работы с контроллером панели управления
 @endverbatim
 *******************************************************************************/
 
/*! @addtogroup	Control
    @{ */

/*__ Заголовочный файл ________________________________________________________*/

#include "Siluet_Main.h"		// все включения заданы в одном файле

/*__ Локальные типы данных ____________________________________________________*/

/* Биты конфигурации портов (* - после сброса)
   см. Table 8. Alternate function mapping в datasheet STM32F205XX */
   
//! GPIO Mode Type
typedef enum {
  MD_IN,				//!< Input*
  MD_GO,				//!< Output
  MD_AF,				//!< Alternate
  MD_AN					//!< Analog  
} GPIO_MODE_t;

//! GPIO Output Type
typedef enum {
  OT_PP,				//!< Push-Pull*
  OT_OD					//!< Open-Drive
} GPIO_OTYPE_t;

//! GPIO Output Speed Type
typedef enum {
  SP_02,				//!< 2 MHz*
  SP_25,				//!< 25 MHz
  SP_50,				//!< 50 MHz
  SP_80					//!< 100/80 MHz
} GPIO_OSPEED_t;			

//! GPIO Pull-Up_Pull-Down Type
typedef enum {
  PL_NP,				//!< No_Pull*
  PL_PU,				//!< Pull-Up
  PL_PD					//!< Pull-Down
					// Reserved
} GPIO_PUPD_t;

//! GPIO Alternate Functions
typedef enum {
  AF_SYSTEM,				//!< AF0 - SYSTEM*
  AF_TIM1,				//!< AF1 - TIM1/2
  AF_TIM2 = 1,				//!< AF1 - TIM1/2
  AF_TIM3,				//!< AF2 - TIM3/4/5
  AF_TIM4 = 2,				//!< AF2 - TIM3/4/5
  AF_TIM5 = 2,				//!< AF2 - TIM3/4/5
  AF_TIM8,				//!< AF3 - TIM9/10/11
  AF_TIM9 = 3,				//!< AF3 - TIM9/10/11
  AF_TIM10 = 3,				//!< AF3 - TIM9/10/11
  AF_TIM11 = 3,				//!< AF3 - TIM9/10/11
  AF_I2C1,				//!< AF4 - I2C1/2/3
  AF_I2C2 = 4,				//!< AF4 - I2C1/2/3
  AF_I2C3 = 4,				//!< AF4 - I2C1/2/3
  AF_SPI1,				//!< AF5 - SPI1/2
  AF_SPI2 = 5,				//!< AF5 - SPI1/2
  AF_SPI3,				//!< AF6 - SPI3
  AF_USART1,				//!< AF7 - USART1/2/3
  AF_USART2 = 7,			//!< AF7 - USART1/2/3
  AF_USART3 = 7,			//!< AF7 - USART1/2/3 
  AF_USART4,				//!< AF8 - USART4/5/6
  AF_USART5 = 8,			//!< AF8 - USART4/5/6
  AF_USART6 = 8,			//!< AF8 - USART4/5/6
  AF_CAN1,				//!< AF9 - CAN1/2
  AF_CAN2 = 9,				//!< AF9 - CAN1/2
  AF_TIM12 = 9,				//!< AF9 - TIM12/13/14
  AF_TIM13 = 9,				//!< AF9 - TIM12/13/14
  AF_TIM14 = 9,				//!< AF9 - TIM12/13/14
  AF_OTGFS,				//!< AF10 - OTGFS
  AF_OTGHS = 10,			//!< AF10 - OTGHS
  AF_ETH,				//!< AF11 - ETH
  AF_FSMC,				//!< AF12 - FSMC
  AF_SDIO = 12,				//!< AF12 - SDIO
  AF_OTGHS_ = 12,			//!< AF12 - OTGHS
  AF_DCMI,				//!< AF13 - DCMI
  AF_AF14,				//!< AF14 - RI
  AF_EVENT				//!< AF15 - SYSTEM (EVENTOUT)
} GPIO_AFLH_t;


/*__ Макрофункции _____________________________________________________________*/

/*!*****************************************************************************
 @brief		Port configuration
 @details	Конфигурация портов целиком
 @param		PORT - имя порта (A..G)
 @param		MDRx, OTRx, SPRx, PLRx, AFRx - режим x-битов порта
 @return	
 @note		Используются перечисляемые типы
 */
#define GPIO_INIT(PORT, 			\
	MDR0,  OTR0,  SPR0,  PLR0,  AFR0,	\
	MDR1,  OTR1,  SPR1,  PLR1,  AFR1,	\
	MDR2,  OTR2,  SPR2,  PLR2,  AFR2,	\
	MDR3,  OTR3,  SPR3,  PLR3,  AFR3,	\
	MDR4,  OTR4,  SPR4,  PLR4,  AFR4,	\
	MDR5,  OTR5,  SPR5,  PLR5,  AFR5,	\
	MDR6,  OTR6,  SPR6,  PLR6,  AFR6,	\
	MDR7,  OTR7,  SPR7,  PLR7,  AFR7,	\
	MDR8,  OTR8,  SPR8,  PLR8,  AFR8,	\
	MDR9,  OTR9,  SPR9,  PLR9,  AFR9,	\
	MDR10, OTR10, SPR10, PLR10, AFR10,	\
	MDR11, OTR11, SPR11, PLR11, AFR11,	\
	MDR12, OTR12, SPR12, PLR12, AFR12,	\
	MDR13, OTR13, SPR13, PLR13, AFR13,	\
	MDR14, OTR14, SPR14, PLR14, AFR14,	\
	MDR15, OTR15, SPR15, PLR15, AFR15);	\
GPIO##PORT->MODER = ((uint32_t)					\
	MDR15<<30 | MDR14<<28 | MDR13<<26 | MDR12<<24 |		\
	MDR11<<22 | MDR10<<20 | MDR9 <<18 | MDR8 <<16 |		\
	MDR7 <<14 | MDR6 <<12 | MDR5 <<10 | MDR4 << 8 |		\
	MDR3 << 6 | MDR2 << 4 | MDR1 << 2 | MDR0);		\
GPIO##PORT->OTYPER = ((uint32_t)				\
	OTR15<<15 | OTR14<<14 | OTR13<<13 | OTR12<<12 |		\
	OTR11<<11 | OTR10<<10 | OTR9 << 9 | OTR8 << 8 |		\
	OTR7 << 7 | OTR6 << 6 | OTR5 << 5 | OTR4 << 4 |		\
	OTR3 << 3 | OTR2 << 2 | OTR1 << 1 | OTR0);		\
GPIO##PORT->OSPEEDR = ((uint32_t)				\
	SPR15<<30 | SPR14<<28 | SPR13<<26 | SPR12<<24 |		\
	SPR11<<22 | SPR10<<20 | SPR9 <<18 | SPR8 <<16 |		\
	SPR7 <<14 | SPR6 <<12 | SPR5 <<10 | SPR4 << 8 |		\
	SPR3 << 6 | SPR2 << 4 | SPR1 << 2 | SPR0);		\
GPIO##PORT->PUPDR = ((uint32_t)					\
	PLR15<<30 | PLR14<<28 | PLR13<<26 | PLR12<<24 |		\
	PLR11<<22 | PLR10<<20 | PLR9 <<18 | PLR8 <<16 |		\
	PLR7 <<14 | PLR6 <<12 | PLR5 <<10 | PLR4 << 8 |		\
	PLR3 << 6 | PLR2 << 4 | PLR1 << 2 | PLR0);		\
GPIO##PORT->AFR[0] = ((uint32_t)				\
	AFR7 <<28 | AFR6 <<24 | AFR5 <<20 | AFR4 <<16 |		\
	AFR3 <<12 | AFR2 << 8 | AFR1 << 4 | AFR0);		\
GPIO##PORT->AFR[1] = ((uint32_t)				\
	AFR15<<28 | AFR14<<24 | AFR13<<20 | AFR12<<16 |		\
	AFR11<<12 | AFR10<< 8 | AFR9 << 4 | AFR8);

/*!*****************************************************************************
 @brief		Port configuration lock
 @details	Зайфиксировать конфигурацию портов
 @param		PORT - имя порта (A..G)
 @param		BITS - биты порта, которые нужно зафиксировать (0xffff - все)
 @return	
 @note		Для чтения используется временная переменная temporal
 */
#define GPIO_LOCK(PORT, BITS);		\
GPIO##PORT->LCKR = 0x00010000 | BITS;	\
GPIO##PORT->LCKR = 0x00000000 | BITS;	\
GPIO##PORT->LCKR = 0x00010000 | BITS;	\
{					\
  register volatile uint32_t temporal;	\
  temporal = GPIO##PORT->LCKR;		\
}


/*__ Определения констант _____________________________________________________*/ 

/*__ Локальные прототипы функций ______________________________________________*/

//__ SFM __//

void SFMWr_enable(void);
void SFMWrEnd_wait(void);
// void SFMSel_low(void);
// void SFMSel_high(void);
uint8_t SFMByte_send(uint8_t data);

//__ PL __//

uint8_t PLPSByte_send(uint8_t data);	// Послать байт данных в ПЛИС

/*__ Глобальные функции _______________________________________________________*/

//__ MCU __//

/*!*****************************************************************************
 @brief		Setup the microcontroller system
 @details	Initialize the Embedded Flash Interface and the PLL
 @param		none
 @retval	
 @note		Частота делится до 1 MHz, затем умножается до 240 MHz
		PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
		Системная частота должна быть 240 / 2 = 120 MHz
		SYSCLK = PLL_VCO / PLL_P
		Частота USB должна быть 240 / 5 = 48 MHz
		USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ
 */
void SystemInit(void) {

/* Вернуть работу от внутреннего генератора HSI
   Reset the RCC clock configuration to the default reset state */
  RCC->CR |= RCC_CR_HSION;		// HSI oscillator ON
  RCC->CFGR = 0x00000000;		// Reset CFGR register (HSI - system clock)
  RCC->CR &= ~(				// Reset HSEON, CSSON and PLLON bits
	RCC_CR_HSEON |
	RCC_CR_CSSON |
	RCC_CR_PLLON);
  RCC->PLLCFGR = 0x24003010;		// Reset PLLCFGR register
  RCC->CR &= ~RCC_CR_HSEBYP;		// Reset HSEBYP bit (only if HSE is disabled)
  RCC->CIR = 0xFFFF0000;		// Reset and disable all interrupts
         
/* Configure the System clock source, PLL Multiplier and Divider factors, 
   AHB/APBx prescalers and Flash settings */
  volatile uint32_t StartUpCounter = HSE_STARTUP_TIMEOUT;
  RCC->CR |= RCC_CR_HSEON;		// Enable HSE

/* Wait till HSE is ready and if Time out is reached exit */
  while (!(RCC->CR & RCC_CR_HSERDY) && StartUpCounter--);
  
  if (RCC->CR & RCC_CR_HSERDY) {
    RCC->CFGR =				// Configure clock dividers
	RCC_CFGR_HPRE_0 * 0 |
	RCC_CFGR_HPRE_1 * 0 |
	RCC_CFGR_HPRE_2 * 0 |
	RCC_CFGR_HPRE_3 * 0 |		// HCLK - system clock not divided, 120 MHz
	RCC_CFGR_PPRE1_0 * 1 |
	RCC_CFGR_PPRE1_1 * 0 |
	RCC_CFGR_PPRE1_2 * 1 |		// PCLK1 - AHB clock / 4 = 30 MHz (max 30 MHz)
	RCC_CFGR_PPRE2_0 * 0 |
	RCC_CFGR_PPRE2_1 * 0 |
	RCC_CFGR_PPRE2_2 * 1 |		// PCLK2 - AHB clock / 2 = 60 MHz (max 60 MHz)
	RCC_CFGR_RTCPRE_0 * 0 |
	RCC_CFGR_RTCPRE_1 * 0 |
	RCC_CFGR_RTCPRE_2 * 0 |
	RCC_CFGR_RTCPRE_3 * 0 |
	RCC_CFGR_RTCPRE_4 * 0 |		// no clock
	RCC_CFGR_MCO1_0 * 1 |
	RCC_CFGR_MCO1_1 * 1 |		// PLL clock selected
	RCC_CFGR_I2SSRC * 0 |		// PLLI2S clock used as I2S clock source
	RCC_CFGR_MCO1PRE_0 * 1 |
	RCC_CFGR_MCO1PRE_1 * 0 |
	RCC_CFGR_MCO1PRE_2 * 1 |	// MCO1 - (PLLCLK) / 3 = 40 MHz (max 100 MHz)
	RCC_CFGR_MCO2PRE_0 * 0 |
	RCC_CFGR_MCO2PRE_1 * 0 |
	RCC_CFGR_MCO2PRE_2 * 0 |	// MCO2 - / 1 (max 100 MHz)
	RCC_CFGR_MCO2_0 * 1 |
	RCC_CFGR_MCO2_1 * 0;		// PLLI2S clock selected 
    RCC->PLLCFGR = 			// Configure the main PLL
	8 |				// PLLM = 8
	240 << 6 |			// PLLN = 240
	RCC_PLLCFGR_PLLP_0 * 0 |
	RCC_PLLCFGR_PLLP_1 * 0 |	// PLLP = 2
	RCC_PLLCFGR_PLLSRC_HSE * 1 |	// HSE selected as PLL and PLLI2S clock entry
	5 << 24;			// PLLQ = 5
    RCC->CR |= RCC_CR_CSSON;		// Включить защиту HSE от сбоев
    ErrorState = ERROR_NONE;		// Нет ошибки генератора
  }
  else {
    RCC->CFGR =				// Configure clock dividers
	RCC_CFGR_HPRE_0 * 0 |
	RCC_CFGR_HPRE_1 * 0 |
	RCC_CFGR_HPRE_2 * 0 |
	RCC_CFGR_HPRE_3 * 0 |		// HCLK - system clock not divided, 120 MHz
	RCC_CFGR_PPRE1_0 * 1 |
	RCC_CFGR_PPRE1_1 * 0 |
	RCC_CFGR_PPRE1_2 * 1 |		// PCLK1 - AHB clock / 4 = 30 MHz (max 30 MHz)
	RCC_CFGR_PPRE2_0 * 0 |
	RCC_CFGR_PPRE2_1 * 0 |
	RCC_CFGR_PPRE2_2 * 1 |		// PCLK2 - AHB clock / 2 = 60 MHz (max 60 MHz)
	RCC_CFGR_RTCPRE_0 * 0 |
	RCC_CFGR_RTCPRE_1 * 0 |
	RCC_CFGR_RTCPRE_2 * 0 |
	RCC_CFGR_RTCPRE_3 * 0 |
	RCC_CFGR_RTCPRE_4 * 0 |		// no clock
	RCC_CFGR_MCO1_0 * 1 |
	RCC_CFGR_MCO1_1 * 1 |		// PLL clock selected
	RCC_CFGR_I2SSRC * 0 |		// PLLI2S clock used as I2S clock source
	RCC_CFGR_MCO1PRE_0 * 1 |
	RCC_CFGR_MCO1PRE_1 * 0 |
	RCC_CFGR_MCO1PRE_2 * 1 |	// MCO1 - (PLLCLK) / 3 = 40 MHz (max 100 MHz)
	RCC_CFGR_MCO2PRE_0 * 0 |
	RCC_CFGR_MCO2PRE_1 * 0 |
	RCC_CFGR_MCO2PRE_2 * 0 |	// MCO2 - / 1 (max 100 MHz)
	RCC_CFGR_MCO2_0 * 1 |
	RCC_CFGR_MCO2_1 * 0;		// PLLI2S clock selected 
    RCC->PLLCFGR = 			// Configure the main PLL
	16 |				// PLLM = 16
	240 << 6 |			// PLLN = 240
	RCC_PLLCFGR_PLLP_0 * 0 |
	RCC_PLLCFGR_PLLP_1 * 0 |	// PLLP = 2
	RCC_PLLCFGR_PLLSRC_HSI * 0 |	// HSI selected as PLL and PLLI2S clock entry
	5 << 24;			// PLLQ = 5
    ErrorState = ERROR_XTAL;		// Ошибка HSE генератора
  }
  RCC->CR |= RCC_CR_PLLON;		// Enable the main PLL
  while (!(RCC->CR & RCC_CR_PLLRDY));	// Wait till the main PLL is ready

/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
  FLASH->ACR =
	FLASH_ACR_LATENCY_3WS |		// Three wait states
	FLASH_ACR_PRFTEN * 1 |		// Prefetch enable
	FLASH_ACR_ICEN * 1 |		// Instruction cache enable
	FLASH_ACR_DCEN * 1;		// Data cache enable

/* Select the main PLL as system clock source */
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;

/* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

/*!*****************************************************************************
 @brief		MCU initialize
 @details	Инициализировать микроконтроллер
 @param		none
 @return	
 @note		Узлы заданы в алфавитном порядке, кроме тактов
 */
void MCU_init(void) {

//* RCC *//
/* Инициализировать регистры частот (до программирования всего остального!)
   Такты для периферийных устройств включаются одной командой
   RCC_AHB1RSTR, RCC_AHB2RSTR, RCC_AHB3RSTR - регистры сброса, не используются */
  RCC->AHB1ENR =
	RCC_AHB1ENR_GPIOAEN * 1 |
	RCC_AHB1ENR_GPIOBEN * 1 |
	RCC_AHB1ENR_GPIOCEN * 1 |
	RCC_AHB1ENR_GPIODEN * 1 |
	RCC_AHB1ENR_GPIOEEN * 1 |
	RCC_AHB1ENR_GPIOFEN * 1 |
	RCC_AHB1ENR_GPIOGEN * 1 |
	RCC_AHB1ENR_GPIOHEN * 0 |
	RCC_AHB1ENR_GPIOIEN * 0 |
	RCC_AHB1ENR_CRCEN * 0 |
	RCC_AHB1ENR_BKPSRAMEN * 1 |	// Backup SRAM clock
	RCC_AHB1ENR_DMA1EN * 1 |
	RCC_AHB1ENR_DMA2EN * 1 |
	RCC_AHB1ENR_ETHMACEN * 0 |
	RCC_AHB1ENR_ETHMACTXEN * 0 |
	RCC_AHB1ENR_ETHMACRXEN * 0 |
	RCC_AHB1ENR_ETHMACPTPEN * 0 |
	RCC_AHB1ENR_OTGHSEN * 1 |	// USB HS module clock enabled
	RCC_AHB1ENR_OTGHSULPIEN * 0;
  RCC->AHB2ENR = 0x00000000;
  RCC->AHB3ENR = RCC_AHB3ENR_FSMCEN;
  RCC->APB1ENR =			// 30 MHz
	RCC_APB1ENR_TIM2EN * 0 |
	RCC_APB1ENR_TIM3EN * 0 |
	RCC_APB1ENR_TIM4EN * 0 |
	RCC_APB1ENR_TIM5EN * 0 |
	RCC_APB1ENR_TIM6EN * 1 |	// Таймер для формирования задержек
	RCC_APB1ENR_TIM7EN * 1 |
	RCC_APB1ENR_TIM12EN * 0 |
	RCC_APB1ENR_TIM13EN * 0 |
	RCC_APB1ENR_TIM14EN * 0 |
	RCC_APB1ENR_WWDGEN * 1 |
	RCC_APB1ENR_SPI2EN * 1 |
	RCC_APB1ENR_SPI3EN * 1 |
	RCC_APB1ENR_USART2EN * 1 |
	RCC_APB1ENR_USART3EN * 1 |
	RCC_APB1ENR_UART4EN * 0 |
	RCC_APB1ENR_UART5EN * 0 |
	RCC_APB1ENR_I2C1EN * 0 |
	RCC_APB1ENR_I2C2EN * 0 |
	RCC_APB1ENR_I2C3EN * 0 |
	RCC_APB1ENR_CAN1EN * 0 |
	RCC_APB1ENR_CAN2EN * 0 |
	RCC_APB1ENR_PWREN * 1 |		// Управление питанием (LSE генератором)
	RCC_APB1ENR_DACEN * 0;
  RCC->APB2ENR =			// 60 MHz 
	RCC_APB2ENR_TIM1EN * 0 |
	RCC_APB2ENR_TIM8EN * 0 |
	RCC_APB2ENR_USART1EN * 0 |
	RCC_APB2ENR_USART6EN * 0 |
	RCC_APB2ENR_ADC1EN * 0 |
	RCC_APB2ENR_ADC2EN * 1 |	// ADC2_IN15 - ITAN
	RCC_APB2ENR_ADC3EN * 0 |
	RCC_APB2ENR_SDIOEN * 0 |
	RCC_APB2ENR_SPI1EN * 1 |
	RCC_APB2ENR_SYSCFGEN * 1 |
	RCC_APB2ENR_TIM11EN * 0 |
	RCC_APB2ENR_TIM10EN * 0 |
	RCC_APB2ENR_TIM9EN * 0;

//* ADC *//

  ADC2->CR1 =
	ADC_CR1_AWDCH_0		* 00 |	// Analog watchdog channel select bits (0..18)
	ADC_CR1_EOCIE		*  0 |	// Interrupt enable for EOC
	ADC_CR1_AWDIE		*  0 |	// Analog Watchdog interrupt enable
	ADC_CR1_JEOCIE		*  0 |	// Interrupt enable for injected channels
	ADC_CR1_SCAN		*  0 |	// Scan mode
	ADC_CR1_AWDSGL		*  0 |	// Enable the watchdog on a single channel in scan mode
	ADC_CR1_JAUTO		*  0 |	// Automatic injected group conversion
	ADC_CR1_DISCEN		*  0 |	// Discontinuous mode on regular channels
	ADC_CR1_JDISCEN		*  0 |	// Discontinuous mode on injected channels
	ADC_CR1_DISCNUM_0	* 00 |	// Discontinuous mode channel count (0..7)
	ADC_CR1_JAWDEN		*  0 |	// Analog watchdog enable on injected channels
	ADC_CR1_AWDEN		*  0 |	// Analog watchdog enable on regular channels
	ADC_CR1_RES_0		* 00 |	// Resolution (0..3) - 12-bit
	ADC_CR1_OVRIE		*  0 ;	// Оverrun interrupt enable
  ADC2->CR2 =
	ADC_CR2_ADON		*  1 |	// A/D Converter ON / OFF
	ADC_CR2_CONT		*  0 |	// Continuous Conversion
	ADC_CR2_DMA		*  0 |	// Direct Memory access mode
	ADC_CR2_DDS		*  0 |	// DMA disable selection (Single ADC)
	ADC_CR2_EOCS		*  0 |	// End of conversion selection
	ADC_CR2_ALIGN		*  0 |	// Data Alignment = Right
	ADC_CR2_JEXTSEL_0	* 15 |	// External event select for injected group (0..15) - EXTI_15 PG15
	ADC_CR2_JEXTEN_0	* 02 |	// External Trigger Conversion mode for injected channels (0..3) - Fall
	ADC_CR2_JSWSTART	*  0 |	// Start Conversion of injected channels
	ADC_CR2_EXTSEL_0	* 00 |	// External Event Select for regular group (0..15)
	ADC_CR2_EXTEN_0		* 00 |	// External Trigger Conversion mode for regular channels (0..3)
	ADC_CR2_SWSTART		*  0 ;	// Start Conversion of regular channels
  ADC2->SMPR1 =
	ADC_SMPR1_SMP15_0	* 02 ;	// Channel 15 Sample time selection (0..7) - (28 Cycles) 
  ADC2->JSQR =
	ADC_JSQR_JSQ4_0		* 15 |	// 4th conversion in injected sequence (0..18) - IN15
	ADC_JSQR_JL_0		* 00 ;	// Injected Sequence length (0..3) - 1 Conversion
  ADC->CCR =
	ADC_CCR_MULTI_0		* 00 |	// MULTI[4:0] (Multi-ADC mode selection)
	ADC_CCR_DELAY_0		* 00 |	// DELAY[3:0] bits (Delay between 2 sampling phases)
	ADC_CCR_DDS		*  0 |	// DMA disable selection (Multi-ADC mode)
	ADC_CCR_DMA_0		* 00 |	// DMA[1:0] bits (Direct Memory Access mode for multimode)
	ADC_CCR_ADCPRE_0	* 00 |	// ADCPRE[1:0] bits (ADC prescaler) - PCLK2/2 = 30 MHz
	ADC_CCR_VBATE		*  1 |	// VBAT Enable
	ADC_CCR_TSVREFE		*  1 ;	// Temperature Sensor and VREFINT Enable

//* CAN *//

//* CRC *//

//* CRYP *//

//* DAC *//

//* DBG *//

///  DBGMCU->CR = 0x00000027;	// Trace Async Mode, Clocks on

//* DCMI *//

//* DMA *//
/* PL --> IntRAM
   ExtRAM --> LCD */
//  DMA2_Channel1->CPAR = EXTDPY + LCDWAV*320;	// Начальный адрес буфера экрана
//  DMA2_Channel1->CMAR = LCDMEM + LCDWAV*320;	// Начальный адрес памяти ЖКИ
//  DMA2_Channel1->CNDTR = 320*200/2;		// 16-битовые пересылки
  // DMA2_Channel1->CCR = 0x000065c0;		// P->M, Disable

//* ETH *//

//* EXTI *//
/* External Interrupt/event configuration */
  EXTI->IMR = EXTI_IMR_MR7;	// Interrupt Mask on line 7
  EXTI->EMR = EXTI_EMR_MR15;	// Event Mask on line 15
  EXTI->FTSR =
	EXTI_FTSR_TR7	|	// Falling trigger event configuration bit of line 7
	EXTI_FTSR_TR15	;	// Falling trigger event configuration bit of line 15

//* FLASH *//

//* FSMC *//
/* При 16-битовом доступе на шину A[24:0] выводится адрес байтов HADDR[25:1] */
/* Bank1 NE1 - ROM 4M x 16 - не используется */
  FSMC_Bank1->BTCR[0] = 0x00001059;	// NoBurst,NoExtMode,WrEn,AccessEn,16-bit,NOR,NoMux,En
  FSMC_Bank1->BTCR[1] = 0x000f0400;	// Mode1	0(+1)\_4__/(+1)

/* Bank1 NE2 - RAM 256K x 16 */
  FSMC_Bank1->BTCR[2] =			// 16-bit,NoMux,SRAM,En
	FSMC_BCR2_MBKEN * 1 |		// Memory bank enable bit
	FSMC_BCR2_MUXEN * 0 |		// Address/data multiplexing enable bit
	FSMC_BCR2_MTYP_0 * 0 |		// MTYP[1:0] bits (Memory type) - SRAM
	FSMC_BCR2_MTYP_1 * 0 |		// 
	FSMC_BCR2_MWID_0 * 1 |		// MWID[1:0] bits (Memory data bus width) - 16 bits
	FSMC_BCR2_MWID_1 * 0 |		// 
	FSMC_BCR2_FACCEN * 0 |		// Flash access enable
	//
	FSMC_BCR2_BURSTEN * 0 |		// Burst enable bit
	FSMC_BCR2_WAITPOL * 0 |		// Wait signal polarity bit
	FSMC_BCR2_WRAPMOD * 0 |		// Wrapped burst mode support
	FSMC_BCR2_WAITCFG * 0 |		// Wait timing configuration
	FSMC_BCR2_WREN * 1 |		// Write enable bit
	FSMC_BCR2_WAITEN * 0 |		// Wait enable bit
	FSMC_BCR2_EXTMOD * 0 |		// Extended mode enable
	FSMC_BCR2_ASYNCWAIT * 0 |	// Asynchronous wait
	//
	//
	//
	FSMC_BCR2_CBURSTRW * 0;		// Write burst enable
  // FSMC_Bank1->BTCR[3] = 0x00000300;	// Mode1	0(+1)\_3_/(+1)
  FSMC_Bank1->BTCR[3] = 0x00000502;	// Mode1	  2\__5__/(+1)  

/* Bank1 NE3 - PL 
   Мультиплексированная шина, Mode Muxed NOR Flash, 16-bit */
//  FSMC_Bank1->BTCR[4] = 0x0000805b;	// Wait,FAccEn,16-bit,MuxEn,NORFlash,En
//  FSMC_Bank1->BTCR[4] = 0x0000005b;	// noWait,FAccEn,16-bit,MuxEn,NORFlash,En
  FSMC_Bank1->BTCR[4] = 
	FSMC_BCR3_MBKEN * 1 |		// Memory bank enable bit
	FSMC_BCR3_MUXEN * 1 |		// Address/data multiplexing enable bit
	FSMC_BCR3_MTYP_0 * 0 |		// MTYP[1:0] bits (Memory type) - NOR Flash
	FSMC_BCR3_MTYP_1 * 1 |		// 
	FSMC_BCR3_MWID_0 * 1 |		// MWID[1:0] bits (Memory data bus width) - 16 bits
	FSMC_BCR3_MWID_1 * 0 |		// 
	FSMC_BCR3_FACCEN * 1 |		// Flash access enable
	//
	FSMC_BCR3_BURSTEN * 0 |		// Burst enable bit
	FSMC_BCR3_WAITPOL * 0 |		// Wait signal polarity bit
	FSMC_BCR3_WRAPMOD * 0 |		// Wrapped burst mode support
	FSMC_BCR3_WAITCFG * 0 |		// Wait timing configuration
	FSMC_BCR3_WREN * 1 |		// Write enable bit
	FSMC_BCR3_WAITEN * 0 |		// Wait enable bit
	FSMC_BCR3_EXTMOD * 0 |		// Extended mode enable
	FSMC_BCR3_ASYNCWAIT * 0 |	// Asynchronous wait
	//
	//
	//
	FSMC_BCR3_CBURSTRW * 0;		// Write burst enable
//  FSMC_Bank1->BTCR[5] = 0x00020222;	// ModeMuxNOR	2(+1)|2\_3_/(+1)wr, 2(+1)|2(+1)\_2(+3)_/rd 2(+1)
  FSMC_Bank1->BTCR[5] = 0x00020424;	// ModeMuxNOR	4|2\_4_/ rd, 4\_2|4_/1 wr, 2 bus turn 

  
/* Bank1 NE4 - LcdCtrl 128K + 128K(80K)
   Используется 16-битовая шина
   Читать-писать 16-битовыми или 8-битовыми словами */
  FSMC_Bank1->BTCR[6] =
	FSMC_BCR4_MBKEN * 1 |		// Memory bank enable bit
	FSMC_BCR4_MUXEN * 0 |		// Address/data multiplexing enable bit
	FSMC_BCR4_MTYP_0 * 0 |		// MTYP[1:0] bits (Memory type) - SRAM
	FSMC_BCR4_MTYP_1 * 0 |		// 
	FSMC_BCR4_MWID_0 * 1 |		// MWID[1:0] bits (Memory data bus width) - 16 bits
	FSMC_BCR4_MWID_1 * 0 |		// 
	FSMC_BCR4_FACCEN * 0 |		// Flash access enable
	//
	FSMC_BCR4_BURSTEN * 0 |		// Burst enable bit
	FSMC_BCR4_WAITPOL * 0 |		// Wait signal polarity bit
	FSMC_BCR4_WRAPMOD * 0 |		// Wrapped burst mode support
	FSMC_BCR4_WAITCFG * 0 |		// Wait timing configuration
	FSMC_BCR4_WREN * 1 |		// Write enable bit
	FSMC_BCR4_WAITEN * 0 |		// Wait enable bit
	FSMC_BCR4_EXTMOD * 0 |		// Extended mode enable
	FSMC_BCR4_ASYNCWAIT * 1 |	// Asynchronous wait
	//
	//
	//
	FSMC_BCR4_CBURSTRW * 0;		//* Write burst enable
  // FSMC_Bank1->BTCR[7] = 0x000f1200;	// Mode1	0(+1)\_18_____ _/(+1)
// уменьшено до предельного значения для MCLK = BCLK/2 (11T)
  //  FSMC_Bank1->BTCR[7] = 0x00000b00;	// Mode1	0(+1)\_11_____ _/(+1)
// экспериментальное значение (8T), дает то же, что и (11T), из-за WAITn
  // FSMC_Bank1->BTCR[7] = 0x00000800;	// Mode1	0(+1)\_08_____ _/(+1)
  FSMC_Bank1->BTCR[7] = 0x00000802;	// Mode1	  2\_XX_________/(+1)

//* GPIO *//
/* После сброса в ODR 0x00000000 */

/* Установить выходы в неактивное состояние (Hi) (BSRR: 0xRRRRSSSS)
   BSRRL - для установки бита порта в Hi
   BSRRH - для установки бита порта в Lo */
  GPIOA->BSRRL = 0x0010;		// SMSS_N
  GPIOA->BSRRH = 0x1000;		// ANSS_N
  GPIOB->BSRRL = 0x1003;		// CONF_N, LED_N, SND
  GPIOB->BSRRH = 0x0000;
  GPIOC->BSRRL = 0x0001;		// RSTO_N
  GPIOC->BSRRH = 0x0000;
  GPIOD->BSRRL = 0x0004;		// ANSYN
  GPIOD->BSRRH = 0x0000;
  GPIOE->BSRRL = 0x0004;		// TEST
  GPIOE->BSRRH = 0x0000;
  GPIOG->BSRRL = 0x0800;		// WRPR_N
  GPIOG->BSRRH = 0x0000;
  
/* Инициализировать GPIOA */   
  GPIO_INIT(A,
	MD_AF, OT_PP, SP_02, PL_PU, AF_USART2,	// PA0 - USART2_CTS in
	MD_AF, OT_PP, SP_02, PL_PU, AF_USART2,	// PA1 - USART2_RTS out
	MD_AF, OT_PP, SP_02, PL_PU, AF_USART2,	// PA2 - USART2_TX out
	MD_AF, OT_PP, SP_02, PL_PU, AF_USART2,	// PA3 - USART2_RX in
	MD_GO, OT_PP, SP_25, PL_PU, AF_SPI1,	// PA4 - SPI1_NSS SMSS_N M25PE40
	MD_AF, OT_PP, SP_25, PL_PU, AF_SPI1,	// PA5 - SPI1_SCK SMSCK
	MD_AF, OT_PP, SP_25, PL_PU, AF_SPI1,	// PA6 - SPI1_MISO SMMISO
	MD_AF, OT_PP, SP_25, PL_PU, AF_SPI1,	// PA7 - SPI1_MOSI SMMOSI
	MD_AF, OT_PP, SP_80, PL_NP, AF_SYSTEM,	// PA8 - MCO1 LCD CLK
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PA9 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PA10 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PU, AF_SYSTEM,	// PA11 - in pull-up
	MD_IN, OT_OD, SP_02, PL_PU, AF_SYSTEM,	// PA12 - in pull-up
	MD_AF, OT_PP, SP_25, PL_NP, AF_SYSTEM,	// PA13 - JTMS-SWDAT
	MD_AF, OT_PP, SP_25, PL_NP, AF_SYSTEM,	// PA14 - JTCK-SWCLK
	MD_GO, OT_PP, SP_25, PL_PU, AF_SYSTEM);	// PA15 - (SPI3_NSS) ANSS_N

/* Инициализировать GPIOB */
  GPIO_INIT(B,
	MD_GO, OT_PP, SP_02, PL_NP, AF_SYSTEM,	// PB0 - SND out
	MD_GO, OT_PP, SP_80, PL_NP, AF_SYSTEM,	// PB1 - Red LED out
	MD_IN, OT_OD, SP_02, PL_NP, AF_SYSTEM,	// PB2 - BOOT1 (Gnd)
	MD_AF, OT_PP, SP_25, PL_NP, AF_SYSTEM,	// PB3 - JTDO
	MD_AF, OT_PP, SP_25, PL_PU, AF_SYSTEM,	// PB4 - JNTRST
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PB5 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PB6 - in pull-down
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PB7 - ADV_N
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PB8 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PB9 - in pull-down
	MD_AF, OT_PP, SP_25, PL_PU, AF_USART3,	// PB10 - USART3_TX PNTR ->
	MD_AF, OT_PP, SP_25, PL_PU, AF_USART3,	// PB11 - USART3_RC PNRC <-
	MD_GO, OT_PP, SP_25, PL_PU, AF_SYSTEM,	// PB12 - CONF_N
	MD_AF, OT_PP, SP_50, PL_PU, AF_SPI2,	// PB13 - SPI2_SCK PLCLK
	MD_AF, OT_PP, SP_80, PL_NP, AF_OTGHS_,	// PB14 - USB DM
	MD_AF, OT_PP, SP_80, PL_NP, AF_OTGHS_);	// PB15 - USB DP
	
/* Инициализировать GPIOC 
   PC14, PC15 - включаются, как OSC32_IN, _OUT, когда включен LSE */
  GPIO_INIT(C,
	MD_GO, OT_PP, SP_25, PL_PU, AF_SYSTEM,	// PC0 - RSTO_N
	MD_IN, OT_OD, SP_02, PL_PU, AF_SYSTEM,	// PC1 - CONFDN in
//	MD_IN, OT_PP, SP_02, PL_PU, AF_SPI2,	// PC2 - (SPI2_MISO) STAT_N
	MD_IN, OT_PP, SP_02, PL_PU, AF_SYSTEM,	// PC2 - (SPI2_MISO) STAT_N
	MD_AF, OT_PP, SP_50, PL_PU, AF_SPI2,	// PC3 - SPI2_MOSI PLDAT
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PC4 - in pull-down
	MD_AN, OT_OD, SP_02, PL_NP, AF_SYSTEM,	// PC5 - ADC12_IN15 ITAN
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PC6 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PC7 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PC8 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PC9 - in pull-down
	MD_AF, OT_PP, SP_25, PL_PU, AF_SPI3,	// PC10 - SPI3_SCK ANSCK
	MD_AF, OT_PP, SP_25, PL_PU, AF_SPI3,	// PC11 - SPI3_MISO ANOUT <-
	MD_AF, OT_PP, SP_25, PL_PU, AF_SPI3,	// PC12 - SPI3_MOSI ANDAT ->
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PC13 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PC14 - OSC32_IN
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM);	// PC15 - OSC32_OUT

/* Инициализировать GPIOD */
  GPIO_INIT(D,
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PD0 - DB2
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PD1 - DB3
	MD_GO, OT_PP, SP_25, PL_PU, AF_SYSTEM,	// PD2 - ANSYN
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PD3 - in pull-down
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PD4 - OE_N
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PD5 - WE_N
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PD6 - WAIT_N
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PD7 - in pull-down
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PD8 - DB13
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PD9 - DB14
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PD10 - DB15
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PD11 - AB16
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PD12 - AB17
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PD13 - in pull-down
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PD14 - DB0
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC);	// PD15 - DB1

/* Инициализировать GPIOE */
  GPIO_INIT(E,
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PE0 - BL0_N
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PE1 - BL1_N
	MD_GO, OT_PP, SP_25, PL_NP, AF_SYSTEM,	// PE2 - Test out
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PE3 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PE4 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PE5 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PE6 - in pull-down
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PE7 - DB4
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PE8 - DB5
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PE9 - DB6
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PE10 - DB7
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PE11 - DB8
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PE12 - DB9
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PE13 - DB10
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PE14 - DB11
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC);	// PE15 - DB12

/* Инициализировать GPIOF */
  GPIO_INIT(F,
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PF0 - AB0
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PF1 - AB1
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PF2 - AB2
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PF3 - AB3
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PF4 - AB4
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PF5 - AB5
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PF6 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PF7 - in pull-down
	MD_AN, OT_OD, SP_02, PL_NP, AF_SYSTEM,	// PF8 - ADC3_IN6 VREG
	MD_AN, OT_OD, SP_02, PL_NP, AF_SYSTEM,	// PF9 - ADC3_IN7 VBAT
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PF10 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PF11 - in pull-down
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PF12 - AB6
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PF13 - AB7
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PF14 - AB8
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC);	// PF15 - AB9

/* Инициализировать GPIOG */
  GPIO_INIT(G,
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PG0 - AB10
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PG1 - AB11
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PG2 - AB12
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PG3 - AB13
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PG4 - AB14
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PG5 - AB15
	MD_IN, OT_OD, SP_02, PL_PU, AF_SYSTEM,	// PG6 - (FSMC INT2) PNIRQ_N
	MD_IN, OT_OD, SP_02, PL_PU, AF_SYSTEM,	// PG7 - (FSMC INT3) PLIRQ_N
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PG8 - in pull-down
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PG9 - NE2 RAMS_N
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PG10 - NE3 PLS_N
	MD_GO, OT_PP, SP_02, PL_PU, AF_SYSTEM,	// PG11 - WRPR_N
	MD_AF, OT_PP, SP_80, PL_PU, AF_FSMC,	// PG12 - NE4 LCDS_N
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PG13 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PD, AF_SYSTEM,	// PG14 - in pull-down
	MD_IN, OT_OD, SP_02, PL_PU, AF_SYSTEM);	// PG15 - ITST_N (Start?)

/* Зафиксировать конфигурацию всех портов */
  GPIO_LOCK(A, 0xFFFF);
  GPIO_LOCK(B, 0xFFFF);
  GPIO_LOCK(C, 0xFFFF);
  GPIO_LOCK(D, 0xFFFF);
  GPIO_LOCK(E, 0xFFFF);
  GPIO_LOCK(F, 0xFFFF);
  GPIO_LOCK(G, 0xFFFF);
  
//* SYSCFG *//
/* Источник внешнего прерывания/события EXTI7 от ПЛИС - PG7 */
  SYSCFG->EXTICR[1] = SYSCFG_EXTICR2_EXTI7_PG;
/* Источник внешнего прерывания/события EXTI15 для АЦП - PG15 */
  SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI15_PG;

//* HASH *//

//* I2C *//

//* IWDG //*
/* Independent Watchdog Configuration */
#if false
  IWDG->KR = 0x5555;		// Key
  // IWDG->PR = 5;			// Prescaler /128 (4 ms)
  IWDG->PR = 3;			// Prescaler /32  (1 ms) 
  IWDG->RLR = 0xFFF;		// 4096 (> 16 s) -> > 4 s
  IWDG->KR = 0xCCCC;		// Start
#endif

//* OTG_FS *//

//* OTG_HS *//

//* PWR *//

  PWR->CR |= PWR_CR_DBP;	// Disable Backup Domain write protection
  PWR->CSR =
	PWR_CSR_EWUP * 0 |		// Enable WKUP pin
	PWR_CSR_BRE * 1;		// Backup regulator enable
	
/* Теперь можно включить НЧ генератор */
  RCC->BDCR =
	RCC_BDCR_LSEON * 1 |
	RCC_BDCR_LSERDY * 0 |
	RCC_BDCR_LSEBYP * 0 |
	RCC_BDCR_RTCSEL_0 * 1 |		// LSE oscillator clock used as the RTC clock
	RCC_BDCR_RTCEN * 1 |		// RTC enabled
	RCC_BDCR_BDRST * 0;

//* RNG *//

//* RTC *//
/* Real Time Clock Configuration */

//* SDIO *//

//* SPI I2S *//
/* SPI1 -- связь с SFM M25PE40
   8-bit, MSB first, SPEn, Fpclk2 / 4 (15 MHz), Master, CPOL=0, CPHA=0
   Раньше разрешал SPI позже! - так и нужно делать */
  SPI1->CR1 = 
	SPI_CR1_CPHA * 0 |		// Clock Phase
	SPI_CR1_CPOL * 0 |		// Clock Polarity
	SPI_CR1_MSTR * 1 |		// Master Selection
	SPI_CR1_BR_0 * 1 |		// Baud Rate Control - fpclk2 / 4 = 15 MHz
	SPI_CR1_BR_1 * 0 |		// 
	SPI_CR1_BR_2 * 0 |		// 
	SPI_CR1_SPE * 1 |		// SPI Enable
	SPI_CR1_LSBFIRST * 0 |		// Frame Format
	SPI_CR1_SSI * 1 |		// Internal slave select
	SPI_CR1_SSM * 1 |		// Software slave management
	SPI_CR1_RXONLY * 0 |		// Receive only
	SPI_CR1_DFF * 0 |		// Data Frame Format
	SPI_CR1_CRCNEXT * 0 |		// Transmit CRC next
	SPI_CR1_CRCEN * 0 |		// Hardware CRC calculation enable
	SPI_CR1_BIDIOE * 0 |		// Output enable in bidirectional mode
	SPI_CR1_BIDIMODE * 0;		// Bidirectional data mode enable
  SPI1->CR2 = 
	SPI_CR2_RXDMAEN * 0 |		// Rx Buffer DMA Enable
	SPI_CR2_TXDMAEN * 0 |		// Tx Buffer DMA Enable
	SPI_CR2_SSOE * 0 |		// SS Output Enable
	SPI_CR2_FRF * 0 |	// Protocol format - 0: SPI Motorola mode, 1: SPI TI mode
	SPI_CR2_ERRIE * 0 |		// Error Interrupt Enable
	SPI_CR2_RXNEIE * 0 |		// RX buffer Not Empty Interrupt Enable
	SPI_CR2_TXEIE * 0;		// Tx buffer Empty Interrupt Enable
  // SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SPE;	// Разрешить, Мастер
  
/* SPI2 -- конфигурирование EP3C5
   CONF_N (NSS) переключается программно */
  SPI2->CR1 = 
	SPI_CR1_CPHA * 0 |		// Clock Phase
	SPI_CR1_CPOL * 0 |		// Clock Polarity
	SPI_CR1_MSTR * 1 |		// Master Selection
	SPI_CR1_BR_0 * 0 |		// Baud Rate Control - fpclk1 / 2 = 15 MHz
	SPI_CR1_BR_1 * 0 |		// 
	SPI_CR1_BR_2 * 0 |		// 
	SPI_CR1_SPE * 1 |		// SPI Enable (раньше включалось позже!)
	SPI_CR1_LSBFIRST * 1 |		// Frame Format
	SPI_CR1_SSI * 1 |		// Internal slave select (раньше задавалось!)
	SPI_CR1_SSM * 1 |		// Software slave management
	SPI_CR1_RXONLY * 0 |		// Receive only
	SPI_CR1_DFF * 0 |		// Data Frame Format - 8 bit
	SPI_CR1_CRCNEXT * 0 |		// Transmit CRC next
	SPI_CR1_CRCEN * 0 |		// Hardware CRC calculation enable
	SPI_CR1_BIDIOE * 0 |		// Output enable in bidirectional mode
	SPI_CR1_BIDIMODE * 0;		// Bidirectional data mode enable
  SPI2->CR2 = 
	SPI_CR2_RXDMAEN * 0 |		// Rx Buffer DMA Enable
	SPI_CR2_TXDMAEN * 0 |		// Tx Buffer DMA Enable
	SPI_CR2_SSOE * 0 |		// SS Output Enable (все равно используется GPIO?)
	SPI_CR2_FRF * 0 |	// Protocol format - 0: SPI Motorola mode, 1: SPI TI mode
	SPI_CR2_ERRIE * 0 |		// Error Interrupt Enable
	SPI_CR2_RXNEIE * 0 |		// RX buffer Not Empty Interrupt Enable
	SPI_CR2_TXEIE * 0;		// Tx buffer Empty Interrupt Enable
  // SPI2->CR1 |= SPI_CR1_SPE;

/* SPI3 -- регистр управления аналоговыми узлами
   8-bit, MSB first, SPEn, Fpclk1 / 2 (15MHz), Master, CPOL=0, CPHA=0
   74HC595 SCK - Pos, AD5314 SCK - Neg (изменить CPOL) */
  SPI3->CR1 = 
	SPI_CR1_CPHA * 0 |		// Clock Phase
	SPI_CR1_CPOL * 0 |		// Clock Polarity HC595
	SPI_CR1_MSTR * 1 |		// Master Selection
	SPI_CR1_BR_0 * 0 |		// Baud Rate Control - fpclk1 / 2 = 15 MHz
	SPI_CR1_BR_1 * 0 |		// 
	SPI_CR1_BR_2 * 0 |		// 
	SPI_CR1_SPE * 1 |		// SPI Enable
	SPI_CR1_LSBFIRST * 0 |		// Frame Format
	SPI_CR1_SSI * 1 |		// Internal slave select
	SPI_CR1_SSM * 1 |		// Software slave management
	SPI_CR1_RXONLY * 0 |		// Receive only
	SPI_CR1_DFF * 0 |		// Data Frame Format (8 bit)
	SPI_CR1_CRCNEXT * 0 |		// Transmit CRC next
	SPI_CR1_CRCEN * 0 |		// Hardware CRC calculation enable
	SPI_CR1_BIDIOE * 0 |		// Output enable in bidirectional mode
	SPI_CR1_BIDIMODE * 0;		// Bidirectional data mode enable
  SPI3->CR2 = 
	SPI_CR2_RXDMAEN * 0 |		// Rx Buffer DMA Enable
	SPI_CR2_TXDMAEN * 0 |		// Tx Buffer DMA Enable
	SPI_CR2_SSOE * 0 |		// SS Output Enable
	SPI_CR2_FRF * 0 |	// Protocol format - 0: SPI Motorola mode, 1: SPI TI mode
	SPI_CR2_ERRIE * 0 |		// Error Interrupt Enable
	SPI_CR2_RXNEIE * 0 |		// RX buffer Not Empty Interrupt Enable
	SPI_CR2_TXEIE * 0;		// Tx buffer Empty Interrupt Enable

//* TIM *//
/* Timer 6 считает задержку интервалами по 1 µs
   Используется удвоенная частота APB1 */
  TIM6->PSC = (60000000 / 1000000 - 1);	// Prescaler 1 MHz, 1 us
  TIM6->CR1 = 
	TIM_CR1_CEN * 1 |		// Counter enable
	TIM_CR1_UDIS * 0 |		// Update disable
	TIM_CR1_URS * 1 |		// Update request source - only on Overflow
	TIM_CR1_OPM * 1 |		// One pulse mode
	//
	//
	//
	TIM_CR1_ARPE * 0;		// Auto-reload preload enable
  
/* Timer 7 считает однократную задержку интервалами по 1, 10, 100, 1000 µs
   Используется удвоенная частота APB1
   Prescaler задается в макрофункции задержки */
  TIM7->CR1 = 
	TIM_CR1_CEN * 0 |		// Counter enable
	TIM_CR1_UDIS * 0 |		// Update disable
	TIM_CR1_URS * 1 |		// Update request source - only on Overflow
	TIM_CR1_OPM * 1 |		// One pulse mode
	//
	//
	//
	TIM_CR1_ARPE * 1;		// Auto-reload preload enable 

//* USART *//
/* USART2 - связь с внешним миром - 
   115200 bps, 8 битов, без четности, 2 стопа 
   проверять в SR бит RXNE */
  USART2->BRR = (30000000 + 115200 / 2) / 115200;	// 115200
//  USART2->BRR = (30000000 + 57600 / 2) / 57600;		// 57600
  USART2->CR1 = 
	USART_CR1_SBK * 0 |	// Send Break
	USART_CR1_RWU * 0 |	// Receiver wakeup
	USART_CR1_RE * 1 |	// Receiver Enable
	USART_CR1_TE * 1 |	// Transmitter Enable
	USART_CR1_IDLEIE * 0 |	// IDLE Interrupt Enable
	USART_CR1_RXNEIE * 0 |	// RXNE Interrupt Enable
	USART_CR1_TCIE * 0 |	// Transmission Complete Interrupt Enable
	USART_CR1_TXEIE * 0 |	// PE Interrupt Enable
	USART_CR1_PEIE * 0 |	// PE Interrupt Enable
	USART_CR1_PS * 0 |	// Parity Selection - Even
	USART_CR1_PCE * 0 |	// Parity Control Enable
	USART_CR1_WAKE * 0 |	// Wakeup method
	USART_CR1_M * 0 |	// Word length - 1 Start bit, 8 Data bits, n Stop bit
	USART_CR1_UE * 1 |	// USART Enable
	//
	USART_CR1_OVER8 * 0;	// USART Oversampling by 8 enable
  USART2->CR2 =
	USART_CR2_STOP_0 * 0 |	// STOP[1:0] bits (STOP bits) - 2 Stop bits
	USART_CR2_STOP_1 * 1;	//
  USART2->CR3 = 
	USART_CR3_EIE * 0 |	// Error Interrupt Enable
	USART_CR3_IREN * 0 |	// IrDA mode Enable
	USART_CR3_IRLP * 0 |	// IrDA Low-Power
	USART_CR3_HDSEL * 0 |	// Half-Duplex Selection
	USART_CR3_NACK * 0 |	// Smartcard NACK enable
	USART_CR3_SCEN * 0 |	// Smartcard mode enable
	USART_CR3_DMAR * 0 |	// DMA Enable Receiver
	USART_CR3_DMAT * 0 |	// DMA Enable Transmitter
	USART_CR3_RTSE * 0 |	// RTS Enable
	USART_CR3_CTSE * 0 |	// CTS Enable
	USART_CR3_CTSIE * 0 |	// CTS Interrupt Enable
	USART_CR3_ONEBIT * 0;	// USART One bit method enable

/* USART3 - связь с панелью управления -
   250000 bps (4 us/bit), 8 битов, без четности, 2 стопа */
  USART3->BRR = (30000000 + 250000 / 2) / 250000;	// 250000 bps
  USART3->CR1 = 
	USART_CR1_SBK * 0 |	// Send Break
	USART_CR1_RWU * 0 |	// Receiver wakeup
	USART_CR1_RE * 1 |	// Receiver Enable
	USART_CR1_TE * 1 |	// Transmitter Enable
	USART_CR1_IDLEIE * 0 |	// IDLE Interrupt Enable
	USART_CR1_RXNEIE * 1 |	// RXNE Interrupt Enable
	USART_CR1_TCIE * 0 |	// Transmission Complete Interrupt Enable
	USART_CR1_TXEIE * 0 |	// PE Interrupt Enable
	USART_CR1_PEIE * 0 |	// PE Interrupt Enable
	USART_CR1_PS * 0 |	// Parity Selection
	USART_CR1_PCE * 0 |	// Parity Control Enable
	USART_CR1_WAKE * 0 |	// Wakeup method
	USART_CR1_M * 0 |	// Word length - 1 Start bit, 8 Data bits, n Stop bit
	USART_CR1_UE * 1 |	// USART Enable
	//
	USART_CR1_OVER8 * 0;	// USART Oversampling by 8 enable
  USART3->CR2 =
	USART_CR2_STOP_0 * 0 |	// STOP[1:0] bits (STOP bits) - 2 Stop bits
	USART_CR2_STOP_1 * 1;	//
  USART3->CR3 = 
	USART_CR3_EIE * 0 |	// Error Interrupt Enable
	USART_CR3_IREN * 0 |	// IrDA mode Enable
	USART_CR3_IRLP * 0 |	// IrDA Low-Power
	USART_CR3_HDSEL * 0 |	// Half-Duplex Selection
	USART_CR3_NACK * 0 |	// Smartcard NACK enable
	USART_CR3_SCEN * 0 |	// Smartcard mode enable
	USART_CR3_DMAR * 0 |	// DMA Enable Receiver
	USART_CR3_DMAT * 0 |	// DMA Enable Transmitter
	USART_CR3_RTSE * 0 |	// RTS Enable
	USART_CR3_CTSE * 0 |	// CTS Enable
	USART_CR3_CTSIE * 0 |	// CTS Interrupt Enable
	USART_CR3_ONEBIT * 0;	// USART One bit method enable

//* WWDG *//
  
//* NVIC *//
  NVIC_ClearPendingIRQ(USART3_IRQn);	// сбросить прерывания от панели 
  NVIC_EnableIRQ(USART3_IRQn);  	// разрешить прерывания от панели

  NVIC_ClearPendingIRQ(USART2_IRQn);	// сбросить прерывания от интерфейса RS-232 
  NVIC_EnableIRQ(USART2_IRQn);  	// разрешить прерывания от интерфейса RS-232

  NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
  NVIC_EnableIRQ(EXTI9_5_IRQn);
#ifdef USE_USB_INTERFACE
  NVIC_ClearPendingIRQ(OTG_HS_IRQn);	// сбросить прерывания от интерфейса USB
  NVIC_EnableIRQ(OTG_HS_IRQn);		// разрешить прерывания от интерфейса USB
  // NVIC_EnableIRQ(OTG_HS_EP1_IN_IRQn);
  // NVIC_EnableIRQ(OTG_HS_EP1_OUT_IRQn);
#endif	// USE_USB_INTERFACE

//* SYSCFG *//

//* SYSTICK *// 
/* System Timer Configuration инициализируется в RTOS */
#if false
  SysTick_Config(9000);					// 1 ms период 
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	// HCLK/8
#endif

}

/*!*****************************************************************************
 @brief		MCU test
 @details	Общие тесты процессора и периферии
 @param		none
 @return	
 @attention	Возврата может не быть!
 @note		
 */
void MCU_test(void) {
/* Проверить работу светодиода */
#ifdef LED_TEST
  LED_ON();   
  WDELAY_START(100000);			// 0.1 s
  WDELAY_WAIT();
  LED_OFF();
#endif
/* Проверить функцию индикации */
#ifdef BLINK_TEST
  Led_blink(50000);
#endif
/* Если неисправен тактовый генератор, светить постоянно */
  if (ErrorState == ERROR_XTAL) LED_ON();
/* Выдать сброс на внешние устройства */
  RSTO_ON();
  WDELAY_START(1000);			// 1 ms
  WDELAY_WAIT();
  RSTO_OFF();
}


/*!*****************************************************************************
 @brief		LED Blink
 @details	Мигание светодиодом для сигнализации ошибки бесконечно
 @param		delay - интервал мигания (on in us / off in us)
 @return	
 @attention	Возврата нет!
 @note		
 */
void Led_blink(uint32_t delay) {
  while (1) {
    LED_ON();
    WDELAY_START(delay); WDELAY_WAIT();
    LED_OFF();
    WDELAY_START(delay); WDELAY_WAIT();
  }
}

/*!*****************************************************************************
 @brief		Error LED Blink
 @details	Мигание светодиодом для сигнализации ошибки
 @return	
 @note		Выдает 1 раз
 */
void Error_blink(void) {
  register uint32_t temp = ErrorState;
  DELAY_START(10); DELAY_WAIT();	// Защитный интервал для синхронизации  
  LED_ON();
  DELAY_START(5); DELAY_WAIT();		// Start
  for (uint32_t i = 8; i--; ) {
    if (temp & 0x80) LED_OFF();		// Hi - Error
    else LED_ON();
    DELAY_START(5);
    temp <<= 1;				// Сдвинуть ошибку    
    DELAY_WAIT();
  }
  LED_OFF();				// Hi
}


/*!*****************************************************************************
 @brief		Test Data send
 @details	Тестовая функция - послать данные по RS-232
 @param		TestData - данные для передачи
 @return	
 */
void Test_send(uint8_t TestData) {
  USART2->DR = TestData;
} 

/*!*****************************************************************************
 @brief		Delay time start in 1 us intervals
 @details	Задать задержку
 @param		time - время в 1 us интервалах (+1)
 @return	
 @note		Максимальная задержка 65536 us
 */
void Timer_start(uint16_t time) {
  TIM6->SR = 0;				// Status reset
  TIM6->ARR = time;			// Autoreload (one pulse)
  TIM6->CR1 |= 0x0001;			// Enable
}

/*!*****************************************************************************
 @brief		Delay check
 @details	Проверить окончание задержки
 @param		none
 @return	
 @note		
 */
bool Timer_check(void) {
  return (TIM6->SR);			// Check Update Interrupt Flag in Status
}

/*!*****************************************************************************
 @brief		Delay wait
 @details	Ожидать конец задержки
 @param		none
 @return	
 */
void Timer_wait(void) {
  while (!TIM6->SR);			// Wait Update Interrupt Flag in Status
}


//__ EXTRAM __//

/*!*****************************************************************************
 @brief		XRAM test
 @details	Тест внешней памяти
 @return	
 @note		По всем адресам памяти записывается 16 битов от адреса
 */
void XRAM_test(void) {
  volatile uint16_t *pMem = (uint16_t *)XRAM_BASE;
  for (uint32_t i = 0; i < XRAM_SIZE / 2; i++) {
    uint16_t j = (uint16_t)i;
    *pMem = j;
    if (*pMem++ == j) {
      LED_ON();
      LED_OFF();
    }
    else {
      ErrorState = ERROR_XRAM;
      break;
    }
  }
}

//__ SFM - Функции работы с последовательной памятью __//

/*!*****************************************************************************
 @brief		Page Read
 @details	Reads a block (Page) of data from the FLASH
 @param		pBuffer - pointer to the buffer that receives the data read from the FLASH
 @param		ReadAddr - FLASH's internal address to read from
 @param		NumByteToRead - number of bytes to read from the FLASH  
 @return	
 */
void SFMPage_read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead) {
  SMSS_ON();				// Select the FLASH: Chip Select low
  SFMByte_send(SFM_READ);		// Send "Read from Memory" instruction
  SFMByte_send((ReadAddr & 0xFF0000) >> 16);	// high nibble address byte
  SFMByte_send((ReadAddr & 0xFF00) >> 8);	// medium nibble address byte
  SFMByte_send(ReadAddr & 0xFF);		//low nibble address byte
  while (NumByteToRead--)		// while there is data to be read
  {
    *pBuffer = SFMByte_send(SFM_DUMMY);	// Read a byte from the FLASH
    pBuffer++;	// Point to the next location where the byte read will be saved
  }

  SMSS_OFF();				// Deselect the FLASH: Chip Select high
}

/*!*****************************************************************************
 @brief		SFM Identificator read
 @details	Читать идентификационный номер последовательной памяти
 @return	Identificator
 @note		Память должна быть выбрана 
 */
#if 0
// 3.7 µs длительность SS
inline uint32_t SFMID_read(void) {
  SMSS_ON();
  volatile uint32_t ID = 0;
  SFMByte_send(SFM_RDID);			// Send "RDID " instruction
  ID = SFMByte_send(SFM_DUMMY);			// Read a byte from the SFM
  ID = (ID<<8) | (SFMByte_send(SFM_DUMMY));	// Read a byte from the SFM
  ID = (ID<<8) | (SFMByte_send(SFM_DUMMY));	// Read a byte from the SFM
  SMSS_OFF();
  return ID;
}
#else
// 2.45 µs (если SMSS_OFF() задать до чтения SPI->DR, время SS равно 2.38 µs)
uint32_t SFMID_read(void) {
  SMSS_ON();
  uint32_t id = 0;
  SPI1->DR = SFM_RDID;			// послать команду "RDID"
  while (!(SPI1->SR & SPI_SR_TXE));	// ждать освобождение передатчика
  SPI1->DR = SFM_DUMMY;			// послать пустой байт 
  while (!(SPI1->SR & SPI_SR_RXNE));	// ждать байт в приемнике
  id = SPI1->DR;				// High Impedance
  while (!(SPI1->SR & SPI_SR_TXE));	// ждать освобождение передатчика
  SPI1->DR = SFM_DUMMY;			// послать пустой байт
  while (!(SPI1->SR & SPI_SR_RXNE));	// ждать байт в приемнике
  id = SPI1->DR << 16;				// Manufacturer
  while (!(SPI1->SR & SPI_SR_TXE));	// ждать освобождение передатчика
  SPI1->DR = SFM_DUMMY;			// послать пустой байт
  while (!(SPI1->SR & SPI_SR_RXNE));	// ждать байт в приемнике  
  id |= SPI1->DR << 8;				// Memory type
  // while (!(SPI1->SR & SPI_SR_TXE));	// ждать освобождение передатчика  
  while (!(SPI1->SR & SPI_SR_RXNE));	// ждать байт в приемнике
  // while (SPI1->SR & SPI_SR_BSY);	// ждать, если SPI занят
  id |= SPI1->DR;				// Memory capacity
  // while (SPI1->SR & SPI_SR_BSY);	// ждать, если SPI занят
  SMSS_OFF();
  // Test = id;
  return id;
}
#endif

/*!*****************************************************************************
 @brief		SFM Word read
 @details	Читать слово из последовательной памяти
 @param		Addr - адрес
 @return	Word
 @note		low-endian (LL,LH,HL,HH)
 */
uint32_t SFMWord_read(uint32_t Addr) {
  SMSS_ON();				// Select SFM: Chip Select low
  uint32_t word = 0;
  SFMByte_send(SFM_READ);		// Send "Read from Memory" instruction
  SFMByte_send((Addr & 0xff0000)>>16);	// high nibble address byte
  SFMByte_send((Addr & 0xff00)>>8);	// medium nibble address byte
  SFMByte_send(Addr & 0xff);		// low nibble address byte
  word = SFMByte_send(SFM_DUMMY);	// Read LL byte of word from the SFM  
  word |= (SFMByte_send(SFM_DUMMY))<<8;	// Read LH byte of word from the SFM
  word |= (SFMByte_send(SFM_DUMMY))<<16;// Read HL byte of word from the SFM
  word |= (SFMByte_send(SFM_DUMMY))<<24;// Read HH byte of word from the SFM
  SMSS_OFF();				// Deselect the SFM: Chip Select high
  return word;
}

/*!*****************************************************************************
 @brief		Bulk Erase the entire FLASH (512KB)
 @details	Стереть всю память SPI FLASH
 @return	
 */
void SFMBulk_erase(void) {
  SFMWr_enable();			// Write enable
  //
  SMSS_ON();				// Select the FLASH: Chip Select low
  SFMByte_send(SFM_BE);			// Send Bulk Erase instruction
  SMSS_OFF();				// Deselect the FLASH: Chip Select high
  //
  SFMWrEnd_wait();			// Wait the end of Flash writing
}

/*!*****************************************************************************
 @brief		Erase the specified FLASH sector (64KB)
 @details	Стереть выбранный сектор SPI FLASH
 @param		Addr - address of the sector to erase
 @return	
 */
void SFMSect_erase(uint32_t Addr) {
  SFMWr_enable();			// Write enable
  //
  SMSS_ON();  				// Select the FLASH: Chip Select low
  SFMByte_send(SFM_SE);			// Send Sector Erase instruction
  SFMByte_send((Addr & 0xff0000)>>16);	// high nibble address byte
  SFMByte_send((Addr & 0xff00)>>8);	// medium nibble address byte
  SFMByte_send(Addr & 0xff);		// low nibble address byte
  SMSS_OFF();				// Deselect the FLASH: Chip Select high
  //
  SFMWrEnd_wait();			// Wait the end of Flash writing
}

/*!*****************************************************************************
 @brief		Erase the specified FLASH subsector (4KB)
 @details	Стереть выбранный субсектор SPI FLASH
 @param		Addr - address of the subsector to erase
 @return	
 */
void SFMSubSect_erase(uint32_t Addr) {
  SFMWr_enable();			// Write enable
  //
  SMSS_ON(); 				// Select the FLASH: Chip Select low
  SFMByte_send(SFM_SSE);		// Send SubSector Erase instruction
  SFMByte_send((Addr & 0xff0000)>>16);	// high nibble address byte
  SFMByte_send((Addr & 0xff00)>>8);	// medium nibble address byte
  SFMByte_send(Addr & 0xff);		// low nibble address byte
  SMSS_OFF();				// Deselect the FLASH: Chip Select high
  //
  SFMWrEnd_wait();			// Wait the end of Flash writing
}

/*!*****************************************************************************
 @brief		Erase the specified FLASH page (256B)
 @details	Стереть выбранную страницу SPI FLASH
 @param		Addr - address of the sector to erase
 @return	
 */
void SFMPage_erase(uint32_t Addr) {
  SFMWr_enable();			// Write enable
  //
  SMSS_ON();				// Select the FLASH: Chip Select low
  SFMByte_send(SFM_PE);			// Send Page Erase instruction
  SFMByte_send((Addr & 0xff0000)>>16);	// high nibble address byte
  SFMByte_send((Addr & 0xff00)>>8);	// medium nibble address byte
  SFMByte_send(Addr & 0xff);		// low nibble address byte
  SMSS_OFF();				// Deselect the FLASH: Chip Select high
  //
  SFMWrEnd_wait();			// Wait the end of Flash writing
}

/*!*****************************************************************************
 @brief		Page Write (11ms)
 @details	Writes more than one byte to the FLASH with a single WRITE cycle
 @note		The number of byte can't exceed the FLASH page size
 @param		pMem - pointer to the buffer containing the data to be written
 @param		Addr - FLASH's internal address to write to
 @param		Numb - number of bytes to write to the FLASH,\n
 		must be equal or less than "SFM_PAGESIZE" value
 @return	
 */
void SFMPage_write(uint8_t *pMem, uint32_t Addr, uint32_t Numb) {
  SFMWr_enable();			// Enable the write access to the FLASH
  //
  SMSS_ON();				// Select the FLASH: Chip Select low
  SFMByte_send(SFM_PW);			// Send "Write to Memory Page" instruction
  SFMByte_send((Addr & 0xff0000)>>16);	// high nibble address byte
  SFMByte_send((Addr & 0xff00)>> 8);	// medium nibble address byte
  SFMByte_send(Addr & 0xff);		// low nibble address byte
  while (Numb--)			// while there is data to be written
    SFMByte_send(*pMem++);		// Send the current byte
  SMSS_OFF();				// Deselect the FLASH: Chip Select high
  //
  SFMWrEnd_wait();			// Wait the end of Flash writing
}

/*!*****************************************************************************
 @brief		Page Program (0.8ms)
 @details	Writes more than one byte to the FLASH with a single PROGR cycle
 @note		The number of byte can't exceed the FLASH page size
 @param		pMem - pointer to the buffer containing the data to be written
 @param		Addr - FLASH's internal address to write to
 @param		Numb - number of bytes to write to the FLASH,\n
 		must be equal or less than "SFM_PAGESIZE" value 
 @return	
 */
void SFMPage_progr(uint8_t *pMem, uint32_t Addr, uint32_t Numb) {
  SFMWr_enable();			// Enable the write access to the FLASH
  //
  SMSS_ON();				// Select the FLASH: Chip Select low
  SFMByte_send(SFM_PP);			// Send "Program to Memory Page" instruction
  SFMByte_send((Addr & 0xff0000)>>16);	// high nibble address byte
  SFMByte_send((Addr & 0xff00)>> 8);	// medium nibble address byte
  SFMByte_send(Addr & 0xff);		// low nibble address byte
  while (Numb--)			// while there is data to be written
    SFMByte_send(*pMem++);		// Send the current byte
  SMSS_OFF();				// Deselect the FLASH: Chip Select high
  //
  SFMWrEnd_wait();			// Wait the end of Flash writing
}

/* Функции среднего уровня для работы с последовательной Flash памятью */ 

/*!*****************************************************************************
 @brief		SFM Write Enable
 @details	
 @return	
 @note	
 */
void SFMWr_enable(void) {
  SMSS_ON();				// Select the FLASH: Chip Select low
  SFMByte_send(SFM_WREN);		// Write enable
  SMSS_OFF();				// Deselect the FLASH: Chip Select high
}

/*!*****************************************************************************
 @brief		Wait For Write End
 @details	Polls the status of the Write In Progress (WIP) flag in the FLASH's
 		status register and loop until write opertaion has completed
 @return	
 */
void SFMWrEnd_wait(void) {
  SMSS_ON();				// Select the FLASH: Chip Select low
  SFMByte_send(SFM_RDSR);		// Send "Read Status Register" instruction
  while ((SFMByte_send(SFM_DUMMY)) & SFM_WIP);	// Write in progress
  SMSS_OFF();  				// Deselect the FLASH: Chip Select high
}

/*!*****************************************************************************
 @brief		Serial Flash Memory read start
 @details	Подготовить чтение байтов
 @param		Addr - адрес
 @return	
 @note		Дальше использовать SFMByte_read, завершить SFMRead_stop
 */
void SFMRead_start(uint32_t Addr) {
  SMSS_ON();
  SFMByte_send(SFM_FSTRD);		// FastRead
  SFMByte_send((Addr & 0xff0000)>>16);	// high nibble address byte
  SFMByte_send((Addr & 0xff00)>> 8);	// medium nibble address byte
  SFMByte_send(Addr & 0xff);		// low nibble address byte  
  SFMByte_send(SFM_DUMMY);		// for fastread
  //
}

/*!*****************************************************************************
 @brief		Serial Flash Memory Byte read
 @details	Читать байт (за байтом)
 @return	байт
 @note	
 */
uint8_t SFMByte_read(void) {
  return SFMByte_send(SFM_DUMMY);
}

/*!*****************************************************************************
 @brief		Serial Flash Memory read stop
 @details	Завершить чтение байтов
 @return	
 @note	
 */
void SFMRead_stop(void) {
  SMSS_OFF();
}

/*!*****************************************************************************
 @brief	
 @details	Послать байт данных
 @param		data - данные
 @return	Принятый байт
 @note		Память должна быть выбрана
 */
inline uint8_t SFMByte_send(uint8_t data) {
  while (!(SPI1->SR & SPI_SR_TXE));	// ждать освобождения
  SPI1->DR = data;			// послать данные
  while (!(SPI1->SR & SPI_SR_RXNE));	// ждать байта в приемнике (конца передачи)
//  while (SPI1->SR & SPI_SR_BSY);	// другой способ дождаться конца передачи
  return SPI1->DR;
}

//__ PL __//

/*!*****************************************************************************
 @brief		EP3C5 configure 
 @details	Конфигурировать ПЛИС EP3C5 кодом из SPI EEPROM 
 @return	0 - инициализация прошла успешно, иначе - ошибка
 @verbatim
 3000000 байтов кода читается из M25PE40 и передается в
 FPGA EP3С5E144I7N - 144EQFP, 2x23 M9K, 5K LE, $19.20($24.19).
 Получается по 20К в канале.
 3000000 bits config, сжатый код меньше на 35-55%.
 Потребуется максимум 11718,75 страниц памяти по 256 байтов.
 Код расположен с начала памяти. Размер кода (4 байта) находится в начале кода
 (заносится в буфер после приема).
 @endverbatim
 */
/* 
 Используется схема Passive Serial Standard (все MSEL подключены к GND).
 * FPGA сбрасывается низким уровнем на любом из выводов CONFIGn или STATUSn.   
   После включения питания Cyclone-III находится в состоянии сброса POR в
   течение 50 - 100 ms. Во время POR FPGA удерживает STATUSn и CONF_DONE в 
   низком состоянии, а USER I/O в высокоимпедансном состоянии. После POR 
   Cyclone III отпускает nSTATUS, который подтянут к питанию внешним 10 kOhm.
   После POR на USER I/O включаются week pull-up резисторы, которые разрешены 
   до и во время конфигурации.
 * Когда CONFIGn переводится из низкого уровня в высокий, сброс заканчивается,
   и STATUSn переходит в высокоимпедансное состояние (через max 800 us!).
   Все USER I/O остаются в высокоимпедансном состоянии.
   Когда STATUSn отпущен, ПЛИС готова принять код конфигурации через min 2 us до
   фронта такта. От фронта CONFIGn до фронта такта должно быть min 800 us!  
   VCCINT и VCCIO выводы должны быть запитаны требуемым напряжением. 
 * Код из памяти пересылается в ПЛИС побайтно, до тех пор пока ПЛИС не отпустит
   CONF_DONE, который за счет внешнего резистора 10 kOhm станет высоким.
   Данные DATA[0] защелкиваются в ПЛИС по фронту тактов DCLK (период min 7.5 ns,
   133 MHz). Первым посылается младший бит байта (LSB). Конфигурирование (такты)
   может быть остановлено на неопределенное время. Никакого сигнала BUSY не 
   выдается (успевает записаться за 10 ns?).
 * Два среза DCLK требуются после того, как CONF_DONE станет высоким, для
   начала инициализации. Такты DCLK после конфигурирования не влияют на ПЛИС.
   Для тактов инициализации можно задать использовать CLKUSR, или использовать 
   внутренний тактовый генератор (~ 10 MHz) по-умолчанию. Требуется 3185 тактов,
   допускается частота до 133 MHz. Такты CLKUSR не влияют на процесс 
   конфигурации.
 * Сигнал INIT_DONE, если используется, говорит об окончании инициализации.
   INIT_DONE становится низким после первого кадра конфигурации, и 
   устанавливается высоким после инициализации за счет подтягивающего резистора
   10 kOhm.
 * После этого ПЛИС входит в USER режим (через max 650 us max). В USER режиме 
   внутренние weak pull-up резисторы на user I/O отключаются.
 * Если во время конфигурации случился сбой, STATUSn становится низким, и FPGA
   сбрасывает себя внутри. Для реконфигурации ПЛИС нужно подать низкий сигнал 
   CONFIGn (min 500 ns).
 */
uint8_t PL_config(void) {
  // #define  CODE_COUNT 3000000		// количество байтов несжатого кода
/* Задать режим конфигурации */  
  // LED_ON();
  CONF_ON();				// принудительный сброс ПЛИС
  DELAY_START(2);			// 2 us
  DELAY_WAIT();
  CONF_OFF();				// начать конфигурацию
  while (!STAT_GET());			// ждать готовности ПЛИС (Max 800 us)
  DELAY_START(800);			// 800 us min
/* Прочитать размер кода конфигурации */
  volatile uint32_t cnt = SFMWord_read(SFMCODE_BASE);	// размер кода в начале SFM
  if (cnt == 0 || cnt == 0xFFFFFFFF) return ERROR_CODE;
  DELAY_WAIT();				// дождаться конца задержки
/* Выдать в последовательную память команду чтения */
  SFMRead_start(SFMCODE_BASE + 4);
/* Читать код из SFM побайтно и записывать в FPGA (2 байта - запас на 
   конфигурирование). В SFM первым идет MSB, в ПЛИС первым идет LSB */
  for (cnt + 2; cnt--; ) {
/* если STATUSn упадет - ошибка, выйти из цикла
   если CONFDN поднимется - конфигурирование завершено */
    /// if (!(PLPSByte_send(SFMByte_read())) || !STAT_GET() || CONFDN_GET()) break;
    PLPSByte_send(SFMByte_read());
    if (!STAT_GET() || CONFDN_GET()) break;
  }
/* Завершить чтение байтов */
  SFMRead_stop();
  // LED_OFF();
/* Конфигурация закончена, или попытка была неудачной */  
  if (!CONFDN_GET()) return ERROR_CONF;
  else return ERROR_NONE;
}

/*!*****************************************************************************
 @brief		PL registers initialize
 @details	
 @return	
 @note	 
 */
void PL_init(void) {
  PL->STAT = PL_START_ITDCST | PL_START_RESET;
  PL->VOLT = PL_VoltBuf;
  PL->ACQU = PL_AcquBuf;
  PL->TRIG = PL_TrigBuf;

#if false
// SDAC Settings
  PL->SDAC = PL_SDAC_DCRES;		// Data & Control reset
  DELAY_START(2); DELAY_WAIT();
  PL->SDAC = PL_SDAC_LDAC;		// Update continuously
  DELAY_START(2); DELAY_WAIT();
  PL->SDAC = PL_SDAC_GAIN | 0x000c;	// Buffered Reference
  DELAY_START(2); DELAY_WAIT();
// SDAC buffers
  PL->SDAC = PL_SDAC_S1POS | (Chan1Ctrl.SigPos<<2);  
  DELAY_START(2); DELAY_WAIT();
  PL->SDAC = PL_SDAC_S2POS | (Chan2Ctrl.SigPos<<2);
  DELAY_START(2); DELAY_WAIT();
  PL->SDAC = PL_SDAC_S1REF | PL_S1RefBuf;
  DELAY_START(2); DELAY_WAIT();
  PL->SDAC = PL_SDAC_S2REF | PL_S2RefBuf;
  DELAY_START(2); DELAY_WAIT();
  PL->SDAC = PL_SDAC_T1LEV | PL_T1LevBuf;
  DELAY_START(2); DELAY_WAIT();
  PL->SDAC = PL_SDAC_T2LEV | PL_T2LevBuf;
  DELAY_START(2); DELAY_WAIT();
  PL->SDAC = PL_SDAC_TXLEV | PL_TXLevBuf;
  DELAY_START(2); DELAY_WAIT();
  PL->SDAC = PL_SDAC_TEST | 0x0fff;	// Test - maximum output, 2.048V
#endif
  PL->PRET = SweepCtrl.Pret << (MemCtrl.SizeNum + 5);
  PL->TIMH = 0;
  PL->TIML = 5000 - 2;			// 0.5 ms / div
  uint16_t temp = PL->STAT;  
  PL->STAT = PL_START_ACQUST;
}

/* Функции ПЛИС низкого уровня */

/*!*****************************************************************************
 @brief		Programmable Logic Device Send Byte
 @details	Sends a byte through the SPI and return received byte
 @param		data - byte to send
 @return	received byte
 */
uint8_t PLPSByte_send(uint8_t data) {
  while (!(PLPS->SR & SPI_SR_TXE));	// ждать освобождение
  PLPS->DR = data;			// послать данные
  while (!(PLPS->SR & SPI_SR_RXNE));	// ждать байта в приемнике (конца передачи)
//  while (PLPS->SR & SPI_SR_BSY);	// другой способ дождаться конца передачи
  return PLPS->DR;
}

//__ Analog Part Control __//

/*!*****************************************************************************
 @brief		Analog Shift Register Buffer send
 @details	Послать данные из буфера в сдвиговый регистр
 @param		нет
 @note		CPOL = 0 
 @return	
 @note		Выдается импульс для загрузки в параллельный регистр
 @note		RXNE сбрасывается при чтении из DR
 */
void AnaReg_send(void) {
  // while (!(ANAS->SR & SPI_SR_TXE));	// ждать освобождение буфера - не нужно
  ANAS->DR = (uint8_t)(AnaRegBuf >>16);	// послать биты 23 - 16
  while (!(ANAS->SR & SPI_SR_TXE));	// ждать освобождение буфера
  ANAS->DR = (uint8_t)(AnaRegBuf >>8);	// послать биты 15 - 8
  while (!(ANAS->SR & SPI_SR_TXE));	// ждать освобождение буфера
  ANAS->DR = (uint8_t)(AnaRegBuf);	// послать биты 7 - 0
  while (!(ANAS->SR & SPI_SR_TXE));	// ждать освобождение буфера
  // while (!(ANAS->SR & SPI_SR_RXNE));	// ждать конец передачи - нельзя!
  while (ANAS->SR & SPI_SR_BSY);	// ждать освобождение приемопередатчика
  ANSS_ON();				// _/
  __nop(); __nop(); __nop();		// 25 ns
  ANSS_OFF();				//      \_
}

/*!*****************************************************************************
 @brief		Send Data to Analog Serial DAC
 @details	Послать данные в последовательный ЦАП
 @param		нет
 @return	
 @note		На время передачи ANSYN должен быть в нуле
 */
void AnaDAC_send(uint16_t data) {
  ANAS->CR1 &= ~SPI_CR1_SPE;		// запретить SPI
  ANAS->CR1 |= SPI_CR1_CPOL;		// данные стабильные на срезе \_
  ANAS->CR1 |= SPI_CR1_SPE;		// разрешить SPI 

  ANSYN_ON();				// \__
  ANAS->DR = (uint8_t)(data >> 8);	// послать биты 15 - 8
  while (!(ANAS->SR & SPI_SR_TXE));	// ждать освобождение буфера
  ANAS->DR = (uint8_t)(data);		// послать биты 7 - 0
  while (!(ANAS->SR & SPI_SR_TXE));	// ждать освобождение буфера
  // while (!(ANAS->SR & SPI_SR_RXNE));	// ждать конец передачи - нельзя!
  while (ANAS->SR & SPI_SR_BSY);	// ждать освобождение приемопередатчика
  ANSYN_OFF();				//			__/

  ANAS->CR1 &= ~SPI_CR1_SPE;		// запретить SPI
  ANAS->CR1 &= ~SPI_CR1_CPOL;		// данные стабильные на фронте _/
  ANAS->CR1 |= SPI_CR1_SPE;		// разрешить SPI
}

/*!*****************************************************************************
 \brief		Balance initialization 
 \details	Восстановить сохраненные данные баланса
 */
void Balance_init(void) {
/* Извлечь сохраненные данные баланса в канале 1 */
  SFMRead_start(SFMBAL1);
  Bal1Ctrl.Ofs20DC = SFMByte_read();
  Bal1Ctrl.Ofs20AC = SFMByte_read();
  Bal1Ctrl.Ofs10DC = SFMByte_read();
  Bal1Ctrl.Ofs10AC = SFMByte_read();
  Bal1Ctrl.Ofs5DC = SFMByte_read();
  Bal1Ctrl.Ofs5AC = SFMByte_read();
  Bal1Ctrl.Ofs5GND = SFMByte_read();
  Bal1Ctrl.Ofs2DC = SFMByte_read();
  Bal1Ctrl.Ofs2AC = SFMByte_read();
  Bal1Ctrl.Ofs2GND = SFMByte_read();
  SFMRead_stop();
/* Извлечь сохраненные данные баланса в канале 2 */
  SFMRead_start(SFMBAL2);
  Bal2Ctrl.Ofs20DC = SFMByte_read();
  Bal2Ctrl.Ofs20AC = SFMByte_read();
  Bal2Ctrl.Ofs10DC = SFMByte_read();
  Bal2Ctrl.Ofs10AC = SFMByte_read();
  Bal2Ctrl.Ofs5DC = SFMByte_read();
  Bal2Ctrl.Ofs5AC = SFMByte_read();
  Bal2Ctrl.Ofs5GND = SFMByte_read();
  Bal2Ctrl.Ofs2DC = SFMByte_read();
  Bal2Ctrl.Ofs2AC = SFMByte_read();
  Bal2Ctrl.Ofs2GND = SFMByte_read();  
  SFMRead_stop(); 
}


/*!*****************************************************************************
 \brief		Power On Control initialization
 \details	Инициализировать все режимы работы при включении
 \note		Управляющие структуры размещены в Backup RAM
 */
void Ctrl_init(void) {

/* Проверить исправность содержимого backup памяти по сигнатуре */
  if (Signature != 0x76543210) { 

/* Содержимое памяти потеряно.
   Установить управляющие структуры по умолчанию */
// Channel 1
    Chan1Ctrl.On = true;
    Chan1Ctrl.Prb = 2;
    Chan1Ctrl.Inv = false;
    Chan1Ctrl.BWLim = false;
    Chan1Ctrl.Cplg = 1;
    Chan1Ctrl.VoltNum = VOLT_DEFAULT;
    Chan1Ctrl.SigPos = 512 + 50;
    Chan1Ctrl.DispPos = 0;
// Channel 2
    Chan2Ctrl.On = true;
    Chan2Ctrl.Prb = 2;
    Chan2Ctrl.Inv = false;
    Chan2Ctrl.BWLim = false;
    Chan2Ctrl.Cplg = 1;
    Chan2Ctrl.VoltNum = VOLT_DEFAULT;
    Chan2Ctrl.SigPos = 512 - 50;
    Chan2Ctrl.DispPos = 0;
// Mathematics
    MathCtrl.On = false;
    MathCtrl.Fun = MATH_SUM;
    MathCtrl.Sour = 0;
    MathCtrl.Wind = WIND_HANN;
    MathCtrl.Scale = 0x10;
    MathCtrl.Ofs = 0;
// Sweep
    SweepCtrl.Mode = 0;
    SweepCtrl.Roll = false;
    SweepCtrl.Stop = false;
    SweepCtrl.XY = false;
    SweepCtrl.TimeNum = TIME_DEFAULT;
    SweepCtrl.TimePos = 1024 / 2 - 125;
    SweepCtrl.Pret = 16;	// при использовании сдвигается на размер памяти
    SweepCtrl.TimeItp = 1;
// Trigger
    TrigCtrl.Sour = 1;
    TrigCtrl.Rise = true;
    TrigCtrl.Cplg = 1;
    TrigCtrl.NRej = true;
    TrigCtrl.TrigDet = false;
    TrigCtrl.Level = 0x0200;
// Display
    DispCtrl.HelpEna = false;
    DispCtrl.Lang = 1;
    DispCtrl.Vect = true;
    DispCtrl.Wide = true;
    DispCtrl.Scale = 3;
// Acquisition
    AcquCtrl.Filt = 0;
    AcquCtrl.Peak = false;
    AcquCtrl.Pers = false;
    AcquCtrl.Aver = 0;
// Cursors
    CursCtrl.On = false;
    CursCtrl.xY = false;
    CursCtrl.Sel = 0;
    CursCtrl.CX1 = 256 - 120;
    CursCtrl.CX2 = 256 + 120;
    CursCtrl.CY1 = 128 - 95;
    CursCtrl.CY2 = 128 + 95;
// Measure
    MeasCtrl.On = false;
    MeasCtrl.Sour = 0;
    MeasCtrl.Par1 = 0 << 2;
    MeasCtrl.Par2 = 1 << 2;
// Memory
    MemCtrl.SizeNum = 0;
    MemCtrl.BufNum = 0;
    MemCtrl.SigZone = 0;
    MemCtrl.SetZone = 0;
    
/* Задать управляющие переменные */
    MenuNum = MENU_SERV;
    SetNum = SET_VOID;
    
/* Восстановить сигнатуру */
    Signature = 0x76543210;
  }
  
/* Некоторые параметры сбрасывать при включении */  
  DispCtrl.Wide = true;
  
/* По содержимому управляющих структур задать режимы работы */
  //  FpVolt1_hand(0);
  FpPos1_hand(0);  
  //  FpVolt2_hand(0);
  FpPos2_hand(0);
  FpTime_hand(0);  
  //  FpPosT_hand(0);
  FpLevel_hand(0);
// Start - Stop  
  StSp_disp();
// Channel 1  
  Chan1Mode_sett();
  Chan1Cplg_sett();
  Chan1BWLim_sett();
  //  Chan1St_disp();		// отобразить в статусе
  //  PosChan1_draw(Chan1Ctrl.On);	// отобразить маркер
// Channel 2  
  Chan2Mode_sett();
  Chan2Cplg_sett();
  Chan2BWLim_sett();
  //  Chan2St_disp();		// отобразить в статусе
  //  PosChan2_draw(Chan2Ctrl.On);	// отобразить маркер
// Mathematics
  MathSt_disp();		// Индикатор состояния
// Sweep
  SweepMode_sett();
  SweepRoll_sett();
//  SweepSt_disp();
  Sweep_init();  
// Trigger
  TrigSour_sett();
  TrigSlp_sett();
  TrigCplg_sett();
  TrigNRej_sett();
  //  TrigSt_disp();
// Acquisition
  AcquFilt_sett();
  AcquPeak_sett();  
// Cursors
  Curs_draw(CursCtrl.On);	// Нарисовать на экране  
  CursMeasSt_disp();		// Показать/стереть измерения по курсорам
  SetCursSt_disp();		// Показать/стереть результат измерения
// Measure
  MeasPar1St_disp();
  MeasPar2St_disp();
  SigPar_meas(); 
// Memory
  MemSize_sett();
  SetPret_sett();		// Задать предзапуск в ПЛИС  
}

/*! @} */
