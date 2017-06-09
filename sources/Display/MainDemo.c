//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
/****************************************************************************
 * Microchip Graphics Library Demo Application
 * This program shows how to use the Graphics Primitives Layer.
 *****************************************************************************/

/*

    Для сборки испльзовать MPLAB IDE v8.92 и компилятор xc16-v1.26

    Выводы.

    19 - RE9 - 1
    20 - RB5 - 0
*/
 
#include "MainDemo.h"
#include <p24FJ256DA210.h>
#include "PPS.h"
#include "Graphics/mchpGfxDrv.h"
#include "Configs/HWP_DA210_BRD_16PMP_QVGAv1.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define AUTORELOAD  // Перезагрузка при зависании


_CONFIG1( WDTPS_PS128 & FWPSA_PR128 & ALTVREF_ALTVREDIS & ICS_PGx2 & GWRP_OFF & JTAGEN_OFF & 
#ifdef AUTORELOAD
    FWDTEN_ON
#else
    FWDTEN_OFF
#endif
& GCP_OFF) //// & WINDIS_OFF &JTAGEN_OFF
_CONFIG2( POSCMOD_XT & IOL1WAY_OFF & OSCIOFNC_OFF & OSCIOFNC_OFF & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
_CONFIG3( WPFP_WPFP255 & SOSCSEL_SOSC & WUTSEL_LEG & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM) //ALTPMP_ALTPMPEN & 


#define OUT_PIN_PPS_RP0				RPOR0bits.RP0R


typedef union
{
  BYTE b[4];
  SHORT s[2];
} DATA;


typedef void(*pFuncVV)(void);


void ReadNextCommand(void);     // Читает байты следующей команды в буфер codeBuffer
void InitializeBoard(void);  
void InitializePWM(void);
void InitializePMP_Common(void);
void InitializePMP_COM(void);   // Для коммуникации с внешним устройством
void InitializePMP_RAM(void);   // Обращение к RAM
BYTE NextByte(void);
SHORT NextShort(void);
static void SetOrientation(void);
static void PMP_SetOrientation(void);
static void PMP_GetPixel(SHORT x, SHORT y);


#define SIZE_BUFFER 1024 * 11
volatile static BYTE codeBuffer[SIZE_BUFFER] =
{
    9, 0, 0, 0,                 // 4 SET_PALETTE 0, 0
    9, 1, 255, 255,             // 4 SET_PALETTE 1, 65535
    1, 0,                       // 2 SET_COLOR 0
    2, 0, 0,  0,  63, 1, 239,   // 7 FILL_REGION 0, 0, 319, 239
    1, 1,                       // 2 SET_COLOR 1
    2, 20, 0, 20, 100, 0, 100,  // 7 FILL_REGION 20, 20, 100, 100
    3                           // 1 END_SCENE
};


int pc = 0; // Program counter - PC указывает номер текущего байта для исполнения


#include "fonts/font5display.inc"
#include "fonts/font8display.inc"
#include "fonts/fontUGOdisplay.inc"
#include "fonts/fontUGO2display.inc"


const BYTE* fonts[4];
BYTE curFont[3080];


union PMP_Color
{
   BYTE b[2];
   WORD w;
};


union PWM
{
  BYTE b[2];
  SHORT s;
};


union PixelColor
{
   UINT16 u[2];
   DWORD d;
};


BYTE curColor;


typedef struct
{
    pFuncVV func;
    int numBytes;
} StructCommand;

typedef enum
{
    Directly = 254,
    Variable = 255
} LengthCommand;

const StructCommand comArray[] =
{
    // -1 - переменная длина команды
    // -2 - команда выполняется непосредственно, без занесения в буфер
    {0, 0},
    {PMP_SetColorX,         2},         // 1
    {PMP_FillRectX,         7},         // 2
    {PMP_InvalidateX,       1},         // 3
    {PMP_HorLineX,          6},         // 4
    {PMP_VerLineX,          5},         // 5
    {PMP_PutPixelX,         4},         // 6
    {PMP_DrawSignalLines,   284},       // 7  Нарисовать сигнал линиями
    {PMP_DrawText,          -1},        // 8
    {PMP_SetPalColorL,      4},         // 9
    {PMP_SetFont,           2},         // 10
    {0, 0},                             // 11 Не используется
    {0, 0},                             // 12 Не используется
    {PMP_DrawVLinesArray,   -1},        // 13
    {PMP_DrawSignalDot,     284},       // 14 Нарисовать сигнал точками
    {PMP_Brightness,        3},         // 15
    {0, 0},                             // 16 Не используется в С8-53 и С8-54. Возможно, в ней ошибка
    {PMP_DrawMultiHPointLine, -1},      // 17
    {PMP_DrawMultiVPointLine, -1},      // 18
    {0, 0},                             // 19
    {PMP_LoadImage,         0},         // 20
    {(pFuncVV)PMP_GetPixel, -2},        // 21 Получть точку. Команда не заносится в буфер и выполняется непосредственно.
    {PMP_RunBuffer,         -2},        // 22 Выполнить команды, находящиеся в буфере. Команда не заносится с буфер и выполнятся непосредственно.
    {PMP_SetReInit,         2},         // 23 Установить режим перезагрузки монитора в случае зависания. 1 - перезагружать (режим по умолчанию) 2 - не перезагружать
    {PMP_SetOrientation,    2},         // 24 Установить ориентацию дисплея. Параметр 0 или 1
    {PMP_DrawLine,          5}          // 25 Нарисовать произвольную линию
};

BYTE *dataRun = 0;


#define EXTERNAL_RESOURCE_SIZE 0x40000L
#define CS_START_ADDRESS 0x200000L

__eds__ BYTE pmp_data[EXTERNAL_RESOURCE_SIZE] __attribute__((eds, address(CS_START_ADDRESS), noload));
__eds__ BYTE* pointer;

static BYTE orientation = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    InitializeBoard();
    InitializePWM();
    InitializePMP_Common();
    InitializePMP_COM();

    PORTBbits.RB1 = 0;

    fonts[0] = font5display;
    fonts[1] = font8display;
    fonts[2] = fontUGOdisplay;
    fonts[3] = fontUGO2display;

    SetPaletteDirect(0, 0);     // WARN Без этой страки функция рисования текста вызывает артефакт при рисовании - цвета меняются, экран темнеет процентов на 50

    PORTBbits.RB1 = 1;

    pointer = pmp_data;

    while(1)
    {
        if(_IBF == 1)               // Input buffer full
        {
            PORTBbits.RB1 = 0;      // RB = 0 - признак того, что дисплей занят и ему ничего слать не нужно
            ReadNextCommand();
            PMSTATbits.IBOV = 0;    // Input buffer overflow
            PORTBbits.RB1 = 1;      // RB = 1 - дисплей освободился, можно слать следующую команду
#ifdef AUTORELOAD
            ClrWdt();
#endif
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void InitializeBoard(void)
{  
    ANSBbits.ANSB0 = 0;
    ANSBbits.ANSB1 = 0;
    Nop();

    // Data Direction Register - determines whether the pin is an input or an output. If the data direction bit is a '1', the the pin is an input.
    TRISBbits.TRISB1 = 0;   // RB1 - output . Pin24 - SEL_O. Признак того, что запись в дисплей разрешена

    TRISBbits.TRISB0 = 0;   // RB0 - output. Pin25 - SEL_I. Переключает режим управления шиной - внешним устройством (для приёма команд) и PIC24 - для записи/чтения RAM

    TRISBbits.TRISB5 = 0;   //-\ Используются для управление поворотом монитора.
    TRISEbits.TRISE9 = 0;   //-/ Должны быть в противоположном состоянии

    SetOrientation();

	ResetDevice();
    SetColor(0);
    ClearDevice();

    TRISBbits.TRISB0 = 0;
}    


//------------------------------------------------------------------------------------------------------------------------------------------------------
void InitializePWM(void)
{
    iPPSOutput(OUT_PIN_PPS_RP11,OUT_FN_PPS_OC1);    // RPOR5bits.RP11R = 18 (RPn tied to Output Compare 1)

    OC1CON1 = 0x0000;   // Turn off Output Compare 1 Module
    OC1R = 0x1620;      // Initialize Compare Register1
    OC1RS = 0x0320;     // Initialize Secondary Compare Register1
    OC1CON1 = 0x0006;   // Load new compare mode to OC1CON*/
    PR2 = 0x063F;       // =0x004C;//// 
    T2CONbits.TON = 1;  // Initialize PR2 with 0x004C
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void InitializePMP_Common(void)
{
    PMCON1bits.PMPEN = 0;

    PMCS1CFbits.SM = 0;     // read and write strobes on separate lines

    PMCON3bits.PTWREN = 1;	// Write/Enable Strobe Port Enable bit
                            // PMWR/PMENB port is enabled

    PMCON3bits.PTRDEN = 1;	// Read/Write Strobe Port Enable bit
                            // PMRD/PMWR port is enabled

    PMCS2CFbits.CSDIS = 1;      // disable CS2 functionallity

    PMCS1CFbits.PTSZ = 0b00;    // data bus width is 8-bit

    // For communication
    PMCON1bits.IRQM1 = 0;   // IRQM<1:0> Interrupt Request Mode bits
    PMCON1bits.IRQM0 = 0;   // No interrupt is generated

    // For RAM
    PMCS1MDbits.ACKM = 0b00;        // PMACK is not used
    PMCS1MDbits.DWAITB = 0b01;      // data setup before read/write strobe is 1 + 1 / 4 Tcy
    PMCS1MDbits.DWAITM = 0b0100;    // read/write strobe width 3 + 1 / 2 Tcy for write
                                    // 3 + 3 / 4 Tcy for read
    PMCS1MDbits.DWAITB = 0b01;      // data hold after read/write strobe is 1 + 1 / 4 Tcy for write
                                    // 1 Tcy for read

    PMCON1bits.PMPEN = 1;

    //RCONbits.WDTO = 1;              // Включаем сброс по таймеру
    //WDTPS_PS32
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void InitializePMP_COM(void)
{
    PMCON1bits.PMPEN = 0;

    //PMCON1 - EPMP control register 1

    PMCON1bits.PSIDL = 0;   // Stop in Idle Mode bit
                            // Continue module operation in Idle mode

    PMCON1bits.CSF1 = 1;    // CS<1:0> Chip Select Function Bits
    PMCON1bits.CSF0 = 0;    
                            // PMCS1  function as chip select !!!!!!!!!!!! and PMCS2
                            // PMA<15> used for Chip Select 2, PMA<14> used for Chip Select 1

    PMCON1bits.MODE = 1;    // MODE<1:0> Parallel port mode select bits
                            // Buffered PSP; pins used are PMRD, PMWR, PMCS and PMD<7:0>

    // PMCON3 - EPMP control register 3

    PMCON3bits.PTBE0EN = 0; // Low Nibble/Byte Enable Port Enable bit
                            // PMBE0 port is disabled

    PMCON3bits.PTBE1EN = 0;	// Hight Nibble/Byte Enable Port Enable bit
                            // PMBE1 port is disabled

    // PMCS1CF - Chip select 1 configuration register

    PMCS1CFbits.CSP	= 0;	// Chip Select 1 Polarity bit
                            // Active-low

    PMCS1CFbits.WRSP = 0;	// Chip Select 1 Write Strobe Polarity bit
                            // For Slave mode and master mode when SM = 0:
                            // Write strobe active-low

    PMCS1CFbits.RDSP = 0;	// Chip Select 1 Read Strobe Polarity bit
                            // For slave mode and master mode when SM=0:
                            // Read strobe active-low

    PMCS1CFbits.CSPTEN = 1; // PMCS1 Port Enable bit
                            // PMCS1 port is enabled

    PMCS1CFbits.CSDIS = 1;  // Chip Select 1 Disable bit
                            // Disable the Chip Select 1 functionality

    PMCON1bits.PMPEN = 1;   // Parallel master port enable bit
                            // Enabled
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void InitializePMP_RAM(void)
{
    // PMCON1BITS
    PMCON1bits.PMPEN = 0;

    PMCON1bits.ADRMUX = 0b00;   // Address is demultiplexed
    PMCON1bits.MODE = 0b11;     // master mode
    PMCON1bits.CSF = 0b00;      // PMCS1 pin used for chip select 1
                                // PMCS2 pin used for chip select 2
    PMCON1bits.BUSKEEP = 0;     // bus keeper is not used
    PMCS1BS = 0x2000;           // set CS1 start address to the start of the EPMP EDS space (0x200000)
    PMCS2BS = 0x2400;           // set CS1 end addrss to cover a 256 KBytes region for CS1 (0x240000 EPMP EDS address)

    PMCON3bits.PTEN16 = 1;      // enable PMA16
    PMCON3bits.PTEN17 = 1;      // enable PMA17

    PMCON4 = 0xffff;            // enabled PMA<15:0> ports

    PMCS1CFbits.CSDIS = 0;      // enable CS1
    PMCS1CFbits.CSP = 0;        // CS1 active low
    PMCS1CFbits.CSPTEN = 1;     // enable CS1
    PMCS1CFbits.BEP = 0;        // byte enable active low
    PMCS1CFbits.WRSP = 0;       // write strobe active low
    PMCS1CFbits.RDSP = 0;       // read strobe active low

    PMCON1bits.PMPEN = 1;       // enable EPMP
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
#define READ_NEXT_4_BYTES    \
    PMSTATbits.IBOV = 0;    \
    PORTBbits.RB1 = 1;      \
    while (_IBF == 0) {};   \
    PORTBbits.RB1 = 0;      \
    data.s[0] = PMDIN1;     \
    data.s[1] = PMDIN2;

//------------------------------------------------------------------------------------------------------------------------------------------------------
void ReadNextCommand(void)
{
    DATA data;
    data.s[0] = PMDIN1;
    data.s[1] = PMDIN2;

    BYTE code = data.b[0];

    StructCommand cmd = comArray[code];

    int numBytes = cmd.numBytes;

    if (numBytes == -1)
    {
        if (code == 8)          // DrawText
        {
            memcpy((void*)&codeBuffer[pc], (void*)&data, 4);
            pc += 4;

            READ_NEXT_4_BYTES

            numBytes = data.b[0] + 1;
        }
        else if (code == 13)    // DrawVlinesArray
        {
            numBytes = (SHORT)data.b[3] * 2 + 4;
        }
        else if (code == 17)    // PMP_DrawMultiHPointLine
        {
            numBytes = 1 + data.b[1] + 5;
        }
        else if (code == 18)    // PMP_DrawMultiVPointsLine
        {
            numBytes = 1 + data.b[1] * 2 + 5;
        }
    }

    if ((pc + numBytes) >= SIZE_BUFFER)   // Если в буфере нет места для записи команды
    {
        PMP_RunBuffer();                // То выполним сначала команды из буфера
    }

    if (numBytes > 0)
    {    
        int readingBytes = 0;

        while (1)
        {
            memcpy((void*)&codeBuffer[pc + readingBytes], (void*)&data, 4);

            readingBytes += 4;

            if (readingBytes >= numBytes)
            {
                break;
            }

            READ_NEXT_4_BYTES
        }

        pc += numBytes;
    }
    else if (code == 21)    // GetPixel
    {
        SHORT x = (SHORT)data.b[1] | ((SHORT)(data.b[2]) << 8);
        SHORT y = data.b[3];
        PMP_GetPixel(x, y);
    }
    else if (code == 22)    // RunBuffer
    {
        PMP_RunBuffer();
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_SetColorX(void)
{
    BYTE color = NextByte();
    SetColor(color);
    SetColorN();
    curColor = color;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void PMP_SetOrientation()
{
    orientation = NextByte() & 0x01;
    SetOrientation();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_FillRectX(void)
{
    SHORT x = NextShort();
    SHORT y = NextByte();
    SHORT width = NextShort();
    SHORT height = NextByte();

    FillRectN(x, y, width, height);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_InvalidateX(void)
{
    MemoryTrans();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_SetReInit(void)
{

}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_VerLineX(void)
{
    SHORT x = NextShort();
    SHORT y1 = NextByte();
    SHORT y2 = NextByte();

    VerLineN(x, y1, y2);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_HorLineX(void)
{
    SHORT y = NextByte();
    SHORT x1 = NextShort();
    SHORT x2 = NextShort();

    HorLineN(x1, x2, y);
}

static int Sign(int x)
{
    if (x == 0)
    {
        return 0;
    }
    else if (x > 0)
    {
        return 1;
    }
    return -1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_DrawLine(void)
{
    SHORT x1 = NextShort();
    SHORT y1 = NextByte();
    SHORT x2 = NextShort();
    SHORT y2 = NextShort();

    if ((x2 - x1) == 0 && (y2 - y1) == 0)
    {
        ++x1;
    }
    int x = x1;
    int y = y1;
    int dx = (int)fabsf((float)(x2 - x1));
    int dy = (int)fabsf((float)(y2 - y1));
    int s1 = Sign(x2 - x1);
    int s2 = Sign(y2 - y1);
    int temp;
    int exchange = 0;
    if (dy > dx)
    {
        temp = dx;
        dx = dy;
        dy = temp;
        exchange = 1;
    }
    int e = 2 * dy - dx;
    int i = 0;
    for (; i <= dx; i++)
    {
        PutPixelN(x, y);
        while (e >= 0)
        {
            if (exchange)
            {
                x += s1;
            }
            else
            {
                y += s2;
            }
            e = e - 2 * dx;
        }
        if (exchange)
        {
            y += s2;
        }
        else
        {
            x += s1;
        }
        e = e + 2 * dy;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_PutPixelX(void)
{
    SHORT x = NextShort();
    SHORT y = NextByte();

    PutPixelN(x, y);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_DrawSignalLines(void)
{
    SHORT x = NextShort();
    SHORT yPrev = NextByte();

    int i = 0;

    for (i = 0; i < 280; i++)
    {
        SHORT y = NextByte();

        if (y == yPrev)
        {
            PutPixelN(x, y);
        }
        else if (y > yPrev)
        {
            VerLineN(x, y - 1, yPrev);
        }
        else
        {
            VerLineN(x, yPrev, y + 1);
        }

        yPrev = y;
        x++;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_DrawSignalDot(void)
{
    SHORT x = NextShort();

    int i = 0;

    for (i = 0; i < 281; i++)
    {
        PutPixelN(x++, NextByte());
    }
}


UXCHAR text[101];


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_DrawText(void)
{
    SHORT x = NextShort();
    SHORT y = NextByte();

    int numSymbols = NextByte();

    UXCHAR *pText = &text[0];

    int i = 0;

    for (i = 0; i < numSymbols; i++)
    {
        *pText = NextByte();
        pText++;
    }

    *pText = 0;

    TextOutN(x, y, &text[0], (WORD)(curFont));
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_SetPalColorL(void)
{
    BYTE number = NextByte();

    union PMP_Color color;
    
    color.b[0] = NextByte();
    color.b[1] = NextByte();
   
    if (IsPaletteEnabled() != 0)
    {
        DisablePalette();
    }

    SetPaletteBpp(COLOR_DEPTH);
  	mSetPaletteColor(color.w, number);  //_VMRGN
	EnablePalette();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_SetFont(void)
{
    int numFont = NextByte();

    memcpy(curFont, fonts[numFont], 3080);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_DrawVLinesArray(void)
{
    SHORT x = NextShort();
    SHORT num = NextByte();

    int i = 0;

    for (i = 0; i < num; i++)
    {
        SHORT y1 = NextByte();
        SHORT y2 = NextByte();
        VerLineN(x, y1, y2);
        x++;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_Brightness(void) // функция обработки команды Invalidate();
{  
    union PWM pwm;
   
    pwm.b[0] = NextByte();
    pwm.b[1] = NextByte();

    OC1R = pwm.s;//620;//pwm.b[0]; // Initialize Compare Register1
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_DrawMultiHPointLine(void)
{
    /*
    Рисует numLines горизонтальных линий, состоящих из точек, расстояние между точками delta. Количество точек в линии - count
    1 - numLines
    2,3 - x
    4 - count
    5 - delta
    6,...,6+numLines - y[]
    */

    SHORT numLines = NextByte();
    SHORT x = NextShort();
    SHORT numDots = NextByte();
    BYTE delta = NextByte();

    BYTE pixels[160] = {0};

    SHORT numPixels = numDots * delta;
    BYTE color = curColor << 4;

    SHORT i = 0;

    if ((delta % 2) == 0)
    {
        for (i = 1; i < numDots; i++)
        {
            pixels[((delta / 2) * i) - 1] = color;
        }
    }
    else
    {
        for (i = 1; i < numDots; i++)
        {
            pixels[((delta / 2 + 1) * i) - 1] = color;
        }
    }

    // Рисуем все горизонтальные линии
    for (i = 0; i < numLines; i++)
    {
        SHORT y = NextByte();
        PutPixelN(x, y);
        mSetArea(x + 1, y, numPixels, 1, (WORD)pixels, curColor);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_DrawMultiVPointLine(void)
{
    /*
    Рисует numLines вертикальных линий, состоящих из точек.
    1 - numLines
    2 - y
    3 - count
    4 - delta
    5 - 0
    6,6 + 1,...,6 + numLines * 2 - x[]
    */

    BYTE pixels[160] = {0};

    SHORT numLines = NextByte();
    SHORT y = NextByte();
    SHORT numDots = NextByte();
    BYTE delta = NextByte();
    NextByte();

    SHORT numPixels = numDots * delta;
    BYTE color = curColor << 4;

    SHORT i = 0;

    if ((delta % 2) == 0)
    {
        for (i = 1; i < numDots; i++)
        {
            pixels[((delta / 2) * i) - 1] = color;
        }
    }
    else
    {
        for (i = 1; i < numDots; i++)
        {
            pixels[((delta / 2 + 1) * i) - 1] = color;
        }
    }

    // Рисуем все вертикальные линии
    for (i = 0; i < numLines; i++)
    {
        SHORT x = NextShort();
        PutPixelN(x, y);
        mSetArea(x, y + 1, 1, numPixels, (WORD)pixels, curColor);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_LoadImage(void)
{
    /*
    Загружает картинку
    1,2 - x
    3 - y
    4,5 - width
    6 - height
    7 - 0
    8,...,8 + width * height / 2 - байты изображения
    */

    BYTE pixels[160];

    SHORT x = NextShort();
    SHORT y = NextByte();
    SHORT width = NextShort();
    SHORT height = NextByte();
    NextByte();

    SHORT row = 0;
    SHORT col = 0;

    for (col = 0; col < height; col++)
    {
        for (row = 0; row < width; row++)
        {
            pixels[row] = NextByte();
        }
        mSetAreaN(x, y + col, width * 2, 1, (WORD)pixels);
    }
    
    // Очистим буфер для пиксеелей
    for (row = 0; row < width; row++)
    {
        pixels[row] = 0;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_GetPixel(SHORT x, SHORT y)
{
    union PixelColor pixel;

	pixel.d = vGetPixel(x, y);
    
    PMDOUT1 = pixel.u[0];
    PMDOUT2 = pixel.u[1];
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void PMP_RunBuffer(void)
{
    dataRun = (BYTE*)(&codeBuffer[0]);

    BYTE *dataEnd = (BYTE*)(&codeBuffer[pc]);

    while (dataRun < dataEnd)
    {
        StructCommand cmd = comArray[*dataRun++];
        cmd.func();
    }

    pc = 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
BYTE NextByte(void)
{
    BYTE retValue = *dataRun;
    dataRun++;
    return retValue;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
SHORT NextShort(void)
{
    union PWM retValue;

    retValue.b[0] = *dataRun++;
    retValue.b[1] = *dataRun++;

    return retValue.s;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void SetPaletteDirect(BYTE numColor, SHORT valueColor)
{
    if (IsPaletteEnabled() != 0)
    {
        DisablePalette();
    }

    SetPaletteBpp(COLOR_DEPTH);
    mSetPaletteColor(valueColor, numColor);
    EnablePalette();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void SetColorDirect(BYTE numColor)
{
    SetColor(numColor);
    SetColorN();
    curColor = numColor;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FillRegion(SHORT x, SHORT y, SHORT width, SHORT height)
{
    FillRectN(x, y, width, height);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void EndScene(void)
{
    MemoryTrans();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void SetFontDirect(BYTE numFont)
{
    memcpy(curFont, fonts[numFont], 3080);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawTextDirect(SHORT x, SHORT y, UXCHAR *text)
{
    TextOutN(x, y, text, (WORD)(curFont));
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void TestFunc(void)
{
    SetColorDirect(0);
    FillRegion(0, 0, 319, 239);

    BYTE numLines = 255;

    codeBuffer[0] = 0;
    codeBuffer[1] = 0;
    codeBuffer[2] = numLines;

    int i = 0;


    codeBuffer[3] = rand() % 239;
    codeBuffer[4] = 239;

    for (i = 1; i < numLines; i++)
    {
        int yPrev = codeBuffer[3 + i * 2 - 2];
        int y = yPrev - 5 + (rand() % 10);
        while (y < 0 || y > 239)
        {
            y = yPrev - 5 + (rand() % 10);
        }
        codeBuffer[3 + i * 2] = y;
        codeBuffer[3 + i * 2 + 1] = 239;
    }


    for (i = 0; i < numLines; i++)
    {
        codeBuffer[3 + i * 2] = 40;
        codeBuffer[3 + i * 2 + 1] = 200;
    }

    dataRun = (BYTE*)&codeBuffer[0];

    SetColorDirect(1);

    PMP_DrawVLinesArray();

    for (i = 0; i < 32760; i++)
    {
        int j = 0;
        for (j = 0; j < 32760; j++)
        {
            int k = 0;
            for (k = 0; k < 32760; k++)
            {

            }
        }
    }

    EndScene();
}

static void SetOrientation()
{
    PORTBbits.RB5 = orientation;
    PORTEbits.RE9 = (orientation == 0) ? 1 : 0;
}
