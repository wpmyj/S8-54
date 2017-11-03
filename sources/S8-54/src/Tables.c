#include "Log.h"
#include "Display/font/Font.h"
#include "Settings/Settings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Структура для описания диапазона масштаба по времени.
typedef struct
{
    const char * const name[2];    // Название диапазона в текстовом виде, пригодном для вывода на экран.
} TBaseStruct;


// Массив структур описаний масштабов по времени.
static const TBaseStruct tBases[TBaseSize] =
{
    {"1\x10нс",     "1\x10ns"},
    {"2\x10нс",     "2\x10ns"},
    {"5\x10нс",     "5\x10ns"},
    {"10\x10нс",    "10\x10ns"},
    {"20\x10нс",    "20\x10ns"},
    {"50\x10нс",    "50\x10ns"},
    {"0.1\x10мкс",  "0.1\x10us"},
    {"0.2\x10мкс",  "0.2\x10us"},
    {"0.5\x10мкс",  "0.5\x10us"},
    {"1\x10мкс",    "1\x10us"},
    {"2\x10мкс",    "2\x10us"},
    {"5\x10мкс",    "5\x10us"},
    {"10\x10мкс",   "10\x10us"},
    {"20\x10мкс",   "20\x10us"},
    {"50\x10мкс",   "50\x10us"},
    {"0.1\x10мс",   "0.1\x10ms"},
    {"0.2\x10мс",   "0.2\x10ms"},
    {"0.5\x10мс",   "0.5\x10ms"},
    {"1\x10мс",     "1\x10ms"},
    {"2\x10мс",     "2\x10ms"},
    {"5\x10мс",     "5\x10ms"},
    {"10\x10мс",    "10\x10ms"},
    {"20\x10мс",    "20\x10ms"},
    {"50\x10мс",    "50\x10ms"},
    {"0.1\x10с",    "0.1\x10s"},
    {"0.2\x10с",    "0.2\x10s"},
    {"0.5\x10с",    "0.5\x10s"},
    {"1\x10с",      "1\x10s"},
    {"2\x10с",      "2\x10s"},
    {"5\x10с",      "5\x10s"},
    {"10\x10с",     "10\x10s"}
};

/// Таблица символов алфавита и спецсимволов для ввода текста
const char * const symbolsAlphaBet[] =
{
    /* 0x00 */ "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", /* 0x19 */
    /* 0x1a */ " ", "_", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", /* 0x26 */
    /* 0x27 */ "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", /* 0x40 */
    //         0x41  0x42  0x43  0x44  0x45  0x46   0x47   - это порядковый номер элемента в этом массиве
    /* 0x41 */ "%y", "%m", "%d", "%H", "%M", "%S", "%N" /* 0x47 */
    //         0x01  0x02  0x03  0x04  0x05  0x06   0x07   - под этими значениями элементы хранятся в set.memory.fileNameMask
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ENumSignalsInSec Tables_ENumSignalsInSecToENUM(int numSignalsInSec)
{
    if(numSignalsInSec == 1)
    {
        return ENumSignalsInSec_1;
    }
    else if(numSignalsInSec == 2)
    {
        return ENumSignalsInSec_2;
    }
    else if(numSignalsInSec == 5)
    {
        return ENumSignalsInSec_5;
    }
    else if(numSignalsInSec == 10)
    {
        return ENumSignalsInSec_10;
    }
    else if(numSignalsInSec == 25)
    {
        return ENumSignalsInSec_25;
    }
    LOG_ERROR_TRACE("Число сигналов в секунду равно %d", numSignalsInSec);
    return ENumSignalsInSec_1;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Tables_ENUMtoENumSignalsInSec(ENumSignalsInSec numSignalsInSec)
{
    static const int fps[] = {25, 10, 5, 2, 1};
    if(numSignalsInSec <= ENumSignalsInSec_1)
    {
        return fps[numSignalsInSec];
    }
    LOG_ERROR_TRACE("Неправильный параметр %d", (int)numSignalsInSec);
    return 1;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *Tables_GetTBaseString(TBase tBase)
{
    return tBases[tBase].name[LANG];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *Tables_GetTBaseStringEN(TBase tBase)
{
    return tBases[tBase].name[English];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *Tables_RangeNameFromValue(Range range)
{
    static const char * const names[RangeSize] =
    {
        "Range_2mV",
        "Range_5mV",
        "Range_10mV",
        "Range_20mV",
        "Range_50mV",
        "Range_100mV",
        "Range_200mV",
        "Range_500mV",
        "Range_1V",
        "Range_2V",
        "Range_5V"
    };
    return names[range];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int Tables_GetNumChannel(Channel ch)
{
    return ch == A ? 1 : 2;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawStr(int index, int x, int y)
{
    const char *str = symbolsAlphaBet[index];
    if (index == INDEX_SYMBOL)
    {
        painter.FillRegion(x - 1, y, Font_GetLengthText(str), 9, Color::FLASH_10);
    }
    painter.DrawText(x, y, symbolsAlphaBet[index], index == INDEX_SYMBOL ? Color::FLASH_01 : gColorFill);
}
