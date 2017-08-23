// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Globals.h"
#include "Display/DisplayTypes.h"
#include "Settings/SettingsMemory.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HCD_HandleTypeDef handleHCD;

USBH_HandleTypeDef handleUSBH;


DMA_HandleTypeDef handleDMA_RAM =
{
    DMA2_Stream0,                   // Instance
    {
        DMA_CHANNEL_0,              // Channel
        DMA_MEMORY_TO_MEMORY,       // Direction
        DMA_PINC_ENABLE,            // PeriphInc
        DMA_MINC_ENABLE,            // MemInc
        DMA_PDATAALIGN_HALFWORD,    // PeriphDataAlignment
        DMA_MDATAALIGN_HALFWORD,    // MemDataAlignment
        DMA_NORMAL,                 // Mode
        DMA_PRIORITY_HIGH,          // Priority
        DMA_FIFOMODE_DISABLE,       // FIFOMode
        DMA_FIFO_THRESHOLD_FULL,    // FIFOThreshold
        DMA_MBURST_SINGLE,          // MemBurst
        DMA_PBURST_SINGLE,          // PeriphBurst
    }
};


SPI_HandleTypeDef handleSPI =
{
    SPI1,
    {
        SPI_MODE_SLAVE,                 // Init.Mode
        SPI_DIRECTION_2LINES,           // Init.Direction
        SPI_DATASIZE_8BIT,              // Init.DataSize
        SPI_POLARITY_HIGH,              // Init.CLKPolarity
        SPI_PHASE_1EDGE,                // Init.CLKPhase
        SPI_NSS_SOFT,                   // Init.NSS
        SPI_BAUDRATEPRESCALER_2,        // Init.BaudRatePrescaler
        SPI_FIRSTBIT_MSB,               // Init.FirstBit
        SPI_TIMODE_DISABLED,            // Init.TIMode
        SPI_CRCCALCULATION_DISABLED,    // Init.CRCCalculation
        7                               // InitCRCPolynomial
    }
};


ADC_HandleTypeDef handleADC;


DAC_HandleTypeDef handleDAC =
{
    DAC
};


volatile BitField gBF =
{
    0,  // numDrawingSignals

    // FPGA
    0,  // FPGAneedAutoFind
    0,  // FPGAinProcessingOfRead
    0,  // FPGAfirstAfterWrite

    // Flash drive
    1,  // cursorInDirs
    1,  // needRedrawFileManager



    0,  // alreadyLoadSettings


    0,  // showHelpHints

    0,  // showDebugMenu

    0,  // tuneTime

    GRID_BOTTOM, // topMeasures

    0,  // disable power
    0,  // временный показ строки меню навигации
    0,  // needSetOrientation
    0,  // cableEthIsConnected

    0,  // panelControlReceive
    0,  // needToMountFlash

    0,  // consoleInPause
    0   // needStopAfterReadFrameP2P
};


GMemory gMemory =
{
    0,
    0,
    0,
    0,  // runningFPGAbeforeSmallButtons
    0,  // exitFromROMtoRAM
    0,  // после установки имени и сохранения на флешку возвращаемся в основное рабочее окно
    0,  // needForSaveToFlashDrive
    1   // recordSaveToRAM
};


StateFPGA gStateFPGA = 
{
    false,
    StateWorkFPGA_Stop,
    StateCalibration_None
};

const char *gStringForHint = 0;
void *gItemHint = 0;

void SetItemForHint(void *item)
{
    gStringForHint = 0;
    gItemHint = item;
}
