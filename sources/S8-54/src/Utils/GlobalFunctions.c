#include "defines.h"
#include "GlobalFunctions.h"
#include "Log.h"
#include "Display/Symbols.h"
#include "FPGA/fpgaTypes.h"
#include "Settings/Settings.h"
#include "Utils/Debug.h"
#include "Utils/Dictionary.h"
#include "Utils/Math.h"
#include <string.h>
#include <stdlib.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void *AllocMemForChannelFromHeap(Channel ch, DataSettings *ds)
{
    int numBytes = RequestBytesForChannel(ch, ds);
    if(numBytes)
    {
        return malloc(numBytes);
    }
    return 0;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int RequestBytesForChannel(Channel ch, DataSettings *ds)
{
    ENumPointsFPGA numBytes;
    PeakDetMode peakDet;

    if(ds)
    {
        numBytes = (ENumPointsFPGA)ENUM_BYTES(ds);
        peakDet = PEAKDET(ds);
    }
    else
    {
        numBytes = NumPoints_2_ENumPoints(BytesInChannel(ds));
        peakDet = SET_PEAKDET;
    }

    if((numBytes == FNP_32k) || (numBytes == FNP_16k && peakDet == PeakDet_Enable))
    {
        if(ch == A)
        {
            return FPGA_MAX_POINTS * 2;
        }
        return 0;
    }

    return FPGA_MAX_POINTS;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
char *FloatFract2String(float value, bool alwaysSign, char bufferOut[20])
{
    return Float2String(value, alwaysSign, 4, bufferOut);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int NumDigitsInIntPart(float value)
{
    float fabsValue = fabsf(value);

    int numDigitsInInt = 0;
    if (fabsValue >= 10000)
    {
        numDigitsInInt = 5;
    }
    else if (fabsValue >= 1000)
    {
        numDigitsInInt = 4;
    }
    else if (fabsValue >= 100)
    {
        numDigitsInInt = 3;
    }
    else if (fabsValue >= 10)
    {
        numDigitsInInt = 2;
    }
    else
    {
        numDigitsInInt = 1;
    }

    return numDigitsInInt;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Float2String(float value, bool alwaysSign, int numDigits, char bufferOut[20])
{
    bufferOut[0] = 0;
    char *pBuffer = bufferOut;

    if(value == ERROR_VALUE_FLOAT)
    {
        strcat(bufferOut, ERROR_STRING_VALUE);
        return bufferOut;
    }
    if(!alwaysSign)
    {
        if(value < 0)
        {
            *pBuffer = '-';
            pBuffer++;
        }
    }
    else
    {
        *pBuffer = value < 0 ? '-' : '+';
        pBuffer++;
    }
    char format[] = "%4.2f\0\0";

    format[1] = (char)numDigits + 0x30;

    int numDigitsInInt = NumDigitsInIntPart(value);

    format[3] = (char)((numDigits - numDigitsInInt) + 0x30);
    if(numDigits == numDigitsInInt)
    {
        format[5] = '.';
    }

    snprintf(pBuffer, 15, format, fabsf(value));

    float val = (float)atof(pBuffer);

    if (NumDigitsInIntPart(val) != numDigitsInInt)
    {
        numDigitsInInt = NumDigitsInIntPart(val);
        format[3] = (char)((numDigits - numDigitsInInt) + 0x30);
        if (numDigits == numDigitsInInt)
        {
            format[5] = '.';
        }
        snprintf(pBuffer, 20, format, value);
    }

    bool signExist = alwaysSign || value < 0;
    while((uint)strlen(bufferOut) < (uint)numDigits + (signExist ? 2 : 1))
    {
        strcat(bufferOut, "0");
    }

    return bufferOut;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Int2String(int value, bool alwaysSign, int numMinFields, char buffer[20])
{
    const int SIZE = 20;
    char format[SIZE] = "%";
    snprintf(&(format[1]), SIZE, "0%d", numMinFields);
    strcat(format, "d");
    if(alwaysSign && value >= 0)
    {
        buffer[0] = '+';
        snprintf(buffer + 1, SIZE - 1, format, value);
    }
    else
    {
        snprintf(buffer, SIZE, format, value);
    }
    return buffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool String2Int(char *str, int *value)  
{
    int sign = str[0] == '-' ? -1 : 1;
    if (str[0] < '0' || str[0] > '9')
    {
        str++;
    }
    int length = strlen(str);
    if (length == 0)
    {
        return false;
    }
    *value = 0;
    int pow = 1;
    for(int i = length - 1; i >= 0; i--)
    {
        int val = str[i] & (~(0x30));
        if(val < 0 || val > 9)
        {
            return false;
        }
        *value += val * pow;
        pow *= 10;
    }
    if(sign == -1)
    {
        *value *= -1;
    }
    return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Bin2String(uint8 value, char buffer[9])
{
    for(int bit = 0; bit < 8; bit++)
    {
        buffer[7 - bit] = GetBit(value, bit) ? '1' : '0';
    }
    buffer[8] = '\0';
    return buffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Bin2String16(uint16 value, char valBuffer[19])
{
    char buffer[9];
    strcpy(valBuffer, Bin2String(value >> 8, buffer));
    strcpy((valBuffer[8] = ' ', valBuffer + 9), Bin2String((uint8)value, buffer));
    valBuffer[18] = '\0';
    return valBuffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Hex8toString(uint8 value, char buffer[3], bool upper)
{
    sprintf(buffer, upper ? "%02X" : "%02x", value);
    return buffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Hex16toString(uint16 value, char buffer[5], bool upper)
{
    sprintf(buffer, upper ? "%04X" : "%04x", value);
    return buffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Hex32toString(uint32 value, char buffer[9], bool upper)
{
    sprintf(buffer, upper ? "%08X" : "%08x", value);
    return buffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Voltage2String(float voltage, bool alwaysSign, char buffer[20])
{
    buffer[0] = 0;
    const char *suffix = DICT(D10kV);
    if(voltage == ERROR_VALUE_FLOAT)
    {
        strcat(buffer, ERROR_STRING_VALUE);
        return buffer;
    }
    else if(fabsf(voltage) + 0.5e-4f < 1e-3f)
    {
        suffix = DICT(D10uV);
        voltage *= 1e6f;
    }
    else if(fabsf(voltage) + 0.5e-4f < 1.0f)
    {
        suffix = DICT(D10mV);
        voltage *= 1e3f;
    }
    else if(fabsf(voltage) + 0.5e-4f < 1000.0f)
    {
        suffix = DICT(D10V);
    }
    else
    {
        voltage *= 1e-3f;
    }

    char bufferOut[20];

    Float2String(voltage, alwaysSign, 4, bufferOut);
    
    strcat(buffer, bufferOut);
    strcat(buffer, suffix);
    return buffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Time2String(float time, bool alwaysSign, char buffer[20])
{
    return Time2StringAccuracy(time, alwaysSign, buffer, 4);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Time2StringAccuracy(float time, bool alwaysSign, char buffer[20], int numDigits)
{
    buffer[0] = 0;
    const char *suffix = DICT(Ds);

    float fabsTime = fabsf(time);

    if (time == ERROR_VALUE_FLOAT)
    {
        strcat(buffer, ERROR_STRING_VALUE);
        return buffer;
    }
    else if (fabsTime + 0.5e-10f < 1e-6f)
    {
        suffix = DICT(Dns);
        time *= 1e9f;
    }
    else if (fabsTime + 0.5e-7f < 1e-3f)    
    {
        suffix = DICT(Dus);
        time *= 1e6f;
    }
    else if (fabsTime + 0.5e-3f < 1.0f)
    {
        suffix = DICT(Dms);
        time *= 1e3f;
    }

    char bufferOut[20];
    strcat(buffer, Float2String(time, alwaysSign, numDigits, bufferOut));
    strcat(buffer, suffix);

    return buffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Phase2String(float phase, bool empty, char bufferOut[20])
{
    const int SIZE = 20;
    char buffer[SIZE];
    snprintf(bufferOut, SIZE, "%s\xa8", Float2String(phase, false, 4, buffer));
    return bufferOut;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Freq2String(float freq, bool alwaysSign, char bufferOut[20])
{
    return Freq2StringAccuracy(freq, bufferOut, 4);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Freq2StringAccuracy(float freq, char bufferOut[20], int numDigits)
{
    bufferOut[0] = 0;
    const char *suffix = DICT(DHz);
    if (freq == ERROR_VALUE_FLOAT)
    {
        strcat(bufferOut, ERROR_STRING_VALUE);
        return bufferOut;
    }
    if (freq >= 1e6f)
    {
        suffix = DICT(DMHz);
        freq /= 1e6f;
    }
    else if (freq >= 1e3f)
    {
        suffix = DICT(DkHz);
        freq /= 1e3f;
    }
    char buffer[20];
    strcat(bufferOut, Float2String(freq, false, numDigits, buffer));
    strcat(bufferOut, suffix);
    return bufferOut;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *Float2Db(float value, int numDigits, char bufferOut[20])
{
    bufferOut[0] = 0;
    char buffer[20];
    strcat(bufferOut, Float2String(value, false, numDigits, buffer));
    strcat(bufferOut, "дЅ");
    return bufferOut;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool IntInRange(int value, int min, int max)
{
    return (value >= min) && (value <= max);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float MaxFloat(float val1, float val2, float val3)
{
    float retValue = val1;
    if(val2 > retValue)
    {
        retValue = val2;
    }
    if(val3 > retValue)
    {
        retValue = val3;
    }
    return retValue;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int8 CircleIncreaseInt8(int8 *val, int8 min, int8 max)
{
    (*val)++;
    if((*val) > max)
    {
        (*val) = min;
    }
    return (*val);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int16 CircleIncreaseInt16(int16 *val, int16 min, int16 max)
{
    (*val)++;
    if((*val) > max)
    {
        (*val) = min;
    }
    return (*val);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int CircleIncreaseInt(int *val, int min, int max)
{
    (*val)++;
    if((*val) > max)
    {
        (*val) = min;
    }
    return (*val);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int8 CircleDecreaseInt8(int8 *val, int8 min, int8 max)
{
    (*val)--;
    if((*val) < min)
    {
        (*val) = max;
    }
    return *val;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int16 CircleDecreaseInt16(int16 *val, int16 min, int16 max)
{
    (*val)--;
    if((*val) < min)
    {
        (*val) = max;
    }
    return (*val);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int CircleDecreaseInt(int *val, int min, int max)
{
    (*val)--;
    if((*val) < min)
    {
        (*val) = max;
    }
    return (*val);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CircleAddFloat(float *val, float delta, float min, float max)
{
    *val += delta;
    if(*val > max)
    {
        *val = min;
    }
    return *val;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
float CircleSubFloat(float *val, float delta, float min, float max)
{
    *val -= delta;
    if(*val < min)
    {
        *val = max;
    }
    return *val;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void AddLimitationFloat(float *val, float delta, float min, float max)
{
    float sum = *val + delta;
    if(sum < min)
    {
        *val = min;
    }
    else if(sum > max)
    {
        *val = max;
    }
    else
    {
        *val = sum;
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void SwapInt(int *value0, int *value1)
{
    int temp = *value0;
    *value0 = *value1;
    *value1 = temp;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void SortInt(int *value0, int *value1)
{
    if(*value1 < *value0)
    {
        SwapInt(value0,  value1);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char GetSymbolForGovernor(int value)
{
    static const char chars[] =
    {
        SYMBOL_GOVERNOR_SHIFT_0,
        SYMBOL_GOVERNOR_SHIFT_1,
        SYMBOL_GOVERNOR_SHIFT_2,
        SYMBOL_GOVERNOR_SHIFT_3
    };
    while(value < 0)
    {
        value += 4;
    }
    return chars[value % 4];
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void EmptyFuncVV(void) { }


//----------------------------------------------------------------------------------------------------------------------------------------------------
void EmptyFuncVpV(void *empty) { }


//----------------------------------------------------------------------------------------------------------------------------------------------------
void EmptyFuncVII(int i, int ii) { }


//----------------------------------------------------------------------------------------------------------------------------------------------------
void EmptyFuncpVII(void *v, int i, int ii) { }

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FillArrayUINT8(uint8 *data, uint8 value, int numElements)
{
    for (int i = 0; i < numElements; i++)
    {
        data[i] = value;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int FindAnotherElement(uint8 *data, uint8 value, int numElements)
{
    for (int i = 0; i < numElements; i++)
    {
        if (data[i] != value)
        {
            return i;
        }
    }

    return -1;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
char *IntToStrCat(char *_buffer, int _value)
{
#define LENGTH_INTTOSTRCAT 10
    char buffer[LENGTH_INTTOSTRCAT];
    for(int i = 0; i < LENGTH_INTTOSTRCAT; i++)
    {
        buffer[i] = 0;
    }
    int pointer = LENGTH_INTTOSTRCAT -1;

    while(_value > 0)
    {
        buffer[pointer] = (unsigned char)(_value % 10);
        _value /= 10;
        pointer--;
    }

    while(*_buffer)
    {
        _buffer++;
    }
    int i = 0;

    for(; i < LENGTH_INTTOSTRCAT; i++)
    {
        if(buffer[i] > 0)
        {
            break;
        }
    }

    for(; i < LENGTH_INTTOSTRCAT; i++)
    {
        *_buffer = 0x30 | buffer[i];
        _buffer++;
    }

    *_buffer = 0;

    return _buffer;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
int BCD2Int(uint bcd)
{
    int pow = 1;

    int value = 0;

    for (int i = 0; i < 8; i++)
    {
        value += (bcd & 0x0f) * pow;
        pow *= 10;
        bcd = bcd >> 4;
    }

    return value;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void BubbleSortINT(int array[], int num)
{
    for (int i = 0; i < num; i++)
    {
        for (int j = 0; j < num - 1; j++)
        {
            if (array[j] > array[j + 1])
            {
                int time = array[j];
                array[j] = array[j + 1];
                array[j + 1] = time;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void LoggingArrayUINT8(char *label, uint8 *array, int numElements)
{                                   // (1 + 3) -> 1 позици€ на пробел и максимум 3 на 8-битное число
                                    // 1 -> завершающий ноль
                                    // strlen(label) -> на запись метки
                                    // 3 -> на " - "
    char *buffer = (char *)malloc(numElements * (1 + 3) + 1 + strlen(label) + 3);   // 1 + 3 - 

    buffer[0] = 0;

    strcat(buffer, label);
    strcat(buffer, " - ");

    char bufferNumber[5];                               // Ѕуфер дл€ текстового представлени€ 8-битового значени€

    for (int i = 0; i < numElements; i++)
    {
        sprintf(bufferNumber, "%d ", array[i]);
        strcat(buffer, bufferNumber);
        for (int num = 0; num < 4 - strlen(bufferNumber); num++)
        {
            strcat(buffer, " ");
        }
    }

    LOG_WRITE(buffer);

    free(buffer);
}
