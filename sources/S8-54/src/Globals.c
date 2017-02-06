#include "defines.h"
#include "Globals.h"
#include "Display/DisplayTypes.h"
#include "Settings/SettingsMemory.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    // FPGA
    0,  // FPGAneedAutoFind
    0,  // FPGAtemporaryPause
    0,  // FPGAinProcessingOfRead
    1,  // FPGAcanReadData
    0,  // FPGAcritiacalSituation
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
    0,  // ��������� ����� ������ ���� ���������
    0,  // needSetOrientation
    0,  // cableEthIsConnected
    0,  // panelControlReceive
    0,  // needToMountFlash

    0   // soundIsBeep
};


GMemory gMemory =
{
    0,
    0,
    0,
    0,  // runningFPGAbeforeSmallButtons
    0,  // exitFromIntToLast
    0,  // ����� ��������� ����� � ���������� �� ������ ������������ � �������� ������� ����
    0   // needForSaveToFlashDrive
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

uint8           *gDataA = 0;
uint8           *gDataB = 0;
DataSettings    *gDSet = 0;

uint8           *gDataAmemInt = 0;
uint8           *gDataBmemInt = 0;
DataSettings    *gDSmemInt = 0;

uint8           *gDataAmemLast = 0;
uint8           *gDataBmemLast = 0;
DataSettings    *gDSmemLast = 0;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NumBytesInChannel(DataSettings *ds)
{
    return FPGA_NUM_POINTS_2_NumPoints((NumPoinstFPGA)ds->indexLength);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int NumBytesInData(DataSettings *ds)
{
    return NumBytesInChannel(ds) * (ds->enableChA + ds->enableChB);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int NumPointsInChannel(DataSettings *ds)
{
    if (ds->peackDet == 0)
    {
        return NumBytesInChannel(ds);
    }
    
    return NumBytesInChannel(ds) * 2;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
uint8 *AddressChannel(DataSettings *ds, Channel ch)
{
    if (ch == A && ds->enableChA)
    {
        return ds->addrData;
    }

    if (ch == B && ds->enableChB)
    {
        return ds->enableChA ? (ds->addrData + NumBytesInChannel(ds)) : ds->addrData;
    }

    return 0;
}

DataSettings gDatas[NUM_DATAS];
uint8 gDataAve[NumChannels][FPGA_MAX_POINTS]; 

int gAddNStop = 0;
