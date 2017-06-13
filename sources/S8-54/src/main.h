#pragma once
#include "Settings/Settings.h"
#include <usbd_desc.h>
#include <usbd_cdc.h>
#include <usbd_cdc_interface.h>

/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
// 00 13 8f af 2e 8b
// #define MAC_ADDR0   0x8b
// #define MAC_ADDR1   0x2e
// #define MAC_ADDR2   0xaf
// #define MAC_ADDR3   0x8f
// #define MAC_ADDR4   0x13
// #define MAC_ADDR5   0x00

/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
// 192.168.1.92
#define IP_ADDR0   set.eth_ip0
#define IP_ADDR1   set.eth_ip1
#define IP_ADDR2   set.eth_ip2
#define IP_ADDR3   set.eth_ip3

/*NETMASK*/
#define NETMASK_ADDR0   set.eth_mask0
#define NETMASK_ADDR1   set.eth_mask1
#define NETMASK_ADDR2   set.eth_mask2
#define NETMASK_ADDR3   set.eth_mask3

/*Gateway Address*/
#define GW_ADDR0   set.eth_gw0
#define GW_ADDR1   set.eth_gw1
#define GW_ADDR2   set.eth_gw2
#define GW_ADDR3   set.eth_gw3

#define MII_MODE

/* Uncomment the define below to clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef 	MII_MODE
#define PHY_CLOCK_MCO
#endif

