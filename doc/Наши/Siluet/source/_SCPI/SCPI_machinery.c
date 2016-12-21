/**
  ******************************************************************************
  * @file    SCPI_machinery.c
  * @author  san.sergeich
  * @version V0.1
  * @date    18-September-2013
  * @brief   SCPI parser functions
  ******************************************************************************
  * 
  *
  *
  ******************************************************************************
  */
	
	
	#include "SCPI_defines.h"
	#include "SCPI_include.h"
	
	#include "../Siluet_Main.h"
	
	#include "stdio.h"
		/////////////////////////////////////////////////////
		//				LOCAL FUNCTIONS PROTOTYPES							 //
		/////////////////////////////////////////////////////

void MakeStringLowerCase(uint8_t string[], uint16_t Count);
void MakeStringUpperCase(char string[], uint32_t Count);
bool IsNumber(uint8_t ch);

// extern int   rt_init_mem  (void *pool, U32  size);
// extern void *rt_alloc_mem (void *pool, U32  size);
// extern int   rt_free_mem  (void *pool, void *mem);


//void SCPI_Error_Handler(SCPI_Errors_TypeDef ErrCode);
char message[128]; 
extern USB_Tx_Buffer_TypeDef 				USB_Tx_Buffer;
extern osMutexId	MutDpy_id;
//uint8_t* SCPI_ParseOneCommandFromBuffer(const uint8_t* CmBuffer);

// temporary here
// nedd for range-settings
static const double VoltDivValues[] = {
  500,				// (0)	500 V/div 
	200,				// (1)	200 V/div
	100,				// (2)	100 V/div
	50,					// (3)	50 V/div	____
	20,					// (4)	20 V/div
  10,					// (5)	10 V/div
  5,					// (6)	 5 V/div
  2,					// (7)	 2 V/div
  1,					// (8)	 1 V/div
  0.5,				// (9)	0.5 V/div
  0.2,				// (10)	0.2 V/div *
  0.1,				// (11)	0.1 V/div
  50e-3,			// (12)	50 mV/div
  20e-3,			// (13)	20 mV/div
  10e-3,			// (14)	10 mV/div
  5e-3,				// (15)	5 mV/div
  2e-3,				// (16)	2 mV/div
	NULL
};

static const double ProbeCoefTable[] = {
	100,				// (0) 	1/100 probe
	10,					// (1)	1/10 probe
	1,					// (2)	1/1 probe
	0.1					// (3)	x10 probe
};


// temporary here
// nedd for range-settings
  static const double TimeDivValues[] = {
  50.0000000,			//  0   50 s/div (2 s/dot)
  20.0000000,			//  1   20 s/div (0.8 s/dot)
  10.0000000,			//  2   10 s/div (0.4 s/dot)
   5.0000000,			//  3    5 s/div (0.2 s/dot)
   2.0000000,			//  4    2 s/div (80 ms/dot)
   1.0000000,			//  5    1 s/div (40 ms/dot)
   0.5000000,			//  6  0.5 s/div (20 ms/dot)
   0.2000000,			//  7  0.2 s/div (8 ms/dot)
   0.1000000,			//  8  0.1 s/div (4 ms/dot)
   0.0500000, 		//  9  50 ms/div (2 ms/dot)
   0.0200000, 		// 10  20 ms/div (0.8 ms/dot)
   0.0100000,			// 11  10 ms/div (0.4 ms/dot)
   0.0050000,			// 12   5 ms/div (0.2 ms/dot)
   0.0020000,			// 13   2 ms/div (80 us/dot)
   0.0010000,			// 14   1 ms/div (40 us/dot)
   0.0005000,			// 15 0.5 ms/div (20 us/dot) *
   0.0002000,			// 16 0.2 ms/div (8 us/dot)
   0.0001000,			// 17 0.1 ms/div (4 us/dot)
   0.0000500,			// 18  50 us/div (2 us/dot)
   0.0000200,			// 19  20 us/div (0.8 us/dot)
   0.0000100,			// 20  10 us/div (0.4 us/dot)
   0.0000050,			// 21   5 us/div (0.2 us/dot)
   0.0000020,			// 22   2 us/div (80 ns/dot)
   0.0000010,			// 23   1 us/div (40 ns/dot)
   0.0000005,			// 24 0.5 us/div (20 ns/dot)
   0.0000002,			// 25 0.2 us/div (8 ns/dot)
//--------------------------------------------------
			100e-9,			// 26 0.1 us/div (4 ns/dot)
			050e-9,			// 27  50 ns/div (Random sample)
			020e-9, 		// 28  20 ns/div
			 10e-9,			// 29  10 ns/div
			 20e-9,			// 30   5 ns/div
			 50e-9,			// 31   2 ns/div
				1e-9,			// 32   1 ns/div
				5e-10,		// 33 0.5 ns/div
				2e-10,		// 34 0.2 ns/div
				1e-10,		// 35 0.1 ns/div
				NULL
  };


	
	///////////////////////////////////////////////
	//					MAIN PROCESSING FUNCTION				 //	
	///////////////////////////////////////////////
	
	
	
/*******************************************************************************
* @Name 			 : ParseCommandFromBuffer();
* @Brief  		 : parse one command from buffer & calls appropriate callback
* @param       : pointer to string buffer with command or to the ring buffer struct
* @Output      : Callback actions
* @Return      : Pointer to the end of command or ring buffer undex.
* @Note				 : Need in some SCPI-datatypes to be declared (See SCPI_defines.h), need callbacks to be declared; 
*******************************************************************************/	
#ifdef 	USE_RING_BUFFER	
void SCPI_ParseOneCommandFromBuffer( Ring_Buffer_TypeDef* RingCmBuffer)	{
#else	
	uint8_t* SCPI_ParseOneCommandFromBuffer(const uint8_t* CmBuffer)	{	
#endif	/*	USE_RING_BUFFER	*/
	
	//declarations for data processing	
	SCPI_CommandContext_TypeDef CommandContext;

	uint32_t	i;
	uint32_t	CmBufferLength;
	uint8_t*  SCPI_CommandTokenList[4];

//	char* RecievedCommand;

	char Delimiters[] = ": ;?*";
	char* IsQuery;
	char	tmp;
	char RecievedCommand[64]; 
		
	///////////////////////////////////////////////
	//							DATA PREPARATION						 //	
	///////////////////////////////////////////////
	

	// detect single command length
#ifdef 	USE_RING_BUFFER	

		for(i=0;;i++)	{
			RING_BUFF_READ_BY_ADDR(RingCmBuffer, tmp);
			RecievedCommand[i] = tmp;
			if((NULL == tmp)||(';' == tmp))	break;
		}
		CmBufferLength = i;

// 	for(i = RingCmBuffer->readIndex;i < RingCmBuffer->writeIndex ; i++)	{
// 		if((!RingCmBuffer->data[i])||(';' == RingCmBuffer->data[i]))	{
// 			CmBufferLength = i - RingCmBuffer->readIndex;
// 			break;
// 		}
// 	}
	
#else		
	char* ptr;
	ptr = strchr((char*)CmBuffer,';');
	if(!ptr)	{
		CmBufferLength = strlen((char*)CmBuffer);
	}	else	{
		CmBufferLength = ptr - (char*)CmBuffer;
	}

#endif	/*	USE_RING_BUFFER	*/	
	
	// allocate some memory for current command
//	rt_alloc_mem(RecievedCommand,CmBufferLength * sizeof(char));
//	RecievedCommand = (char*)calloc(CmBufferLength, sizeof(char));
//		RecievedCommand = (char*)malloc(CmBufferLength * sizeof(char));	

	
	// make a copy of buffer string (take it from USB buffer to another place in memory)	
#ifdef 	USE_RING_BUFFER	
// 	strncpy( RecievedCommand, (char*)RingCmBuffer->data, CmBufferLength);
// 	for(i=0; tmp; i++) RING_BUFF_READ_BY_ADDR(RingCmBuffer, tmp); // move ReadIndex start of next command;
#else		
	strcpy( RecievedCommand, (char*)CmBuffer);
#endif	/*	USE_RING_BUFFER	*/	
	
	// make case-independing string processing (convert to upper-case-only, then work only with it)
	MakeStringUpperCase(RecievedCommand, CmBufferLength);
		
	
	///////////////////////////////////////////////
	//							DATA PROCESSING							 //	
	///////////////////////////////////////////////
	
	
	// check, if command is Query
	IsQuery = strchr(RecievedCommand, '?');
	
	// make token list from RecievedCommand
// // // //	SCPI_CommandTokenList[0] = (uint8_t*)strtok(RecievedCommand, Delimiters);
// // // //	for(i = 1; (SCPI_CommandTokenList[i] = (uint8_t*)strtok(NULL, Delimiters)); i++);			//warning here is ok. nothing need to be fixed 
	
	// make token list from RecievedCommand
	SCPI_CommandTokenList[0] = (uint8_t*)strtok(RecievedCommand, Delimiters);												// no-warning wersion
	i = 1;
	do	{
 		SCPI_CommandTokenList[i] = (uint8_t*)strtok(NULL, Delimiters);
	}	while(NULL != SCPI_CommandTokenList[i++]);

	
	// analyze tokens
	//root command
	for(i=0; NULL != RootCommandList[i].pattern; i++)	{
		if(!strcmp((char*)RootCommandList[i].pattern, (char*)SCPI_CommandTokenList[0])	)	{							
				CommandContext.callback = RootCommandList[i].callback;
				break;
		}
	}
	// second lvl command	
	for(i=0; NULL != SecondLvlCommandList[i].pattern; i++)	{
		if(!strcmp((char*)SecondLvlCommandList[i].pattern, (char*)SCPI_CommandTokenList[1])	)	{							
				CommandContext.command = SecondLvlCommandList[i].command;
				break;
		}
	}
	
	// detect unit value or selector
	char* UnitPos;
	
	if(!IsQuery)	{

		if(IsNumber(SCPI_CommandTokenList[2][0]))	{
			CommandContext.parameter.value = strtod((char* )SCPI_CommandTokenList[2], &UnitPos);
			for(i = 0; NULL != UnitList[i].pattern; i++)	{
				if(!strcmp((char*)UnitList[i].pattern, UnitPos))	{
					CommandContext.parameter.value *= UnitList[i].mult;
					break;
				}
			}
		}	else	{
			for(i = 0; NULL != SelectorsList[i].pattern; i++)	{
				if(!strcmp((char*)SelectorsList[i].pattern, (char*)SCPI_CommandTokenList[2])	)	{							
						CommandContext.parameter.selector = SelectorsList[i].selector;
						break;
				}
			}
		}
	} else	{
			CommandContext.parameter.selector = QUERY;
	}
	
	if(CommandContext.callback) { CommandContext.callback(CommandContext.command, CommandContext.parameter);
	}	else	{SCPI_Error_Handler(UNKNOWN_COMMAND_ERROR);};	
	

	// free allocated memory
//		rt_free_mem(RecievedCommand,CmBufferLength * sizeof(char));
//	free(RecievedCommand);
#ifdef 	USE_RING_BUFFER	
	//return (uint8_t*)(&RingCmBuffer->data[CmBufferLength]+1);			//?? maybe all work are done with readIndex, so...maybe it should be void
#else
	return (uint8_t*)(&CmBuffer[CmBufferLength]+1);
#endif	/*	USE_RING_BUFFER	*/	
}


	///////////////////////////////////////////////
	//							UTILS & FUNCTIONS						 //	
	///////////////////////////////////////////////

#ifdef 	USE_RING_BUFFER	
/*******************************************************************************
* @Name 		 		  : CopyRxBufferToRing;
* @Brief  		 	  : Take data from USB_Rx_Buffer to the Ring Buffer
* @param          : SCPI_Errors_TypeDef ErrCode
* @Output         : None.
* @Return         : None.
*******************************************************************************/
void CopyRxBufferToRing( const USB_Rx_Buffer_TypeDef* USB_Rx_Buffer, Ring_Buffer_TypeDef* Ring_Buffer)	{
	uint32_t i;
	
	for(i = 0; (NULL != USB_Rx_Buffer->Data[i]); i++)	{
		RING_BUFF_WRITE_BY_ADDR(Ring_Buffer, USB_Rx_Buffer->Data[i]);
	}
}


/*******************************************************************************
* @Name 		 		  : CorrectRingBufferIndexes();
* @Brief  		 	  : Мove readIndex fot compensation of ambiguous end of command string
* @param          : RingBuffer struct pointer
* @Output         : None.
* @Return         : None.
*******************************************************************************/
RingBuffStatus_TypeDef CorrectRingBufferIndexes(Ring_Buffer_TypeDef* Ring_Buffer)	{
	uint8_t tmp;
	for(;':'!= tmp;)	{	
		RING_BUFF_READ_BY_ADDR(Ring_Buffer, tmp);
		if(RING_BUFF_EMPTY_BY_ADDR(Ring_Buffer)) return RING_EMPTY;
	}
	Ring_Buffer->readIndex--;
	return(RING_OK);
}

#endif	/*	USE_RING_BUFFER	*/	

/*******************************************************************************
* @Name  				  : MakeStringLowerCase();
* @Brief 			    : converts cahrs from upper case to lower case
* @Input          : String pointer, character count
* @Output         : Converted String
* @Return         : None.
*******************************************************************************/
void MakeStringLowerCase(uint8_t string[], uint16_t Count)	{
	uint32_t i;
	if(0 == Count) return;
	for(i=0; i<Count; i++)	{
		if((0x40<string[i])&&(string[i]<0x5B)) string[i] = string[i] + 0x20 ;
	}
}	

/*******************************************************************************
* @Name  			  	: MakeStringUpperCase();
* @Brief		      : converts cahrs from lower case to upper case
* @Input          : String pointer, charactger count
* @Output         : Converted String
* @Return         : None.
*******************************************************************************/
void MakeStringUpperCase(char string[], uint32_t Count)	{
	uint32_t i;
	if(0 == Count) return;
	for(i=0; i<Count; i++)	{
		if((0x60<string[i])&&(string[i]<0x7B)) string[i] = string[i] - 0x20 ;
	}
}	

/*******************************************************************************
* @Name				   : IsNumber();
* @Brief         : returns ansver on qestion "It is number?"
* @Input         : char
* @Return        : bool.
*******************************************************************************/
bool IsNumber(uint8_t ch)	{
	if(((0x30 <= ch)&&(ch <= 0x39))||(0x2E == ch)||(0x2B == ch)||(0x2C == ch)||(0x2D == ch)) return TRUE;
	return FALSE;
}

/*******************************************************************************
* @Name				   : SCPI_RangeSelector();
* @Brief		     : return numder of range selector;
* @Input         : double value, double * ValuesTable
* @Output        : None.
* @Return        : number
*******************************************************************************/
uint8_t SCPI_RangeSelector(double Value, const double * ValuesTable, const double Coef)	{
uint8_t i;
	
	for(i = 0;ValuesTable[i]*Coef > Value;i++) {
		if(ValuesTable[i]*Coef  == NULL)	return i-1;
	}
	return i;
}

/*******************************************************************************
* @Name				   : SCPI_Message();
* @Brief		     : return numder of range selector, if length = NULL, it calculates automatically;
* @Input         : uint8_t message , uint32_t length
* @Output        : None.
* @Return        : number
*******************************************************************************/
void SCPI_Message( uint8_t * string, uint32_t length)	{

	uint32_t i;
	//while(true == USB_Tx_Buffer.TxState );		// wait, if transfer is not completed;
	USB_Tx_Buffer.pData = string;
	USB_Tx_Buffer.Length = length;
	
	if(NULL == length)	{
	for(i=0;NULL != string[i]; i++);
	USB_Tx_Buffer.Length = i;
	}
	
	USB_Tx_Buffer.Ready = true;
	//USB_Tx_Buffer.Transfer = false;
}

/*******************************************************************************
* @Name				   : SCPI_Measure_to_Message();
* @Brief		     : return numder of range selector;
* @Input         : double value, double * ValuesTable
* @Output        : None.
* @Return        : number
*******************************************************************************/
void  SCPI_Measure_to_Message(int32_t Value, const Unit_type_TypeDef Type)	{
	double tmpVal;
	//uint32_t length;
	static char Message[16];// = "sprintf disabled";
	
	switch(Type)	{
		case TIME:	{
			tmpVal = TimeDivValues[SweepCtrl.TimeNum] * 0.04 * (double)Value;																			// just simple Seconds
			break;
		}
		case VOLTAGE:	{
			tmpVal = VoltDivValues[Chan1Ctrl.VoltNum] * ProbeCoefTable[Chan1Ctrl.Prb] * 0.04 * (double)Value;			// just simple volts
			break;
		} 
		case FREQENCY:	{
			tmpVal = (TimeDivValues[SweepCtrl.TimeNum] * 0.04 * (double)Value);
			if(0 != tmpVal) tmpVal = 1/tmpVal;																																		// just simple Hertz
			break;
		}
	}

	//length = 
	sprintf(Message, "%e \n", tmpVal );
	SCPI_Message((uint8_t*)Message, NULL);
}

/*******************************************************************************
* @Name				   : SCPI_BufferSize_to_Header();
* @Brief		     : puts buffer size to string header
* @Input         : uint - Size, char * - start pointer
* @Output        : None.
* @Return        : number
*******************************************************************************/
void SCPI_BufferSize_to_Header(char *buff , uint32_t val )
{
	buff[0] = (val/10000000)%10+0x30; 
	buff[1] = (val/1000000)%10+0x30; 
	buff[2] = (val/100000)%10+0x30; 
	buff[3] = (val/10000)%10+0x30; 
	buff[4] = (val/1000)%10+0x30; 
	buff[5] = (val/100)%10+0x30; 
	buff[6] = (val/10)%10+0x30; 
	buff[7] = (val)%10+0x30; 
}


/*!*****************************************************************************
 @brief		Measure parameters of signal by SCPI request
 @details	Измерить параметры сигнала по кадру данных
 @return	
 @note		учитывается и дробная часть числа
 @note		Диапазон смещения 0...1023 уменьшается в 2 раза,
		и его середина становится равной 256.
		К целой части сигнала добавляется 128,
		и его середина перемещается на 256.
 *******************************************************************************/
void SCPI_Measure_Request(uint8_t Source, uint8_t Parameter)	{

	// Указатель на начало массива сигнала      
	uint16_t *pStart = (Source ? SigChan2.Buf16K : SigChan1.Buf16K) + (512 << MemCtrl.SizeNum) * MemCtrl.BufNum;
	
	// Указатель на конец массива сигнала, на 1 больше 
	uint16_t *pEnd = (Source ? SigChan2.Buf16K : SigChan1.Buf16K) + (512 << MemCtrl.SizeNum) * (MemCtrl.BufNum + 1);
	uint16_t* pSig = pStart;
	
	// signal cross-line (zero, 0.9, 0.1, etc ) points;
	uint16_t *T1;
	uint16_t *T2;
	uint16_t *T3;
	
	/* 1. Найти минимальное, максимальное и среднее значения */
  uint32_t	min = 255 << 8;
  uint32_t	max = 0 << 0;
  uint32_t	mean = 0 << 0;
  
  for (uint32_t i = 512 << MemCtrl.SizeNum; i--; ) {
    uint32_t temp = *pSig++;		// прочитать слово, перейти на следующее 
    if (temp < min) min = temp;		// новый минимум
    if (temp > max) max = temp;		// новый максимум
    mean += temp;									// суммировать
  }
  mean >>= 9 + MemCtrl.SizeNum;		// поделить на общее количество

						/*
							Если минимум или максимум - на пределе, результат измерений будет неверным
	
							Для минимума и максимума можно проверять только минимум или максимум
							if ((min >> 8) == 0 || (min >> 8) == 0xFF)
							if ((max >> 8) == 0 || (max >> 8) == 0xFF)
						*/

  if ((min >> 8) != 0 && (max >> 8) != 0xFF)	{		// Измерения напряжения будут достоверными  
 
/* 2. Вычислить минимум, максимум, размах или среднее, если нужно */

		if ((MAX == (Parameter)) || (MIN == (Parameter)) || (VAMP == (Parameter)) || (VAV == (Parameter)) )	{
			//pSig = pStart;
			uint32_t spos = Source ? Chan2Ctrl.SigPos : Chan1Ctrl.SigPos;

			
			switch (Parameter) {
			case MIN:
				SCPI_Measure_to_Message((min >> 8) + 128 - (spos >> 1), VOLTAGE);
				//Num2Volt_conv((min >> 8) + 128 - (spos >> 1), volt, MeasTxt);
				break;
			case MAX:
				SCPI_Measure_to_Message((max >> 8) + 128 - (spos >> 1), VOLTAGE);
				//Num2Volt_conv((max >> 8) + 128 - (spos >> 1), volt, MeasTxt);
				break;
			case VAMP:
				SCPI_Measure_to_Message((max - min) >> 8, VOLTAGE);
				//vampNum2Volt_conv((max - min) >> 8, volt, MeasTxt);
				break;
			case VAV:
				SCPI_Measure_to_Message((mean >> 8) + 128 - (spos >> 1), VOLTAGE);
				//Num2Volt_conv((mean >> 8) + 128 - (spos >> 1), volt, MeasTxt);
				break;  
			}
		}
/* 3. Найти длительности низкого и высокого уровней, период и частоту, если нужно */

		if ((((PWIDTH == Parameter)||(NWIDTH ==Parameter)||(FREQ ==Parameter)||(PERIOD ==Parameter))) && (((max - min) >> 8) > 8))	{
			uint32_t mid = (max + min) / 2;		// Найти середину
				
			pSig = pStart;										// Указатель сигнала установить в начало 
			
			bool stlo = false;								// сигнал с начале кадра ниже порога
			bool find = false;								// переход найден
		 
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
				} else {				// искать срез
					while (pSig < pEnd) {
						if (*pSig < mid) {
							find = true;
							break;
						}
						pSig++;
					}
				}
				// Если первый переход найден, продолжить...
				if (find) {
					// Запомнить время первого перехода
					T1 = pSig;
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
					} else {				// искать фронт
						while (pSig < pEnd) {
							if (*pSig > mid) {
								find = true;
								break;
							}
							pSig++;
						}
					}
					// Если второй переход найден, продолжить...
					if (find) {
						// Запомнить время второго перехода
						T2 = pSig;
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
						} else {			// искать срез
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
							T3 = pSig;

							switch (Parameter) {
							case PERIOD:
								SCPI_Measure_to_Message(T3 - T1,TIME);
								break;
							case FREQ:
								SCPI_Measure_to_Message(T3 - T1,FREQENCY);
								break;
							case NWIDTH:
								SCPI_Measure_to_Message(stlo ? (T3 - T2) : (T2 - T1),TIME);
								break;
							case PWIDTH:
								SCPI_Measure_to_Message(stlo ? (T2 - T1) : (T3 - T2),TIME);
								break;
							}

						}		// find T3
					}			// find T2
				}				// find T1
			}					// find signal
		}					// meas Period, Frequense, Width

/* 4. Найти длительность среза */

		if ((FALLTIME == (Parameter)) && ((max - min) >> 8) > 8)	{
			pSig = pStart;										// Указатель сигнала установить в начало 

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
					T1 = pSig;
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
						T2 = pSig;
						// Num_draw(T2 - T1);
						// Вывести длительность среза
						if ((FALLTIME == (Parameter))) SCPI_Measure_to_Message(T2 - T1, TIME);//Num2Time_conv(T2 - T1, MeasTxt);
					}		// find T2
				}			// find T1
			}				// find signal
		}					// meas Tfa


/* 5. Найти длительность фронта */
		if ((RISETIME == (Parameter)) && ((max - min) >> 8) > 8)	{
			pSig = pStart;										// Указатель сигнала установить в начало 
			// Пороги 0.1, 0.9
			uint16_t trlo = min + (max - min) / 10;
			uint16_t trhi = max - (max - min) / 10;

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
					T1 = pSig;
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
						T2 = pSig;
						// Num_draw(T2 - T1);

						// Вывести длительность фронта
						if (RISETIME == (Parameter))	SCPI_Measure_to_Message(T2 - T1, TIME);//Num2Time_conv(T2 - T1, MeasTxt);
					}			// find T2
				}				// find T1
			}					// find signal
		}						// meas Tfa
	}							// Сигнал в пределах АЦП

/* 6. Отобразить результаты измерений */
//	return MeasTxt;
}
/*******************************************************************************
* @Name				   : SCPI_Button_Push_Emulation();
* @Brief		     : writes data to PanCtrl.RcBuf & sends signal for processing it;
* @Input         : values of buttons, 
* @Output        : None.
* @Return        : number
*******************************************************************************/
extern osThreadId	Panel_id;
void SCPI_Button_Push_Emulation(int8_t KeyNum, int8_t KeyCnt)	{
	osMutexRelease(MutDpy_id);
	
	PanCtrl.RcBuf[0] = KeyNum;
	PanCtrl.RcBuf[1] = KeyCnt;
	osSignalSet(Panel_id, 1);
	
	osDelay(50);
	
	PanCtrl.RcBuf[0] = KeyNum;
	PanCtrl.RcBuf[1] = - KeyCnt;
	osSignalSet(Panel_id, 1);
	
	osMutexWait(MutDpy_id, osWaitForever);
}

/*******************************************************************************
* @Name				   : SCPI_Encoder_Move_Emulation();
* @Brief		     : writes data to PanCtrl.RcBuf & sends signal for processing it;
* @Input         : values of buttons, 
* @Output        : None.
* @Return        : number
*******************************************************************************/
void SCPI_Encoder_Move_Emulation(int8_t KeyNum, int8_t KeyCnt)	{
	osMutexRelease(MutDpy_id);
	
	PanCtrl.RcBuf[0] = KeyNum;
	PanCtrl.RcBuf[1] = KeyCnt;
	osSignalSet(Panel_id, 1);
	osDelay(25);	
	
	osMutexWait(MutDpy_id, osWaitForever);
}

/*******************************************************************************
* @Name				   : SCPI_Erroe_Handler();
* @Brief		     : temporary version of error handler
* @Input         : SCPI_Errors_TypeDef ErrCode
* @Output        : None.
* @Return        : None.
*******************************************************************************/
void SCPI_Error_Handler(SCPI_Errors_TypeDef ErrCode)	{
	uint32_t i;
	char ErrString[20];
	for(i = 0;ErrorList[i].message;i++)	{
		if(ErrorList[i].errorcode == ErrCode)	{ 
			strcpy(ErrString, ErrorList[i].message);
			break;
		} 
	}
	
	if(!ErrorList[i].message)	{
		strcpy(ErrString, ErrorList[0].message);
	}
	
	SCPI_Message((uint8_t*)ErrString, NULL);
	//for(;;);	/* inf loop	*/
}




		/////////////////////////////////////////////////////
		//						ACTUATION	CALLBACKS									 //
		/////////////////////////////////////////////////////
	  // edit here for adapting osciloscope reactions 	 //



/*******************************************************************************
* @Name 		 : SCPI_IDN_callback();
* @Brief     : perfoms IDN answer
*******************************************************************************/
void SCPI_IDN_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	
	char IDN_Message[80];
	char *IDN_Message_Pointer = IDN_Message;
	char IDN_Manufacture[] 	= USBD_MANUFACTURER_STRING;
	char IDN_Product[] 			= USBD_PRODUCT_FS_STRING;
	char IDN_Serial[]			  = USBD_SERIALNUMBER_FS_STRING;
	char IDN_Software[]     = "1.0";
		
	uint32_t i;
		
	for(i = 0; IDN_Manufacture[i]; i++)	*IDN_Message_Pointer++ = IDN_Manufacture[i] ;
	*IDN_Message_Pointer++ = ',';

	for(i = 0; IDN_Product[i]; i++)	*IDN_Message_Pointer++ = IDN_Product[i];
	*IDN_Message_Pointer++ = ',';
	
	for(i = 0; IDN_Serial[i]; i++)	*IDN_Message_Pointer++ = IDN_Serial[i];
	*IDN_Message_Pointer++ = ',';


	for(i = 0; IDN_Software[i]; i++) *IDN_Message_Pointer++ = IDN_Software[i];
	*IDN_Message_Pointer++ = ';';
	*IDN_Message_Pointer = '\n';

	SCPI_Message((uint8_t*) IDN_Message, NULL);
		
}
/*******************************************************************************
* @Name 		 : SCPI_Chn1_callback();
* @Brief     : perfoms Channel one controlls
*******************************************************************************/
void SCPI_Chn1_callback(SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	
	switch(command)	{
		// MODE selectors
		case MODE:	{
			switch(parameter.selector)	{
				case	cmON:	{
					Chan1Ctrl.On = 1;
					Chan1Mode_sett();
					Chan1St_disp();								// отобразить в статусе
					PosChan1_draw(Chan1Ctrl.On);	// отобразить маркер
					CursMeasSt_disp();         
					SetCursSt_disp();    
					break;
				}
				case	cmOFF:	{
					Chan1Ctrl.On = 0;
					Chan1Mode_sett();
					Chan1St_disp();								// отобразить в статусе
					PosChan1_draw(Chan1Ctrl.On);	// отобразить маркер
					CursMeasSt_disp();         
					SetCursSt_disp();    
					break;
				}
				case	INV:	{
					Chan1Ctrl.On = 1;
					Chan1Ctrl.Inv = 1;
					break;
				}
				case	NORM:	{
					Chan1Ctrl.On = 1;
					Chan1Ctrl.Inv = 0;
					Chan1Mode_sett();
					Chan1St_disp();								// отобразить в статусе
					PosChan1_draw(Chan1Ctrl.On);	// отобразить маркер
					break;
				}
				case	QUERY:	{
				SCPI_Error_Handler(NOT_SUPPORTED_ERROR); 	// Ответить о состояни канала
				}
				default:	{
				SCPI_Error_Handler(PARAMETER_ERROR);  // err wrong parameter
				}
			}
		break;
		}
		
		
		// SET RANGE	
		case RANGE:	{			
			if (!Chan1Ctrl.On) break;

			int8_t temp  = SCPI_RangeSelector(parameter.value, VoltDivValues, ProbeCoefTable[Chan1Ctrl.Prb]);
		  if (temp < VOLTBEG) temp = VOLTBEG;
			if (temp > VOLTEND) temp = VOLTEND;
			
			Chan1Ctrl.VoltNum = temp;
			Chan1Cplg_sett();
			
			FpPos1_hand(0);	
			Chan1St_disp();
			FrameCnt = 0;
			
			break;
		}
		
		
		// SET OFFSET	
		case OFFSET:	{
			
			SigChan1_draw(0);					// clear signal image (old DispPos)
			PosChan1_draw(false);			// clear pos marker
			TrigLev_draw(false);
			
			int16_t NewPosition = 512 + (int16_t)((parameter.value * 50)/(ProbeCoefTable[Chan1Ctrl.Prb] * VoltDivValues[Chan1Ctrl.VoltNum] ));
			
			if (NewPosition < 0) NewPosition = 0;				// min limit (0), 
			if (NewPosition > 1023) NewPosition = 1023;	// max limit (1023), 
      Chan1Ctrl.SigPos =  NewPosition;			
      Chan1Ctrl.DispPos = NewPosition;;
						
			uint16_t pos = Chan1Ctrl.SigPos;
			if (Chan1Ctrl.VoltNum == VOLTEND) {	// 2mV
				if (Chan1Ctrl.Cplg == 2)	pos += Bal1Ctrl.Ofs2DC;
				if (Chan1Ctrl.Cplg == 1)	pos += Bal1Ctrl.Ofs2AC;
				if (Chan1Ctrl.Cplg == 0)	pos += Bal1Ctrl.Ofs2GND;
			}
			if (Chan1Ctrl.VoltNum % 3 == 0) {	// 5mV
				if (Chan1Ctrl.Cplg == 2)	pos += Bal1Ctrl.Ofs5DC;
				if (Chan1Ctrl.Cplg == 1)	pos += Bal1Ctrl.Ofs5AC;
				if (Chan1Ctrl.Cplg == 0)	pos += Bal1Ctrl.Ofs5GND;
			}
			if (Chan1Ctrl.VoltNum % 3 == 2) {	// 10mV
				if (Chan1Ctrl.Cplg == 2)	pos += Bal1Ctrl.Ofs10DC;
				if (Chan1Ctrl.Cplg == 1)	pos += Bal1Ctrl.Ofs10AC;
				if (Chan1Ctrl.Cplg == 0)	pos += Bal1Ctrl.Ofs5GND;
			}
			if (Chan1Ctrl.VoltNum % 3 == 1) {	// 20mV
				if (Chan1Ctrl.Cplg == 2)	pos += Bal1Ctrl.Ofs20DC;
				if (Chan1Ctrl.Cplg == 1)	pos += Bal1Ctrl.Ofs20AC;
				if (Chan1Ctrl.Cplg == 0)	pos += Bal1Ctrl.Ofs5GND;
			}
			
			ANADAC(DAC_CHAN1, pos << 2);
		
			SigChan1_draw(3);			// draw signal image (new DispPos)
			PosChan1_draw(true);			// draw pos marker
			TrigLev_draw(true);
				
			break;
		}
		
		
		// SET COUPLING
		case COUPLING:	{
			switch(parameter.selector)	{
				case DC	:	{
					Chan1Ctrl.Cplg = 2;
					Chan1Cplg_sett();
					Chan1St_disp();
					break;
				}
				case AC :	{
					Chan1Ctrl.Cplg = 1;
					Chan1Cplg_sett();
					Chan1St_disp();
					break;
				}
				case GND :{
					Chan1Ctrl.Cplg = 0;
					Chan1Cplg_sett();
					Chan1St_disp();	
					break;
				}
				default:	{
				SCPI_Error_Handler(PARAMETER_ERROR);			//	for(;;); // err wrong parameter
				}
			}
		break;	
		}
		
		// SET BANDWIDTH limitation 
		case BAND:	{
			switch(parameter.selector)	{
				case	cmON:	{
				  Chan1Ctrl.BWLim = 1;
					Chan1BWLim_sett();
					Chan1St_disp();	
					break;
				} 
				case	cmOFF:	{
					Chan1Ctrl.BWLim = 0;
					Chan1BWLim_sett();
					Chan1St_disp();	
					break;
				}
				default:	{
				SCPI_Error_Handler(PARAMETER_ERROR);
				//for(;;); // err wrong parameter
				}
			}
			break;
		}

		
		case PROBE:	{
			SCPI_Error_Handler(NOT_SUPPORTED_ERROR);
			//for(;;);		// set PROBE value
			break;
		}
		
		// UNRECOGNIZED COMMAND HANDLING
		default:	{
			SCPI_Error_Handler(SYNTAX_ERROR);
			//for(;;); // err	wrong command
		}
	}
		
}

/*******************************************************************************
* @Name 		 : SCPI_Chn2_callback();
* @Brief     : perfoms Channel two controlls
*******************************************************************************/
void SCPI_Chn2_callback(SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	
	switch(command)	{
		// MODE selectors
		case MODE:	{
			switch(parameter.selector)	{
				case	cmON:	{
					Chan2Ctrl.On = 1;
					Chan2Mode_sett();
					Chan2St_disp();								// отобразить в статусе
					PosChan2_draw(Chan2Ctrl.On);	// отобразить маркер
					break;
				}
				case	cmOFF:	{
					Chan2Ctrl.On = 0;
					Chan2Mode_sett();
					Chan2St_disp();								// отобразить в статусе
					PosChan2_draw(Chan2Ctrl.On);	// отобразить маркер
					break;
				}
				case	INV:	{
					Chan2Ctrl.On = 1;
					Chan2Ctrl.Inv = 1;
					break;
				}
				case	NORM:	{
					Chan2Ctrl.On = 1;
					Chan2Ctrl.Inv = 0;
					Chan2Mode_sett();
					Chan2St_disp();								// отобразить в статусе
					PosChan2_draw(Chan2Ctrl.On);	// отобразить маркер
					break;
				}
				case	QUERY:	{
					SCPI_Error_Handler(NOT_SUPPORTED_ERROR);
					//for(;;);	// Ответить о состояни канала
				}
				default:	{
					SCPI_Error_Handler(PARAMETER_ERROR);
					//for(;;); // err wrong parameter
				}
			}
			break;
		}
		
		
		// SET RANGE	
		case RANGE:	{
			if (!Chan2Ctrl.On) break;

			int8_t temp  = SCPI_RangeSelector(parameter.value, VoltDivValues, ProbeCoefTable[Chan2Ctrl.Prb]);
		  if (temp < VOLTBEG) temp = VOLTBEG;
			if (temp > VOLTEND) temp = VOLTEND;
			
			Chan2Ctrl.VoltNum = temp;
			Chan2Cplg_sett();
			
			FpPos2_hand(0);	
			Chan2St_disp();
			FrameCnt = 0;
			
			break;
		}
		
		
		// SET OFFSET	
		case OFFSET:	{
			SigChan2_draw(0);					// clear signal image (old DispPos)
			PosChan2_draw(false);			// clear pos marker
			TrigLev_draw(false);
			
			int16_t NewPosition = 512 + (int16_t)((parameter.value * 50)/(ProbeCoefTable[Chan2Ctrl.Prb] * VoltDivValues[Chan2Ctrl.VoltNum] ));
			
			if (NewPosition < 0) NewPosition = 0;				// min limit (0), 
			if (NewPosition > 1023) NewPosition = 1023;	// max limit (1023), 
      Chan2Ctrl.SigPos =  NewPosition;			
      Chan2Ctrl.DispPos = NewPosition;;
						
			uint16_t pos = Chan2Ctrl.SigPos;
			if (Chan2Ctrl.VoltNum == VOLTEND) {	// 2mV
				if (Chan2Ctrl.Cplg == 2)	pos += Bal2Ctrl.Ofs2DC;
				if (Chan2Ctrl.Cplg == 1)	pos += Bal2Ctrl.Ofs2AC;
				if (Chan2Ctrl.Cplg == 0)	pos += Bal2Ctrl.Ofs2GND;
			}
			if (Chan2Ctrl.VoltNum % 3 == 0) {	// 5mV
				if (Chan2Ctrl.Cplg == 2)	pos += Bal2Ctrl.Ofs5DC;
				if (Chan2Ctrl.Cplg == 1)	pos += Bal2Ctrl.Ofs5AC;
				if (Chan2Ctrl.Cplg == 0)	pos += Bal2Ctrl.Ofs5GND;
			}
			if (Chan2Ctrl.VoltNum % 3 == 2) {	// 10mV
				if (Chan2Ctrl.Cplg == 2)	pos += Bal2Ctrl.Ofs10DC;
				if (Chan2Ctrl.Cplg == 1)	pos += Bal2Ctrl.Ofs10AC;
				if (Chan2Ctrl.Cplg == 0)	pos += Bal2Ctrl.Ofs5GND;
			}
			if (Chan2Ctrl.VoltNum % 3 == 1) {	// 20mV
				if (Chan2Ctrl.Cplg == 2)	pos += Bal2Ctrl.Ofs20DC;
				if (Chan2Ctrl.Cplg == 1)	pos += Bal2Ctrl.Ofs20AC;
				if (Chan2Ctrl.Cplg == 0)	pos += Bal2Ctrl.Ofs5GND;
			}
			
			ANADAC(DAC_CHAN2, pos << 2);
		
			SigChan2_draw(3);			// draw signal image (new DispPos)
			PosChan2_draw(true);			// draw pos marker
			TrigLev_draw(true);
				

			break;
		}
				
		// SET COUPLING
		case COUPLING:	{
			switch(parameter.selector)	{
				case DC	:	{
					Chan2Ctrl.Cplg = 2;
					Chan2Cplg_sett();
					Chan2St_disp();	
					break;
				}
				case AC :	{
					Chan2Ctrl.Cplg = 1;
					Chan2Cplg_sett();
					Chan2St_disp();	
					break;
				}
				case GND :{
					Chan2Ctrl.Cplg = 0;
					Chan2Cplg_sett();
					Chan2St_disp();	
					break;
				}
				default:	{
					SCPI_Error_Handler(PARAMETER_ERROR);
				  //for(;;); // err wrong parameter
				}
			}
			break;	
		}
		
		// SET BANDWIDTH limitation 
		case BAND:	{
			switch(parameter.selector)	{
				case	cmON:	{
				  Chan2Ctrl.BWLim = 1;
					Chan2BWLim_sett();
					Chan2St_disp();	
				break;
				} 
				case	cmOFF:	{
					Chan2Ctrl.BWLim = 0;
					Chan2BWLim_sett();
					Chan2St_disp();	
				break;
				}
				default:	{
					SCPI_Error_Handler(PARAMETER_ERROR);
					for(;;); // err wrong parameter
				}
			}
			break;
		}
		
		
		case PROBE:	{
			SCPI_Error_Handler(NOT_SUPPORTED_ERROR);
			//for(;;);		// set PROBE value
			break;
		}
		
		// UNRECOGNIZED COMMAND HANDLING
		default:	{
			SCPI_Error_Handler(SYNTAX_ERROR);
			//for(;;); // err	wrong command
		}
	}
		
}

/*******************************************************************************
* @Name 		 : SCPI_Chn2_callback();
* @Brief    	 : perfoms RESET function callback
*******************************************************************************/
void SCPI_RES_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	
	NVIC_SystemReset();
	// MEGA-END; NOTHING MORE EXISTS, NEW UNIVERSE STARTS;
}

/*******************************************************************************
* Function Name  : SCPI_RUN_callback();
* Description    : perfoms RUN command callback
*******************************************************************************/
void SCPI_STOP_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	
	  SweepCtrl.Stop = 1;
    StSp_disp();			// display
 }
/*******************************************************************************
* Function Name  : SCPI_RUN_callback();
* Description    : perfoms RUN command callback
*******************************************************************************/
void SCPI_RUN_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	
	SweepCtrl.Stop = 0;
  StSp_disp();			// display
  Sweep_init();
  if (SweepCtrl.Roll) DpyWork_clear(MASK_MATSIG);
}

/*******************************************************************************
* @Name 		 : SCPI_TRAN_callback();
* @Brief     : perfoms TRANSFER functionality command
*******************************************************************************/
#define TX_FRAME_BUFF_SIZE 16384																											// put to SCPI_defines.h
static uint8_t TxFrameBuffer[TX_FRAME_BUFF_SIZE+10+1] __attribute((section("fft")));  // put to SCPI_defines.h

void SCPI_TRAN_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	// only 8-bit transfer yet, aytodetect buffer size, chan_select 
	uint32_t BuffSize = (512 << MemCtrl.SizeNum);
	uint32_t i;
	
	uint16_t * pWaveForm;
	char Header[] = "#800000000";
		
	if(!MeasCtrl.Sour) {
		pWaveForm = SigChan1.Buf16K + BuffSize * MemCtrl.BufNum;
	}	else	{
		pWaveForm = SigChan2.Buf16K + BuffSize * MemCtrl.BufNum;
	}
	
	SCPI_BufferSize_to_Header(&Header[2], BuffSize);

	for(i = 0; i<10;i++)	{
		TxFrameBuffer[i] = Header[i];
	}
	for(i = 10; i < (BuffSize + 10); i++)	{
		TxFrameBuffer[i] = (pWaveForm[i-10]>>8);
	}
	
	TxFrameBuffer[BuffSize+10] = '\n';
	
	SCPI_Message(TxFrameBuffer, BuffSize+10+1);			//	send all
	
}

/*******************************************************************************
* @Name  		: SCPI_AUTO_callback();
* @Brief		: perfoms AUTOSCALE command
*******************************************************************************/
extern void FpAutoset_hand(int32_t Cnt);
void SCPI_AUTO_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	
	GenCtrl.Autoset = 0;
	FpAutoset_hand(1);
	
}

/*******************************************************************************
* @Name 		 : SCPI_Tbase_callback();
* @Brief     : perfoms SWEEP controlls
*******************************************************************************/
extern void SetPret_sett(void);
void SCPI_Tbase_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	switch(command)	{
		case RANGE:	{
			
						int32_t temp = SCPI_RangeSelector(parameter.value, TimeDivValues, 1);
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
						DpyWork_clear(MASK_MATSIG);
						Sweep_init();
						FrameCnt = 0;			// Очистить счетчик кадров с неизменным режимом
						if (CursCtrl.On) SetCursSt_disp();
						break;
		}
		case DELAY:	{
			SCPI_Error_Handler(NOT_SUPPORTED_ERROR);
			//for(;;);// set delay
			break;
		}
		case MODE:	{
			switch (parameter.selector)	{
				case XYSWEEP:	{
					SweepCtrl.XY = 1;
					SweepCtrl.Roll = 0;
					break;
				}
				case NORM:	{
					SweepCtrl.XY = 0;
					SweepCtrl.Roll = 0;
					SweepRoll_sett();
					SweepSt_disp();
					Sweep_init();
					DpyWork_clear(MASK_MATSIG);
					break;
				}
				case GRAPH:	{
					SweepCtrl.Roll = 1;
					SweepCtrl.Roll = 0;
					SweepRoll_sett();
					SweepSt_disp();
					Sweep_init();
					DpyWork_clear(MASK_MATSIG);
					break;
				}
				default:	{
					SCPI_Error_Handler(PARAMETER_ERROR);
					//for(;;);
					break;
				}
			}
		break;
		}
		default:	{
			SCPI_Error_Handler(SYNTAX_ERROR);
		}
	}

}

/*******************************************************************************
* @Name 		 : SCPI_Trig_callback();
* @Brief     : perfoms TRIGGER controlls
*******************************************************************************/
void SCPI_Trig_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	switch(command)	{
		case SOURCE:	{
			switch(parameter.selector)	{
				case LINE:	{
					TrigCtrl.Sour = 0;
					TrigSour_sett();
					TrigSt_disp();
					break;
				}
				case CH1:	{
					TrigCtrl.Sour = 1;
					TrigSour_sett();
					TrigSt_disp();
					break;
				}
				case CH2:	{
					TrigCtrl.Sour = 2;
					TrigSour_sett();
					TrigSt_disp();
					break;
				}
				case EXT:	{
					TrigCtrl.Sour = 3;
					TrigSour_sett();
					TrigSt_disp();
					break;
				}
				default:	{
					SCPI_Error_Handler(PARAMETER_ERROR);
					break;
				}
			}
		break;
		}
		case COUPLING:	{
			switch(parameter.selector)	{
				case LF:	{
					TrigCtrl.Cplg = 0;
				  TrigCplg_sett();
					TrigSt_disp();
					break;
				}
				case DC:	{
					TrigCtrl.Cplg = 1;
				  TrigCplg_sett();
					TrigSt_disp();
					break;
				}
				case AC:	{
					TrigCtrl.Cplg = 2;
				  TrigCplg_sett();
					TrigSt_disp();
					break;
				}
				case HF:	{
					TrigCtrl.Cplg = 3;
				  TrigCplg_sett();
					TrigSt_disp();
					break;
				}
				default:	{
					SCPI_Error_Handler(PARAMETER_ERROR);
				}
			}
		break;
		}
		case SLOPE:	{
			switch(parameter.selector)	{
				case POSITIVE:	{
					TrigCtrl.Rise = 0;
					TrigSlp_sett();
					TrigSt_disp();
					break;
				}
				case NEGATIVE:	{
					TrigCtrl.Rise = 1;
					TrigSlp_sett();
					TrigSt_disp();
					break;
				}
				default:{
					SCPI_Error_Handler(PARAMETER_ERROR);
				}
			}
		break;
		}
		case MODE:	{
			switch(parameter.selector)	{
				case AUTO:	{ 
					SweepCtrl.Mode = 0;
					SweepMode_sett();
					SweepSt_disp();
					SweepCtrl.Stop = 0;
					PL->STAT = PL_START_ACQUST;
					StSp_disp();					
					break;
				}
				case NORMAL:	{ 
					SweepCtrl.Mode = 1;
					SweepMode_sett();
					SweepSt_disp();
					SweepCtrl.Stop = 0;
					PL->STAT = PL_START_ACQUST;
					StSp_disp();				
					break;
				}
				case	SINGLE:	{ 
					SweepCtrl.Mode = 2;
					SweepMode_sett();
					SweepSt_disp();
					SweepCtrl.Stop = 1;
					StSp_disp();
					break;
				}
				default:	{
					SCPI_Error_Handler(PARAMETER_ERROR);
					break;
				}
			}
		break;
		}
		case LEVEL:	{
			
			TrigLev_draw(false);
			//512 + (int16_t)((parameter.value * 50)/(ProbeCoefTable[Chan1Ctrl.Prb] * VoltDivValues[Chan1Ctrl.VoltNum] ));
			//int16_t tmpLevel = TrigCtrl.Level + Cnt;
			int16_t tmpLevel = 512 + (int16_t)((parameter.value * 50)/(ProbeCoefTable[Chan1Ctrl.Prb] * VoltDivValues[Chan1Ctrl.VoltNum] ));
			if (tmpLevel < 0) tmpLevel = 0;
			if (tmpLevel > 1023) tmpLevel = 1023;
			TrigCtrl.Level = tmpLevel;
			TrigLev_draw(true);

			ANADAC(DAC_LEVEL, TrigCtrl.Level << 2);
						
			break;
			// set level
		}
		case FILTER: {
			switch(parameter.selector)	{
				case cmON:	{
					TrigCtrl.NRej = 1;
					TrigNRej_sett();
					break;
				}
				case cmOFF:	{
					TrigCtrl.NRej = 0;
					TrigNRej_sett();
					break;
				}
				default:	{
				SCPI_Error_Handler(PARAMETER_ERROR);
				}
			}
		}
		default:	{
		SCPI_Error_Handler(SYNTAX_ERROR);
		}
	}
}

/*******************************************************************************
* @Name 		 : SCPI_Measure_callback();
* @Brief     : perfoms MEASURE controlls
*******************************************************************************/
void SCPI_Measure_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	switch(command)	{
		// controll display measurements
		case MODE:	{
			switch(parameter.selector)	{
			case cmON:	{
				
				MeasCtrl.On = 1;
				// Выключить и погасить курсоры
				CursCtrl.On = 0;
				Curs_draw(0);
				// Стереть функции управления курсорами от энкодера
				if (SetNum == SET_CURS1 || SetNum == SET_CURS2 || SetNum == SET_CURSTR) SetNum = SET_VOID;
				
				// apply settings
				SigPar_meas();
				MeasPar1St_disp();
				MeasPar2St_disp();
				break;
			}
			case cmOFF:	{
				
				MeasCtrl.On = 0;
				SigPar_meas();
				MeasPar1St_disp();
				MeasPar2St_disp();
				break;			
			}
			default:	{
				SCPI_Error_Handler(PARAMETER_ERROR);
			}
		}
		break;
		}
		// choose source (both for display & interface measurement)
		case SOURCE:	{
			switch(parameter.selector)	{
			case CH1:	{
				MeasCtrl.Sour = 0;
				SigPar_meas();
				break;
			}
			case CH2:	{}
				MeasCtrl.Sour = 1;
				SigPar_meas();
				break;
			default:	{
				SCPI_Error_Handler(PARAMETER_ERROR);	
			}
			}
	  MeasPar1St_disp();
		MeasPar2St_disp();
		break;
		}
		// choose parameters for display measurement
		case PARAMETER1:	{
			switch(parameter.selector)	{
			case SELECT_PERIOD:	{
				MeasCtrl.Par1 = 0;
				SigPar_meas();
				break;
			}
			case SELECT_FREQ:	{
				MeasCtrl.Par1 = 1<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_NWIDTH:	{
				MeasCtrl.Par1 = 2<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_PWIDTH:	{
				MeasCtrl.Par1 = 3<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_FALLTIME:	{
				MeasCtrl.Par1 = 4<<2;
				SigPar_meas();
				break;
			}
			case SELECT_RISETIME:	{
				MeasCtrl.Par1 = 5<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_MIN:	{
				MeasCtrl.Par1 = 6<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_MAX:	{
				MeasCtrl.Par1 = 7<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_VAMP:	{
				MeasCtrl.Par1 = 8<<2;
				SigPar_meas();
				break;
			}
			case SELECT_VAV:	{
				MeasCtrl.Par1 = 9<<2;
				SigPar_meas();				
				break;
			}			
			default:	{
				SCPI_Error_Handler(PARAMETER_ERROR);
			}
			}
	  MeasPar1St_disp();
		MeasPar2St_disp();
		break;
		}
		case PARAMETER2:	{
			switch(parameter.selector)	{
			case SELECT_PERIOD:	{
				MeasCtrl.Par2 = 0<<2;
				SigPar_meas();
				break;
			}
			case SELECT_FREQ:	{
				MeasCtrl.Par2 = 1<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_NWIDTH:	{
				MeasCtrl.Par2 = 2<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_PWIDTH:	{
				MeasCtrl.Par2 = 3<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_FALLTIME:	{
				MeasCtrl.Par2 = 4<<2;
				SigPar_meas();
				break;
			}
			case SELECT_RISETIME:	{
				MeasCtrl.Par2 = 5<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_MIN:	{
				MeasCtrl.Par2 = 6<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_MAX:	{
				MeasCtrl.Par2 = 7<<2;
				SigPar_meas();				
				break;
			}
			case SELECT_VAMP:	{
				MeasCtrl.Par2 = 8<<2;
				SigPar_meas();
				break;
			}
			case SELECT_VAV:	{
				MeasCtrl.Par2 = 9<<2;
				SigPar_meas();				
				break;
			}
			default:	{
				SCPI_Error_Handler(PARAMETER_ERROR);
			}
			}
	  MeasPar1St_disp();
		MeasPar2St_disp();
		break;	
		}
		// single measurements
		default:	{
			if((QUERY == parameter.selector)&&((PERIOD == command)||(FREQ == command)||(NWIDTH == command)||(PWIDTH == command)||(FALLTIME == command)||(RISETIME == command)||(MAX == command)||(MIN == command)||(VAMP == command)||(VAV == command)))	{ 
				SCPI_Measure_Request(MeasCtrl.Sour, (command));
			} else	{
				SCPI_Error_Handler(SYNTAX_ERROR);
			}
		}	
	}
}

/*******************************************************************************
* @Name 		 : SCPI_Disp_callback();
* @Brief   	 : perfoms Display controlls
*******************************************************************************/
void SCPI_Disp_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	switch(command)	{
		case REFERENCE:	{
			switch(parameter.selector)	{
				case DOT:	{
					SigChan1_draw(0);
					SigChan2_draw(0);
					DispCtrl.Vect = 0;
					SigChans_redraw();
					break;
				}
				case VECTOR:	{
					SigChan1_draw(0);
					SigChan2_draw(0);
					DispCtrl.Vect = 1;
					SigChans_redraw();
					break;
				}
				default:	{
					SCPI_Error_Handler(PARAMETER_ERROR);
				}
			}
			break;
		}
		case SCALE:	{
			switch(parameter.selector)	{
				case FRAME:	{
					DispCtrl.Scale = 0;
					DpyScale_draw();  
					break;
				}
				case CROSS:	{
					DispCtrl.Scale = 1;
					DpyScale_draw();  
					break;
				}
				case GRID:	{
					DispCtrl.Scale = 2;
					DpyScale_draw();  
					break;
				}			
				case ALL:	{
					DispCtrl.Scale = 3;
					DpyScale_draw();  
					break;
				}
				default:	{
					SCPI_Error_Handler(PARAMETER_ERROR);
				}
			}
		break;
		}
		case CLEAR:	{
			DpyWork_clear(MASK_MATSIG);
			break;
		}
		default:	{SCPI_Error_Handler(SYNTAX_ERROR);}
	}
}

/*******************************************************************************
* @Name 		 : SCPI_Key_callback();
* @Brief   	 : perfoms Display controlls
*******************************************************************************/
void SCPI_Key_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter)	{
	switch(command)	{
		case LOCKKEYS:
			switch(parameter.selector)	{
				case cmON:		{
					//NVIC_ClearPendingIRQ(USART3_IRQn);	// сбросить прерывания от панели 
					NVIC_DisableIRQ(USART3_IRQn);  	// разрешить прерывания от панели					
					break;
				}
				case cmOFF:	{
					NVIC_ClearPendingIRQ(USART3_IRQn);	// сбросить прерывания от панели 
					osDelay(25);
					NVIC_EnableIRQ(USART3_IRQn);  	// разрешить прерывания от панели				
					break;
				}
				default:	{
				SCPI_Error_Handler(PARAMETER_ERROR);
				break;
				}
			}
		
		case CH1_SCALE_INC:	{
			SCPI_Encoder_Move_Emulation(0, 1);
			break;
		}
		case CH1_SCALE_DEC:	{
			SCPI_Encoder_Move_Emulation(0, -1);
			break;
		}
		case CH2_SCALE_INC:	{
			SCPI_Encoder_Move_Emulation(1, 1);
			break;
		}
		case CH2_SCALE_DEC:	{
			SCPI_Encoder_Move_Emulation(1, -1);
			break;
		}
		case TIME_SCALE_INC:	{
			SCPI_Encoder_Move_Emulation(2, 1);
			break;
		}
		case TIME_SCALE_DEC:	{
			SCPI_Encoder_Move_Emulation(2, -1);
			break;
		}
		case FUNCTION_INC:	{
			SCPI_Encoder_Move_Emulation(3, 4);
			break;
		}
		case FUNCTION_DEC:	{
			SCPI_Encoder_Move_Emulation(3, -4);
			break;
		}
		case CH1_POS_INC:	{
			SCPI_Encoder_Move_Emulation(4, 5);
			break;
		}
		case CH1_POS_DEC:	{
			SCPI_Encoder_Move_Emulation(4, -5);
			break;
		}
		case CH2_POS_INC:	{
			SCPI_Encoder_Move_Emulation(5, 5);
			break;
		}
		case CH2_POS_DEC:	{
			SCPI_Encoder_Move_Emulation(5, -5);
			break;
		}
		case TIME_POS_INC:	{
			SCPI_Encoder_Move_Emulation(6, -4);
			break;
		}
		case TIME_POS_DEC:	{
			SCPI_Encoder_Move_Emulation(6, -4);
			break;
		}
		case TRIG_LVL_INC:	{
			SCPI_Encoder_Move_Emulation(7, -4);
			break;
		}
		case TRIG_LVL_DEC:	{
			SCPI_Encoder_Move_Emulation(7, -4);
			break;
		}
		case CHAN1KEY:	{
			SCPI_Button_Push_Emulation(8, 1);
			break;
			}
		case MATHKEY:	{
			SCPI_Button_Push_Emulation(9, 1);
			break;
			}
		case CHAN2KEY:	{
			SCPI_Button_Push_Emulation(10, 1);
			break;
			}
		//FP11
		case SWEEPKEY:	{
			SCPI_Button_Push_Emulation(12, 1);
			break;
			}
		// FP13
		case TRIGGERKEY:	{
			SCPI_Button_Push_Emulation(14, 1);
			break;
			}
		// FP15
		case DISPLAYKEY:	{
			SCPI_Button_Push_Emulation(16, 1);
			break;
			}
		case ACQUIREKEY:	{
			SCPI_Button_Push_Emulation(17, 1);
			break;
			}
		case CURSORKEY:	{
			SCPI_Button_Push_Emulation(18, 1);
			break;
			}
		case MEASUREKEY:	{
			SCPI_Button_Push_Emulation(19, 1);
			break;
			}	
		case MEMORYKEY:	{
			SCPI_Button_Push_Emulation(20, 1);
			break;
			}
		case UTILITYKEY:	{
			SCPI_Button_Push_Emulation(21, 1);
			break;
			}
		case AUTOKEY:	{
			SCPI_Button_Push_Emulation(22, 1);
			break;
			}			
		case RUNKEY:	{
			SCPI_Button_Push_Emulation(23, 1);
			break;
			}
		case MENUTOGGLEKEY:	{
			SCPI_Button_Push_Emulation(24, 1);
			break;
			}
		case F1KEY:	{
			SCPI_Button_Push_Emulation(25, 1);
			break;
			}
		case F2KEY:	{
			SCPI_Button_Push_Emulation(26, 1);
			break;
			}
		case F3KEY:	{
			SCPI_Button_Push_Emulation(27, 1);
			break;
			}
		case F4KEY:	{
			SCPI_Button_Push_Emulation(28, 1);
			break;
			}
		case F5KEY:	{
			SCPI_Button_Push_Emulation(29, 1);
			break;
			}
		// FP 30
		// FP 31
		default:	{
				SCPI_Error_Handler(SYNTAX_ERROR);
				break;
		}

	}
}

