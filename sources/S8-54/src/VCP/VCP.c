// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "usbd_cdc_interface.h"
#include "usbd_desc.h"
#include "Utils/Math.h"
#include <stdarg.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USBD_HandleTypeDef handleUSBD;
PCD_HandleTypeDef handlePCD;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VCP_Init(void)
{
    USBD_Init(&handleUSBD, &VCP_Desc, 0);
    USBD_RegisterClass(&handleUSBD, &USBD_CDC);
    USBD_CDC_RegisterInterface(&handleUSBD, &USBD_CDC_fops);
    USBD_Start(&handleUSBD);
} 


//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool PrevSendingComplete(void)
{
    USBD_CDC_HandleTypeDef *pCDC = handleUSBD.pClassData;
    return pCDC->TxState == 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP_SendDataAsinch(uint8 *buffer, int size)
{
#define SIZE_BUFFER 64
    static uint8 trBuf[SIZE_BUFFER];

    size = Math_MinInt(size, SIZE_BUFFER);
    while (!PrevSendingComplete())  {};
    memcpy(trBuf, buffer, size);

    USBD_CDC_SetTxBuffer(&handleUSBD, trBuf, (uint16)size);
    USBD_CDC_TransmitPacket(&handleUSBD);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
#define SIZE_BUFFER_VCP 256     /// \todo если поставить размер буфера 512, то на ТЕ207 глюки
static uint8 buffSend[SIZE_BUFFER_VCP];
static int sizeBuffer = 0;


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP_Flush(void)
{
    if (sizeBuffer)
    {
        USBD_CDC_HandleTypeDef *pCDC = handleUSBD.pClassData;
        while (pCDC->TxState == 1) {};
        USBD_CDC_SetTxBuffer(&handleUSBD, buffSend, (uint16)sizeBuffer);
        USBD_CDC_TransmitPacket(&handleUSBD);
        while (pCDC->TxState == 1) {};
    }
    sizeBuffer = 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP_SendDataSynch(const uint8 *buffer, int size)
{
    if (!gConnectToHost)
    {
        return;
    }

    USBD_CDC_HandleTypeDef *pCDC = handleUSBD.pClassData;

    do 
    {
        if (sizeBuffer + size > SIZE_BUFFER_VCP)
        {
            int reqBytes = SIZE_BUFFER_VCP - sizeBuffer;
            LIMITATION(reqBytes, 0, size);
            while (pCDC->TxState == 1) {};
            memcpy(buffSend + sizeBuffer, buffer, reqBytes);
            USBD_CDC_SetTxBuffer(&handleUSBD, buffSend, SIZE_BUFFER_VCP);
            USBD_CDC_TransmitPacket(&handleUSBD);
            size -= reqBytes;
            buffer += reqBytes;
            sizeBuffer = 0;
        }
        else
        {
            memcpy(buffSend + sizeBuffer, buffer, size);
            sizeBuffer += size;
            size = 0;
        }
    } while (size);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void SendData(const uint8 *buffer, int size)
{

}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP_SendStringAsinch(char *data)
{
    VCP_SendDataAsinch((uint8 *)data, strlen(data));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP_SendStringSynch(char *data)
{
    VCP_SendDataSynch((uint8 *)data, strlen(data));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP_SendFormatStringAsynch(char *format, ...)
{
    if (!gConnectToHost)
    {
        return;
    }

    static char buffer[200];
    __va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    strcat(buffer, "\r\n");
    VCP_SendDataAsinch((uint8 *)buffer, strlen(buffer));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP_SendFormatStringSynch(char *format, ...)
{
    char buffer[200];
    __va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    strcat(buffer, "\r\n");
    VCP_SendDataSynch((uint8 *)buffer, strlen(buffer));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP_SendByte(uint8 byte)
{
    VCP_SendDataSynch(&byte, 1);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void OTG_HS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&handlePCD);
}
