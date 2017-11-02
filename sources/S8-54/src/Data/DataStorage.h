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

class DataStorage
{
public:
    /// Удаление всех сохранённых измерений.
    void Clear(void);
    /// Добавить считанные данные. При этом настройками считаются текущие настройки прибора.
    void AddData(uint8 *dataA, uint8 *dataB, DataSettings dss);
    /// Возвращает число непрерывных измерений, начиная с последнего, с такими же настройками, как у последнего.
    int NumElementsWithSameSettings(void);
    /// Возвращает число непрерывных измерений, начиная с последнего, с текущими настройками прибора.
    int NumElementsWithCurrentSettings(void);
    /// Возвращает количество сохранённых измерений.
    int NumElementsInStorage(void);
    /// Получить данные. Счёт идёт с конца. Даннные будут сохранены по адресам ds, dataA, dataB..
    bool GetDataFromEnd(int fromEnd, DataSettings *ds, uint8 *dataA, uint8 *dataB);
    /// Получить указатели на сохранённые данные. Данные хранятся в ОЗУ.
    bool GetDataFromEnd_RAM(int fromEnd, DataSettings **ds, uint16 **dataA, uint16 **dataB);
    /// Возвращет указатель, который указывает на данные в памяти внешнего ОЗУ. Доступ к данным осуществляется по полусловам
    uint8 *GetData_RAM(Channel ch, int fromEnd);
    /// Получить усреднённые данные по нескольким измерениям
    uint8 *GetAverageData(Channel ch);
    /// Возвращает указатель на данные, отстоящие на indexFromEnd oт последнего сохранённого
    DataSettings *DataSettingsFromEnd(int indexFromEnd);
    /// Получить ограничивающую линию сигнала 0 - снизу, 1 - сверху
    bool GetLimitation(Channel ch, uint8 *data, int direction);
    /// Сколько измерений может быть сохранено при такой же длине данных, как у последнего записанного
    int NumberAvailableEntries(void);

    //////////////////////////////// Функции для поточечного вывода ////////////////////////////////////////

    /// Создаёт в хранилище место для помещения туда точек.Данные этого фрейма можно получить GetDataFromEnd_RAM(0...)
    void NewFrameP2P(DataSettings *dss);
    /// Поместить очередные считанные точки в специально выделенный для них фрейм
    void AddPointsP2P(uint16 dataA, uint16 dataB);
    /// \brief Получить данные фрейма поточечного вывода. Последние полученные данные можно получить вызовом GetDataFromEnd_RAM(1...). 
    /// Возвращает общее количество точек (оно может быть больше, чем количество точек в канале. В этом случае в канале находятся последние 
    /// numPointsInChan точек.
    int GetFrameP2P_RAM(DataSettings **ds, uint8 **dataA, uint8 **dataB);
};

extern DataStorage dS;

////////////////////////////// Функции для самописца //////////////////////////////////////////////////

/** @}  @}
 */
