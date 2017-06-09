#pragma once
#include "main.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspEthernet;


// СЕРВИС - ETHERNET  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void FuncOfChangedEthernetSettings(bool active)
{
    Display_ShowWarning(NeedRebootDevice);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcServEthEnable =
{
    Item_Choice, &mspEthernet, 0,
    {
        "Ethernet",    "Ethernet"
        ,
        "Чтобы задействовать ethernet, выберите \"Включено\" и выключите прибор.\n"
        "Чтобы отключить ethernet, выберите \"Отключено\" и выключите прибор."
        ,
        "To involve ethernet, choose \"Included\" and switch off the device.\n"
        "To disconnect ethernet, choose \"Disconnected\" and switch off the device."
    },
    {
        {"Включено",    "Included"},
        {"Отключено",   "Disconnected"}
    },
    (int8*)&ETH_ENABLED, FuncOfChangedEthernetSettings
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const IPaddress ipAddress =
{
    Item_IP, &mspEthernet, 0,
    {
        "IP адрес", "IP-address",
        "Установка IP адреса",
        "Set of IP-address"
    },
    &IP_ADDR0, &IP_ADDR1, &IP_ADDR2, &IP_ADDR3,
    FuncOfChangedEthernetSettings,
    &ETH_PORT
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const IPaddress ipNetMask =
{
    Item_IP, &mspEthernet, 0,
    {
        "Маска подсети", "Network mask",
        "Установка маски подсети",
        "Set of network mask"
    },
    &NETMASK_ADDR0, &NETMASK_ADDR1, &NETMASK_ADDR2, &NETMASK_ADDR3,
    FuncOfChangedEthernetSettings
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
    FuncOfChangedEthernetSettings
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
    FuncOfChangedEthernetSettings
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const Page mspEthernet =
{
    Item_Page, &pService, 0,
    {
        "ETHERNET", "ETHERNET",
        "Настройки ethernet",
        "Settings of ethernet"
    },
    Page_ServiceEthernet,
    {
        (void*)&mcServEthEnable,
        (void*)&ipAddress,
        (void*)&ipNetMask,
        (void*)&ipGateway,
        (void*)&macMAC
    }
};
