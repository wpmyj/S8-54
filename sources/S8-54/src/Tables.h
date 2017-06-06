#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    ExcessValues        // Превышение значения количества сигналов в "НАКОПЛЕНИЕ", "УСРЕДНЕНИЕ", "Мин Макс"
} WarningWithNumber;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ENumSignalsInSec Tables_ENumSignalsInSecToENUM(int numSignalsInSec);
int             Tables_ENUMtoENumSignalsInSec(ENumSignalsInSec numSignalsInSec);
const char*     Tables_GetTBaseString(TBase tBase);
const char*     Tables_GetTBaseStringEN(TBase tBase);
const char*     Tables_RangeNameFromValue(Range range);
int             Tables_GetNumChannel(Channel ch);     // Возвращает номер канала от 1 до 2
