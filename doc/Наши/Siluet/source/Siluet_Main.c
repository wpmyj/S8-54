
/*!*****************************************************************************
 @file		Siluet_main.c
 @author	Козлячков В.К.
 @date		2012.10 - 2013.09
 @version	V1.0
 @brief		Siluet CPU main program
 @details	Осциллограф Силуэт
 @note		STM32F207ZC
 *******************************************************************************
 @verbatim
 Управляющие структуры хранятся в Backup RAM
 
 Main задача занимается выводом изображения на индикатор
 Display задача удалена
 Автоматическими измерениями занимается задача Meas 
 
 @endverbatim
 *******************************************************************************/
 
#define VAR_DECLS		// Здесь будут объявлены переменные

/*__ Заголовочные файлы _______________________________________________________*/

#include "Siluet_Main.h"	// Все включения заданы в одном файле

#ifdef USE_USB_INTERFACE
// global struct with device type
USB_OTG_CORE_HANDLE    USB_OTG_dev;
#endif	// USE_USB_INTERFACE

#ifdef	USE_SCPI
extern Ring_Buffer_TypeDef RingRxBuffer;
#endif	// USE_SCPI

/*__ Прототипы функций ________________________________________________________*/

void Panel_thread(void const *argument);
void Signal_thread(void const *argument);
void Meas_thread(void const *argument);
#ifdef	USE_SCPI
void SCPI_thread(void const *argument);
#endif	// USE_SCPI


/*__ Переменные _______________________________________________________________*/


/* Указатели на задачи (идентификаторы задач) */
osThreadId	Main_id;	//!< указатель на задачу main
osThreadId	Panel_id;	//!< Указатель на задачу Panel_thread
osThreadId	Signal_id;	//!< Указатель на задачу Signal_thread
osThreadId	Meas_id;	//!< Указатель на задачу Meas_thread
#ifdef	USE_SCPI
osThreadId	SCPI_id;	//!< Указатель на задачу SCPI_thread
#endif	/* USE_SCPI */

/* Определения задач (структуры)
   osThreadDef(name, priority, instances, stacksz) */
// osThreadDef(Panel_thread, osPriorityNormal, 1, 0);
osThreadDef(Panel_thread, osPriorityAboveNormal, 1, 0);
// osThreadDef(Panel_thread, osPriorityNormal, 1, 400);
// osThreadDef(Signal_thread, osPriorityBelowNormal, 1, 0);
// osThreadDef(Signal_thread, osPriorityNormal, 1, 0);
// osThreadDef(Signal_thread, osPriorityHigh, 1, 512);
osThreadDef(Signal_thread, osPriorityAboveNormal, 1, 512);
osThreadDef(Meas_thread, osPriorityAboveNormal, 1, 0);
#ifdef	USE_SCPI
osThreadDef(SCPI_thread, osPriorityAboveNormal, 1, 0);
#endif	/* USE_SCPI */

/* Мьютекс доступа к образу индикатора */
osMutexDef	(MutexDpy);	//!< Имя мьютекса
osMutexId	MutDpy_id;	//!< Указатель на мьютекс


/*__ Глобальные функции _______________________________________________________*/

/*!*****************************************************************************
 @brief		Main thread
 @details	Основная задача - инициализировать устройства
 @param	
 @return	
 @note		После выполнения задача уничтожается
 */
int32_t main(void) {

/* Инициализировать центральный микроконтроллер */
  MCU_init();
 
/* Проверить внешнее ОЗУ */
//  XRAM_test();
//  if (ErrorState == ERROR_XRAM) Led_blink(62500);	// 8 раз в сек
  
/* Очистить память контроллера ЖКИ (залить черным цветом)
   Без этого на экране виден шум
   ШИМ подсветки всключается с задержкой, поэтому очистка не нужна */
//  LcdMem_clear();

/* Инициализировать контроллер ЖКИ */  
  LcdCtrl_init();

#ifdef DISPLAY_TEST
  LcdMem_test();
  if (ErrorState == ERROR_DPY) Led_blink(125000);	// 4 раза в сек
#endif

/* Прочитать идентификатор памяти! */
  if (SFMID_read() != M25PE40ID) {
    ErrorState = ERROR_SFM;
    Led_blink(250000);					// 2 раза в сек
  }
  
/* Конфигурация ПЛИС
   После включения ПЛИС находится в состоянии POR и держит STATUSn низким
   4 попытки конфигурации ПЛИС */
  for (uint32_t trials = 4; trials--; ) {
    ErrorState = PL_config();
    if (ErrorState == ERROR_NONE) break;
  }
  
/* Для ПЛИС нужна задержка между Config_Done и User_Mode 650 µs
   Очистить память буфера ЖКИ */
  DpyScreen_blank();
  
/* Если ПЛИС не сконфигурировалась, на рабочий цикл не выходить
   Если нет кода конфигурации, зажечь светодиод */
///  if (ErrorState == ERROR_CONF) Led_blink(500000);	// 1 раз в сек
  if (ErrorState == ERROR_CODE || ErrorState == ERROR_CONF) LED_ON();
/* Инициализировать регистры ПЛИС, если сконфигурирована */ 
  else PL_init();		// если не режим программирования  

/* Восстановить структуры смещения в каналах */
  Balance_init();
  
/* Инициализировать режимы управления */
  Ctrl_init();

/* Задать начальные значения в аналоговом модуле */
  AnaReg_send();
  // ANADAC(DAC_CHAN1, Chan1Ctrl.SigPos << 2);
  // FpPos1_hand(0);
  // ANADAC(DAC_CHAN2, Chan2Ctrl.SigPos << 2);
  // FpPos2_hand(0);
  ANADAC(DAC_LEVEL, TrigCtrl.Level << 2);

/* Извлечь сохраненные значения интерполятора */
  uint32_t tmts = SFMWord_read(SFMINTERP);
  if (~tmts) {
    Interp.Tmts = tmts;
    Interp.Tmad = SFMWord_read(SFMINTERP + 4);
  }
 
/* Заполнить буфер ЖКИ во внешней памяти */
#ifdef BKG_GRAY
  DpyRect_fill(DPYPOS(0, 0), 320, 240, 0x0000FF20);	// выделить фон (серый)
#endif
  Manufact_draw();		// Написать имя производителя
  DpyScale_draw();		// Нарисовать шкалу
  MemBar_draw();
  MemBarPret_draw(true);
  MemBarPos_draw(true);
  MemSizeBuf_disp();
  Chan1St_disp();
  Chan2St_disp();
  TimeSt_disp();
  SweepSt_disp();
  TrigSt_disp();

#ifdef MARKS_DRAW
/* Нарисовать маркеры в углах экрана */
  DpyRect_fill(DPYPOS(0,       0),       2, 2, 0x0000FFFC);
  DpyRect_fill(DPYPOS(320 - 2, 240 - 2), 2, 2, 0x0000FFF3);
#endif
  
#ifdef SIGNAL_TEST
/* Заполнить буферы сигналов тестовыми сигналами */
  SigChanAC_load();			// заполнить буфер синусом и косинусом
//  SigChanDC_load();			// заполнить буфер постоянным напряжением
  SigChans_redraw();
#endif

  PosChans_redraw();
  TrigLev_draw(true);
  Pret_draw(true);
  
  DpyBuf2Mem_copy(0, 320 * 240);


  


/* Читать (и сбросить) состояние синхронизации, индицировать */
  if (PL->STAT & PL_STAT_TRIGDET)	// Если есть синхронизация
    PanCtrl.TxBuf &= ~(1 << 4);		// Зажечь диод синхронизации
  else
    PanCtrl.TxBuf |=  (1 << 4);		// Погасить диод синхронизации
  PanCtrl.TxBuf |= 0x40;
  USART3->DR = PanCtrl.TxBuf;		// Послать в панель

/* Задать функцию развертки */  
  Sweep_init();

#ifdef	USE_USB_INTERFACE
//* Инициировать USB интерфейс   *//
	USBD_Init(&USB_OTG_dev, USB_OTG_HS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);
#endif	/*	USE_USB_INTERFACE	*/

/* Задать идентификатор задачи */
  Main_id = osThreadGetId();
  osThreadSetPriority(Main_id, osPriorityAboveNormal);
  
  MutDpy_id = osMutexCreate(osMutex(MutexDpy));
  
/* Задать идентификаторы других задач */
  Panel_id = osThreadCreate(osThread(Panel_thread), NULL);
#ifdef	USE_SCPI
	SCPI_id = osThreadCreate(osThread(SCPI_thread), NULL);
#endif	/* USE_SCPI	*/

  if (ErrorState != ERROR_CODE || ErrorState != ERROR_CONF) {
    Signal_id = osThreadCreate(osThread(Signal_thread), NULL);
    Meas_id = osThreadCreate(osThread(Meas_thread), NULL);
  }

  osDelay(100);
  
// Включить PWM
  *(uint16_t *)PWMCVCONF |= 0x10;

/* Main task Superloop */
  for ( ; ; ) {
    // IWDG->KR =  0xAAAA;		// Reload Independent Whatchdog Timer  
    osSignalWait(0, osWaitForever);	// ожидать любого запроса на вывод
//    LED_ON();
    osMutexWait(MutDpy_id, osWaitForever);
    DpyBuf2Mem_copy(0, 320 * 240);
    osMutexRelease(MutDpy_id);
//    LED_OFF();
    // osDelay(100);			// Задержка на 100 ms
    // osDelay(50);			// Задержка на 50 ms
  }

}


/*__ Задачи ___________________________________________________________________*/


/*!*****************************************************************************
 @brief		Panel thread
 @details	Обработать панель управления
 @param		
 @return	
 @note	
 */
void Panel_thread(void const *argument) {
  for ( ; ; ) {
    osSignalWait(0, osWaitForever);	// ожидать запрос на обработку
    // osSignalClear(Main_id, 1);
    osMutexWait(MutDpy_id, osWaitForever);
    Panel_Process();
    osMutexRelease(MutDpy_id);
///    osSignalSet(Display_id, 1);
    osSignalSet(Main_id, 1);  		// запрос на индикацию
  }
}


/*!*****************************************************************************
 @brief		Signal Service task
 @details	Задача обработки сигнала
 @param		
 @return	
 @note	
 */
#if false
void Signal_thread(void const *arguments) {
  for ( ; ; ) {
    if (!PLIRQ_GET()) {			// Получен запрос прерывания
      uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
      if (stat & PL_STAT_TDTIF) {	// Если есть прерывание по изменению...
        if (stat & PL_STAT_TRIGDET)	// Если есть синхронизация
	  PanCtrl.TxBuf &= ~(1 << 4);	// Зажечь диод синхронизации
	else
	  PanCtrl.TxBuf |=  (1 << 4);	// Погасить диод синхронизации
        PanCtrl.TxBuf |= 0x40;
        USART3->DR = PanCtrl.TxBuf;	// Послать в панель
      }
/* Если нет СТОПа и есть флаги кадра или точки */
      if (!SweepCtrl.Stop && stat & (PL_STAT_FRMIF | PL_STAT_DOTIF)) {
        osMutexWait(MutDpy_id, osWaitForever);
        Sweep_exe();
        // Math_exe();
	// XYDots_draw(true);
        osMutexRelease(MutDpy_id);
//	if (MeasCtrl.On) {
//	  osMutexWait(MutDpy_id, osWaitForever);
//	  SigPar_meas();			// Измерения (если есть)
//	  osMutexRelease(MutDpy_id);
//	  osDelay(200);				// 5 раз в сек
//	}
        if (MeasCtrl.On) osSignalSet(Meas_id, 1);
        osSignalSet(Main_id, 2); 
      }
    }					// if PLIRQ
  }					// for (;;)
}
#endif

void Signal_thread(void const *arguments) {
  for ( ; ; ) {
    osSignalWait(0, osWaitForever);	// Ждать сигнал от ПЛИС
    uint_fast16_t stat = PL->STAT;	// Читать и сбросить состояние
    if (stat & PL_STAT_TDTIF) {		// Если есть прерывание по изменению...
      if (stat & PL_STAT_TRIGDET)	// Если есть синхронизация
        PanCtrl.TxBuf &= ~(1 << 4);	// Зажечь диод синхронизации
      else
	PanCtrl.TxBuf |=  (1 << 4);	// Погасить диод синхронизации
      PanCtrl.TxBuf |= 0x40;
      USART3->DR = PanCtrl.TxBuf;	// Послать в панель
    }
/* Если нет СТОПа и есть флаги кадра или точки */
    if (!SweepCtrl.Stop && stat & (PL_STAT_FRMIF | PL_STAT_DOTIF)) {
      osMutexWait(MutDpy_id, osWaitForever);	// Ждать освобождение мьютекса
      Sweep_exe();
      // Math_exe();
      // XYDots_draw(true);
      osMutexRelease(MutDpy_id);
      if (MeasCtrl.On) osSignalSet(Meas_id, 0x00000001);
      osSignalSet(Main_id, 0x00000002); 
    }
  }					// for (;;)
}



/*!*****************************************************************************
 @brief		Signal Measure task
 @details	Задача автоматических измерений сигнала
 @param		
 @return	
 @note	
 */
void Meas_thread(void const *arguments) {
  for ( ; ; ) {
    osSignalWait(0, osWaitForever);	// ожидать запрос на обработку
    osDelay(200);			// в ms! 5 раз в сек  
    osMutexWait(MutDpy_id, osWaitForever);
    SigPar_meas();			// Измерения
    osMutexRelease(MutDpy_id);
    osSignalSet(Main_id, 1);      
  }
}

#ifdef	USE_SCPI
/*!*****************************************************************************
 @brief		SCPI task
 @details	Задача разбора команд, принятых через внешний интерфейс
 @param		
 @return	
 @note	
 */
void SCPI_thread(void const *arguments) {
  for ( ; ; ) {
		osEvent RcvdSignal;
		RcvdSignal = osSignalWait(0, osWaitForever);
	  osMutexWait(MutDpy_id, osWaitForever);
#ifdef	USE_RING_BUFFER	
  do {
    SCPI_ParseOneCommandFromBuffer( &RingRxBuffer);
  } while (CorrectRingBufferIndexes(&RingRxBuffer));
#else
  SCPI_ParseOneCommandFromBuffer(USB_Rx_Buffer.Data);
#endif /* USE_RING_BUFFER */
   // ho-ho, here will be cucle for calling   SCPI_ParseOneCommandFromBuffer() till ringbuffer will not be empty.
	 // also here should be some trigger for starting & stoping parsing process.
	 osMutexRelease(MutDpy_id);
  }
}
#endif	/* USE_SCPI	*/


/*__ Обработчики прерываний ___________________________________________________*/


/*!*****************************************************************************
 @brief		NonMaskable Interrupt Handler
 @details	Очистить флаг прерывания CSS
 @param		none
 @return	
 @note	
 */
void NMI_Handler(void) {
  RCC->CIR |= RCC_CIR_CSSC;
}

/*!*****************************************************************************
 @brief		HardFault Handler
 @details	Обработчик исключения, читает адрес команды, на которой возникло
 @param		none
 @return	
 @note		и зажигает светодиод
 */
__asm void HardFault_Handler(void) {
  TST LR, #4 
  ITE EQ 
  MRSEQ R0, MSP				; Main Stack was used, put MSP in R0
  MRSNE R0, PSP				; Process Stack was used, put PSP in R0
  LDR R0, [R0, #24]			; Get stacked PC from stack

  LDR R1, =0x40020418			; GPIOB->BSRR
  MOVS R2, #0x0002			; Bit 2
  STRH R2, [R1, #2]			; Reset bit (LED_ON)
  B .
}

/*!*****************************************************************************
 @brief		HardFault generate 
 @details	Создать исключительное событие (для проверки)
 @param		none
 @return	
 @note		Create unaligned multiple load/store
 */
__asm void HardFault_generate(void) {
  MOVS R0, #1
  LDM R0, {R1-R2}
  B .
}

/*!*****************************************************************************
 @brief		USART3 Receive IRQ Handler
 @details	Обработчик прерывания от панели управления по USART3
 @param		none
 @return	
 @note		LSB передается первым
 */
void USART3_IRQHandler(void) {
  uint8_t tmp = USART3->DR;		// временное хранение принятого байта
  if (PanCtrl.First) {			// есть первый байт сообщения
    PanCtrl.RcBuf[1] = tmp;		// записать второй байт
    PanCtrl.Ready = 1;			// сообщение принято - готово
    PanCtrl.First = 0;			// ждать первого байта следующей команды
    osSignalSet(Panel_id, 1);
  }
  else {				// если нет первого байта сообщения
    if (tmp < 32) {			// если номер кнопки допустимый
      PanCtrl.RcBuf[0] = tmp;		// записать первый байт
      PanCtrl.First = 1;		// первый байт принят
      PanCtrl.Ready = 0;		// на всякий случай "не готово"
    }
  }
}

#ifdef	USE_SCPI
/*!*****************************************************************************
 @brief		USART2 Receive IRQ Handler
 @details	Обработчик прерывания от интерфейса RS-232
 @param		none
 @return	
 @note		
 */
void USART2_IRQHandler(void) {
  uint8_t tmp = USART3->DR;
  RING_BUFF_WRITE(RingRxBuffer, tmp);
  RingRxBuffer.source = RS232;
  if(!tmp) osSignalSet(SCPI_id, 1);
}
#endif	/* USE_SCPI */

/*!*****************************************************************************
 \brief		EXTI9_5 IRQ Handler
 \note		Общий обработчик для нескольких источников прерывания!
 */
void EXTI9_5_IRQHandler(void) {
  osSignalSet(Signal_id, 0x00000001);
  EXTI->PR = EXTI_PR_PR7;	// Clear pending bit
}


#if false
/*!*****************************************************************************
 @brief		printf remap
 @param		none
 @return	
 @note		
 */
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

struct __FILE { int handle; /* Add whatever needed */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}
#endif
