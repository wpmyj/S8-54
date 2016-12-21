
/*!*****************************************************************************
 @file		Siluet_main.h
 @author	Козлячков В.К.
 @date		2012.09
 @version	V0.5
 @brief		Siluet CPU main header
 @details	Определения, используемые в проекте
 @note		STM32F207ZC, 120 MHz, 256K Flash, 112K + 16K RAM, 4K Backup 
 @note		1 cycle = 8.33 ns, 3 cycles = 25 ns
 @note		ExtMem 256Kx16 (512KB)
 *******************************************************************************/

#ifndef MAIN_H				// включить файл только один раз
#define MAIN_H

/*__ Глобальные определения констант __________________________________________*/

/* Тип микроконтроллера (можно задать в свойствах проекта) */
// #define STM32F2XX

/* Резонатор 8 MHz */
// #define HSE_VALUE ((uint32_t)8000000)

// Версия программы, для индикации
#define MANUFACT "МНИПИ"
#define VERSION "1.0"


/* Распределение адресного пространства для внешних устройств */

/* External RAM - IS62WV25616BLL-55T 256K x 16 Ultra Low Power CMOS SRAM */
#define XRAM_BASE	0x64000000UL	//!< стартовый адрес внешнего ОЗУ
#define XRAM_SIZE	0x80000UL	//!< размер внешнего ОЗУ (в байтах)

/* FPGA - Адреса ПЛИС (16-битовая шина, A25:A1 сдвинуты в AB24:AB0)
 16 16-битовых регистров, мультиплексированная шина адрес / данные */
#define	PL_BASE		((uint32_t)0x68000000)	//!< FPGA base address

/* LCD Controller - S1D13706 */
#define DPYCTRL_BASE	((uint32_t)0x6C000000)	//!< Dysplay base address

/*! Буфер ЖКИ во внешней памяти */
#define	DPYBUF_BASE	(XRAM_BASE + XRAM_SIZE - DPYMEM_SIZE)
// #define DPYMEM	DPYMEM_BASE	// использовать внутреннюю память ЖКИ
// #define DPYMEM	DPYBUF		// использовать буфер ЖКИ во внешней памяти


/* Определения для условной компиляции */

// #define LED_TEST
// #define BLINK_TEST
// #define DISPLAY_TEST
// #define SIGNAL_TEST
// #define MARKS_DRAW
// #define SWEEP_TEST		// Тест формирования развертки
// #define BKG_GRAY		// Серый фон для отладки
#define TRIGDIV_USE		// Использовать делитель синхронизации
#define INVPOS_USE		// Использовать инверсию позиции
// #define SICUBE		// Кубическая интерполяция
#define SILINE			// Линейная интерполяция
#define STRA_VECT		// Прямые вектора
#define MATH_USE		// Математика (Сумма, Произведение, БПФ)
// #define USE_USB_INTERFACE
// #define USE_SCPI


/*__ Заголовочные файлы компилятора ___________________________________________*/

#include "stm32f2xx.h"		// определения регистров и битов
#include <stdbool.h>		// bool, true, false
#include <math.h>		// sin, cos
#include <stdlib.h>		// rand
// #include <stdio.h>		// printf
#include "cmsis_os.h"		// CMSIS-RTOS RTX

#ifdef USE_USB_INTERFACE
  #include "usb_add_module.h"	// USB
#endif


/*__ Глобальные типы данных ___________________________________________________*/

/*!*****************************************************************************
 @brief		Setup variable declaration macros
 @note		Макро, чтобы переменные определялись и инициализировались лишь однажды
		В одном файле (xx_main.c) определяется VAR_DECLS перед использованием xx_main.h
		В остальных просто используется xx_main.h, поэтому переменные - extern
 */
#ifndef VAR_DECLS
#define _DECL extern
#define _INIT(x)
#else
#define _DECL
#define _INIT(x) = x
#endif

/*__ Глобальные макрофункции __________________________________________________*/

/*!*****************************************************************************
 @brief		Define bits through bit-banding
 @details	определение битов периферийных устройств через bit-band alias
 @param		VarPtr - Указатель на элемент структуры периферийного устройства
 @param		BitNum - Номер бита в элементе структуры
 @note		Использовать ALIAS_PTR для периферийных устройств микроконтроллера
 */
#define ALIAS(VarAddr, BitNum) \
  *((volatile uint32_t *) ((VarAddr & 0xF0000000) | 0x02000000 \
  + ((VarAddr & 0xFFFFF) <<5) | (BitNum <<2)))

#define ALIAS_PTR(VarPtr, BitNum) \
  *((volatile uint32_t *) (((uint32_t) &(VarPtr) & 0xF0000000) | 0x02000000 \
  + (((uint32_t) &(VarPtr) & 0xFFFFF) <<5) | (BitNum <<2)))


/*! Инкрементировать переменную в пределах 0...MAX */
#define INC_MAX(Var, MAX)  Var = (Var == MAX)? 0 : Var + 1

/*! Декрементировать переменную в пределах 0...MAX */
#define DEC_MAX(Var, MAX)  Var = (Var == 0)? MAX : Var - 1 


/*__ Глобальные переменные ____________________________________________________*/

// DECL uint32_t Temporal;		// Временная переменная для чтения и тестов

/// DECL volatile uint32_t Temporal INIT(0);
// _DECL volatile uint32_t Test _INIT(0);
/* Таймер задержки в 1 ms интервалах, декрементируется по прерыванию SysTick */
/// _DECL volatile uint32_t TimerTicks _INIT(0);
/// _DECL bool PL_Ready _INIT(false);		// ПЛИС сконфигурирована
// DECL uint16 * TestWord INIT(EXTRAM_START);	// тестовое слово


/// extern osThreadId	Main_id;	//!< указатель на задачу main
/// extern osMutexId	MutDpy_id;	//!< Указатель на мьютекс

/*__ Заголовочные файлы проекта _______________________________________________*/

#include "s1d13706.h"
#include "Siluet_Control.h"
#include "Siluet_Display.h"
#include "Siluet_Interface.h"
#include "Siluet_Signal.h"
#ifdef USE_SCPI
  #include "SCPI_include.h"
#endif

#endif
