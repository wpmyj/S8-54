#include "VCP.h"
#include "usbd_cdc_interface.h"
#include "usbd_desc.h"
#include "Utils/Math.h"
#include <stdarg.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VCP vcp;

USBD_HandleTypeDef handleUSBD;
PCD_HandleTypeDef handlePCD;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VCP::Init(void)
{
    USBD_Init(&handleUSBD, &VCP_Desc, 0);
    USBD_RegisterClass(&handleUSBD, &USBD_CDC);
    USBD_CDC_RegisterInterface(&handleUSBD, &USBD_CDC_fops);
    USBD_Start(&handleUSBD);
} 


//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool PrevSendingComplete(void)
{
    USBD_CDC_HandleTypeDef *pCDC = (USBD_CDC_HandleTypeDef *)handleUSBD.pClassData;
    return pCDC->TxState == 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP::SendDataAsynch(uint8 *buffer, int size)
{
#define SIZE_BUFFER 64
    static uint8 trBuf[SIZE_BUFFER];

    size = math.MinInt(size, SIZE_BUFFER);
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
void VCP::Flush(void)
{
    if (sizeBuffer)
    {
        USBD_CDC_HandleTypeDef *pCDC = (USBD_CDC_HandleTypeDef *)handleUSBD.pClassData;
        while (pCDC->TxState == 1) {};
        USBD_CDC_SetTxBuffer(&handleUSBD, buffSend, (uint16)sizeBuffer);
        USBD_CDC_TransmitPacket(&handleUSBD);
        while (pCDC->TxState == 1) {};
    }
    sizeBuffer = 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP::SendDataSynch(const uint8 *buffer, int size)
{
    if (CONNECTED_TO_USB)
    {
        USBD_CDC_HandleTypeDef *pCDC = (USBD_CDC_HandleTypeDef *)handleUSBD.pClassData;
    
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
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void SendData(const uint8 *buffer, int size)
{

}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP::SendStringAsynch(char *data)
{
    SendDataAsynch((uint8 *)data, strlen(data));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP::SendStringSynch(char *data)
{
    SendDataSynch((uint8 *)data, strlen(data));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP::SendFormatStringAsynch(char *format, ...)
{
    if (CONNECTED_TO_USB)
    {
        static char buffer[200];
        va_list args;
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);
        strcat(buffer, "\r\n");
        SendDataAsynch((uint8 *)buffer, strlen(buffer));
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP::SendFormatStringSynch(char *format, ...)
{
    char buffer[200];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    strcat(buffer, "\r\n");
    SendDataSynch((uint8 *)buffer, strlen(buffer));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void VCP::SendByte(uint8 byte)
{
    SendDataSynch(&byte, 1);
}


#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OTG_HS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&handlePCD);
}

#ifdef __cplusplus
}
#endif
