
/*!*****************************************************************************
 @file		Siluet_Signal.c
 @author	Козлячков В.К.
 @date		2013.03
 @version	V1.0
 @brief		Signal Processing functions
 @details	Функции обработки и индикации сигнала
 @note		
 *******************************************************************************
 @verbatim
 2013.09.11
  Добавил функции умножения, вектор рисуется тот же, что и для суммы 
 
 2013.04.22
   Инверсию сигнала делаю в двух вариантах, определяемых INVPOS_USE.
   Когда определено INVPOS_USE, программно инвертируется и сигнал,
   и маркер позиции. Все переворачивается снизу вверх.
   Когда INVPOS_USE не определено, инвертируется только сигнал, и
   привязывается к той же позиции, что была. Позиция на экране не меняется.
   Но если сигнал ограничивается АЦП, то это ограничение становится видным
   в середине экрана!
   Инверсия делается только при выводе на экран.
   При суммировании-произведении нужно инвертировать отдельно.
 
 2013.04.19
   TimPos теперь указывает на начало экрана в памяти сигнала.
 
 2012.04.12
   Для начальной установки функций обработки сигнала нужно создать отдельную
   функцию. Она задаст в зависимости от развертки указатель функции обработки, 
   установит указатель точки в промежуточном буфере и индексы начала и конца 
   в рабочем буфере сигнала.
   TimePos соответствует началу экрана (по умолчанию предзапуск находится
   в середине экрана с МЕНЮ - 5 делений).
   Сигнал нужно рисовать между SigBeg и SigEnd.
   
	SigBuf[][]	0(TimePos)	125			Dpy_width
	[		<		!		>	>	]
			   xxxxxxxxxxxxxxxxxxxxx_
			   ^			^
			   SigBeg		SigEnd (don't draw)

 2012.04
   Начинаю программы работы по точкам
   Убрал кубическую интерполяцию при выводе (рисуются выбросы).
   Уменьшил логарифм усреднения на 1, чтобы новый режим начинался без усреднения.   

 2011.02
   Переделал функции для работы с буфером ЖКИ во внешнем ОЗУ
   Заменил LCDMEM на EXTDPY
 @endverbatim
 *******************************************************************************/

/*! @addtogroup	Signal
    @{ */

/*__ Заголовочные файлы _______________________________________________________*/

#include "Siluet_Main.h"	// все включения заданы в одном файле
#include "FFT_Windows.h"	// таблица оконных функций

/*__ Определения констант _____________________________________________________*/

/* Границы рабочей области экрана */
#define DPYWAMIN ((uint8_t *)(DPYBUF_BASE + 320 * DPYWAV + DPYWAH))
#define DPYWAMAX ((uint8_t *)(DPYBUF_BASE + 320 * (DPYWAV + 200) + DPYWAH + 300))

/* Коэффициенты ослабления оконных функций */
#define WATT_RECT	1
#define WATT_HANN	0.5
#define WATT_HAMM	0.53888984
#define WATT_BART	0.5
// #define WATT_FLAT	0.21570319
#define WATT_FLAT	0.254

/*__ Макрофункции _____________________________________________________________*/


/*__ Локальные типы данных ____________________________________________________*/


/*__ Локальные переменные _____________________________________________________*/

/*! Промежуточный буфер для хранения точек сигнала */
uint16_t DotBuf[16384];		// Chan2<<8 | Chan1
uint16_t *pDotBuf = DotBuf;	//!< Указатель на записанную точку в буфере


/*__ Локальные прототипы функций ______________________________________________*/

void SigDots_rdavr(void);

void SigChan1Dot_draw(uint32_t bright);
void SigChan2Dot_draw(uint32_t bright);
void SigChan1Vect_draw(uint32_t bright);
void Vect1_draw(int32_t s1, int32_t brk, int32_t s2, 
		uint32_t dpos, uint32_t bright);
void SigChan2Vect_draw(uint32_t bright);
void Vect2_draw(int32_t s1, int32_t brk, int32_t s2, 
		uint32_t dpos, uint32_t bright);

void SigChans_clear(void);
void Sweep_start(void);

void Sum_draw(bool bright);
void SumDots_draw(bool bright);
void SumVect_draw(bool bright);
void Mult_draw(bool bright);
void MultDots_draw(bool bright);
void MultVect_draw(bool bright);
void VectMath_draw(int32_t s1, int32_t brk, int32_t s2,
		   uint32_t dpos, bool bright);
		
void FFT_calc(void);
void FFT_draw(bool bright);

void NumWrong_conv(uint8_t *pBuf);
void Num2Volt_conv(int32_t Num, uint32_t Volts, uint8_t *pBuf);
void Num2Time_conv(int32_t Num, uint8_t *pBuf);
void Num2Freq_conv(int32_t Num, uint8_t *pBuf);

void XYDots_draw(bool bright);

/*__ Глобальные функции _______________________________________________________*/


/*!*****************************************************************************
 @brief		Sweep initialize
 @details	Инициализировать указатель функции обработки сигнала
 @param		void
 @return	
 @note		Sweep.TimeNum:	0..TIME_DOT(6)	- Dots,
				 ..26		- Full Frame,
				 ..TIMECNT(36)	- Sparse Frame 
 */
void Sweep_init(void) {

/* Dots */
  if (SweepCtrl.TimeNum <= TIME_DOT) {
  
/* Dots, ROLL */  
    if (SweepCtrl.Roll) {
      SigCtrl.SigNum = ROLL;
      SigCtrl.SigInx = 0;
      SigCtrl.SigBeg = SigCtrl.SigEnd = (1024 << MemCtrl.SizeNum) - 1;
    }
    
/* Dots, SCAN */    
    else {
      SigCtrl.SigNum = SCAN_PRE;
      pDotBuf = DotBuf;		// Указатель точки - в начало буфера      
    }
  }
  
/* Frame, Full FRAME */  
  else if (SweepCtrl.TimeNum < TIME_SPR) {
    SigCtrl.SigNum = FRAME_FULL;
    SigCtrl.SigBeg = 0;
    SigCtrl.SigEnd = (1024 << MemCtrl.SizeNum) - 1;	// Конец буфера + 1
    // SigCtrl.SigBeg = 25;
    // SigCtrl.SigEnd = (1024 << MemCtrl.SizeNum) - 25;    
  }
  
/* Frame, SPARSE Frame */  
  else {
    SigCtrl.SigNum = FRAME_SPARSE;
    SigChans_clear();
    SigCtrl.SigBeg = 0;
    SigCtrl.SigEnd = (1024 << MemCtrl.SizeNum) - 1;	// Конец буфера
  }
  if (!SweepCtrl.Stop) PL->STAT = PL_START_ACQUST;	// Start new frame
}

/*!*****************************************************************************
 @brief		Sweep execute
 @details	Функция обработки сигнала с индексом
 @param		void
 @return	
 @note		
 */
void Sweep_exe(void) {
#ifdef SWEEP_TEST
  LED_ON();
#endif
  uint_fast16_t temp_stat = PL->STAT;	// read (reset interrupt request)
  // Temporal = temp_stat;		// Для просмотра в отладчике

/* Корректировать границы распределения trig - second sample */
  if (SweepCtrl.TimeNum > TIME_DOT && temp_stat & PL_STAT_TRIGDET) {

    // Interp.Tcur = PL->ITRP;
    while (!(ADC2->SR & ADC_SR_JEOC));	// Цикл, пока нет конца преобразования
    ADC2->SR &= ~ADC_SR_JEOC;		// Сбросить бит конца преобразования
    Interp.Tcur = ADC2->JDR1;

    PL->STAT = PL_START_ITDCST;		// Начать разрядку интерполятора
    // Num_draw(Interp.Tcur);
    // Num_draw(Interp.Tmts >> 16);
    // Num_draw(Interp.Tmad >> 16);
    
    // if (Interp.Tcur > Interp.Tmin - 32 && Interp.Tcur < Interp.Tmax + 32)
    // if (Interp.Tcur > Interp.Tmin - 32)
    if (Interp.Tcur > Interp.Tmin - 256 && Interp.Tcur < Interp.Tmax + 256)
    //  if (Interp.Tcur > 32 && Interp.Tcur < 4096 - 32)
    {
      if (Interp.Awgt < MAXWGT) Interp.Awgt++;	// Увеличить вес усреднения
      Interp.Tmts = Interp.Tmts - Interp.Tmts / Interp.Awgt
		  + (Interp.Tcur << 16) / Interp.Awgt;
      int32_t dev = Interp.Tmts - (Interp.Tcur << 16);
      Interp.Tmad = Interp.Tmad - Interp.Tmad / Interp.Awgt
	  	  + ((dev >= 0) ? dev : -dev) / Interp.Awgt;
      Interp.Tmin = (Interp.Tmts - Interp.Tmad * 2) >> 16;
      Interp.Tmax = (Interp.Tmts + Interp.Tmad * 2) >> 16;    
      if (ServCtrl.Calibr) {
        // Num_draw(Interp.Tcur);
        Num_draw(Interp.Tmts >> 16);
        // Num_draw(Interp.Tmad >> 16);
        // Num_draw(Interp.Tmin);
        // Num_draw(Interp.Tmax);
      }
    }
  }

/* Очистить изображение прошлого кадра
   из-за очистки редко выскакивал черный экран на быстрых развертках */
  ///  if (Chan1Ctrl.On) SigChan1_draw(0);	// clear old wave
  ///  if (Chan2Ctrl.On) SigChan2_draw(0);	// clear old wave
  //  if (SweepCtrl.XY) XYDots_draw(false);	// clear old XY
  static uint16_t *pSigChan1;		// указатель в буфере сигнала 1
  static uint16_t *pSigChan2;		// указатель в буфере сигнала 2

/* ROLL */
  if (SigCtrl.SigNum == ROLL) {		
    uint_fast16_t temp_dot = PL->RINT;	// read Dot from Both Channels
    // Temporal = temp_dot;		// Для просмотра в отладчике
    if (Chan1Ctrl.On) {
// Вычислить указатель и записать точку в буфер сигнала 1
      pSigChan1 = SigChan1.Buf16K + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
      pSigChan1 += SigCtrl.SigInx;
      *pSigChan1 = temp_dot << 8;	// Chan1 (int part)
    }
    if (Chan2Ctrl.On) {
// Вычислить указатель и записать точку в буфер сигнала 2
      pSigChan2 = SigChan2.Buf16K + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
      pSigChan2 += SigCtrl.SigInx;
      *pSigChan2 = temp_dot & 0xFF00;	// Chan2 (int part)
    }
// Увеличить индекс; если он стал больше размера буфера, обнулить
    SigCtrl.SigInx++;
    if (SigCtrl.SigInx >= (1024 << MemCtrl.SizeNum)) SigCtrl.SigInx = 0;
// Приблизить индекс начала сигнала к началу буфера    
    if (SigCtrl.SigBeg) SigCtrl.SigBeg--;
  }

/* SCAN_PRE */
  else if (SigCtrl.SigNum == SCAN_PRE) {
    *pDotBuf++ = PL->RINT;		// Read-write dot to DotBuf
    if (pDotBuf == DotBuf + 16384) pDotBuf = DotBuf;  
    if (temp_stat & PL_STAT_TRIGDN) {	// Triggered
      pDotBuf -= SweepCtrl.Pret << MemCtrl.SizeNum + 5;
      if (pDotBuf < DotBuf) pDotBuf += 16384;	// начало читаемых точек
// Читать-усреднить часть кадра до предзапуска      
      SigDots_rdavr();
      // SigCtrl.SigInx = Sweep.Pret;	// Индекс для расчета длины кадра
// Смещение в буфере 16-битовых выборок сигнала (в выборках, не в байтах!)
      pSigChan1 = SigChan1.Buf16K + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
      pSigChan1 += SweepCtrl.Pret << MemCtrl.SizeNum + 5;
      pSigChan2 = SigChan2.Buf16K + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
      pSigChan2 += SweepCtrl.Pret << MemCtrl.SizeNum + 5;
      SigCtrl.SigNum = SCAN_PST;	// дальше работать по точкам
    }
  }
  
/* SCAN_PST */  
  else if (SigCtrl.SigNum == SCAN_PST) {
    uint32_t cnt = FrameCnt;		// счетчик кадров с неизменным режимом
    uint32_t clog = 0;			// логарифм счетчика обработанных кадров
    while (cnt >>= 1) clog++;
    clog--;				// для 1-го кадра нет усреднения (>>0)
    uint32_t aver = AcquCtrl.Aver >> 2;	// вес усреднения
    if (clog < aver) aver = clog;
// Записать точку во включенные каналы
    uint32_t temp_mem;
    uint32_t temp_dot = PL->RINT;	// прочитать два канала (00002211)
    if (Chan1Ctrl.On) {
      temp_mem = *pSigChan1 << 16;	// загрузить в старшие разряды (iiff0000)
      temp_mem -= temp_mem >> aver;	// уменьшить на величину усреднения (0iiff000)
      temp_mem += ((temp_dot & 0x00ff) << 24) >> aver;	// добавить часть канала
      *pSigChan1++ = temp_mem >> 16;	// сохранить старшие разряды
    }
    if (Chan2Ctrl.On) {
      temp_mem = *pSigChan2 << 16;	// загрузить в старшие разряды (iiff0000)
      temp_mem -= temp_mem >> aver;	// уменьшить на величину усреднения (0iiff000)
      temp_mem += ((temp_dot & 0xff00) << 16) >> aver;	// добавить часть канала
      *pSigChan2++ = temp_mem >> 16;	// сохранить старшие разряды
    }
    if (temp_stat & PL_STAT_POSTDN) {
      // if (!SweepCtrl.Stop) PL->STAT = PL_START_ACQUST;	// Start new frame
      Sweep_start();
      // pDotBuf = DotBuf;			// Указатель точки - в начало буфера
      SigCtrl.SigNum = SCAN_PRE;
    }     
  }  

/* FRAME_FULL */  
  else if (SigCtrl.SigNum == FRAME_FULL) {
// Прочитать и усреднить оба, один или ни одного из каналов      
    // SigChans_read();
// Если измеренное время не укладывается между границами, выйти
    // if (Interp.Tcur >= Interp.Tmin || Interp.Tcur <= Interp.Tmax)

    SigChans_rdavr();			// Read-average channels
    
    FrameCnt++;				// Увеличить счетчик кадров с неизменным режимом
    if (!FrameCnt) FrameCnt--;		// если было переполнение, остаться на максимуме
    uint32_t cnt = FrameCnt;		// счетчик кадров с неизменным режимом
    uint32_t clog = 0;			// логарифм счетчика обработанных кадров
    while (cnt >>= 1) clog++;
    clog--;				// для 1-го кадра нет усреднения (>>0)
    uint32_t aver = AcquCtrl.Aver >> 2;	// вес усреднения
    if (clog < aver) aver = clog;

/* Смещение в буфере 16-битовых выборок сигнала (в выборках, не в байтах!) */  
    uint16_t *pSigChan1 = SigChan1.Buf16K
			+ (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
    uint16_t *pSigChan2 = SigChan2.Buf16K
			+ (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;

// Корректировать указатель по результату измерения
/*  
  if (Interp.Tcur > Interp.Tmts >> 16) {
    DpyCharPad_draw(DPYPOS(DPYWAH + 300 - 2 * 8 - 2, 1), '>', 0xf3f30000);
    pSigChan1 += 1; 
    pSigChan2 += 1; 
  }
  else DpyCharPad_draw(DPYPOS(DPYWAH + 300 - 2 * 8 - 2, 1), '<', 0xf3f30000);
  // Num_draw(Interp.Tcur);
 */

    uint32_t temp_pld;
    uint32_t temp_mem;
    
/* Переписать включенные каналы */
    for (uint32_t i = 1024 << MemCtrl.SizeNum; i--; ) {
      temp_pld = (uint32_t)PL->MINT;	// прочитать два канала (00002211)
      if (Chan1Ctrl.On) {
        temp_mem = *pSigChan1 << 16;	// загрузить в старшие разряды (iiff0000)
        temp_mem -= temp_mem >> aver;	// уменьшить на величину усреднения (0iiff000)
        temp_mem += ((temp_pld & 0x00FF) << 24) >> aver;	// добавить часть канала
        *pSigChan1++ = temp_mem >> 16;	// сохранить старшие разряды
      }
      if (Chan2Ctrl.On) {
        temp_mem = *pSigChan2 << 16;	// загрузить в старшие разряды (iiff0000)
        temp_mem -= temp_mem >> aver;	// уменьшить на величину усреднения (0iiff000)
        temp_mem += ((temp_pld & 0xFF00) << 16) >> aver;	// добавить часть канала
        *pSigChan2++ = temp_mem >> 16;	// сохранить старшие разряды
      }
    }
    // if (!SweepCtrl.Stop) PL->STAT = PL_START_ACQUST;	// Start new frame
    Sweep_start();
  }
  
/* FRAME_SPARSE
   Весь кадр переписывается из PL в DotBuf.
   Из DotBuf выбирается часть кадра от Sweep.Pret - Sweep.Pret / Sweep.TimeItp
   и переписывается с усреднением в SigChan1, SigChan2 
   со смещением Tofs = TimeItp * (Tcur - Tmin) / (Tmax - Tmin + 1)
   с шагом Sweep.TimeItp, пока не заполнится буфер.
   
   Границы распределения Tmin...Tmax вычисляются по формуле
   Tmin,Tmax = Tmts ± 2 * Tmad, где: 
   Tmts - среднее значение времени между синхроимпульсом и
	  вторым тактом дискретизации
   Tmad - среднее абсолютное отклонение от среднего времени 
	  (половина от максимального)
   Обе эти величины накапливаются с усреднением, когда есть синхронизация.
   
   Если синхронизации нет, сдвиг Tofs задается принудительно, последовательно 
   из диапазона 0...TimeItp.
   AD7910 дает 10-битовый результат измерений,
   самый большой TimeItp = 200 (0.5 ns/div).
   АЦП в STM32 дает 12-разрядный результат.
 */
  else {
// Если измеренное время не укладывается между границами, выйти
    if (Interp.Tcur < Interp.Tmin || Interp.Tcur > Interp.Tmax) {
      // if (!SweepCtrl.Stop) PL->STAT = PL_START_ACQUST;	// Start new frame
      Sweep_start();
      return;
    }
    
// Переписать весь кадр из ПЛИС в промежуточный буфер  
    pDotBuf = DotBuf;
    for (uint32_t i = 1024 << MemCtrl.SizeNum; i--; ) {
      *pDotBuf++ = PL->MINT;
    }
    
// Установить указатель в промежуточном буфере
    // pDotBuf = DotBuf + SweepCtrl.Pret - SweepCtrl.Pret / SweepCtrl.TimeItp;
    // pDotBuf = DotBuf
    //    + ((SweepCtrl.Pret - SweepCtrl.Pret / SweepCtrl.TimeItp)
    //        << MemCtrl.SizeNum);
    pDotBuf = DotBuf
	    + (SweepCtrl.Pret << MemCtrl.SizeNum + 5)
	    - (SweepCtrl.Pret << MemCtrl.SizeNum + 5) / SweepCtrl.TimeItp;    
    
// Вычислить смещение в буфере сигналов
    uint32_t static Tofs;
    if (temp_stat & PL_STAT_TRIGDET) {
    /*
      Tofs = SweepCtrl.TimeItp * 2 * (Interp.Tcur - Interp.Tmin)
           / (uint32_t)(Interp.Tmax - Interp.Tmin + 1);
      */
    //  Tofs = SweepCtrl.TimeItp * (Interp.Tcur - Interp.Tmin)
    //       / (uint32_t)(Interp.Tmax - Interp.Tmin + 1);
      Tofs = SweepCtrl.TimeItp * (Interp.Tcur - Interp.Tmin)
           / (Interp.Tmad * 4 >> 16);
	   
    }
// Если нет синхронизации, изменять Tofs последовательно
    else {
      Tofs++;
      if (Tofs >= SweepCtrl.TimeItp) Tofs = 0;
    }
  
// Задать указатели в буфере сигналов  
    pSigChan1 = SigChan1.Buf16K + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum
	      + Tofs + 0;
    pSigChan2 = SigChan2.Buf16K + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum
	      + Tofs + 0;
/*
    if (Interp.Tcur > Interp.Tmts >> 16) {
      DpyCharPad_draw(DPYPOS(DPYWAH + 300 - 2 * 8 - 2, 1), '>', 0xf3f30000);
      pSigChan1 += 2; 
      pSigChan2 += 2; 
    }
    else DpyCharPad_draw(DPYPOS(DPYWAH + 300 - 2 * 8 - 2, 1), '<', 0xf3f30000);
 */   
// Определить вес усреднения
    FrameCnt++;				// Увеличить счетчик кадров с неизменным режимом
    if (!FrameCnt) FrameCnt--;		// если было переполнение, остаться на максимуме
    uint32_t cnt = FrameCnt;		// счетчик кадров с неизменным режимом
    uint32_t clog = 0;			// логарифм счетчика обработанных кадров
    while (cnt >>= 1) clog++;
    clog--;				// для 1-го кадра нет усреднения (>>0)
    uint32_t aver = AcquCtrl.Aver >> 2;	// вес усреднения
    if (clog < aver) aver = clog;

    uint32_t temp_dot;
    uint32_t temp_mem;
// Переписать с усреднением разреженные выборки, пока не превысится буфер сигнала
    do {
      temp_dot = (uint32_t)*pDotBuf++;	// прочитать два канала (00002211)
      if (Chan1Ctrl.On) {
        temp_mem = *pSigChan1 << 16;	// загрузить в старшие разряды (iiff0000)
        temp_mem -= temp_mem >> aver;	// уменьшить на величину усреднения (0iiff000)
        temp_mem += ((temp_dot & 0x00ff) << 24) >> aver;	// добавить часть канала
        *pSigChan1 = temp_mem >> 16;	// сохранить старшие разряды
      }
      pSigChan1 += SweepCtrl.TimeItp;
      if (Chan2Ctrl.On) {
        temp_mem = *pSigChan2 << 16;	// загрузить в старшие разряды (iiff0000)
        temp_mem -= temp_mem >> aver;	// уменьшить на величину усреднения (0iiff000)
        temp_mem += ((temp_dot & 0xff00) << 16) >> aver;	// добавить часть канала
        *pSigChan2 = temp_mem >> 16;	// сохранить старшие разряды
      }
      pSigChan2 += SweepCtrl.TimeItp;
    } while ( (pSigChan1 <= SigChan1.Buf16K
		        + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1))
	   || (pSigChan2 <= SigChan2.Buf16K
		        + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1)) );
    // if (!SweepCtrl.Stop) PL->STAT = PL_START_ACQUST;	// Start new frame
    Sweep_start();
  }

/* Нарисовать часть кадра, точку или целый кадр */
//    if (Chan1Ctrl.On) {
//      Chan1Ctrl.DispPos = 0;		// clear display position
//      SigChan1_draw(3);			// draw new wave
//    }
//    if (Chan2Ctrl.On) {
//      Chan2Ctrl.DispPos = 0;		// clear display position
//      SigChan2_draw(3);			// draw new wave
//    }

  if (Chan1Ctrl.On) Chan1Ctrl.DispPos = 0;
  if (Chan2Ctrl.On) Chan2Ctrl.DispPos = 0;
  DpyWork_clear(MASK_MATSIG);
  SigChans_draw();
  // XYDots_draw(true);
    
#ifdef SWEEP_TEST
  LED_OFF();
#endif

}



/*!*****************************************************************************
 @brief		Signal Channels draw
 @details	Нарисовать сигналы (во времени или XY)
 @return	
 */
void SigChans_draw(void) {
  if (!SweepCtrl.XY) {
    if (Chan1Ctrl.On) SigChan1_draw(3);
    if (Chan2Ctrl.On) SigChan2_draw(3);
    Math_exe();
  }
  else XYDots_draw(true);
}

/*!*****************************************************************************
 @brief		Signal Channels redraw after close MENU
 @details	Перерисовать сигналы после закрытия МЕНЮ
 @return	
 */
void SigChans_redraw(void) {
  if (!SweepCtrl.XY) {
    if (Chan1Ctrl.On) SigChan1_draw(3);
    if (Chan2Ctrl.On) SigChan2_draw(3);
    Math_exe();
  }
  else XYDots_draw(true);
}

/*!*****************************************************************************
 @brief		Position Channels redraw
 @details	
 @return	
 @note	
 */
void PosChans_redraw(void) {
  if (Chan1Ctrl.On) PosChan1_draw(true);
  if (Chan2Ctrl.On) PosChan2_draw(true);
}

/*!*****************************************************************************
 @brief		Signal Channel1 draw in ExtRAM
 @details	
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note	
 */
void SigChan1_draw(uint32_t bright) {
  // if (!DispCtrl.Vect || (SigCtrl.SigNum == FRAME_SPARSE && FrameCnt < 1000))
  if (!DispCtrl.Vect)
       SigChan1Dot_draw(bright);
  else SigChan1Vect_draw(bright);
}

/*!*****************************************************************************
 @brief		Signal Channel2 draw
 @details	
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note		
 */
void SigChan2_draw(uint32_t bright) {
  // if (!DispCtrl.Vect  || (SigCtrl.SigNum == FRAME_SPARSE && FrameCnt < 1000))
  if (!DispCtrl.Vect)
       SigChan2Dot_draw(bright);
  else SigChan2Vect_draw(bright);
}

/*!*****************************************************************************
 @brief		Signal Channel1 Dots draw
 @details	Нарисовать точки сигнала, перемещаясь по индексу в буфере
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note		lmarg, rmarg - границы рисуемого сигнала
 */
void SigChan1Dot_draw(uint32_t bright) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  int32_t lmarg = (SigCtrl.SigBeg > SweepCtrl.TimePos) ?
                   SigCtrl.SigBeg - SweepCtrl.TimePos : 0;
  int32_t rmarg = (SigCtrl.SigEnd < SweepCtrl.TimePos + width) ?
                   SigCtrl.SigEnd - SweepCtrl.TimePos : width;
  if (lmarg >= width || rmarg < 0) return;
  uint16_t *pSig = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum 
		 + SweepCtrl.TimePos 
		 + lmarg + SigCtrl.SigInx;
  uint16_t *pLim = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
  if (pSig >= pLim) pSig -= (1024 << MemCtrl.SizeNum);
  volatile uint8_t *pDpy;
  for (uint32_t i = lmarg; i < rmarg; i++) {
    int16_t temp = (*pSig++) >> 8;	// старшая (целая) часть слова
    if (pSig >= pLim) pSig -= (1024 << MemCtrl.SizeNum);
#ifdef INVPOS_USE
    if (Chan1Ctrl.Inv) temp = 0x100 - temp;
#else
    if (Chan1Ctrl.Inv) temp = (Chan1Ctrl.SigPos) - temp - 256 - Chan1Ctrl.DispPos;
#endif
    temp = (Chan1Ctrl.DispPos / 2U + temp);
    pDpy = (uint8_t *)(DPYBUF_BASE + DPYWAH + i + 320 * (DPYWAV + 228 - temp));
    if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) { 
      uint8_t pix = *pDpy;  pix ^= (pix ^ bright) & 0x03;  *pDpy = pix;
    }
  }
}

/*!*****************************************************************************
 @brief		Signal Channel2 Dots draw
 @details	Нарисовать точки сигнала, перемещаясь по индексу в буфере
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note		lmarg, rmarg - границы рисуемого сигнала по экрану (0..width)
 */
void SigChan2Dot_draw(uint32_t bright) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  // uint32_t pos = Sweep.TimePos;
  int32_t lmarg = (SigCtrl.SigBeg > SweepCtrl.TimePos) ? 
                   SigCtrl.SigBeg - SweepCtrl.TimePos : 0;
  int32_t rmarg = (SigCtrl.SigEnd < SweepCtrl.TimePos + width) ? 
                   SigCtrl.SigEnd - SweepCtrl.TimePos : width;
  // Hex32_draw (lmarg);
  // if (lmarg >= rmarg) return;
  if (lmarg >= width || rmarg < 0) return;
  // Hex32_draw (lmarg);
  uint16_t *pSig = SigChan2.Buf16K  
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum
		 + SweepCtrl.TimePos
		 + lmarg + SigCtrl.SigInx;
//  uint16_t *pSig = SigChan2.Buf8K  
//                 + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum)
//		 + SigCtrl.SigInx + SigCtrl.SigBeg;
  uint16_t *pLim = SigChan2.Buf16K
                 + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
  if (pSig >= pLim) pSig -= (1024 << MemCtrl.SizeNum); 
  // Hex32_draw ((uint32_t)pSig);
  // Hex32_draw ((uint32_t)pLim);
  
  volatile uint8_t *pDpy; 

  for (uint32_t i = lmarg; i < rmarg; i++) {
    int32_t temp = (*pSig++) >> 8;	// старшая (целая) часть слова
    if (pSig >= pLim) pSig -= (1024 << MemCtrl.SizeNum);
#ifdef INVPOS_USE
    if (Chan2Ctrl.Inv) temp = 0x100 - temp;
#else
    if (Chan2Ctrl.Inv) temp = (Chan2Ctrl.SigPos) - temp - 256 - Chan2Ctrl.DispPos;
#endif
    temp = (Chan2Ctrl.DispPos / 2U + temp);
    // Temporal = temp;
    pDpy = (uint8_t *)(DPYBUF_BASE + DPYWAH + i + 320 * (DPYWAV + 228 - temp));
    if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
      uint8_t pix = *pDpy; pix ^= (pix ^ bright << 2) & 0x03 << 2; *pDpy = pix;
    }
  }
}

#if false
/*!*****************************************************************************
 @brief		Signal Channel1 Dots draw
 @details	
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note		lmarg, rmarg - границы рисуемого сигнала
 */
void SigChan1Dot_draw(uint32_t bright) {
  uint32_t width = DispCtrl.Wide? 300 : 250;
  volatile uint8_t *pDpy;
  uint32_t pos = Sweep.TimePos;
  uint32_t lmarg = (SigCtrl.SigBeg > pos) ? SigCtrl.SigBeg - pos : 0;
  uint32_t rmarg = (SigCtrl.SigEnd < pos + width) ? SigCtrl.SigEnd - pos : width;
  if (lmarg >= rmarg) return;
  uint16_t *pSig = SigChan1.Buf8K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum 
		 + pos + lmarg;
  for (int32_t i = lmarg; i < rmarg; i++) {
    int16_t temp = (*pSig++) >> 8;	// старшая (целая) часть слова
    temp = (Chan1.DispPos / 2U + temp);
    pDpy = (uint8_t *)(DPYMEM + DPYWAH + i + 320 * (DPYWAV + 228 - temp));
    if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) { 
      uint8_t pix = *pDpy;  pix ^= (pix ^ bright) & 0x03;  *pDpy = pix;
    }
  }
}

/*!*****************************************************************************
 @brief		Signal Channel2 Dots draw
 @details	
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note		
 */
void SigChan2Dot_draw(uint32_t bright) {
  uint32_t width = DispCtrl.Wide? 300:250;
  volatile uint8_t *pDpy;
  uint32_t pos = Sweep.TimePos;
  uint32_t lmarg = (SigCtrl.SigBeg > pos) ? SigCtrl.SigBeg - pos : 0;
  uint32_t rmarg = (SigCtrl.SigEnd < pos + width) ? SigCtrl.SigEnd - pos : width;
  if (lmarg >= rmarg) return;
  uint16_t *pSig = SigChan2.Buf8K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum 
		 + pos + lmarg;
  for (int32_t i = lmarg; i < rmarg; i++) {
    int16_t temp = (*pSig++) >> 8;	// старшая (целая) часть слова
    if (Chan2.Inv) temp = 0x100 - temp;
    temp = (Chan2.DispPos / 2U + temp);
    pDpy = (uint8_t *)(DPYMEM + DPYWAH + i + 320 * (DPYWAV + 228 - temp));
    if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) { 	
      uint8_t pix = *pDpy;  pix ^= (pix ^ bright << 2) & 0x03 << 2;  *pDpy = pix;
    }
  }
}
#endif

#ifdef SICUBE
/*!*****************************************************************************
 @brief		Signal Channel1 Vectors draw
 @details	Нарисовать сигнал канала 1 векторами с кубической интерполяцией
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note		
 */
void SigChan1Vect_draw(uint32_t bright) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  uint16_t *BufBeg = SigChan1.Buf8K + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
  uint16_t *pSig = BufBeg + Sweep.TimePos;
  int32_t s0, s1, s2, s3;			// samples
  int32_t brk;
/* Первый вектор изображения */
  if (pSig == BufBeg)  s0 = (*pSig) >> 8;	// s0/2
  else  s0 = (*(pSig - 1)) >> 8;
  s1 = (*pSig++) >> 8; s2 = (*pSig++) >> 8; s3 = (*pSig++) >> 8;
  brk = ((s1 + s2) * 9 - s0 - s3) / 16;
  // margin = (s1 + s2)>>1;
  // margin = ((s1 + s2) * 9U - s0 - s3) >> 4;
  Vect1_draw(s1, brk, s2, 0, bright);
/* Цикл между первым вектором и последним */
  for (uint32_t i = 1; i < width - 1; i++) {
    s0 = s1; s1 = s2; s2 = s3; s3 = (*pSig++) >> 8;
    brk = ((s1 + s2) * 9 - s0 - s3) / 16;
    // margin = (s1 + s2)>>1;
    // margin = ((s1 + s2) * 9U - s0 - s3) >> 4;  
    Vect1_draw(s1, brk, s2, i, bright);
  }
/* Последний вектор изображения */
  s0 = s1; s1 = s2; s2 = s3;
  if (pSig == BufBeg + (1024 << MemCtrl.SizeNum))  s3 = (*(pSig - 1)) >> 8;
  else  s3 = (*pSig) >> 8;
  brk = ((s1 + s2) * 9 - s0 - s3) / 16;
  // margin = (s1 + s2)>>1;
  // margin = ((s1 + s2) * 9U - s0 - s3) >> 4;
  Vect1_draw(s1, brk, s2, width-1, bright);      
}
#endif	// SICUBE

#ifdef SILINE
/*!*****************************************************************************
 @brief		Signal Channel1 Vectors draw
 @details	Нарисовать сигнал канала 1 векторами с линейной интерполяцией
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note		
 */
void SigChan1Vect_draw(uint32_t bright) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  // uint32_t pos = Sweep.TimePos;
  int32_t lmarg = (SigCtrl.SigBeg > SweepCtrl.TimePos) ?
                   SigCtrl.SigBeg - SweepCtrl.TimePos : 0;
  int32_t rmarg = (SigCtrl.SigEnd < SweepCtrl.TimePos + width) ?
                   SigCtrl.SigEnd - SweepCtrl.TimePos : width;
  if (lmarg >= width || rmarg < 0) return;
  uint16_t *pSig = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum
                 + SweepCtrl.TimePos
		 + lmarg + SigCtrl.SigInx;
  uint16_t *pLim = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
  if (pSig >= pLim) pSig -= (1024 << MemCtrl.SizeNum);		 
  int32_t s1, s2;			// previous & next samples
  int32_t brk;
/* Цикл между первым вектором и последним */
  for (int32_t i = lmarg; i < rmarg; i++) {
    s1 = (*pSig++) >> 8;		// integer part, begin
    if (pSig >= pLim) pSig -= (1024 << MemCtrl.SizeNum);
    s2 = (*pSig) >> 8;			// integer part, end
#ifdef INVPOS_USE
    if (Chan1Ctrl.Inv) {
      s1 = 0x100 - s1;
      s2 = 0x100 - s2;
    }
#else
    if (Chan1Ctrl.Inv) {
      s1 = (Chan1Ctrl.SigPos) - s1 - 256 - Chan1Ctrl.DispPos;
      s2 = (Chan1Ctrl.SigPos) - s2 - 256 - Chan1Ctrl.DispPos;
    }
#endif
#ifdef STRA_VECT
    brk = (s1 == s2) ? s1 : ((s1 > s2) ? (s1 - 1) : (s1 + 1));
#else    
    brk = (s1 + s2) >> 1;		// jump from left to right column
#endif
    Vect1_draw(s1, brk, s2, i, bright);
  }
}
#endif					// SILINE

/*!*****************************************************************************
 @brief		Vector1 draw
 @details	Нарисовать вектор (в двух соседних столбцах)
 @param		s1, brk, s2
 @param		dpos - позиция начала вектора по горизонтали
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note		
 */
void Vect1_draw(int32_t s1, int32_t brk, int32_t s2, uint32_t dpos, uint32_t bright)
{
  if (SigCtrl.SigNum == FRAME_SPARSE && (!(s1) || !(s2))) return;
  s1 += Chan1Ctrl.DispPos / 2;
  brk += Chan1Ctrl.DispPos / 2;
  s2 += Chan1Ctrl.DispPos / 2;
  uint32_t size;			// размер вектора
  uint8_t *pDpy = (uint8_t *)(DPYBUF_BASE + DPYWAH + dpos + 320 * (DPYWAV + 228 - s1));
/* от левой выборки до середины */
  if (s1 < brk) {			// up vector			
    size = brk - s1;
    for (uint32_t j=size; j--; ) {
      pDpy -= 320;			// перескочить - нарисовать
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy;  pix ^= (pix ^ bright) & 0x03;  *pDpy = pix;
      }
    }
  }
  else if (s1 > brk) {		// down vector
    size = s1 - brk;
    for (uint32_t j=size; j--; ) {
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy;  pix ^= (pix ^ bright) & 0x03;  *pDpy = pix;
      }
      pDpy += 320;			// нарисовать - перескочить
    }
  }
  else {				// s1 == brk, right vector
    if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
      uint8_t pix = *pDpy;  pix ^= (pix ^ bright) & 0x03;  *pDpy = pix;
    }
  }
/* сдвиг на следующий ряд */
  pDpy++;				
/* от середины до правой выборки */    
  if (brk < s2) {			// up vector  
    size = s2 - brk;
    for (uint32_t j=size; j--; ) {
      pDpy -= 320;			// перескочить - нарисовать    
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy;  pix ^= (pix ^ bright) & 0x03;  *pDpy = pix;
      }
    }
  }
  else if (brk > s2) {		// down vector
    size = brk - s2;
    for (uint32_t j=size; j--; ) {
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy;  pix ^= (pix ^ bright) & 0x03;  *pDpy = pix;
      }
      pDpy += 320;			// нарисовать - перескочить      
    }
  }
//  else {				// s1 == brk, right vector
//    if (pDpy >= (uint8_t *)(LCD_MEM_START+LCDWAH+320*LCDWAV) &&
//        pDpy <= (uint8_t *)(LCD_MEM_START+LCDWAH+320*(LCDWAV+200))) {
//      uint8_t pix = *pDpy;  pix ^= (pix ^ bright) & 0x03;  *pDpy = pix;
//    }
//  }
}

#ifdef SICUBE
/*!*****************************************************************************
 @brief		Signal Channel2 Vectors draw
 @details	Нарисовать сигнал канала 2 векторами с кубической интерполяцией
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note		
 */
void SigChan2Vect_draw(uint32_t bright) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  uint16_t *BufBeg = SigChan2.Buf8K + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
  uint16_t *pSig = BufBeg + Sweep.TimePos;
  int32_t s0, s1, s2, s3;			// samples
  int32_t brk;
/* Первый вектор изображения */
  if (pSig == BufBeg)  s0 = (*pSig) >> 8;	// s0/2
  else  s0 = (*(pSig - 1)) >> 8;
  s1 = (*pSig++) >> 8; s2 = (*pSig++) >> 8; s3 = (*pSig++) >> 8;
  if (Chan2.Inv) { s0 = 0x100 - s0;  s1 = 0x100 - s1;  s2 = 0x100 - s2;  s3 = 0x100 - s3; }
  brk = ((s1 + s2) * 9 - s0 - s3) / 16;
  // margin = (s1 + s2)>>1;
  // margin = ((s1 + s2) * 9U - s0 - s3) >> 4;
  Vect2_draw(s1, brk, s2, 0, bright);
/* Цикл между первым вектором и последним */
  for (uint32_t i = 1; i < width - 1; i++) {
    s0 = s1; s1 = s2; s2 = s3; s3 = (*pSig++) >> 8;
  if (Chan2.Inv)  s3 = 0x100 - s3;
    brk = ((s1 + s2) * 9 - s0 - s3) / 16;
    // margin = (s1 + s2)>>1;
    // margin = ((s1 + s2) * 9U - s0 - s3) >> 4;
    Vect2_draw(s1, brk, s2, i, bright);
  }
/* Последний вектор изображения */
  s0 = s1; s1 = s2; s2 = s3;
  if (pSig == BufBeg + (1024 << MemCtrl.SizeNum))  s3 = (*(pSig - 1)) >> 8;
  else  s3 = (*pSig) >> 8;
  if (Chan2.Inv)  s3 = 0x100 - s3;
  brk = ((s1 + s2) * 9 - s0 - s3) / 16;
  // margin = (s1 + s2)>>1;
  // margin = ((s1 + s2) * 9U - s0 - s3) >> 4;
  Vect2_draw(s1, brk, s2, width - 1, bright);      
}
#endif	// SICUBE

#ifdef SILINE
/*!*****************************************************************************
 @brief		Signal Channel2 Vectors draw
 @details	Нарисовать сигнал канала 2 векторами с линейной интерполяцией
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note	
 */
void SigChan2Vect_draw(uint32_t bright) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  // uint32_t pos = Sweep.TimePos;
  int32_t lmarg = (SigCtrl.SigBeg > SweepCtrl.TimePos) ?
                   SigCtrl.SigBeg - SweepCtrl.TimePos : 0;
  int32_t rmarg = (SigCtrl.SigEnd < SweepCtrl.TimePos + width) ?
                   SigCtrl.SigEnd - SweepCtrl.TimePos : width;
  if (lmarg >= width || rmarg < 0) return;
  uint16_t *pSig = SigChan2.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum 
		 + SweepCtrl.TimePos
		 + lmarg + SigCtrl.SigInx;
  uint16_t *pLim = SigChan2.Buf16K
                 + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
  if (pSig >= pLim) pSig -= (1024 << MemCtrl.SizeNum);		 
  int32_t s1, s2;			// samples
  int32_t brk;
/* Цикл между первым вектором и последним */
  for (uint32_t i = lmarg; i < rmarg - 1; i++) {
    s1 = (*pSig++) >> 8;
    if (pSig >= pLim) pSig -= (1024 << MemCtrl.SizeNum);
    s2 = (*pSig) >> 8;
#ifdef INVPOS_USE
    if (Chan2Ctrl.Inv) {
      s1 = 0x100 - s1;
      s2 = 0x100 - s2;
    }
#else
    if (Chan2Ctrl.Inv) {
      s1 = (Chan2Ctrl.SigPos) - s1 - 256 - Chan2Ctrl.DispPos;
      s2 = (Chan2Ctrl.SigPos) - s2 - 256 - Chan2Ctrl.DispPos;
    }
#endif
#ifdef STRA_VECT
    brk = (s1 == s2) ? s1 : ((s1 > s2) ? (s1 - 1) : (s1 + 1));
#else
    brk = (s1 + s2) >> 1;
#endif
    Vect2_draw(s1, brk, s2, i, bright);
  }
}
#endif	// SILINE

/*!*****************************************************************************
 @brief		Vector2
 @details	Нарисовать вектор (в двух соседних столбцах)
 @param		s1, brk, s2
 @param		dpos - позиция начала вектора по горизонтали
 @param		bright - яркость в диапазоне 3, 2, 1, 0 (выкл)
 @return	
 @note		
 */
void Vect2_draw(int32_t s1, int32_t brk, int32_t s2, uint32_t dpos, uint32_t bright)
{
  if (SigCtrl.SigNum == FRAME_SPARSE && (!(s1) || !(s2))) return;
  s1 += Chan2Ctrl.DispPos / 2;
  brk += Chan2Ctrl.DispPos / 2;
  s2 += Chan2Ctrl.DispPos / 2;
//  uint8_t thresh = (uint8_t)((thr_bri>>6)&0x0c);
//  uint8_t bright = (uint8_t)(thr_bri);
  uint32_t size;			// размер вектора
  uint8_t *pDpy = (uint8_t *)(DPYBUF_BASE + DPYWAH + dpos + 320 * (DPYWAV + 228 - s1));
/* от левой выборки до середины */
  if (s1 < brk) {			// up vector			
    size = brk - s1;
    for (uint32_t j=size; j--; ) {
      pDpy -= 320;			// перескочить - нарисовать
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy;  pix ^= (pix ^ bright << 2) & 0x03 << 2;  *pDpy = pix;
      }
    }
  }
  else if (s1 > brk) {		// down vector
    size = s1 - brk;
    for (uint32_t j=size; j--; ) {
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy;  pix ^= (pix ^ bright << 2) & 0x03 << 2;  *pDpy = pix;
      }
      pDpy += 320;			// нарисовать - перескочить
    }
  }
  else {				// s1 == brk, right vector
    if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
      uint8_t pix = *pDpy;  pix ^= (pix ^ bright << 2) & 0x03 << 2;  *pDpy = pix;
    }
  }
/* сдвиг на следующий ряд */
  pDpy++;				
/* от середины до правой выборки */    
  if (brk < s2) {			// up vector  
    size = s2 - brk;
    for (uint32_t j=size; j--; ) {
      pDpy -= 320;			// перескочить - нарисовать    
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy;  pix ^= (pix ^ bright << 2) & 0x03 << 2;  *pDpy = pix;
      }
    }
  }
  else if (brk > s2) {		// down vector
    size = brk - s2;
    for (uint32_t j=size; j--; ) {
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy;  pix ^= (pix ^ bright << 2) & 0x03 << 2;  *pDpy = pix;
      }
      pDpy += 320;			// нарисовать - перескочить      
    }
  }
//  else {				// s1 == marg, right vector
//    if (pDpy >= (uint8_t *)(LCD_MEM_START+LCDWAH+320*LCDWAV) &&
//        pDpy <= (uint8_t *)(LCD_MEM_START+LCDWAH+320*(LCDWAV+200))) {
//      uint8_t pix = *pDpy;  pix ^= (pix ^ bright<<2) & 0x03<<2;  *pDpy = pix;
//    }
//  }
}  

/*!*****************************************************************************
 @brief		Position1 Marker draw
 @details	Погасить или нарисовать маркер нулевой линии канала 1
 @param		On - Вкл/Выкл
 @return	
 @note		
 */
void PosChan1_draw(bool On) {
  int32_t pos = Chan1Ctrl.SigPos / 2U - 256;
#ifdef INVPOS_USE
  if (Chan1Ctrl.Inv) pos = -pos;
#endif
  if (!On) {
    if (pos < -100)
      DpySpec_draw(DPYPOS(DPYWAH - 7, DPYWAV + 199),
		   SS_MKDN, 0x03, 0x00000000);
    else if (pos >= -100 && pos <= 100)
      DpySpec_draw(DPYPOS(DPYWAH - 4, DPYWAV + 100 - pos - 4),
		   SS_MKLT, 0x03, 0x00000000);
    else	// pos > 100
      DpySpec_draw(DPYPOS(DPYWAH - 7, DPYWAV - 8),
		   SS_MKUP, 0x03, 0x00000000);
  }
  else {	// On
    if (pos < -100)
    DpySpec_draw(DPYPOS(DPYWAH - 7, DPYWAV + 199),
		 SS_MKDN, 0x03, 0x02020000);  
  else if (pos >= -100 && pos <= 100)
    DpySpec_draw(DPYPOS(DPYWAH - 4, DPYWAV + 100 - pos - 4),
		 SS_MKLT, 0x03, 0x02020000);
  else		// pos > 100
    DpySpec_draw(DPYPOS(DPYWAH - 7, DPYWAV - 8),
		 SS_MKUP, 0x03, 0x02020000);
  }
}

/*!*****************************************************************************
 @brief		Position2 Marker draw
 @details	Погасить или нарисовать маркер нулевой линии канала 2
 @param		On - Вкл/Выкл
 @return	
 @note		
 */
void PosChan2_draw(bool On) {
  int32_t pos = Chan2Ctrl.SigPos / 2U - 256;
#ifdef INVPOS_USE
  if (Chan2Ctrl.Inv) pos = -pos;
#endif
  if (!On) {
    if (pos < -100)
      DpySpec_draw(DPYPOS(DPYWAH - 7, DPYWAV + 199),
		   SS_MKDN, 0x0c, 0x00000000);  
    else if (pos >= -100 && pos <= 100)
      DpySpec_draw(DPYPOS(DPYWAH - 4, DPYWAV + 100 - pos - 4),
		   SS_MKLT, 0x0c, 0x00000000);
    else	// pos > 100
      DpySpec_draw(DPYPOS(DPYWAH - 7, DPYWAV - 8),
		   SS_MKUP, 0x0C, 0x00000000);
  }
  else {	// On
    if (pos < -100)
    DpySpec_draw(DPYPOS(DPYWAH - 7, DPYWAV + 199),
		 SS_MKDN, 0x0C, 0x08080000);  
  else if (pos >= -100 && pos <= 100)
    DpySpec_draw(DPYPOS(DPYWAH - 4, DPYWAV + 100 - pos - 4),
		 SS_MKLT, 0x0C, 0x08080000);
  else	// pos > 100
    DpySpec_draw(DPYPOS(DPYWAH - 7, DPYWAV - 8),
		 SS_MKUP, 0x0C, 0x08080000);
  }
}

/*!*****************************************************************************
 @brief		Trigger Level draw
 @details	Нарисовать маркер уровня винхронизации (когда нет меню)
 @param		On - Вкл/Выкл
 */
void TrigLev_draw(bool On) {
  if (GenCtrl.Menu) return;
  
  int32_t lev = TrigCtrl.Level / 2U - 256;
  int32_t pos = 0;
  if (TrigCtrl.Sour == 1) pos = Chan1Ctrl.SigPos / 2 - 256;
  if (TrigCtrl.Sour == 2) pos = Chan2Ctrl.SigPos / 2 - 256;
  lev += pos;
//  Num_draw(lev);
  
  if (!On) {		// !On
    if (lev < -100)
      DpySpec_draw(DPYPOS(DPYWAH + 300 + 1, DPYWAV + 199),
		   SS_MKDN, 0x10, 0x00000000);  
    else if (lev >= -100 && lev <= 100)
      DpySpec_draw(DPYPOS(DPYWAH + 300 - 3, DPYWAV + 100 - lev - 4),
		   SS_MKRT, 0x10, 0x00000000);
    else		// lev > 100
      DpySpec_draw(DPYPOS(DPYWAH + 300 + 1, DPYWAV - 8),
		   SS_MKUP, 0x10, 0x00000000);
  }
  else {		// On
    if (lev < -100)
    DpySpec_draw(DPYPOS(DPYWAH + 300 + 1, DPYWAV + 199),
		 SS_MKDN, 0x10, 0x10100000);  
  else if (lev >= -100 && lev <= 100)
    DpySpec_draw(DPYPOS(DPYWAH + 300 - 3, DPYWAV + 100 - lev - 4),
		 SS_MKRT, 0x10, 0x10100000);
  else			// lev > 100
    DpySpec_draw(DPYPOS(DPYWAH + 300 + 1, DPYWAV - 8),
		 SS_MKUP, 0x10, 0x10100000);
  }
}


/*!*****************************************************************************
 @brief		Pretrigger Marker draw
 @details	Погасить или нарисовать маркер времени синхронизации
 @param		On - Вкл/Выкл
 @return	
 @note		
 */
void Pret_draw(bool On) {
  int32_t pret = (SweepCtrl.Pret & 0x1F) << (MemCtrl.SizeNum + 5); // предзапуск
  int32_t pos = SweepCtrl.TimePos;
  int32_t offset = pret - pos;
  if (!On) {
    if (offset < 0)
      DpySpec_draw(DPYPOS(DPYWAH - 7, DPYWAV - 8),
		   SS_MKRT, 0x0F, 0x00000000);  
    else if (offset >= 0 && offset <= 300)
      DpySpec_draw(DPYPOS(DPYWAH + offset - 3, DPYWAV - 5),
		   SS_MKDN, 0x0F, 0x00000000);
    else	// offset > 300
      DpySpec_draw(DPYPOS(DPYWAH + 300, DPYWAV - 8),
		   SS_MKLT, 0x0F, 0x00000000);
  }
  else {	// On
    if (offset < 0)
    DpySpec_draw(DPYPOS(DPYWAH - 7, DPYWAV - 8),
		 SS_MKRT, 0x0F, 0x0A0A0000);  
  else if (offset >= 0 && offset <= 300)
    DpySpec_draw(DPYPOS(DPYWAH+offset - 3, DPYWAV - 5),
		 SS_MKDN, 0x0F, 0x0A0A0000);
  else	// offset > 300
    DpySpec_draw(DPYPOS(DPYWAH + 300, DPYWAV - 8),
		 SS_MKLT, 0x0F, 0x0A0A0000);
  }
}

/*!*****************************************************************************
 @brief		
 @details	Перерисовать курсоры после закрытия МЕНЮ
 @return	
 */
void Curs_redraw(void) {
  if (CursCtrl.On) Curs_draw(true);
}

/*!*****************************************************************************
 @details	Нарисовать (стереть) курсоры
 @param		On - Вкл/Выкл
 @note		
 */
void Curs_draw(bool On) {
  if (CursCtrl.CX1 >= SweepCtrl.TimePos &&
      CursCtrl.CX1 <= SweepCtrl.TimePos + (DispCtrl.Wide ? 300 : 250)) {
    DpyLineVert_draw(DPYPOS(DPYWAH - SweepCtrl.TimePos + CursCtrl.CX1, DPYWAV),
			200, 0x00668000 | On << 7);
  }
  if (CursCtrl.CX2 >= SweepCtrl.TimePos &&
      CursCtrl.CX2 <= SweepCtrl.TimePos + (DispCtrl.Wide ? 300 : 250)) {
    DpyLineVert_draw(DPYPOS(DPYWAH - SweepCtrl.TimePos + CursCtrl.CX2, DPYWAV),
			200, 0x003C8000 | On << 7);
  }
  if (CursCtrl.CY1 >= 128 - 100 && CursCtrl.CY1 <= 128 + 100) {
    DpyLineHorz_draw(DPYPOS(DPYWAH, DPYWAV + (228 - CursCtrl.CY1)),
			(DispCtrl.Wide ? 300 : 250), 0x00668000 | On << 7);
  }
  if (CursCtrl.CY2 >= 128 - 100 && CursCtrl.CY2 <= 128 + 100) {
    DpyLineHorz_draw(DPYPOS(DPYWAH, DPYWAV + (228 - CursCtrl.CY2)),
			(DispCtrl.Wide ? 300 : 250), 0x003C8000 | On << 7);
  }

}

/*!*****************************************************************************
 @brief		Signal Memory clear
 @details	Очистить память сигналов
 @param		
 @return	
 @note		
 */
void Sig_clear(void) {
  uint16_t *pSig1 = SigChan1.Buf16K;
  uint16_t *pSig2 = SigChan2.Buf16K;
  for (uint32_t i = 16384; i--; ) {
    *pSig1++ = 0;
    *pSig2++ = 0;
  }
}

/*!*****************************************************************************
 @brief		Channels 2|1 cos|sin load for test
 @details	Загрузить буферы каналов синусом и косинусом
 @return	
 @note		
 */
void SigChanAC_load(void) {
  uint16_t *pSigChan1 = SigChan1.Buf16K;
  uint16_t *pSigChan2 = SigChan2.Buf16K;
  for (uint32_t i = 16384; i--; ) {
    *pSigChan1++ = (uint8_t)(128 + 75 * cos(2 * 3.1415926 * i / 128)) << 8;
    *pSigChan2++ = (uint8_t)(128 + 75 * sin(2 * 3.1415926 * i / 128)) << 8;
  }
}

/*!*****************************************************************************
 @brief		Channels 2|1 random noise load
 @details	Загрузить буферы каналов постоянным сигналом с небольшим шумом
 @return	
 @note		
 */
void SigChanDC_load(void) {
  uint16_t *pSigChan1 = SigChan1.Buf16K;
  uint16_t *pSigChan2 = SigChan2.Buf16K;
  for (uint32_t i = 16384; i--; ) {
    uint16_t temp = rand();
    *pSigChan1++ = 0x7A7A
		 + ((temp + (temp >> 2) + (temp >> 4) + (temp >> 6)) & 0x0707);
    temp = rand();
    *pSigChan2++ = 0x7A7A
		 + ((temp + (temp >> 2) + (temp >> 4) + (temp >> 6)) & 0x0707);
  }
}

/*!*****************************************************************************
 @brief		Signals dots both or one channel read-average from DotBuf to Buf
 @details	Переписать и усреднить точки из буфера точек в буфер каналов
 @return	
 @note		
 */
void SigDots_rdavr(void) {
  FrameCnt++;				// Увеличить счетчик кадров с неизменным режимом
  if (!FrameCnt) FrameCnt--;		// если было переполнение, остаться на максимуме
  uint32_t cnt = FrameCnt;		// счетчик кадров с неизменным режимом
  uint32_t clog = 0;			// логарифм счетчика обработанных кадров
  while (cnt >>= 1) clog++;
  clog--;				// для 1-го кадра нет усреднения (>>0)
  uint32_t aver = AcquCtrl.Aver >> 2;	// вес усреднения
  if (clog < aver) aver = clog;
  
/* Смещение в буфере 16-битовых выборок сигнала (в выборках, не в байтах!) */  
  uint16_t *pSigChan1 = SigChan1.Buf16K
		      + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
  uint16_t *pSigChan2 = SigChan2.Buf16K
		      + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
  uint32_t temp_dot;
  uint32_t temp_mem;
    
/* Переписать включенные каналы */
  for (uint32_t i = SweepCtrl.Pret << MemCtrl.SizeNum + 5; i--; ) {
    temp_dot = (uint32_t)*pDotBuf++;	// прочитать два канала (00002211)
    if (pDotBuf == DotBuf + 16384) pDotBuf = DotBuf;	// ограничить указатель
    if (Chan1Ctrl.On) {
      temp_mem = *pSigChan1 << 16;	// загрузить в старшие разряды (iiff0000)
      temp_mem -= temp_mem >> aver;	// уменьшить на величину усреднения (0iiff000)
      temp_mem += ((temp_dot & 0x00FF) << 24) >> aver;	// добавить часть канала
      *pSigChan1++ = temp_mem >> 16;	// сохранить старшие разряды
    }
    if (Chan2Ctrl.On) {
      temp_mem = *pSigChan2 << 16;	// загрузить в старшие разряды (iiff0000)
      temp_mem -= temp_mem >> aver;	// уменьшить на величину усреднения (0iiff000)
      temp_mem += ((temp_dot & 0xFF00) << 16) >> aver;	// добавить часть канала
      *pSigChan2++ = temp_mem >> 16;	// сохранить старшие разряды
    }
  }  
}



/*!*****************************************************************************
 @brief		Signals both, one, neither channels read-average from PL to MCU
 @details	Переписать и усреднить сигналы из ПЛИС в МК
 @return	
 @note		
 */
void SigChans_rdavr(void) {
  FrameCnt++;			// Увеличить счетчик кадров с неизменным режимом
  if (!FrameCnt) FrameCnt--;	// если было переполнение, остаться на максимуме
  uint32_t cnt = FrameCnt;		// счетчик кадров с неизменным режимом
  uint32_t clog = 0;			// логарифм счетчика обработанных кадров
  while (cnt >>= 1) clog++;
  clog--;				// для 1-го кадра нет усреднения (>>0)
  uint32_t aver = AcquCtrl.Aver >> 2;	// вес усреднения
  if (clog < aver) aver = clog;

/* Смещение в буфере 16-битовых выборок сигнала (в выборках, не в байтах!) */  
  uint16_t *pSigChan1 = SigChan1.Buf16K + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
  uint16_t *pSigChan2 = SigChan2.Buf16K + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;

// Корректировать указатель по результату измерения
/*  
  if (Interp.Tcur > Interp.Tmts >> 16) {
    DpyCharPad_draw(DPYPOS(DPYWAH + 300 - 2 * 8 - 2, 1), '>', 0xf3f30000);
    pSigChan1 += 1; 
    pSigChan2 += 1; 
  }
  else DpyCharPad_draw(DPYPOS(DPYWAH + 300 - 2 * 8 - 2, 1), '<', 0xf3f30000);
  // Num_draw(Interp.Tcur);
 */
  
  uint32_t temp_pld;
  uint32_t temp_mem;
    
/* Переписать включенные каналы */
  for (uint32_t i = 1024 << MemCtrl.SizeNum; i--; ) {
    temp_pld = (uint32_t)PL->MINT;	// прочитать два канала (00002211)
    if (Chan1Ctrl.On) {
      temp_mem = *pSigChan1 << 16;	// загрузить в старшие разряды (iiff0000)
      temp_mem -= temp_mem >> aver;	// уменьшить на величину усреднения (0iiff000)
      temp_mem += ((temp_pld & 0x00FF) << 24) >> aver;	// добавить часть канала
      *pSigChan1++ = temp_mem >> 16;	// сохранить старшие разряды
    }
    if (Chan2Ctrl.On) {
      temp_mem = *pSigChan2 << 16;	// загрузить в старшие разряды (iiff0000)
      temp_mem -= temp_mem >> aver;	// уменьшить на величину усреднения (0iiff000)
      temp_mem += ((temp_pld & 0xFF00) << 16) >> aver;	// добавить часть канала
      *pSigChan2++ = temp_mem >> 16;	// сохранить старшие разряды
    }
  }
}

/*!*****************************************************************************
 @brief		Signals Buffer clear
 @details	Очистить буферы сигналов
 @return	
 @note		
 */
void SigChans_clear(void) {
// Смещение в буфере 16-битовых выборок сигнала (в выборках, не в байтах!)  
  uint32_t offset = (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
  uint16_t *pSigChan1 = SigChan1.Buf16K + offset;
  uint16_t *pSigChan2 = SigChan2.Buf16K + offset;
// Очистить каналы
  for (uint32_t i = 1024 << MemCtrl.SizeNum; i--; ) {
    *pSigChan1++ = 0;
    *pSigChan2++ = 0;
  }
}

/*!*****************************************************************************
 @brief		Memory Bar draw
 @details	Нарисовать полосу памяти сигналов
 @return	
 @note		Длина полосы 128 точек (в 8 раз меньше памяти 1K)
 */
void MemBar_draw(void) {
  DpyLineHorz_draw(DPYPOS(DPYWAH + 125 - 64, 6), 128, 0x00FFFF40);
  DpyLineVert_draw(DPYPOS(DPYWAH + 125 - 64, 4), 5, 0x00FFFF40);
  DpyLineVert_draw(DPYPOS(DPYWAH + 125 + 64, 4), 5, 0x00FFFF40);  
}

/*!*****************************************************************************
 @brief		Memory Bar Pretrigger draw
 @details	Нарисовать позицию предзапуска на полосе памяти
 @return	
 @note		
 */
void MemBarPret_draw(bool On) {
  int32_t pret = SweepCtrl.Pret << 2;	// шаг через 4 точки
  if (!On) {				// Стереть
    DpySpec_draw(DPYPOS(DPYWAH + 125 - 64 + pret - 3, 0),
		 SS_MKSM, 0x0F, 0x00000000);  
  }
  else {				// Нарисовать
    DpySpec_draw(DPYPOS(DPYWAH + 125 - 64 + pret - 3, 0),
		 SS_MKSM, 0x0F, 0x0A0A0000);
  }
}

/*!*****************************************************************************
 @brief		Memory Bar Position draw
 @details	Нарисовать позицию экрана на полосе памяти
 @return	
 @note		
 */
void MemBarPos_draw(bool On) {
  int32_t pos = SweepCtrl.TimePos >> MemCtrl.SizeNum + 3;
  // int32_t size = DispCtrl.Wide ? 75 : 62;
  int32_t size = 300 >> MemCtrl.SizeNum + 3; 
  // int32_t color = On ? 0x00FFFFCC : 0x00FFFF00;		// зеленыый CC
  int32_t color = On ? 0x00FFFF0A : 0x00FFFF00;		// как оба канала 0F
  DpyLineHorz_draw(DPYPOS(DPYWAH + 125 - 64 + pos, 5), size, color);
  DpyLineHorz_draw(DPYPOS(DPYWAH + 125 - 64 + pos, 7), size, color);
}

/*!*****************************************************************************
 @brief		Mathematics execute
 @details	Выполнить математическую функцию 
 @return	
 @note		
 */
void Math_exe(void) {
#ifdef MATH_USE
  DpyWork_clear(MASK_MAT);  
  if (!MathCtrl.On) return;
  if (MathCtrl.Fun == MATH_SUM) Sum_draw(true);
  else if (MathCtrl.Fun == MATH_MULT) Mult_draw(true);
  else if (MathCtrl.Fun == MATH_FFT) {
    FFT_calc();
    FFT_draw(true);
  }
#endif
}

/*!*****************************************************************************
 @brief		Summa draw
 @details	Нарисовать сумму каналов 
 @return	
 @note		
 */
void Sum_draw(bool bright) {
  if (DispCtrl.Vect)	SumVect_draw(bright);
  else			SumDots_draw(bright);
}

/*!*****************************************************************************
 @brief		Summa Dots draw
 @details	Нарисовать точки суммы сигналов, перемещаясь по индексу в буфере
 @param		bright - яркость 1, 0 (выкл)
 @return	
 @note		lmarg, rmarg - границы рисуемого сигнала
 */
void SumDots_draw(bool bright) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  int32_t lmarg = (SigCtrl.SigBeg > SweepCtrl.TimePos) ?
                   SigCtrl.SigBeg - SweepCtrl.TimePos : 0;
  int32_t rmarg = (SigCtrl.SigEnd < SweepCtrl.TimePos + width) ?
                   SigCtrl.SigEnd - SweepCtrl.TimePos : width;
  if (lmarg >= width || rmarg < 0) return;
  uint16_t *pSig1 = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum 
		 + SweepCtrl.TimePos
		 + lmarg + SigCtrl.SigInx;
  uint16_t *pSig2 = SigChan2.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum 
		 + SweepCtrl.TimePos
		 + lmarg + SigCtrl.SigInx;
  uint16_t *pLim1 = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
/* Вернуть указатели внутрь буферов */
  if (pSig1 >= pLim1) {
    pSig1 -= (1024 << MemCtrl.SizeNum);
    pSig2 -= (1024 << MemCtrl.SizeNum);
  }
  volatile uint8_t *pDpy;
  for (uint32_t i = lmarg; i < rmarg; i++) {
    int16_t temp1 = (*pSig1++) >> 8;	// старшая (целая) часть
    int16_t temp2 = (*pSig2++) >> 8;	// старшая (целая) часть
    if (pSig1 >= pLim1) {
      pSig1 -= (1024 << MemCtrl.SizeNum);
      pSig2 -= (1024 << MemCtrl.SizeNum);
    }
#ifdef INVPOS_USE
    if (Chan1Ctrl.Inv) temp1 = 0x100 - temp1;
    if (Chan2Ctrl.Inv) temp2 = 0x100 - temp2;   
#else
    if (Chan1Ctrl.Inv) temp1 = (Chan1Ctrl.SigPos) - temp1 - 256 - Chan1Ctrl.DispPos;
    if (Chan2Ctrl.Inv) temp2 = (Chan2Ctrl.SigPos) - temp2 - 256 - Chan2Ctrl.DispPos;    
#endif
    temp1 = (Chan1Ctrl.DispPos / 2U + temp1);
    temp2 = (Chan2Ctrl.DispPos / 2U + temp2);
    
    pDpy = (uint8_t *)(DPYBUF_BASE + DPYWAH + i 
	 + 320 * (DPYWAV + 228 - (temp1 + temp2 - 128)));
    if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) { 
      uint8_t pix = *pDpy;  pix ^= (pix ^ bright << 4) & 0x10;  *pDpy = pix;
    }
  }
}

/*!*****************************************************************************
 @brief		Summa Vectors draw
 @details	Нарисовать точки суммы сигналов, перемещаясь по индексу в буфере
 @param		bright - яркость 1, 0 (выкл)
 @return	
 @note		lmarg, rmarg - границы рисуемого сигнала
 */
void SumVect_draw(bool bright) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  int32_t lmarg = (SigCtrl.SigBeg > SweepCtrl.TimePos) ?
                   SigCtrl.SigBeg - SweepCtrl.TimePos : 0;
  int32_t rmarg = (SigCtrl.SigEnd < SweepCtrl.TimePos + width) ?
                   SigCtrl.SigEnd - SweepCtrl.TimePos : width;
  if (lmarg >= width || rmarg < 0) return;
  uint16_t *pSig1 = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum
                 + SweepCtrl.TimePos
		 + lmarg + SigCtrl.SigInx;
  uint16_t *pSig2 = SigChan2.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum
                 + SweepCtrl.TimePos
		 + lmarg + SigCtrl.SigInx;
  uint16_t *pLim1 = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
/* Вернуть указатели внутрь буферов */
  if (pSig1 >= pLim1) {
    pSig1 -= (1024 << MemCtrl.SizeNum);
    pSig2 -= (1024 << MemCtrl.SizeNum);
  }
  uint32_t t1p, t2p, t1n, t2n;		// previous & next samples two channels
/* Цикл между первым вектором и последним */
  for (int32_t i = lmarg; i < rmarg; i++) {
    t1p = (*pSig1++) >> 8;		// integer part, begin
    t2p = (*pSig2++) >> 8;
    if (pSig1 >= pLim1) {
      pSig1 -= (1024 << MemCtrl.SizeNum);
      pSig2 -= (1024 << MemCtrl.SizeNum);
    }
    t1n = (*pSig1) >> 8;		// integer part, end
    t2n = (*pSig2) >> 8;
#ifdef INVPOS_USE
    if (Chan1Ctrl.Inv) {
      t1p = 0x100 - t1p;
      t1n = 0x100 - t1n;
    }
    if (Chan2Ctrl.Inv) {
      t2p = 0x100 - t2p;
      t2n = 0x100 - t2n;
    }
#else
    if (Chan1Ctrl.Inv) {
      t1p = (Chan1Ctrl.SigPos) - t1p - 256 - Chan1Ctrl.DispPos;
      t1n = (Chan1Ctrl.SigPos) - t1n - 256 - Chan1Ctrl.DispPos;
    }
    if (Chan2Ctrl.Inv) {
      t2p = (Chan2Ctrl.SigPos) - t2p - 256 - Chan2Ctrl.DispPos;
      t2n = (Chan2Ctrl.SigPos) - t2n - 256 - Chan2Ctrl.DispPos;
    }
#endif
    t1p += Chan1Ctrl.DispPos / 2U;
    t1n += Chan1Ctrl.DispPos / 2U;
    t2p += Chan2Ctrl.DispPos / 2U;
    t2n += Chan2Ctrl.DispPos / 2U;
    int32_t s1, s2, brk;
    s1 = t1p + t2p - 128;
    s2 = t1n + t2n - 128;
#ifdef STRA_VECT
    brk = (s1 == s2) ? s1 : ((s1 > s2) ? (s1 - 1) : (s1 + 1));
#else
    brk = (t1p + t2p + t1n + t2n - 256) >> 1;
#endif   
    VectMath_draw(s1, brk, s2, i, bright);
  }
}

/*!*****************************************************************************
 @brief		Multiply draw
 @details	Нарисовать произведение каналов 
 @return	
 @note		
 */
void Mult_draw(bool bright) {
  if (DispCtrl.Vect)	MultVect_draw(bright);
  else			MultDots_draw(bright);
}

/*!*****************************************************************************
 @brief		Multiply Dots draw
 @details	Нарисовать точки произведения сигналов
 @param		bright - яркость
 @return	
 @note		lmarg, rmarg - границы рисуемого сигнала
 */
void MultDots_draw(bool bright) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  int32_t lmarg = (SigCtrl.SigBeg > SweepCtrl.TimePos) ?
                   SigCtrl.SigBeg - SweepCtrl.TimePos : 0;
  int32_t rmarg = (SigCtrl.SigEnd < SweepCtrl.TimePos + width) ?
                   SigCtrl.SigEnd - SweepCtrl.TimePos : width;
  if (lmarg >= width || rmarg < 0) return;
  uint16_t *pSig1 = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum 
		 + SweepCtrl.TimePos
		 + lmarg + SigCtrl.SigInx;
  uint16_t *pSig2 = SigChan2.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum 
		 + SweepCtrl.TimePos
		 + lmarg + SigCtrl.SigInx;
  uint16_t *pLim1 = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
/* Вернуть указатели внутрь буферов */
  if (pSig1 >= pLim1) {
    pSig1 -= (1024 << MemCtrl.SizeNum);
    pSig2 -= (1024 << MemCtrl.SizeNum);
  }
  volatile uint8_t *pDpy;
  for (uint32_t i = lmarg; i < rmarg; i++) {
    int16_t temp1 = ((*pSig1++) >> 8) - 128;	// старшая (целая) часть
    int16_t temp2 = ((*pSig2++) >> 8) - 128;	// старшая (целая) часть
    if (pSig1 >= pLim1) {
      pSig1 -= (1024 << MemCtrl.SizeNum);
      pSig2 -= (1024 << MemCtrl.SizeNum);
    }
    temp1 = temp1 - (Chan1Ctrl.SigPos / 2U - 256);
    temp2 = temp2 - (Chan2Ctrl.SigPos / 2U - 256);
    int16_t res = (temp1 * temp2) / 64 + 128;

    pDpy = (uint8_t *)(DPYBUF_BASE + DPYWAH + i 
	 + 320 * (DPYWAV + 228 - res));
    if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) { 
      uint8_t pix = *pDpy;  pix ^= (pix ^ bright << 4) & 0x10;  *pDpy = pix;
    }
  }
}

/*!*****************************************************************************
 @brief		Multiply Vectors draw 
 */
void MultVect_draw(bool bright) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  int32_t lmarg = (SigCtrl.SigBeg > SweepCtrl.TimePos) ?
                   SigCtrl.SigBeg - SweepCtrl.TimePos : 0;
  int32_t rmarg = (SigCtrl.SigEnd < SweepCtrl.TimePos + width) ?
                   SigCtrl.SigEnd - SweepCtrl.TimePos : width;
  if (lmarg >= width || rmarg < 0) return;
  uint16_t *pSig1 = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum
                 + SweepCtrl.TimePos
		 + lmarg + SigCtrl.SigInx;
  uint16_t *pSig2 = SigChan2.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum
                 + SweepCtrl.TimePos
		 + lmarg + SigCtrl.SigInx;
  uint16_t *pLim1 = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
/* Вернуть указатели внутрь буферов */
  if (pSig1 >= pLim1) {
    pSig1 -= (1024 << MemCtrl.SizeNum);
    pSig2 -= (1024 << MemCtrl.SizeNum);
  }

/* Цикл между первым вектором и последним */
  for (int32_t i = lmarg; i < rmarg; i++) {
    int16_t t1p = ((*pSig1++) >> 8) - 128;	// integer part Ch1, previous
    int16_t t2p = ((*pSig2++) >> 8) - 128;	// integer part Ch2, previous
    if (pSig1 >= pLim1) {
      pSig1 -= (1024 << MemCtrl.SizeNum);
      pSig2 -= (1024 << MemCtrl.SizeNum);
    }
    int16_t t1n = ((*pSig1) >> 8) - 128;	// integer part Ch1, next
    int16_t t2n = ((*pSig2) >> 8) - 128;	// integer part Ch2, next
    
    t1p -= Chan1Ctrl.SigPos / 2U - 256;
    t2p -= Chan2Ctrl.SigPos / 2U - 256;

    
    t1n -= Chan1Ctrl.SigPos / 2U - 256;
    t2n -= Chan2Ctrl.SigPos / 2U - 256;

    int32_t s1 = (t1p * t2p) / 64 + 128;
    int32_t s2 = (t1n * t2n) / 64 + 128;
    int32_t brk = (s1 == s2) ? s1 : ((s1 > s2) ? (s1 - 1) : (s1 + 1));
  
    VectMath_draw(s1, brk, s2, i, bright);
  }
}


/*__ Локальные функции ________________________________________________________*/

/*!*****************************************************************************
 @brief		Move Dot from FPGA to Dot Buffer
 @details	Переписать регистр АЦП из ПЛИС в буфер точек
 @return	
 @note		pDot указывает на последнюю записанную точку
 */
void Sweep_start(void) {
  if (SweepCtrl.Mode == 2) {		// Если режим однократный
    SweepCtrl.Stop = 1;			// Остановить
    StSp_disp();			// Написать
  }
  else PL->STAT = PL_START_ACQUST;	// Старт нового сбора
}

#if false
/*!*****************************************************************************
 @brief		Move Dot from FPGA to Dot Buffer
 @details	Переписать регистр АЦП из ПЛИС в буфер точек
 @return	
 @note		pDot указывает на последнюю записанную точку
 */
void Dot_move(void) {
  pDot++;
  if (pDotBuf >= DotBuf + 16384) pDotBuf = DotBuf;
  *pDotBuf = PL->SBREG;
}
#endif

/*!*****************************************************************************
 @brief		One Math Vector draw
 @details	Нарисовать вектор (в двух соседних столбцах)
 @param		s1, brk, s2
 @param		dpos - позиция начала вектора по горизонтали
 @param		bright - яркость 1, 0 (выкл)
 @return	
 @note		
 */
void VectMath_draw(int32_t s1, int32_t brk, int32_t s2, 
		uint32_t dpos, bool bright)
{
//  if (SigCtrl.SigNum == FRAME_SPARSE && (!(s1) || !(s2))) return;
  if (SigCtrl.SigNum == FRAME_SPARSE && (s1 < 8 || s2 < 8)) return;

  uint32_t size;			// размер вектора
  uint8_t *pDpy = (uint8_t *)(DPYBUF_BASE + DPYWAH + dpos + 320 * (DPYWAV + 228 - s1));
/* от левой выборки до середины */
  if (s1 < brk) {			// up vector			
    size = brk - s1;
    for (uint32_t j = size; j--; ) {
      pDpy -= 320;			// перескочить - нарисовать
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy; pix ^= (pix ^ bright << 4) & 0x10; *pDpy = pix;
      }
    }
  }
  else if (s1 > brk) {		// down vector
    size = s1 - brk;
    for (uint32_t j = size; j--; ) {
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy; pix ^= (pix ^ bright << 4) & 0x10; *pDpy = pix;
      }
      pDpy += 320;			// нарисовать - перескочить
    }
  }
  else {				// s1 == brk, right vector
    if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
      uint8_t pix = *pDpy; pix ^= (pix ^ bright << 4) & 0x10; *pDpy = pix;
    }
  }
/* сдвиг на следующий ряд */
  pDpy++;
/* от середины до правой выборки */    
  if (brk < s2) {			// up vector  
    size = s2 - brk;
    for (uint32_t j = size; j--; ) {
      pDpy -= 320;			// перескочить - нарисовать    
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy; pix ^= (pix ^ bright << 4) & 0x10; *pDpy = pix;
      }
    }
  }
  else if (brk > s2) {		// down vector
    size = brk - s2;
    for (uint32_t j = size; j--; ) {
      if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) {
        uint8_t pix = *pDpy; pix ^= (pix ^ bright << 4) & 0x10; *pDpy = pix;
      }
      pDpy += 320;			// нарисовать - перескочить      
    }
  }
}


/*!*****************************************************************************
 @brief		Measure parameters of signal
 @details	Измерить параметры сигнала по кадру данных
 @return	
 @note		учитывается и дробная часть числа
 @note		Диапазон смещения 0...1023 уменьшается в 2 раза,
		и его середина становится равной 256.
		К целой части сигнала добавляется 128,
		и его середина перемещается на 256.
 */
void SigPar_meas(void) {
  if (!MeasCtrl.On) return;
  // if (CursCtrl.On) return;

  uint8_t Meas1Txt[10];
  uint8_t Meas2Txt[10];
  
// Написать ошибку (по умолчанию)
  NumWrong_conv(Meas1Txt);
  NumWrong_conv(Meas2Txt);

/* 1. Найти минимальное, максимальное и среднее значения */

  uint16_t *pSig = (MeasCtrl.Sour ? SigChan2.Buf16K : SigChan1.Buf16K)
		 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;

  uint32_t	min = 255 << 8;
  uint32_t	max = 0 << 0;
  uint32_t	mean = 0 << 0;
  
  for (uint32_t i = 1024 << MemCtrl.SizeNum; i--; ) {
    uint32_t temp = *pSig++;		// прочитать слово, перейти на следующее 
    if (temp < min) min = temp;		// новый минимум
    if (temp > max) max = temp;		// новый максимум
    mean += temp;			// суммировать
  }
  mean >>= 9 + MemCtrl.SizeNum;		// поделить на общее количество
  // Num_draw(min >> 8);
  // Num_draw(max >> 8);  
  // Num_draw(mean >> 8);

/*
  Если минимум или максимум - на пределе, результат измерений будет неверным
  Для минимума и максимума можно проверять только минимум или максимум
  if ((min >> 8) == 0 || (min >> 8) == 0xFF)
  if ((max >> 8) == 0 || (max >> 8) == 0xFF)
 */
  if ((min >> 8) != 0 && (max >> 8) != 0xFF)
  {
/* Измерения напряжения будут достоверными */  

/* 2. Вычислить минимум, максимум, размах или среднее, если нужно */

    if ((MeasCtrl.Par1 >> 2) >= PAR_MIN ||
        (MeasCtrl.Par2 >> 2) >= PAR_MIN)
    {

      uint32_t volt = (MeasCtrl.Sour ? Chan2Ctrl.VoltNum : Chan1Ctrl.VoltNum) - 6
                    + (MeasCtrl.Sour ? Chan2Ctrl.Prb : Chan1Ctrl.Prb) * 3 + 7;
      uint32_t spos = MeasCtrl.Sour ? Chan2Ctrl.SigPos : Chan1Ctrl.SigPos;

      switch (MeasCtrl.Par1 >> 2) {
      case PAR_MIN:
        Num2Volt_conv((min >> 8) + 128 - (spos >> 1), volt, Meas1Txt);
        break;
      case PAR_MAX:
        Num2Volt_conv((max >> 8) + 128 - (spos >> 1), volt, Meas1Txt);
        break;
      case PAR_AMP:
        Num2Volt_conv((max - min) >> 8, volt, Meas1Txt);
        break;
      case PAR_AVR:
        Num2Volt_conv((mean >> 8) + 128 - (spos >> 1), volt, Meas1Txt);
        break;  
      }

      switch (MeasCtrl.Par2 >> 2) {
      case PAR_MIN:
        Num2Volt_conv((min >> 8) + 128 - (spos >> 1), volt, Meas2Txt);
        break;
      case PAR_MAX:
        Num2Volt_conv((max >> 8) + 128 - (spos >> 1), volt, Meas2Txt);
        break;
      case PAR_AMP:
        Num2Volt_conv((max - min) >> 8, volt, Meas2Txt);
        break;
      case PAR_AVR:
        Num2Volt_conv((mean >> 8) + 128 - (spos >> 1), volt, Meas2Txt);
        break;  
      }
    }

/* 3. Найти длительности низкого и высокого уровней, период и частоту
      если нужно */

    if (((MeasCtrl.Par1 >> 2) <= PAR_WIP
     || (MeasCtrl.Par2 >> 2) <= PAR_WIP)
     && ((max - min) >> 8) > 8)
    {
// Найти середину
      uint32_t mid = (max + min) / 2;
    
// Указатель сигнала установить в начало    
      uint16_t *pSig = (MeasCtrl.Sour ? SigChan2.Buf16K : SigChan1.Buf16K)
		     + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
		     
// Указатель на конец массива сигнала, на 1 больше 
      uint16_t *pEnd = (MeasCtrl.Sour ? SigChan2.Buf16K : SigChan1.Buf16K)
                     + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);

      bool stlo = false;		// сигнал с начале кадра ниже порога
      bool find = false;		// переход найден
 
// Искать сигнал за пределами шумовых порогов (+- 4?)
      while (pSig < pEnd) {
	if (*pSig < mid - 4 || *pSig > mid + 4) {
	  if (*pSig < mid - 4) stlo = true;
	  find = true;
	  break;
	}
	pSig++;
      }
// Если сигнал большой, продолжить...
      if (find) {
// Найти первый переход через середину после сигнала за порогом шума
        find = false;
        if (stlo) {			// искать фронт
          while (pSig < pEnd) {
	    if (*pSig > mid) {
	      find = true;
	      break;
	    }
	    pSig++;
	  }
	}
	else {				// искать срез
	  while (pSig < pEnd) {
	    if (*pSig < mid) {
	      find = true;
	      break;
	    }
	    pSig++;
	  }
	}
// Запомнить время первого перехода
	uint16_t *T1 = pSig;
// Передвинуться на несколько выборок
	pSig++;
	pSig++;
	pSig++;
// Если кадр закончился, искать нечего...
	if (pSig > pEnd) find = false;

// Если первый переход найден, продолжить...
	if (find) {
// Искать второй переход через середину
	  find = false;
	  if (stlo) {			// искать срез
	    while (pSig < pEnd) {
	      if (*pSig < mid) {
	        find = true;
		break;
	      }
	      pSig++;
	    }
	  }
	  else {			// искать фронт
	    while (pSig < pEnd) {
	      if (*pSig > mid) {
	        find = true;
		break;
	      }
	      pSig++;
	    }
	  }
// Запомнить время второго перехода
          uint16_t *T2 = pSig;
// Передвинуться на несколько выборок
	  pSig++;
	  pSig++;
	  pSig++;
// Если кадр закончился, искать нечего...
	  if (pSig > pEnd) find = false;

// Если второй переход найден, продолжить...
          if (find) {
// Искать третий переход через середину
            find = false;
	    if (stlo) {			// искать фронт
	      while (pSig < pEnd) {
	        if (*pSig > mid) {
		  find = true;
		  break;
		}
		pSig++;
	      }
	    }
	    else {			// искать срез
	      while (pSig < pEnd) {
	        if (*pSig < mid) {
		  find = true;
		  break;
		}
		pSig++;
	      }
	    }
// Если третий переход найден, найти ширину импульсов...
	    if (find) {
	      uint16_t *T3 = pSig;
	      // Num_draw(T3 - T1);

              switch (MeasCtrl.Par1 >> 2) {
              case PAR_TIM:
                Num2Time_conv(T3 - T1, Meas1Txt);
                break;
	      case PAR_FRQ:
	        Num2Freq_conv(T3 - T1, Meas1Txt);
		break;
              case PAR_WIN:
                Num2Time_conv(stlo ? (T3 - T2) : (T2 - T1), Meas1Txt);
                break;
              case PAR_WIP:
                Num2Time_conv(stlo ? (T2 - T1) : (T3 - T2), Meas1Txt);
                break;
              }
      
              switch (MeasCtrl.Par2 >> 2) {
              case PAR_TIM:
                Num2Time_conv(T3 - T1, Meas2Txt);
                break;
	      case PAR_FRQ:
	        Num2Freq_conv(T3 - T1, Meas2Txt);
		break;
              case PAR_WIN:
                Num2Time_conv(stlo ? (T3 - T2) : (T2 - T1), Meas2Txt);
                break;
              case PAR_WIP:
                Num2Time_conv(stlo ? (T2 - T1) : (T3 - T2), Meas2Txt);
                break;
              }
      
	    }			// find T3
	  }			// find T2
	}			// find T1
      }				// find signal
    }				// meas Period, Frequense, Width

/* 4. Найти длительность среза */

    if (((MeasCtrl.Par1 >> 2) == PAR_TFA
     || (MeasCtrl.Par2 >> 2) == PAR_TFA)
     && ((max - min) >> 8) > 8)
    {
// Указатель на начало массива сигнала      
      uint16_t *pSig = (MeasCtrl.Sour ? SigChan2.Buf16K : SigChan1.Buf16K)
		     + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
// Указатель на конец массива сигнала, на 1 больше 
      uint16_t *pEnd = (MeasCtrl.Sour ? SigChan2.Buf16K : SigChan1.Buf16K)
                     + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
// Пороги 0.1, 0.9
      uint16_t trlo = min + (max - min) / 10;
      uint16_t trhi = max - (max - min) / 10;
      // Num_draw(trlo >> 8);
      // Num_draw(trhi >> 8);

      bool find = false;	// состояние (переход) найдено
// Искать сигнал над порогом
      while (pSig < pEnd) {
	if (*pSig > trhi) {
	  find = true;
	  break;
	}
	pSig++;
      }
// Если сигнал над порогом, продолжить...
      if (find) {
// Найти переход через порог, начало среза
        find = false;
	while (pSig < pEnd) {
	  if (*pSig < trhi) {
	    find = true;
	    break;
	  }
	  pSig++;
	}
// Если первый переход найден, продолжить...
	if (find) {
// Запомнить время первого перехода
	  uint16_t *T1 = pSig;
	  pSig++;
// Найти второй переход через другой порог, конец среза
	  find = false;
	  while (pSig < pEnd) {
	    if (*pSig < trlo) {
	      find = true;
	      break;
	    }
	    pSig++;
	  }
// Если второй переход найден, продолжить...
          if (find) {
// Запомнить время второго перехода
            uint16_t *T2 = pSig;
	    // Num_draw(T2 - T1);

// Вывести длительность среза
            if ((MeasCtrl.Par1 >> 2) == PAR_TFA)
              Num2Time_conv(T2 - T1, Meas1Txt);
            if ((MeasCtrl.Par2 >> 2) == PAR_TFA)
              Num2Time_conv(T2 - T1, Meas2Txt);

	  }			// find T2
	}			// find T1
      }				// find signal
    }				// meas Tfa

/* 5. Найти длительность фронта */

    if (((MeasCtrl.Par1 >> 2) == PAR_TRI
     || (MeasCtrl.Par2 >> 2) == PAR_TRI)
     && ((max - min) >> 8) > 8)
    {
// Указатель на начало массива сигнала      
      uint16_t *pSig = (MeasCtrl.Sour ? SigChan2.Buf16K : SigChan1.Buf16K)
		     + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
// Указатель на конец массива сигнала, на 1 больше 
      uint16_t *pEnd = (MeasCtrl.Sour ? SigChan2.Buf16K : SigChan1.Buf16K)
                     + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
// Пороги 0.1, 0.9
      uint16_t trlo = min + (max - min) / 10;
      uint16_t trhi = max - (max - min) / 10;
      // Num_draw(trlo >> 8);
      // Num_draw(trhi >> 8);

      bool find = false;	// состояние (переход) найдено
// Искать сигнал под порогом
      while (pSig < pEnd) {
	if (*pSig < trlo) {
	  find = true;
	  break;
	}
	pSig++;
      }
// Если сигнал под порогом, продолжить...
      if (find) {
// Найти переход через порог, начало фронта
        find = false;
	while (pSig < pEnd) {
	  if (*pSig > trlo) {
	    find = true;
	    break;
	  }
	  pSig++;
	}
// Если первый переход найден, продолжить...
	if (find) {
// Запомнить время первого перехода
	  uint16_t *T1 = pSig;
	  pSig++;
// Найти второй переход через другой порог, конец фронта
	  find = false;
	  while (pSig < pEnd) {
	    if (*pSig > trhi) {
	      find = true;
	      break;
	    }
	    pSig++;
	  }
// Если второй переход найден, продолжить...
          if (find) {
// Запомнить время второго перехода
            uint16_t *T2 = pSig;
	    // Num_draw(T2 - T1);

// Вывести длительность фронта
            if ((MeasCtrl.Par1 >> 2) == PAR_TRI)
              Num2Time_conv(T2 - T1, Meas1Txt);
            if ((MeasCtrl.Par2 >> 2) == PAR_TRI)
              Num2Time_conv(T2 - T1, Meas2Txt);

	  }			// find T2
	}			// find T1
      }				// find signal
    }				// meas Tfa
    
  }				// Сигнал в пределах АЦП

/* 6. Отобразить результаты измерений */

  DpyStringPad_draw(DPYPOS(DPYVALH, DPYVAL1V), Meas1Txt, 0xE2E20000, true);
  DpyStringPad_draw(DPYPOS(DPYVALH, DPYVAL2V), Meas2Txt, 0xE2E20000, true);
}

/*!*****************************************************************************
 @brief		Number Wrong
 @details	Результат измерений неверен, написать "?"
 @param		*pBuf - указатель на буфер результата (9 символов + \0)
 @note		
 */
void NumWrong_conv(uint8_t *pBuf) {
  *(uint32_t *)pBuf = 0x20202020;
  *(uint32_t *)(pBuf + 4) = 0x20202020;
  *(uint16_t *)(pBuf + 8) = 0x003F;
}

/*!*****************************************************************************
 @brief		Number to Volt
 @details	Преобразовать число в вольты
 @param		Num - число
 @param		Volt - В/дел, определяет масштаб
 @param		*pBuf - указатель на буфер результата (9 символов + \0)
 @note		
 */
void Num2Volt_conv(int32_t Num, uint32_t Volt, uint8_t *pBuf) {
  *(uint32_t *)pBuf = 0x20202020;		// "____xxxxx"x
  *(uint32_t *)(pBuf + 4) = 0x20202020;		// "________x"x
  
  if (Num < 0) {
    *pBuf = '-';
    Num = -Num;
  }
  else *pBuf = '+';
  
  uint32_t com = 4 - Volt % 9 / 3;		// comma pos
  *(pBuf + com - 1) = '0';			// "0_______x"x
  *(pBuf + com) = '.';				// "0.______x"x
  for (uint32_t i = com + 1; i < 6; i++)
    *(pBuf + i) = '0';				// "0.000___x"x
  *(pBuf + 6) = "k m\xB5"[Volt / 9];		// "0.000___x"x
  *(pBuf + 7) = 'V';				// "0.000_V_x"x
  *(pBuf + 8) = ' ';				// "0.000_V__"x 
  *(pBuf + 9) = '\0';				// "0.000_V__"\0
  
  static const uint32_t mag[] = {40, 20, 8};			// К-т умножения результата

  Num *= mag[Volt % 3];

  pBuf += 5;					// Последняя цифра в буфере
  while (Num) {
    *pBuf-- = Num % 10 + '0';
    if (*pBuf == '.') pBuf--;
    Num /= 10;
  }
}

/*!*****************************************************************************
 @brief		Number to Time
 @details	Преобразовать число во время
 @param		Num - число
 @param		*pBuf - указатель на буфер результата (9 символов + \0)
 @note		
 */
void Num2Time_conv(int32_t Num, uint8_t *pBuf) {
  *(uint32_t *)pBuf = 0x20202020;			// "    xxxxx"x
  *(uint32_t *)(pBuf + 4) = 0x20202020;			// "        x"x

  bool less = (Num < 2);
  
  uint32_t tcom = 5 - (SweepCtrl.TimeNum + 1) % 9 / 3;	// comma pos
  *(pBuf + tcom - 1) = '0';				// "__0_____x"x
  *(pBuf + tcom) = '.';					// "__0.____x"x
  for (uint32_t i = tcom + 1; i < 7; i++)
    *(pBuf + i) = '0';					// "__0.000_x"x
  *(pBuf + 7) = " m\xB5np"[(SweepCtrl.TimeNum + 1) / 9];// "______0mx"x
  *(pBuf + 8) = 's';					// "______0ms"x
  *(pBuf + 9) = '\0';					// "______0ms"\0

  static const uint32_t mag[] = {40, 20, 8};			// К-т умножения результата

  Num *= mag[(SweepCtrl.TimeNum + 1) % 3];

  pBuf += 6;					// Последняя цифра в буфере
  while (Num) {
    *pBuf-- = Num % 10 + '0';
    if (*pBuf == '.') pBuf--;
    Num /= 10;
  }
  
  if (less) {
    while (*pBuf != ' ') *pBuf--;
    *pBuf = '<';
  }
}

/*!*****************************************************************************
 @brief		Number to Frequence
 @details	Преобразовать число в частоту
 @param		Num - число
 @param		*pBuf - указатель на буфер результата (9 символов + \0)
 @note		
 */
void Num2Freq_conv(int32_t Num, uint8_t *pBuf) {
  if (!Num) return;				// не делить на 0
  *(uint32_t *)pBuf = 0x20202020;		// "    xxxxx"x
  *(uint32_t *)(pBuf + 4) = 0x20202020;		// "        x"x
  
  uint32_t fcom = 1 + (SweepCtrl.TimeNum + 8) % 9 / 3;		// comma pos
  uint32_t fmul = 1 + (SweepCtrl.TimeNum + 8) / 9;		// mult index
  Num = (500000000 >> (2 - (SweepCtrl.TimeNum + 2) % 3)) / Num;
  while (!(Num / 100000000)) {		// Если слева нет значащей цифры
    fcom--;				// перенести запятую влево
    if (!fcom) {			// Если запятая на краю
      fcom = 3;				// перенести запятую на позицию 3
      fmul--;				// изменить индекс множителя
    }
    Num *= 10;				// увеличить число
  }					// Теперь число прижато к левому краю
  *(pBuf + fcom) = '.';
  *(pBuf + 6) = "\xB5m kMG"[fmul];
  *(pBuf + 7) = 'H';
  *(pBuf + 8) = 'z';
  *(pBuf + 9) = '\0';
  pBuf += 5;
  Num /= 10000;				// Оставить 5 значащих цифр
    
  while (Num) {
    *pBuf-- = Num % 10 + '0';
    if (*pBuf == '.') pBuf--;
    Num /= 10;
  }
}

/*!*****************************************************************************
 @brief		XY draw
 @details	Нарисовать кадр в режиме XY
 @param		нет
 @note		
 */
void XYDots_draw(bool bright) {
  uint16_t *pSig1 = SigChan1.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
  uint16_t *pSig2 = SigChan2.Buf16K
                 + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum;
//  uint16_t *pLim1 = SigChan1.Buf16K
//                 + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);

  uint32_t width = DispCtrl.Wide ? 300 : 250;
  
  volatile uint8_t *pDpy;
/* По всему кадру взять синхронно точки из двух каналов */
  for (uint32_t i = 1024 << MemCtrl.SizeNum; i--; ) {
    int16_t temp1 = (*pSig1++) >> 8;	// старшая (целая) часть
    int16_t temp2 = (*pSig2++) >> 8;	// старшая (целая) часть
    
/* Инвертировать, если нужно */
    if (Chan1Ctrl.Inv) temp1 = 0x100 - temp1;
    if (Chan2Ctrl.Inv) temp2 = 0x100 - temp2;   

/* Добавить смещение */
    temp1 = (Chan1Ctrl.DispPos / 2U + temp1);
    temp2 = (Chan2Ctrl.DispPos / 2U + temp2);

/* За горизонтальными пределами рабочей зоны не рисовать */
    if (temp1 > width) break;
    
    pDpy = (uint8_t *)(DPYBUF_BASE + DPYWAH + temp1 
	 + 320 * (DPYWAV + 228 - temp2));

/* За вертикальными пределами рабочей зоны не рисовать */
    if (pDpy > DPYWAMIN && pDpy < DPYWAMAX) { 
      uint8_t pix = *pDpy;  pix ^= (pix ^ bright << 4) & 0x10;  *pDpy = pix;
    }
  }
}  



/*!*****************************************************************************
 @brief		FFT calculate 
 @note		256 точек сигнала, которые выводятся на экран
 @verbatim
  Написано по примеру, показанному Ruslan1 с Электроникса.
  Точнее описано в документе LRZfft.pdf.
  Используется прореживание по частоте.
  Поворачивающие множители рассчитываются по формулам
  синуса и косинуса суммы углов.
  sin(a + b) = sin a * cos b + cos a * sin b
  cos(a + b) = cos a * cos b - sin a * sin b
  Действительная и мнимая части сигнала размещаются во внешнем ОЗУ
  Результат БПФ (модуль) размещается на месте действительной части  
 @endverbatim
 */

static float SigRe[FFT_N] __attribute((section("extram")));
static float SigIm[FFT_N] __attribute((section("extram")));

/* Коэффициенты деления спектральных составляющих */
static const float FFT_Att[] = {
  FFT_N / 2 * WATT_RECT,
  FFT_N / 2 * WATT_HANN,
  FFT_N / 2 * WATT_HAMM,
  FFT_N / 2 * WATT_BART,
  FFT_N / 2 * WATT_FLAT
};

void FFT_calc(void) {

  float *pSigRe;
  float *pSigIm;  
  
/* Заполнить массив оконной функцией */
  pSigRe = SigRe;		// установить указатель в начало
  const float *pWind;
  switch (MathCtrl.Wind) {
    case (WIND_RECT): {			// Прямоугольное окно
      for (uint32_t i = FFT_N; i--; )  *pSigRe++ = 1.0;
      break;
    }
    case (WIND_HANN): {			// Окно Ханна (Ханнинг)
      pWind = WindHann;
      for (uint32_t i = FFT_N; i--; )  *pSigRe++ = *pWind++;
      break;       
    }
    case (WIND_HAMM): {			// Окно Хэмминга
      pWind = WindHamm;
      for (uint32_t i = FFT_N; i--; )  *pSigRe++ = *pWind++;
      break;
    }
    case (WIND_BART): {			// Треугольное окно (Бартлетт)
      pWind = WindBart;
      for (uint32_t i = FFT_N; i--; )  *pSigRe++ = *pWind++;
      break;
    }
    case (WIND_FLAT): {		// Flat Top
      pWind = WindFlat;
      for (uint32_t i = FFT_N; i--; )  *pSigRe++ = *pWind++;
      break;
    }
  }

// Читать смещение сигнала в усилителе, добавить смещение в АЦП, для удаления
  int32_t pos = ((MathCtrl.Sour ? Chan2Ctrl.SigPos : Chan1Ctrl.SigPos)
	      - 512) / 2 + 128;

// Задать указатель на начало массива сигнала      
  uint16_t *pSig = (MathCtrl.Sour ? SigChan2.Buf16K : SigChan1.Buf16K)
		     + (1024 << MemCtrl.SizeNum) * MemCtrl.BufNum
		     + SweepCtrl.TimePos
		     + SigCtrl.SigInx;
// Задать предел указателя на 1 больше конца массива сигнала  
  uint16_t *pLim = (MathCtrl.Sour ? SigChan2.Buf16K : SigChan1.Buf16K)
                     + (1024 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
// Вернуть указатель внутрь буфера
  if (pSig >= pLim)  pSig -= (1024 << MemCtrl.SizeNum);

/* Перенести в буферы БПФ выборки сигнала
   Умножить на оконную функцию */
  pSigRe = SigRe;		// установить указатель в начало
  pSigIm = SigIm;		// установить указатель в начало
  for (uint32_t i = FFT_N; i--; ) {
    *pSigRe++ = (float)(((*pSig++) >> 8) - pos) * (*pSigRe);
    if (pSig >= pLim)  pSig -= (1024 << MemCtrl.SizeNum);
    *pSigIm++ = 0;
  }

/*
 БПФ с прореживанием по частоте:
 Внешний цикл перебирает стадии БПФ
 Средний цикл последовательно перебирает индексы в блоках бабочек
 Внутренний цикл перебирает блоки бабочек с одинаковым поворачивающим множителем
 */
  uint32_t step = FFT_N;	// шаг между блоками бабочек
  for (uint32_t n = 0; n < FFT_P; n++) {  
//  while (step >= 2) {		// стадии БПФ до двухточечного
    uint32_t dist = step / 2;	// расстояние между индексами одной бабочки
    float Wre = 1.0;		// поворачивающий множитель
    float Wim = 0.0;
    float Ac = AC[n];
    float As = AS[n];    
  
    for (uint32_t j = 0; j < dist; j++) {	// для бабочек каждого блока, W j_S
      for (uint32_t i = j; i < FFT_N; i += step) {	// новый блок
					// i - младший индекс бабочки 
        uint32_t p = i + dist;		// старший индекс бабочки
        float SumRe = *(SigRe + i) + *(SigRe + p);	// сумма, Re
        float SumIm = *(SigIm + i) + *(SigIm + p);	// сумма, Im
        float DifRe = *(SigRe + i) - *(SigRe + p);	// разность, Re
        float DifIm = *(SigIm + i) - *(SigIm + p);	// разность, Im
        *(SigRe + i) = SumRe;	// результат по младшему индексу, не умножается
        *(SigIm + i) = SumIm;
        *(SigRe + p) = DifRe * Wre - DifIm * Wim;	// старш. индекс, * W
        *(SigIm + p) = DifRe * Wim + DifIm * Wre;
      }
      // новый поворачивающий множитель
      float Wtm = Wre * Ac - Wim * As;	// новые cos и sin
      Wim = Wim * Ac + Wre * As;
      Wre = Wtm;
    }
    step /= 2;			// новая стадия
  }
  // Перестановка результатов (крайние точки уже на своих местах)
  for (uint32_t i = 1; i < FFT_N - 1; i++) {
    uint32_t j = (__rbit(i)) >> (32 - FFT_P);
    if (i < j) {		// чтобы избежать двойной перестановки
      float TmRe = *(SigRe + j);
      float TmIm = *(SigIm + j);
      *(SigRe + j) = *(SigRe + i);
      *(SigIm + j) = *(SigIm + i);
      *(SigRe + i) = TmRe;
      *(SigIm + i) = TmIm;
    }
  }

/* Вычислить модуль (амплитуду) спектра */
/* Поделить на FFT_N и коэффициент ослабления оконной функции */

  pSigRe = SigRe;		// установить указатель в начало
  pSigIm = SigIm;		// установить указатель в начало
  for (uint32_t i = FFT_N; i--; ) {
     // *pSigRe = sqrt(*pSigRe * *pSigRe + *pSigIm * *pSigIm);
     *pSigRe = hypotf(*pSigRe, *pSigIm);
     *pSigRe /= FFT_Att[MathCtrl.Wind];
     pSigRe++;
     pSigIm++;
  }
  SigRe[0] /= 2;		// постоянная составляющая
  
/* Вычислить логарифм модуля (рисовать в dB)
   Значение ограничивается снизу */
  pSigRe = SigRe;
  for (uint32_t i = FFT_N; i--; ) {
    *pSigRe++ = log2f(fmax(*pSigRe, 0.015625)) * 32;
  }

}

/*!*****************************************************************************
 @brief		FFT draw
 @note		
 */
void FFT_draw(bool bright) {
  uint32_t width = DispCtrl.Wide ? 300 : 250;
  float *pSigRe = SigRe;	// pointer to FFT array (module)
  int32_t s1, s2, brk;		// previous & next samples FFT, break position
/* Цикл между первым вектором и последним */
  for (int32_t i = 0; i < width; i++) {
    s1 = (int32_t)(*pSigRe++) / 2 + 128;	// integer part, begin
    s2 = (int32_t)(*pSigRe) / 2 + 128;
    // brk = (s1 == s2) ? s1 : ((s1 > s2) ? (s1 - 1) : (s1 + 1));
    brk = (s1 + s2) >> 1;
    VectMath_draw(s1, brk, s2, i, bright);
  }
}


/*! @} */
