#pragma once


#include "lwip/err.h"
#include "lwip/netif.h"


err_t ethernetif_init(struct netif *netif);
void ethernetif_input(struct netif *netif);
void ethernetif_set_link(struct netif *netif);
void ethernetif_update_config(struct netif *netif);
void ethernetif_notify_conn_changed(struct netif *netif);


extern uint gEthTimeLastEthifInput;
