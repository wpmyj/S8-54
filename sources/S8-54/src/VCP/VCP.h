#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void    VCP_Init(void);
void    VCP_SendDataAsynch(uint8 *data, int size);
void    VCP_SendDataSynch(const uint8 *data, int size);
void    VCP_SendStringAsynch(char *data);               // Передаётся строка без завершающего нуля
void    VCP_SendStringSynch(char *data);                // Передаётся строка без завершающего нуля
void    VCP_SendFormatStringAsynch(char *format, ...);  // Эта строка передаётся с завершающими символами \r\n
void    VCP_SendFormatStringSynch(char *format, ...);   // Эта строка передаётся с завершающими символами \r\n
void    VCP_SendByte(uint8 data);
void    VCP_Flush(void);
