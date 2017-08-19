#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Hardware
 *  @{
 *  @defgroup RAM
 *  @brief Работа с внешним ОЗУ
 *  @{
 */

typedef enum
{
    FPGA_DATA_A,
    FPGA_DATA_B,

    // DataStorage
    DS_DATA_IMPORT_REL_A,
    DS_DATA_IMPORT_REL_B,
    DS_AVE_DATA_A,
    DS_AVE_DATA_B,
    DS_SUM_A,
    DS_SUM_B,
    DS_LIMIT_UP_A,
    DS_LIMIT_UP_B,
    DS_LIMIT_DOWN_A,
    DS_LIMIT_DOWN_B,

    DRAW_MATH_DATA_REL_A,
    DRAW_MATH_DATA_REL_B,

    DS_POOL_BEGIN,         ///< Здесь начало места, где хранятся сигналы во внешнем ОЗУ
    DS_POOL_END            ///< Здесь конец места, где хранятся сигналы во внешнем ОЗУ
} Address;


/// Инициализация
void RAM_Init(void);
/// Число в названии функции указывает на количество байт, которым кратно numBytes
void RAM_WriteRead_Sinch2(void *src, void *dest, int numBytes);
/// Чем больше число, тем больше скорость
void RAM_WriteRead_Sinch4(void *src, void *dest, int numBytes);

void RAM_WriteRead_Sinch8(void *src, void *dest, int numBytes);
/// Для записи использует режим DMA. Не работает, если данные размещены в памяти CCM data RAM
void RAM_WriteRead_Asinch(uint16 *src, uint16 *dest, int numHalfWords);

bool RAM_WriteReadComplete(void);

void RAM_WaitWriteReadComplete(void);

void RAM_MemSet_Sinch(uint8 *dest, uint8 value, int numBytes);

void RAM_MemShiftLeft(uint8 *address, int numBytes, int shift);

void RAM_WriteByte(void *dest, uint8 value);

uint8 RAM_ReadByte(void *src);

void RAM_WriteWord(void *dest, uint value);

uint RAM_ReadWord(void *src);
/// Копирует буфер с размером, кратным 16. Адрес во внешнем ОЗУ должен быть выравнен по полусловам
void RAM_MemCpy16(void *src, void *dest, int numBytes);
/// Читает данные из ОЗУ. Рамзем кратен одному байту. src может быть кратен байту
void RAM_ReadBuffer1(void *src, void *dest, int numBytes);

void RAM_MemClear(void *address, int numHalfWords);

void RAM_Test(void);

uint8 *RAM(Address address);

/** @}  @}
 */
