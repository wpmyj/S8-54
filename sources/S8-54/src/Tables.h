#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    ExcessValues        // Превышение значения количества сигналов в "НАКОПЛЕНИЕ", "УСРЕДНЕНИЕ", "Мин Макс"
} WarningWithNumber;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ENumSignalsInSec Tables_ENumSignalsInSecToENUM(int numSignalsInSec);

int Tables_ENUMtoENumSignalsInSec(ENumSignalsInSec numSignalsInSec);

const char* Tables_GetTBaseString(TBase tBase);

const char* Tables_GetTBaseStringEN(TBase tBase);

const char* Tables_RangeNameFromValue(Range range);
/// Возвращает номер канала от 1 до 2.
int Tables_GetNumChannel(Channel ch);

extern const char* symbolsAlphaBet[0x48];
/// Выводит строку из таблицы symbolsAlphaBet
void DrawStr(int index, int x, int y);
