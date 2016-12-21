
/*!*****************************************************************************
 @file		Siluet_Interface.h
 @author	Козлячков В.К.
 @date		2012.10
 @version	V0.5 
 @brief		Header for Interface.c
 @details	Заголовочный файл для работы с интерфейсными функциями
 @note		
 *******************************************************************************/

/*! @addtogroup	Interface
    @{ */

#ifndef  INTERFACE_H		// Убедиться, что данный файл включен один раз
#define  INTERFACE_H

/*__ Заголовочные файлы _______________________________________________________*/


/*__ Глобальные типы данных ___________________________________________________*/


/*__ Макроопределения _________________________________________________________*/

// #define VOLTCNT		12	// Количество шагов Вольт/дел (5V..1mV)
// #define VOLTCNT		13	// Количество шагов Вольт/дел
#define VOLT_BEG	4	// Начало диапазона Вольт/дел (20V)
#define VOLT_DEFAULT	10	// Вольт/дел по умолчанию (0.2V)
#define VOLT_END	16	// Конец диапазона Вольт/дел (2mV)

// #define TIME_CNT	33		// Количество шагов Время/дел (50s..1ns)
// #define TIME_CNT	34		// Количество шагов Время/дел (50s..0.5ns)
#define TIME_CNT	36	// Количество шагов Время/дел (50s..0.1ns)
#define TIME_DEFAULT	15	// Развертка по умолчанию (0.5ms / div)
// #define TIME_DOT	5		// Максимально быстрая развертка по точкам (1s)
#define TIME_DOT	6	// Последняя развертка по точкам (0.5s / div)
#define TIME_SPR	27	// Первая развертка по разреженному кадру (50ns)


/*__ Глобальные переменные ____________________________________________________*/

/*! Указатель на функцию - обработчик панели управления */
typedef void (*pPanel_t)(int32_t Cnt);

/*! Указатель на функцию - обработчик элемента МЕНЮ
    такой же тип имеет функция индикации элемента МЕНЮ */
typedef void (*pMenu_t)(void);

/*! Указатель на функцию - обработчик ручки УСТАНОВКА */
typedef void (*pSet_t)(int32_t Cnt);

/*! Указатель на функцию - обработчик кнопки Выполнить */
typedef void (*pExe_t)(void);

/*! Номер функции МЕНЮ
    сервисное меню вызывается только после включения */
typedef enum {
  MENU_SERV, MENU_CHAN1, MENU_MATH, MENU_CHAN2,
  MENU_SWEEP, MENU_TRIG, MENU_DISP, MENU_ACQU,
  MENU_CURS, MENU_MEAS, MENU_MEM, MENU_UTIL
} MENU_t;

/*! Номер функции УСТАНОВКА */
typedef enum {
  SET_VOID, SET_PRET, SET_HOLD, SET_AVER,
  SET_CURS1, SET_CURS2, SET_CURSTR,
  SET_PAR1, SET_PAR2,
  SET_BUFNUM			// номер буфера памяти сигнала
} SET_t;

/*! Номер функции Выполнить */
typedef enum {
  EXE_VOID,
  EXE_CALIBR, EXE_CALSAVE,	// Калибровать интерполятор, сохранить  
  EXE_CPUIMEM, EXE_CPUEMEM,
  EXE_PLRECV, EXE_PLCOMP, EXE_PLWRITE, EXE_PLREAD,
  EXE_DPYWHITE, EXE_DPYBLACK, EXE_DPYFONT, EXE_DPYPALET,
  EXE_DPYRED, EXE_DPYGREEN, EXE_DPYBLUE, EXE_DPYDEFAULT
} EXE_t;

/*! Номер функции Выполнить в меню памяти */
typedef enum {
  SIG1SAVE, SIG2SAVE,
  SIG1LOAD, SIG2LOAD
} MEMEXE_t;

/*! Номер функции ПОМОЩЬ */
typedef enum {
  HELP_VOID, HELP_CHAN1, HELP_CHAN2, HELP_SWEEP, HELP_TRIG
} HELP_t;

/*! Номер математической функции */
typedef enum {
  MATH_SUM, MATH_MULT, MATH_FFT
} MATH_t;

/*! Окна БПФ */
typedef enum {
  WIND_RECT,
  WIND_HANN,
  WIND_HAMM,
  WIND_BART,
  WIND_FLAT
} Wind_t;

/*! Номер измеряемого параметра */
typedef enum {
  PAR_TIM, PAR_FRQ,
  PAR_WIN, PAR_WIP,
  PAR_TFA, PAR_TRI,
  PAR_MIN, PAR_MAX,
  PAR_AMP, PAR_AVR
} PAR_t;

/*! Перечисление Выключить/Включить, используется в однобитовых полях */
typedef enum {
  OFF,
  ON
} Mode_t;


/*! Структура интерфейса Панели управления */
typedef struct {
  uint8_t	First	: 1;	//!< Panel Message First Byte
  uint8_t	Ready	: 1;	//!< Panel Message Ready
  uint8_t	dummy1	: 6;
  uint8_t	TxBuf;		//!< Panel Transmit Buffer
  uint8_t	RcBuf[2];	//!< Panel Receive Buffer
} Pan_t		/*__attribute__((bitband))*/;	// С атрибутом - вылетает компилятор

#define PAN_INIT {0, 0, 0, 0x3F, 0, 0}
_DECL Pan_t PanCtrl _INIT(PAN_INIT);	// Front Panel Interface

/*! Структура интерфейсная общего назначения */
typedef struct {
  uint8_t	Lock	: 1;	//!< Front Panel Lock (only UTIL, HELP, F5)
  uint8_t	Menu	: 1;	//!< MENU Display
  uint8_t	Help	: 1;	//!< MENU Help Display
  uint8_t	Redr	: 1;	//!< Redraw Request
  uint8_t	Autoset	: 1;	//!< Autoset
  uint8_t	FPCal	: 1;	//!< Front Panel Calibrator On (0 - Off)
  uint8_t	Bal	: 1;	//!< Channels Balance
} Gen_t		/*__attribute__((bitband))*/;

#define GEN_INIT {0, 0, 0, 1, 0, 1}
_DECL Gen_t GenCtrl _INIT(GEN_INIT);

/*__ Структуры управления __*/

/*! Структура сервисных переменных */
typedef struct {
  uint8_t	Dump	: 1;	//!< отображать память
  uint8_t	Numb	: 1;	//!< отображать переменную
  uint8_t	Calibr	: 1;	//!< накапливать и отображать статистику
  uint8_t	CalNum;		//!< Номер сервисной функции калибровки
  uint8_t	CPUNum;		//!< номер сервисной функции Процессора
  uint8_t	PLNum;		//!< номер сервисной функции ПЛИС
  uint8_t	DpyNum;		//!< номер сервисной функции ЖКИ
} Serv_t	/*__attribute__((bitband))*/;

#define SERV_INIT {0, 1, 0, 0, 0, 0, 0}
_DECL Serv_t ServCtrl _INIT(SERV_INIT);

/*! Структура режимов КАНАЛ 1 и 2 */
typedef struct {
  uint8_t	On	: 1;	//!< Выключен / Включен
  uint8_t	Prb	: 2;	//!< Пробник 1/100, 1/10, 1/1, x10
  uint8_t	Inv	: 1;	//!< Инверсия
  uint8_t	BWLim	: 1;	//!< Ограничение полосы
  uint8_t	Cplg	: 2;	//!< 0 - Gnd, 1 - AC, 2 - DC
  uint8_t	dummy	: 1;
  uint8_t	VoltNum;	//!< Вольт/дел номер (4 - 0.2V/div)
  uint16_t	SigPos;		//!< Смещение по сигналу (0..1023) << 2
  int16_t	DispPos;	//!< Смещение по экрану (-256..[-200..0..+200]..+255)
} Chan_t	/*__attribute__((bitband))*/;

// #define CHAN1_INIT {ON, 2, OFF, OFF, 1, 0, VOLT_DEFAULT, 512 + 50, 0}
// _DECL Chan_t Chan1Ctrl _INIT(CHAN1_INIT);
_DECL Chan_t Chan1Ctrl __attribute((section("backup")));

// #define CHAN2_INIT {ON, 2, OFF, OFF, 1, 0, VOLT_DEFAULT, 512 - 50, 0}
// _DECL Chan_t Chan2Ctrl _INIT(CHAN2_INIT);
_DECL Chan_t Chan2Ctrl __attribute((section("backup")));

/* Структура коррекции смещения (баланса) */
typedef struct {
  int8_t	Ofs20DC;	//!< 20mV DC
  int8_t	Ofs20AC;	//!< 20mV AC
  int8_t	Ofs10DC;	//!< 10mV DC
  int8_t	Ofs10AC;	//!< 10mV AC
  int8_t	Ofs5DC;		//!<  5mV DC
  int8_t	Ofs5AC;		//!<  5mV AC
  int8_t	Ofs5GND;	//!<  5mV GND (10mV, 20mV)
  int8_t	Ofs2DC;		//!<  2mV DC
  int8_t	Ofs2AC;		//!<  2mV AC
  int8_t	Ofs2GND;	//!<  2mV GND
} Balance_t;

#define BAL_INIT {0, 0, 0, 0, 0, 0, 0, 0}
_DECL Balance_t Bal1Ctrl _INIT(BAL_INIT);
_DECL Balance_t Bal2Ctrl _INIT(BAL_INIT);

/*! Структура режимов МАТематическая ОБРаботка */
typedef struct {
  uint8_t	On	: 1;	//!< Выкл / Вкл
  uint8_t	Fun	: 2;	//!< функция: Сумма, Произведение, БПФ
  uint8_t	Sour	: 1;	//!< Источник для функции БПФ: Кан1, Кан2
  uint8_t	Wind	: 3;	//!< Окно для БПФ: Прям, Хэнн, Хэмм, Треуг, FlatTop
  uint8_t	dummy	: 1;
  uint8_t	Scale;		//!< Масштаб (целая/дробная части показателя степени)
  int16_t	Ofs;		//!< Смещение (±256)
} Math_t	/*__attribute__((bitband))*/;

// #define MATH_INIT {OFF, MATH_SUM, 0, WIND_HANN, 0, 0x10, 0}
// _DECL Math_t MathCtrl _INIT(MATH_INIT);
_DECL Math_t MathCtrl __attribute((section("backup")));

/*! Структура режимов РАЗВертка
    Предзапуск изменяется скачками, делит память на 32 фрагмента */
typedef struct {
  uint8_t	Mode	: 2;	//!< Режим: Авто, Ждущ, Однокр
  uint8_t	Roll	: 1;	//!< Регистратор (Самописец)
  uint8_t	Stop	: 1;	//!< Stop sweep, Single Start 
  uint8_t	XY	: 1;	//!< X-Y
  uint8_t	dummy	: 3;
  uint8_t	TimeNum;	//!< Время/дел номер
  uint16_t	TimePos;	//!< Позиция по буферу сигналов - начало экрана
  uint16_t	Pret;		//!< Предзапуск (1/2 памяти по умолчанию)
  uint16_t	TimeItp;	//!< Коэффициент интерполяции на строб. разв.
} Sweep_t	/*__attribute__((bitband))*/;

// #define SWEEP_INIT {0, OFF, OFF, OFF, 0, 15, 1024 / 2 - 125, 0x100, 1}
// _DECL Sweep_t SweepCtrl _INIT(SWEEP_INIT);
_DECL Sweep_t SweepCtrl __attribute((section("backup")));

/*! Структура режимов СИНХРонизация */
typedef struct {
  uint8_t	Sour	: 2;	//!< Источник: Сеть, Кан1*, Кан2, Внеш
  uint8_t	Rise	: 1;	//!< Наклон: 0 - Fall, 1 - Rise
  uint8_t	Cplg	: 2;	//!< Связь (LF, DC*, AC, HF)
  uint8_t	NRej	: 1;	//!< Шумоподавление  
  uint8_t	dummy	: 1;
  uint8_t	TrigDet	: 1;	//!< Синхронизация есть 
  int16_t	Level;		//!< Уровень синхронизации (0..1023) << 2
} Trig_t	/*__attribute__((bitband))*/;

// #define TRIG_INIT {1, 1, 1, ON, 0, 0, 0x0200}
// _DECL Trig_t TrigCtrl _INIT(TRIG_INIT);
_DECL Trig_t TrigCtrl __attribute((section("backup")));

/*! Структура режимов ДИСПЛЕЙ */
typedef struct {
  uint8_t	HelpEna	: 1;	//!< Индикация помощи
  uint8_t	Lang	: 2;	//!< Язык
  uint8_t	Vect	: 1;	//!< Точки / Вектора
  uint8_t	Wide	: 1;	//!< WorkArea Wide
  uint8_t	Scale	: 2;	//!< Work Scale Form
  uint8_t	dummy	: 1;
} Disp_t	/*__attribute__((bitband))*/;

// #define DISP_INIT {OFF, 1, ON, ON, 3, 0}
// _DECL Disp_t DispCtrl _INIT(DISP_INIT);
_DECL Disp_t DispCtrl __attribute((section("backup")));

/*! Структура режимов СБОР ИНФ */
typedef struct {
  uint8_t	Filt	: 2;	//!< Фильтр Высокого разрешения
  uint8_t	Peak	: 1;	//!< Пиковый детектор
  uint8_t	Pers	: 1;	//!< Накопление (послесвечение)
  uint8_t	dummy1	: 4;
  uint8_t	Aver	: 6;	//!< Степень веса усреднения (поделить на 4)
  uint8_t	dummy2	: 2;
} Acqu_t	/*__attribute__((bitband))*/;

// #define ACQU_INIT {0, OFF, OFF, 0, 0, 0}
// _DECL Acqu_t AcquCtrl _INIT(ACQU_INIT);
_DECL Acqu_t AcquCtrl __attribute((section("backup")));

/*! Структура режимов КУРСОРЫ */
typedef struct {
  uint8_t	On	: 1;	//!< Включены
  uint8_t	xY	: 1;	//!< Параметр: 0 - dX, 1 - dY
  uint8_t	Sel	: 2;	//!< Выбор: 0 - оба (слежение), 1, 2
  uint8_t	dummy	: 4;
  uint16_t	CX1;		//!< Курсор 1 по X
  uint16_t	CX2;		//!< Курсор 2 по X
  uint8_t	CY1;		//!< Курсор 1 по Y
  uint8_t	CY2;		//!< Курсор 2 по Y
} Curs_t;

// #define CURS_INIT {OFF, OFF, 0, 0, 256 - 125, 256 + 125, 128 - 100, 128 + 100}
// _DECL Curs_t CursCtrl _INIT(CURS_INIT);
_DECL Curs_t CursCtrl __attribute((section("backup")));

/*! Структура режимов ИЗМЕРЕНИЯ */
typedef struct {
  uint8_t	On	: 1;	//!< Включены
  uint8_t	Sour	: 1;	//!< Источник: 0 - Канал 1, 1 - Канал 2  
  uint8_t	Par1	: 6;	//!< Параметр 1  
  uint8_t	dummy	: 2;
  uint8_t	Par2	: 6;	//!< Параметр 2
} Meas_t;

// #define MEAS_INIT {OFF, 0, 0 << 2, 0, 1 << 2}
// _DECL Meas_t MeasCtrl _INIT(MEAS_INIT);
_DECL Meas_t MeasCtrl __attribute((section("backup")));

/*! Структура режимов ПАМЯТЬ */
typedef struct {
  uint8_t	SizeNum	: 3;	//!< Размер кадра (0..4 => 1024 ... 16384)
  uint8_t	BufNum	: 5;	//!< номер буфера в памяти сигнала (0..15)
  uint8_t	SigZone	: 5;	//!< Зона сохранения сигнала
  uint8_t	SetZone	: 3;	//!< Зона сохранения установок
} Mem_t		/*__attribute__((bitband))*/;

// #define MEM_INIT {0, 0, 0, 0}
// _DECL Mem_t MemCtrl _INIT(MEM_INIT);
_DECL Mem_t MemCtrl __attribute((section("backup")));

#if false
/*! Суперструктура управления */
typedef struct {
  Chan_t Chan1Ctrl;
  Chan_t Chan2Ctrl;
  Math_t MathCtrl;
  Sweep_t SweepCtrl;
  Trig_t TrigCtrl;
  Disp_t DispCtrl;
  Acqu_t AcquCtrl;
  Curs_t CursCtrl;
  Meas_t MeasCtrl;
  Mem_t MemCtrl;
} Ctrl_t;

// _DECL Ctrl_t CtrlMain;
_DECL Ctrl_t CtrlBack __attribute((section("backup")));
#endif


/*__ Глобальные переменные ____________________________________________________*/

 
// _DECL MENU_t MenuNum _INIT(MENU_SERV);	//!< Текущий номер функции МЕНЮ
_DECL MENU_t MenuNum __attribute((section("backup")));
// _DECL SET_t SetNum _INIT(SET_VOID);	//!< Номер функции УСТАНОВКА
_DECL SET_t SetNum __attribute((section("backup")));
_DECL EXE_t ExeNum  _INIT(EXE_VOID);	//!< Номер функции Выполнить (СЕРВИС)
_DECL MEMEXE_t MemItem _INIT(SIG1SAVE);	//!< Номер функции Выполнить (ПАМЯТЬ)
_DECL HELP_t HelpNum _INIT(HELP_VOID);	//!< Номер функции ПОМОЩЬ

_DECL uint32_t CodeCnt _INIT(0);	//!< Счетчик кода загрузки ПЛИС
 
/*__ Прототипы экспортируемых функций _________________________________________*/

void Panel_ProcessTest(void);		// Front Panel Interface processing test
void Panel_Process(void);

void Chan1St_disp(void);
void Chan2St_disp(void);
void TimeSt_disp(void);
void SweepSt_disp(void);
void TrigSt_disp(void);

void StSp_disp(void);
void MemSizeBuf_disp(void);

void Num_draw(int32_t numb);		// Напечатать число (для проверки)
void Hex32_draw (uint32_t var);
void Dump_draw(uint8_t *mem);		// Напечатать содержимое памяти


/* Прототипы обработчиков органов панели управления */

void FpVolt1_hand(int32_t Cnt);			// Volts 1 / div
void FpVolt2_hand(int32_t Cnt);			// Volts 2 / div
void FpTime_hand(int32_t Cnt);			// Time / div
void FpSet_hand(int32_t Cnt);			// Setting
void FpPos1_hand(int32_t Cnt);			// Position 1
void FpPos2_hand(int32_t Cnt);			// Position 2
void FpPosT_hand(int32_t Cnt);			// Position Time
void FpLevel_hand(int32_t Cnt);			// Trigger Level
void FpChan1_hand(int32_t Cnt);		// Channel 1 Menu
void FpMath_hand(int32_t Cnt);		// Mathematic Menu
void FpChan2_hand(int32_t Cnt);		// Channel 2 Menu
void Fp11_hand(int32_t Cnt);
void FpSweep_hand(int32_t Cnt);		// Horizontal Sweeep Menu
void Fp13_hand(int32_t Cnt);		// Position Time Switch
void FpTrig_hand(int32_t Cnt);		// Trigger Menu
void Fp15_hand(int32_t Cnt);		// (Trigger Level Switch)
void FpDisp_hand(int32_t Cnt);		// Display Menu
void FpAcqu_hand(int32_t Cnt);		// Acquisition Menu
void FpCurs_hand(int32_t Cnt);		// Cursors Menu
void FpMeas_hand(int32_t Cnt);		// Measure Menu
void FpMem_hand(int32_t Cnt);		// Memory Menu
void FpUtil_hand(int32_t Cnt);		// Utilities Menu
void FpAutoset_hand(int32_t Cnt);		// Autoset
void FpStSp_hand(int32_t Cnt);			// Start / Stop
void FpHelp_hand(int32_t Cnt);			// Help
void FpF1_hand(int32_t Cnt);				// F1
void FpF2_hand(int32_t Cnt);				// F2
void FpF3_hand(int32_t Cnt);				// F3
void FpF4_hand(int32_t Cnt);				// F4
void FpF5_hand(int32_t Cnt);				// F5
void Fp30_hand(int32_t Cnt);
void Fp31_hand(int32_t Cnt);			// (Setting Switch)

/* Прототипы функций установки режимов
   Прототипы функций индикации режимов (в СТАТУСЕ) */

void Chan1Mode_sett(void);
void Chan1BWLim_sett(void);
void Chan1Cplg_sett(void);
void Chan1St_disp(void);
//
void Chan2Mode_sett(void);
void Chan2BWLim_sett(void);
void Chan2Cplg_sett(void);
void Chan2St_disp(void);
//
void MathSt_disp(void);
//
void Time_sett(void);
void TimeSt_disp(void);
//
void SweepMode_sett(void);
void SweepRoll_sett(void);
void SweepSt_disp(void);
//
void TrigSour_sett(void);
void TrigSlp_sett(void);
void TrigCplg_sett(void);
void TrigNRej_sett(void);
void TrigSt_disp(void);
//
void AcquFilt_sett(void);	// Установщик фильтра в ПЛИС
void AcquPeak_sett(void);	// Установщик пикового детектора 
//
void CursMeasSt_disp(void);	// Индикатор режима измерений в статусе
//
void MeasPar1St_disp(void);	// Индикатор параметра 1 в статусе
void MeasVal1St_disp(void);	// Индикатор значения 1 в статусе
void MeasPar2St_disp(void);	// Индикатор параметра 2 в статусе
void MeasVal2St_disp(void);	// Индикатор значения 2 в статусе
//
void MemSize_sett(void);	// установщик памяти в ПЛИС
 
/* Прототипы обработчиков ручки УСТАНОВКА
   Прототифы индикаторов ручки УСТАНОВКА
      __
    / > \
   |    |
   \___/
 */

void SetVoid_hand(int32_t Cnt);
void SetVoid_disp(void);
//
void SetPret_hand(int32_t Cnt);
void SetPret_disp(void);
//
void SetHold_hand(int32_t Cnt);
void SetHold_disp(void);
//
void SetAver_hand(int32_t Cnt);
//
void SetCurs1_hand(int32_t Cnt);
void SetCurs2_hand(int32_t Cnt);
void SetCursTr_hand(int32_t Cnt);
void SetCursSt_disp(void);		// Индицировать результат в статусе
//
void SetPar1_hand(int32_t Cnt);
void SetPar2_hand(int32_t Cnt);
//
void SetBufNum_hand(int32_t Cnt);

/* Прототипы установщиков режимов ручки УСТАНОВКА */

void SetPret_sett(void);
void SetHold_sett(void);



#endif				// INTERFACE_H

/*! @} */
