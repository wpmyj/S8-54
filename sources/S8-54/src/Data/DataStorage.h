#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "Hardware/RTC.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup FPGA
 *  @{
 *  @defgroup DataStorage Data Storage
 *  @{
 */

/// Удаление всех сохранённых измерений.
void DS_Clear(void);
/// Добавить считанные данные. При этом настройками считаются текущие настройки прибора.
void DS_AddData(uint8 *dataA, uint8 *dataB, DataSettings dss);
/// Возвращает число непрерывных измерений, начиная с последнего, с такими же настройками, как у последнего.
int DS_NumElementsWithSameSettings(void);
/// Возвращает число непрерывных измерений, начиная с последнего, с текущими настройками прибора.
int DS_NumElementsWithCurrentSettings(void);
/// Возвращает количество сохранённых измерений.
int DS_NumElementsInStorage(void);
/// Получить данные. Счёт идёт с конца. Даннные будут сохранены по адресам ds, dataA, dataB..
bool DS_GetDataFromEnd(int fromEnd, DataSettings *ds, uint8 *dataA, uint8 *dataB);
/// Получить указатели на сохранённые данные. Данные хранятся в ОЗУ.
bool DS_GetDataFromEnd_RAM(int fromEnd, DataSettings **ds, uint16 **dataA, uint16 **dataB);
/// Возвращет указатель, который указывает на данные в памяти внешнего ОЗУ. Доступ к данным осуществляется по полусловам
uint8 *DS_GetData_RAM(Channel ch, int fromEnd);
/// Получить усреднённые данные по нескольким измерениям
uint8 *DS_GetAverageData(Channel ch);
/// Возвращает указатель на данные, отстоящие на indexFromEnd oт последнего сохранённого
DataSettings *DS_DataSettingsFromEnd(int indexFromEnd);
/// Получить ограничивающую линию сигнала 0 - снизу, 1 - сверху
uint8 *DS_GetLimitation(Channel ch, int direction);
/// Сколько измерений может быть сохранено при такой же длине данных, как у последнего записанного
int DS_NumberAvailableEntries(void);

//////////////////////////////// Функции для поточечного вывода ////////////////////////////////////////

/// Создаёт в хранилище место для помещения туда точек.Данные этого фрейма можно получить DS_GetDataFromEnd_RAM(0...)
void DS_NewFrameP2P(DataSettings dss);
/// Поместить очередные считанные точки в специально выделенный для них фрейм
void DS_AddPointsP2P(uint16 dataA, uint16 dataB);
/// \brief Получить данные фрейма поточечного вывода. Последние полученные данные можно получить вызовом DS_GetDataFromEnd_RAM(1...). 
/// Возвращает общее количество точек (оно может быть больше, чем количество точек в канале. В этом случае в канале находятся последние 
/// numPointsInChan точек.
int DS_GetLastFrameP2P_RAM(DataSettings **ds, uint8 **dataA, uint8 **dataB);
/// Возвращает число считанных точек в поточечном режиме
int DS_NumPointsInLastFrameP2P(void);

////////////////////////////// Функции для самописца //////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DataSettings_IsEquals(const DataSettings *ds1, const DataSettings *ds2);

/** @}  @}
 */
