
/*!*****************************************************************************
 @file		Siluet_Signal.h
 @author	Козлячков В.К.
 @date		2012.10
 @version	V0.5 
 @brief		Signal Processing functions
 @details	Функции обработки и индикации сигнала
 @note		Память сигналов 2 х 10 х 16384, по 16K в каждом канале
 *******************************************************************************
 @verbatim
 2013.02
 Буфер сигналов длиной 16K содержит 10 разрядов в каждом канале.
 Для целой и дробной части нужно иметь uint16_t слова, всего 16K * 16 = 32 KB.
 Для двух каналов нужно 64 KB. 
 
 2011.11
 Заменил один общий буфер для каналов на два раздельных
 @endverbatim
 *******************************************************************************/

/*! @addtogroup	Signal
    @{ */
 
#ifndef SIGNAL_H		// Включить файл только один раз
#define SIGNAL_H


/*__ Заголовочные файлы _______________________________________________________*/


/*__ Глобальные определения констант __________________________________________*/

#define MAXWGT (1 << 14)	//!< Максимальный вес усреднения

#define FFT_P	10		// Степень размерности БПФ
#define FFT_N	(1 << FFT_P)	// Размерность БПФ
#define PI 3.1415926535897932384626433832795L


/*__ Глобальные макрофункции __________________________________________________*/


/*__ Глобальные типы данных ___________________________________________________*/


/*__ Глобальные переменные ____________________________________________________*/

/* Тип - Индекс функции обработки сигнала */
typedef enum {
  SWEEP_SKIP,
  ROLL,
  SCAN_PRE,
  SCAN_PST,
  FRAME_FULL,
  FRAME_SPARSE
} SIGNUM_t;

/*! Структура обработки сигнала
    Номер функции обработки сигнала
    Индекс точки в буфере (указывает на свободную точку кадра)
    Индексы начала и конца сигнала в буферах сигналов 1 и 2 (конец на 1 больше,
    чем последняя точка), привязаны к началу буфера (не к текущей точке)
    SigBuf[SigEnd] не выводить! */       
typedef struct {
  SIGNUM_t SigNum;		//!< Номер функции обработки сигнала
  uint16_t SigInx;		//!< Индекс свободной точки в буфере
  uint16_t SigBeg;		//!< Индекс начала сигнала в буфере
  uint16_t SigEnd;		//!< Индекс конца сигнала в буфере
} SIGCTRL_t;

/* Структура обработки сигнала */
// _DECL SIGCTRL_t SigCtrl _INIT({FRAME_FULL, 0, 1024});
// _DECL SIGCTRL_t SigCtrl _INIT((SIGCTRL_T){.SigNum = FRAME_FULL, .SigBeg = 0, .SigEnd = 1024});
// _DECL SIGCTRL_t SigCtrl _INIT({({FRAME_FULL, 0, 1024})});
// #define SIGINIT {FRAME_FULL, 0, 0, 511}
#define SIGINIT {FRAME_FULL, 0, 0, 1023}
_DECL SIGCTRL_t SigCtrl _INIT(SIGINIT);

/*! Структура интерполятора времени */
typedef struct {
  uint32_t Tmts;		//!< Среднее время trigger - second sample (<<16)
  uint32_t Tmad;		//!< Среднее абсолютное отклонение (<<16)
  uint16_t Tmin;		//!< Минимальное время trigger - second sample
  uint16_t Tmax;		//!< Максимальное время trigger - second sample  
  uint16_t Awgt;		//!< Вес усреднения времени
  uint16_t Tcur;		//!< Текущее время trigger - second sample
} INTERP_t;

#define ITPINIT {3030 << 16, 240 << 16, 3030 - 240, 3030 + 240, 1, 3030}
_DECL INTERP_t Interp _INIT(ITPINIT);



/*!*****************************************************************************
 @brief		Буферы сигналов разных количества и размеров
 @note		Для хранения дробной части при усреднениии нужны доп. массивы
		[MemCtrl.BufNum][Sweep.TimePos] (0..1<<(9+MemCtrl.SizeNum))
		Каждому сигналу - свой массив буферов!
 */
typedef union {
  // uint16_t BufK5[32][512];
  uint16_t Buf1K[16][1024];
  uint16_t Buf2K[8][2048];
  uint16_t Buf4K[4][4096];
  uint16_t Buf8K[2][8192];
  uint16_t Buf16K[16384];
} SigBuf_t;			// integer<<8 | fractional

/* Буферы сигналов */
_DECL SigBuf_t SigChan1 _INIT({0});
_DECL SigBuf_t SigChan2 _INIT({0});

// _DECL SigBuf_t __attribute__((at(0x20002000))) SigChan1 _INIT({0});
// _DECL SigBuf_t __attribute__((at(0x20006000))) SigChan2 _INIT({0});

/*! Счетчик кадров с неизменным режимом усреднения */
_DECL uint16_t FrameCnt _INIT(0);

/* Указатель на обработчик сигнала - по точкам..., по кадрам... */
// _DECL pFun_t Sig_fun;




/*__ Внешние объявления _______________________________________________________*/


/*__ Глобальные прототипы функций _____________________________________________*/
void Sweep_init(void);		// Sweep initialize
void Sweep_exe(void);		// Sweep execute
void Math_exe(void);		// Mathematics execute
void Sum_draw(bool On);		// Summa draw

void SigChans_draw(void);
void SigChans_redraw(void);
void PosChans_redraw(void);

void SigChan1_draw(uint32_t bright);
void SigChan2_draw(uint32_t bright);

void PosChan1_draw(bool On);
void PosChan2_draw(bool On);
void TrigLev_draw(bool On);

void Pret_draw(bool On);

void Curs_redraw(void);		// Перерисовать курсоры
void Curs_draw(bool On);	// Нарисовать (стереть) курсоры

void Sig_clear(void);		// Очистить память сигналов
void SigChanAC_load(void);
void SigChanDC_load(void);

// void SigChans_read(void);	// Читать память сигналов ПЛИС
void SigChans_rdavr(void);	// Read-average both channels, one or neither

void MemBar_draw(void);		// Нарисовать полосу памяти
void MemBarPret_draw(bool On);	// Нарисовать позицию предзапуска на полосе
void MemBarPos_draw(bool On);	// Нарисовать позицию экрана на полосе памяти

void SigPar_meas(void);

void XYDots_draw(bool bright);

/*__ Раздел ошибок ____________________________________________________________*/

//#error

#endif				// SIGNAL_H

/*! @} */
