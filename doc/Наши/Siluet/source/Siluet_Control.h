
/*!*****************************************************************************
 @file		Siluet_Control.h
 @author	Козлячков В.К.
 @date		2012.10
 @version	V0.5
 @brief		Header for Control.c
 @details	Заголовочный файл для работы с функциями микроконтроллера
 @note		Должен быть подключен в Main.h
 @note		Должен находиться в папке проекта (путь к ней прописан в проекте)
 @note		Определения для отдельных устройств сгруппированы по разделам
 *******************************************************************************/

/*! @addtogroup	Control
    @{ */
 
#ifndef CONTROL_H		// Убедиться, что данный файл включен один раз
#define CONTROL_H

/*__ Заголовочные файлы _______________________________________________________*/

/*__ Глобальные типы данных ___________________________________________________*/

/*! Коды ошибок (для индикации, пересылки, и т.п.)
   заносятся в ErrorState */
typedef enum {
  ERROR_NONE,			//!< Нет ошибок
  ERROR_XTAL,			//!< Ошибка тактового генератора
  ERROR_PROG,			//!< Ошибка кода программы
  ERROR_XRAM,			//!< Ошибка внешнего ОЗУ
  //
  ERROR_SFM,			//!< Нет Serial Flash Memory
  ERROR_CODE,			//!< Нет кода конфигурации ПЛИС
  ERROR_CONF,			//!< Ошибка конфигурации ПЛИС
  //
  ERROR_DPY,			//!< Ошибка контроллера ЖКИ
  //
  ERROR_REG,			//!< Ошибка регулятора ~220 -> +12V
  ERROR_BAT			//!< Ошибка аккумулятора +12V
} Error_t;

/*! FPGA Registers
    Структура из регистров ПЛИС */
typedef struct {
  volatile uint16_t STAT;		//!< Status (rd), Sets (wr) / SIRq (rd)
  volatile uint16_t MADR;		//!< Signal Memory Address (rd-wr, не используется)  
  volatile uint16_t MFRA;		//!< Signal Memory fractional part (rd)
  volatile uint16_t MINT;		//!< Signal Memory integer part (rd)
  volatile uint16_t VOLT;		//!< Volts external Control (wr)
  volatile uint16_t ACQU;		//!< Acquisition Mode (wr)  
  volatile uint16_t RFRA;		//!< Signal Register fractional part (rd, last)
  volatile uint16_t RINT;		//!< Signal Register integer part (rd, last)
  volatile uint16_t SDAC;		//!< Serial DAC (wr, 8 registers)
  volatile uint16_t TRIG;		//!< Trigger internal & external control (wr)
  volatile uint16_t PRET;		//!< PreTrigger Counter (wr)
  volatile uint16_t HOLD;		//!< HoldOff Counter (wr)  
  volatile uint16_t ITRP;		//!< Interpolator Data (rd, Serial ADC)
  volatile uint16_t US13;   
  volatile uint16_t TIML;		//!< Samples Timer Low (wr)
  volatile uint16_t TIMH;		//!< Samples Timer High (wr)
} PL_TypeDef;

/*__ Глобальные определения констант __________________________________________*/

//__ MCU__//

/*******************************************************************************
 MicroController Unit
 In case of a 16-bit external memory width, the FSMC will internally use
 HADDR[25:1] to generate the address for external memory FSMC_A[24:0]
 Bank 1 NOR/PSRAM 4 x 64 MB
 Device	Select	StartAddress	Size
 FLMS_N	NE1	0x60000000	0x800000
 RAMS_N	NE2	0x64000000	0x80000
 PLS_N	NE3	0x68000000	0x20
 LCDS_N	NE4	0x6C000000	0x40000 
 */

//__ XRAM __//

/*******************************************************************************
 External RAM:
 IS62WV25616BLL-55T 256K x 16 Ultra Low Power CMOS SRAM
 Start Address 0x64000000
 Size 0x80000
 */
// #define XRAM_BASE	0x64000000UL	//!< стартовый адрес внешнего ОЗУ
// #define XRAM_SIZE	0x80000UL	//!< размер внешнего ОЗУ (в байтах)


//__ SFM __//

/*******************************************************************************
 Serial Flash Memory M25PE40
 4 Mbit (512 KB) Page-Erasable Flash memory
 Page size: 256 bytes (total 2048 pages)
 SubSector Erase (4 Kbytes), Sector Erase (64 Kbytes), Bulk Erase (4 Mbits)
 Первым посылается-принимается старший бит
 В многобайтовых словах первым посылается-принимается старший байт (big-endian)
 */
#define SFM		SPI1			//!< Serial Flash Memory M25PE40 Interface
#define M25PE40ID	((uint32_t)0x208013)	//!< идентификатор M25PE40
#define SFMSIZE		((uint32_t)0x00080000)	//!< размер памяти в байтах (512 * 1024)
#define SFMPAGE		0x100			//!< Serial Flash Memory Page Size

/* Распределение пространства памяти SFM */

/* Код инициализации ПЛИС (сжат) может занимать до половины памяти (до 256 K)
   счетчик кода инициализации ПЛИС (4 байта) расположен в начале кода 
   инициализации (3000000 битов, если без сжатия) */
#define SFMCODE_BASE	((uint32_t)0x00000000)

/* Сигнал сохраняется в третьей четверти памяти */
#define SFMSIG_BASE	((uint32_t)0x00040000)
// Сигналы памяти 1
#define SFMSIG1		((uint32_t)0x00040000)
// Сигналы памяти 2
#define SFMSIG2		((uint32_t)0x00050000)

/* Системные переменные находятся в последней четверти памяти */
#define SFMSYS_BASE	((uint32_t)0x00060000)
// Переменные интерполятора
#define SFMINTERP	((uint32_t)(SFMSYS_BASE + SFMPAGE * 0))
// Баланс 1
#define SFMBAL1		((uint32_t)(SFMSYS_BASE + SFMPAGE * 1))
// Баланс 2
#define SFMBAL2		((uint32_t)(SFMSYS_BASE + SFMPAGE * 2))

/*! Описание команд Serial Flash Memory M25PE40
    отмеченные * только для uC T9HX процесса
    в комментариях указывается количество байтов - Address, Dummy, Data */
#define SFM_WREN	0x06		//!< Write Enable - 0, 0, 0
#define SFM_WRDI	0x04		//!< Write Disable - 0, 0, 0
#define SFM_RDID	0x9F		//!< Read Identification - 0, 0, 1..3
#define SFM_RDSR	0x05		//!< * Read Status Register - 0, 0, 1 to ~
#define SFM_WRLR	0xE5		//!< * Write to Lock Register - 3, 0, 1
#define SFM_WRSR	0x01		//!< * Write Status Register - 0, 0, 1
#define SFM_RDLR	0xE8		//!< Read Lock Register - 3, 0, 1
#define SFM_READ	0x03		//!< Read Data Bytes - 3, 0, 1 to ~
#define SFM_FSTRD	0x0B		//!< Fast Read Data Bytes - 1, 1, 1 to ~
#define SFM_PW		0x0A		//!< Page Write - 3, 0, 1 to 256
#define SFM_PP		0x02		//!< Page Program - 3, 0, 1 to 256
#define SFM_PE		0xDB		//!< Page Erase - 3, 0, 0
#define SFM_SSE		0x20		//!< * SubSector Erase - 3, 0, 0
#define SFM_SE		0xD8		//!< Sector Erase - 3, 0, 0
#define SFM_BE		0xC7		//!< * Bulk Erase - 0, 0, 0
#define SFM_DP		0xB9		//!< Deep Power-down - 0, 0, 0
#define SFM_RDP		0xAB		//!< Release from Deep Power-down - 0, 0, 0
//
#define SFM_DUMMY	0xA5		//!< Dummy Byte for read
#define SFM_WIP		0x01		//!< Write In Progress Flag


//__ PL __//

#define PLPS		SPI2		//!< PL Passive Serial Configuration

/*******************************************************************************
 Адреса ПЛИС (16-битовая шина, A25:A1 сдвинуты в AB24:AB0)
 16 16-битовых регистров, мультиплексированная шина адрес / данные
 Базовый адрес 0x68000000
 */
#define	PL		((PL_TypeDef *)PL_BASE)

/* Биты регистров ПЛИС */

/* STAT */
#define	PL_START_RESET	((uint16_t)0x0001)	//!< Reset Registers (wr)
#define	PL_START_ACQUST	((uint16_t)0x0002)	//!< Start Frame Acquisition (wr)
#define	PL_START_ITDCST	((uint16_t)0x0004)	//!< Interpolator Discharge start (wr)

#define	PL_STAT_DOTIF	((uint16_t)0x0001)	//!< Dot Interrupt Flag (rd_clear)
#define	PL_STAT_FRMIF	((uint16_t)0x0002)	//!< Frame Interrupt Flag (rd_clear)
#define	PL_STAT_TDTIF	((uint16_t)0x0004)	//!< Trig Detector Toggle IF (rd_clear)

#define	PL_STAT_PRETDN	((uint16_t)0x0010)	//!< Pretrig Done
#define	PL_STAT_TRIGDN	((uint16_t)0x0020)	//!< Trigger Done
#define	PL_STAT_POSTDN	((uint16_t)0x0040)	//!< Posttrig Done (full frame)
#define	PL_STAT_TRIGDET	((uint16_t)0x0080)	//!< Trigger Detected

/* VOLT */
// Управление V/div (x5/x1 не работает)
// #define	PL_VOLT_S1ON2DE		((uint16_t)0x0002)	//!< Ones to Deci
// #define	PL_VOLT_S1DE2ON		((uint16_t)0x000d)	//!< Deci to Ones
// #define	PL_VOLT_S1ON2CE		((uint16_t)0x0007)	//!< Ones to Centi
// #define	PL_VOLT_S1CE2ON		((uint16_t)0x0008)	//!< Centi to Ones
// #define	PL_VOLT_S1DC2AC		((uint16_t)0x0006)	//!< DC to AC
// #define	PL_VOLT_S1AC2DC		((uint16_t)0x0009)	//!< AC to DC
// #define	PL_VOLT_S1DIV1		((uint16_t)0x0000)	//!< Divider /1
// #define	PL_VOLT_S1DIV2		((uint16_t)0x0010)	//!< Divider /2
// #define	PL_VOLT_S1DIV4		((uint16_t)0x0020)	//!< Divider /4
// #define	PL_VOLT_S1DIV10		((uint16_t)0x0030)	//!< Divider /10
// #define	PL_VOLT_S1EN		((uint16_t)0x0040)	//!< Signal Channel 1 enable

// #define	PL_VOLT_S2ON2DE		((uint16_t)0x0200)	//!< Ones to Deci
// #define	PL_VOLT_S2DE2ON		((uint16_t)0x0d00)	//!< Deci to Ones
// #define	PL_VOLT_S2ON2CE		((uint16_t)0x0700)	//!< Ones to Centi
// #define	PL_VOLT_S2CE2ON		((uint16_t)0x0800)	//!< Centi to Ones
// #define	PL_VOLT_S2DC2AC		((uint16_t)0x0600)	//!< DC to AC
// #define	PL_VOLT_S2AC2DC		((uint16_t)0x0900)	//!< AC to DC
// #define	PL_VOLT_S2DIV1		((uint16_t)0x0000)	//!< Divider /1
// #define	PL_VOLT_S2DIV2		((uint16_t)0x1000)	//!< Divider /2
// #define	PL_VOLT_S2DIV4		((uint16_t)0x2000)	//!< Divider /4
// #define	PL_VOLT_S2DIV10		((uint16_t)0x3000)	//!< Divider /10
// #define	PL_VOLT_S2EN		((uint16_t)0x4000)	//!< Signal Channel 2 enable
// #define	PL_VOLT_SKODI		((uint16_t)0x8000)	//!< Volts Relay Logic Output disable 

#define PL_VOLT_S1EN	((uint16_t)0x0000)	//!< Signal Channel 1 enable
#define	PL_VOLT_S1DI	((uint16_t)0x0040)	//!< Signal Channel 1 disable
#define	PL_VOLT_S2EN	((uint16_t)0x0000)	//!< Signal Channel 2 enable
#define	PL_VOLT_S2DI	((uint16_t)0x4000)	//!< Signal Channel 2 disable
#define	PL_VOLT_S2ALT	((uint16_t)0x0000)	//!< Signal Channel 2 clock alternate
#define	PL_VOLT_S2SYN	((uint16_t)0x8000)	//!< Signal Channel 2 clock synchronous

/* ACQU */
#define PL_ACQU_FILT	((uint16_t)0x0003)	//!< Hi-Res Filter S
#define PL_ACQU_FILT1S	((uint16_t)0x0000)	//!< Hi-Res Filter 1 Sample
#define PL_ACQU_FILT2S	((uint16_t)0x0001)	//!< Hi-Res Filter 2 Sample
#define PL_ACQU_FILT4S	((uint16_t)0x0002)	//!< Hi-Res Filter 4 Sample
#define PL_ACQU_FILT8S	((uint16_t)0x0003)	//!< Hi-Res Filter 8 Sample
#define PL_ACQU_PEAK	((uint16_t)0x0004)	//!< Peak Detection
#define PL_ACQU_IE	((uint16_t)0x0070)	//!< Interrupt enable bits
#define PL_ACQU_DOTIE	((uint16_t)0x0010)	//!< Dot Interrupt enable
#define PL_ACQU_FRMIE	((uint16_t)0x0020)	//!< Frame Interrupt enable
#define PL_ACQU_TDTIE	((uint16_t)0x0040)	//!< Trigger detector toggle IE
#define PL_ACQU_MEM	((uint16_t)0x0700)	//!< Acquisition Memory Bits
#define PL_ACQU_MEM05K	((uint16_t)0x0000)	//!< Acquisition Memory 0.5K
#define PL_ACQU_MEM1K	((uint16_t)0x0100)	//!< Acquisition Memory 1K
#define PL_ACQU_MEM2K	((uint16_t)0x0200)	//!< Acquisition Memory 2K
#define PL_ACQU_MEM4K	((uint16_t)0x0300)	//!< Acquisition Memory 4K
#define PL_ACQU_MEM8K	((uint16_t)0x0400)	//!< Acquisition Memory 8K
#define PL_ACQU_MEM16K	((uint16_t)0x0500)	//!< Acquisition Memory 16K

/* SDAC */
// К битам выбора нужного канала ЦАП добавить 12 битов данных
// #define	PL_SDAC_S1POS		((uint16_t)0x0000)	//!< Signal 1 Position
// #define	PL_SDAC_S2POS		((uint16_t)0x1000)	//!< Signal 2 Position
// #define	PL_SDAC_S1REF		((uint16_t)0x2000)	//!< Signal 1 Reference
// #define	PL_SDAC_S2REF		((uint16_t)0x3000)	//!< Signal 2 Reference
// #define	PL_SDAC_T1LEV		((uint16_t)0x4000)	//!< Trigger 1 Level
// #define	PL_SDAC_T2LEV		((uint16_t)0x5000)	//!< Trigger 2 Level
// #define	PL_SDAC_TXLEV		((uint16_t)0x6000)	//!< Trigger Ext Level
// #define	PL_SDAC_TEST		((uint16_t)0x7000)	//!< Test 
// #define	PL_SDAC_GAIN		((uint16_t)0x8000)	//!< Gain, Ref Mode
// #define	PL_SDAC_LDAC		((uint16_t)0xa000)	//!< LDAC Mode
// #define	PL_SDAC_PWRDN		((uint16_t)0xc000)	//!< Power down
// #define	PL_SDAC_DARES		((uint16_t)0xe000)	//!< Data reset
// #define	PL_SDAC_DCRES		((uint16_t)0xf000)	//!< Data & Control reset

//__ TRIG __//
#define PL_TRIG_SRC	((uint16_t)0x0003)	//!< Trigger Source Bits
#define PL_TRIG_LINE	((uint16_t)0x0000)	//!< Trigger Source Line
#define PL_TRIG_CH1	((uint16_t)0x0001)	//!< Trigger Source Channel 1
#define PL_TRIG_CH2	((uint16_t)0x0002)	//!< Trigger Source Channel 2
#define PL_TRIG_EXT	((uint16_t)0x0003)	//!< Trigger Source External
//
#define PL_TRIG_CPLG	((uint16_t)0x000C)	//!< Trigger Coupling Bits
#define PL_TRIG_LF	((uint16_t)0x0000)	//!< Trigger Low Freq
#define PL_TRIG_DC	((uint16_t)0x0004)	//!< Trigger DC
#define PL_TRIG_AC	((uint16_t)0x0008)	//!< Trigger AC
#define PL_TRIG_HF	((uint16_t)0x000C)	//!< Trigger High Freq
//
#define PL_TRIG_HYST	((uint16_t)0x0010)	//!< Trigger Hysteresis On
#define PL_TRIG_FALL	((uint16_t)0x0000)	//!< Trigger Fall Slope Polarity
#define PL_TRIG_RISE	((uint16_t)0x0020)	//!< Trigger Rise Slope Polarity
#define PL_TRIG_AUTO	((uint16_t)0x0000)	//!< Trigger Auto
#define PL_TRIG_NORM	((uint16_t)0x0040)	//!< Trigger Normal
#define PL_TRIG_DIV	((uint16_t)0x0080)	//!< Trigger Divide On

/* TIME */
// #define PL_TIMH_MAIN		((uint16_t)0x0000)	//!< Timer Main (write to TIMERH)
// #define PL_TIMH_FAST1		((uint16_t)0x4000)	//!< Timer Fast /1 (write to TIMERH)
// #define PL_TIMH_FAST2		((uint16_t)0x8000)	//!< Timer Fast /2 (write to TIMERH)
// #define PL_TIMH_FAST5		((uint16_t)0xc000)	//!< Timer Fast /5 (write to TIMERH)
#define PL_TIMH_LDP	((uint16_t)0x8000)	//!< TimerHigh Continuous Load Pulse 


//__ Analog Part Control __//

/* Для управления аналоговой частью используется SPI3 */
#define ANAS		SPI3		//!< Analog Part Serial Interface

/* Биты выбора ЦАП, добавить к пересылаемому значению */
#define DAC_CHAN1	((uint16_t)0x2000)	//!< DACA - Channel 1
#define DAC_CHAN2	((uint16_t)0x6000)	//!< DACB - Channel 2
#define DAC_LEVEL	((uint16_t)0xA000)	//!< DACC - Trig Level
#define DAC_XTRA	((uint16_t)0xE000)	//!< DACD - extra

/*__ Глобальные макрофункции __________________________________________________*/

//__ MCU bits set, reset, read __//
/* BSRRL - set bits, BSRRH - reset bits */

#define SMSS_ON()	GPIOA->BSRRH = 0x0010		//!< SFM select (Low)
#define SMSS_OFF()	GPIOA->BSRRL = 0x0010		//!< SFM deselect (High)
#define ANSS_ON()	GPIOA->BSRRL = (uint16_t)(1<<15)	//!< ANSS High
#define ANSS_OFF()	GPIOA->BSRRH = (uint16_t)(1<<15)	//!< ANSS Low
#define LED_ON()	GPIOB->BSRRH = 0x0002		//!< Включить светодиод (Low)
#define LED_OFF()	GPIOB->BSRRL = 0x0002		//!< Выключить светодиод (High)
#define RSTO_ON()	GPIOC->BSRRH = (uint16_t)(1<<0)	//!< Выдать сброс (Low)
#define RSTO_OFF()	GPIOC->BSRRL = (uint16_t)(1<<0)	//!< Убрать сброс (High)
#define ANSYN_ON()	GPIOD->BSRRH = (uint16_t)(1<<2)	//!< ANSYN Low
#define ANSYN_OFF()	GPIOD->BSRRL = (uint16_t)(1<<2)	//!< ANSYN High
#define TEST_ON()	GPIOE->BSRRL = 0x0004		//!< Установить TestPin (Pin_1)
#define TEST_OFF()	GPIOE->BSRRH = 0x0004		//!< Сбросить TestPin (Pin_1)


//__ PL bits set, reset, read __//

#define CONF_ON()	GPIOB->BSRRH = 0x1000		//!< CONF Low
#define CONF_OFF()	GPIOB->BSRRL = 0x1000		//!< CONF High
#define PLIRQ_GET()	(ALIAS_PTR(GPIOG->IDR,7))	//!< Get status PLIRQ_N
#define STAT_GET()	(ALIAS_PTR(GPIOC->IDR,2))	//!< Get PL STAT_N
#define CONFDN_GET()	(ALIAS_PTR(GPIOC->IDR,1))	//!< Get PL CONFDN


/*!*****************************************************************************
 @brief		Delay start & wait time in us (1 us steps)
 @details	Старт и ожидание окончания задержки в микросекундаx
 @param		US - delay time (1 ... 65 536 us)
 @return	
 @note		Используется Timer_start(), Timer_wait()
 */
#define DELAY_START(US)	Timer_start(US-1)
#define DELAY_CHECK()	Timer_check()
#define DELAY_WAIT()	Timer_wait()

/*!*****************************************************************************
 * @brief	Wide time delay start, check, wait in 1, 10, 100, 1000 us steps
 * @details	Старт, проверка и ожидание окончания задержки в микросекундаx
 * @param	US - delay time (1 ... 65 536 000 us)
 */
#define WDELAY_START(US);						\
  TIM7->PSC = ((US < 65536)? (60 - 1) :					\
	      ((US < 655360)? (600 - 1) : 				\
	      ((US < 6553600)? (6000 - 1) : (60000 - 1))));		\
  TIM7->ARR = ((US < 65536)? (US - 1) :					\
	      ((US < 655360)? (US / 10 - 1) :				\
	      ((US < 6553600)? (US / 100 - 1) : (US / 1000 - 1))));	\
  TIM7->EGR = TIM_EGR_UG;						\
  TIM7->SR = 0;								\
  TIM7->CR1 |= TIM_CR1_CEN;

#define WDELAY_RESTART();		\
  TIM7->EGR = TIM_EGR_UG;		\
  TIM7->SR = 0;				\
  TIM7->CR1 |= TIM_CR1_CEN;

#define WDELAY_CHECK	(TIM7->SR)
#define WDELAY_WAIT();	while (!TIM7->SR);

//__ Analog DAC send __//

/*!*****************************************************************************
 @brief		Send Data to Analog Serial DAC
 @details	Добавить адрес и послать данные в ЦАП
 @param		ADDR - адрес (DAC_CHAN1, DAC_CHAN2, DAC_TRIG, DAC_XTRA)
 @param		DATA - данные (0x0000 - 0x0FFF) 
 @return	
 @note		для AD5314 младшие два бита не используются
 */
#define ANADAC(ADDR, DATA)	AnaDAC_send(ADDR | DATA & 0x0FFF)

/*__ Глобальные переменные ____________________________________________________*/

_DECL volatile uint8_t ErrorState _INIT(0);	//<! Состояния ошибок для индикации

//__ PL vars __//

// Буферы для управления регистрами в ПЛИС
_DECL uint16_t PL_VoltBuf _INIT(PL_VOLT_S2SYN | PL_VOLT_S2EN | PL_VOLT_S1EN);
_DECL uint16_t PL_AcquBuf _INIT(PL_ACQU_MEM1K | PL_ACQU_TDTIE | PL_ACQU_FRMIE | PL_ACQU_FILT1S);
_DECL uint16_t PL_TrigBuf _INIT(PL_TRIG_AUTO | PL_TRIG_RISE | PL_TRIG_HYST
		| PL_TRIG_DC | PL_TRIG_CH1);
// _DECL uint16_t PL_S1PosBuf _INIT(0x0800);	// PL_SDAC_S1POS |
// _DECL uint16_t PL_S2PosBuf _INIT(0x0800);	// PL_SDAC_S2POS |
// _DECL uint16_t PL_S1RefBuf _INIT(0x0800);	// PL_SDAC_S1REF |
// _DECL uint16_t PL_S2RefBuf _INIT(0x0800);	// PL_SDAC_S2REF | 
// _DECL uint16_t PL_T1LevBuf _INIT(0x0800);	// PL_SDAC_T1LEV |
// _DECL uint16_t PL_T2LevBuf _INIT(0x0800);	// PL_SDAC_T2LEV |
// _DECL uint16_t PL_TXLevBuf _INIT(0x0800);	// PL_SDAC_TXLEV |
// _DECL uint16_t PL_PreTrBuf _INIT(0x0040);

//__ Analog Part Control __//

_DECL uint32_t AnaRegBuf _INIT(0x00005454);	// Analog Shift Register Buffer

/* Сигнатура для проверки батарейного питания памяти */
_DECL uint32_t Signature __attribute((section("backup")));

/*__ Глобальные прототипы функций _____________________________________________*/

//__ MCU __//

void MCU_init(void);			// Инициализировать микроконтроллер
void MCU_test(void);			// Общие тесты процессора и периферии
void Led_blink(uint32_t delay);		// Мигать светодиодом бесконечно
void Error_blink(void);			// Индицировать ошибку светодиодом
void Test_send(uint8_t TestData);	// Послать тестовые данные в порт
void Timer_start(uint16_t time);	// Timer start
bool Timer_check(void);			// Timer done check
// void Timer_wait(void) __attribute__((always_inline));	// Timer done wait
void Timer_wait(void);

void XRAM_test(void);			// Тест внешней памяти
// void DpyBuf2LCD_copy(uint32_t Offset, uint32_t Size);	// копировать буфер в ЖКИ

//__ SFM __//

uint32_t SFMID_read(void);		// Читать идентификатор
uint32_t SFMWord_read(uint32_t Addr);	// Читать слово из памяти
void SFMBulk_erase(void);		// Стереть всю память
void SFMSect_erase(uint32_t Addr);	// Стереть сектор 64KB
void SFMSubSect_erase(uint32_t Addr);	// Стереть субсектор 4KB
void SFMPage_erase(uint32_t Addr);	// Стереть страницу 256B
void SFMPage_write(uint8_t *pMem, uint32_t Addr, uint32_t Numb);	// Записать страницу 11ms
void SFMPage_progr(uint8_t *pMem, uint32_t Addr, uint32_t Numb);	// Запрограммировать 0.8ms
//
void SFMRead_start(uint32_t Addr);	// Подготовить чтение байтов
uint8_t SFMByte_read(void);		// Читать байт (за байтом)
void SFMRead_stop(void);		// Завершить чтение байтов

//__ PL __//

uint8_t PL_config(void);	// Конфигурация ПЛИС
void PL_init(void);		// Инициализация регистров, созданных в ПЛИС

//__ Analog Part Control __//

void AnaReg_send(void);			// Послать данные в сдвиговый регистр
void AnaDAC_send(uint16_t data);	// Послать данные в ЦАП

//__ Power On Control __//
void Balance_init(void);	// Восстановить сохраненные данные баланса
void Ctrl_init(void);		// Инициализировать все режимы работы 


#endif				// SILUET_CONTROL

/*! @} */
