// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Ethernet.h"
#include "TcpSocket.h"
#include "main.h"
#include "Hardware/Timer.h"
#include "ethernetif.h"
#include "VCP/SCPI/SCPI.h"
#include "Log.h"


#include <lwip/init.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <lwip/timeouts.h>
#include <netif/etharp.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Netif_Config(void);

static struct netif gnetif;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void FuncConnect(void)
{
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncReceiver(const char *buffer, uint length)
{
    static int sizeData = 0;

#define SIZE_BUFFER_TCP 128
    static char data[SIZE_BUFFER_TCP];

    for (uint i = 0; i < length; i++)
    {
        if (0 == sizeData && buffer[0] != ':')
        {
            continue;
        }

        data[sizeData] = buffer[i];
        sizeData++;
        if (sizeData > 2 && data[sizeData - 1] == '\x0a' && data[sizeData - 2] == '\x0d')
        {
            SCPI_ParseNewCommand((uint8*)&data[1]);
            sizeData = 0;
        }
        if (sizeData == SIZE_BUFFER_TCP)
        {
            LOG_ERROR_TRACE("Переполнение приёмного буфера ЕTH");
            sizeData = 0;
            break;
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Ethernet_Init(void)
{
    // Initilaize the LwIP stack
    lwip_init();

    // Configure the Network interface
    Netif_Config();

    TCPSocket_Init(FuncConnect, FuncReceiver);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Ethernet_Update(uint timeMS)
{
    uint time = gTimerMS;

    do 
    {
        gBF.cableEthIsConnected = gTimerMS - gEthTimeLastEthifInput <= 1500;

        ethernetif_input(&gnetif);
        sys_check_timeouts();
    } while (gTimerMS - time < timeMS);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Netif_Config(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    // IP address default setting
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    // add the network interface
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    // Registers the default network interface
    netif_set_default(&gnetif);

    if (netif_is_link_up(&gnetif))
    {
        // When the netif is fully configured this function must be called
        netif_set_up(&gnetif);
    }
    else
    {
        // When the netif link is down this function must be called
        netif_set_down(&gnetif);
    }

    // Set the link callback function, this function is called on change of link status
    netif_set_link_callback(&gnetif, ethernetif_update_config);
}
