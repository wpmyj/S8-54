#pragma once
#include "defines.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @defgroup VCP
 *  @brief Virtual Com Port
 *  @{
 */

class VCP
{
public:
    void Init(void);

    void SendDataAsynch(uint8 *data, int size);

    void SendDataSynch(const uint8 *data, int size);
    /// Передаётся строка без завершающего нуля
    void SendStringAsynch(char *data);
    /// Передаётся строка без завершающего нуля
    void SendStringSynch(char *data);
    /// Эта строка передаётся с завершающими символами \\r\\n
    void SendFormatStringAsynch(char *format, ...);
    /// Эта строка передаётся с завершающими символами \\r\\n
    void SendFormatStringSynch(char *format, ...);

    void SendByte(uint8 data);

    void Flush(void);
};


extern VCP vcp;


/** @}
 */
