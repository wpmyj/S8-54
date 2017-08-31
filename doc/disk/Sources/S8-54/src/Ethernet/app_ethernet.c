#include "lwip/opt.h"
#include "main.h"
#include "lwip/dhcp.h"
#include "app_ethernet.h"
#include "ethernetif.h"
#include "globals.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void User_notification(struct netif *netif) 
{
    if (netif_is_up(netif))
    {
    }
    else
    {
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ethernetif_notify_conn_changed(struct netif *netif)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    if (netif_is_link_up(netif))
    {
        IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
        IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);    

        netif_set_addr(netif, &ipaddr, &netmask, &gw);
    
        /* When the netif is fully configured this function must be called.*/
        netif_set_up(netif);
        CABLE_LAN_IS_CONNECTED = true;
    }
    else
    {   
        /*  When the netif link is down this function must be called.*/
        netif_set_down(netif);
        CABLE_LAN_IS_CONNECTED = false;
    }
}
