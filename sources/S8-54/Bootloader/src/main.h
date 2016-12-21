#pragma once

#include <stm32f4xx_hal.h>


typedef unsigned int uint;

// Key configuration
#define KEY_START_ADDRESS   (uint)0x0
#define KEY_PAGE_NUMBER     20
#define KEY_VALUE           0xAAAA5555

// Flash configuration
#define MAIN_PROGRAM_START_ADDRESS  (uint)0x8020000
#define MAIN_PROGRAM_PAGE_NUMBER    21
#define NUM_OF_PAGES                256
#define FLASH_PAGE_SIZE             2048


typedef enum
{
    State_Start,
    State_Mount,            // Монтирование флешки

    State_WrongFlash,       // Флешка есть, но прочитать нельзя
    State_RequestAction,    // Что делать - апгрейдить или нет
    State_Upgrade,          // Процесс апгрейда
    State_Ok                // Обновление удачно завершено
} State;

extern State state;
