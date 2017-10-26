

#include "defines.h"
#include "Globals.h"
#include "Log.h"
#include "RAM.h"
#include "FPGA/FPGAtypes.h"
#include "Hardware/FSMC.h"
#include "Hardware/Timer.h"
#include "Hardware/Hardware.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static volatile bool transferComplete = true;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RAM_WriteRead_Sinch2(void *src_, void *dest_, int numBytes)
{
    uint16 *src = (uint16 *)src_;
    uint16 *end = src + numBytes / 2;
    uint16 *dest = (uint16 *)dest_;

    FSMC_SetMode(ModeFSMC_RAM);

    while (src < end)
    {
        *dest++ = *src++;
    }

    FSMC_RestoreMode();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_WriteRead_Sinch4(void *src_, void *dest_, int numBytes)
{
    uint16 *src = (uint16 *)src_;
    uint16 *end = src + numBytes / 2;
    uint16 *dest = (uint16 *)dest_;

    FSMC_SetMode(ModeFSMC_RAM);

    while (src < end)
    {
        *dest++ = *src++;
        *dest++ = *src++;
    }

    FSMC_RestoreMode();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_WriteRead_Sinch8(void *src_, void *dest_, int numBytes)
{
    uint16 *src = (uint16 *)src_;
    uint16 *end = src + numBytes / 2;
    uint16 *dest = (uint16 *)dest_;

    FSMC_SetMode(ModeFSMC_RAM);

    while (src < end)
    {
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
    }

    FSMC_RestoreMode();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_MemCpy16(void *src_, void *dest_, int numBytes)
{
    FSMC_SET_MODE(ModeFSMC_RAM);

    uint16 *src = (uint16 *)src_;
    uint16 *dest = (uint16 *)dest_;
    uint16 *end = src + numBytes / 2;

    while (src < end)
    {
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
    }

    FSMC_RESTORE_MODE();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_ReadBuffer1(void *src, void *dest, int numBytes)
{
    uint16 *addrRAM = (uint16 *)src;
    uint16 *addrDest = (uint16 *)dest;

    if ((uint)addrRAM & 0x1)
    {
        uint8 data = RAM_ReadByte(src);
        *((uint8 *)dest) = data;
        numBytes--;
        addrRAM = (uint16 *)(((uint8 *)src) + 1);
        addrDest = (uint16 *)(((uint8 *)dest) + 1);
    }

    int numHalfWords = numBytes / 2;

    for (int i = 0; i < numHalfWords; i++)
    {
        *addrDest++ = *addrRAM++;
    }

    if (numBytes & 0x1)
    {
        uint8 data = RAM_ReadByte(src);
        *((uint8 *)dest) = data;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_WriteRead_Asinch(uint16 *src, uint16 *dest, int numHalfWords)
{
    RAM_WaitWriteReadComplete();

    transferComplete = false;

    HAL_DMA_Start_IT(&handleDMA_RAM, (uint)src, (uint)dest, numHalfWords * 2); //-V221
    
    RAM_WaitWriteReadComplete();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_MemSet_Sinch(uint8 *dest, uint8 value_, int numBytes)
{
    uint16 value = (value_ << 8) | value_;
    
    uint16 *address = (uint16 *)dest;
    for (int i = 0; i < numBytes / 2; i++)
    {
        *address++ = value;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool RAM_WriteReadComplete(void)
{
    return transferComplete;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_WaitWriteReadComplete(void)
{
    while (!transferComplete) {};
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void TransferComplete(DMA_HandleTypeDef *dmaHandle)
{
    transferComplete = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void TransferError(DMA_HandleTypeDef *dmaHandle)
{
    HARDWARE_ERROR
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_Init(void)
{
    __HAL_RCC_DMA2_CLK_ENABLE();

    handleDMA_RAM.XferCpltCallback = TransferComplete;
    handleDMA_RAM.XferErrorCallback = TransferError;

    HAL_DMA_DeInit(&handleDMA_RAM);

    if (HAL_DMA_Init(&handleDMA_RAM) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, PRIORITY_RAM_DMA2_STREAM0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_WriteByte(void *dest, uint8 value)
{
    if ((int)dest & 0x1) //-V205
    {
        uint16 *addr = (uint16 *)((int)dest & 0xfffffffe); //-V205 //-V204
        *addr = (value << 8) + (uint8)(*addr);
    }
    else
    {
        uint16 *addr = (uint16 *)dest;      
        *addr = ((*addr) & 0xff00) + value;
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_WriteWord(void *dest, uint value)
{
    uint16 *addr = (uint16 *)((int)dest); //-V205 //-V204 //-V220
    *addr = (uint16)value;
    *(addr + 1) = (uint16)(value >> 16);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_MemShiftLeft(uint8 *address, int numBytes, int shift)
{
    uint16 *addr = (uint16 *)address;
    int counter = numBytes / 2;
    int s = shift / 2;

    for (int i = 0; i < counter; i++)
    {
        *(addr - s) = *(addr);
        addr++;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 RAM_ReadByte(void *src)
{
    uint16 *addr = 0;
    bool odd = (int)src & 0x1; //-V205

    if(odd)
    {
        addr = (uint16 *)((int)src & 0xfffffffe); //-V205 //-V204
    }
    else
    {
        addr = (uint16 *)src;
    }

    uint16 value = *addr;

    if(odd)
    {
        value = value >> 8;
    }

    return (uint8)value;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
uint RAM_ReadWord(void *src)
{
    uint16 *addr0 = (uint16 *)((int)src); //-V205 //-V204 //-V220
    uint16 *addr1 = addr0 + 1;

    return (*addr0) + ((*addr1) << 16);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void RAM_MemClear(void *address_, int numHalfWords)
{
    FSMC_SET_MODE(ModeFSMC_RAM);
    
    uint16 *address = (uint16 *)address_;

    for (int i = 0; i < numHalfWords; i++)
    {
        address[i] = 0;
    }
    
    FSMC_RESTORE_MODE();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 *RAM8(Address address)
{                                                                                           // битность,  пам€ть, всего
                                                                                            //    шт        кЅ
#define RAM_FPGA_DATA_A             (ADDR_RAM)                                              //    8 
#define RAM_FPGA_DATA_B             ((RAM_FPGA_DATA_A)          + (FPGA_MAX_POINTS))        //    8 
#define RAM_DS_DATA_IMPORT_REL_A    ((RAM_FPGA_DATA_B)          + (FPGA_MAX_POINTS))        //    8 
#define RAM_DS_DATA_IMPORT_REL_B    ((RAM_DS_DATA_IMPORT_REL_A) + (FPGA_MAX_POINTS))        //    8 
#define RAM_DS_AVE_DATA_A           ((RAM_DS_DATA_IMPORT_REL_B) + (FPGA_MAX_POINTS))        //    32
#define RAM_DS_AVE_DATA_B           ((RAM_DS_AVE_DATA_A)        + (FPGA_MAX_POINTS) * 4)    //    32
#define RAM_DS_SUM_A                ((RAM_DS_AVE_DATA_B)        + (FPGA_MAX_POINTS) * 4)    //    32
#define RAM_DS_SUM_B                ((RAM_DS_SUM_A)             + (FPGA_MAX_POINTS) * 4)    //    32
#define RAM_DS_LIMIT_UP_A           ((RAM_DS_SUM_B)             + (FPGA_MAX_POINTS) * 4)    //    8 
#define RAM_DS_LIMIT_UP_B           ((RAM_DS_LIMIT_UP_A)        + (FPGA_MAX_POINTS))        //    8 
#define RAM_DS_LIMIT_DOWN_A         ((RAM_DS_LIMIT_UP_B)        + (FPGA_MAX_POINTS))        //    8 
#define RAM_DS_LIMIT_DOWN_B         ((RAM_DS_LIMIT_DOWN_A)      + (FPGA_MAX_POINTS))        //    8 
#define RAM_DS_P2P_FRAME            ((RAM_DS_LIMIT_DOWN_B)      + (FPGA_MAX_POINTS) * 2)    //
#define RAM_DRAW_MATH_DATA_REL_A    ((RAM_DS_P2P_FRAME  )       + (FPGA_MAX_POINTS))        //    32
#define RAM_DRAW_MATH_DATA_REL_B    ((RAM_DRAW_MATH_DATA_REL_A) + (FPGA_MAX_POINTS) * 4)    //    32
#define RAM_DS_POOL_BEGIN           ((RAM_DRAW_MATH_DATA_REL_B) + (FPGA_MAX_POINTS) * 4)    //
#define RAM_DS_POOL_END             ((ADDR_RAM)                 + (1024 * 1024))

    static const uint8 *addresses[] =
    {
        RAM_FPGA_DATA_A,
        RAM_FPGA_DATA_B,

        RAM_DS_DATA_IMPORT_REL_A,
        RAM_DS_DATA_IMPORT_REL_B,
        RAM_DS_AVE_DATA_A,
        RAM_DS_AVE_DATA_B,
        RAM_DS_SUM_A,
        RAM_DS_SUM_B,
        RAM_DS_LIMIT_UP_A,
        RAM_DS_LIMIT_UP_B,
        RAM_DS_LIMIT_DOWN_A,
        RAM_DS_LIMIT_DOWN_B,

        RAM_DS_P2P_FRAME,

        RAM_DRAW_MATH_DATA_REL_A,
        RAM_DRAW_MATH_DATA_REL_B,

        RAM_DS_POOL_BEGIN,
        RAM_DS_POOL_END
    };
 
    return (uint8 *)addresses[address];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint16 *RAM16(Address address)
{
    return (uint16 *)RAM8(address);
}
