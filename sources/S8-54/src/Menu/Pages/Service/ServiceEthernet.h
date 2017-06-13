#pragma once
#include "main.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspEthernet;
const Page pService;

static const IPaddress ipNetMask =
{
    Item_IP, &mspEthernet, 0,
    {
        "Маска подсети", "Network mask",
        "Установка маски подсети",
        "Set of network mask"
    },
    &NETMASK_ADDR0, &NETMASK_ADDR1, &NETMASK_ADDR2, &NETMASK_ADDR3,
    OnChanged_Ethernet_Settings
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const IPaddress ipGateway =
{
    Item_IP, &mspEthernet, 0,
    {
        "Шлюз", "Gateway",
        "Установка адреса основного шлюза",
        "Set of gateway address"
    },
    &GW_ADDR0, &GW_ADDR1, &GW_ADDR2, &GW_ADDR3,
    OnChanged_Ethernet_Settings
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const MACaddress macMAC =
{
    Item_MAC, &mspEthernet, 0,
    {
        "Физ адрес", "MAC-address",
        "Установка физического адреса",
        "Set of MAC-address"
    },
    &set.eth_mac0, &set.eth_mac1, &set.eth_mac2, &set.eth_mac3, &set.eth_mac4, &set.eth_mac5,
    OnChanged_Ethernet_Settings
};
