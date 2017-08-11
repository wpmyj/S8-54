// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#pragma once
#include <stm32f4xx_hal.h>
//#include <stdio.h>
#include <math.h>
#include <limits.h>

//#define S8_54

#define INTERRUPT_P2P   // Если определено, то для чтения точек используется прерывание
 
#include <stm32f4xx_hal.h>

#define NUM_VER "1.3"

#define OLD_RECORDER

typedef signed      char        int8;
typedef signed      char        int8_t;
typedef signed      short   int int16;
typedef unsigned    char        uint8;
typedef unsigned    char        uint8_t;
typedef unsigned    short   int uint16;
typedef unsigned    short   int uint16_t;
typedef unsigned long long int  uint64;
typedef unsigned    int         uint;
typedef unsigned    int         uint32;
typedef unsigned    int         uint32_t;
typedef unsigned    char        uchar;
typedef uint8                   BYTE; //-V6770


#ifndef bool
typedef unsigned char bool;
#define false   0
#define true    (!false)
#endif


#define DEBUG


#define __IO volatile
    
// Тип цвета
typedef enum
{
    COLOR_BLACK             = 0x00,     ///< Чёрный
    COLOR_WHITE             = 0x01,     ///< Белый
    COLOR_GRID              = 0x02,     ///< Сетка
    COLOR_DATA_1            = 0x03,     ///< Данные первого канала
    COLOR_DATA_2            = 0x04,     ///< Данные второго канала
    COLOR_MENU_FIELD        = 0x05,
    COLOR_MENU_TITLE        = 0x06,
    COLOR_MENU_TITLE_DARK   = 0x07,
    COLOR_MENU_TITLE_BRIGHT = 0x08,
    COLOR_MENU_ITEM         = 0x09,
    COLOR_MENU_ITEM_DARK    = 0x0a,
    COLOR_MENU_ITEM_BRIGHT  = 0x0b,
    COLOR_DATA_A_WHITE      = 0x0c,
    COLOR_DATA_B_WHITE      = 0x0d,
    COLOR_GRID_WHITE        = 0x0e,
    COLOR_EMPTY             = 0x0f,
    NUM_COLORS,
    COLOR_FLASH_10,
    COLOR_FLASH_01,
    INVERSE
} Color;

typedef void    (*pFuncVV)(void);               ///< Указатель на функцию, ничего не принимающую и ничего не возвращающую.
typedef void    (*pFuncVpV)(void*);             ///< Указатель на функцию, принимающую указатель на void и ничего не возвращающую.
typedef bool    (*pFuncBV)(void);
typedef void    (*pFuncVB)(bool);
typedef void    (*pFuncVI)(int);
typedef void    (*pFuncVII)(int, int);
typedef void    (*pFuncVIIC)(int, int, Color);
typedef void    (*pFuncVI16)(int16);
typedef bool    (*pFuncBU8)(uint8);
typedef void    (*pFuncVI16pI16pI16)(int16, int16*, int16*);
typedef float   (*pFuncFU8)(uint8);
typedef char*   (*pFuncCFB)(float, bool);
typedef char*   (*pFuncCFBC)(float, bool, char*);
typedef void    (*pFuncpU8)(uint8*);
typedef void    (*pFuncVpVIIB)(void*, int, int, bool);

#define GetBit(value, bit) (((value) >> (bit)) & 0x01)
#define SetBit_(value, bit) ((value) |= (1 << (bit)))

#define BIT_MASK(numBit) (1 << (numBit))

#pragma anon_unions

// Объединение размером 16 бит
typedef union
{
    uint16 halfWord;
    uint8  byte[2];
    struct
    {
        uint8 byte0;
        uint8 byte1;
    };
} BitSet16;

// Объединение размером 32 бита
typedef union
{
    uint    word;
    uint16  halfWord[2];
    struct
    {
        uint16 halfWord0;
        uint16 halfWord1;
    };
    uint8   byte[4]; //-V112
    struct
    {
        uint8 byte0;
        uint8 byte1;
        uint8 byte2;
        uint8 byte3;
    };
} BitSet32;

// Объединение размером 64 бита
typedef union
{
    long long unsigned int  dword;
    uint                    word[2];
    struct
    {
        uint word0;
        uint word1;
    };
    uint16  halfWord[4];
    struct
    {
        uint16 halfWord0;
        uint16 halfWord1;
        uint16 halfWord2;
        uint16 halfWord3;
    };
    uint8   byte[8];
    struct
    {
        uint8 byte0;
        uint8 byte1;
        uint8 byte2;
        uint8 byte3;
        uint8 byte4;
        uint8 byte5;
        uint8 byte6;
        uint8 byte7;
    };

} BitSet64;

typedef struct
{
    int16 rel;
    float abs;
} StructRelAbs;

#define _bitset(bits)                               \
  ((uint8)(                                         \
  (((uint8)((uint)bits / 01)        % 010) << 0) |  \
  (((uint8)((uint)bits / 010)       % 010) << 1) |  \
  (((uint8)((uint)bits / 0100)      % 010) << 2) |  \
  (((uint8)((uint)bits / 01000)     % 010) << 3) |  \
  (((uint8)((uint)bits / 010000)    % 010) << 4) |  \
  (((uint8)((uint)bits / 0100000)   % 010) << 5) |  \
  (((uint8)((uint)bits / 01000000)  % 010) << 6) |  \
  (((uint8)((uint)bits / 010000000) % 010) << 7)))

#define BINARY_U8( bits ) _bitset(0##bits)

#define MAX_UINT ((uint)-1)

#define DISABLE_RU  "Откл"
#define DISABLE_EN  "Disable"
#define ENABLE_RU   "Вкл"
#define ENABLE_EN   "Enable"

#define ERROR_VALUE_FLOAT   1.111e29f
#define ERROR_VALUE_INT16   SHRT_MAX
#define ERROR_VALUE_UINT8   255
#define ERROR_VALUE_INT     INT_MAX
#define ERROR_STRING_VALUE  "--.--"
#define M_PI                3.14159265358979323846f

#define SET_FLAG(flag)      ((flag) = 1)
#define CLEAR_FLAG(flag)    ((flag) = 0)

#define HARDWARE_ERROR HardwareErrorHandler(__FILE__, __FUNCTION__, __LINE__);
void HardwareErrorHandler(const char *file, const char *function, int line);

#define SAFE_FREE(x) if(x) free(x); (x) = 0;
