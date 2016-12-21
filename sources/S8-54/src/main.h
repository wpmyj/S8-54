/** @file
  * @version V0.1
  * @date 14-March-2014
  * @brief Точка входа - инициализация и главный цикл
  * @verbatim
  *
  * Здесь будет описание
  *
  * @note
  * Для компиляции используйте MDK5.11.1.0 с установленными пакетами
  *     ARV CMSIS 4.1.4
  *     Keil MDK-Middleware 6.1.0
  *     Keil STM32F2xx_DFP 1.0.7
  *
  * Соглашения о наименовании
  * К функциям, доступ к которым требуется из других подсистем, добавляется префикс со знаком подчёркивания. Например, Display_.
  * Функции, используемые внутри одного каталога/подсистемы, такого префикса не имеют.
  * Функции сохранения/чтения настроек (подсистема SSettings) начинаются с Get/Set/Inc/Dec/Add/Sub/Is. Они не влияют на состояние прибора, 
  * служат ТОЛЬКО для записи/чтения настроек. Исключением являются функции SetIncreaseSmoothlyRange(), SetDecreaseSmoothlyRange(). Они вызывают FPGA_SetRange().
  *
  c:\Keil_v5\ARM\Pack\ARM\CMSIS\4.1.1\CMSIS\Include\
  c:\Keil_v5\ARM\Pack\Keil\STM32F2xx_DFP\1.0.7\Device\Include\

  * @endverbatim
  */ 

/*
Замеченные баги и пути решения
1.  Пример CDC_Standalone из STM32Cube V1.1.0 успешно запускался на 
    приборе при компиляции с startup_stm32f217xx.s и также запускался
    c startup_stm32f207xx.s, но в этом случае, хотя устройство и коннектилось,
    диспетчер устройств показывал ошибку 10 и работа с устройством была
    невозможна.
    Исследование показало, что в startup_stm32f217xx.s
    Stack_Size увеличен до 0x2000, а Heap_Size - до 0x4000. При этих диких
    значениях заработало и с startup_stm32f207xx.s
2.The ST's USBD_CDC_TransmitPacket() function contains a bug: it sets the 
    'transmission in progress' flag AFTER starting the actual transmission,
    so if the 'transmission done' interrupt happens too early, the flag will
    be set AFTER the interrupt and hence will never be cleared. To fix this,
    move "hcdc->TxState = 1" line before the call to USBD_LL_Transmit():
        if(hcdc->TxState == 0) {
            hcdc->TxState = 1;
            USBD_LL_Transmit(pdev, CDC_IN_EP, hcdc->TxBuffer,
            hcdc->TxLength);
            return USBD_OK;
        }
*/
  
  
  // Слово WARN в комментарии означает ошибку в коде.
  // Слова BUG означает ошибку в коде.

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
#define IP_ADDR0   set.eth.ip0
#define IP_ADDR1   set.eth.ip1
#define IP_ADDR2   set.eth.ip2
#define IP_ADDR3   set.eth.ip3

/*NETMASK*/
#define NETMASK_ADDR0   set.eth.mask0
#define NETMASK_ADDR1   set.eth.mask1
#define NETMASK_ADDR2   set.eth.mask2
#define NETMASK_ADDR3   set.eth.mask3

/*Gateway Address*/
#define GW_ADDR0   set.eth.gw0
#define GW_ADDR1   set.eth.gw1
#define GW_ADDR2   set.eth.gw2
#define GW_ADDR3   set.eth.gw3

#define MII_MODE

/* Uncomment the define below to clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef 	MII_MODE
#define PHY_CLOCK_MCO
#endif

