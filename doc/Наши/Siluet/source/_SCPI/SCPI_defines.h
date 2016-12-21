/**
  ******************************************************************************
  * @file    SCPI_defines.h
  * @author  san.sergeich
  * @version V0.1
  * @date    18-September-2013
  * @brief   SCPI parser functions function prototypes, 
	*					 datatypes & pattern defines.
  ******************************************************************************
  * 
  *
  *
  ******************************************************************************
  */
	
	
#ifndef _SCPI_SIMPLE_PARSER_H
#define _SCPI_SIMPLE_PARSER_H
	
	
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>	
	
#ifndef	BOOL_TYPE
#define BOOL_TYPE	
	typedef enum
	{
		FALSE = 0, TRUE  = !FALSE
	}	bool;
#endif	/*	bool	*/	

		/////////////////////////////////////////////////////
		//						SCPI TYPEDEFS												 //
		/////////////////////////////////////////////////////
		//				edit it for adding new commands					 //
	
typedef enum	{
	
		//CHANELS
	MODE,
	RANGE,
	OFFSET,
	COUPLING,
	BAND,
	PROBE,
	
		// Time Parameters

	
		// TRIGGER
	DELAY,
	SOURCE,
	SLOPE,
	LEVEL,
	FILTER,
	
		// Measurement
	PARAMETER1,
	PARAMETER2,
	PERIOD,
	FREQ,
	NWIDTH,
	PWIDTH,
	FALLTIME,
	RISETIME,
	MAX,
	MIN,
	VAMP,
	VAV,
	
		// Display
	REFERENCE,
	SCALE,
	CLEAR,
	
		// Key
	LOCKKEYS,
	RUNKEY,
	AUTOKEY,
	CHAN1KEY,
	CHAN2KEY,
	MATHKEY,
	F1KEY,
	F2KEY,
	F3KEY,
	F4KEY,
	F5KEY,
	MENUTOGGLEKEY,
	CURSORKEY,
	ACQUIREKEY,
	DISPLAYKEY,
	MEMORYKEY,
	UTILITYKEY,
	SWEEPKEY,
	TRIGGERKEY,
	MEASUREKEY,
	CH1_POS_INC,
	CH1_POS_DEC,
	CH2_POS_INC,
	CH2_POS_DEC,
	CH1_SCALE_INC,
	CH1_SCALE_DEC,
	CH2_SCALE_INC,
	CH2_SCALE_DEC,
	TIME_SCALE_INC,
	TIME_SCALE_DEC,
	TIME_POS_INC,
	TIME_POS_DEC,
	TRIG_LVL_INC,
	TRIG_LVL_DEC,
	FUNCTION_INC,
	FUNCTION_DEC
} SCPI_2ndLvlCommands_TypeDef; 

typedef enum	{
	VOLTAGE,
	TIME,
	FREQENCY	
}	Unit_type_TypeDef;


typedef struct	{
	const char *pattern;	
	SCPI_2ndLvlCommands_TypeDef command;
}	SCPI_2ndLvlCommand_TypeDef;

typedef struct	{
	const char *pattern;
	double mult;
	Unit_type_TypeDef type;
}	SCPI_Units_TypeDef;

typedef struct	{
	const char *pattern;
	
}	SCPI_Parameter_TypeDef;

typedef enum	{
	cmON,
	cmOFF,
	NORM,
	INV,
	DC,
	AC,
	GND,
	LIM,
	FULL,
	PRx100,
	PRx10,
	PRx1,
	PR10x,
	CH1,
	CH2,
	EXT,
	CONNECTOR,
	LINE,
	LF,
	HF,
	POSITIVE,
	NEGATIVE,
	AUTO,
	NORMAL,
	SINGLE,
	MATH,
	DOT,
	VECTOR,
	FRAME,
	CROSS,
	GRID,
	ALL,
	QUERY,
	GRAPH,
	XYSWEEP,
	
	//measurement 
	
	SELECT_PERIOD,
	SELECT_FREQ,
	SELECT_NWIDTH,
	SELECT_PWIDTH,
	SELECT_FALLTIME,
	SELECT_RISETIME,
	SELECT_MAX,
	SELECT_MIN,
	SELECT_VAMP,
	SELECT_VAV
	
	
} SCPI_Selectors_TypeDef;

typedef struct {
	const char *pattern;	
	SCPI_Selectors_TypeDef selector;
} SCPI_Selector_TypeDef;


typedef union	{
	double value;
	SCPI_Selectors_TypeDef selector; 
} SCPI_Value_Typedef;

typedef struct	{
	void ( *callback)( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);
	SCPI_2ndLvlCommands_TypeDef command;
	SCPI_Value_Typedef parameter;
}	SCPI_CommandContext_TypeDef;


typedef struct	{
	const char *pattern;	
	void ( *callback)( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);
}	SCPI_RootCommand_TypeDef;

typedef enum	{
	SYNTAX_ERROR,
	PARAMETER_ERROR,
	INTERNAL_ERROR,
	NOT_SUPPORTED_ERROR,
	UNKNOWN_ERROR,
	UNKNOWN_COMMAND_ERROR
}	SCPI_Errors_TypeDef;

typedef struct {
	const char* message;
	SCPI_Errors_TypeDef errorcode;
} SCPI_Error_Typedef;

		/////////////////////////////////////////////////////
		//								ERRORS LIST											 //
		/////////////////////////////////////////////////////

const SCPI_Error_Typedef ErrorList[] = {
	{.message = "UNKNOWN_ERROR\n",			.errorcode = UNKNOWN_ERROR				},
	{.message = "SYNTAX_ERROR\n",				.errorcode = SYNTAX_ERROR					},
	{.message = "PARAMETER_ERROR\n",		.errorcode = PARAMETER_ERROR			},
	{.message = "NOT SUPPORTED YET\n",	.errorcode = NOT_SUPPORTED_ERROR	},
	{.message = "WTF YOU WANT?\n",			.errorcode = UNKNOWN_COMMAND_ERROR},
	{.message = "INTERNAL_ERROR\n",			.errorcode = INTERNAL_ERROR				},
	{NULL }
};


		/////////////////////////////////////////////////////
		//				ACTUATION	CALLBACKS PROTOTYPES					 //
		/////////////////////////////////////////////////////
		//				edit it for adding new commands					 //




void SCPI_RES_callback		( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);
void SCPI_RUN_callback		( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);
void SCPI_TRAN_callback		( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);
void SCPI_AUTO_callback		( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);
void SCPI_STOP_callback		( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);

void SCPI_Chn1_callback		( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);
void SCPI_Chn2_callback		( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);

void SCPI_Tbase_callback	( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);
void SCPI_Trig_callback		( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);
void SCPI_Measure_callback( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);
void SCPI_Disp_callback		( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);

void SCPI_Key_callback    ( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);
void SCPI_IDN_callback    ( SCPI_2ndLvlCommands_TypeDef command, SCPI_Value_Typedef parameter);


		//																								 //
		//																								 //
		/////////////////////////////////////////////////////
		//								PATTERTNS LIST BEGIN						 //
		/////////////////////////////////////////////////////
		//				edit it for adding new commands					 //



		/////////////////////////////////////////////////////
		//								SELECTORS LIST									 //
		/////////////////////////////////////////////////////

const SCPI_Selector_TypeDef SelectorsList[] = {
	{.pattern = "ON", 						.selector = cmON, 	},
	{.pattern = "OFF",						.selector = cmOFF		},
	{.pattern = "NORM",						.selector = NORM	},

	{.pattern = "INV",						.selector = INV		},
	{.pattern = "INVERT",					.selector = INV		},

	{.pattern = "DC",							.selector = DC		},
	{.pattern = "AC",							.selector = AC		},
	{.pattern = "GND",						.selector = GND		},

	{.pattern = "LIM",						.selector = LIM		},
	{.pattern = "LIMITED",				.selector = LIM		},


	{.pattern = "FUL",						.selector = FULL		},
	{.pattern = "FULL",						.selector = FULL		},


	{.pattern = "1/100",					.selector = PRx100	},
	{.pattern = "1/10",						.selector = PRx10		},
	{.pattern = "1/1",						.selector = PRx1		},

	{.pattern = "10x",						.selector = PR10x		},
	{.pattern = "10X",						.selector = PR10x		},

	{.pattern = "CHA",						.selector = CH1		},
	{.pattern = "CH1",						.selector = CH1		},
	{.pattern = "CHB",						.selector = CH2		},
	{.pattern = "CH2",						.selector = CH2		},
	
	{.pattern = "EXT",						.selector = EXT		},
	{.pattern = "EXTERNAl",				.selector = EXT		},
	{.pattern = "LINE",			 			.selector = LINE		},
	{.pattern = "LIN",			 			.selector = LINE		},
	
	{.pattern = "LF",							.selector = LF		},
	{.pattern = "HF",							.selector = HF		},
	
	{.pattern = "POS",						.selector = POSITIVE		},
	{.pattern = "POSITIVE",				.selector = POSITIVE		},
	
	{.pattern = "NEG",						.selector = NEGATIVE		},
	{.pattern = "NEGATIVE",				.selector = NEGATIVE		},
	
	{.pattern = "AUT",						.selector = AUTO		},
	{.pattern = "AUTOMATIC",			.selector = AUTO		},
	
	{.pattern = "NORM",						.selector = NORMAL		},
	{.pattern = "NORMAL",					.selector = NORMAL		},
	
	{.pattern = "SCAN",			  .selector = NORMAL},
	{.pattern = "ROLL",			  .selector = GRAPH},
	{.pattern = "ROL",			  .selector = GRAPH},
	{.pattern = "XYSWEEP",   	.selector = XYSWEEP},
	{.pattern = "XY", 				.selector = XYSWEEP},
	
	
	{.pattern = "SING",						.selector = SINGLE		},
	{.pattern = "SINGLE",					.selector = SINGLE		},
	
	
	{.pattern = "MATH",						.selector = MATH		},
	{.pattern = "DOT",						.selector = DOT		},
	
	{.pattern = "VECTOR",					.selector = VECTOR		},	
	{.pattern = "VECT",						.selector = VECTOR		},
	
	{.pattern = "FRAME",					.selector = FRAME		},
	{.pattern = "FR",							.selector = FRAME		},
	{.pattern = "CROSS",					.selector = CROSS		},
	{.pattern = "CR",							.selector = CROSS		},
	{.pattern = "GRID",						.selector = GRID		},
	{.pattern = "GR",							.selector = GRID		},
	{.pattern = "ALL",						.selector = ALL		},
	{.pattern = "AL",							.selector = ALL		},
	
	{.pattern = "PERIOD",							.selector = SELECT_PERIOD		},
	{.pattern = "PER",								.selector = SELECT_PERIOD		},
	{.pattern = "FREQUENCY",					.selector = SELECT_FREQ		},
	{.pattern = "FREQ",								.selector = SELECT_FREQ		},
	{.pattern = "NWIDTH",							.selector = SELECT_NWIDTH		},
	{.pattern = "NWID",								.selector = SELECT_NWIDTH		},
	{.pattern = "PWIDTH",							.selector = SELECT_PWIDTH		},
	{.pattern = "PWID",								.selector = SELECT_PWIDTH		},
	{.pattern = "FALLTIME",						.selector = SELECT_FALLTIME		},
	{.pattern = "FALL",								.selector = SELECT_FALLTIME		},
	{.pattern = "RISETIME",						.selector = SELECT_RISETIME		},
	{.pattern = "RISE",								.selector = SELECT_RISETIME		},
	{.pattern = "MINIMUM",						.selector = SELECT_MIN		},
	{.pattern = "MIN",								.selector = SELECT_MIN		},
	{.pattern = "MAXIMUM",						.selector = SELECT_MAX		},
	{.pattern = "MAX",								.selector = SELECT_MAX		},
	{.pattern = "VAMP",								.selector = SELECT_VAMP		},
	{.pattern = "VAMPLITUDE",					.selector = SELECT_VAMP		},
	{.pattern = "VAV",								.selector = SELECT_VAV		},
	{.pattern = "VAVERAGE",						.selector = SELECT_VAV		},

	{.pattern = "?",									.selector = QUERY			},

	{NULL}
};


		/////////////////////////////////////////////////////
		//										UNITS LIST									 //
		/////////////////////////////////////////////////////

const SCPI_Units_TypeDef UnitList[] = {
	// voltage
	{.pattern = "UV",			.mult = 1e-6,	.type = VOLTAGE	},
	{.pattern = "MV",			.mult = 1e-3,	.type = VOLTAGE	},
	{.pattern = "V",			.mult = 1, 		.type = VOLTAGE	},
	{.pattern = "KV",			.mult = 1e3, 	.type = VOLTAGE	},
	// time
	{.pattern = "PS",			.mult = 1e-12,	.type = TIME	},
	{.pattern = "NS",			.mult = 1e-9,		.type = TIME 	},
	{.pattern = "US",			.mult = 1e-6,		.type = TIME 	},
	{.pattern = "MS",			.mult = 1e-3,		.type = TIME 	},
	{.pattern = "S",			.mult = 1,			.type = TIME	},
	{.pattern = "MIN",		.mult = 60,			.type = TIME	},
	{.pattern = "HR",			.mult = 3600,		.type = TIME 	},
	// freq
	{.pattern = "HZ",			.mult = 1,		.type = FREQENCY },
	{.pattern = "KHZ",		.mult = 1e3,	.type = FREQENCY },
	{.pattern = "MHZ",		.mult = 1e6,	.type = FREQENCY },
	{.pattern = "GHZ",		.mult = 1e9,	.type = FREQENCY },
			
	{NULL, NULL}	// end of list
};


		/////////////////////////////////////////////////////
		//						TOP LEVEL COMMANDS									 //
		/////////////////////////////////////////////////////

const SCPI_RootCommand_TypeDef RootCommandList[] = {												//orange part
	{.pattern = "IDN",				.callback = SCPI_IDN_callback},
	{.pattern = "RES", 				.callback = SCPI_RES_callback},
	{.pattern = "RST", 				.callback = SCPI_RES_callback},
	{.pattern = "RUN", 				.callback = SCPI_RUN_callback},
	{.pattern = "STOP", 			.callback = SCPI_STOP_callback},
	
	{.pattern = "TRAN", 				.callback = SCPI_TRAN_callback},
	{.pattern = "TRANSFER", 		.callback = SCPI_TRAN_callback},

	{.pattern = "AUTO", 				.callback = SCPI_AUTO_callback},
	{.pattern = "AUTOSCALE",		.callback = SCPI_AUTO_callback},

	{.pattern = "KEY", 					.callback = SCPI_Key_callback},
																																			//cyan part
	{.pattern = "CHANNELA", 		.callback = SCPI_Chn1_callback},
	{.pattern = "CHANA", 			.callback = SCPI_Chn1_callback},
	{.pattern = "CHANNEL1", 		.callback = SCPI_Chn1_callback},
	{.pattern = "CHAN1", 			.callback = SCPI_Chn1_callback},
	{.pattern = "CH1", 				.callback = SCPI_Chn1_callback},
	{.pattern = "CHA", 				.callback = SCPI_Chn1_callback},


	{.pattern = "CHANNEL2", 		.callback = SCPI_Chn2_callback},
	{.pattern = "CHAN2", 			.callback = SCPI_Chn2_callback},
	{.pattern = "CHANNELB", 		.callback = SCPI_Chn2_callback},
	{.pattern = "CHANB", 			.callback = SCPI_Chn2_callback},
	{.pattern = "CH2", 				.callback = SCPI_Chn2_callback},
	{.pattern = "CHB", 				.callback = SCPI_Chn2_callback},

	{.pattern = "TBAS", 				.callback = SCPI_Tbase_callback},
	{.pattern = "TBASE", 			.callback = SCPI_Tbase_callback},

	{.pattern = "TRIG", 				.callback = SCPI_Trig_callback},
	{.pattern = "TRIGER", 			.callback = SCPI_Trig_callback},
	
	{.pattern = "MEASU", 			.callback = SCPI_Measure_callback},
	{.pattern = "MEASUREMET", 	.callback = SCPI_Measure_callback},
	
	{.pattern = "DISP", 				.callback = SCPI_Disp_callback},
	{.pattern = "DISPLAY", 		.callback = SCPI_Disp_callback},
	{NULL, NULL, }
	//{.pattern = ":CHANnelB", 		.callback = SCPI_Chn2_callback}
};



		/////////////////////////////////////////////////////
		//						SECOND LEVEL COMMANDS								 //
		/////////////////////////////////////////////////////
								/*		BLUE PART		*/
const SCPI_2ndLvlCommand_TypeDef SecondLvlCommandList[] = {	

// Keys
	{.pattern = "LOCK",		.command = LOCKKEYS	},
	{.pattern = "RUN",			.command = RUNKEY	},
	{.pattern = "AUTO",		.command = AUTOKEY	},
	{.pattern = "CHAN1",			.command = CHAN1KEY	},
	{.pattern = "CHAN2",			.command = CHAN2KEY	},
	{.pattern = "MATH",			.command = MATHKEY	},
	{.pattern = "F1",			.command = F1KEY	},
	{.pattern = "F2",			.command = F2KEY	},
	{.pattern = "F3",			.command = F3KEY	},
	{.pattern = "F4",			.command = F4KEY	},
	{.pattern = "F5",			.command = F5KEY	},
	{.pattern = "MNU",			.command = MENUTOGGLEKEY	},
	{.pattern = "CURS",			.command = CURSORKEY	},
	{.pattern = "ACQ",			.command = ACQUIREKEY	},
	{.pattern = "DISP",			.command = DISPLAYKEY	},
	{.pattern = "MEM",			.command = MEMORYKEY	},
	{.pattern = "UTIL",			.command = UTILITYKEY	},
	{.pattern = "SWEEP",			.command = SWEEPKEY	},
	{.pattern = "TRIG",			.command = TRIGGERKEY	},
	{.pattern = "MEAS",			.command = MEASUREKEY	},
	{.pattern = "CH1_POS_INC",			.command = CH1_POS_INC	},
	{.pattern = "CH1_POS_DEC",			.command = CH1_POS_DEC	},
	{.pattern = "CH2_POS_INC",			.command = CH2_POS_INC	},
	{.pattern = "CH2_POS_DEC",			.command = CH2_POS_DEC	},
	{.pattern = "CH1_SCALE_INC",			.command = CH1_SCALE_INC 	},
	{.pattern = "CH1_SCALE_DEC",			.command = CH1_SCALE_DEC	},
	{.pattern = "CH2_SCALE_INC",			.command = CH2_SCALE_INC	},
	{.pattern = "CH2_SCALE_DEC",			.command = CH2_SCALE_DEC	},
	{.pattern = "TIME_SCALE_INC",			.command = TIME_SCALE_INC	},
	{.pattern = "TIME_SCALE_DEC",			.command = TIME_SCALE_DEC	},
	{.pattern = "TIME_POS_INC",			.command = TIME_POS_INC	},
	{.pattern = "TIME_POS_DEC",			.command = TIME_POS_DEC	},
	{.pattern = "FUNCTION_INC",			.command = FUNCTION_INC	},
	{.pattern = "FUNCTION_DEC",			.command = FUNCTION_DEC	},	
	
	
//CHannels	
	{.pattern = "MODE", 			.command = MODE 	},
	{.pattern = "MOD", 				.command = MODE 	},

	{.pattern = "RANG",	 			.command = RANGE 	},
	{.pattern = "RANGE", 			.command = RANGE 	},

	{.pattern = "OFFS",	 			.command = OFFSET},
	{.pattern = "OFFSET", 			.command = OFFSET},

	{.pattern = "COUP",		 		.command =  COUPLING},
	{.pattern = "COUPLING", 	.command =  COUPLING},

	{.pattern = "BAN",				.command =  BAND},
	{.pattern = "BANDWIDTH", 	.command =  BAND},

	{.pattern = "PRO",		 	  .command =  PROBE},
	{.pattern = "PROBE", 		  .command =  PROBE},

// TimeBase
//  
// 	{.pattern = "SCAN",			  .command = ROLL},
// 	{.pattern = "NORMAL",		  .command = ROLL},
// 	{.pattern = "ROLL",			  .command = ROLL},
// 	{.pattern = "ROL",			  .command = ROLL},
// 	{.pattern = "XYSWEEP",   	.command = XYSWEEP},
// 	{.pattern = "XY", 				.command = XYSWEEP},

// Trig	
	{.pattern = "DEL",		 		.command = DELAY},
	{.pattern = "DELAY", 			.command = DELAY},

	{.pattern = "SOU", 					.command = SOURCE},
	{.pattern = "SOURCE", 			.command = SOURCE},
	
	{.pattern = "LEV", 				.command = LEVEL},
	{.pattern = "LEVEL", 			.command = LEVEL},
	{.pattern = "LVL", 				.command = LEVEL},
	
	{.pattern = "SLOP",	 			.command = SLOPE},
	{.pattern = "SLOPE", 			.command = SLOPE},
	
	{.pattern = "FILTER", 			.command = FILTER},

// Measurement
	{.pattern = "PARAMETER1",		 	.command = PARAMETER1},
	{.pattern = "PARAM1",		 			.command = PARAMETER1},
	{.pattern = "PARAMETER2",		 	.command = PARAMETER2},
	{.pattern = "PARAM2",		 			.command = PARAMETER2},
	
	{.pattern = "PER",		 			.command = PERIOD},
	{.pattern = "PERIOD", 			.command = PERIOD},
	
	{.pattern = "FREQ", 				.command = FREQ},
	{.pattern = "FREQUENCY", 		.command = FREQ},
	
	{.pattern = "NWID",	 				.command = NWIDTH},
	{.pattern = "NWIPDTH", 			.command = NWIDTH},
	
	{.pattern = "PWID", 				.command = PWIDTH},
	{.pattern = "PWIDTH", 			.command = PWIDTH},
	
	{.pattern = "FALL",		 	  	.command = FALLTIME},
	{.pattern = "FALLTIME", 	  .command = FALLTIME},
	
	{.pattern = "RIS",			 	  .command = RISETIME},
	{.pattern = "RISETIME", 	  .command = RISETIME},
	
	{.pattern = "MAX", 			  .command = MAX},
	{.pattern = "MAXIMUM", 	  .command = MAX},
	
	{.pattern = "MIN", 			  .command = MIN},
	{.pattern = "MINIMUM", 	  .command = MIN},
	
	{.pattern = "VAMP", 		  	.command = VAMP},	
	{.pattern = "VAMPLITUDE",  	.command = VAMP},
	{.pattern = "VAV", 		 		 	.command = VAV},
	{.pattern = "VAVERAGE", 	 	.command = VAV},

// Display	
	{.pattern = "REF",				 	.command = REFERENCE},
	{.pattern = "REFERENCE",	 	.command = REFERENCE},
	
	{.pattern = "SCALE", 				.command = SCALE},
	{.pattern = "CLEAR", 				.command = CLEAR},
	
// end of list	
	{	NULL }
};

		
		/////////////////////////////////////////////////////
		//								PATTERTNS LIST END							 //
		/////////////////////////////////////////////////////
		//																								 //
		//																								 //



		/////////////////////////////////////////////////////
		//				GLOBAL FUNCTIONS PROTOTYPES							 //
		/////////////////////////////////////////////////////

void SCPI_Error_Handler(SCPI_Errors_TypeDef ErrCode);
//uint8_t* SCPI_ParseOneCommandFromBuffer(const uint8_t* CmBuffer);


#endif	/*	_SCPI_SIMPLE_PARSER_H	*/

