
/*!*****************************************************************************
 @file		Siluet_Display.c
 @author	Козлячков В.К.
 @date		2012.10
 @version	V0.5 
 @brief		LCD Controller functions
 @details	Функции контроллера ЖКИ S1D13706
 @note
 *******************************************************************************
 @verbatim
 Описание контроллера S1D13706 и ЖКИ Tianma TM057KDH01
   320 x 240 color TFT, 8 bpp (256 colors)
   The LUT supports six bits per primary color (256K possible colors)

   На шину адреса AB15:AB0 STM32 выводится сдвинутая шина A16:A1.
   Схема подключения контроллера ЖКИ к микроконтроллеру создана, исходя из 
   сравнения сигналов микроконтроллера STM32 в режиме 16-битовой шины и 
   контроллера ЖКИ S1D13706 в режиме Generic #2 (Little Endian).
   
	STM32		S1D13706	Memory
	NBL0	NBL1	A0	BHE#	2N+1	2N
	0	0	0	0	DH	DL
	0	1	0	1	_	DL
	1	0	1	0	DH	_
	1	1	1	1	_	_

   Получается, нужно соединить сигналы следующим образом:
	STM32F103	S1D13706
	A16		M/R#
	A15:A0		A16:A1
	NBL0		A0
	NBL1		BHE#(WE1#)
	NOE		RD#
	NWR		WE#(WE0#)
	
   На вход CLKI (max 66 MHz) S1D13706 подается частота 40 MHz с MCO1 STM32.
   Доступ к контроллеру ЖКИ формируется сигналом FSMC_NE4 (0x6C000000)
   Доступ к регистрам или памяти (M/R#) определяется сигналом A16 (AB17) 
   Схема позволяет читать и записывать 16-битовыми или 8-битовыми словами
   (используется 16-битовое обращение)
  
   Описание сигналов:
	Микроконтроллер	Шина		Контроллер ЖКИ
	PG12(NE4)	LCDS_N		CS#
	(A16)		AB17		M/R#
	(A15..A0)	AB16..AB1	AB16..AB1
	PE0		BL0_N		AB0
	PE1		BL1_N		WE1#
	(DB15..DB0)	DB15..DB0	DB15..DB0
	PD4(NOE)	OE_N		RD#
	PD5(NWE)	WE_N		WE0#
	PD6(NWAIT)	WAIT_N		WAIT#
	PC0		RSTO_N		RESET#
	PA8(MCO1)	LCDCLK		CLKI, CLKI2

   Сигналы контроллера ЖКИ:
	GPIO4		FP_R/L
	GPIO5		FP_U/D
	GPO		VRMT (вкл. питание подсветки)
 @endverbatim
 *******************************************************************************/
/* 
 2011.02
 Оставляю функции для внешнего (или внутреннего) буфера
 Куда писать - определится константой DPYMEM (может быть LCDRAM или DPYBUF)
 Переименовал функции к виду DpyBorder_draw()

 2012.10
 Использую только внешний буфер DPYBUF, потом переношу его в контроллер ЖКИ
 Доступ к контроллеру ЖКИ происходит только полусловами (16-битовыми).
 Байтовый (8-битовый) доступ не используется.
 */ 

/*! @addtogroup	Display
    @{ */


/*__ Заголовочные файлы _______________________________________________________*/

#include "Siluet_Main.h"		// Все включения заданы в одном файле
// #include "s1d13706.h"			// Определения регистров S1D13706
#include "Siluet_Fonts.h"		// Шрифты, специальные символы

/*__ Определения констант _____________________________________________________*/

/*__ Макрофункции _____________________________________________________________*/

/*__ Локальные прототипы функций ______________________________________________*/

void ExtDpyBorder_draw(bool On);
void ExtDpyCross_draw(bool On);
void ExtDpyGrid_draw(bool On);

void DpyBorder_draw(bool On);		// рисовать рамку вокруг рабочей области
void DpyCross_draw(bool On);		// рисовать крест в рабочей области
void DpyGrid_draw(bool On);		// рисовать точки сетки в рабочей области

uint32_t DpyChar_draw(volatile uint8_t *pDpy, uint8_t symb, uint32_t palet);
uint32_t DpyCharPad_draw(volatile uint8_t *pDpy, uint8_t symb, uint32_t palet);

/*__ Локальные переменные _____________________________________________________*/

// uint16_t ExtDpyBuf[0x9600] __attribute__((at(0x64000000), zero_init));
// uint16_t ExtDpyBuf[0x9600];
// uint16_t ExtDpyBuf[0x3600] __attribute__((at(0x64000000), zero_init));

/*__ Глобальные функции _______________________________________________________*/

/*!*****************************************************************************
 @brief		Display Controller Initialize
 @details	Инициализировать контроллер ЖКИ S1D13706
 @return	
 @note		Использовался файл s1d13706.h, сгенерированный S1D13706CFG.EXE
		Panel: 320 x 240 x 8bpp 60Hz, Color TFT 18-bit (PCLK=6.667MHz)
		Reg 0x12 содержит Horizontal Total, + 1) x 8 = количество пикселов
		0x34 = 52 + 1 = 53 x 8 = 424
 */
void LcdCtrl_init(void) {
#define REGS_NUM (sizeof(Regs) / sizeof(Regs_t))
  typedef struct {
    uint8_t Index;
    uint16_t Value;
  } Regs_t;				// индекс регистров и их содержимое

/* Структура для конфигурации S1D13706
   BCLK -> MCLK -> PCLK
   40MHz -> 20MHz -> 6.667MHz 400x280@60Hz */
  static const Regs_t Regs[] = {
    {0x04, 0x2010},	// PCLK Config  Register, BUSCLK MEMCLK Config Register
    {0x10, 0x0061},	// MOD Rate Register, PANEL Type Register
//    {0x12, 0x0031},	// Horizontal Total Register
    {0x12, 0x0032},		// (50 + 1) * 8 = 408
    {0x14, 0x0027},	// Horizontal Display Period Register
//    {0x16, 0x0013},	// Horizontal Display Period Start Pos Register 1, Register 0
//    {0x16, 0x0041},		// 65 + 5 = 70
    {0x16, 0x0049},		// 73 + 5 = 78 
//    {0x18, 0x0117},	// Vertical Total Register 1, Register 0
    {0x18, 0x0105},		// 261 + 1 = 262
    {0x1C, 0x00EF},	// Vertical Display Period Register 1, Register 0
//    {0x1E, 0x000C},	// Vertical Display Period Start Pos Register 1, Register 0
//    {0x1E, 0x000D},		// 13
    {0x1E, 0x0015},		// 21
//    {0x20, 0x0007},	// Horizontal Sync Pulse Width Register
    {0x20, 0x0003},		// 4
    {0x22, 0x0007},	// Horizontal Sync Pulse Start Pos Register 1, Register 0
    {0x24, 0x0007},	// Vertical Sync Pulse Width Register//x //@
    {0x26, 0x0008},	// Vertical Sync Pulse Start Pos Register 1, Register 0
    {0x70, 0x0003},	// Special Effects Register, Display Mode Register
    {0x74, 0x0000},	// Main Window Display Start Address Register 1, Register 0
    {0x76, 0x0000},	// Main Window Display Start Address Register 2
    {0x78, 0x0050},	// Main Window Address Offset Register 1, Register 0
    {0x7C, 0x0000},	// Sub Window Display Start Address Register 1, Register 0
    {0x7E, 0x0000},	// Sub Window Display Start Address Register 2
    {0x80, 0x0050},	// Sub Window Address Offset Register 1, Register 0
    {0x84, 0x0000},	// Sub Window X Start Pos Register 1, Register 0
    {0x88, 0x0000},	// Sub Window Y Start Pos Register 1, Register 0
    {0x8C, 0x004F},	// Sub Window X End Pos Register 1, Register 0
    {0x90, 0x00EF},	// Sub Window Y End Pos Register 1, Register 0
    {0xA0, 0x0000},	// CPU Access Control Register, Power Save Config Register
    {0xA2, 0x0000},	// BIG Endian Support Register, Software Reset Register
    {0xA4, 0x0000},	// Scratch Pad Register 1, Register 0
    {0xA8, 0x0030},	// GPIO Config Register 1, Register 0
    {0xAC, 0x0020},	// GPIO Status Control Register 1, Register 0
// Подсветку пока не включать (PWM Clock Enable = 0), было {0xB0, 0x2410}    
    {0xB0, 0x2400},	// PWM CV Clock Config Register, Control Register
    {0xB2, 0xFA00},	// PWM Clock Duty Cycle Register, CV Clock Burst Length Register
  };

/* Палитра цветов R, G, B
   D7  D6  D5  D4  D3  D2  D1  D0
   Cur Grd Sel Mat Ch2 Ch2 Ch1 Ch1
 
   0   0   0   0  1/0 1/0 1/0 1/0  Канал 2, Канал 1
   0   0   0   1   X   X   X   X   Математика
   0   0   1   0  1/0 1/0 1/0 1/0  Выделение каналов
   0   0   1   1   X   X   X   X   Выделение математики
   0   1   0   0   X   X   X   X   Сетка поверх каналов
   0   1   0   1   X   X   X   X   Сетка поверх математики
   0   1   1   0   S   S   S   S   Дополнительные цвета (0x38)
   0   1   1   1   S   S   S   S   Дополнительные цвета (0x38)
   1   0   0   0   X   X   X   X   Курсоры поверх каналов
   1   0   0   1   X   X   X   X   Курсоры поверх математики
   1   0   1   0   G   G   G   G   Дополнительные цвета (серая шкала)
   1   0   1   1   G   G   G   G   Дополнительные цвета (грубая серая шкала)
   1   1   0   0   C   C   C   C   Дополнительные цвета (B = 0x00)
   1   1   0   1   C   C   C   C   Дополнительные цвета (B = 0x54)
   1   1   1   0   C   C   C   C   Дополнительные цвета (B = 0xA8)
   1   1   1   1   C   C   C   C   Дополнительные цвета (B = 0xFC)
 
   В каждом из 8-битовых слов R, G, B младшие 2 бита нулевые (максимальный код 252).
   Разложив 252/4 на простые сомножители, имеем 63 = 7 * 3 * 3 = 21 * 3 = 7 * 9.
   Любой один из двух сомножителей можно умножить на 4, второй даст количество шагов.
   Все варианты градаций цветов представлены ниже.
    (4 st) x00, x54, xa8, xfc
    (8 st) x00, x24, x48, x6c, x90, xb4, xa8, xfc
   (10 st) x00, x1c, x38, x54, x70, x8c, xa8, xc4, xe0, xfc
   (22 st) x00, x0c, x18, x24, x30, x3c, x48, x54, x60, x6c, x78,
           x84, x90, x9c, xa8, xb4, xc0, xcc, xd8, xe4, xf0, xfc
   (64 st) ... через 4 ...
   
   Новый расчет, считая максимальное значение FF = 3 * 5 * 17
   Возможны следующие градации:
    (4st) 0x00, 0x55, 0xAA, 0xFF
    (6st) 0x00, 0x33, 0x66, 0x99, 0xCC, 0xFF
   (16st) 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
          0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF 
   (18st) 0x00, 0x0F, 0x1E, 0x2D, 0x3C, 0x4B, 0x5A, 0x69, 0x78,
          0x87, 0x96, 0xA5, 0xB4, 0xC3, 0xD2, 0xE1, 0xF0, 0xFF 
   
   
 */
  static const uint8_t LUT_Color[256 * 3] = {
/* 00..0F
   Канал 1 (в строке 0), Канал 2 (в столбце 0) и их смесь */
    0x00, 0x00, 0x00,  0x38, 0x54, 0x00,  0x70, 0xA8, 0x00,  0xA8, 0xFC, 0x00,
    0x00, 0x54, 0x38,  0x38, 0x54, 0x38,  0x70, 0xA8, 0x38,  0xA8, 0xFC, 0x38,
    0x00, 0xA8, 0x70,  0x38, 0xA8, 0x70,  0x70, 0xA8, 0x70,  0xA8, 0xFC, 0x70,
    0x00, 0xFC, 0xA8,  0x38, 0xFC, 0xA8,  0x70, 0xFC, 0xA8,  0xA8, 0xFC, 0xA8,
/* 10..1F
   Математическая обработка, синхронизация */
    0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,
    0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,
    0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,
    0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,  0xAA, 0x55, 0xFF,
/* 20..2F
   Выделение каналов */
    0x38, 0x38, 0x38,  0x54, 0x54, 0x54,  0x70, 0x70, 0x70,  0x8c, 0x8c, 0x8c,
    0x54, 0x54, 0x54,  0x54, 0x54, 0x54,  0x70, 0x70, 0x70,  0x8c, 0x8c, 0x8c,
    0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x8c, 0x8c, 0x8c,  0xa8, 0xa8, 0xa8,
    0x8c, 0x8c, 0x8c,  0x8c, 0x8c, 0x8c,  0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,
/* 30..3F
   Выделение математики */
    0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,
    0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,
    0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,
    0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,  0xa8, 0xa8, 0xa8,
/* 40..4F
   Сетка поверх каналов */
    0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,
    0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,
    0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,
    0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,
/* 50..5F
   Сетка поверх математики */
    0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,
    0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,
    0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,
    0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,  0x70, 0x70, 0x70,
/* 60..6F
   Дополнительные цвета (грубая серая шкала - 10 градаций)
   Дополнительные цвета (точная серая шкала) - 22 градации) */
    0x00, 0x00, 0x00,  0x1c, 0x1c, 0x1c,  0x38, 0x38, 0x38,  0x54, 0x54, 0x54,
    0x70, 0x70, 0x70,  0x8c, 0x8c, 0x8c,  0xa8, 0xa8, 0xa8,  0xc4, 0xc4, 0xc4,
    0xe0, 0xe0, 0xe0,  0xfc, 0xfc, 0xfc,  0x00, 0x00, 0x00,  0x0c, 0x0c, 0x0c,
    0x18, 0x18, 0x18,  0x24, 0x24, 0x24,  0x30, 0x30, 0x30,  0x3c, 0x3c, 0x3c,
/* 70..7F
   Дополнительные цвета (продолжение точной серой шкалы) */
    0x48, 0x48, 0x48,  0x54, 0x54, 0x54,  0x60, 0x60, 0x60,  0x6c, 0x6c, 0x6c,
    0x78, 0x78, 0x78,  0x84, 0x84, 0x84,  0x90, 0x90, 0x90,  0x9c, 0x9c, 0x9c,
    0xa8, 0xa8, 0xa8,  0xb4, 0xb4, 0xb4,  0xc0, 0xc0, 0xc0,  0xcc, 0xcc, 0xcc,
    0xd8, 0xd8, 0xd8,  0xe4, 0xe4, 0xe4,  0xf0, 0xf0, 0xf0,  0xfc, 0xfc, 0xfc,
/* 80..8F
   Курсоры поверх каналов */
    0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,
    0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,
    0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,
    0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,
/* 90..9F
   Курсоры поверх математики */
    0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,
    0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,
    0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,
    0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xAA, 0x55, 0x00,
/* A0..AF
   Цвета текста со сглаживанием (недостающие цвета) */
    0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0xfc, 0xfc, 0xfc,  0xfc, 0xfc, 0xfc,
    0x00, 0x00, 0x00,  0x00, 0x00, 0xfc,  0xfc, 0xfc, 0x00,  0xfc, 0xfc, 0xfc,
    0xfc, 0xfc, 0xfc,  0xfc, 0xfc, 0xfc,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,
    0x60, 0x54, 0x30,  0x00, 0x00, 0x6c,  0x6c, 0x6c, 0xfc,  0xc0, 0xa8, 0x60,
/* B0..BF
   Цвета текста со сглаживанием (продолжение) */
    0x90, 0x90, 0x00,  0x00, 0x00, 0x90,  0x90, 0x90, 0xfc,  0xfc, 0xfc, 0x90,
    0x30, 0x30, 0x30,  0x30, 0x30, 0xfc,  0xfc, 0xfc, 0x30,  0xfc, 0xfc, 0xfc,
    0x00, 0x00, 0x30,  0x00, 0x00, 0x60,  0x60, 0x54, 0x00,  0xc0, 0xa8, 0x00,
    0x60, 0x54, 0x30,  0xc0, 0xa8, 0x60,  0x60, 0x54, 0xfc,  0xfc, 0xfc, 0x60,
/* C0..CF
   64 универсальных цвета, весь цветовой круг
   Дополнительные цвета (R = 0..F; G = 0, 5, A, F; B = 0) */
    0x00, 0x00, 0x00,  0x55, 0x00, 0x00,  0xAA, 0x00, 0x00,  0xFF, 0x00, 0x00,
    0x00, 0x55, 0x00,  0x55, 0x55, 0x00,  0xAA, 0x55, 0x00,  0xFF, 0x55, 0x00,
    0x00, 0xAA, 0x00,  0x55, 0xAA, 0x00,  0xAA, 0xAA, 0x00,  0xFF, 0xAA, 0x00,
    0x00, 0xFF, 0x00,  0x55, 0xFF, 0x00,  0xAA, 0xFF, 0x00,  0xFF, 0xFF, 0x00,
/* D0..DF
   Дополнительные цвета (R = 0..F; G = 0, 5, A, F; B = 5) */
    0x00, 0x00, 0x55,  0x55, 0x00, 0x55,  0xAA, 0x00, 0x55,  0xFF, 0x00, 0x55,
    0x00, 0x55, 0x55,  0x55, 0x55, 0x55,  0xAA, 0x55, 0x55,  0xFF, 0x55, 0x55,
    0x00, 0xAA, 0x55,  0x55, 0xAA, 0x55,  0xAA, 0xAA, 0x55,  0xFF, 0xAA, 0x55,
    0x00, 0xFF, 0x55,  0x55, 0xFF, 0x55,  0xAA, 0xFF, 0x55,  0xFF, 0xFF, 0x55,
/* E0..EF
   Дополнительные цвета (R = 0..F; G = 0, 5, A, F; B = A) */
    0x00, 0x00, 0xAA,  0x55, 0x00, 0xAA,  0xAA, 0x00, 0xAA,  0xFF, 0x00, 0xAA,
    0x00, 0x55, 0xAA,  0x55, 0x55, 0xAA,  0xAA, 0x55, 0xAA,  0xFF, 0x55, 0xAA,
    0x00, 0xAA, 0xAA,  0x55, 0xAA, 0xAA,  0xAA, 0xAA, 0xAA,  0xFF, 0xAA, 0xAA,
    0x00, 0xFF, 0xAA,  0x55, 0xFF, 0xAA,  0xAA, 0xFF, 0xAA,  0xFF, 0xFF, 0xAA,
/* F0..FF
   Дополнительные цвета (R = 0..F; G = 0, 5, A, F; B = F) */
    0x00, 0x00, 0xFF,  0x55, 0x00, 0xFF,  0xAA, 0x00, 0xFF,  0xFF, 0x00, 0xFF,
    0x00, 0x55, 0xFF,  0x55, 0x54, 0xFF,  0xAA, 0x55, 0xFF,  0xFF, 0x55, 0xFF,
    0x00, 0xAA, 0xFF,  0x55, 0xAA, 0xFF,  0xAA, 0xAA, 0xFF,  0xFF, 0xAA, 0xFF,
    0x00, 0xFF, 0xFF,  0x55, 0xFF, 0xFF,  0xAA, 0xFF, 0xFF,  0xFF, 0xFF, 0xFF
  };

  // uint16_t idx = 0;			// индекс в списке регистров и палитре (256)
/* Выключить ЖКИ (Blank display)
   16-битовая шина, 16-битовый доступ, сдвинутый адрес */
  // *((volatile uint16_t *)(LCDREG + 0x70)) |= 0x80;
//  DISPLAYMODE->DMR |= 0x80;
  *(uint16_t *)DISPLAYMODE_BASE |= 0x0080;
  
/* Загрузить все S1D13706 регистры */
  for(uint32_t i = 0; i < REGS_NUM; i++) {
    // *((volatile uint16_t *)(LCDREG + S1DRegs[i].Index)) = S1DRegs[idx].Value;
    *((volatile uint16_t *)(DPYREGS_BASE + Regs[i].Index)) = Regs[i].Value;  
    // idx++;
  }
  
/* Загрузить палитру (LookUp Table)
   16-битовая шина, 8-битовый доступ */
#if false
  volatile uint8_t *pLCDReg = (uint8_t *)(LCDREG);
  for (idx = 0; idx < 256; idx++) {
    *(pLCDReg + 0x0A) = LUT_Color[3 * idx];	// Red
    *(pLCDReg + 0x09) = LUT_Color[3 * idx + 1];	// Green
    *(pLCDReg + 0x08) = LUT_Color[3 * idx + 2];	// Blue
    *(pLCDReg + 0x0B) = ((uint8_t)idx);		// write index to update LUT
  }

  for (uint32_t i = 0; i < 256; i++) {
    LOOKUPTABLE->LUTRWDR = LUT_Color[3 * i];		// Red
    LOOKUPTABLE->LUTGWDR = LUT_Color[3 * i + 1];	// Green
    LOOKUPTABLE->LUTBWDR = LUT_Color[3 * i + 2];	// Blue
    LOOKUPTABLE->LUTWAR = (uint8_t)i;			// write index
  }
#endif  
  for (uint32_t i = 0; i < 256; i++)
    *(uint32_t *)LOOKUPTABLE_BASE = i << 24			// Addr
				  | LUT_Color[3 * i] << 16	// Red
				  | LUT_Color[3 * i + 1] << 8	// Green
				  | LUT_Color[3 * i + 2];	// Blue
  
/* Убрать энергосберегающий режим */
  // *(pLCDReg + 0xa0) &= ~0x01;
///  MISCELL->PSCR &= ~0x01;
  *(uint16_t *)MISCELL &= ~0x0001;
  
  // *(pLCDReg + 0xad) |= 0x80;		// Включить лампу подсветки
  // *(pLCDReg + 0x70) &= ~0x80;		// включить ЖКИ
///  DISPLAYMODE->DMR &= ~0x80;
  *(uint16_t *)DISPLAYMODE_BASE &= ~0x0080;
  
// Включить PWM (Рано, есть белая засветка экрана!)  
//  *(uint16_t *)PWMCVCONF |= 0x10;

/* Тест - прочитать и сравнить 9-й номер цвета */
#if false
  *(pLCDReg + 0x0f) = 9;
  if ( (*(pLCDReg + 0x0c) == LUT_Color[3 * 9 + 2]) &&
       (*(pLCDReg + 0x0d) == LUT_Color[3 * 9 + 1]) &&
       (*(pLCDReg + 0x0e) == LUT_Color[3 * 9 + 0]) ) {
    LED_ON();
    Delay(1);
    LED_OFF();
  }
#endif
}

/*!*****************************************************************************
 @brief		LCD Test
 @details	Проверить память контроллера ЖКИ - записать, прочитать, сравнить
 @return	
 @note
 */
void LcdMem_test(void) {
#if false
  TEST_ON();
  volatile uint8_t *pLcd = (uint8_t *)(DPYMEM_BASE);
  for (uint32_t i = 0; i < DPYMEM_SIZE; i++) {
    *(pLcd + i) = (uint8_t)(i & 0xF0 | ~i & 0x0F);
    if (*(pLcd + i) == (uint8_t)(i & 0xF0 | ~i & 0x0F)) {
      LED_ON();
      __nop();
      LED_OFF();
    }
    else {
      ErrorState = ERROR_DPY;
      break;
    }
  }
  TEST_OFF();

// Рисовать одним цветом 4 пиксела в строке 
  TEST_ON();
  volatile uint32_t *pLcd = (uint32_t *)DPYMEM_BASE;
  for (uint32_t i = 0; i < DPYMEM_SIZE / 4; i++) {
    uint32_t j  = (uint8_t)i << 24 | (uint8_t)i << 16
		| (uint8_t)i <<  8 | (uint8_t)i;
    *pLcd = j;
    if (*pLcd++ == j) {
      LED_ON();
      LED_OFF();
    }
    else {
      ErrorState = ERROR_DPY;
      break;
    }
  }
  TEST_OFF();
#endif

// Нарисовать три полных палитры по 32 * 8 кубиков
  TEST_ON();
  volatile uint16_t *pLcd = (uint16_t *)DPYMEM_BASE;
  uint8_t n = 0;				// индекс цвета
  for (uint32_t pal = 3; pal--; ) {		// палитры на экране
    for (uint32_t row = 8; row--; ) {		// ряды в палитре 
      for (uint32_t str = 10; str--; ) {	// строки в ряду
        for (uint32_t col = 32; col--; ) {	// колонки в строке
          for (uint32_t pair = 5; pair--; ) {	// пары пикселов в колонках
            uint16_t m = n << 8 | n;		// цвет пары пикселов
	    *pLcd = m;
            if (*pLcd++ == m) { LED_ON(); LED_OFF(); }
            else { ErrorState = ERROR_DPY; return; }
          }
          n++;					// новый цвет
        }
        n -= 32;				// восстановить цвет в строке
      }
      n += 32;					// новый ряд, цвет продолжить
    }						// ряды закончить
  }						// палитры закончить
  TEST_OFF();
}


/*!*****************************************************************************
 @brief		LCD Clear
 @details	Очистить память контроллера ЖКИ (записать 0x00)
 @return	
 @note		Очищается вся память контроллера ЖКИ 
 */
void LcdMem_clear(void) {
  uint32_t *pLcd = (uint32_t *)(DPYMEM_BASE);
  for (uint32_t i = DPYMEM_SIZE / 4; i--; ) {
    *pLcd++ = 0x00000000;	// black
    // *pLcd++ = 0xFCFCFCFC;	// cyan
  }
}

/*!*****************************************************************************
 @brief		Vertical non-Display period status
 @details	Состояние, что в текущий момент изображение не выводится
 @return	1 - nonDysplay period
 @note		использовалось для устранения мерцания меняющегося изображения
 */
bool LcdVnD_check(void) {
  // return ((*(uint8_t *)(DPYREG+0xa0))>>7);
  return (MISCELL->PSCR)>>7;
}

/*!*****************************************************************************
 @brief		ExtDpy Buffer copy to DpyMem
 @details	Копировать буфер в память ЖКИ
 @param		Offset - смещение в буфере (и памяти ЖКИ) в байтах
 @param		Size - количество байтов для пересылки
 @return	
 @note		Для работы из Flash использовать DMA
 @note		Проверять TCIF в основной программе перед вызовом
 */
void DpyBuf2Mem_copy(uint32_t Offset, uint32_t Size) {
/*
  LED_ON();					// ~38 команд
  DMA2->IFCR |= DMA_IFCR_CTCIF1;		// сбросить флаг прерывания
  DMA2_Channel1->CCR &= ~0x00000001;		// запретить пересылку
  DMA2_Channel1->CPAR = DPYBUF + Offset;	// начальный адрес буфера экрана
  DMA2_Channel1->CMAR = LCDRAM + Offset;	// начальный адрес памяти ЖКИ
  DMA2_Channel1->CNDTR = Size/2;		// 16-битовые пересылки  
  DMA2_Channel1->CCR |= 0x00000001;		// разрешить пересылку
//  while (!(DMA2->ISR & DMA_ISR_TCIF1));	// ждать флаг прерывания
  LED_OFF();
 */
/*
// Программная пересылка буфера в контроллер ЖКИ
  uint16_t *pSour = (uint16_t *)(DPYBUF + Offset);
  uint16_t *pDist = (uint16_t *)(LCDRAM + Offset);
  LED_ON();
  for (uint32_t i=Size/2; i--; ) {
    *pDist++ = *pSour++;
  }
  LED_OFF();
 */

// Программная пересылка буфера в контроллер ЖКИ 32-битовыми словами
  uint32_t *pSour = (uint32_t *)(DPYBUF_BASE + Offset);
  uint32_t *pDist = (uint32_t *)(DPYMEM_BASE + Offset);
//  LED_ON();
  for (uint32_t i = Size / 4; i--; ) {
    *pDist++ = *pSour++;
  }
//  LED_OFF();  
}

/*!*****************************************************************************
 @brief		Clear Screen blank
 @details	Очистить весь экран (залить черным цветом)
 @return	
 @note
 */
void DpyScreen_blank(void) {
  DPY_RECT(0, 0, 319, 239, 0xFF, BLANK);
}

/*!*****************************************************************************
 @brief		Work area clear blank
 @details	Очистить рабочее поле (залить черным цветом)
 @return	
 @note
 */
void DpyWork_blank(void) {
  DPY_RECT(DPYWAH, DPYWAV, DPYWAH + 250, DPYWAV + 200, 0xFF, BLANK);
}

/*!*****************************************************************************
 @brief		Clear MENU Area blank
 @details	Очистить область МЕНЮ (залить черным цветом)
 @return	
 @note		
 */
void DpyMenu_blank(void) {
  DPY_RECT(DPYWAH + 248, DPYWAV, 319, DPYWAV + 200, 0xFF, BLANK);
}

/*!*****************************************************************************
 @brief		Clear area except MENU blank
 @details	Очистить пространство, кроме МЕНЮ (залить черным цветом)
 @return	
 @note		
 */
void DpyExMenu_blank(void) {
  DPY_RECT(0, 0, DPYWAH + 250, 239, 0xFF, BLANK);
}

/*!*****************************************************************************
 @brief		Clear area Number draw
 @details	Очистить пространство, где печатается служебное число
 @return	
 @note		
 */
void DpyNum_blank(void) {
  DPY_RECT(DPYWAH + 300 - 11 * 8 - 2, 1, 319, 10, 0xFF, BLANK);
}

/*!*****************************************************************************
 @brief		Signal & Mathematics clear
 @details	Очистить изображение (сигналов, математики, курсоров...)
 @param		mask	битовая маска для 2-х пикселов (0 - сбрасывает биты)
 @return	
 @note		Обрабатывается по 2 пиксела, адрес должен быть выравнен на 2
 */
void DpyWork_clear(uint16_t mask) {
  uint32_t width  = DispCtrl.Wide ? 300 / 2 : 250 / 2;
  uint16_t *pDpy = (uint16_t *)(DPYBUF_BASE + ((DPYWAV) * 320) + 8);
  for (uint32_t y = 200; y--; ) {
    for (uint32_t x = width; x--; ) {
      uint16_t temp = *pDpy; temp &= mask; *pDpy++ = temp;
    }
    pDpy += 320 / 2 - width;			// next line
  }
}


/*!*****************************************************************************
 @brief		Draw Palette
 @details	Нарисовать всю палитру, 16 х 16 прямоугольников
 @return	
 @note		
 */
void DpyPalet_draw(void) {
// Указатель на начало в памяти контроллера ЖКИ
  volatile uint8_t *pDpy = (uint8_t *)(DPYBUF_BASE + 40);  
  uint8_t index = 0;
  for (uint8_t YRect = 16; YRect--; ) {		// Части в столбце
    uint8_t indexTemp = index;			// Сохранить начальный индекс
    for (uint8_t YLine = 15; YLine--; ) {	// Строки в части столбца
      index = indexTemp;			// Восстановить начальный индекс
      for (uint8_t XRect = 16; XRect--; ) {	// Части в строке
        for (uint8_t XDot = 15; XDot--; ) {	// Точки в части строки (20 FullScreen)
          *pDpy++ = index;			// Записать точку
        }
        index++;				// Следующий индекс в строке
      }
      pDpy += 80;
    }
  }
}

/*!*****************************************************************************
 @brief		Draw Font
 @details	Нарисовать шрифт в матрице 16 х 14
 @return	
 @note
 */
void DpyFont_draw(void) {
// Белым по черному
//  volatile uint8 *pLCDMem = (uint8 *)(LCD_MEM_START + 16 + 16 * 320);
// volatile uint8 *pLCDMem = LCDPOS(LCDWAH + 14,4+15*2);
  volatile uint8_t *pDpy = DPYPOS(43, 4 + 15 * 2);
  uint8_t Test = 0x20;
  uint8_t *pTest = &Test;
  volatile uint8_t *pTmp = pDpy;
  for (int32_t j = 0; j < 14; j++) {
    for (int32_t i = 0; i < 16; i++) {
      DpyChar_draw(pDpy, *pTest, WHITE_ON_BLACK);
      pDpy += 15;
      Test++;
    }
    pDpy = pTmp += 320 * 15;		// так и должно быть!
  }
/*
  // Черным по белому
  pLCDMem = (uint8 *)(LCD_MEM_START + 160 + 16 + 16 * 320);
  Test = 0x20;
  pLCDTmp = pLCDMem;
  for (int8 j=0; j<14; j++) {
    for (int8 i=0; i<16; i++) {
      LCD_DrawChar(pLCDMem, *pTest, BLACK_ON_WHITE);
      pLCDMem += 8;
      Test++;
    }
    pLCDMem = pLCDTmp += 320 * 12;
  }
 */
  pDpy = DPYPOS(43, 4);
  for (int32_t i = 0; i < 16; i++) {
    DpySpec_draw(pDpy, (SSYM_t)i, 0xff, WHITE_ON_BLACK);
    pDpy += 15;    
  }
}

/*!*****************************************************************************
 @brief		Manufacturer draw
 @details	
 @note		
 */
void Manufact_draw(void) {
  // DpyStringPad_draw(DPYPOS(DPYWAH, 2), "МНИПИ", 0xF0F00000, false);
  // DpyStringPad_draw(DPYPOS(DPYWAH, 1), "МНИПИ", 0x1F1F0000, false);
  DpyStringPad_draw(DPYPOS(DPYWAH + 2, 1), MANUFACT, 0x40400000, false);  
}


/*!*****************************************************************************
 @brief		Work Scale draw
 @details	Нарисовать шкалу во внешнем буфере нужной формы и размера
 @return	
 @note		Disp.Scale:
		0 - Frame, 1 - Cross(+Frame), 2 - Grid(+Frame), 3 - Cross+Grid(+Frame)
 @note		Disp.Wide:
		0 - 250(5+5 div), 1 - 300(5+7 div)
 */
/*
    0    5   10   15   20   25                       125                  300:250 (319)
  0 ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
  1 f    f    f    f    f    f    f    f    f    f    f    f    f    f    f    f
  2 f                        f                        f                        f
  3 f                                                 f                        f
  4 f                                                                          f
  5 ff                       g                       cgc                      ff
    f                                                                          f
    f                                                 c                        f
    f                                                 c                        f
    f                                                                          f
 10 ff                       g                       cgc                      ff
    f                                                                          f
    f                                                 c                        f
    f                                                 c                        f
    f                                                                          f
 15 ff                       g                       cgc                      ff
    f                                                                          f
    f                                                 c                        f
    f                                                 c                        f
    f                                                                          f
 20 ff                       g                       cgc                      ff
    f                                                                          f
    f                                                 c                        f
    f                                                 c                        f
    f                                                                          f
 25 fff  g    g    g    g    g    g    g    g    g   cgc   g    g    g    g  fff
    f                                                                          f
    f                                                 c                        f
    f                                                 c                        f
    f                                                                          f
 30 ff                       g                       cgc                      ff
    f                                                                          f
    f                                                 c                        f
    f                                                 c                        f
    f                                                                          f
 35 ff                       g                       cgc                      ff
    f                                                                          f
    f                                                 c                        f
    f                                                 c                        f
    f                                                                          f
 40 ff                       g                       cgc                      ff
    f                                                                          f
    f                                                 c                        f
    f                                                 c                        f
    f                                                                          f
    ff                       g                       cgc                      ff
    f                                                                          f
    f                                                 c                        f
    f                                                 c                        f
    f    c    c    c    c    c    c    c    c    c    c    c    c    c    c    f
100 ffff g cc g cc g cc g cc g cc g cc g cc g cc g cccgccc g cc g cc g cc g ffff
    f    c    c    c    c    c    c    c    c    c    c    c    c    c    c    f
    f                                                 c                        f
    f                                                 c                        f
    f                                                                          f
195 ff                       g                       cgc                      ff
196 f                                                                          f
197 f                                                 f                        f
198 f                        f                        f                        f
199 f    f    f    f    f    f    f    f    f    f    f    f    f    f    f    f
200 ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
 */

void DpyScale_draw(void) {
  if (GenCtrl.Help) return;
  switch(DispCtrl.Scale) {
  case 3:				// Нарисовать всё - сетку, крест, рамку
    DpyCross_draw(true);
    DpyGrid_draw(true);
    DpyBorder_draw(true);
    break;
  case 2:				// Нарисовать сетку
    DpyCross_draw(false);
    DpyGrid_draw(true);
    DpyBorder_draw(true);
    break;
  case 1:				// Нарисовать центральный крест
    DpyCross_draw(true);
    DpyBorder_draw(true);
    break;
  case 0:				// Нарисовать рамку вокруг раб. поля
  default:
    DpyCross_draw(false);
    DpyGrid_draw(false);
    DpyBorder_draw(true);
  }
}

/*!*****************************************************************************
 @brief		Text String draw
 @details	Написать текстовую строку
 @param		pDpy - указатель на память в контроллере ЖКИ
 @param		pText - указатель на строку в памяти микроконтроллера
 @param		palet - палитра для 0, 1, 2 и 3 субпикселов
 @param		monospace - моноширинный шрифт
 @return	
 */
void DpyString_draw(uint8_t *pDpy,
		    const uint8_t *pText, uint32_t palet, bool monospace)
{
  while (*pText) {
    uint32_t space = DpyChar_draw(pDpy, *pText, palet);
    pDpy += (monospace ? 8 : space);
    pText++;
  }
}

/*!*****************************************************************************
 @brief		Text String with Background Pad draw
 @details	Написать текстовую строку с фоном
 @param		pDpy - указатель на память в контроллере ЖКИ
 @param		pText - указатель на строку в памяти микроконтроллера
 @param		palette - палитра для 0, 1, 2 и 3 субпикселов
 @param		monospace - моноширинный шрифт
 @return	
 @note		Зазор между символами равен 8 пикселов, но записывается 10
 */
void DpyStringPad_draw(uint8_t *pDpy,
		       const uint8_t *pText, uint32_t palet, bool monospace)
{
  while (*pText) {
    uint32_t space = DpyCharPad_draw(pDpy, *pText, palet);
    pDpy += (monospace ? 8 : space);
    pText++;
  }
}

/*!*****************************************************************************
 @brief		Display Rectangle fill
 @details	Заполнить прямоугольник во внешнем буфере дисплея
 @param		pDpy - указатель на верхний левый угол
 @param		XSize, YSize - размеры
 @param		MaskColor - маска << 8 | цвет
 @return	
 @note		
 */
void DpyRect_fill(uint8_t *pDpy,
		  uint32_t XSize, uint32_t YSize, uint32_t MaskColor)
{
  uint32_t color = (uint8_t)MaskColor;		// маска заменяемых битов
  uint32_t mask = (uint8_t)(MaskColor >> 8);	// цвет для замены
  for (uint32_t j = 0; j < YSize; j++) {
    for (uint32_t i = 0; i < XSize; i++) {
      uint8_t temp = *(pDpy + i); temp ^= (temp ^ color) & mask; *(pDpy + i) = temp;
    }
    pDpy += 320;				// начало новой строки
  }
}

/*!*****************************************************************************
 @brief		Display Rounded Rectangle fill
 @details	Закрасить скругленный прямоугольник во внешнем буфере дисплея
 @param		pDpy - указатель на верхний левый угол
 @param		XSize, YSize - размеры
 @param		MaskColor - маска << 8 | цвет
 @return	
 @note		
 */
void DpyRoRect_fill(uint8_t *pDpy,
		    uint32_t XSize, uint32_t YSize, uint32_t MaskColor)
{
  uint32_t color = (uint8_t)MaskColor;		// Маска битов для замены
  uint32_t mask = (uint8_t)(MaskColor >> 8);	// Цвет для замены
/* Первая строка */
  for (uint32_t i = 4; i < XSize - 4; i++) {
    uint8_t temp = *(pDpy + i); temp ^= (temp ^ color) & mask; *(pDpy + i) = temp;
  }
  pDpy += 320;
/* Вторая строка */
  for (uint32_t i = 2; i < XSize - 2; i++) {
    uint8_t temp = *(pDpy + i); temp ^= (temp ^ color) & mask; *(pDpy + i) = temp;
  }
  pDpy += 320;
/* Третья и четвертая */
  for (uint32_t j = 0; j < 2; j++) {
    for (uint32_t i = 1; i < XSize - 1; i++) {
      uint8_t temp = *(pDpy + i); temp ^= (temp ^ color) & mask; *(pDpy + i) = temp;
    }
    pDpy += 320;
  }
/* Остальные строки */
  for (uint32_t j = 0; j < YSize - 8; j++) {
    for (uint32_t i = 0; i < XSize; i++) {
      uint8_t temp = *(pDpy + i); temp ^= (temp ^ color) & mask; *(pDpy + i) = temp;
    }
    pDpy += 320;
  }
/* Четвертая и третья с конца строки */
  for (uint32_t j = 0; j < 2; j++) {
    for (uint32_t i = 1; i < XSize - 1; i++) {
      uint8_t temp = *(pDpy + i); temp ^= (temp ^ color) & mask; *(pDpy + i) = temp;
    }
    pDpy += 320;
  }
/* Вторая с конца строка */
  for (uint32_t i = 2; i < XSize - 2; i++) {
    uint8_t temp = *(pDpy + i); temp ^= (temp ^ color) & mask; *(pDpy + i) = temp;
  }
  pDpy += 320;
/* Последняя строка */
  for (uint32_t i = 4; i < XSize - 4; i++) {
    uint8_t temp = *(pDpy + i); temp ^= (temp ^ color) & mask; *(pDpy + i) = temp;
  }
}

/*!*****************************************************************************
 @brief		Horizontal Line draw
 @details	Нарисовать горизонтальную линию во внешнем буфере
 @param		pDpy - указатель на верхний левый угол
 @param		XSize - размеры по горизонтали
 @param		PattMaskColor - шаблон << 16 | маска << 8 | цвет
 @return	
 @note		
 */
void DpyLineHorz_draw(uint8_t *pDpy, uint32_t XSize, uint32_t PattMaskColor)
{
  uint32_t color = (uint8_t) PattMaskColor;		// цвет для замены
  uint32_t mask = (uint8_t)(PattMaskColor >> 8);	// маска битов для замены  
  uint32_t pattern = (uint8_t)(PattMaskColor >> 16);	// шаблон линии
  for (uint32_t i = 0; i < XSize; i++) {
    if (pattern & 0x01) {
      uint8_t temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
    }
    pattern = (pattern >> 1) | (pattern << 7);
  }
}

/*!*****************************************************************************
 @brief		Vertical Line draw
 @details	Нарисовать вертикальную линию во внешнем буфере
 @param		pDpy - указатель на верхний левый угол
 @param		YSize - размеры по верттикали
 @param		PattMaskColor - шаблон << 16 | маска << 8 | цвет
 @return	
 @note		
 */
void DpyLineVert_draw(uint8_t *pDpy, uint32_t YSize, uint32_t PattMaskColor)
{
  uint32_t color = (uint8_t) PattMaskColor;		// цвет для замены
  uint32_t mask = (uint8_t)(PattMaskColor >> 8);	// маска битов для замены  
  uint32_t pattern = (uint8_t)(PattMaskColor >> 16);	// шаблон линии  
  for (uint32_t i = 0; i < YSize; i++) {
    if (pattern & 0x01) {
      uint8_t temp = *pDpy;  temp ^= (temp ^ color) & mask;  *pDpy = temp;
    }
    pattern = (pattern >> 1) | (pattern << 7);
    pDpy += 320;
  }
}

 
/* Локальные функции ___________________________________________________________*/

/*!*****************************************************************************
 @brief		Work Border draw
 @details	Нарисовать рамку рабочего поля во внешнем буфере
 @param		On - Вкл/Выкл
 @return	
 */
void DpyBorder_draw(bool On) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  uint32_t color = On << 6;		// цвет сетки (выкл/вкл)
  uint32_t mask = 1 << 6;		// маска сетки
  uint8_t  temp;			// временное хранилище пиксела  
// Линия 0 (верхняя)
  volatile uint8_t *pDpy = (uint8_t *)(DPYBUF_BASE + DPYWAV * 320 + DPYWAH);
  for (uint32_t i = 0; i <= width; i++) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }
// Линия 1 (через 5 точек)
  pDpy += 320;
  for (uint32_t i = 0; i <= width; i += 5) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }
// Линия 2 (через 25 точек)
  pDpy += 320;
  for (uint32_t i = 0; i <= width; i += 25) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }
// Линия 3 (хвостик в середине)
  pDpy += 320;
  temp = *(pDpy + 125);  temp ^= (temp ^ color) & mask;  *(pDpy + 125) = temp;
// Линия 197 (хвостик в середине)
  pDpy = (uint8_t *)(DPYBUF_BASE +((DPYWAV + 197) * 320) + DPYWAH);
  temp = *(pDpy + 125);  temp ^= (temp ^ color) & mask;  *(pDpy + 125) = temp;
// Линия 198 (через 5 точек)
  pDpy += 320;
  for (uint32_t i = 0; i <= width; i += 25) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }
// Линия 199 (через 5 точек)
  pDpy += 320;
  for (uint32_t i = 0; i <= width; i += 5) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }
// Линия 200 (нижняя)
  pDpy += 320;
  for (uint32_t i = 0; i <= width; i++) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }  
// С 3-й линии по 197 включительно, группами по 5 линий
  pDpy = (uint8_t *)(DPYBUF_BASE + ((DPYWAV + 3) * 320) + DPYWAH);
  for (uint32_t i = 1; i <= 39; i++) {
    temp = *(pDpy);  temp ^= (temp ^ color) & mask;  *(pDpy) = temp;
    temp = *(pDpy + width);  temp ^= (temp ^ color) & mask;  *(pDpy + width) = temp;
    pDpy += 320;
    temp = *(pDpy);  temp ^= (temp ^ color) & mask;  *(pDpy) = temp;
    temp = *(pDpy + width);  temp ^= (temp ^ color) & mask;  *(pDpy + width) = temp;
    pDpy += 320;
    temp = *(pDpy);  temp ^= (temp ^ color) & mask;  *(pDpy) = temp;
    temp = *(pDpy + 1);  temp ^= (temp ^ color) & mask;  *(pDpy + 1) = temp;
    temp = *(pDpy + width - 1);  temp ^= (temp ^ color) & mask;  *(pDpy + width - 1) = temp;
    temp = *(pDpy + width);  temp ^= (temp ^ color) & mask;  *(pDpy + width) = temp;
    pDpy += 320;
    temp = *(pDpy);  temp ^= (temp ^ color) & mask;  *(pDpy) = temp;
    temp = *(pDpy + width);  temp ^= (temp ^ color) & mask;  *(pDpy + width) = temp;
    pDpy += 320;
    temp = *(pDpy);  temp ^= (temp ^ color) & mask;  *(pDpy) = temp;
    temp = *(pDpy + width);  temp ^= (temp ^ color) & mask;  *(pDpy + width) = temp;
    pDpy += 320;
  }
// Начиная с 25-й линии
  pDpy = (uint8_t *)(DPYBUF_BASE + ((DPYWAV + 25) * 320) + DPYWAH);
  for (uint32_t i = 25; i <= 175; i += 25) {
    temp = *(pDpy + 2);  temp ^= (temp ^ color) & mask;  *(pDpy + 2) = temp;
    temp = *(pDpy + width - 2);  temp ^= (temp ^ color) & mask;  *(pDpy + width - 2) = temp;
    pDpy += 8000;
  }
// Линия 100 (центральная)
  pDpy = (uint8_t *)(DPYBUF_BASE + ((DPYWAV + 100) * 320) + DPYWAH);
  temp = *(pDpy + 3);  temp ^= (temp ^ color) & mask;  *(pDpy + 3) = temp;
  temp = *(pDpy + width - 3);  temp ^= (temp ^ color) & mask;  *(pDpy + width - 3) = temp;
}

/*!*****************************************************************************
 @brief		Center Cross draw
 @details	Нарисовать центральный крест в рабочем поле во внешнем буфере ЖКИ
 @param		On - Вкл/Выкл
 @return	
 */
void DpyCross_draw(bool On) {
  uint32_t width = (DispCtrl.Wide ? 300 : 250);
  uint32_t color = On << 6;		// цвет сетки (выкл/вкл)
  uint32_t mask = 1 << 6;		// маска сетки
  uint8_t  temp;			// временное хранилище пиксела
// С 3-й линии по 197 включительно, группами по 5 линий
  uint8_t *pDpy = (uint8_t *)(DPYBUF_BASE + ((DPYWAV + 3) * 320) + DPYWAH);
  for (uint32_t i = 1; i <= 39; i++) {
    temp = *(pDpy + 125);  temp ^= (temp ^ color) & mask;  *(pDpy + 125) = temp;
    pDpy += 640;			// через две линии
    temp = *(pDpy + 124);  temp ^= (temp ^ color) & mask;  *(pDpy + 124) = temp;
    temp = *(pDpy + 125);  temp ^= (temp ^ color) & mask;  *(pDpy + 125) = temp;
    temp = *(pDpy + 126);  temp ^= (temp ^ color) & mask;  *(pDpy + 126) = temp;
    pDpy += 640;			// через две линии
    temp = *(pDpy + 125);  temp ^= (temp ^ color) & mask;  *(pDpy + 125) = temp;
    pDpy += 320;			// следующая линия
  }
// Линия 99 (через 5 точек)
  pDpy = (uint8_t *)(DPYBUF_BASE + ((DPYWAV + 99) * 320) + DPYWAH);
  for (uint32_t i = 5; i < width; i += 5) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }
// Линия 100 (* * * через 5 точек)
  pDpy += 320;
  for (uint32_t i = 5; i < width; i += 5) {
    temp = *(pDpy + i - 2);  temp ^= (temp ^ color) & mask;  *(pDpy + i - 2) = temp;
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
    temp = *(pDpy + i + 2);  temp ^= (temp ^ color) & mask;  *(pDpy + i + 2) = temp;
  }
// Линия 101 (через 5 точек)
  pDpy += 320;
  for (uint32_t i = 5; i < width; i += 5) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }
}

/*!*****************************************************************************
 @brief		Work Grid (Dots) draw
 @details	Нарисовать точечную сетку в рабочем поле во внешнем буфере
 @param		On - Вкл/Выкл
 @return	
 */
void DpyGrid_draw(bool On) {
  uint32_t width = (DispCtrl.Wide ? 300 : 250);
  uint32_t color = On << 6;		// цвет сетки (выкл/вкл)
  uint32_t mask = 1 << 6;		// маска сетки
  uint8_t  temp;			// временное хранилище пиксела
// Линия 5 (через 25 точек)
  uint8_t *pDpy = (uint8_t *)(DPYBUF_BASE + ((DPYWAV + 5) * 320) + DPYWAH);
  for (uint32_t i = 25; i <= width; i += 25) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }
// Линия 10 (через 25 точек)
  pDpy += 1600;				// 320 * 5
  for (uint32_t i = 25; i <= width; i += 25) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }
  pDpy += 1600;				// 320 * 5
  for (uint32_t j = 1; j <= 7; j++) {
    for (uint32_t i = 25; i <= width; i += 25) {
      temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
    }
    pDpy += 1600;
    for (uint32_t i = 25; i <= width; i += 25) {
      temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
    }
    pDpy += 1600;
    for (uint32_t i = 5; i <= width; i += 5) {
      temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
    }
    pDpy += 1600;
    for (uint32_t i = 25; i <= width; i += 25) {
      temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
    }
    pDpy += 1600;
    for (uint32_t i = 25; i <= width; i += 25) {
      temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
    }
    pDpy += 1600;
  }
// Линия 190
  for (uint32_t i = 25; i <= width; i += 25) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }
  pDpy += 1600;
// Линия 195
  for (uint32_t i = 25; i <= width; i += 25) {
    temp = *(pDpy + i);  temp ^= (temp ^ color) & mask;  *(pDpy + i) = temp;
  }
}

/*!*****************************************************************************
 @brief		LCD Draw Character
 @details	Нарисовать символ (пропорциональным или моноширинным шрифтом)
 @param		pDpy - указатель на вехний левый край символа в памяти LCD
 @param		symb - символ
 @param		palet - начало палитры цвета для rgb, rgB, RGb, RGB
 @return	Ширина символа в пикселах (для пропорционального вывода шрифта)
 @note		Переписываются 10х8 пикселов (фон - только на краях символа)
 */
uint32_t DpyChar_draw(volatile uint8_t *pDpy, uint8_t symb, uint32_t palet) {
  uint32_t width = 0;
  uint32_t index = (symb - 0x10) * 10;
//  if (!index) width = 4;		// ширина пробела
  if (symb == ' ') width = 4;		// ширина пробела
  for (uint32_t i = 0; i < 10; i++) {
    uint32_t row = Font8[index + i];
    for (uint32_t j = 0; j < 8; j++) {
      uint32_t temp = (row & 0x0000c000) >> 14;	// два бита
      if (temp)
        *(pDpy + j) = (uint8_t)(palet >> (8 * temp));
      if (temp && (j > width)) width = j;
      row <<= 2;
    }
    pDpy += 320;
  }
  return width + 2;			// ширина с учетом зазора между символами
}

/*!*****************************************************************************
 @brief		LCD Draw Character
 @details	Нарисовать символ (пропорциональным или моноширинным шрифтом)
 @param		pDpy - указатель на вехний левый край символа в памяти LCD
 @param		symb - символ
 @param		palet - палитра цвета для rgb, rgB, RGb, RGB (4-байтовое слово)
 @return	Ширина символа в пикселах (для пропорционального вывода шрифта)
 @note		Переписываются 10х10 пикселов (зазор между символами, фон тоже)
 @note		Для 2-пиксельного зазора можно использовать символы \x7f, \x98
 */
uint32_t DpyCharPad_draw(volatile uint8_t *pDpy, uint8_t symb, uint32_t palet) {
  uint32_t width = 0;
  uint32_t index = (symb - 0x10) * 10;
//  if (!index) width = 4;		// ширина пробела
  if (symb == ' ') width = 4;		// ширина пробела
  for (uint32_t i = 0; i < 10; i++) {
    uint32_t row = Font8[index + i];
    for (uint32_t j = 0; j < 10; j++) {
      uint32_t temp = (row & 0x0000c000) >> 14;	// два бита
      *(pDpy + j) = (uint8_t)(palet >> (8 * temp));
      if (temp && (j > width)) width = j;
      row <<= 2;
    }
    pDpy += 320;
  }
  return width + 2;			// ширина с учетом зазора между символами
}

/*!*****************************************************************************
 @brief		Special Symbol draw
 @details	Нарисовать специальный символ
 @param		pDpy - указатель на вехний левый край символа в памяти LCD
 @param		symb - special symbol
 @param		mask - маска изменяемых битов
 @param		palet - начало палитры цвета для rgb, rgB, RGb, RGB
 @return	Ширина символа в пикселах (для пропорционального вывода шрифта)
 @note		Переписываются 10х8 пикселов (фон - только на краях символа)
 */
void DpySpec_draw(volatile uint8_t *pDpy, SSYM_t symb, uint8_t mask, uint32_t palet) {
  uint16_t *pSpec = (uint16_t *) &Spec8[symb][0];
  for (uint32_t i = 0; i < 10; i++) {
    uint32_t row = *pSpec++;
    for (uint32_t j = 0; j < 8; j++) {
      uint32_t temp = (row & 0x0000c000) >> 14;	// два бита
      if (temp) {
        uint8_t color = palet >> (8 * temp);	// цвет пиксела
	uint8_t pixel = *(pDpy + j);
	pixel ^= (pixel ^ color) & mask;
	*(pDpy + j) = pixel;
      }
      row <<= 2;
    }
    pDpy += 320;
  }
}

/*! @} */
