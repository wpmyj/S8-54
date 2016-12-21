
/*!*****************************************************************************
 @file		Siluet_Interface.c
 @author	Козлячков В.К.
 @date		2012.10 - 2013.05
 @version	V0.5
 @brief		Interface functions
 @details	User interface functions
 @note		
 *******************************************************************************
 @verbatim
 Обработчик панели управления
   Протокол обмена
   Передача от панели: KNm KCt
   KNm - номер энкодера (0..7) или кнопки (8..31)
   KCt - код энкодера (-128..+127) или кнопки (-1, 1)
   Передача от MCU: ALC - код ответа и светодиодов aallllll
   aa - 00 - положительный ответ (ComBusy = 0) + код светодиодов
	01 - инициированная процессором передача кода светодиодов
	10 - ошибка, (ComBusy = 0), повторить (+ код светодиодов)
	11 - общая ошибка, (ComBusy = 0), сброс? (+ код светодиодов)
   llllll - _t_21p (синхр, канал2, канал1, питание) (Lights_n)
  
 Номера кнопок [0..31]
   0     1     2     3     4     5     6     7
   V1    V2    Tim   Set   Pos1  Pos2  PosT  Level
   Chan1 Math  Chan2 [11]  Sweep [13]  Trig  [15]
   Disp  Acqu  Curs  Meas  Mem   Util  Autos StSp
   Help  F1    F2    F3    F4    F5    [30]  [31]
   
 2013.04.17
 
 Имена функций:
   FpXXX_hand - обработчики органов панели управления
   MnXXX_hand - обработчики элементов Меню
   MnXXX_disp - индикаторы элементов Меню в Меню
   XXX_sett - установщики режимов в ПЛИС
   XXXSt_disp - индикаторы режимов в статусе
   SetXXX_hand - обработчик функции, заданной ручке Установка
   SetXXX_disp - индикатор функции, заданной ручке Установка
   SetXXX_sett - установщик режимов, заданных ручкой Установка
   ExeXXX_hand - обработчик функции, заданной кнопке Выполнить в Меню Сервис 
 
 Структура файла по функциям:
 
   Обработчик панели - вызывает по указателю функции
   
     Энкодеры
     Кнопки, включающие МЕНЮ
     Кнопки без МЕНЮ
     
     Кнопки, обслуживающие МЕНЮ (Функциональные F1 - F5)
     Обработчики элементов в МЕНЮ, Индикаторы переменных в МЕНЮ
     
       Обработчики ручки УСТАНОВКА, Индикаторы ручки УСТАНОВКА
       Обработчики кнопки Выполнить (В меню СЕРВИС)
     
       Установщики режимов, Индикаторы режимов в области Состояния
       Установщики ручки УСТАНОВКА
 
 @endverbatim
 *******************************************************************************/

/*! @addtogroup	Interface
    @{ */
 
/*__ Заголовочный файл ________________________________________________________*/

#include "Siluet_Main.h"	// все включения заданы в одном файле

/*__ Локальные типы данных ____________________________________________________*/

/*__ Определения констант _____________________________________________________*/

/*__ Локальные прототипы функций ______________________________________________*/

// /* Прототипы обработчиков органов панели управления */

// void FpVolt1_hand(int32_t Cnt);			// Volts 1 / div
// void FpVolt2_hand(int32_t Cnt);			// Volts 2 / div
// void FpTime_hand(int32_t Cnt);			// Time / div
// void FpSet_hand(int32_t Cnt);			// Setting
// void FpPos1_hand(int32_t Cnt);			// Position 1
// void FpPos2_hand(int32_t Cnt);			// Position 2
// void FpPosT_hand(int32_t Cnt);			// Position Time
// void FpLevel_hand(int32_t Cnt);			// Trigger Level
// void FpChan1_hand(int32_t Cnt);		// Channel 1 Menu
// void FpMath_hand(int32_t Cnt);		// Mathematic Menu
// void FpChan2_hand(int32_t Cnt);		// Channel 2 Menu
// void Fp11_hand(int32_t Cnt);
// void FpSweep_hand(int32_t Cnt);		// Horizontal Sweeep Menu
// void Fp13_hand(int32_t Cnt);		// Position Time Switch
// void FpTrig_hand(int32_t Cnt);		// Trigger Menu
// void Fp15_hand(int32_t Cnt);		// (Trigger Level Switch)
// void FpDisp_hand(int32_t Cnt);		// Display Menu
// void FpAcqu_hand(int32_t Cnt);		// Acquisition Menu
// void FpCurs_hand(int32_t Cnt);		// Cursors Menu
// void FpMeas_hand(int32_t Cnt);		// Measure Menu
// void FpMem_hand(int32_t Cnt);		// Memory Menu
// void FpUtil_hand(int32_t Cnt);		// Utilities Menu
// void FpAutoset_hand(int32_t Cnt);		// Autoset
// void FpStSp_hand(int32_t Cnt);			// Start / Stop
// void FpHelp_hand(int32_t Cnt);			// Help
// void FpF1_hand(int32_t Cnt);				// F1
// void FpF2_hand(int32_t Cnt);				// F2
// void FpF3_hand(int32_t Cnt);				// F3
// void FpF4_hand(int32_t Cnt);				// F4
// void FpF5_hand(int32_t Cnt);				// F5
// void Fp30_hand(int32_t Cnt);
// void Fp31_hand(int32_t Cnt);			// (Setting Switch)

/* Прототипы обработчиков элементов МЕНЮ
   Прототипы индикаторов элементов МЕНЮ */

void MnVoid_hand(void);
void MnVoid_disp(void);

// Сервис
void MnServCal_hand(void);
void MnServCal_disp(void);
void MnServCPU_hand(void);
void MnServCPU_disp(void);
void MnServPL_hand(void);
void MnServPL_disp(void);
void MnServDpy_hand(void);
void MnServDpy_disp(void);
void MnServExe_hand(void);
void MnServExe_disp(void);

// Канал 1
void MnChan1Mode_hand(void);
void MnChan1Mode_disp(void);
void MnChan1Prb_hand(void);
void MnChan1Prb_disp(void);
void MnChan1Inv_hand(void);
void MnChan1Inv_disp(void);
void MnChan1Cplg_hand(void);
void MnChan1Cplg_disp(void);
void MnChan1BWLim_hand(void);
void MnChan1BWLim_disp(void);
// void MnChan1Gnd_hand(void);
// void MnChan1Gnd_disp(void);

// Канал 2
void MnChan2Mode_hand(void);
void MnChan2Mode_disp(void);
void MnChan2Prb_hand(void);
void MnChan2Prb_disp(void);
void MnChan2Inv_hand(void);
void MnChan2Inv_disp(void);
void MnChan2Cplg_hand(void);
void MnChan2Cplg_disp(void);
void MnChan2BWLim_hand(void);
void MnChan2BWLim_disp(void);
// void MnChan2Gnd_hand(void);
// void MnChan2Gnd_disp(void);

// Математика
void MnMathMode_hand(void);
void MnMathMode_disp(void);
void MnMathFun_hand(void);
void MnMathFun_disp(void);
void MnMathSour_hand(void);
void MnMathSour_disp(void);
void MnMathWind_hand(void);
void MnMathWind_disp(void);
void MnMathOffs_hand(void);
void MnMathOffs_disp(void);

// Развертка
void MnSweepMode_hand(void);
void MnSweepMode_disp(void);
void MnSweepRoll_hand(void);
void MnSweepRoll_disp(void);
void MnSweepPret_hand(void);
void MnSweepPret_disp(void);
void MnSweepXY_hand(void);
void MnSweepXY_disp(void);

// Синхронизация
void MnTrigSour_hand(void);
void MnTrigSour_disp(void);
void MnTrigSlp_hand(void);
void MnTrigSlp_disp(void);
void MnTrigCplg_hand(void);
void MnTrigCplg_disp(void);
void MnTrigNRej_hand(void);
void MnTrigNRej_disp(void);

// Дисплей
void MnDispHelp_hand(void);
void MnDispHelp_disp(void);
void MnDispVect_hand(void);
void MnDispVect_disp(void);
void MnDispScale_hand(void);
void MnDispScale_disp(void);
void MnDispClear_hand(void);
void MnDispClear_disp(void);

// Сбор информации
void MnAcquFilt_hand(void);	// Фильтр (Высокого разрешения)
void MnAcquFilt_disp(void);
void MnAcquPeak_hand(void);	// Пиковый детектор
void MnAcquPeak_disp(void);
void MnAcquAver_hand(void);	// Усреднение
void MnAcquAver_disp(void);
void MnAcquPers_hand(void);	// Накопление (послесвечение)
void MnAcquPers_disp(void);

// Курсоры
void MnCursMode_hand(void);
void MnCursMode_disp(void);
void MnCursPara_hand(void);
void MnCursPara_disp(void);
void MnCursSel1_hand(void);
void MnCursSel1_disp(void);
void MnCursSel2_hand(void);
void MnCursSel2_disp(void);
void MnCursSelTr_hand(void);
void MnCursSelTr_disp(void);

// Измерения
void MnMeasMode_hand(void);
void MnMeasMode_disp(void);
void MnMeasSour_hand(void);
void MnMeasSour_disp(void);
void MnMeasPar1_hand(void);
void MnMeasPar1_disp(void);
void MnMeasPar2_hand(void);
void MnMeasPar2_disp(void);

// Память
void MnMemSize_hand(void);
void MnMemSize_disp(void);
void MnMemNum_hand(void);
void MnMemNum_disp(void);
void MnMemSig_hand(void);
void MnMemSig_disp(void);
void MnMemSet_hand(void);
void MnMemSet_disp(void);
void MnMemExe_hand(void);
void MnMemExe_disp(void);

// Утилиты
void MnUtilCal_hand(void);
void MnUtilCal_disp(void);
void MnUtilBal_hand(void);
void MnUtilBal_disp(void);
void MnUtilReset_hand(void);
void MnUtilReset_disp(void);
void MnUtilVers_hand(void);
void MnUtilVers_disp(void);


/* Прототипы функций установки режимов
   Прототипы функций индикации режимов (в СТАТУСЕ)
   перемещены в отдельный заголовочный файл Siluet_Interface.h */

/* Прототипы установщиков режимов ручки УСТАНОВКА */

void SetPret_sett(void);
void SetHold_sett(void);

/* Прототипы обработчиков кнопки Выполнить (СЕРВИС) */

void ExeVoid_hand(void);
void ExeCalibr_hand(void);	// Калибровать интерполятор
void ExeCalSave_hand(void);	// Сохранить результаты калибровки
void ExeCPUiMem_hand(void);	
void ExeCPUeMem_hand(void);
void ExePLRecv_hand(void);
void ExePLComp_hand(void);
void ExePLWrite_hand(void);
void ExePLRead_hand(void);
void ExeDpyWhite_hand(void);
void ExeDpyBlack_hand(void);
void ExeDpyFont_hand(void);
void ExeDpyPalet_hand(void);
void ExeDpyRed_hand(void);
void ExeDpyGreen_hand(void);
void ExeDpyBlue_hand(void);
void ExeDpyDefault_hand(void);

/* Прототипы функций работы с памятью */
// Сохранить сигнал в одной из 2-х зон
void Sig1_save(void);
void Sig2_save(void);
// Загрузить сигнал из одной из 2-х зон
void Sig1_load(void);
void Sig2_load(void);

/* Прототипы индикаторов ПУСК/СТОП, АВТОУСТ */

void StSp_disp(void);
void Autoset_disp(void);

/* Остальные прототипы, вспомогательные функции */

void MenuHead_disp(void);	// Нарисовать МЕНЮ с нажатой кнопкой
void MenuHeadDepr_disp(void);	// Нарисовать отпущенную кнопку меню
void Help_draw(void);		// Нарисовать Помощь
void DigitBar_disp(void);
void HelpMenu_clear(void);	// Очистить экран или только МЕНЮ

void DpyOffOn_draw(uint8_t *pDpy, bool On);	// Написать Выкл / Вкл
void DpySetting_draw(uint8_t *pDpy, bool On);	// Нарисовать Установку и крутелку
void DpyRotor_draw(uint8_t *pDpy, bool On);	// нарисовать Крутелку

void DpyChanGnd_draw(uint8_t *pDpy);		// Написать заземление
void DpyChanCplg_draw(uint32_t Cplg);		// Написать Связь в канале
void DpyChanPrb_draw(uint32_t Prb);		// Написать Пробник в канале
uint32_t DpyVolt_draw(uint8_t *pDpy, uint32_t Num, uint32_t Color);	// Volt/div

void itoad_conv(int32_t numb, uint8_t *str);
void itoah_conv(uint32_t numb, uint8_t *str);


/*__ Локальные переменные (в пределах файла - глобальные) _____________________*/


/* Массивы функций - обработчиков и индикаторов элементов МЕНЮ
   Номера МЕНЮ: (сервисное меню вызывается только после включения прибора)
   MenuServ, MenuChan1, MenuChan2, MenuSweep, MenuTrig,
   MenuDisp, MenuAcq, MenuCurs, MenuMeas, MenuMem, MenuUtil,  */
   
/* Массив указателей на функции - обработчики элементов МЕНЮ */
static const pMenu_t MenuItem_hand[][5] = {
  MnServCal_hand, MnServCPU_hand, MnServPL_hand, MnServDpy_hand, MnServExe_hand,
  MnChan1Mode_hand, MnChan1Prb_hand, MnChan1Inv_hand, MnChan1Cplg_hand, MnChan1BWLim_hand,
  MnMathMode_hand, MnMathFun_hand, MnMathSour_hand, MnMathWind_hand, MnMathOffs_hand,
  MnChan2Mode_hand, MnChan2Prb_hand, MnChan2Inv_hand, MnChan2Cplg_hand, MnChan2BWLim_hand,
  MnSweepMode_hand, MnSweepRoll_hand, MnSweepPret_hand, MnVoid_hand, MnSweepXY_hand,
  MnTrigSour_hand, MnTrigSlp_hand, MnVoid_hand, MnTrigCplg_hand, MnTrigNRej_hand,
  MnDispHelp_hand, MnDispVect_hand, MnDispScale_hand, MnVoid_hand, MnDispClear_hand,
  MnAcquFilt_hand, MnAcquPeak_hand, MnAcquAver_hand, MnAcquPers_hand, MnVoid_hand,
  MnCursMode_hand, MnCursPara_hand, MnCursSel1_hand, MnCursSel2_hand, MnCursSelTr_hand,
  MnMeasMode_hand, MnMeasSour_hand, MnMeasPar1_hand, MnMeasPar2_hand, MnVoid_hand,
  MnMemSize_hand, MnMemNum_hand, MnMemSig_hand, MnMemSet_hand, MnMemExe_hand,
  MnUtilCal_hand, MnUtilBal_hand, MnVoid_hand, MnUtilVers_hand, MnUtilReset_hand  
};

/* Массив указателей на функции индикации значений элементов МЕНЮ */
static const pMenu_t MenuValue_disp[][5] = {
  MnServCal_disp, MnServCPU_disp, MnServPL_disp, MnServDpy_disp, MnServExe_disp,
  MnChan1Mode_disp, MnChan1Prb_disp, MnChan1Inv_disp, MnChan1Cplg_disp, MnChan1BWLim_disp,
  MnMathMode_disp, MnMathFun_disp, MnMathSour_disp, MnMathWind_disp, MnMathOffs_disp,
  MnChan2Mode_disp, MnChan2Prb_disp, MnChan2Inv_disp, MnChan2Cplg_disp, MnChan2BWLim_disp,
  MnSweepMode_disp, MnSweepRoll_disp, MnSweepPret_disp, MnVoid_disp, MnSweepXY_disp,
  MnTrigSour_disp, MnTrigSlp_disp, MnVoid_disp, MnTrigCplg_disp, MnTrigNRej_disp,
  MnDispHelp_disp, MnDispVect_disp, MnDispScale_disp, MnVoid_disp, MnDispClear_disp, 
  MnAcquFilt_disp, MnAcquPeak_disp, MnAcquAver_disp, MnAcquPers_disp, MnVoid_disp,
  MnCursMode_disp, MnCursPara_disp, MnCursSel1_disp, MnCursSel2_disp, MnCursSelTr_disp,
  MnMeasMode_disp, MnMeasSour_disp, MnMeasPar1_disp, MnMeasPar2_disp, MnVoid_disp,
  MnMemSize_disp, MnMemNum_disp, MnMemSig_disp, MnMemSet_disp, MnMemExe_disp,
  MnUtilCal_disp, MnUtilBal_disp, MnVoid_disp, MnUtilVers_disp, MnUtilReset_disp
};

/* Массив указателей на функции - обработчики ручки УСТАНОВКА */
static const pSet_t SetItem_hand[] = {
  SetVoid_hand, SetPret_hand, SetHold_hand, SetAver_hand,
  SetCurs1_hand, SetCurs2_hand, SetCursTr_hand,
  SetPar1_hand, SetPar2_hand,
  SetBufNum_hand  
};

/* Массив указателей на функции - обработчики кнопки Выполнить (СЕРВИС) */
static const pExe_t ExeItem_hand[] = {
  ExeVoid_hand,
  ExeCalibr_hand, ExeCalSave_hand,  
  ExeCPUiMem_hand, ExeCPUeMem_hand,
  ExePLRecv_hand, ExePLComp_hand, ExePLWrite_hand, ExePLRead_hand,
  ExeDpyWhite_hand, ExeDpyBlack_hand, ExeDpyFont_hand, ExeDpyPalet_hand,
  ExeDpyRed_hand, ExeDpyGreen_hand, ExeDpyBlue_hand, ExeDpyDefault_hand
};

/* Массив указателей на функции - обработчики кнопки Выполнить меню ПАМЯТЬ */ 
static const pExe_t MemFun[] = { 
  Sig1_save, Sig2_save, Sig1_load, Sig2_load
};

/*__ Глобальные функции _______________________________________________________*/

/*!*****************************************************************************
 @brief		Front Panel Processing
 @details	Обработка сообщения от панели управления
 @note		
 */
void Panel_Process(void) {
/* Массив указателей на обработчики клавиатурных органов управления */
  static const pPanel_t FpKey_hand[32] = {
    FpVolt1_hand, FpVolt2_hand, FpTime_hand, FpSet_hand,
    FpPos1_hand, FpPos2_hand, FpPosT_hand, FpLevel_hand,
    FpChan1_hand, FpMath_hand, FpChan2_hand, Fp11_hand,
    FpSweep_hand, Fp13_hand, FpTrig_hand, Fp15_hand,
    FpDisp_hand, FpAcqu_hand, FpCurs_hand, FpMeas_hand,
    FpMem_hand, FpUtil_hand, FpAutoset_hand, FpStSp_hand,
    FpHelp_hand, FpF1_hand, FpF2_hand, FpF3_hand,
    FpF4_hand, FpF5_hand, Fp30_hand, Fp31_hand
  };

  uint32_t KeyNum = (uint32_t)PanCtrl.RcBuf[0];
  int32_t KeyCnt = (int8_t)PanCtrl.RcBuf[1];
  
/* прогрессивная плавная регулировка */  
  if (KeyCnt >  2)  KeyCnt =   1 << ( KeyCnt - 1);
  if (KeyCnt < -2)  KeyCnt = -(1 << (-KeyCnt - 1));
  //  if (KeyCnt >  3) KeyCnt =   1 << ( KeyCnt - 2);
  //  if (KeyCnt < -3) KeyCnt = -(1 << (-KeyCnt - 2));
  
/* Вызвать обработчик по кнопке (энкодеру) */
  FpKey_hand[KeyNum](KeyCnt);
}


/*__ Локальные функции ________________________________________________________*/

/* Обработчики клавиатурных органов управления:
   Энкодеры
   Кнопки, вызывающие меню
   Кнопки, выполняющие функции меню
   Кнопки, выполняющие непосредственные функции */


//__ Энкодеры _________________________________________________________________//

 
/*!*****************************************************************************
 @brief	
 @details	ВОЛЬТ/ДЕЛ 1
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 */
void FpVolt1_hand(int32_t Cnt) {
  if (!Chan1Ctrl.On) return;
  int32_t temp = (int32_t)Chan1Ctrl.VoltNum;
  temp += Cnt;
  if (temp < VOLT_BEG) temp = VOLT_BEG;
  if (temp > VOLT_END) temp = VOLT_END;
  Chan1Ctrl.VoltNum = temp;
  Chan1Cplg_sett();
  FpPos1_hand(0);		// Скорректировать позицию    
  Chan1St_disp();
  FrameCnt = 0;			// Очистить счетчик кадров с неизменным режимом
  if (CursCtrl.On) SetCursSt_disp();
}

/*!*****************************************************************************
 @brief	
 @details	ВОЛЬТ/ДЕЛ 2
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 */
void FpVolt2_hand(int32_t Cnt) {
  if (!Chan2Ctrl.On) return;
  int32_t temp = (int32_t)Chan2Ctrl.VoltNum;
  temp += Cnt; 
  if (temp < VOLT_BEG) temp = VOLT_BEG;
  if (temp > VOLT_END) temp = VOLT_END;
  Chan2Ctrl.VoltNum = temp;
  Chan2Cplg_sett();
  FpPos2_hand(0);		// Скорректировать позицию
  Chan2St_disp();
  FrameCnt = 0;			// Очистить счетчик кадров с неизменным режимом
  if (CursCtrl.On) SetCursSt_disp();
}

/*!*****************************************************************************
 @brief	
 @details	ВРЕМЯ/ДЕЛ
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 */
void FpTime_hand(int32_t Cnt) {

/* Прочитать, изменить, ограничить, записать Время/дел */
  int32_t temp = (int32_t)SweepCtrl.TimeNum;
  temp += Cnt;
  if (temp < 0) temp = 0;
  if (temp >= TIME_CNT) temp = TIME_CNT - 1;
  SweepCtrl.TimeNum = temp;
  Time_sett();
  TimeSt_disp();
  SweepSt_disp();
  
/* Задать прерывание по кадрам или по точкам (1sec/div и медленнее) */
  PL_AcquBuf &= ~(PL_ACQU_FRMIE | PL_ACQU_DOTIE);
  if (temp <= TIME_DOT)	PL_AcquBuf |= PL_ACQU_DOTIE;
  else			PL_AcquBuf |= PL_ACQU_FRMIE;
  PL->ACQU = PL_AcquBuf;
  
/* Скорректировать предзапуск на быстрых развертках */
  SetPret_sett();
  // Hex32_draw(PL_AcquBuf);
  
/* Выбрать функцию обработки */
  // if (Chan1Ctrl.On) SigChan1_draw(0);	// clear old wave
  // if (Chan2Ctrl.On) SigChan2_draw(0);	// clear old wave
  DpyWork_clear(MASK_MATSIG);
  Sweep_init();
  FrameCnt = 0;			// Очистить счетчик кадров с неизменным режимом
  if (CursCtrl.On) SetCursSt_disp();
}

/*!*****************************************************************************
 @brief	
 @details	УСТАНОВКА
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 */
void FpSet_hand(int32_t Cnt) {
  SetItem_hand[SetNum](Cnt);
}

/*!*****************************************************************************
 @brief		Position Channel 1
 @details	КАНАЛ 1, Смещение
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 @verbatim
 Смещение сигнала добавляется в ЦАП канала, в диапазоне 0..0x3ff (1024 уровня).
 В ЦАП задвигается сдвинутое на 2 разряда смещение (0..0xffc, с шагом 4).
 Смещение индикации добавляется к изображению сигнала,
 диапазон -512..+511, шаг 1 (1024 уровня)
 Предел обоих смещений определяется смещением сигнала  
 Смещение индикации делится на 2 при суммировании с сигналом,
 диапазон -256..+255 (512 уровней)
 Когда приходит новый кадр, смещение индикации устанавливается в середину (0).
 @endverbatim
 */
void FpPos1_hand(int32_t Cnt) {
  if (!Chan1Ctrl.On) return;
  int32_t temp;				// temporal level
  SigChan1_draw(0);			// clear signal image (old DispPos)
  PosChan1_draw(false);			// clear pos marker
  TrigLev_draw(false);
  temp = (int32_t)Chan1Ctrl.SigPos + Cnt;	// desired level
  if (temp < 0) Cnt += (0 - temp);		// min limit (0), incr Cnt
  if (temp > 1023) Cnt -= (temp - 1023);	// max limit (1023), decr Cnt
  Chan1Ctrl.SigPos += Cnt;			// add limited counter
  Chan1Ctrl.DispPos += Cnt;			// add limited counter

//  ANADAC(DAC_CHAN1, Chan1Ctrl.SigPos << 2);
  uint16_t pos = Chan1Ctrl.SigPos;
  if (Chan1Ctrl.VoltNum == VOLT_END) {	// 2mV
    if (Chan1Ctrl.Cplg == 2)
      pos += Bal1Ctrl.Ofs2DC;
    if (Chan1Ctrl.Cplg == 1)
      pos += Bal1Ctrl.Ofs2AC;
    if (Chan1Ctrl.Cplg == 0)
      pos += Bal1Ctrl.Ofs2GND;
  }
  if (Chan1Ctrl.VoltNum % 3 == 0) {	// 5mV
    if (Chan1Ctrl.Cplg == 2)
      pos += Bal1Ctrl.Ofs5DC;
    if (Chan1Ctrl.Cplg == 1)
      pos += Bal1Ctrl.Ofs5AC;
    if (Chan1Ctrl.Cplg == 0)
      pos += Bal1Ctrl.Ofs5GND;
  }
  if (Chan1Ctrl.VoltNum % 3 == 2) {	// 10mV
    if (Chan1Ctrl.Cplg == 2)
      pos += Bal1Ctrl.Ofs10DC;
    if (Chan1Ctrl.Cplg == 1)
      pos += Bal1Ctrl.Ofs10AC;
    if (Chan1Ctrl.Cplg == 0)
      pos += Bal1Ctrl.Ofs5GND;
  }
  if (Chan1Ctrl.VoltNum % 3 == 1) {	// 20mV
    if (Chan1Ctrl.Cplg == 2)
      pos += Bal1Ctrl.Ofs20DC;
    if (Chan1Ctrl.Cplg == 1)
      pos += Bal1Ctrl.Ofs20AC;
    if (Chan1Ctrl.Cplg == 0)
      pos += Bal1Ctrl.Ofs5GND;
  }
  
  ANADAC(DAC_CHAN1, pos << 2);
  //  Num_draw(pos);  
  
  SigChan1_draw(3);			// draw signal image (new DispPos)
  PosChan1_draw(true);			// draw pos marker
  TrigLev_draw(true);
}

/*!*****************************************************************************
 @brief		Position Channel 2
 @details	КАНАЛ 2, Смещение
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpPos2_hand(int32_t Cnt) {
  if (!Chan2Ctrl.On) return;
  int32_t temp;
  SigChan2_draw(0);
  PosChan2_draw(false);
  TrigLev_draw(false);
  temp = (int32_t)Chan2Ctrl.SigPos + Cnt;
  if (temp < 0) Cnt += (0 - temp);
  if (temp > 1023) Cnt -= (temp - 1023);
  Chan2Ctrl.SigPos += Cnt;
  Chan2Ctrl.DispPos += Cnt;
  
//  ANADAC(DAC_CHAN2, Chan2Ctrl.SigPos << 2);
  uint16_t pos = Chan2Ctrl.SigPos;
  if (Chan2Ctrl.VoltNum == VOLT_END) {	// 2mV
    if (Chan2Ctrl.Cplg == 2)
      pos += Bal2Ctrl.Ofs2DC;
    if (Chan2Ctrl.Cplg == 1)
      pos += Bal2Ctrl.Ofs2AC;
    if (Chan2Ctrl.Cplg == 0)
      pos += Bal2Ctrl.Ofs2GND;
  }
  if (Chan2Ctrl.VoltNum % 3 == 0) {	// 5mV
    if (Chan2Ctrl.Cplg == 2)
      pos += Bal2Ctrl.Ofs5DC;
    if (Chan2Ctrl.Cplg == 1)
      pos += Bal2Ctrl.Ofs5AC;
    if (Chan2Ctrl.Cplg == 0)
      pos += Bal2Ctrl.Ofs5GND;
  }
  if (Chan2Ctrl.VoltNum % 3 == 2) {	// 10mV
    if (Chan2Ctrl.Cplg == 2)
      pos += Bal2Ctrl.Ofs10DC;
    if (Chan2Ctrl.Cplg == 1)
      pos += Bal2Ctrl.Ofs10AC;
    if (Chan2Ctrl.Cplg == 0)
      pos += Bal2Ctrl.Ofs5GND;
  }
  if (Chan2Ctrl.VoltNum % 3 == 1) {	// 20mV
    if (Chan2Ctrl.Cplg == 2)
      pos += Bal2Ctrl.Ofs20DC;
    if (Chan2Ctrl.Cplg == 1)
      pos += Bal2Ctrl.Ofs20AC;
    if (Chan2Ctrl.Cplg == 0)
      pos += Bal2Ctrl.Ofs5GND;
  }
 
  ANADAC(DAC_CHAN2, pos << 2);
  //  Num_draw(pos);  
  
  SigChan2_draw(3);
  PosChan2_draw(true);
  TrigLev_draw(true);
}

/*!*****************************************************************************
 @brief		Time Position
 @details	РАЗВЕРТКА, ПОЗИЦИЯ
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note	
 @verbatim
 Позиция указывает на начало экрана
 Позиция в буфере сигнала изначально установлена так, чтобы в середине экрана
 при 10 делениях была середина памяти.
 Там же находится позиция предзапуска (256 при кадре 0.5K)
	После того, как изображение формируется в ExtDpy памяти
	функция выполняется за 3.2ms, а мин. интервал появления 23.2ms 
 @endverbatim
 */
void FpPosT_hand(int32_t Cnt) {
//  LED_ON();			// тест, период появления ~16ms (62.5Hz)
#if false
// стирать во время non-Display
//  while (DPY_VnD_check()) { };	// ждать 0
//  while (!DPY_VnD_check()) { };	// ждать 1 (начало non-Display period)
#endif
  Pret_draw(false);
  MemBarPos_draw(false);
  // if (CursCtrl.On) Curs_draw(false);
  // if (Chan1Ctrl.On) SigChan1_draw(0);
  // if (Chan2Ctrl.On) SigChan2_draw(0);
// Очистить изображения курсоров, математики, сигналов */
  DpyWork_clear(MASK_CURSMATSIG);
  
//  LED_OFF();			// тест, ~2ms для двух каналов (1ms+1ms)
  int32_t pos = SweepCtrl.TimePos;
  pos += Cnt;
  int32_t lmarg = 0;		// Задается для положения 0 делений
  int32_t rmarg = (1024 << MemCtrl.SizeNum) - 300;
  if (pos < lmarg) pos = lmarg;
  if (pos > rmarg) pos = rmarg;
  SweepCtrl.TimePos = pos;
  if (Chan1Ctrl.On) SigChan1_draw(3);
  if (Chan2Ctrl.On) SigChan2_draw(3);
  Pret_draw(true);
  MemBarPos_draw(true);
  if (CursCtrl.On) Curs_draw(true);
//  LED_OFF();		// тест, ~4.25ms, период 30.50ms (изменил в панели управления)
}

/*!*****************************************************************************
 @brief		Trigger Level
 @details	Уровень Синхр
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpLevel_hand(int32_t Cnt) {
  TrigLev_draw(false);
  int16_t temp = TrigCtrl.Level + Cnt;
  if (temp < 0) temp = 0;
  if (temp > 1023) temp = 1023;
  TrigCtrl.Level = temp;
  TrigLev_draw(true);
//  PL->SDAC = PL_SDAC_T1LEV | (TrigCtrl.Level<<2);
//  DELAY_START(2); DELAY_WAIT();
//  PL->SDAC = PL_SDAC_T2LEV | (TrigCtrl.Level<<2);
//  DELAY_START(2); DELAY_WAIT();
//  PL->SDAC = PL_SDAC_TXLEV | (TrigCtrl.Level<<2);
  ANADAC(DAC_LEVEL, TrigCtrl.Level << 2);
}


//__ Кнопки, включающие МЕНЮ __________________________________________________//


/*!*****************************************************************************
 @brief		
 @details	Меню КАНАЛ 1
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpChan1_hand(int32_t Cnt) {
  if (Cnt > 0) {			// Кнопка нажата
    if (GenCtrl.Menu
        && MenuNum == MENU_CHAN1) {	// Есть свое МЕНЮ
      GenCtrl.Menu = 0;			// Выключить свое МЕНЮ
      HelpMenu_clear();      
      DispCtrl.Wide = 1;
      SigChans_redraw();
      Curs_redraw();
    }
    else {				// Нет своего МЕНЮ
      GenCtrl.Menu = 1;			// Включить свое МЕНЮ 
      MenuNum = MENU_CHAN1;
      MenuHead_disp();			// Нарисовать меню  
      DispCtrl.Wide = 0;
    }
    DpyScale_draw();
  }
  else	MenuHeadDepr_disp();		// Кнопка отпущена
}

#if 0
void FpChan1_hand(int32_t Cnt) {
  if (Cnt > 0) {			// Если кнопка нажата
    if (GenCtrl.Menu && 
        MenuNum == MENU_CHAN1) {	// Если есть свое МЕНЮ
      GenCtrl.Menu = 0;			// Выключить свое МЕНЮ
      HelpMenu_clear();
      DispCtrl.Wide = 1;
      SigChans_redraw();
    }
    else {				// Нет своего меню
      GenCtrl.Menu = 1;			// Включить свое меню
      MenuNum = MENU_CHAN1;
      MenuHead_disp();			// Нарисовать меню
      DispCtrl.Wide = 0;
      Chan1Ctrl.On ^= 1;		// Изменить состояние канала Выкл/Вкл
      // MnChan1Mode_disp();
      Chan1Mode_sett();
      Chan1St_disp();			// отобразить в статусе
      PosChan1_draw(Chan1Ctrl.On);	// отобразить маркер
    }
    DpyScale_draw();			// Нарисовать шкалу
  }
  else MenuHeadDepr_disp();		// Кнопка отпущена
}    
#endif
    
/*!*****************************************************************************
 @brief		
 @details	Меню МАТ ОБР
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpMath_hand(int32_t Cnt) {
  if (Cnt > 0) {			// Кнопка нажата
    if (GenCtrl.Menu &&
        MenuNum == MENU_MATH) {
      GenCtrl.Menu = 0;			// Выключить свое МЕНЮ
      HelpMenu_clear();
      DispCtrl.Wide = 1;
      SigChans_redraw();
      Curs_redraw();
    }
    else {
      GenCtrl.Menu = 1;			// Включить свое МЕНЮ
      MenuNum = MENU_MATH;
      MenuHead_disp();
      DispCtrl.Wide = 0;
    }
    DpyScale_draw();
  }
  else MenuHeadDepr_disp();
}

/*!*****************************************************************************
 @brief	
 @details	Меню КАНАЛ 2
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpChan2_hand(int32_t Cnt)
{
  if (Cnt > 0) {			// Кнопка нажата
    if (GenCtrl.Menu &&
        MenuNum == MENU_CHAN2) {
      GenCtrl.Menu = 0;			// Выключить свое МЕНЮ
      HelpMenu_clear();
      DispCtrl.Wide = 1;
      SigChans_redraw();
      Curs_redraw();
    }
    else {
      GenCtrl.Menu = 1;			// Включить свое МЕНЮ 
      MenuNum = MENU_CHAN2;
      MenuHead_disp();
      DispCtrl.Wide = 0;
    }
    DpyScale_draw();
  }
  else  MenuHeadDepr_disp();		// Кнопка отпущена
}

#if 0
void FpChan2_hand(int32_t Cnt) {
  if (Cnt > 0) {			// Если кнопка нажата
    if (GenCtrl.Menu && 
        MenuNum == MENU_CHAN2) {	// Если есть свое МЕНЮ
      GenCtrl.Menu = 0;			// Выключить свое МЕНЮ
      HelpMenu_clear();
      DispCtrl.Wide = 1;
      SigChans_redraw();
    }
    else {				// Нет своего меню
      GenCtrl.Menu = 1;			// Включить свое меню
      MenuNum = MENU_CHAN2;
      MenuHead_disp();			// Нарисовать меню
      DispCtrl.Wide = 0;
      Chan2Ctrl.On ^= 1;		// Изменить состояние канала Выкл/Вкл
      // MnChan1Mode_disp();
      Chan2Mode_sett();
      Chan2St_disp();			// отобразить в статусе
      PosChan2_draw(Chan2Ctrl.On);	// отобразить маркер
    }
    DpyScale_draw();			// Нарисовать нормальную или широкую шкалу
  }
  else MenuHeadDepr_disp();		// Кнопка отпущена
}    
#endif

/*!*****************************************************************************
 */
void Fp11_hand(int32_t Cnt) { }

/*!*****************************************************************************
 @brief	
 @details	Меню РАЗВЕРТКА
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note	
 */
void FpSweep_hand(int32_t Cnt) {
  if (Cnt > 0) {				// Кнопка нажата
    if (GenCtrl.Menu && MenuNum == MENU_SWEEP) {
      GenCtrl.Menu = 0;				// Выключить МЕНЮ
      HelpMenu_clear();
      DispCtrl.Wide = 1;
      SigChans_redraw();
      Curs_redraw();
    }
    else {
      GenCtrl.Menu = 1;				// Включить МЕНЮ
      MenuNum = MENU_SWEEP;
      MenuHead_disp();
      DispCtrl.Wide = 0;
    }
    DpyScale_draw();
  }
  else MenuHeadDepr_disp();			// Кнопка отпущена
}

/*!*****************************************************************************
 */
void Fp13_hand(int32_t Cnt) { }

/*!*****************************************************************************
 @brief	
 @details	Меню СИНХР
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note	
 */
void FpTrig_hand(int32_t Cnt) {
  if (Cnt > 0) {				// Кнопка нажата
    if (GenCtrl.Menu && MenuNum == MENU_TRIG) {
      GenCtrl.Menu = 0;				// Выключить МЕНЮ
      HelpMenu_clear();
      DispCtrl.Wide = 1;
      SigChans_redraw();
      Curs_redraw();
    }
    else {
      GenCtrl.Menu = 1;				// Включить МЕНЮ
      MenuNum = MENU_TRIG;
      MenuHead_disp();
      DispCtrl.Wide = 0;
    }
    DpyScale_draw();
  }
  else MenuHeadDepr_disp();			// Кнопка отпущена
}

/*!*****************************************************************************
 */
void Fp15_hand(int32_t Cnt) { }

/*!*****************************************************************************
 @brief	
 @details	Меню ДИСПЛЕЙ
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note	
 */
void FpDisp_hand(int32_t Cnt) {
  if (Cnt > 0) {				// Кнопка нажата
    if (GenCtrl.Menu && MenuNum == MENU_DISP) {
      GenCtrl.Menu = 0;				// Выключить МЕНЮ
      HelpMenu_clear();
      DispCtrl.Wide = 1;
      SigChans_redraw();
      Curs_redraw();
    }
    else {
      GenCtrl.Menu = 1;				// Включить МЕНЮ
      MenuNum = MENU_DISP;
      MenuHead_disp();
      DispCtrl.Wide = 0;
    }
    DpyScale_draw();
  }
  else MenuHeadDepr_disp();			// Кнопка отпущена
}

/*!*****************************************************************************
 @brief		
 @details	Меню СБОР ИНФ
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpAcqu_hand(int32_t Cnt) {
  if (Cnt > 0) {				// Кнопка нажата
    if (GenCtrl.Menu && MenuNum == MENU_ACQU) {
      GenCtrl.Menu = 0;				// Выключить МЕНЮ
      HelpMenu_clear();
      DispCtrl.Wide = 1;
      SigChans_redraw();
      Curs_redraw();
    }
    else {
      GenCtrl.Menu = 1;				// Включить МЕНЮ
      MenuNum = MENU_ACQU;
      MenuHead_disp();
      DispCtrl.Wide = 0;
    }
    DpyScale_draw();
  }
  else MenuHeadDepr_disp();			// Кнопка отпущена
}

/*!*****************************************************************************
 @brief	
 @details	Меню КУРСОРЫ
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note	
 */
void FpCurs_hand(int32_t Cnt) {
  if (Cnt > 0) {				// Кнопка нажата
    if (GenCtrl.Menu && MenuNum == MENU_CURS) {
      GenCtrl.Menu = 0;				// Выключить МЕНЮ
      HelpMenu_clear();
      DispCtrl.Wide = 1;
      SigChans_redraw();
      Curs_redraw();
    }
    else {
      GenCtrl.Menu = 1;				// Включить МЕНЮ
      MenuNum = MENU_CURS;
      MenuHead_disp();
      DispCtrl.Wide = 0;
    }
    DpyScale_draw();
  }
  else MenuHeadDepr_disp();			// Кнопка отпущена
}

/*!*****************************************************************************
 @brief	
 @details	Меню ИЗМЕР
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note	
 */
void FpMeas_hand(int32_t Cnt) {
  if (Cnt > 0) {				// Кнопка нажата
    if (GenCtrl.Menu && MenuNum == MENU_MEAS) {
      GenCtrl.Menu = 0;				// Выключить МЕНЮ
      HelpMenu_clear();
      DispCtrl.Wide = 1;
      SigChans_redraw();
      Curs_redraw();
    }
    else {
      GenCtrl.Menu = 1;				// Включить МЕНЮ
      MenuNum = MENU_MEAS;
      MenuHead_disp();
      DispCtrl.Wide = 0;
    }
    DpyScale_draw();
  }
  else MenuHeadDepr_disp();			// Кнопка отпущена
}

/*!*****************************************************************************
 @brief	
 @details	Меню ПАМЯТЬ
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note	
 */
void FpMem_hand(int32_t Cnt) {
  if (Cnt > 0) {				// Кнопка нажата
    if (GenCtrl.Menu && MenuNum == MENU_MEM) {
      GenCtrl.Menu = 0;				// Выключить МЕНЮ
      HelpMenu_clear();
      DispCtrl.Wide = 1;
      SigChans_redraw();
      Curs_redraw();
    }
    else {
      GenCtrl.Menu = 1;				// Включить МЕНЮ
      MenuNum = MENU_MEM;
      MenuHead_disp();
      DispCtrl.Wide = 0;
    }
    DpyScale_draw();
  }
  else MenuHeadDepr_disp();			// Кнопка отпущена
}

/*!*****************************************************************************
 @brief	
 @details	Меню УТИЛИТЫ
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note	
 */
void FpUtil_hand(int32_t Cnt) {
  if (Cnt > 0) {				// Кнопка нажата
    if (GenCtrl.Menu && MenuNum == MENU_UTIL) {
      GenCtrl.Menu = 0;				// Выключить МЕНЮ
      HelpMenu_clear();
      // DPY_ClearExMenu();			// в утилитах портится экран
      DispCtrl.Wide = 1;
      SigChans_redraw();
      Curs_redraw();
    }
    else {
      GenCtrl.Menu = 1;				// Включить МЕНЮ
      MenuNum = MENU_UTIL;
      MenuHead_disp();
      DispCtrl.Wide = 0;
    }
    DpyScale_draw();
  }
  else MenuHeadDepr_disp();			// Кнопка отпущена
}


//__ Кнопки без МЕНЮ __________________________________________________________//


/*!*****************************************************************************
 @brief		
 @details	АВТО УСТ
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpAutoset_hand(int32_t Cnt) {
#define LEN 8		// time lenth (LEN * 0.5 s / 25 = 160 ms)
#define DTM 8		// dead time
#define HYS 8		// hysteresis

  if (Cnt > 0 && GenCtrl.Autoset == 0) {	// Key Auto pressed, Autoset done
    GenCtrl.Autoset = 1;

/* Установить режимы, общие для двух каналов */

// Задать фильтр и пиковый детектор
    /// AcquCtrl.Filt = 3;
    /// AcquFilt_sett();
    AcquCtrl.Peak = 1;
    AcquPeak_sett();    
    
// Установить развертку по точкам
    SweepCtrl.TimeNum = TIME_DOT;
    Time_sett();

// Установить запрос прерываний от ПЛИС только по точкам
    PL_AcquBuf &= ~(PL_ACQU_TDTIE | PL_ACQU_FRMIE | PL_ACQU_DOTIE);
    PL_AcquBuf |= PL_ACQU_DOTIE;
    PL->ACQU = PL_AcquBuf;
    SweepCtrl.Stop = 0;

    uint32_t min;
    uint32_t max;
    uint32_t rng;		// range
    
/* Искать сигнал в канале 1, если включен */
    if (Chan1Ctrl.On) {
      
// Задать ограничение полосы
//    Chan1Ctrl.BWLim = 1;
//    Chan1BWLim_sett();

// Установить самое малое усиление, связь по переменному току
      Chan1Ctrl.VoltNum = 4;				// 20V
      Chan1Ctrl.Cplg = 1;
      Chan1Cplg_sett();

// Установить позицию в середину экрана
      PosChan1_draw(false);
      TrigLev_draw(false); 
      Chan1Ctrl.SigPos = 512;
      Chan1Ctrl.DispPos = 0;
      FpPos1_hand(0);
      TrigLev_draw(false);      

// Старт сбора сигнала    
      PL->STAT = PL_START_ACQUST;

// Найти минимум, максимум
      min = 255;
      max = 0;
      for (uint32_t i = DTM; i--; ) {
        while (PLIRQ_GET()) { };	// Ждать, пока нет запроса от точки
        uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      }
      for (uint32_t i = LEN; i--; ) {
        while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
        uint_fast16_t stat = PL->STAT;		// Читать и сбросить состояние
        uint32_t rint = (uint8_t)PL->RINT;	// читать канал 1 
        if (rint < min) min = rint;
        if (rint > max) max = rint;
      }
      
// Выбрать коэфициент отклонения
      rng = max - min;
      if (rng > 100) { }				// 20V
      else if (rng > 50) Chan1Ctrl.VoltNum = 5;		// 10V
      else if (rng > 20) Chan1Ctrl.VoltNum = 6;		// 5V
      else {
      
// Установить к-т в 10 раз чувствительнее
        Chan1Ctrl.VoltNum = 7;				// 2V
	Chan1Cplg_sett();
	min = 255;
	max = 0;
        for (uint32_t i = DTM; i--; ) {
          while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
          uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
        }
        for (uint32_t i = LEN; i--; ) {
          while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
          uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
          uint32_t rint = (uint8_t)PL->RINT;	// читать канал 1 
          if (rint < min) min = rint;
          if (rint > max) max = rint;
        }

// Выбрать коэфициент отклонения
        rng = max - min;
        if (rng > 100) { }				// 2V
	else if (rng > 50) Chan1Ctrl.VoltNum = 8;	// 1V
	else if (rng > 20) Chan1Ctrl.VoltNum = 9;	// 0.5V
	else {

// Установить к-т в 10 раз чувствительнее
          Chan1Ctrl.VoltNum = 10;			// 0.2V
	  Chan1Cplg_sett();
	  min = 255;
	  max = 0;
          for (uint32_t i = DTM; i--; ) {
            while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
            uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
          }
          for (uint32_t i = LEN; i--; ) {
            while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
            uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
            uint32_t rint = (uint8_t)PL->RINT;	// читать канал 1 
            if (rint < min) min = rint;
            if (rint > max) max = rint;
          }

// Выбрать коэфициент отклонения
          rng = max - min;
          if (rng > 100) { }				// 0.2V
	  else if (rng > 50) Chan1Ctrl.VoltNum = 11;	// 0.1V
	  else if (rng > 20) Chan1Ctrl.VoltNum = 12;	// 50mV
	  else {

// Установить к-т в 10 раз чувствительнее
            Chan1Ctrl.VoltNum = 13;			// 20mV
	    Chan1Cplg_sett();
	    min = 255;
	    max = 0;
            for (uint32_t i = DTM; i--; ) {
              while (PLIRQ_GET()) { };	// Ждать, пока нет запроса от точки
              uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
            }
    
            for (uint32_t i = LEN; i--; ) {
              while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
              uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
              uint32_t rint = (uint8_t)PL->RINT;	// читать канал 1 
              if (rint < min) min = rint;
              if (rint > max) max = rint;
            }  

// Выбрать коэфициент отклонения
            rng = max - min;
            if (rng > 100) { }				// 20mV
	    else if (rng > 50) Chan1Ctrl.VoltNum = 14;	// 10mV
	    else if (rng > 20) Chan1Ctrl.VoltNum = 15;	// 5mV
	    else Chan1Ctrl.VoltNum = 16;		// 2mV

          }	// 20mV
	}	// 0.2V
      }		// 2V
      
// Если включены оба канала, уменьшить к-т (кроме самого чувствительного)
      if (Chan2Ctrl.On) {
        if (Chan1Ctrl.VoltNum != 16)
          Chan1Ctrl.VoltNum--;      
        PosChan1_draw(false);
	Chan1Ctrl.SigPos += 100;
      }
      
    Chan1Cplg_sett();
    FpPos1_hand(0);
    TrigLev_draw(false);		// стереть символ    
    }		// Chan 1

/* Искать сигнал в канале 2, если включен */
    if (Chan2Ctrl.On) {
      
// Задать ограничение полосы
//    Chan1Ctrl.BWLim = 1;
//    Chan1BWLim_sett();

// Установить самое малое усиление, связь по переменному току
      Chan2Ctrl.VoltNum = 4;				// 20V
      Chan2Ctrl.Cplg = 1;
      Chan2Cplg_sett();

// Установить позицию в середину экрана
      PosChan2_draw(false);
      Chan2Ctrl.SigPos = 512;
      Chan2Ctrl.DispPos = 0;
      FpPos2_hand(0);
      TrigLev_draw(false);      

// Старт сбора сигнала    
      PL->STAT = PL_START_ACQUST;

// Найти минимум, максимум
      min = 255;
      max = 0;
      for (uint32_t i = DTM; i--; ) {
        while (PLIRQ_GET()) { };	// Ждать, пока нет запроса от точки
        uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      }
      for (uint32_t i = LEN; i--; ) {
        while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
        uint_fast16_t stat = PL->STAT;		// Читать и сбросить состояние
        uint32_t rint = (uint8_t)(PL->RINT >> 8);	// читать канал 2 
        if (rint < min) min = rint;
        if (rint > max) max = rint;
      }
      
// Выбрать коэфициент отклонения
      rng = max - min;
      if (rng > 100) { }				// 20V
      else if (rng > 50) Chan2Ctrl.VoltNum = 5;		// 10V
      else if (rng > 20) Chan2Ctrl.VoltNum = 6;		// 5V
      else {
      
// Установить к-т в 10 раз чувствительнее
        Chan2Ctrl.VoltNum = 7;				// 2V
	Chan2Cplg_sett();
	min = 255;
	max = 0;
        for (uint32_t i = DTM; i--; ) {
          while (PLIRQ_GET()) { };	// Ждать, пока нет запроса от точки
          uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
        }
        for (uint32_t i = LEN; i--; ) {
          while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
          uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
          uint32_t rint = (uint8_t)(PL->RINT >> 8);	// читать канал 2 
          if (rint < min) min = rint;
          if (rint > max) max = rint;
        }

// Выбрать коэфициент отклонения
        rng = max - min;
        if (rng > 100) { }				// 2V
	else if (rng > 50) Chan2Ctrl.VoltNum = 8;	// 1V
	else if (rng > 20) Chan2Ctrl.VoltNum = 9;	// 0.5V
	else {

// Установить к-т в 10 раз чувствительнее
          Chan2Ctrl.VoltNum = 10;			// 0.2V
	  Chan2Cplg_sett();
	  min = 255;
	  max = 0;
          for (uint32_t i = DTM; i--; ) {
            while (PLIRQ_GET()) { };	// Ждать, пока нет запроса от точки
            uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
          }
          for (uint32_t i = LEN; i--; ) {
            while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
            uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
            uint32_t rint = (uint8_t)(PL->RINT >> 8);	// читать канал 2 
            if (rint < min) min = rint;
            if (rint > max) max = rint;
          }

// Выбрать коэфициент отклонения
          rng = max - min;
          if (rng > 100) { }				// 0.2V
	  else if (rng > 50) Chan2Ctrl.VoltNum = 11;	// 0.1V
	  else if (rng > 20) Chan2Ctrl.VoltNum = 12;	// 50mV
	  else {

// Установить к-т в 10 раз чувствительнее
            Chan2Ctrl.VoltNum = 13;			// 20mV
	    Chan2Cplg_sett();
	    min = 255;
	    max = 0;
            for (uint32_t i = DTM; i--; ) {
              while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
              uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
            }
            for (uint32_t i = LEN; i--; ) {
              while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
              uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
              uint32_t rint = (uint8_t)(PL->RINT >> 8);	// читать канал 2 
              if (rint < min) min = rint;
              if (rint > max) max = rint;
            }

// Выбрать коэфициент отклонения
            rng = max - min;
            if (rng > 100) { }				// 20mV
	    else if (rng > 50) Chan2Ctrl.VoltNum = 14;	// 10mV
	    else if (rng > 20) Chan2Ctrl.VoltNum = 15;	// 5mV
	    else Chan2Ctrl.VoltNum = 16;		// 2mV

          }	// 20mV
	}	// 0.2V
      }		// 2V
      
// Если включены оба канала, уменьшить к-т (кроме самого чувствительного)
      if (Chan1Ctrl.On) {
        if (Chan2Ctrl.VoltNum != 16)
          Chan2Ctrl.VoltNum--;      
        PosChan2_draw(false);
	Chan2Ctrl.SigPos -= 100;
      }
          
    Chan2Cplg_sett();
    FpPos2_hand(0);
    TrigLev_draw(false);
    }		// Chan 2

/* Установить синхронизацию по более сильному сигналу */

    TrigLev_draw(false);
    TrigCtrl.Level = 512;
    ANADAC(DAC_LEVEL, TrigCtrl.Level << 2);
    
    if (Chan2Ctrl.VoltNum < Chan1Ctrl.VoltNum) {
      TrigCtrl.Sour = 2;
    }
    else TrigCtrl.Sour = 1;
    TrigSour_sett();
    TrigSt_disp();
    TrigLev_draw(true);
    
/* Установить Время/дел */

    // SweepCtrl.TimeNum = 15;
    // Time_sett();

// Найти середину сильного сигнала
    min = 255;
    max = 0;
    for (uint32_t i = DTM; i--; ) {
       while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
       uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    }

    if (TrigCtrl.Sour == 1) {
      for (uint32_t i = LEN; i--; ) {
        while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
        uint_fast16_t stat = PL->STAT;		// Читать и сбросить состояние
        uint32_t rint = (uint8_t)PL->RINT;	// читать канал 1 
        if (rint < min) min = rint;
        if (rint > max) max = rint;
      }
    }
    
    if (TrigCtrl.Sour == 2) {
      for (uint32_t i = LEN; i--; ) {
        while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
        uint_fast16_t stat = PL->STAT;		// Читать и сбросить состояние
        uint32_t rint = (uint8_t)(PL->RINT >> 8);	// читать канал 2 
        if (rint < min) min = rint;
        if (rint > max) max = rint;
      }
    }
    
    uint32_t mid = (max + min) / 2;
    // Num_draw(mid);

// Убрать фильтр и пиковый детектор
    AcquCtrl.Filt = 0;
    AcquFilt_sett();    
    AcquCtrl.Peak = 0;
    AcquPeak_sett();
    
// Установить запрос прерываний от ПЛИС только кадрам
    PL_AcquBuf &= ~(PL_ACQU_TDTIE | PL_ACQU_FRMIE | PL_ACQU_DOTIE);
    PL_AcquBuf |= PL_ACQU_FRMIE;
    PL->ACQU = PL_AcquBuf;
    
// Установить размер памяти 0.5K (не нужно! просто считать 512 выборок)
    // MemCtrl.SizeNum = 0;		// 0.5K
    // MemSizeBuf_disp();
    // MemSize_sett();
    
// Установить развертку по кадрам, 0.1 us, самую быструю по полному кадру
// 0.1 us * (512 / 25) = 2.048 us
    SweepCtrl.TimeNum = 26;
    Time_sett();    

// Старт сбора сигнала, ожидать готовность кадра    
    PL->STAT = PL_START_ACQUST;
      
    while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от кадра
    uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние

    bool     sthi = false;		// Сигнал выше середины
    uint32_t edge = 0;			// Количество переходов через середину
    
// Проверка в канале 1
    if (TrigCtrl.Sour == 1) {   
      uint32_t rint = (uint8_t)(PL->MINT);	// Начальное состояние сигнала в кадре
      if (rint < mid - HYS) sthi = 0;
      if (rint > mid + HYS) sthi = 1;     
      for (uint32_t i = 511; i--; ) {
        rint = (uint8_t)PL->MINT;
        if ((rint < mid - HYS && sthi) || (rint > mid + HYS && !sthi)) {
          edge++;
          sthi ^= 1;
	}
      }
    }

// Проверка в канале 2
    if (TrigCtrl.Sour == 2) {   
      uint32_t rint = (uint8_t)(PL->MINT >> 8);	// Начальное состояние сигнала в кадре
      if (rint < mid - HYS) sthi = 0;
      if (rint > mid + HYS) sthi = 1;     
      for (uint32_t i = 511; i--; ) {
        rint = (uint8_t)(PL->MINT >> 8);
        if ((rint < mid - HYS && sthi) || (rint > mid + HYS && !sthi)) {
          edge++;
          sthi ^= 1;
        }
      }
    }
    
    if (edge > 200) SweepCtrl.TimeNum = 31;
    else if (edge > 80) SweepCtrl.TimeNum = 30;
    else if (edge > 40) SweepCtrl.TimeNum = 29;
    else if (edge > 20) SweepCtrl.TimeNum = 28;
    else if (edge > 8) SweepCtrl.TimeNum = 27;
    else if (edge > 4) SweepCtrl.TimeNum = 26;
    else
    {
      
// Установить развертку 10us
      SweepCtrl.TimeNum = 20;
      Time_sett();    

// Старт сбора сигнала, ожидать готовность кадра    
      PL->STAT = PL_START_ACQUST;
      
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от кадра
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние

      edge = 0;				// Количество переходов через середину
    
// Проверка в канале 1
      if (TrigCtrl.Sour == 1) {
        uint32_t rint = (uint8_t)PL->MINT;	// Начальное состояние сигнала в кадре
        if (rint < mid - HYS) sthi = 0;
        if (rint > mid + HYS) sthi = 1;
        for (uint32_t i = 511; i--; ) {
          rint = (uint8_t)PL->MINT;
          if ((rint < mid - HYS && sthi) || (rint > mid + HYS && !sthi)) {
            edge++;
            sthi ^= 1;
          }
        }
      }

// Проверка в канале 2
      if (TrigCtrl.Sour == 2) {
        uint32_t rint = (uint8_t)(PL->MINT >> 8);	// Начальное состояние сигнала в кадре
        if (rint < mid - HYS) sthi = 0;
        if (rint > mid + HYS) sthi = 1;
        for (uint32_t i = 511; i--; ) {
          rint = (uint8_t)(PL->MINT >> 8);
          if ((rint < mid - HYS && sthi) || (rint > mid + HYS && !sthi)) {
            edge++;
            sthi ^= 1;
          }
        }
      }
      
      if (edge > 200) SweepCtrl.TimeNum = 25;
      else if (edge > 80) SweepCtrl.TimeNum = 24;
      else if (edge > 40) SweepCtrl.TimeNum = 23;
      else if (edge > 20) SweepCtrl.TimeNum = 22;
      else if (edge > 8) SweepCtrl.TimeNum = 21;
      else if (edge > 4) SweepCtrl.TimeNum = 20;
      else
      {
      
// Установить развертку 1ms
        SweepCtrl.TimeNum = 14;
        Time_sett();    

// Старт сбора сигнала, ожидать готовность кадра    
        PL->STAT = PL_START_ACQUST;
      
        while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от кадра
        uint_fast16_t stat = PL->STAT;		// Читать и сбросить состояние

        edge = 0;				// Количество переходов через середину
    
// Проверка в канале 1
        if (TrigCtrl.Sour == 1) {
          uint32_t rint = (uint8_t)PL->MINT;	// Начальное состояние сигнала в кадре
          if (rint < mid - HYS) sthi = 0;
          if (rint > mid + HYS) sthi = 1;
          for (uint32_t i = 511; i--; ) {
            rint = (uint8_t)PL->MINT;
            if ((rint < mid - HYS && sthi) || (rint > mid + HYS && !sthi)) {
              edge++;
              sthi ^= 1;
            }
          }
        }
	
// Проверка в канале 2
        if (TrigCtrl.Sour == 2) {
          uint32_t rint = (uint8_t)(PL->MINT >> 8);	// Начальное состояние сигнала в кадре
          if (rint < mid - HYS) sthi = 0;
          if (rint > mid + HYS) sthi = 1;
          for (uint32_t i = 511; i--; ) {
            rint = (uint8_t)(PL->MINT >> 8);
            if ((rint < mid - HYS && sthi) || (rint > mid + HYS && !sthi)) {
              edge++;
              sthi ^= 1;
            }
          }
        }
      
        if (edge > 200) SweepCtrl.TimeNum = 19;
        else if (edge > 80) SweepCtrl.TimeNum = 18;
        else if (edge > 40) SweepCtrl.TimeNum = 17;
        else if (edge > 20) SweepCtrl.TimeNum = 16;
        else if (edge > 8) SweepCtrl.TimeNum = 15;
        else if (edge > 4) SweepCtrl.TimeNum = 14;
        else
	{

// Установить развертку 0.1s
          SweepCtrl.TimeNum = 8;
          Time_sett();

// Старт сбора сигнала, ожидать готовность кадра    
          PL->STAT = PL_START_ACQUST;
      
          while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от кадра
          uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние

          edge = 0;				// Количество переходов через середину
    
// Проверка в канале 1
          if (TrigCtrl.Sour == 1) {
            uint32_t rint = (uint8_t)PL->MINT;		// Начальное состояние сигнала в кадре
            if (rint < mid - HYS) sthi = 0;
            if (rint > mid + HYS) sthi = 1;
            for (uint32_t i = 511; i--; ) {
              rint = (uint8_t)PL->MINT;
              if ((rint < mid - HYS && sthi) || (rint > mid + HYS && !sthi)) {
                edge++;
                sthi ^= 1;
              }
            }
          }

// Проверка в канале 2
          if (TrigCtrl.Sour == 2) {
            uint32_t rint = (uint8_t)(PL->MINT >> 8);	// Начальное состояние сигнала в кадре
            if (rint < mid - HYS) sthi = 0;
            if (rint > mid + HYS) sthi = 1;
            for (uint32_t i = 511; i--; ) {
              rint = (uint8_t)(PL->MINT >> 8);
              if ((rint < mid - HYS && sthi) || (rint > mid + HYS && !sthi)) {
                edge++;
                sthi ^= 1;
              }
            }
          }
      
          if (edge > 200) SweepCtrl.TimeNum = 13;
          else if (edge > 80) SweepCtrl.TimeNum = 12;
          else if (edge > 40) SweepCtrl.TimeNum = 11;
          else if (edge > 20) SweepCtrl.TimeNum = 10;
          else if (edge > 8) SweepCtrl.TimeNum = 9;
          else if (edge > 4) SweepCtrl.TimeNum = 8;
          else SweepCtrl.TimeNum = 7;

	}	// < 14
      }		// < 20
    }		// < 26
    
    //  Num_draw(edge);
    Time_sett();
    
/* Восстановить нормальные режимы работы */    

// Отобразить маркеры    
    // PosChan1_draw(Chan1Ctrl.On);
    // PosChan2_draw(Chan2Ctrl.On);
    
// Отобразить в статусе    
    Chan1St_disp();
    Chan2St_disp();
    TimeSt_disp(); 
    
// Восстановить запрос прерывания по изменению синхронизации

    PL_AcquBuf |= PL_ACQU_TDTIE;
    PL->ACQU = PL_AcquBuf;
  }

  Autoset_disp();
  GenCtrl.Autoset = 0;
  
  DpyWork_clear(MASK_MATSIG);
  Sweep_init();  
  // PL->STAT = PL_START_ACQUST;
  
// Индицировать синхронизацию

  if (PL->STAT & PL_STAT_TRIGDET)	// Если есть синхронизация
    PanCtrl.TxBuf &= ~(1 << 4);		// Зажечь диод синхронизации
  else
    PanCtrl.TxBuf |=  (1 << 4);		// Погасить диод синхронизации
  PanCtrl.TxBuf |= 0x40;
  USART3->DR = PanCtrl.TxBuf;		// Послать в панель
  
}

/*!*****************************************************************************
 @brief		
 @details	ПУСК/СТОП
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpStSp_hand(int32_t Cnt) {
  if (Cnt > 0) {			// Key Stop pressed
    SweepCtrl.Stop ^= 1;
    StSp_disp();			// display
    if (!SweepCtrl.Stop) {
      Sweep_init();
      if (SweepCtrl.Roll) DpyWork_clear(MASK_MATSIG);
    }
  }
}

/*!*****************************************************************************
 @brief		
 @details	СЕРВИС / МЕНЮ / ПОМОЩЬ
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		Menu = 0; -> Menu = 1; -> Menu = 1;
		Help = 0; -> Help = 0; -> Help = 1;
 */
void FpHelp_hand(int32_t Cnt) {
  if (Cnt > 0) {				// Кнопка нажата
// В сервисное меню входить после остановки кадров
    if (MenuNum == MENU_SERV && !SweepCtrl.Stop) return;
    if (!GenCtrl.Menu) {			// если МЕНЮ нет
      GenCtrl.Menu = 1;				// восстановить МЕНЮ
      if (MenuNum == MENU_SERV)  DpyScreen_blank();
      MenuHead_disp();				// отобразить МЕНЮ
      DispCtrl.Wide = 0;
      DpyScale_draw();
    }
    else {					// МЕНЮ есть
      if (DispCtrl.HelpEna && !GenCtrl.Help) {	// если Помощь разрешена, но не включена
        GenCtrl.Help = 1;			// изменить режим Помощь      
	Help_draw();				// написать Помощь
      }
      else {					// Помощь запрещена, или уже включена
        GenCtrl.Menu = 0;
	GenCtrl.Help = 0;
        if (MenuNum == MENU_SERV)  DpyScreen_blank();
        HelpMenu_clear();
        DispCtrl.Wide = 1;
	DpyScale_draw();			// нарисовать шкалу
        SigChans_redraw();
	Curs_redraw();
      }
    }
  }
  else {					// Кнопка отпущена
    {
      MenuHeadDepr_disp();			// нарисовать отпущенную кнопку
    }
  }
}


//__ Кнопки, обслуживающие МЕНЮ _______________________________________________//


/*!*****************************************************************************
 @brief		
 @details	F1
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpF1_hand(int32_t Cnt) {
  if (GenCtrl.Menu) {
    if (Cnt > 0) {
      if (MenuNum == MENU_SERV) {
        MenuHead_disp();
	MenuHeadDepr_disp();
      }
      DPY_LINEH(DPYMAH + 1, DPYM1AV,      318, 0xff, ITEM_EMFA, 0xff);
      DPY_LINEH(DPYMAH + 1, DPYM1AV + 34, 318, 0xff, ITEM_EMRI, 0xff);
      MenuItem_hand[MenuNum][0]();
    }
    else {
      DPY_LINEH(DPYMAH + 1, DPYM1AV,      318, 0xff, ITEM_EMRI, 0xff);
      DPY_LINEH(DPYMAH + 1, DPYM1AV + 34, 318, 0xff, ITEM_EMFA, 0xff);
    }
  }
}

/*!*****************************************************************************
 @brief		
 @details	F2
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpF2_hand(int32_t Cnt) {
  if (GenCtrl.Menu) {
    if (Cnt > 0) {
      if (MenuNum == MENU_SERV) {
        MenuHead_disp();
	MenuHeadDepr_disp();
      }
      DPY_LINEH(DPYMAH + 1, DPYM2AV,      318, 0xff, ITEM_EMFA, 0xff);
      DPY_LINEH(DPYMAH + 1, DPYM2AV + 34, 318, 0xff, ITEM_EMRI, 0xff);
      MenuItem_hand[MenuNum][1]();
    }
    else {
      DPY_LINEH(DPYMAH + 1, DPYM2AV,      318, 0xff, ITEM_EMRI, 0xff);
      DPY_LINEH(DPYMAH + 1, DPYM2AV + 34, 318, 0xff, ITEM_EMFA, 0xff);
    }
  }
}

/*!*****************************************************************************
 @brief		
 @details	F3
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpF3_hand(int32_t Cnt) {
  if (GenCtrl.Menu) {
    if (Cnt > 0) {
      if (MenuNum == MENU_SERV) {
        MenuHead_disp();
	MenuHeadDepr_disp();
      }
      DPY_LINEH(DPYMAH + 1, DPYM3AV,      318, 0xff, ITEM_EMFA, 0xff);
      DPY_LINEH(DPYMAH + 1, DPYM3AV + 34, 318, 0xff, ITEM_EMRI, 0xff);
      MenuItem_hand[MenuNum][2]();
    }
    else {
      DPY_LINEH(DPYMAH + 1, DPYM3AV,      318, 0xff, ITEM_EMRI, 0xff);
      DPY_LINEH(DPYMAH + 1, DPYM3AV + 34, 318, 0xff, ITEM_EMFA, 0xff);
    }
  }
}

/*!*****************************************************************************
 @brief		
 @details	F4
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpF4_hand(int32_t Cnt) {
  if (GenCtrl.Menu) {
    if (Cnt > 0) {
      if (MenuNum == MENU_SERV) {
        MenuHead_disp();
	MenuHeadDepr_disp();
      }
      DPY_LINEH(DPYMAH + 1, DPYM4AV,      318, 0xff, ITEM_EMFA, 0xff);
      DPY_LINEH(DPYMAH + 1, DPYM4AV + 34, 318, 0xff, ITEM_EMRI, 0xff);
      MenuItem_hand[MenuNum][3]();
    }
    else {
      DPY_LINEH(DPYMAH + 1, DPYM4AV,      318, 0xff, ITEM_EMRI, 0xff);
      DPY_LINEH(DPYMAH + 1, DPYM4AV + 34, 318, 0xff, ITEM_EMFA, 0xff);
    }
  }
}

#if false
/*!*****************************************************************************
 @brief		
 @details	F5
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpF5_hand(int32_t Cnt) {
  if (GenCtrl.Menu) {
    if (Cnt > 0) {
      if (!GenCtrl.Lock) {    
        DPY_LINEH(DPYMAH + 1, DPYM5AV,      318, 0xff, ITEM_EMFA, 0xff);
        DPY_LINEH(DPYMAH + 1, DPYM5AV + 34, 318, 0xff, ITEM_EMRI, 0xff);
// для режима СЕРВИС утопить кнопку Выполнить
	if (MenuNum == MENU_SERV) {
	  DPY_LINEH(DPYMAH + 1, DPYM5AV,      318, 0xFF, ITEM_EMFA, 0xFF);
	  DPY_LINEH(DPYMAH + 1, DPYM5AV + 34, 318, 0xFF, ITEM_EMRI, 0xFF);
	}
      }
      MenuItem_hand[MenuNum][4](); 
    }
    else {
      if (!GenCtrl.Lock) {
        DPY_LINEH(DPYMAH + 1, DPYM5AV,      318, 0xff, ITEM_EMRI, 0xff);
        DPY_LINEH(DPYMAH + 1, DPYM5AV + 34, 318, 0xff, ITEM_EMFA, 0xff);
      }
    }
  }
}
#endif

/*!*****************************************************************************
 @brief		
 @details	F5
 @param		Cnt - Ускоренный счетчик инкремента-декремента
 @return	
 @note		
 */
void FpF5_hand(int32_t Cnt) {
  if (GenCtrl.Menu) {
    if (Cnt > 0) {
      if (!GenCtrl.Lock) {    
        DPY_LINEH(DPYMAH + 1, DPYM5AV,      318, 0xff, ITEM_EMFA, 0xff);
        DPY_LINEH(DPYMAH + 1, DPYM5AV + 34, 318, 0xff, ITEM_EMRI, 0xff);
// для режима СЕРВИС утопить кнопку Выполнить
	if (MenuNum == MENU_SERV) {
	  DPY_LINEH(DPYMAH + 1, DPYM5AV,      318, 0xFF, ITEM_EMFA, 0xFF);
	  DPY_LINEH(DPYMAH + 1, DPYM5AV + 34, 318, 0xFF, ITEM_EMRI, 0xFF);
	}
      }
      MenuItem_hand[MenuNum][4](); 
    }
    else {
      if (!GenCtrl.Lock) {
        DPY_LINEH(DPYMAH + 1, DPYM5AV,      318, 0xff, ITEM_EMRI, 0xff);
        DPY_LINEH(DPYMAH + 1, DPYM5AV + 34, 318, 0xff, ITEM_EMFA, 0xff);
      }
    }
  }
}

/*!*****************************************************************************
 */
void Fp30_hand(int32_t Cnt) { }

/*!*****************************************************************************
 */
void Fp31_hand(int32_t Cnt) { }


//__ Обработчики элементов МЕНЮ, Индикаторы переменных МЕНЮ ___________________//


/*!*****************************************************************************
 @brief		
 @details	Пустой обработчик элемента МЕНЮ
 @note		
 */
void MnVoid_hand(void) { }

/*!*****************************************************************************
 @brief		
 @details	Пустая функция индикации элемента МЕНЮ
 @note		
 */
void MnVoid_disp(void) { }


//* Функции Меню СЕРВИС *//

/*!*****************************************************************************
 @brief		
 @details	Сервис, Калибровка, обработчик в МЕНЮ
 */
void MnServCal_hand(void) {
  uint_fast8_t num = ServCtrl.CalNum;
  num++;
  if (num >= 2) num = 0;
  ServCtrl.CalNum = num;
  ExeNum = (EXE_t)(EXE_CALIBR + num);	// изменить номер функции "Выполнить"
  MnServCal_disp();
}

/*!*****************************************************************************
 @brief		
 @details	Сервис, Калибровка, индикатор в МЕНЮ
 */
void MnServCal_disp(void) {
  static const uint8_t *Caltext[] = {" Интерп", "Сохранить"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 1, DPYM1VV);
  uint_fast8_t numb = ServCtrl.CalNum;
  DpyRect_fill(pDpy, 58, 10, 0xff<<8 | VOFF_BKG);
  DpyString_draw(pDpy, Caltext[numb], VOFF_TXT, 0); 
}

/*!*****************************************************************************
 @brief		
 @details	Сервис, Функции процессора, обработчик в МЕНЮ
 */
void MnServCPU_hand(void) { 
  uint32_t numb = ServCtrl.CPUNum;
  numb++;
  if (numb >= 2) numb = 0;
  ServCtrl.CPUNum = numb;
  ExeNum = (EXE_t)(EXE_CPUIMEM + numb);	// изменить номер функции "Выполнить"
  GenCtrl.Lock = 0;
  MnServCPU_disp();
}

/*!*****************************************************************************
 @brief		
 @details	Сервис, Функции процессора, индикатор в МЕНЮ
 */
void MnServCPU_disp(void) {
  static const uint8_t *CPUtext[2] = {"Внутр ОЗУ", "Внешн ОЗУ"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 1, DPYM2VV);
  uint32_t numb = ServCtrl.CPUNum;
  DpyRect_fill(pDpy, 58, 10, 0xff<<8 | VOFF_BKG);
//  DpyRoRect_fill(pDpy+9*mode, 40, 14, 0xff<<8|VON_BKG);
  DpyString_draw(pDpy, CPUtext[numb], VOFF_TXT, 0);
}

/*!*****************************************************************************
 @brief		
 @details	Сервис, Функции ПЛИС, обработчик в МЕНЮ
 */
void MnServPL_hand(void) { 
  uint32_t numb = ServCtrl.PLNum;
  numb++;
  if (numb >= 4)  numb = 0;
  ServCtrl.PLNum = numb;
  ExeNum = (EXE_t)(EXE_PLRECV + numb);	// изменить номер функции "Выполнить"
  GenCtrl.Lock = 0;
  MnServPL_disp();
}

/*!*****************************************************************************
 @brief		
 @details	Сервис, Функции ПЛИС, индикатор в МЕНЮ
 */
void MnServPL_disp(void) {
  static const uint8_t *PLtext[4] = {
    " Принять", "Сравн прн", "\x98\x98Записать", "Сравн зап"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 1,DPYM3VV);
  uint32_t numb = ServCtrl.PLNum;
  DpyRect_fill(pDpy, 58, 10, 0xff<<8 | VOFF_BKG);
//  DpyRoRect_fill(pDpy+9*mode, 40, 14, 0xff<<8|VON_BKG);
  DpyString_draw(pDpy, PLtext[numb], VOFF_TXT, 0);
}

/*!*****************************************************************************
 @brief		
 @details	Сервис, Функции ЖКИ, обработчик в МЕНЮ
 */
void MnServDpy_hand(void) {
  uint32_t numb = ServCtrl.DpyNum;
  numb++;
  if (numb >= 8)  numb = 0;
  ServCtrl.DpyNum = numb;
  ExeNum = (EXE_t)(EXE_DPYWHITE + numb);		// изменить номер функции "Выполнить"
  if (numb != 8)  GenCtrl.Lock = 1;
  else		  GenCtrl.Lock = 0; 
  MnServDpy_disp();
}

/*!*****************************************************************************
 @brief		
 @details	Сервис, Функции ЖКИ, индикатор в МЕНЮ
 */
void MnServDpy_disp(void) {
  static const uint8_t *LCDtext[] = {
    "  Белый", " \x98Черный", "  Шрифт", " Палитра",
    " Красный", " Зеленый", "  Синий", "\x98Восстанов"
    };
  uint8_t *pDpy = DPYPOS(DPYMHH + 1, DPYM4VV);
  uint32_t numb = ServCtrl.DpyNum;
  DpyRect_fill(pDpy, 58, 10, 0xff<<8 | VOFF_BKG);
//  DpyRoRect_fill(pDpy+9*mode, 40, 14, 0xff<<8|VON_BKG);
  DpyString_draw(pDpy, LCDtext[numb], VOFF_TXT, 0);
}

/*!*****************************************************************************
 @details	Сервис, Функция Выполнить, обработчик в МЕНЮ
 */
void MnServExe_hand(void) {
  ExeItem_hand[ExeNum]();
}

/*!*****************************************************************************
 @details	Сервис, Функция Выполнить, индикатор в МЕНЮ
 */
void MnServExe_disp(void) { }


//* КАНАЛ 1 *//

/*!*****************************************************************************
 @details	Канал 1, Режим, обработчик
 @note		Выкл <-> Вкл
 */
void MnChan1Mode_hand(void) {
  Chan1Ctrl.On ^= 1;
  MnChan1Mode_disp();
  Chan1Mode_sett();
  Chan1St_disp();		// отобразить в статусе
  PosChan1_draw(Chan1Ctrl.On);	// отобразить маркер
  CursMeasSt_disp();		// Показать/стереть измерения по курсорам
  SetCursSt_disp();		// Показать/стереть результат измерения
}

/*!*****************************************************************************
 @details	Канал 1, Режим, индикатор в МЕНЮ
 @note		Выкл <-> Вкл
 */
void MnChan1Mode_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM1RV), Chan1Ctrl.On);
}

/*!*****************************************************************************
 @details	Канал 1, Пробник, обработчик
 @note		
 */
void MnChan1Prb_hand(void) {
  INC_MAX(Chan1Ctrl.Prb, 3);
  MnChan1Prb_disp();
  Chan1St_disp();		// отобразить в статусе
  if (CursCtrl.On) SetCursSt_disp();  
}

/*!*****************************************************************************
 @details	Канал 1, Пробник, индикатор в МЕНЮ
 @note		
 */
void MnChan1Prb_disp(void) {
  DpyChanPrb_draw((uint32_t)Chan1Ctrl.Prb);
}

/*!*****************************************************************************
 @details	Канал 1, Инверсия, обработчик
 @note		
 */
void MnChan1Inv_hand(void) {
  if (Chan1Ctrl.On) {
    SigChan1_draw(0);
    PosChan1_draw(0);
  }
  Chan1Ctrl.Inv ^= 1;
  MnChan1Inv_disp();
  Chan1St_disp();
  if (Chan1Ctrl.On) {
    SigChan1_draw(3);
    PosChan1_draw(1);
  }
}

/*!*****************************************************************************
 @details	Канал 1, Инверсия, индикатор в МЕНЮ
 @note		
 */
void MnChan1Inv_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM3RV), Chan1Ctrl.Inv);
}

/*!*****************************************************************************
 @details	Канал 1, Связь, обработчик
 @note		
 */
void MnChan1Cplg_hand(void) {
  INC_MAX(Chan1Ctrl.Cplg, 2);
  MnChan1Cplg_disp();
  Chan1Cplg_sett();
  FpPos1_hand(0);		// Скорректировать позицию
  Chan1St_disp();		// отобразить в статусе
}

/*!*****************************************************************************
 @details	Канал 1, Связь, индикатор в МЕНЮ
 @note		
 */
void MnChan1Cplg_disp(void) {
  DpyChanCplg_draw(Chan1Ctrl.Cplg);
}

/*!*****************************************************************************
 @details	Канал 1, Ограничение полосы, обработчик
 @note		
 */
void MnChan1BWLim_hand(void) {
  Chan1Ctrl.BWLim ^= 1;
  MnChan1BWLim_disp();
  Chan1BWLim_sett();
  Chan1St_disp();			// отобразить в статусе
}

/*!*****************************************************************************
 @details	Канал 1, Ограничение полосы, индикатор в МЕНЮ
 @note		
 */
void MnChan1BWLim_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM5RV), Chan1Ctrl.BWLim);
}

#if 0
/*!*****************************************************************************
 @details	Канал 1, Заземление, обработчик
 @note		
 */
void MnChan1Gnd_hand(void) {
  Chan1Ctrl.Gnd ^= 1;
//  MnChan1Gnd_disp();
  Chan1Gnd_sett();
  Chan1St_disp();			// отобразить в статусе
}

/*!*****************************************************************************
 @details	Канал 1, Заземление, индикатор в МЕНЮ
 @note		
 */
void MnChan1Gnd_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM5RV), Chan1Ctrl.Gnd);
}
#endif

//* КАНАЛ 2 *//

/*!*****************************************************************************
 @details	Канал 2, Режим, обработчик
 @note		Выкл <-> Вкл
 */
void MnChan2Mode_hand(void) {
  Chan2Ctrl.On ^= 1;
  MnChan2Mode_disp();
  Chan2Mode_sett();
  Chan2St_disp();		// отобразить в статусе
  PosChan2_draw(Chan2Ctrl.On);	// отобразить маркер
  CursMeasSt_disp();		// Показать/стереть измерения по курсорам
  SetCursSt_disp();		// Показать/стереть результат измерения
}

/*!*****************************************************************************
 @details	Канал 2, Режим, индикатор в МЕНЮ
 @note		Выкл <-> Вкл
 */
void MnChan2Mode_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM1RV), Chan2Ctrl.On);
}

/*!*****************************************************************************
 @details	Канал 2, Пробник, обработчик
 @note		
 */
void MnChan2Prb_hand(void) {
  INC_MAX(Chan2Ctrl.Prb, 3);
  MnChan2Prb_disp();
  Chan2St_disp();
}

/*!*****************************************************************************
 @details	Канал 2, Пробник, индикатор в МЕНЮ
 @note		
 */
void MnChan2Prb_disp(void) {
  DpyChanPrb_draw((uint32_t)Chan2Ctrl.Prb);
  if (CursCtrl.On) SetCursSt_disp();
}

/*!*****************************************************************************
 @details	Канал 2, Инверсия, обработчик
 @note		
 */
void MnChan2Inv_hand(void) {
  if (Chan2Ctrl.On) {
    SigChan2_draw(0);
    PosChan2_draw(0);
  }
  Chan2Ctrl.Inv ^= 1;
  MnChan2Inv_disp();
  Chan2St_disp();
  if (Chan2Ctrl.On) {
    SigChan2_draw(3);
    PosChan2_draw(1);
  }
}

/*!*****************************************************************************
 @details	Канал 2, Инверсия, индикатор в МЕНЮ
 @note		
 */
void MnChan2Inv_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM3RV), Chan2Ctrl.Inv);
}

/*!*****************************************************************************
 @details	Канал 2, Связь, обработчик
 @note		
 */
void MnChan2Cplg_hand(void) {
  INC_MAX(Chan2Ctrl.Cplg, 2);
  MnChan2Cplg_disp();
  Chan2Cplg_sett();
  FpPos2_hand(0);		// Скорректировать позицию
  Chan2St_disp();		// отобразить в статусе
}

/*!*****************************************************************************
 @details	Канал 2, Связь, индикатор в МЕНЮ
 @note		
 */
void MnChan2Cplg_disp(void) {
  DpyChanCplg_draw(Chan2Ctrl.Cplg);
}

/*!*****************************************************************************
 @details	Канал 2, Ограничение полосы, обработчик
 @note		
 */
void MnChan2BWLim_hand(void) {
  Chan2Ctrl.BWLim ^= 1;
  MnChan2BWLim_disp();
  Chan2BWLim_sett();
  Chan2St_disp();			// отобразить в статусе
}

/*!*****************************************************************************
 @details	Канал 2, Ограничение полосы, индикатор в МЕНЮ
 @note		
 */
void MnChan2BWLim_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM5RV), Chan2Ctrl.BWLim);
}

#if 0
/*!*****************************************************************************
 @details	Канал 2, Заземление, обработчик
 @note		
 */
void MnChan2Gnd_hand(void) {
  Chan2Ctrl.Gnd ^= 1;
//  MnChan2Gnd_disp();
  Chan2Gnd_sett();
  Chan2St_disp();
}

/*!*****************************************************************************
 @details	Канал 2, Заземление, индикатор в МЕНЮ
 @note		
 */
void MnChan2Gnd_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM5RV), Chan2Ctrl.Gnd);
}
#endif

//__ Функции индикации в МЕНЮ, общие для двух каналов __//

#if 0
/*!*****************************************************************************
 @brief		Channel Ground Draw
 @details	Нарисовать Заземление в МЕНЮ
 @param		pDpy - указатель в памяти индикации
 @param		Gnd - заземление
 @note	
 */
void DpyChanGnd_draw(uint8_t *pDpy) {
  static const uint8_t *GndTxt = "Заземлен";
  DpyRoRect_fill(pDpy + 1, 55, 14, 0xFF << 8 | VON_BKG);
  DpyString_draw(pDpy + 320 * 3 + 2, GndTxt, VON_TXT, 0);
}
#endif

/*!*****************************************************************************
 @brief		Channel Probe draw
 @details	Нарисовать в МЕНЮ пробник
 @param		*pDpy
 @param		Prb
 @note	
 */
void DpyChanPrb_draw(uint32_t Prb) {
  static const uint8_t *PrbTxt[4] = {"1/100", "\x98\x98""1/10", " 1/1", " 10x"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 2, DPYM2RV);
  DpyRect_fill(pDpy, 58, 14, 0xFF << 8 | VOFF_BKG);
  DpyRoRect_fill(pDpy + 8 * Prb, 33, 14, 0xFF<<8 | VON_BKG);
  DpyString_draw(pDpy + 320 * 3 + 1 + 8 * Prb, PrbTxt[Prb], VON_TXT, 0);
}

/*!*****************************************************************************
 @brief		Channel Coupling Draw
 @details	Нарисовать в МЕНЮ связь в канале
 @param		pDpy - указатель в памяти индикации
 @param		Cplg - вид связи
 @note	
 */
void DpyChanCplg_draw(uint32_t Cplg) {
  static const uint8_t *CplgTxt[] = {"Заземл", "\x98Перем", " \x98Пост"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 2, DPYM4RV);
  DpyRect_fill(pDpy, 58, 14, 0xFF << 8 | VOFF_BKG);
  DpyRoRect_fill(pDpy + 6 * Cplg, 42, 14, 0xFF << 8 | VON_BKG);
  DpyString_draw(pDpy + 320 * 3 + 1 + 6 * Cplg, CplgTxt[Cplg], VON_TXT, 0);
}


//* МАТематическая ОБРаботка *//

/*!*****************************************************************************
 @details	Математика, Режим, обработчик
 */
void MnMathMode_hand(void) {
  MathCtrl.On ^= 1;		// Изменить состояние
  DpyWork_clear(MASK_MAT);
  MnMathMode_disp();
  Math_exe();
}

/*!*****************************************************************************
 @details	Математика, Режим, индикатор
 */
void MnMathMode_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM1RV), MathCtrl.On);  
  MathSt_disp();			// Индикатор состояния
}
 

/*!*****************************************************************************
 @details	Математика, Функция, обработчик
 */
void MnMathFun_hand(void) {
  INC_MAX(MathCtrl.Fun, 2);		// увеличить номер функции
  MnMathFun_disp();
  MnMathSour_disp();
  MnMathWind_disp();
  Math_exe();
}

/*!*****************************************************************************
 @details	Математика, Функция, индикатор в МЕНЮ
 */
void MnMathFun_disp(void) {
  static const uint8_t *FunTxt[3] = {"Сумма", "\x98Умнож", "Спектр"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 4, DPYM2RV);
  uint32_t fun = MathCtrl.Fun;  
  DpyRect_fill(pDpy, 56, 14, 0xFF << 8 | VOFF_BKG);
  DpyRoRect_fill(pDpy + 6 * fun, 40, 14, 0xFF << 8 | VON_BKG);
  DpyString_draw(pDpy + 320 * 3 + 1 + 6 * fun, FunTxt[fun], VON_TXT, 0);
}

/*!*****************************************************************************
 @details	Математика, Источник, обработчик
 @note		Выбирается для функции "Спектр"
 */
void MnMathSour_hand(void)
{
  if (MathCtrl.Fun == MATH_FFT) {
    MathCtrl.Sour ^= 1;
    MnMathSour_disp();
    Math_exe();
  }
}

/*!*****************************************************************************
 @details	Математика, Источник, индикатор в МЕНЮ
 @note		Индицируется для функции "Спектр"
 */
void MnMathSour_disp(void) {
  uint8_t *pDpy = DPYPOS(DPYMHH + 6, DPYM3RV);
  DpyRect_fill(pDpy, 52, 14, 0xFF << 8 | VOFF_BKG);
  if (MathCtrl.Fun == 2) {
    static const uint8_t *SourTxt[2] = {"Канал\x98""1", "Канал\x98""2"};
    DpyRoRect_fill(pDpy, 48, 14, 0xFF << 8 | VON_BKG);
    DpyString_draw(pDpy + 320 * 3 + 4, SourTxt[MathCtrl.Sour], VON_TXT, 0);
  }
}

/*!*****************************************************************************
 @details	Математика, Окно БПФ, обработчик
 */
void MnMathWind_hand(void)
{
  if (MathCtrl.Fun == MATH_FFT) {
    INC_MAX(MathCtrl.Wind, 4);
    MnMathWind_disp();
    Math_exe();
  }
}

/*!*****************************************************************************
 @details	Математика, Окно БПФ, индикатор в МЕНЮ
 */
void MnMathWind_disp(void)
{
  uint8_t *pDpy = DPYPOS(DPYMHH + 2, DPYM4RV);
  DpyRect_fill(pDpy, 58, 14, 0xFF << 8 | VOFF_BKG);
  if (MathCtrl.Fun == 2) {
    static const uint8_t *WindTxt[] = 
      {"Прямоуг", "Ханнинг", "Хэмминг", "Бартлетт", "Пл. верш"};
    DpyRoRect_fill(pDpy, 56, 14, 0xFF << 8 | VON_BKG);
    DpyString_draw(pDpy + 320 * 3 + 4, WindTxt[MathCtrl.Wind], VON_TXT, 0);
  }
}

/*!*****************************************************************************
 @details	Математика, Смещение, обработчик
 */
void MnMathOffs_hand(void) { }

/*!*****************************************************************************
 @details	Математика, Смещение, индикатор в МЕНЮ
 */
void MnMathOffs_disp(void) { }


//* РАЗВертка *//

/*!*****************************************************************************
 @details	Развертка, Режим, обработчик
 @note		
 */
void MnSweepMode_hand(void) {
  uint32_t temp = SweepCtrl.Mode;
  temp++;
  if (temp > 2) temp = 0;
  SweepCtrl.Mode = temp;
  MnSweepMode_disp();
  SweepMode_sett();
  SweepSt_disp();
  if (temp == 2) SweepCtrl.Stop = 1;
  else {
    SweepCtrl.Stop = 0;
    PL->STAT = PL_START_ACQUST; }
  StSp_disp();
}

/*!*****************************************************************************
 @details	Развертка, Режим, индикатор в МЕНЮ
 @note		
 */
void MnSweepMode_disp(void) {
  static const uint8_t *ModeTxt[3] = {" Авто", "\x98\x98Ждущ", "Однокр"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 1, DPYM1RV);
  uint32_t mode = SweepCtrl.Mode;
  DpyRect_fill(pDpy, 58, 14, 0xFF << 8 | VOFF_BKG);
  DpyRoRect_fill(pDpy + 9 * mode, 40, 14, 0xFF << 8 | VON_BKG);
  DpyString_draw(pDpy + 320 * 3 + 1 + 9 * mode, ModeTxt[mode], VON_TXT, 0);  
}

/*!
 *******************************************************************************
 * @details	Развертка, Прокрутка, обработчик
 * @note	
 */
void MnSweepRoll_hand(void) {
  SweepCtrl.Roll ^= 1;
  MnSweepRoll_disp();
  SweepRoll_sett();
  SweepSt_disp();
  Sweep_init();
  DpyWork_clear(MASK_MATSIG);
}

/*!*****************************************************************************
 @details	Развертка, Прокрутка, индикатор в МЕНЮ
 @note	
 */
void MnSweepRoll_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM2RV), SweepCtrl.Roll);
}

/*!*****************************************************************************
 @details	Развертка, Предзапуск, обработчик
 @note	
 */
void MnSweepPret_hand(void) {
// Задать функцию ручки УСТАНОВКА
  if (SetNum != SET_PRET) SetNum = SET_PRET;
  else	SetNum = SET_VOID;
  MnSweepPret_disp();
}

/*!*****************************************************************************
 @details	Развертка, Предзапуск, индикатор в Меню
 @note	
 */
void MnSweepPret_disp(void) {
  bool OffOn = false;			// для индикации
  if (SetNum == SET_PRET)  OffOn = true;
  DpySetting_draw(DPYPOS(DPYMHH + 2, DPYM3RV), OffOn);
}

/*!*****************************************************************************
 @details	Развертка, X-Y, обработчик
 @note	
 */
void MnSweepXY_hand(void) {
  SweepCtrl.XY ^= 1;
  MnSweepXY_disp();
// выводить кадр в координатах X(Ch1)-Y(Ch2)
}

/*!*****************************************************************************
 @details	Развертка, X-Y, индикатор в МЕНЮ
 @note	
 */
void MnSweepXY_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM5RV), SweepCtrl.XY);
}

//* СИНХРонизация *//

/*!*****************************************************************************
 @details	Синхронизация, Источник, обработчик
 @note	
 */
void MnTrigSour_hand(void) {
  uint32_t sour = TrigCtrl.Sour;
  sour++;  if (sour > 3) sour = 0;
  TrigCtrl.Sour = sour;
  MnTrigSour_disp();
  TrigSour_sett();
  TrigSt_disp();
}

/*!*****************************************************************************
 @details	Синхронизация, Источник, индикатор в МЕНЮ
 @note	
 */
void MnTrigSour_disp(void) {
  static const uint8_t *SrcTxt[4] = {"Сеть", "Кан1", "Кан2", "Внеш"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 1, DPYM1RV);
  uint32_t sour = TrigCtrl.Sour;
  DpyRect_fill(pDpy, 58, 14, 0xff << 8 | VOFF_BKG);
  DpyRoRect_fill(pDpy + 10 * sour, 28, 14, 0xff << 8 | VON_BKG);
  DpyString_draw(pDpy + 320 * 3 + 1 + 10 * sour, SrcTxt[sour], VON_TXT, 0);    
}

/*!*****************************************************************************
 @details	Синхронизация, перепад, обработчик
 @note	
 */
void MnTrigSlp_hand(void) {
  TrigCtrl.Rise ^= 1;
  MnTrigSlp_disp();
  TrigSlp_sett();
  TrigSt_disp();
}

/*!*****************************************************************************
 @details	Синхронизация, Перепад, индикатор в МЕНЮ
 @note	
 */
void MnTrigSlp_disp(void) {
  uint8_t *pDpy = DPYPOS(DPYMHH + 1, DPYM2RV);
  DpyRect_fill(pDpy, 58, 14, 0xff << 8 | VOFF_BKG);
  DpyRoRect_fill(pDpy + 1 + 28 * TrigCtrl.Rise, 28, 14, 0xff << 8 | VON_BKG);  
  if (!TrigCtrl.Rise) {
    DpySpec_draw(pDpy + 320 * 3 + 10, SS_FALL, 0xff, VON_TXT);
    DpySpec_draw(pDpy + 320 * 3 + 40, SS_RISE, 0xff, VOFF_TXT);
  }
  else {
    DpySpec_draw(pDpy + 320 * 3 + 10, SS_FALL, 0xff, VOFF_TXT);
    DpySpec_draw(pDpy + 320 * 3 + 40, SS_RISE, 0xff, VON_TXT);
  }
}

/*!*****************************************************************************
 @details	Синхронизация, Связь, обработчик
 @note	
 */
void MnTrigCplg_hand(void) {
  uint32_t cplg = TrigCtrl.Cplg;
  cplg++;  if (cplg > 3) cplg = 0;
  TrigCtrl.Cplg = cplg;
  MnTrigCplg_disp();
  TrigCplg_sett();
  TrigSt_disp();
}

/*!*****************************************************************************
 @details	Синхронизация, Связь, индикатор в МЕНЮ
 @note	
 */
void MnTrigCplg_disp(void) {
  static const uint8_t *CplgTxt[4] = {
    " \x98\x98НЧ", "\x98\x98Пост", "Перем", " \x98\x98ВЧ"
  };
  uint8_t *pDpy = DPYPOS(DPYMHH + 1, DPYM4RV);
  uint32_t cplg = TrigCtrl.Cplg;
  DpyRect_fill(pDpy, 58, 14, 0xff << 8 | VOFF_BKG);
  DpyRoRect_fill(pDpy + 8 * cplg, 34, 14, 0xff << 8 | VON_BKG);
  DpyString_draw(pDpy + 320 * 3 + 1 + 8 * cplg, CplgTxt[cplg], VON_TXT, 0);    
}

/*!*****************************************************************************
 @details	Синхронизация, Шумоподавление, обработчик
 @note	
 */
void MnTrigNRej_hand(void) {
  TrigCtrl.NRej ^= 1;
  MnTrigNRej_disp();
  TrigNRej_sett();
}

/*!*****************************************************************************
 @details	Синхронизация, Шумоподавление, индикатор в МЕНЮ
 @note	
 */
void MnTrigNRej_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM5RV), TrigCtrl.NRej);
}

//* ДИСПЛЕЙ *//

/*!*****************************************************************************
 @details	Дисплей, Помощь, обработчик
 @note	
 */
void MnDispHelp_hand(void) {
  DispCtrl.HelpEna ^= 1;
  MnDispHelp_disp();
}

/*!*****************************************************************************
 @details	Дисплей, Помощь, индикатор в МЕНЮ
 @note	
 */
void MnDispHelp_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM1RV), DispCtrl.HelpEna);
}

/*!*****************************************************************************
 @details	Дисплей, Вывод Точки/Вектора, обработчик
 @note	
 */
void MnDispVect_hand(void) {
  SigChan1_draw(0);
  SigChan2_draw(0);
  DispCtrl.Vect ^= 1;
  MnDispVect_disp();
//  SigChan1_draw(3);
//  SigChan2_draw(3);
  SigChans_redraw();
}

/*!*****************************************************************************
 @details	Дисплей, Вывод Точки/Вектора, индикатор в МЕНЮ
 @note	
 */
void MnDispVect_disp(void) {
  static const uint8_t *VectTxt[2] = {" Точки", "Векторы"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 1, DPYM2RV);
  DpyRect_fill(pDpy, 58, 14, 0xff << 8 | VOFF_BKG);
  DpyRoRect_fill(pDpy + 12 * DispCtrl.Vect, 46, 14, 0xff << 8 | VON_BKG);
  DpyString_draw(pDpy + 320 * 3 + 12 * DispCtrl.Vect + 1, 
	VectTxt[DispCtrl.Vect], VON_TXT, 0);
}

/*!*****************************************************************************
 @details	Дисплей, Шкала, обработчик
 @note	
 */
void MnDispScale_hand(void) {
  INC_MAX(DispCtrl.Scale, 3);
  MnDispScale_disp();
  DpyScale_draw();  
}

/*!*****************************************************************************
 @details	Дисплей, Шкала, индикатор в МЕНЮ
 @note		0-Frame, 1-Cross(+Frame), 2-Grid(+Frame), 3-Cross+Grid(+Frame)
 */
void MnDispScale_disp(void) {
  static const uint8_t *ScaleText[4] = {"Рамка", "Центр", "Сетка", " Все"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 2, DPYM3RV);
  DpyRect_fill(pDpy, 58, 14, 0xFF << 8 | VOFF_BKG);
  DpyRoRect_fill(pDpy + 8 * DispCtrl.Scale, 33, 14, 0xFF << 8 | VON_BKG);
  DpyString_draw(pDpy + 320 * 3 + 1 + 8 * DispCtrl.Scale, 
		ScaleText[DispCtrl.Scale], VON_TXT, 0);
}

/*!*****************************************************************************
 @details	Дисплей, Очистить, обработчик
 @note
 */
void MnDispClear_hand(void) {
  DpyWork_clear(MASK_MATSIG);
}

/*!*****************************************************************************
 @details	Дисплей, Очистить, индикатор
 @note
 */
void MnDispClear_disp(void) { }


//* СБОР ИНФормации *//

/*!*****************************************************************************
 @details	Фильтр (Высокого разрешения), обработчик
 @note	
 */
void MnAcquFilt_hand(void) {
  INC_MAX(AcquCtrl.Filt, 3);
  MnAcquFilt_disp();
  AcquFilt_sett();
}

/*!*****************************************************************************
 @details	Фильтр (Высокого разрешения), индикатор
 @note	
 */
void MnAcquFilt_disp(void) {
  static const uint8_t *FiltText[4] = {
	"Выкл",
	"2\x98\x98Выб",
	"4\x98\x98Выб",
	"8\x98\x98Выб"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 2, DPYM1RV);
  DpyRect_fill(pDpy, 58, 14, 0xFF << 8 | VOFF_BKG);
  DpyRoRect_fill(pDpy + 8 * AcquCtrl.Filt, 33, 14, 0xFF << 8 | VON_BKG);
  DpyString_draw(pDpy + 320 * 3 + 1 + 8 * AcquCtrl.Filt,
		FiltText[AcquCtrl.Filt], VON_TXT, 0);
}

/*!*****************************************************************************
 @details	Пиковый детектор, обработчик
 @note	
 */
void MnAcquPeak_hand(void) {
  AcquCtrl.Peak ^= 1;
  MnAcquPeak_disp();
  AcquPeak_sett();
}

/*!*****************************************************************************
 @details	Пиковый детектор, индикатор
 @note	
 */
void MnAcquPeak_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM2RV), AcquCtrl.Peak);
}

/*!*****************************************************************************
 @details	Усреднение, обработчик
 @note	
 */
void MnAcquAver_hand(void) {
// Задать функцию ручки УСТАНОВКА
  if (SetNum != SET_AVER) SetNum = SET_AVER;
  else			  SetNum = SET_VOID;
  MnAcquAver_disp();
}

/*!*****************************************************************************
 @details	Усреднение, индикатор в МЕНЮ (только, когда свое Меню включено)
 @note	
 */
void MnAcquAver_disp(void) {
  if (MenuNum == MENU_ACQU && GenCtrl.Menu) {
    uint8_t *pDpy = DPYPOS(DPYMHH + 2, DPYM3RV);  
    bool On = false;			// для индикации
    if (SetNum == SET_AVER) On = true;
    DpyRotor_draw(pDpy, On);

    static const uint8_t *AverTxt[13] = {
      "1/1", "1/2", "1/4", "1/8", "1/16",
      "1/32", "1/64", "1/128", "1/256",
      "1/512", "1/1024", "1/2048", "1/4096"
    };

    // DpyRect_fill(pDpy, 58, 14, 0xff<<8|VOFF_BKG);
    DpyRoRect_fill(pDpy + 17, 40, 14, 0xff<<8 | VON_BKG);
    DpyString_draw(pDpy + 18 + 320 * 3, AverTxt[AcquCtrl.Aver >> 2], VON_TXT, 0);
  }
}

/*!*****************************************************************************
 @details	Накопление (послесвечение), обработчик
 @note	
 */
void MnAcquPers_hand(void)
{
  AcquCtrl.Pers ^= 1;
  MnAcquPers_disp();
}

/*!*****************************************************************************
 @details	Накопление (послесвечение), индикатор
 @note	
 */
void MnAcquPers_disp(void)
{
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM4RV), AcquCtrl.Pers);
}


//* КУРСОРЫ *//

/*!*****************************************************************************
 @details	
 @note		
 */
void MnCursMode_hand(void) {
  SET_t static SetCurs = SET_VOID;	// Номер функции установки курсоров
  CursCtrl.On ^= 1;
  MnCursMode_disp();		// Индицировать в Меню
  if (!CursCtrl.On) {
    SetCurs = SetNum;		// Сохранить номер функции установки курсоров
    SetNum = SET_VOID;		// Убрать функцию установки
  }
  else {
    MeasCtrl.On = 0;
    SetNum = SetCurs;		// Восстановить номер функции установки курсоров
  }  
  MnCursSel1_disp();
  MnCursSel2_disp();
  MnCursSelTr_disp();
  
  Curs_draw(CursCtrl.On);	// Нарисовать на экране
  CursMeasSt_disp();		// Индицировать в Статусе
  SetCursSt_disp();		// Показать/стереть результат измерения
}

/*!*****************************************************************************
 @details	
 @note		
 */
void MnCursMode_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM1RV), CursCtrl.On);
}

/*!*****************************************************************************
 @details	
 @note		
 */
void MnCursPara_hand(void) {
  CursCtrl.xY ^= 1;
  MnCursPara_disp();
  CursMeasSt_disp();		// Индицировать в Статусе
  SetCursSt_disp();		// Показать/стереть результат измерения  
}

/*!*****************************************************************************
 @details	
 @note		
 */
void MnCursPara_disp(void) {
  static const uint8_t *MeasTxt[] = {"\x98\x98"S_DELT"X", "\x98\x98"S_DELT"Y"};
  uint8_t *pDpy = DPYPOS(DPYMHH + 2, DPYM2RV);
  if (!CursCtrl.xY) {
    DpyRoRect_fill(pDpy, 28, 14, 0xff << 8 | VON_BKG);
    DpyString_draw(pDpy + 320 * 3 + 1, MeasTxt[0], VON_TXT, 0);
    DpyRect_fill(pDpy + 28, 28, 14, 0xff<<8 | VOFF_BKG);
    DpyString_draw(pDpy + 320 * 3 + 31, MeasTxt[1], VOFF_TXT, 0);
  }
  else {
    DpyRect_fill(pDpy, 28, 14, 0xff<<8 | VOFF_BKG);
    DpyString_draw(pDpy + 320 * 3 + 1, MeasTxt[0], VOFF_TXT, 0);
    DpyRoRect_fill(pDpy + 28, 28, 14, 0xff << 8 | VON_BKG);
    DpyString_draw(pDpy + 320 * 3 + 31, MeasTxt[1], VON_TXT, 0);
  }
}

/*!*****************************************************************************
 @details	Выбрать первый курсор
 @note		
 */
void MnCursSel1_hand(void) {
  SetNum = SET_CURS1;
  MnCursSel1_disp();
  MnCursSel2_disp();
  MnCursSelTr_disp();
}

/*!*****************************************************************************
 @details	
 @note		
 */
void MnCursSel1_disp(void) {
  bool On = false;			// для индикации
  if (SetNum == SET_CURS1) On = true;
  DpySetting_draw(DPYPOS(DPYMHH + 2, DPYM3RV), On);  
}

/*!*****************************************************************************
 @details	Выбрать второй курсор
 @note		
 */
void MnCursSel2_hand(void) {
  SetNum = SET_CURS2;
  MnCursSel1_disp();
  MnCursSel2_disp();
  MnCursSelTr_disp();
}

/*!*****************************************************************************
 @details	
 @note		
 */
void MnCursSel2_disp(void) {
  bool On = false;
  if (SetNum == SET_CURS2) On = true;
  DpySetting_draw(DPYPOS(DPYMHH + 2, DPYM4RV), On);
}

/*!*****************************************************************************
 @details	Выбрать оба курсора (слежение)
 @note		
 */
void MnCursSelTr_hand(void) {
  SetNum = SET_CURSTR;
  MnCursSel1_disp();
  MnCursSel2_disp();
  MnCursSelTr_disp();
}

/*!*****************************************************************************
 @details	
 @note		
 */
void MnCursSelTr_disp(void) {
  bool On = false;
  if (SetNum == SET_CURSTR) On = true;
  DpySetting_draw(DPYPOS(DPYMHH + 2, DPYM5RV), On);
}

/*!*****************************************************************************
 @details	
 @note		
 */
void CursMeasSt_disp(void) {
  if (MeasCtrl.On) return;
  if (!CursCtrl.On) 
    DPY_RECT(DPYPARH, DPYPAR1V,
	     319,     DPYPAR2V + 9, 0xFF, BLANK);
  else {
    if (!CursCtrl.xY) {
      DpyStringPad_draw(DPYPOS(DPYPARH, DPYPAR1V),
			"  " S_DELT "X=", 0x80800000, true);
      DpyStringPad_draw(DPYPOS(DPYPARH, DPYPAR2V),
			"1/" S_DELT "X=", 0x80800000, true);
    }
    else {
      if (!Chan1Ctrl.On)
        DPY_RECT(DPYPARH, DPYPAR1V,
		 319,     DPYPAR1V + 9, 0xFF, BLANK);
      else
        DpyStringPad_draw(DPYPOS(DPYPARH, DPYPAR1V),
			" 1" S_DELT "Y=", 0x80800000, true);
      if (!Chan2Ctrl.On)
        DPY_RECT(DPYPARH, DPYPAR2V,
		 319,     DPYPAR2V + 9, 0xFF, BLANK);
      else DpyStringPad_draw(DPYPOS(DPYPARH, DPYPAR2V),
			" 2" S_DELT "Y=", 0x80800000, true);
    }			
  }  
}


//* ИЗМЕРения *//

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMeasMode_hand(void) {
  MeasCtrl.On ^= 1;
  if (MeasCtrl.On) {
// Выключить и погасить курсоры
    CursCtrl.On = 0;
    Curs_draw(0);
// Стереть функции управления курсорами от энкодера
    if (SetNum == SET_CURS1
     || SetNum == SET_CURS2
     || SetNum == SET_CURSTR)
      SetNum = SET_VOID;
  }
  MnMeasMode_disp();
  SigPar_meas();  
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMeasMode_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM1RV), MeasCtrl.On);
  MeasPar1St_disp();
  MeasPar2St_disp();
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMeasSour_hand(void) {
  MeasCtrl.Sour ^= 1;
  MnMeasSour_disp();
  SigPar_meas();
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMeasSour_disp(void) {
  static const uint8_t *SourTxt[] = {"Кан1", "Кан2"};
  uint8_t *pDpy = DPYPOS(DPYMHH, DPYM2RV);
  DpyRect_fill(pDpy, 60, 14, 0xFF << 8 | VOFF_BKG);
  if (!MeasCtrl.Sour) {
    DpyRoRect_fill(pDpy + 2, 28, 14, 0xFF << 8 | VON_BKG);
    DpyString_draw(pDpy + 3 + 320 * 3, SourTxt[0], VON_TXT, false);
    DpyString_draw(pDpy + 3 + 56 - 28 + 320 * 3, SourTxt[1], VOFF_TXT, false);
  }
  else {
    DpyString_draw(pDpy + 3 + 320 * 3, SourTxt[0], VOFF_TXT, false);
    DpyRoRect_fill(pDpy + 2 + 56 - 28, 28, 14, 0xFF << 8 | VON_BKG);
    DpyString_draw(pDpy + 3 + 56 - 28 + 320 * 3, SourTxt[1], VON_TXT, false);
  }
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMeasPar1_hand(void) {
// Задать функцию ручки УСТАНОВКА
  if (SetNum != SET_PAR1) SetNum = SET_PAR1;
  else			  SetNum = SET_VOID;
  MnMeasPar1_disp();
  MnMeasPar2_disp();
  SigPar_meas();
}

static const uint8_t *ParTxt[] = {
  "Период", " Част.",
  "Длит -", "Длит +",
  "Вр.спд", "Вр.нар",
  "Миним.", "Максм.",
  "Размах", "Средн."
};

static const uint8_t *ParStTxt[] = {
  "   T=", "   F=",
  "  T-=", "  T+=",
  "Tспд=", "Tнар=",
  "Vмин=", "Vмкс=",
  "Vраз=", "Vсрд="
};

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMeasPar1_disp(void) {
  if (MenuNum == MENU_MEAS && GenCtrl.Menu) {
    uint8_t *pDpy = DPYPOS(DPYMHH + 2, DPYM3RV);  
    bool On = false;			// для индикации
    if (SetNum == SET_PAR1) On = true;
    DpyRotor_draw(pDpy, On);

    DpyRoRect_fill(pDpy + 16, 42, 14, 0xFF << 8 | VON_BKG);
    DpyString_draw(pDpy + 18 + 320 * 3, ParTxt[MeasCtrl.Par1 >> 2], VON_TXT, false);
  }
  MeasPar1St_disp();
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMeasPar2_hand(void) {
// Задать функцию ручки УСТАНОВКА
  if (SetNum != SET_PAR2) SetNum = SET_PAR2;
  else			  SetNum = SET_VOID;
  MnMeasPar1_disp();
  MnMeasPar2_disp();
  SigPar_meas();
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMeasPar2_disp(void) {
  if (MenuNum == MENU_MEAS && GenCtrl.Menu) {
    uint8_t *pDpy = DPYPOS(DPYMHH + 2, DPYM4RV);  
    bool On = false;			// для индикации
    if (SetNum == SET_PAR2) On = true;
    DpyRotor_draw(pDpy, On);

    DpyRoRect_fill(pDpy + 16, 42, 14, 0xFF << 8 | VON_BKG);
    DpyString_draw(pDpy + 18 + 320 * 3, ParTxt[MeasCtrl.Par2 >> 2], VON_TXT, false);
  }
  MeasPar2St_disp();
}


/*!*****************************************************************************
 @brief		Measurement Parameter 1 display in Status
 @details	
 @note		
 */
void MeasPar1St_disp(void) {
  if (CursCtrl.On) return;	// Не трогать, если есть курсоры
  if (!MeasCtrl.On) 
    DPY_RECT(DPYSAH + 200, DPYS1VV,
	     319,          DPYS1VV + 10, 0xFF, BLANK);
  else {
    DpyStringPad_draw(DPYPOS(DPYSAH + 200, DPYS1VV),
			ParStTxt[MeasCtrl.Par1 >> 2], 0xE2E20000, true); 
  }  
}

/*!*****************************************************************************
 @brief		Measurement Parameter 2 display in Status
 @details	
 @note		
 */
void MeasPar2St_disp(void) {
  if (CursCtrl.On) return;	// Не трогать, если есть курсоры
  if (!MeasCtrl.On) 
    DPY_RECT(DPYSAH + 200, DPYS2VV,
	     319,          DPYS2VV + 10, 0xFF, BLANK);
  else {
    DpyStringPad_draw(DPYPOS(DPYSAH + 200, DPYS2VV),
			ParStTxt[MeasCtrl.Par2 >> 2], 0xE2E20000, true); 
  }  
}

//* ПАМЯТЬ *//

/*!*****************************************************************************
 @details	ПАМЯТЬ, Размер, обработчик
 @note	
 */
void MnMemSize_hand(void) {
// Очистить память сигналов
  Sig_clear();

// Очистить изображения математики, сигналов */
  DpyWork_clear(MASK_CURSMATSIG);

  // Pret_draw(false);			// Стереть старое положение предзапуска
  MemBarPos_draw(false);		// Стереть позицию на полосе памяти
  // if (Chan1Ctrl.On) SigChan1_draw(0);	// Очистить старое изображение сигнала 1
  // if (Chan2Ctrl.On) SigChan2_draw(0);	// Очистить старое изображение сигнала 2
  // if (CursCtrl.On) Curs_draw(false);	// стереть курсоры

// Вычислить смещение позиции, изменить размер памяти, изменить позицию 
  int32_t ofs = (SweepCtrl.Pret << MemCtrl.SizeNum + 5) - SweepCtrl.TimePos;
  // INC_MAX(MemCtrl.SizeNum, 5);
  // if (!MemCtrl.SizeNum) MemCtrl.SizeNum = 1;	// Пропустить 0.5K
  INC_MAX(MemCtrl.SizeNum, 4);
  int32_t pos = (SweepCtrl.Pret << MemCtrl.SizeNum + 5) - ofs;

// Ограничить позицию пределами памяти
  int32_t lmarg = 0;
  int32_t rmarg = (1024 << MemCtrl.SizeNum) - 300;
  if (pos < lmarg) pos = lmarg;
  if (pos > rmarg) pos = rmarg;
  SweepCtrl.TimePos = pos;

// Ограничить курсоры
  if (CursCtrl.On) {
    SetCursTr_hand(0);
    Curs_draw(true);	// Нарисовать курсоры  
  }

// Сбросить номер памяти на первую зону
  MemCtrl.BufNum = 0;
  
  // SigChans_redraw();		// Перерисовать сигнал
  
  MnMemSize_disp();
  MnMemNum_disp();
  MemSizeBuf_disp();		// Написать в заголовке
  // Pret_draw(true);		// Нарисовать новое положение предзапуска
  MemBarPos_draw(true);		// Нарисовать позицию на полосе памяти

  MemSize_sett();  
  SetPret_sett();		// Задать предзапуск в ПЛИС
  // SetPret_hand(0);	// Изменить (ограничить и установить) положение предзапуска
  // PL->STAT = PL_START_ACQUST;		// Start new frame
  Sweep_init();

}

  static const uint8_t *MemSizeTxt[] = {
    // "0.5K",
    " 1K ",
    " 2K ",
    " 4K ",
    " 8K ",
    "\x98\x98""16K"
  };

  static const uint8_t *MemNumTxt[] = {
    " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8",
    " 9", "10", "11", "12", "13", "14", "15", "16",
    "17", "18", "19", "20", "21", "22", "23", "24",
    "25", "26", "27", "28", "29", "30", "31", "32"
  };
  
/*!*****************************************************************************
 @details	ПАМЯТЬ, Размер, индикатор в своем МЕНЮ
 @note	
 */
void MnMemSize_disp(void)
{
  uint8_t *pDpy = DPYPOS(DPYMHH, DPYM1RV);
  DpyRect_fill(pDpy, 60, 14, 0xFF << 8 | VOFF_BKG);  
  DpyRoRect_fill(pDpy + 3 * (5 - MemCtrl.SizeNum), 30 + 6 * MemCtrl.SizeNum,
      14, 0xFF << 8 | VON_BKG);
    DpyString_draw(pDpy + 18 + 320 * 3, MemSizeTxt[MemCtrl.SizeNum], VON_TXT, 0); 
}

/*!*****************************************************************************
 @details	ПАМЯТЬ, Размер, индикатор в строке заголовка
 @note	
 */
void MemSizeBuf_disp(void)
{
  uint8_t *pDpy = DPYPOS(DPYWAH + 200, 1);
  DpyStringPad_draw(pDpy, MemSizeTxt[MemCtrl.SizeNum], 0x0A0A0000, false);
  // DpyStringPad_draw(pDpy + 24, MemNumTxt[MemCtrl.BufNum], 0x1F1F0000, false);
}

/*!*****************************************************************************
 @brief		
 @details	
 @param		
 @return	
 @note		
 */
void MnMemNum_hand(void) {
  if (SetNum != SET_BUFNUM) SetNum = SET_BUFNUM;
  else			    SetNum = SET_VOID;
  MnMemNum_disp();  
}



/*!*****************************************************************************
 @brief		
 @details	
 @param		
 @return	
 @note		
 */
void MnMemNum_disp(void) {
  if (MenuNum == MENU_MEM && GenCtrl.Menu) {
    uint8_t *pDpy = DPYPOS(DPYMHH, DPYM2RV);
    bool On = false;
    if (SetNum == SET_BUFNUM) On = true;
    DpyRotor_draw(pDpy + 2, On);
    
    // DpyRect_fill(pDpy, 60, 14, 0xFF << 8 | VOFF_BKG);
    DpyRoRect_fill(pDpy + 36, 20, 14, 0xFF << 8 | VON_BKG);
    DpyString_draw(pDpy + 39 + 320 * 3, MemNumTxt[MemCtrl.BufNum], VON_TXT, 0);
  }
}
  


/*!*****************************************************************************
 @brief		
 @details	Выбрать функцию сохранения или загрузки сигнала
 @note		
 */
void MnMemSig_hand(void) {
  uint32_t num = MemItem;
  num++;
  if (num > SIG2LOAD) num = 0;
  MemItem = (MEMEXE_t)num;
  MnMemSig_disp();
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMemSig_disp(void) {
  static const uint8_t *MemFunTxt[] = {
    "Сохр. 1", "Сохр. 2", "Загр. 1", "Загр. 2"
  };
  uint8_t *pDpy = DPYPOS(DPYMHH, DPYM3RV);
  DpyRoRect_fill(pDpy + 4, 52, 14, 0xFF << 8 | VON_BKG);
  DpyString_draw(pDpy + 8 + 3 * 320, MemFunTxt[MemItem], VON_TXT, 0);
  
  pDpy = DPYPOS(DPYMHH, DPYM5RV);  
  DpyRect_fill(pDpy, 60, 14, 0xFF << 8 | VOFF_BKG);
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMemSet_hand(void) {

}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMemSet_disp(void) {

}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMemExe_hand(void) {
  uint8_t *pDpy = DPYPOS(DPYMHH, DPYM5RV);  
  DpyRect_fill(pDpy, 60, 14, 0xFF << 8 | VOFF_BKG);
  MemFun[MemItem]();
  DpyString_draw(pDpy + 2 + 3 * 320,
	"Выполнено", VOFF_TXT, false);
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnMemExe_disp(void) {

}

/*!*****************************************************************************
 @brief		
 @details	Выполнить сохранение сигнала
 @note		
 */
void Sig1_save(void) {
  uint8_t *pSigCh1 = (uint8_t *)SigChan1.Buf16K;
  uint8_t *pSigCh2 = (uint8_t *)SigChan2.Buf16K;
  SweepCtrl.Stop = 1;
  SFMSect_erase(SFMSIG1);		// Стереть сектор 64 КБ
  for (uint32_t i = 128; i--; ) {	// 128 страниц по 256 байтов
    SFMPage_progr(pSigCh1 + 256 * i, SFMSIG1 + 256 * i, 256);
  }
  for (uint32_t i = 128; i--; ) {	// 128 страниц по 256 байтов
    SFMPage_progr(pSigCh2 + 256 * i, SFMSIG1 + 0x8000 + 256 * i, 256);  
  }
  SweepCtrl.Stop = 0;
}

/*!*****************************************************************************
 @brief		
 @details	Выполнить сохранение сигнала
 @note		
 */
void Sig2_save(void) {
  uint8_t *pSigCh1 = (uint8_t *)SigChan1.Buf16K;
  uint8_t *pSigCh2 = (uint8_t *)SigChan2.Buf16K;
  SweepCtrl.Stop = 1;
  SFMSect_erase(SFMSIG2);		// Стереть сектор 64 КБ
  for (uint32_t i = 128; i--; ) {	// 128 страниц по 256 байтов
    SFMPage_progr(pSigCh1 + 256 * i, SFMSIG2 + 256 * i, 256);
  }
  for (uint32_t i = 128; i--; ) {	// 128 страниц по 256 байтов
    SFMPage_progr(pSigCh2 + 256 * i, SFMSIG2 + 0x8000 + 256 * i, 256);  
  }
  SweepCtrl.Stop = 0;
}

/*!*****************************************************************************
 @brief		
 @details	Выполнить загрузку сигнала
 @note		
 */
void Sig1_load(void) {
  uint8_t *pSigCh1 = (uint8_t *)SigChan1.Buf16K;
  uint8_t *pSigCh2 = (uint8_t *)SigChan2.Buf16K;
  SweepCtrl.Stop = 1;
  StSp_disp();
  SFMRead_start(SFMSIG1);
  for (uint32_t i = 1 << 15; i--; ) {	// Читать 32 КБ
    *pSigCh1++ = SFMByte_read();
  }
  SFMRead_stop();
  SFMRead_start(SFMSIG1 + 0x8000);
  for (uint32_t i = 1 << 15; i--; ) {	// Читать 32 КБ
    *pSigCh2++ = SFMByte_read();
  }
  SFMRead_stop();
  // DpyWork_clear(MASK_MATHSIG);
  SigChans_redraw();
  // Math_exe();
  SigPar_meas();			// Измерения (если есть)
}

/*!*****************************************************************************
 @brief		
 @details	Выполнить загрузку сигнала
 @note		
 */
void Sig2_load(void) {
  uint8_t *pSigCh1 = (uint8_t *)SigChan1.Buf16K;
  uint8_t *pSigCh2 = (uint8_t *)SigChan2.Buf16K;
  SweepCtrl.Stop = 1;
  StSp_disp();
  SFMRead_start(SFMSIG2);
  for (uint32_t i = 1 << 15; i--; ) {	// Читать 32 КБ
    *pSigCh1++ = SFMByte_read();
  }
  SFMRead_stop();
  SFMRead_start(SFMSIG2 + 0x8000);
  for (uint32_t i = 1 << 15; i--; ) {	// Читать 32 КБ
    *pSigCh2++ = SFMByte_read();
  }
  SFMRead_stop();
  // DpyWork_clear(MASK_MATHSIG);
  SigChans_redraw();
  // Math_exe();
  SigPar_meas();			// Измерения (если есть)
}



//* УТИЛИТЫ *//

/*!*****************************************************************************
 @brief		
 @details	Калибратор Включить / Выключить (Постоянное напряжение)
 @note		Бит 0 в байте для панели управления включает калибратор
 */
void MnUtilCal_hand(void) {
  GenCtrl.FPCal ^= 1;
  if (GenCtrl.FPCal) PanCtrl.TxBuf |=  0x01;
  else		     PanCtrl.TxBuf &= ~0x01;
  PanCtrl.TxBuf |= 0x40;
  USART3->DR = PanCtrl.TxBuf;
  MnUtilCal_disp();
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnUtilCal_disp(void) {
  DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM1RV), GenCtrl.FPCal);
}

/*!*****************************************************************************
 @brief		Balance
 @details	
 @note		На диапазонах 2mV...20mV включить DC, AC.
		Заземление включить на 2mV, и на всех остальных.
		измерить напряжение, отнять от ожидаемого.
		Смещение мельче сигнала в 2 раза, поэтому накопленное значение
		делится на половину от количества накоплений
 \attention	Проверять GenCtrl.Bal нет смысла
 */
void MnUtilBal_hand(void) {
#define WGT 64		// average weight
#define DTM 8		// dead time

  MnUtilBal_disp();
///  osMutexRelease(MutDpy_id);
///  osSignalSet(Main_id, 1);  		// запрос на индикацию
  
// Выполнять только, если не включен  
  if (!GenCtrl.Bal) {
// Включить баланс, если его нет
    GenCtrl.Bal = true;
// Включить оба канала
    Chan1Ctrl.On = true;
    Chan2Ctrl.On = true;
    Chan1Mode_sett();
    Chan2Mode_sett();
    
// Задать ограничение полосы
    Chan1Ctrl.BWLim = 1;
    Chan2Ctrl.BWLim = 1;
    Chan1BWLim_sett();
    Chan2BWLim_sett();
    
/* Установить позицию в середину экрана */    
    PosChan1_draw(false);
    PosChan2_draw(false);
    Chan1Ctrl.SigPos = 512;
    Chan2Ctrl.SigPos = 512;
    Chan1Ctrl.DispPos = 0;
    Chan2Ctrl.DispPos = 0;
    ANADAC(DAC_CHAN1, Chan1Ctrl.SigPos << 2);
    ANADAC(DAC_CHAN2, Chan2Ctrl.SigPos << 2);
    PosChan1_draw(true);
    PosChan2_draw(true);

// Задать фильтр    
    AcquCtrl.Filt = 3;
    AcquFilt_sett();

// Установить развертку по точкам    
    SweepCtrl.TimeNum = TIME_DOT;
    Time_sett();
    
// Установить запрос прерываний от ПЛИС только по точкам
    PL_AcquBuf &= ~(PL_ACQU_TDTIE | PL_ACQU_FRMIE | PL_ACQU_DOTIE);
    PL_AcquBuf |= PL_ACQU_DOTIE;
    PL->ACQU = PL_AcquBuf;
    SweepCtrl.Stop = 0;

/* 1. Задать 2mV, GND */
    Chan1Ctrl.VoltNum = VOLT_END;	// Задать 2mV
    Chan2Ctrl.VoltNum = VOLT_END;
    Chan1Ctrl.Cplg = 0;			// Задать заземление
    Chan2Ctrl.Cplg = 0;    
    Chan1Cplg_sett();			// Установить V/div и связь
    Chan2Cplg_sett();
    
// Старт сбора сигнала 
    PL->STAT = PL_START_ACQUST;

/* Аккумулировать множество точек */
    uint32_t ofs1 = 0, ofs2 = 0;
    for (uint32_t i = DTM; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    }
    
    for (uint32_t i = WGT; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      uint32_t rint = PL->RINT;
      ofs1 += (uint8_t)rint;
      ofs2 += (uint8_t)(rint >> 8);
    }

/* Вычислить и сохранить смещение */
    // Num_draw(ofs1 / WGT);
    Bal1Ctrl.Ofs2GND = 256 - ofs1 / (WGT / 2);
    Bal2Ctrl.Ofs2GND = 256 - ofs2 / (WGT / 2);

/* 2. Задать 2mV, AC */
    Chan1Ctrl.VoltNum = VOLT_END;	// Задать 2mV
    Chan2Ctrl.VoltNum = VOLT_END;
    Chan1Ctrl.Cplg = 1;			// Задать связь по переменному току
    Chan2Ctrl.Cplg = 1;    
    Chan1Cplg_sett();			// Установить V/div и связь
    Chan2Cplg_sett();
    
/* Аккумулировать множество точек */
    ofs1 = 0;
    ofs2 = 0;
    for (uint32_t i = DTM; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    }
    
    for (uint32_t i = WGT; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      uint32_t rint = PL->RINT;
      ofs1 += (uint8_t)rint;
      ofs2 += (uint8_t)(rint >> 8);
    }

/* Вычислить и сохранить смещение */
    // Num_draw(ofs1 / WGT);
    Bal1Ctrl.Ofs2AC = 256 - ofs1 / (WGT / 2);
    Bal2Ctrl.Ofs2AC = 256 - ofs2 / (WGT / 2);
    // Num_draw(Bal1Ctrl.Ofs2AC);
    
/* 3. Задать 2mV, DC */
    Chan1Ctrl.Cplg = 2;			// Задать связь по постоянному току
    Chan2Ctrl.Cplg = 2;    
    Chan1Cplg_sett();			// Установить V/div и связь
    Chan2Cplg_sett();
  
/* Аккумулировать множество точек */
    ofs1 = 0;
    ofs2 = 0;
    for (uint32_t i = DTM; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    }
    
    for (uint32_t i = WGT; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      uint32_t rint = PL->RINT;
      ofs1 += (uint8_t)rint;
      ofs2 += (uint8_t)(rint >> 8);
    }

/* Вычислить и сохранить смещение */
    // Num_draw(ofs1 / WGT);
    Bal1Ctrl.Ofs2DC = 256 - ofs1 / (WGT / 2);
    Bal2Ctrl.Ofs2DC = 256 - ofs2 / (WGT / 2);
    
/* 4. Задать 5mV, GND */
    Chan1Ctrl.VoltNum = VOLT_END - 1;	// Задать 5mV
    Chan2Ctrl.VoltNum = VOLT_END - 1;
    Chan1Ctrl.Cplg = 0;			// Задать заземление
    Chan2Ctrl.Cplg = 0;    
    Chan1Cplg_sett();			// Установить V/div и связь
    Chan2Cplg_sett();
  
/* Аккумулировать множество точек */
    ofs1 = 0;
    ofs2 = 0;
    for (uint32_t i = DTM; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    }

    for (uint32_t i = WGT; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      uint32_t rint = PL->RINT;
      ofs1 += (uint8_t)rint;
      ofs2 += (uint8_t)(rint >> 8);
    }

/* Вычислить и сохранить смещение */
    // Num_draw(ofs1 / WGT);
    Bal1Ctrl.Ofs5GND = 256 - ofs1 / (WGT / 2);
    Bal2Ctrl.Ofs5GND = 256 - ofs2 / (WGT / 2);

/* 5. Задать 5mV, AC */
    Chan1Ctrl.VoltNum = VOLT_END - 1;	// Задать 5mV
    Chan2Ctrl.VoltNum = VOLT_END - 1;
    Chan1Ctrl.Cplg = 1;			// Задать связь по переменному току
    Chan2Ctrl.Cplg = 1;    
    Chan1Cplg_sett();			// Установить V/div и связь
    Chan2Cplg_sett();
    
/* Аккумулировать множество точек */
    ofs1 = 0;
    ofs2 = 0;
    for (uint32_t i = DTM; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    }
    
    for (uint32_t i = WGT; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      uint32_t rint = PL->RINT;
      ofs1 += (uint8_t)rint;
      ofs2 += (uint8_t)(rint >> 8);
    }

/* Вычислить и сохранить смещение */
    // Num_draw(ofs1 / WGT);
    Bal1Ctrl.Ofs5AC = 256 - ofs1 / (WGT / 2);
    Bal2Ctrl.Ofs5AC = 256 - ofs2 / (WGT / 2);
    
/* 6. Задать 5mV, DC */
    Chan1Ctrl.Cplg = 2;			// Задать связь по постоянному току
    Chan2Ctrl.Cplg = 2;    
    Chan1Cplg_sett();			// Установить V/div и связь
    Chan2Cplg_sett();
  
/* Аккумулировать множество точек */
    ofs1 = 0;
    ofs2 = 0;
    for (uint32_t i = DTM; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    }
    
    for (uint32_t i = WGT; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      uint32_t rint = PL->RINT;
      ofs1 += (uint8_t)rint;
      ofs2 += (uint8_t)(rint >> 8);
    }

/* Вычислить и сохранить смещение */
    // Num_draw(ofs1 / WGT);
    Bal1Ctrl.Ofs5DC = 256 - ofs1 / (WGT / 2);
    Bal2Ctrl.Ofs5DC = 256 - ofs2 / (WGT / 2);

/* 7. Задать 10mV, AC */
    Chan1Ctrl.VoltNum = VOLT_END - 2;	// Задать 10mV
    Chan2Ctrl.VoltNum = VOLT_END - 2;
    Chan1Ctrl.Cplg = 1;			// Задать связь по переменному току
    Chan2Ctrl.Cplg = 1;    
    Chan1Cplg_sett();			// Установить V/div и связь
    Chan2Cplg_sett();

  
/* Аккумулировать множество точек */
    ofs1 = 0;
    ofs2 = 0;
    for (uint32_t i = DTM; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    }
    
    for (uint32_t i = WGT; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      uint32_t rint = PL->RINT;
      ofs1 += (uint8_t)rint;
      ofs2 += (uint8_t)(rint >> 8);
    }

/* Вычислить и сохранить смещение */
    // Num_draw(ofs1 / WGT);
    Bal1Ctrl.Ofs10AC = 256 - ofs1 / (WGT / 2);
    Bal2Ctrl.Ofs10AC = 256 - ofs2 / (WGT / 2);

/* 8. Задать 10mV, DC */
    Chan1Ctrl.Cplg = 2;			// Задать связь по постоянному току
    Chan2Ctrl.Cplg = 2;    
    Chan1Cplg_sett();			// Установить V/div и связь
    Chan2Cplg_sett();
  
/* Аккумулировать множество точек */
    ofs1 = 0;
    ofs2 = 0;
    for (uint32_t i = DTM; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    }
   
    for (uint32_t i = WGT; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      uint32_t rint = PL->RINT;
      ofs1 += (uint8_t)rint;
      ofs2 += (uint8_t)(rint >> 8);
    }

/* Вычислить и сохранить смещение */
    // Num_draw(ofs1 / WGT);
    Bal1Ctrl.Ofs10DC = 256 - ofs1 / (WGT / 2);
    Bal2Ctrl.Ofs10DC = 256 - ofs2 / (WGT / 2);

/* 9. Задать 20mV, AC */
    Chan1Ctrl.VoltNum = VOLT_END - 3;	// Задать 20mV
    Chan2Ctrl.VoltNum = VOLT_END - 3;
    Chan1Ctrl.Cplg = 1;			// Задать связь по переменному току
    Chan2Ctrl.Cplg = 1;    
    Chan1Cplg_sett();			// Установить V/div и связь
    Chan2Cplg_sett();
  
/* Аккумулировать множество точек */
    ofs1 = 0;
    ofs2 = 0;
    for (uint32_t i = DTM; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    }
    
    for (uint32_t i = WGT; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      uint32_t rint = PL->RINT;
      ofs1 += (uint8_t)rint;
      ofs2 += (uint8_t)(rint >> 8);
    }

/* Вычислить и сохранить смещение */
    // Num_draw(ofs1 / WGT);
    Bal1Ctrl.Ofs20AC = 256 - ofs1 / (WGT / 2);
    Bal2Ctrl.Ofs20AC = 256 - ofs2 / (WGT / 2);
    
/* 10. Задать 20mV, DC */
    Chan1Ctrl.Cplg = 2;			// Задать связь по постоянному току
    Chan2Ctrl.Cplg = 2;    
    Chan1Cplg_sett();			// Установить V/div и связь
    Chan2Cplg_sett();
  
/* Аккумулировать множество точек */
    ofs1 = 0;
    ofs2 = 0;
    for (uint32_t i = DTM; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    }
    
    for (uint32_t i = WGT; i--; ) {
      while (PLIRQ_GET()) { };		// Ждать, пока нет запроса от точки
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      uint32_t rint = PL->RINT;
      ofs1 += (uint8_t)rint;
      ofs2 += (uint8_t)(rint >> 8);
    }

/* Вычислить и сохранить смещение */
    // Num_draw(ofs1 / WGT);
    Bal1Ctrl.Ofs20DC = 256 - ofs1 / (WGT / 2);
    Bal2Ctrl.Ofs20DC = 256 - ofs2 / (WGT / 2);

/* X. Сохранить во flash */
    SFMPage_write((uint8_t *)&Bal1Ctrl.Ofs20DC, SFMBAL1, 10);
    SFMPage_write((uint8_t *)&Bal2Ctrl.Ofs20DC, SFMBAL2, 10);    

/* Убрать режим балансировки */
    GenCtrl.Bal = false;

/* Сброс прибора */
    Signature = 0x00000000;	// Стереть сигнатуру в Backup RAM
    NVIC_SystemReset(); 
  }
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnUtilBal_disp(void) {
  // DpyOffOn_draw(DPYPOS(DPYMHH + 2, DPYM2RV), GenCtrl.Bal);
  DpyString_draw(DPYPOS(DPYMHH + 2, DPYM2RV), "Вып.(1мин)", VOFF_TXT, false);
}


/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnUtilVers_hand(void) {

}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void MnUtilVers_disp(void) {
  uint8_t *pDpy = DPYPOS(DPYMHH + 20, DPYM4RV);
  DpyRoRect_fill(pDpy, 24, 14, 0xFF << 8 | VON_BKG);
  DpyString_draw(pDpy + 320 * 3 + 4, VERSION, VON_TXT, false);
}

/*!*****************************************************************************
 \brief		Menu Utilities Reset
 */
void MnUtilReset_hand(void) {
  Signature = 0x00000000;	// Стереть сигнатуру в Backup RAM
  NVIC_SystemReset();
}

void MnUtilReset_disp(void) {
  uint8_t *pDpy = DPYPOS(DPYMHH + 4, DPYM5RV + 3);
  DpyString_draw(pDpy, "настроек", ITEM_TXT, false);
}


/* ########################################################################### */

//__ Обработчики ручки УСТАНОВКА, Индикаторы ручки УСТАНОВКА __________________//


/*!*****************************************************************************
 @details	Пустая функция ручки УСТАНОВКА, обработчик
 @note	
 */
void SetVoid_hand(int32_t Cnt) { }

/*!*****************************************************************************
 @details	Установить Предзапуск, обработчик
 @note		Предзапуск изменяется скачками по 32 для кадра 1K выборок
		Для больших кадров скачки умножаются, всего есть 32-2 позиции  
 */
void SetPret_hand(int32_t Cnt) {
  Pret_draw(false);
  MemBarPret_draw(false);
  int16_t pret = (int16_t)SweepCtrl.Pret;
//  int32_t offs = pret - SweepCtrl.TimePos;	// смещение позиции
  // pret += Cnt << 4;
  pret += Cnt;
  int16_t lmarg = 1;		// левый предел 1/32 памяти
  int16_t rmarg = 31;		// правый предел 31/31 памяти

  if (pret < lmarg) pret = lmarg;
  if (pret > rmarg) pret = rmarg;
  SweepCtrl.Pret = pret;
//  SweepCtrl.TimePos = (pret << MemCtrl.SizeNum) - offs;	// новая позиция
  Pret_draw(true);
  MemBarPret_draw(true);
  SetPret_sett();
  // Sweep_init();
}

/*!*****************************************************************************
 @details	Установить Стабильность, обработчик
 @note	
 */
void SetHold_hand(int32_t Cnt) { }


/*!*****************************************************************************
 @details	Установить Усреднение, обработчик
 @note	
 */
void SetAver_hand(int32_t Cnt) {
  int32_t aver = (int32_t)AcquCtrl.Aver;
  aver += Cnt;				// код расширен!
  if (aver < 0) aver = 0;
  if (aver > 12 << 2) aver = 12 << 2;
  AcquCtrl.Aver = aver;
  MnAcquAver_disp();			// Написать к-т усреднения в Меню
}

/*!*****************************************************************************
 @brief		
 @details	Установить Курсор 1
 @note		По времени ограничивается памятью, по напряжению - экраном
 */
void SetCurs1_hand(int32_t Cnt) {
/* Стереть старый курсор */
  Curs_draw(false);
/* Курсор по X */
  if (!CursCtrl.xY) {		// x
    int32_t curs = CursCtrl.CX1;
    curs += Cnt;
    if (curs < 0) curs = 0;
    if (curs > (1024 << MemCtrl.SizeNum) - (DispCtrl.Wide ? 0 : 50))
      curs = (1024 << MemCtrl.SizeNum) - (DispCtrl.Wide ? 1 : 51);
    CursCtrl.CX1 = curs;
/* Сместить изображение сигналов, если нужно */
    if (curs < SweepCtrl.TimePos) {
      Pret_draw(false);
      MemBarPos_draw(false);
      if (Chan1Ctrl.On) SigChan1_draw(0);
      if (Chan2Ctrl.On) SigChan2_draw(0);
      SweepCtrl.TimePos = curs;
      if (Chan1Ctrl.On) SigChan1_draw(3);
      if (Chan2Ctrl.On) SigChan2_draw(3);
      Pret_draw(true);
      MemBarPos_draw(true);
    }
    if (curs > SweepCtrl.TimePos + (DispCtrl.Wide ? 300 : 250)) {
      Pret_draw(false);
      MemBarPos_draw(false);
      if (Chan1Ctrl.On) SigChan1_draw(0);
      if (Chan2Ctrl.On) SigChan2_draw(0);
      SweepCtrl.TimePos = curs - (DispCtrl.Wide ? 300 : 250);
      if (Chan1Ctrl.On) SigChan1_draw(3);
      if (Chan2Ctrl.On) SigChan2_draw(3);
      Pret_draw(true);
      MemBarPos_draw(true);
    }
  }
/* Курсор по Y */
  else {
    int32_t curs = CursCtrl.CY1;
    curs += Cnt;
    if (curs < 28) curs = 28;
    if (curs > 228) curs = 228;
    CursCtrl.CY1 = curs;
  }
/* Нарисовать новый курсор */
  Curs_draw(true);
  SetCursSt_disp();
}

/*!*****************************************************************************
 @brief		
 @details	Установить Курсор 2
 @note		По времени ограничивается памятью, по напряжению - экраном
 */
void SetCurs2_hand(int32_t Cnt) {
/* Стереть старый курсор */
  Curs_draw(false);
/* Курсор по X */
  if (!CursCtrl.xY) {		// x
    int32_t curs = CursCtrl.CX2;
    curs += Cnt;
    if (curs < 0) curs = 0;
    if (curs > (1024 << MemCtrl.SizeNum) - (DispCtrl.Wide ? 0 : 50))
      curs = (1024 << MemCtrl.SizeNum) - (DispCtrl.Wide ? 1 : 51);
    CursCtrl.CX2 = curs;
/* Сместить изображение сигналов, если нужно */
    if (curs < SweepCtrl.TimePos) {
      Pret_draw(false);
      MemBarPos_draw(false);
      if (Chan1Ctrl.On) SigChan1_draw(0);
      if (Chan2Ctrl.On) SigChan2_draw(0);
      SweepCtrl.TimePos = curs;
      if (Chan1Ctrl.On) SigChan1_draw(3);
      if (Chan2Ctrl.On) SigChan2_draw(3);
      Pret_draw(true);
      MemBarPos_draw(true);
    }
    if (curs > SweepCtrl.TimePos + (DispCtrl.Wide ? 300 : 250)) {
      Pret_draw(false);
      MemBarPos_draw(false);
      if (Chan1Ctrl.On) SigChan1_draw(0);
      if (Chan2Ctrl.On) SigChan2_draw(0);
      SweepCtrl.TimePos = curs - (DispCtrl.Wide ? 300 : 250);
      if (Chan1Ctrl.On) SigChan1_draw(3);
      if (Chan2Ctrl.On) SigChan2_draw(3);
      Pret_draw(true);
      MemBarPos_draw(true);
    }
  }
/* Курсор по Y */
  else {
    int32_t curs = CursCtrl.CY2;
    curs += Cnt;
    if (curs < 28) curs = 28;
    if (curs > 228) curs = 228;
    CursCtrl.CY2 = curs;
  }
/* Нарисовать новый курсор */
  Curs_draw(true);
  SetCursSt_disp();
}

/*!*****************************************************************************
 @brief		
 @details	Установить Курсоры 1 и 2
 @note		По времени ограничивается памятью, по напряжению - экраном
 */
void SetCursTr_hand(int32_t Cnt) {
/* Стереть старые курсоры */
  Curs_draw(false);
/* Курсор по X */
  if (!CursCtrl.xY) {		// x
    int32_t curs1 = CursCtrl.CX1;
    int32_t curs2 = CursCtrl.CX2;
    curs1 += Cnt;
    curs2 += Cnt;
    if (curs1 < 0) curs1 = 0;
    if (curs2 < 0) curs2 = 0;
    if (curs1 > (1024 << MemCtrl.SizeNum) - (DispCtrl.Wide ? 0 : 50))
        curs1 = (1024 << MemCtrl.SizeNum) - (DispCtrl.Wide ? 1 : 51);
    if (curs2 > (1024 << MemCtrl.SizeNum) - (DispCtrl.Wide ? 0 : 50))
        curs2 = (1024 << MemCtrl.SizeNum) - (DispCtrl.Wide ? 1 : 51);
    CursCtrl.CX1 = curs1;
    CursCtrl.CX2 = curs2;
/* Сместить изображение сигналов, если нужно */
    if (curs1 < SweepCtrl.TimePos || curs2 < SweepCtrl.TimePos) {
      Pret_draw(false);
      MemBarPos_draw(false);
      if (Chan1Ctrl.On) SigChan1_draw(0);
      if (Chan2Ctrl.On) SigChan2_draw(0);
      SweepCtrl.TimePos = curs1 < curs2 ? curs1 : curs2;
      if (Chan1Ctrl.On) SigChan1_draw(3);
      if (Chan2Ctrl.On) SigChan2_draw(3);
      Pret_draw(true);
      MemBarPos_draw(true);
    }
    if (curs1 > SweepCtrl.TimePos + (DispCtrl.Wide ? 300 : 250) ||
        curs2 > SweepCtrl.TimePos + (DispCtrl.Wide ? 300 : 250)) {
      Pret_draw(false);
      MemBarPos_draw(false);
      if (Chan1Ctrl.On) SigChan1_draw(0);
      if (Chan2Ctrl.On) SigChan2_draw(0);
      SweepCtrl.TimePos = curs1 > curs2 ? 
	curs1 - (DispCtrl.Wide ? 300 : 250) : curs2 - (DispCtrl.Wide ? 300 : 250);
      if (Chan1Ctrl.On) SigChan1_draw(3);
      if (Chan2Ctrl.On) SigChan2_draw(3);
      Pret_draw(true);
      MemBarPos_draw(true);
    }
  }
/* Курсор по Y */
  else {
    int32_t curs1 = CursCtrl.CY1;
    int32_t curs2 = CursCtrl.CY2;
    curs1 += Cnt;
    curs2 += Cnt;
    if (curs1 < 28) curs1 = 28;
    if (curs2 < 28) curs2 = 28;
    if (curs1 > 228) curs1 = 228;
    if (curs2 > 228) curs2 = 228;
    CursCtrl.CY1 = curs1;
    CursCtrl.CY2 = curs2;
  }
/* Нарисовать новые курсоры */
  Curs_draw(true);
  SetCursSt_disp();
}

/*!*****************************************************************************
 @brief		
 @details	Показать/стереть результаты измерения в Статусе
 @note		
 */
void SetCursSt_disp(void) {
  if (!CursCtrl.On) return;
  uint8_t txt1[10];						// "012345678"9
  uint8_t txt2[10];						// "012345678"9
  uint8_t *ptxt1 = txt1;		// Указатель на символ в буфере
  uint8_t *ptxt2 = txt2;		// Указатель на символ в буфере
  *(uint32_t *)ptxt1 = 0x20202020;				// "____xxxxx"x
  *(uint32_t *)ptxt2 = 0x20202020;
  *(uint32_t *)(ptxt1 + 4) = 0x20202020;			// "________x"x
  *(uint32_t *)(ptxt2 + 4) = 0x20202020;
  *(uint16_t *)(ptxt1 + 8) = 0x0020;				// "_________"\0
  *(uint16_t *)(ptxt2 + 8) = 0x0020;				// "_________"\0
  
  uint32_t mag[] = {40, 20, 8};		// Коэффициент умножения результата  
  
  if (!CursCtrl.xY) {
/* dT */
    uint32_t tcom = 5 - (SweepCtrl.TimeNum + 1) % 9 / 3;	// comma pos
    *(ptxt1 + tcom - 1) = '0';					// "__0_____x"x
    *(ptxt1 + tcom) = '.';					// "__0.____x"x
    for (uint32_t i = tcom + 1; i < 7; i++)
      *(ptxt1 + i) = '0';					// "__0.000_x"x
    *(ptxt1 + 7) = " m\xB5np"[(SweepCtrl.TimeNum + 1) / 9];	// "______0mx"x
    *(ptxt1 + 8) = 's';						// "______0ms"x
    *(ptxt1 + 9) = '\0';					// "______0ms"\0

    *(ptxt2 + 7) = 'H';
    *(ptxt2 + 8) = 'z';
    *(ptxt2 + 9) = '\0';    
    
    int32_t tdel = CursCtrl.CX1 - CursCtrl.CX2;
    if (tdel < 0) tdel = -tdel;
    tdel *= mag[(SweepCtrl.TimeNum + 1) % 3];
    
    ptxt1 += 6;				// Последняя цифра в буфере
    while (tdel) {
      *ptxt1-- = tdel % 10 + '0';
      if (*ptxt1 == '.') ptxt1--;
      tdel /= 10;
    }
/* 1/dT */
    uint32_t fcom = 1 + (SweepCtrl.TimeNum + 8) % 9 / 3;	// comma pos
    uint32_t fmul = 1 + (SweepCtrl.TimeNum + 8) / 9;		// mult index
    int32_t fdel = CursCtrl.CX1 - CursCtrl.CX2;
    if (fdel < 0) fdel = -fdel;
    if (!fdel) {
      fdel++;			// Убрать деление на 0 (заменил вопросом)
      *(ptxt2 + 6) = '?';
    }
    else {
      fdel = (500000000 >> (2 - (SweepCtrl.TimeNum + 2) % 3)) / fdel;
      while (!(fdel / 100000000)) {	// Если слева нет значащей цифры
        fcom--;				// перенести запятую влево
        if (!fcom) {			// Если запятая на краю
          fcom = 3;			// перенести запятую на позицию 3
	  fmul--;			// изменить индекс множителя
        }
        fdel *= 10;			// увеличить число
      }					// Теперь число прижато к левому краю
      *(ptxt2 + fcom) = '.';
      *(ptxt2 + 6) = "\xB5m kMG"[fmul];
      ptxt2 += 5;
      fdel /= 10000;			// Оставить 5 значащих цифр
    
      while (fdel) {
        *ptxt2-- = fdel % 10 + '0';
        if (*ptxt2 == '.') ptxt2--;
        fdel /= 10;
      }
    }
  }

  else {
  
    if (Chan1Ctrl.On) {
      uint32_t num =  (Chan1Ctrl.VoltNum - 6 + Chan1Ctrl.Prb * 3) + 7;
      uint32_t com = 4 - num % 9 / 3;				// comma pos
      *(ptxt1 + com - 1) = '0';					// "0_______x"x
      *(ptxt1 + com) = '.';					// "0.______x"x
      for (uint32_t i = com + 1; i < 6; i++)
        *(ptxt1 + i) = '0';					// "0.000___x"x
      *(ptxt1 + 6) = "k m\xB5"[num / 9];			// "0.000___x"x
      *(ptxt1 + 7) = 'V';					// "0.000_V_x"x
      *(ptxt1 + 8) = ' ';					// "0.000_V__"x 
      *(ptxt1 + 9) = '\0';					// "0.000_V__"\0

      int32_t del = CursCtrl.CY1 - CursCtrl.CY2;
      if (del < 0) del = -del;
      del *= mag[num % 3];

      ptxt1 += 5;			// Последняя цифра в буфере
      while (del) {
        *ptxt1-- = del % 10 + '0';
	if (*ptxt1 == '.') ptxt1--;
	del /= 10;
      }
    }
    
    if (Chan2Ctrl.On) {
      uint32_t num =  (Chan2Ctrl.VoltNum - 6 + Chan2Ctrl.Prb * 3) + 7;      
      uint32_t com = 4 - num % 9 / 3;				// comma pos
      *(ptxt2 + com - 1) = '0';					// "0_______x"x
      *(ptxt2 + com) = '.';					// "0.______x"x
      for (uint32_t i = com + 1; i < 6; i++)
        *(ptxt2 + i) = '0';					// "0.000___x"x
      *(ptxt2 + 6) = "k m\xB5"[num / 9];			// "0.000___x"x
      *(ptxt2 + 7) = 'V';					// "0.000_V_x"x
      *(ptxt2 + 8) = ' ';					// "0.000_V__"x
      *(ptxt2 + 9) = '\0';					// "0.000_V__"\0

      int32_t del = CursCtrl.CY1 - CursCtrl.CY2;
      if (del < 0) del = -del;
      del *= mag[num % 3];

      ptxt2 += 5;			// Последняя цифра в буфере
      while (del) {
        *ptxt2-- = del % 10 + '0';
	if (*ptxt2 == '.') ptxt2--;
	del /= 10;
      }
    }
    
  }
  DpyStringPad_draw(DPYPOS(DPYVALH, DPYVAL1V), txt1, 0x80800000, true);
  DpyStringPad_draw(DPYPOS(DPYVALH, DPYVAL2V), txt2, 0x80800000, true);
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void SetPar1_hand(int32_t Cnt) {
  int32_t par = (int32_t)MeasCtrl.Par1;
  par += Cnt;				// параметр расширен!
  if (par < 0) par = 0;
  if (par > PAR_AVR << 2) par = PAR_AVR << 2;
  MeasCtrl.Par1 = par;
  MnMeasPar1_disp();			// Написать параметр в Меню
  MeasPar1St_disp();			// Написать параметр в Статусе
  SigPar_meas();
}

/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void SetPar2_hand(int32_t Cnt) {
  int32_t par = (int32_t)MeasCtrl.Par2;
  par += Cnt;				// параметр расширен!
  if (par < 0) par = 0;
  if (par > PAR_AVR << 2) par = PAR_AVR << 2;
  MeasCtrl.Par2 = par;
  MnMeasPar2_disp();			// Написать параметр в Меню
  MeasPar2St_disp();			// Написать параметр в Статусе
  SigPar_meas();
} 

 
/*!*****************************************************************************
 @brief		
 @details	
 @note		
 */
void SetBufNum_hand(int32_t Cnt)
{
  static int32_t cnt;
  cnt += Cnt;
  int32_t num = MemCtrl.BufNum;
  if (cnt > +4) { num++; cnt = 0; }
  if (cnt < -4) { num--; cnt = 0; }
  if (MemCtrl.SizeNum == 0) { if (num < 0) num = 31; if (num > 31) num = 0; }
  if (MemCtrl.SizeNum == 1) { if (num < 0) num = 15; if (num > 15) num = 0; }
  if (MemCtrl.SizeNum == 2) { if (num < 0) num = 7; if (num > 7) num = 0; }
  if (MemCtrl.SizeNum == 3) { if (num < 0) num = 3; if (num > 3) num = 0; }
  if (MemCtrl.SizeNum == 4) { if (num < 0) num = 1; if (num > 1) num = 0; }
  if (MemCtrl.SizeNum >= 5) num = 0;
  MemCtrl.BufNum = num;
  MnMemNum_disp();
  DpyWork_clear(MASK_MATSIG);
  SigChans_redraw();
  Math_exe();
  SigPar_meas();
}
 

//__ Обработчики кнопки Выполнить (В меню СЕРВИС) _____________________________//


/*!*****************************************************************************
 @details	Пустая функция Выполнить, обработчик
 @note	
 */
void ExeVoid_hand(void) { }

/*!*****************************************************************************
 @brief		Calibrate execute
 @details	Выполнить калибровку интерполятора
 @note
 */
void ExeCalibr_hand(void) {
  uint32_t tmts = SFMWord_read(SFMINTERP);
  if (~tmts) {
    Interp.Tmts = tmts;
    Interp.Tmad = SFMWord_read(SFMINTERP + 4);
  }
/* Задать ждущий режим, по фронту, от канала 1, связь по ВЧ */
  // PL_TrigBuf |= PL_TRIG_NORM;
  // PL->TRIG = PL_TrigBuf;
  Chan2Ctrl.On = 0;			// Выключить канал 2
  PL->TRIG = PL_TRIG_DIV | PL_TRIG_NORM | PL_TRIG_RISE
	   | PL_TRIG_HYST | PL_TRIG_AC | PL_TRIG_CH1;
  SetNum = SET_AVER;		// Усреднение выбрать
  ServCtrl.Calibr = 1;		// Включить отображение значения
  SweepCtrl.Stop = 0;		// Убрать СТОП 
}

/*!*****************************************************************************
 @brief		Calibrate Save execute
 @details	Выполнить сохранение калибровки интерполятора
 @note		Записать два слова (8 байтов) на страницу SFMINTERP
 */
void ExeCalSave_hand(void) {
  SFMPage_write((uint8_t *)&Interp.Tmts, SFMINTERP, 8);
  ServCtrl.Calibr = 0;			// Выключить отображение
  DpyNum_blank();			// Очистить зону
  SetNum = SET_VOID;			// Убрать выбор усреднения
}

/*!*****************************************************************************
 @details	
 @note		
 */
void ExeCPUiMem_hand(void) { }

/*!*****************************************************************************
 @details	
 @note		
 */
void ExeCPUeMem_hand(void) { }


/*!*****************************************************************************
 @details	Принять код по RS-232
 @note		Первые 4 байта зарезервированы для хранения размера кода
 */
void ExePLRecv_hand(void)
{
  NVIC_DisableIRQ(USART2_IRQn);
  uint8_t *dat = (uint8_t *)(XRAM_BASE + 4);	// указатель на буфер в памяти
  uint8_t *dpy = (uint8_t *)(DPYMEM_BASE);	// указатель на буфер в ЖКИ
  uint32_t cnt = 0;			// счетчик для отображения по 256x240 байтов
  CodeCnt = 0;				// глобальный счетчик байтов
  WDELAY_START(8000000);		// 8 sec
  // TimerTicks = 8000;			// 8 sec
  // while (TimerTicks) {
  while (!(WDELAY_CHECK)) {
    if (USART2->SR & USART_SR_RXNE) {
      uint8_t temp = USART2->DR;
      *dat++ = temp;
      *dpy++ = temp;
      cnt++;
      CodeCnt++;
      if ((cnt & 0x000000FF) == 0) {
        dpy += (320 - 256);
        // TimerTicks = 8000;
	WDELAY_RESTART();
      }	
      if ((cnt > 256U * 240U)) {
        dpy = (uint8_t *)(DPYMEM_BASE);
	cnt = 0;
      }
    }
  }
  Num_draw(CodeCnt);
  *(uint32_t *)(XRAM_BASE) = CodeCnt;	// записать размер кода 
}

/*!*****************************************************************************
 @details	Проверить равенство принятого по RS-232 кода и кода в памяти
 @note	
 */
void ExePLComp_hand(void) {
  NVIC_DisableIRQ(USART2_IRQn);
  uint8_t *dat = (uint8_t *)(XRAM_BASE + 4);	// указатель на буфер в памяти
  uint8_t *dpy = (uint8_t *)(DPYMEM_BASE);	// указатель на буфер в ЖКИ
  uint32_t cnt = 0;			// счетчик для отображения 256x240 байтов
  CodeCnt = 0;				// глобальный счетчик байтов
  WDELAY_START(8000000);		// 8 sec
  // TimerTicks = 8000;			// 8 sec
  // while (TimerTicks) {
  while (!(WDELAY_CHECK)) {
    if (USART2->SR & USART_SR_RXNE) {
      uint8_t temp = USART2->DR;
      if (*dat++ == temp)  *dpy++ = 0xCC;
      else  *dpy++ = 0xC3;
      cnt++;
      CodeCnt++;
      if ((cnt & 0x000000FF) == 0) {
        dpy += (320 - 256);
        // TimerTicks = 8000;
        WDELAY_RESTART();
      }	
      if ((cnt > 256U * 240U)) {
        dpy = (uint8_t *)(DPYMEM_BASE);
        cnt = 0;
      }
    }
  }
  Num_draw(CodeCnt);
}

/*!*****************************************************************************
 @details	Записать код в последовательную память
 @note		Первые 4 байта кода - размер
 @note		Окончание записи проверяется в самих вызываемых функциях
 */
void ExePLWrite_hand(void) {
  if (!CodeCnt)  return;			// если кода нет, уйти
  uint8_t *dat = (uint8_t *)(XRAM_BASE);	// указатель на буфер в ОЗУ
  uint8_t *dpy = (uint8_t *)(DPYMEM_BASE);	// указатель на буфер в ЖКИ
  uint32_t adr = SFMCODE_BASE;		// адрес последовательной памяти
  uint32_t cnt = 0;			// счетчик для отображения 256x240 байтов

  for (uint32_t i = 4; i > 0; i--) {
    SFMSect_erase(adr);			// стереть 4 сектора (262144 байта)
    adr += (1 << 16);			// +65536
  }
  uint32_t pages = (CodeCnt + 4) / 256;	// количество полных страниц
  uint32_t bytes = (CodeCnt + 4) % 256;	// количество оставшихся байтов
  adr = SFMCODE_BASE;
  for (uint32_t i = pages; i > 0; i--) {
    SFMPage_progr(dat, adr, 256);
    dat += 256;
    adr += 256;  
    for (uint32_t j = 256; j > 0; j--) {
      *dpy++ = 0xCF;			// желтый
      cnt++;
      if ((cnt & 0x000000FF) == 0) {
        dpy += (320 - 256);
      }
      if ((cnt > 256U * 240U)) {
        dpy = (uint8_t *)(DPYMEM_BASE);
	cnt = 0;
      }
    }
  }
  SFMPage_progr(dat, adr, bytes);	// запрограммировать остаток
  for (uint32_t j = bytes; j > 0; j--) {
    *dpy++ = 0xcf;
    cnt++;
    if ((cnt & 0x000000FF) == 0) {
      dpy += (320 - 256);
    }
    if ((cnt > 256U * 240U)) {
      dpy = (uint8_t *)(DPYMEM_BASE);
      cnt = 0;
    }
  }    
  
  Num_draw(adr + bytes);		// написать количество байтов
  WDELAY_START(4000000);
  // TimerTicks = 4000;			// 4 sec
  // while (TimerTicks);			// задержка
  WDELAY_WAIT();
}

/*!*****************************************************************************
 @details	Прочитать и сравнить код из последовательной памяти
 @note		
 */
void ExePLRead_hand(void) {
  if (!CodeCnt) return;			// если кода нет, уйти
  uint8_t *dat = (uint8_t *)(XRAM_BASE);	// указатель на буфер в ОЗУ
  uint8_t *dpy = (uint8_t *)(DPYMEM_BASE);	// указатель на буфер в ЖКИ
  uint32_t adr = SFMCODE_BASE;
  uint32_t cnt = 0;			// счетчик для отображения 256x240 байтов

  SFMRead_start(adr);
  for (uint32_t i = CodeCnt + 4; i > 0; i--) {
    if (*dat++ == SFMByte_read()) *dpy++ = 0xFC;	// совпадает (голубой)
    else  *dpy++ = 0xF3;				// не совпадает (фиолетовый)
    cnt++;
    if ((cnt & 0x000000FF) == 0) { dpy += (320 - 256); }
    if ((cnt > 256U * 240U)) { dpy = (uint8_t *)(DPYMEM_BASE); cnt = 0; }
  }
  SFMRead_stop();
  
  WDELAY_START(4000000);
  // TimerTicks = 4000;
  // while (TimerTicks);
  WDELAY_WAIT();
}

/*!*****************************************************************************
 @details	Закрасить экран белым цветом
 @note	
 */
void ExeDpyWhite_hand(void) {
  DPY_RECT(0, 0, 319, 239, 0xFF, WHITE);
//  Gen.Lock = 1;			// Заблокировать панель управления
}

/*!*****************************************************************************
 @details	Закрасить экран черным цветом
 @note	
 */
void ExeDpyBlack_hand(void) {
  DPY_RECT(0, 0, 319, 239, 0xFF, BLACK);
//  Gen.Lock = 1;
}

/*!*****************************************************************************
 @details	Изобразить все символы на экране
 @note	
 */
void ExeDpyFont_hand(void) {
  DpyScreen_blank();
  DigitBar_disp();
  DpyFont_draw();
}

/*!*****************************************************************************
 @details	Нарисовать всю палитру на экране
 @note	
 */
void ExeDpyPalet_hand(void) {
  DpyScreen_blank();
  DigitBar_disp();
  DpyPalet_draw();
}

/*!*****************************************************************************
 @details	Закрасить экран красным цветом
 @note	
 */
void ExeDpyRed_hand(void) {
  DPY_RECT(0, 0, 319, 239, 0xFF, 0xC3);
//  Gen.Lock = 1;		// Заблокировать панель управления
}

/*!*****************************************************************************
 @details	Закрасить экран зеленым цветом
 @note	
 */
void ExeDpyGreen_hand(void) {
  DPY_RECT(0, 0, 319, 239, 0xFF, 0xCC);
//  Gen.Lock = 1;			// Заблокировать панель управления
}

/*!*****************************************************************************
 @details	Закрасить экран синим цветом
 @note	
 */
void ExeDpyBlue_hand(void) {
  DPY_RECT(0, 0, 319, 239, 0xFF, 0xF0);
//  Gen.Lock = 1;		// Заблокировать панель управления
}

/*!*****************************************************************************
 @details	Восстановить экран
 @note	
 */
void ExeDpyDefault_hand(void) {
    DpyScreen_blank();
    DpyScale_draw();
    MenuHead_disp();
    MenuHeadDepr_disp();    
//    Gen.Lock = 0;	// Разблокировать панель управления
}


//__ Установщики режимов, Индикаторы режимов в области Состояния ______________//

/* Таблица кодов Volt/div для установки в AnaReg */
static const uint8_t VoltDivReg[] = {
  0x56,					// (4)	20 V/div
  0x66,					// (5)	10 V/div
  0x46,					// (6)	 5 V/div
  0x52,					// (7)	 2 V/div
  0x62,					// (8)	 1 V/div
  0x42,					// (9)	0.5 V/div
  0x54,					// (10)	0.2 V/div *
  0x64,					// (11)	0.1 V/div
  0x44,					// (12)	50 mV/div
  0x50,					// (13)	20 mV/div
  0x60,					// (14)	10 mV/div
  0x40,					// (15)	5 mV/div
  0x00					// (16)	2 mV/div
};
  
//* Канал 1 *//

/*!*****************************************************************************
 @details	Канал 1, Режим, установщик
 @note	
 */
void Chan1Mode_sett(void) {
  if (Chan1Ctrl.On) SigChan1_draw(3);
  else              SigChan1_draw(0);
  if (Chan1Ctrl.On) PL_VoltBuf |= PL_VOLT_S1EN;
  else              PL_VoltBuf &= ~PL_VOLT_S1EN;
  PL->VOLT = PL_VoltBuf;
}

/*!*****************************************************************************
 @details	Канал 1, Ограничение полосы, установщик
 @note	
 */
void Chan1BWLim_sett(void) {
  AnaRegBuf &= ~(1 << 7);
  AnaRegBuf |= Chan1Ctrl.BWLim << 7;
  AnaReg_send();
}

/* Каналы 1 + 2, индикатор в зоне состояния */

/*!*****************************************************************************
 @details	Канал 1, Связь, установщик
 @note		
 */
void Chan1Cplg_sett(void) {
  AnaRegBuf &= ~0x00000076;	// задать биты Volt/div
  AnaRegBuf |= VoltDivReg[Chan1Ctrl.VoltNum - VOLT_BEG];
  if (!Chan1Ctrl.Cplg) {	// Заземление
    AnaRegBuf |= 0x00000030;	// установить биты
  }
  AnaRegBuf &= ~(1 << 3);	// Очистить бит связи
  AnaRegBuf |= !!(Chan1Ctrl.Cplg & 0x02) << 3;	// 01 - AC, 10 - DC

  AnaReg_send();		// Послать буфер
}

/*!*****************************************************************************
 @details	Канал 1, индикатор в СТАТУСЕ
 @note		Отображает связь, коэффициент, гасит в выключенном состоянии
 */
void Chan1St_disp(void) {
  if (Chan1Ctrl.On) PanCtrl.TxBuf &= ~(1 << 1);	// Зажечь диод
  else	PanCtrl.TxBuf |= (1 << 1);		// Погасить диод
  PanCtrl.TxBuf |= 0x40;
  USART3->DR = PanCtrl.TxBuf;

  if (Chan1Ctrl.On) {
    DpyRect_fill(DPYPOS(DPYSAH, DPYS1VV - 1), 9, 10, 0x0000FF02);
    DpyChar_draw(DPYPOS(DPYSAH + 1, DPYS1VV), '1', 0x00000202);
    uint32_t spaces = DpyVolt_draw(DPYPOS(DPYSAH + 12, DPYS1VV),
        Chan1Ctrl.VoltNum - 6 + Chan1Ctrl.Prb * 3, 0x02020000);
    // Num_draw(spaces);
    SSYM_t CplgSym[] = {SS_GND, SS_CPAC, SS_CPDC};
    DpySpec_draw(DPYPOS(DPYSAH + 12 + 8 * spaces, DPYS1VV),
      CplgSym[Chan1Ctrl.Cplg], 0x03, 0x02020000);
//    if (Chan1Ctrl.Gnd) ssym = SS_GND;
//    else if (!Chan1Ctrl.Cplg) ssym = SS_CPAC;
//      else ssym = SS_CPDC;
//    DpySpec_draw(DPYPOS(DPYSAH + 12 + 8 * spaces, DPYS1VV), ssym, 0x03, 0x03030000);
    if (Chan1Ctrl.Inv) {
      DpySpec_draw(DPYPOS(DPYSAH + 12 + 40, DPYS1VV), SS_ARDN, 0x03, 0x02020000);
    }    
  }
  else
    DPY_RECT(DPYSAH, DPYS1VV - 1, DPYSAH + 60, DPYS1VV + 9, 0xFF, BLANK);
}

//* Канал 2 *//

/*!*****************************************************************************
 @details	Канал 2, Режим, установщик
 @note	
 */
void Chan2Mode_sett(void) {
  if (Chan2Ctrl.On)  SigChan2_draw(3);
  else               SigChan2_draw(0);
  if (Chan2Ctrl.On) PL_VoltBuf |=  PL_VOLT_S2EN;
  else              PL_VoltBuf &= ~PL_VOLT_S2EN;
  PL->VOLT = PL_VoltBuf;
}

/*!*****************************************************************************
 @details	Канал 2, Ограничение полосы, установщик
 @note	
 */
void Chan2BWLim_sett(void) {
  AnaRegBuf &= ~(1 << 8);
  AnaRegBuf |= Chan2Ctrl.BWLim << 8;
  AnaReg_send();
}

/*!*****************************************************************************
 @details	Канал 2, Связь, установщик
 @note	
 */
void Chan2Cplg_sett(void) {
  AnaRegBuf &= ~0x00007600;	// задать биты Volt/div
  AnaRegBuf |= VoltDivReg[Chan2Ctrl.VoltNum - VOLT_BEG] << 8;
  if (!Chan2Ctrl.Cplg) {	// Если есть заземление,
    AnaRegBuf |= 0x00003000;	// установить биты
  }
  AnaRegBuf &= ~(1 << 3 + 8);
  AnaRegBuf |= !!(Chan2Ctrl.Cplg & 0x02) << 3 + 8;	// 01 - AC, 10 - DC
  
  AnaReg_send();		// Послать буфер
}

/*!*****************************************************************************
 @details	Канал 2, индикатор в СТАТУСЕ
 @note		Отображает связь, коэффициент, гасит в выключенном состоянии
 */
void Chan2St_disp(void) {
  if (Chan2Ctrl.On) PanCtrl.TxBuf &= ~(1 << 3);	// Зажечь диод
  else PanCtrl.TxBuf |= (1 << 3);		// Погасить диод
  PanCtrl.TxBuf |= 0x40;
  USART3->DR = PanCtrl.TxBuf;
  
  if (Chan2Ctrl.On) {
    DpyRect_fill(DPYPOS(DPYSAH, DPYS2VV - 1), 9, 10, 0x0000FF08);
    DpyChar_draw(DPYPOS(DPYSAH + 1, DPYS2VV), '2', 0x00000808);
    uint32_t spaces = DpyVolt_draw(DPYPOS(DPYSAH + 12, DPYS2VV),
        Chan2Ctrl.VoltNum - 6 + Chan2Ctrl.Prb * 3, 0x08080000);
    // Num_draw(spaces);
    SSYM_t CplgSym[] = {SS_GND, SS_CPAC, SS_CPDC};
    DpySpec_draw(DPYPOS(DPYSAH + 12 + 8 * spaces, DPYS2VV),
      CplgSym[Chan2Ctrl.Cplg], 0x0C, 0x08080000);
    
//    if (Chan2Ctrl.Gnd) ssym = SS_GND;
//    else if (!Chan2Ctrl.Cplg) ssym = SS_CPAC;
//      else ssym = SS_CPDC;
//    DpySpec_draw(DPYPOS(DPYSAH + 12 + 8 * spaces, DPYS2VV), ssym, 0x0c, 0x0c0c0000);
    if (Chan2Ctrl.Inv) {
      DpySpec_draw(DPYPOS(DPYSAH + 12 + 40, DPYS2VV), SS_ARDN, 0x0C, 0x08080000);
    }      
  }
  else
    DPY_RECT(DPYSAH, DPYS2VV - 1, DPYSAH + 60, DPYS2VV + 9, 0xFF, BLANK);
}

//* Математическая обработка *//

/*!*****************************************************************************
 @details	Мат. Обр, индикатор
 @note		зажигает или гасит светодиод
 */
void MathSt_disp(void) {
  if (MathCtrl.On) PanCtrl.TxBuf &= ~(1 << 2);	// Зажечь диод
  else             PanCtrl.TxBuf |=  (1 << 2);	// Погасить диод
  PanCtrl.TxBuf |= 0x40;
  USART3->DR = PanCtrl.TxBuf;
}

//* Развертка *//

/*!*****************************************************************************
 @details	Время/дел, установщик
 @note		Работает один таймер в ПЛИС
 @note		(2^31 * 4 ns > 8.5 s) * 25 > 200 s/div
 @note		Оставил 50 s/div
 @verbatim
  0  -  50 s/div
  26 - 100 ns/div	250 MSps, 4 ns sample
  27 -  50 ns/div	TimeItp = 2
  ...
  35 - 0.1 ns/div	TimeItp = 1000
 @endverbatim
 */
void Time_sett(void) {
  static const uint32_t PL_TimeDiv[] = {
  500000000 - 2,			//  0   50 s/div (2 s/dot)
  200000000 - 2,			//  1   20 s/div (0.8 s/dot)
  100000000 - 2,			//  2   10 s/div (0.4 s/dot)
   50000000 - 2,			//  3    5 s/div (0.2 s/dot)
   20000000 - 2,			//  4    2 s/div (80 ms/dot)
   10000000 - 2,			//  5    1 s/div (40 ms/dot)
    5000000 - 2,			//  6  0.5 s/div (20 ms/dot)
    2000000 - 2,			//  7  0.2 s/div (8 ms/dot)
    1000000 - 2,			//  8  0.1 s/div (4 ms/dot)
     500000 - 2,			//  9  50 ms/div (2 ms/dot)
     200000 - 2,			// 10  20 ms/div (0.8 ms/dot)
     100000 - 2,			// 11  10 ms/div (0.4 ms/dot)
      50000 - 2,			// 12   5 ms/div (0.2 ms/dot)
      20000 - 2,			// 13   2 ms/div (80 us/dot)
      10000 - 2,			// 14   1 ms/div (40 us/dot)
       5000 - 2,			// 15 0.5 ms/div (20 us/dot) *
       2000 - 2,			// 16 0.2 ms/div (8 us/dot)
       1000 - 2,			// 17 0.1 ms/div (4 us/dot)
        500 - 2,			// 18  50 us/div (2 us/dot)
        200 - 2,			// 19  20 us/div (0.8 us/dot)
        100 - 2,			// 20  10 us/div (0.4 us/dot)
         50 - 2,			// 21   5 us/div (0.2 us/dot)
         20 - 2,			// 22   2 us/div (80 ns/dot)
         10 - 2,			// 23   1 us/div (40 ns/dot)
          5 - 2,			// 24 0.5 us/div (20 ns/dot)
          2 - 2,			// 25 0.2 us/div (8 ns/dot)
  };
  static const uint16_t TimeItp_table[] = {
              1,			// 26 0.1 us/div (4 ns/dot)
              2,			// 27  50 ns/div (Random sample)
              5,			// 28  20 ns/div
             10,			// 29  10 ns/div
             20,			// 30   5 ns/div
             50,			// 31   2 ns/div
            100,			// 32   1 ns/div
            200,			// 33 0.5 ns/div
            500,			// 34 0.2 ns/div
           1000				// 35 0.1 ns/div
  };
  
  if (SweepCtrl.TimeNum < 26) {
    PL->TIML = (uint16_t)PL_TimeDiv[SweepCtrl.TimeNum];
    PL->TIMH = (uint16_t)(PL_TimeDiv[SweepCtrl.TimeNum] >> 16);
    SweepCtrl.TimeItp = 1;
  }
  else {
    PL->TIML = 0;
    PL->TIMH = PL_TIMH_LDP;
    SweepCtrl.TimeItp = TimeItp_table[SweepCtrl.TimeNum - 26];
  }

#ifdef TRIGDIV_USE
/* Задать делитель на быстрых развертках */
  if (SweepCtrl.TimeNum < 27)
    PL_TrigBuf &= ~PL_TRIG_DIV;		// Убрать делитель на 4
  else
    PL_TrigBuf |= PL_TRIG_DIV;		// Задать делитель на 4  
  PL->TRIG = PL_TrigBuf;
#endif
}

/*!*****************************************************************************
 @details	Время/дел, индикатор в СТАТУСЕ
 */
void TimeSt_disp(void) {
  static const uint8_t *TimeTxt[] =
  { "  50s", "  20s", "  10s",
    "   5s", "   2s", "   1s",
    " 0.5s", " 0.2s", " 0.1s",    
    " 50ms", " 20ms", " 10ms",
    "  5ms", "  2ms", "  1ms",
    "0.5ms", "0.2ms", "0.1ms",
    " 50\xB5s", " 20\xB5s", " 10\xB5s",
    "  5\xB5s", "  2\xB5s", "  1\xB5s",
    "0.5\xB5s", "0.2\xB5s", "0.1\xB5s",
    " 50ns", " 20ns", " 10ns",
    "  5ns", "  2ns", "  1ns",                            
    "0.5ns", "0.2ns", "0.1ns" };
  DpyStringPad_draw(DPYPOS(DPYSAH + 72, DPYS2VV), 
		TimeTxt[SweepCtrl.TimeNum], 0x0A0A0000, true);
}

/*!*****************************************************************************
 @details	Развертка, Режим, установщик
 */
void SweepMode_sett(void) {
  if (SweepCtrl.Mode) PL_TrigBuf |=  PL_TRIG_NORM;	// Norm, Single
  else		      PL_TrigBuf &= ~PL_TRIG_NORM;	// Auto
  PL->TRIG = PL_TrigBuf;
}

/*!*****************************************************************************
 @details	Развертка, Регистратор, установщик
 */
void SweepRoll_sett(void) { }

/*!*****************************************************************************
 @details	Развертка, Режим, индикатор в СТАТУСЕ
 */
void SweepSt_disp(void) {
  static const uint8_t *ModeTxt[4] = {" АВТО  ", " ЖДУЩ  ", "ОДНОКР ", "САМОПИС"};
  uint32_t mode = SweepCtrl.Mode;
  if (SweepCtrl.Roll && SweepCtrl.TimeNum <= TIME_DOT) mode = 3;
  DpyStringPad_draw(DPYPOS(DPYSAH + 72, DPYS1VV), ModeTxt[mode], 0x0A0A0000, 1);
}

//* Синхронизация *//

/*!*****************************************************************************
 @details	Синхронизация, Источник, установщик
 */
void TrigSour_sett(void) {
  static const uint8_t TrigSett[4] = {0x01, 0x00, 0x02, 0x01};
  AnaRegBuf &= ~(3 << 22);
  AnaRegBuf |= (TrigSett[TrigCtrl.Sour]) << 22;	// Ext, Ch1, Ch2, Ext
  AnaReg_send();			// Послать буфер    
}

/*!*****************************************************************************
 @details	Синхронизация, Источник, индикатор в СТАТУСЕ
 */
void TrigSt_disp(void) {
  static const uint8_t *SrcTxt[4] = {"СЕТЬ", "КАН1", "КАН2", "ВНЕШ"};
  DpyStringPad_draw(DPYPOS(DPYSAH + 132, DPYS1VV), 
		    SrcTxt[TrigCtrl.Sour], 0x0A0A0000, true);
  DpyRect_fill(DPYPOS(DPYSAH + 132, DPYS2VV), 32, 8, 0xFF<<8 | BLANK);
  DpySpec_draw(DPYPOS(DPYSAH + 136, DPYS2VV), 
	      (SSYM_t)(SS_FALL + TrigCtrl.Rise), 0xFF, 0x0A0A0000);
  uint8_t *pDpy = DPYPOS(DPYSAH + 148, DPYS2VV);
  switch (TrigCtrl.Cplg) {
  case 0:
    DpyString_draw(pDpy, "НЧ", 0x0A0A0000, true);
    break;
  case 1:
  default:  
    DpySpec_draw(pDpy + 4, SS_CPDC, 0xFF, 0x0A0A0000);
    break;
  case 2:
    DpySpec_draw(pDpy + 4, SS_CPAC, 0xFF, 0x0A0A0000);      
    break;
  case 3:
    DpyString_draw(pDpy, "ВЧ", 0x0A0A0000, true);
  }
}

/*!*****************************************************************************
 @details	Синхронизация, Перепад, установщик
 */
void TrigSlp_sett(void) {
  if (TrigCtrl.Rise) PL_TrigBuf |=  PL_TRIG_RISE;	// Rise
  else		     PL_TrigBuf &= ~PL_TRIG_RISE;	// Fall
  PL->TRIG = PL_TrigBuf;
}

/*!*****************************************************************************
 @details	Синхронизация, связь, установщик
 */
void TrigCplg_sett(void) {
  AnaRegBuf &= ~(1 << 15);
  AnaRegBuf |= (!(bool)TrigCtrl.Cplg) << 15;	// LF, DC, AC, HF
  AnaReg_send();			// Послать буфер  
}

/*!*****************************************************************************
 @details	Синхронизация, Шумоподавление, установщик
 */
void TrigNRej_sett(void) {
// Подается прямой код (проинвертируется в ПЛИС)
  if (TrigCtrl.NRej) PL_TrigBuf |=  (PL_TRIG_HYST);
  else		     PL_TrigBuf &= ~(PL_TRIG_HYST); 
  PL->TRIG = PL_TrigBuf;
}

//* Память *//

/*!*****************************************************************************
 @details	Память, Длина, установщик
 \note		Пока размер памяти в ПЛИС начинается с 0.5K 
 */
void MemSize_sett(void) {
  PL_AcquBuf &= ~PL_ACQU_MEM;
  PL_AcquBuf |= (MemCtrl.SizeNum + 1) << 8;
  PL->ACQU = PL_AcquBuf;
}

//* Сбор Информации *//

/*!*****************************************************************************
 @details	Фильтр, установщик
 */
void AcquFilt_sett(void) {
  PL_AcquBuf &= ~PL_ACQU_FILT;
  PL_AcquBuf |= AcquCtrl.Filt;
  PL->ACQU = PL_AcquBuf;
}

/*!*****************************************************************************
 @details	Пиковый детектор, установщик
 */
void AcquPeak_sett(void) {
  PL_AcquBuf &= ~PL_ACQU_PEAK;
  PL_AcquBuf |= AcquCtrl.Peak << 2;
  PL->ACQU = PL_AcquBuf;
}

/*!*****************************************************************************
 @details	ПУСК/СТОП индикатор
 */
void StSp_disp(void) {
  static const uint8_t *StopText[2] = {"    ", "СТОП"};
  // DpyStringPad_draw(DPYPOS(DPYMHH, 1), StopText[SweepCtrl.Stop], 0xf3f30000, 0);
  DpyStringPad_draw(DPYPOS(DPYWAH + 300 - (3 * 7 + 5), 1),
		StopText[SweepCtrl.Stop], 0xF3F30000, 0);

// Проверка склеивания строк
//  static const uint8_t *TestText = "Test" S_ROTR "A";
//  DpyString_draw(DPYPOS(DPYWAH, 2), TestText, 0xF3F30000, 0);
}

/*!*****************************************************************************
 @details	АВТОУСТ индикатор
 */
void Autoset_disp(void) { }


//__ Установщики ручки УСТАНОВКА ______________________________________________//

/*!*****************************************************************************
 @details	Установка предзапуска в ПЛИС
 @note		Обнулять младшие биты не нужно, в ПЛИС они не запоминаются
 */
void SetPret_sett(void) {
  uint16_t pret = SweepCtrl.Pret << MemCtrl.SizeNum + 5;
//  if (SweepCtrl.TimeNum >= 26) pret -= 16;	// остановить на 16 точек позже
  PL->PRET = pret;
  // Num_draw(pret);
}

/*!*****************************************************************************
 @details	Установка стабильности в ПЛИС
 */
void SetHold_sett(void) { }


/*!*****************************************************************************
 @brief		Volt/Div draw
 @details	Нарисовать Вольты в СТАТУСЕ
 @param		pDpy
 @param		Num
 @param		Color
 @return	количество лидирующих пробелов
 @note		первый пробел для символа связи	
 */
uint32_t DpyVolt_draw(uint8_t *pDpy, uint32_t Num, uint32_t Color) {
  static const uint8_t *VoltTxt[] = {
    " 500V ", " 200V ", " 100V ",		// 1/100
    "  50V ", "  20V ", "  10V ",		// 1/10
    "   5V ", "   2V ", "   1V ",
    " 0.5V ", " 0.2V ", " 0.1V ",
    " 50mV ", " 20mV ", " 10mV ",
    "  5mV ", "  2mV ", "  1mV ",
    " .5mV ", " .2mV ", " .1mV "		// x10
  };
  DpyStringPad_draw(pDpy, VoltTxt[Num], Color, 1);
  uint32_t Spaces = 0;
  const uint8_t *ptr = VoltTxt[Num] + 1;	// первый пробел пропустить
  while (*ptr++ == ' ') Spaces++;
  return Spaces;
}


/*__ Низкоуровневые функции ____________________________________________________*/


/*!*****************************************************************************
 @brief		Number draw
 @details	Напечатать число (32-битовую переменную) в Служебной зоне
 @param		number
 @return	
 @note	
 */
void Num_draw(int32_t number) {
  if (!ServCtrl.Numb) return;
  uint8_t TextBuf[12];			// текстовый буфер для тестового вывода
  itoad_conv(number, TextBuf);
  // uint8_t TextBuf[12] = "123456789AB";	// строка для проверки
  // DpyStringPad_draw(DPYPOS(DPYWAH+1,1), TextBuf, 0xf3f30000, 1);
  DpyStringPad_draw(DPYPOS(DPYWAH + 300 - 11 * 8 - 2, 1), TextBuf, 0xF3F30000, 1);
  // DpyStringPad_draw(DPYPOS(DPYWAH, 1), TextBuf, 0xF3F30000, true);
}

/*!*****************************************************************************
 @brief		Hexadecimal 32-bit variable draw
 @details	Напечатать 32-битовую переменную в 16-ричном виде в Служебной зоне
 @param		var - переменная
 @return	
 @note	
 */
void Hex32_draw(uint32_t var) {
  if (!ServCtrl.Numb) return;		// Чтобы не удалять везде 
  uint8_t TextBuf[11];			// текстовый буфер для тестового вывода
  uint8_t *pText = &TextBuf[10];
  *pText-- = 0;
  for (uint32_t i = 8; i--; ) {
    uint8_t dig = (var & 0xf);
    dig += (dig < 10)? 0x30 : 0x37;
    *pText-- = dig;
    var >>= 4;
  }
  *pText-- = 'x';
  *pText = '0';
  DpyStringPad_draw(DPYPOS(DPYWAH + 300 - 10 * 8 - 2, 1), TextBuf, 0xf3f30000, 1);
}

/*!*****************************************************************************
 @brief		Dump draw
 @details	Напечатать содержимое памяти (32-битовую переменную)
 @param		mem - указатель на переменную
 @return	
 @note	
 */
void Dump_draw(uint8_t *mem) {
  if (!ServCtrl.Dump) return;
/* текстовый буфер */
  uint8_t TextBuf[38] = {"0xMMMMMMMM: HHHHHHHHHHHHHHHH TTTTTTTT"}; 
  itoah_conv((uint32_t)mem, TextBuf + 2);
  uint8_t *pTxt = TextBuf + 12;
  for (int32_t i = 8; i > 0; i--) {
    uint8_t dig = (*mem & 0xf0)>>4;
    *pTxt++ = dig + ((dig < 10)? 0x30 : 0x37);
    dig = *(mem++) & 0x0f; 
    *pTxt++ = dig + ((dig < 10)? 0x30 : 0x37);
  }
  pTxt++;
  mem -= 8;
  for (int32_t i = 8; i > 0; i--) {
    *pTxt++ = *mem++;
  }
  DpyStringPad_draw(DPYPOS(DPYWAH + 1, 1), TextBuf, 0xf3f30000, 1);
}

/*!*****************************************************************************
 @brief		Draw Menu
 @details	нарисовать Меню (Заголовок меню, заголовки элементов, значения) 
 @return	
 @note
 */
void MenuHead_disp(void) {
/* Заголовки МЕНЮ [MenuNum]
   Номера МЕНЮ:
   MenuServ, MenuChan1, MenuMath, MenuChan2, MenuSweep, MenuTrig,
   MenuDisp, MenuAcq, MenuCurs, MenuMeas, MenuMem, MenuUtil */
  static const uint8_t *MenuHead[] = {
    " \x98\x98СЕРВИС",
    " КАНАЛ 1", " МАТ. ОБР", " КАНАЛ 2", "РАЗВертка", "  СИНХР",
    "\x98\x98ДИСПЛЕЙ", "\x98СБОР ИНФ", "\x98\x98КУРСОРЫ", "ИЗМЕРения",
    " \x98\x98ПАМЯТЬ", "\x98\x98УТИЛИТЫ"
  }; 

/* Заголовки элементов МЕНЮ [MenuNum][FNum(0:4)] */
  static const uint8_t *MenuItemHead[][5] = {
    "Калибровка", "\x98Тест ЦПУ", "\x98Код ПЛИС", "\x98Тест ЖКИ", "\x98Выполнить",
    "  Режим", " \x98Пробник", "\x98\x98Инверсия", "  \x98Связь", "Огр.полосы",
    "  Режим", " \x98Функция", " Источник", S_SPSM"Окно БПФ", S_SPSM S_SPSM"Смещение",
    "  Режим", " \x98Пробник", "\x98\x98Инверсия", "  \x98Связь", "Огр.полосы",
    "  Режим", "\x98Самописец", " \x98Предзап", "", "   X-Y",
    " Источник", " \x98Перепад", "", "  \x98Связь", "Шумоподав",
    " \x98Помощь", "  \x98Вывод", "  \x98Шкала", "", " Очистить", 
    "  Фильтр", "Пик\x98детект", " Усредн-е", " Накопл-е", "",
    "  Режим", " Параметр", " Курсор 1", " Курсор 2", "\x98\x98Курс 1_2",
    "  Режим", " Источник", S_SPSM"Параметр1", S_SPSM"Параметр2", "",
    "  \x98Длина", "Номер буф", "  Сигнал", "\x98Установки", "\x98Выполнить",
    "Калибратор", "Бал-с"S_SPSM"усил", "", "  Версия", "  Сброс"
  };
  
  DPY_RECT(DPYMAH, DPYMAV,      319, DPYMAV +  24, 0xFF, HEAD_BKG);
  DPY_RECT(DPYMAH, DPYMAV + 25, 319, DPYMAV + 200, 0xFF, ITEM_BKG);

  DPY_LINEH(DPYMAH + 1, DPYMAV +  1, 318, 0xff, HEAD_EMFA, 0xFF);
  DPY_LINEH(DPYMAH + 1, DPYMAV + 24, 318, 0xff, HEAD_EMRI, 0xFF);

  DPY_LINEH(DPYMAH + 1, DPYM1AV,      318, 0xff, ITEM_EMRI, 0xFF);
  DPY_LINEH(DPYMAH + 1, DPYM1AV + 34, 318, 0xff, ITEM_EMFA, 0xFF);
  DPY_LINEH(DPYMAH + 1, DPYM2AV,      318, 0xff, ITEM_EMRI, 0xFF);
  DPY_LINEH(DPYMAH + 1, DPYM2AV + 34, 318, 0xff, ITEM_EMFA, 0xFF);
  DPY_LINEH(DPYMAH + 1, DPYM3AV,      318, 0xff, ITEM_EMRI, 0xFF);
  DPY_LINEH(DPYMAH + 1, DPYM3AV + 34, 318, 0xff, ITEM_EMFA, 0xFF);
  DPY_LINEH(DPYMAH + 1, DPYM4AV,      318, 0xff, ITEM_EMRI, 0xFF);
  DPY_LINEH(DPYMAH + 1, DPYM4AV + 34, 318, 0xff, ITEM_EMFA, 0xFF);
  DPY_LINEH(DPYMAH + 1, DPYM5AV,      318, 0xff, ITEM_EMRI, 0xFF);
  DPY_LINEH(DPYMAH + 1, DPYM5AV + 34, 318, 0xff, ITEM_EMFA, 0xFF);

  DpyString_draw(DPYPOS(DPYMHH,DPYMHV), MenuHead[MenuNum], HEAD_TXT, 0); 

  DpyString_draw(DPYPOS(DPYMHH,DPYM1IV), MenuItemHead[MenuNum][0], ITEM_TXT, 0);
  DpyString_draw(DPYPOS(DPYMHH,DPYM2IV), MenuItemHead[MenuNum][1], ITEM_TXT, 0);
  DpyString_draw(DPYPOS(DPYMHH,DPYM3IV), MenuItemHead[MenuNum][2], ITEM_TXT, 0);
  DpyString_draw(DPYPOS(DPYMHH,DPYM4IV), MenuItemHead[MenuNum][3], ITEM_TXT, 0);
  DpyString_draw(DPYPOS(DPYMHH,DPYM5IV), MenuItemHead[MenuNum][4], ITEM_TXT, 0);

  MenuValue_disp[MenuNum][0]();
  MenuValue_disp[MenuNum][1]();
  MenuValue_disp[MenuNum][2]();
  MenuValue_disp[MenuNum][3]();
  MenuValue_disp[MenuNum][4]();  
}

/*!*****************************************************************************
 @brief		Menu Head Depress
 @details	Нарисовать отпущенную кнопку МЕНЮ
 @return	
 @note		
 */
void MenuHeadDepr_disp(void) {
  if (GenCtrl.Menu) {
    DPY_LINEH(DPYMAH + 1, DPYMAV +  1, 318, 0xff, HEAD_EMRI, 0xFF);
    DPY_LINEH(DPYMAH + 1, DPYMAV + 24, 318, 0xff, HEAD_EMFA, 0xFF);
  }
}

/*!*****************************************************************************
 @brief		Draw Help
 @details	Нарисовать Помощь
 @return	
 @note		
 */
void Help_draw(void) {
  DPY_RECT(DPYWAH, DPYWAV, DPYWAH + 250, DPYWAV + 200, 0xFF, HEAD_BKG);
}

/*!*****************************************************************************
 @brief		Clear Help MENU
 @details	Очистить Помощь МЕНЮ, если она включена
 @return	
 @note		Используется при входе в МЕНЮ
 */
void HelpMenu_clear(void) {
  if (DispCtrl.HelpEna) {		// Если есть Помощь МЕНЮ
//    Gen.Help = 0;
    // DPY_ClearScreen();		// Очистить весь экран
    //		Здесь нужно перерисовать строки состояния
    DpyWork_blank();
  }
  // else
  DpyMenu_blank();			// Очистить только МЕНЮ
  TrigLev_draw(true);
}

/*!*****************************************************************************
 @brief		Off/On Draw
 @details	Нарисовать Выкл / Вкл (на фоне)
 @param		pDpy - указатель на память индикатора
 @param		OffOn - 1 Вкл, 0 Выкл
 @return	
 @note	
 */
void DpyOffOn_draw(uint8_t *pDpy, bool On) {
  static const uint8_t *OffTxt = "Выкл";
  static const uint8_t *OnTxt  = "\x98Вкл";
  if (!On) {
    DpyRoRect_fill(pDpy, 28, 14, 0xFF << 8 | VON_BKG);
    DpyString_draw(pDpy + 320 * 3 + 1, OffTxt, VON_TXT, 0);
    DpyRect_fill(pDpy + 28, 28, 14, 0xFF << 8 | VOFF_BKG);
    DpyString_draw(pDpy + 320 * 3 + 31, OnTxt, VOFF_TXT, 0);
  }
  else {
    DpyRect_fill(pDpy, 28, 14, 0xFF << 8 | VOFF_BKG);
    DpyString_draw(pDpy + 320 * 3 + 1, OffTxt, VOFF_TXT, 0);
    DpyRoRect_fill(pDpy + 28, 28, 14, 0xFF << 8 | VON_BKG);
    DpyString_draw(pDpy + 320 * 3 + 31, OnTxt, VON_TXT, 0);
  }
}

/*!*****************************************************************************
 @brief		SETTING & Rotor Draw
 @details	Нарисовать УСТАНОВКА (_)
 @param		pDpy - указатель на память индикатора
 @param		On - 1 Вкл, 0 Выкл
 @return	
 @note	
 */
void DpySetting_draw(uint8_t *pDpy, bool On) {
  static const uint8_t *Set = "Установ";
  DpyString_draw(pDpy + 320 * 3, Set, VOFF_TXT, false);
  if (On)  DpyRoRect_fill(pDpy + 43, 14, 14, 0xFF << 8 | VON_BKG);
  else DpyRect_fill(pDpy + 43, 14, 14, 0xFF << 8 | VOFF_BKG);
  DpySpec_draw(pDpy + 46 + 320 * 1, SS_ROTR, 0xFF, VON_TXT);     
}

/*!*****************************************************************************
 @brief		Rotor Draw
 @details	Нарисовать (_)
 @param		pDpy - указатель на память индикатора
 @param		OffOn
 @return	
 @note	
 */
void DpyRotor_draw(uint8_t *pDpy, bool On) {
  if (On)  DpyRoRect_fill(pDpy, 14, 14, 0xFF << 8 | VON_BKG);
  else	   DpyRect_fill  (pDpy, 14, 14, 0xFF << 8 | VOFF_BKG);
  DpySpec_draw(pDpy + 3 + 320 * 1, SS_ROTR, 0xFF, VON_TXT);     
}

/*!*****************************************************************************
 @brief		Vertical Digits Bar Display
 @details	
 @return	
 @note	
 */
void DigitBar_disp(void) {
  DpyString_draw(DPYPOS(10, 4 + 15 *  0), "00", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 *  1), "10", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 *  2), "20", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 *  3), "30", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 *  4), "40", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 *  5), "50", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 *  6), "60", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 *  7), "70", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 *  8), "80", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 *  9), "90", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 * 10), "A0", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 * 11), "B0", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 * 12), "C0", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 * 13), "D0", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 * 14), "E0", WHITE_ON_BLACK, 1);
  DpyString_draw(DPYPOS(10, 4 + 15 * 15), "F0", WHITE_ON_BLACK, 1);
}

/*!*****************************************************************************
 @brief		32-bit Integer with sign convert to ASCII decimal
 @details	32-битовое целое со знаком преобразуется в строку \n
 @details	формат строки: " ... 0", " ...+1", " ...-32767", "-2147483647"
 @param		numb - число
 @param		str - указатель на строку-буфер (12 байтов)
 @return	
 */
void itoad_conv(int32_t numb, uint8_t *str) {
  *(uint32_t *)(str)   = 0x20202020;
  *(uint32_t *)(str + 4) = 0x20202020;    
  *(uint32_t *)(str + 8) = 0x00302020;
  str += 10;
  uint8_t  sign = '+';
  if (!numb) return;
  if (numb < 0) { numb = -numb; sign = '-'; }
  do { *str-- = numb % 10 + '0'; } while (numb /= 10);
  *str = sign;
}

/*!*****************************************************************************
 @brief		32-bit Integer convert to ASCII hexadecimal
 @details	32-битовое беззнаковое целое преобразуется в строку
 @param		numb - число
 @param		str - указатель на строку-буфер
 @return	
 */
void itoah_conv(uint32_t numb, uint8_t *str) {
  for (int32_t i = 8; i > 0; i--) {
    uint8_t dig = (numb & 0xF0000000)>>28;
    dig += (dig < 10)? 0x30 : 0x37;
    *str++ = dig;
    numb <<= 4;
  }
}




/*! @} */
