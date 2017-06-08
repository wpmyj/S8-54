// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "main.h"
#include "Globals.h"
#include "VCP/VCP.h"
#include "VCP/SCPI/SCPI.h"
#include "Log.h"
#include "Hardware/Timer.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern USBD_HandleTypeDef handleUSBD;


static USBD_CDC_LineCodingTypeDef LineCoding =
{
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
};

#define APP_RX_DATA_SIZE  32
static uint8_t UserRxBuffer[APP_RX_DATA_SIZE];/* Received Data over USB are stored in this buffer */


static int8_t CDC_Itf_Init     (void);
static int8_t CDC_Itf_DeInit   (void);
static int8_t CDC_Itf_Control  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Itf_Receive  (uint8_t* pbuf, uint32_t *Len);


USBD_CDC_ItfTypeDef USBD_CDC_fops = 
{
  CDC_Itf_Init,
  CDC_Itf_DeInit,
  CDC_Itf_Control,
  CDC_Itf_Receive
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void SetAttributeConnected(void)
{
    gCableVCPisConnected = true;
    gConnectToHost = false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int8_t CDC_Itf_Init(void)
{
    USBD_CDC_SetRxBuffer(&handleUSBD, UserRxBuffer);
    Timer_SetAndStartOnce(kUSB, SetAttributeConnected, 100);   // WARN Задержка введена для того, чтобы не было ложных срабатываний в 
    return (USBD_OK);                                           // usbd_conf.c:HAL_PCD_SetupStageCallback при определении подключения хоста
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int8_t CDC_Itf_DeInit(void)
{
    gCableVCPisConnected = false;
    gConnectToHost = false;

    return (USBD_OK);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int8_t CDC_Itf_Control (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{ 
    switch (cmd)
    {
    case CDC_SEND_ENCAPSULATED_COMMAND:
        /* Add your code here */
        break;

    case CDC_GET_ENCAPSULATED_RESPONSE:
        /* Add your code here */
        break;

    case CDC_SET_COMM_FEATURE:
        /* Add your code here */
        break;

    case CDC_GET_COMM_FEATURE:
        /* Add your code here */
        break;

    case CDC_CLEAR_COMM_FEATURE:
        /* Add your code here */
        break;

    case CDC_SET_LINE_CODING:
        LineCoding.bitrate    = (uint32_t)(pbuf[0] | (pbuf[1] << 8) |\
                                (pbuf[2] << 16) | (pbuf[3] << 24));
        LineCoding.format     = pbuf[4];
        LineCoding.paritytype = pbuf[5];
        LineCoding.datatype   = pbuf[6];
        break;

    case CDC_GET_LINE_CODING:
        pbuf[0] = (uint8_t)(LineCoding.bitrate);
        pbuf[1] = (uint8_t)(LineCoding.bitrate >> 8);
        pbuf[2] = (uint8_t)(LineCoding.bitrate >> 16);
        pbuf[3] = (uint8_t)(LineCoding.bitrate >> 24);
        pbuf[4] = LineCoding.format;
        pbuf[5] = LineCoding.paritytype;
        pbuf[6] = LineCoding.datatype;
        /* Add your code here */
        break;

    case CDC_SET_CONTROL_LINE_STATE:
        /* Add your code here */
        break;

    case CDC_SEND_BREAK:
        /* Add your code here */
        break;    
    
    default:
        break;
    }
  
    return (USBD_OK);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int8 CDC_Itf_Receive(uint8* buffer, uint *length)
{
    static int sizeData = 0;

    for (uint i = 0; i < *length; i++)
    {
        if(0 == sizeData && buffer[0] != ':')
        {
            continue;
        }
        UserRxBuffer[sizeData] = buffer[i];
        sizeData++;
        if (sizeData > 2 && UserRxBuffer[sizeData - 1] == '\x0a' && UserRxBuffer[sizeData - 2] == '\x0d')
        {
            SCPI_ParseNewCommand(UserRxBuffer + 1);
            sizeData = 0;
        }
        if (sizeData == APP_RX_DATA_SIZE)
        {
            LOG_ERROR_TRACE("Переполнение приёмного буфера VCP");
            sizeData = 0;
            break;
        }
    }

    USBD_CDC_SetRxBuffer(&handleUSBD, UserRxBuffer + sizeData);
    USBD_CDC_ReceivePacket(&handleUSBD);
    return (USBD_OK);
}
