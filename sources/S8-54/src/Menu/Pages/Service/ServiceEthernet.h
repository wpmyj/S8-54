#pragma once


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
    (int8*)&set.eth.enable, FuncOfChangedEthernetSettings
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
    &set.eth.ip0, &set.eth.ip1, &set.eth.ip2, &set.eth.ip3,
    FuncOfChangedEthernetSettings,
    &set.eth.port
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
    &set.eth.mask0, &set.eth.mask1, &set.eth.mask2, &set.eth.mask3,
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
    &set.eth.gw0, &set.eth.gw1, &set.eth.gw2, &set.eth.gw3,
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
    &set.eth.mac0, &set.eth.mac1, &set.eth.mac2, &set.eth.mac3, &set.eth.mac4, &set.eth.mac5,
    FuncOfChangedEthernetSettings
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
const Page mspEthernet =
{
    Item_Page, &mpService, 0,
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
