#pragma once
#include "defines.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Ethernet
 *  @{
 *  @defgroup TcpSocket
 *  @{
 */

struct tcp_pcb;

static const int    DEFAULT_PORT = 7,
                    POLICY_PORT = 843;

bool TCPSocket_Init(void (*funcConnect)(void), void (*funcReceiver)(const char *buffer, uint length));

bool TCPSocket_Send(const char *buffer, uint length);

void TCPSocket_SendFormatString(char *format, ...);

void ETH_SendFormatString(char *format, ...);

extern bool gEthIsConnected;

/** @}  @}
 */
