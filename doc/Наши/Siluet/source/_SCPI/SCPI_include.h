/**
  ******************************************************************************
  * @file    SCPI_include.h
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
	
#ifndef _SCPI_SIMPLE_PARSER_INCLUDED_H
#define _SCPI_SIMPLE_PARSER_INCLUDED_H
	
#include <stdint.h>
 #include "usbd_conf.h"

//********************************************//
//								SCPI config									//
//********************************************//

// comment or uncomment this line to choose direct work or with ring buffer
#define USE_RING_BUFFER
	
	
	
// #ifndef	BOOL_TYPE
// #define BOOL_TYPE
// typedef enum
// {
//   FALSE = 0, TRUE  = !FALSE
// }
// bool;

// #endif	/*	bool	*/


// #ifndef	USB_Rx_Buffer_TypeDef
// typedef struct	{
// 			bool 			Fresh;										// if TRUE - data is not processed yet
// 			uint32_t	Length;										// number of recieved databytes
// 			uint8_t 	Data[64];									// Buffer
// } USB_Rx_Buffer_TypeDef;
// #endif	/*	USB_Rx_Buffer_TypeDef 	*/	
// 	

#ifdef USE_RING_BUFFER
/*----------------------------------------------------------------------------
  Define macros for ring buffers																								//OK
 *----------------------------------------------------------------------------*/
#define RING_BUF_SIZE (256)              		/* serial buffer in bytes (power 2)   */
#define RING_BUF_MASK (RING_BUF_SIZE-1ul) 	/* buffer size mask                   */

typedef enum {
	RS232 = 1,
	USB = 2
}	RingBuffDataSourse_TypeDef;

typedef enum {
	RING_FULL = 0,
	RING_EMPTY = 0,
	RING_OK = 2
}	RingBuffStatus_TypeDef;

/* buffer type
*/
typedef struct {
	RingBuffDataSourse_TypeDef source;
  uint32_t writeIndex;
  uint32_t readIndex;
  uint8_t data[RING_BUF_SIZE];
} Ring_Buffer_TypeDef;

/* RingBuffer operation macro commands	*/                                          
#define RING_BUFF_RESET(ringBuf)     		 							(ringBuf.readIndex = ringBuf.writeIndex = 0)
#define RING_BUFF_RESET_BY_ADDR(ringBuf)     		 			(ringBuf->readIndex = ringBuf->writeIndex = 0)

#define RING_BUFF_WRITE(ringBuf, byteWrite)						(ringBuf.data[RING_BUF_MASK & ringBuf.writeIndex++] = (byteWrite))
#define RING_BUFF_WRITE_BY_ADDR(ringBuf, byteWrite)		(ringBuf->data[RING_BUF_MASK & ringBuf->writeIndex++] = (byteWrite))

#define RING_BUFF_READ(ringBuf, byteRead)	 						ringBuf.readIndex++; byteRead = ringBuf.data[RING_BUF_MASK & (ringBuf.readIndex-1)];
#define RING_BUFF_READ_BY_ADDR(ringBuf, byteRead)	 		ringBuf->readIndex++; byteRead = ringBuf->data[RING_BUF_MASK & (ringBuf->readIndex-1)];

#define RING_BUFF_EMPTY(ringBuf)     									((RING_BUF_MASK & ringBuf.readIndex) == (RING_BUF_MASK & ringBuf.writeIndex))
#define RING_BUFF_EMPTY_BY_ADDR(ringBuf)     					((RING_BUF_MASK & ringBuf->readIndex) == (RING_BUF_MASK & ringBuf->writeIndex))

#define RING_BUFF_FULL(ringBuf)     			 						((RING_BUF_MASK & ringBuf.readIndex) == (RING_BUF_MASK & (ringBuf.writeIndex+1)))
#define RING_BUFF_FULL_BY_ADDR(ringBuf)     					((RING_BUF_MASK & ringBuf->readIndex) == (RING_BUF_MASK & (ringBuf->writeIndex+1)))

#define RING_BUFF_COUNT(ringBuf)     									(RING_BUF_MASK & (ringBuf.writeIndex - ringBuf.readIndex))
#define RING_BUFF_COUNT_BY_ADDR(ringBuf)     					(RING_BUF_MASK & (ringBuf->writeIndex - ringBuf->readIndex))
/* end of RingBuffer operation macro commands	*/ 

	void CopyRxBufferToRing( const USB_Rx_Buffer_TypeDef* USB_Rx_Buffer, Ring_Buffer_TypeDef* Ring_Buffer);
	RingBuffStatus_TypeDef CorrectRingBufferIndexes(Ring_Buffer_TypeDef* Ring_Buffer);
	void SCPI_ParseOneCommandFromBuffer( Ring_Buffer_TypeDef* RingCmBuffer);

#else
	uint8_t* SCPI_ParseOneCommandFromBuffer(const uint8_t* CmBuffer);
#endif	/*	USE_RING_BUFFER	*/

	//void SCPI_Error_Handler(SCPI_Errors_TypeDef ErrCode);
	
#endif	/*	_SCPI_SIMPLE_PARSER_INCLUDED_H	*/
	
