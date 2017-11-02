#pragma once
#include "Globals.h"
#include "VCP/VCP.h"
#include "VCP/usbd_conf.h"
#include "Ethernet/TcpSocket.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @defgroup SCPI
 *  @{
 */

#define ENTER_ANALYSIS                                  \
    Word parameter;                                     \
    if (GetWord(buffer, &parameter, 0)) {               \
        uint8 value = GetValueFromMap(map, &parameter); \
        if (value < 255) {

#define LEAVE_ANALYSIS   }              \
    else                                \
    {                                  \
        SCPI_SEND("COMMAND ERROR");    \
    }                                    \
    }


#define SCPI_SEND(...)                              \
    if(CONNECTED_TO_USB)                            \
    {                                               \
        vcp.SendFormatStringAsynch(__VA_ARGS__);    \
    };                                              \
    if (gEthIsConnected)                            \
    {                                               \
        TCPSocket_SendFormatString(__VA_ARGS__);    \
    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct 
{
    char       *name;   ///< Название команды
    pFuncVU8   func;    ///< Функция выполнения
} StructCommand;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Декодирует и выполняет данные из buffer. Возвращает число обработанных байт, которые нужно удалить из начала буфера
int SCPI_ParseNewCommand(uint8 *buffer, int length);

void SCPI_ProcessingCommand(const StructCommand *commands, uint8 *buffer);

void Process_DISPLAY(uint8 *buffer);

void Process_CHANNEL(uint8 *buffer);

void Process_TRIG(uint8 *buffer);

void Process_TBASE(uint8 *buffer);
/// \brief Извлекает целое число из буфера и помещает по адресу value. Возвращает false, если в начале буфера не находится число или число не входит 
/// в границы [min; max]
bool SCPI_FirstIsInt(uint8 *buffer, int *value, int min, int max);

/** @}
 */
