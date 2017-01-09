#include "FLASH.h"
#include "Log.h"
#include "Hardware/Hardware.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
    Принцип хранения данных.

    В секторе ADDR_SECTOR_DATA_DATA хранятся массивы с адресами MAX_NUM_SAVED_WAVES данных (если адрес == -1, то данные стёрты). Его размер MAX_NUM_SAVED_WAVES + 1
    При этом в элементе с индексом MAX_NUM_SAVED_WAVES хранится маркер, указываеющий на то, является ли текущий массив активным. Если там -1, то пользуемся им, если нет - переходим к следующему
    Для хранения собственно данных предназначены сектора ADDR_SECTOR_DATA_0...6. Данные хранятся так - сначала DataSettings, а потом каналы в естественном порядке.\
*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Программа и константные данные
#define ADDR_SECTOR_BOOT_0      ((uint)0x08000000)  // 16k Загрузчик
#define ADDR_SECTOR_BOOT_1      ((uint)0x08004000)  // 16k Загрузчик
#define ADDR_FLASH_SECTOR_2     ((uint)0x08008000)  // 16k
#define ADDR_FLASH_SECTOR_3     ((uint)0x0800C000)  // 16k
#define ADDR_FLASH_SECTOR_4     ((uint)0x08010000)  // 64k
#define ADDR_SECTOR_PROGRAM_0   ((uint)0x08020000)  // 128k Основная программа
#define ADDR_SECTOR_PROGRAM_1   ((uint)0x08040000)  // 128k Основная программа
#define ADDR_SECTOR_PROGRAM_2   ((uint)0x08060000)  // 128k Основная программа
#define ADDR_SECTOR_TEMP_1      ((uint)0x08080000)  // 128k | Это сектора для хранения временных даннх. Например, при упллотненнии данных сюда писать будем
#define ADDR_SECTOR_TEMP_2      ((uint)0x080A0000)  // 128k /
#define ADDR_SECTOR_RESOURCES   ((uint)0x080C0000)  // 128k Графические и звуковые ресурсы
#define ADDR_SECTOR_SETTINGS    ((uint)0x080E0000)  // 128k Настройки
#define ADDR_FLASH_SECTOR_12    ((uint)0x08100000)  // 16k
#define ADDR_FLASH_SECTOR_13    ((uint)0x08104000)  // 16k
#define ADDR_FLASH_SECTOR_14    ((uint)0x08108000)  // 16k
#define ADDR_FLASH_SECTOR_15    ((uint)0x0810C000)  // 16k
#define ADDR_SECTOR_DATA_DATA   ((uint)0x08110000)  // 64k  Здесь будем сохранять массивы адресов с нашими данными
#define ADDR_SECTOR_DATA_0      ((uint)0x08120000)  // 128k |
#define ADDR_SECTOR_DATA_1      ((uint)0x08140000)  // 128k |
#define ADDR_SECTOR_DATA_2      ((uint)0x08160000)  // 128k | Здесь будут храниться собственно данные
#define ADDR_SECTOR_DATA_3      ((uint)0x08180000)  // 128k |
#define ADDR_SECTOR_DATA_4      ((uint)0x081A0000)  // 128k |
#define ADDR_SECTOR_DATA_5      ((uint)0x081C0000)  // 128k |
#define ADDR_SECTOR_DATA_6      ((uint)0x081E0000)  // 128k /

#define SIZE_SECTOR_SETTINGS    (128 * 1024)        // Размер сектора, куда сохраняются настройки, в байтах

// Структура для записи массива указателей на данные
typedef struct
{
    uint address[MAX_NUM_SAVED_WAVES + 1];
} ArrayDatas;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#undef CLEAR_FLASH_FLAGS
#define CLEAR_FLASH_FLAGS                                                                   \
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP     |  /* end of operation flag              */   \
                            FLASH_FLAG_OPERR  |  /* operation error flag               */   \
                            FLASH_FLAG_WRPERR |  /* write protected error flag         */   \
                            FLASH_FLAG_PGAERR |  /* programming alignment error flag   */   \
                            FLASH_FLAG_PGPERR |  /* programming parallelism error flag */   \
                            FLASH_FLAG_PGSERR);  /* programming sequence error flag    */

#undef FIRST_RECORD
#define FIRST_RECORD ((RecordConfig*)ADDR_ARRAY_RECORDS)

#undef READ_WORD
#define READ_WORD(address) (*((volatile uint*)address))

#undef RECORD_EXIST
#define RECORD_EXIST (READ_WORD(ADDR_ARRAY_RECORDS) != MAX_UINT)   // true, если есть хоть одна заполненная запись (сохранённые данные)

#undef MEMORY_FOR_SETTINGS
#define MEMORY_FOR_SETTINGS ((ADDR_LAST_SET + 1) - ADDR_FIRST_SET)  // Это размер памяти, доступной для сохранения настроек. Это размер сектора за вычетом 4-х байт для записи маркера и буфера для хранения записей


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    uint addr;  // Начиная с этого адреса записаны данные. Если addrData == MAX_UINT, то это пустая запись, сюда можно писать данные
    int  size;  // Размер в байтах записанных данных
} RecordConfig;


#define ADDR_ARRAY_RECORDS             ((ADDR_SECTOR_SETTINGS) + 4)
#define MAX_NUM_RECORDS 1024
#define SIZE_ARRAY_RECORDS_IN_BYTES    ((MAX_NUM_RECORDS) * sizeof(RecordConfig))
static const uint ADDR_FIRST_SET = ADDR_ARRAY_RECORDS + SIZE_ARRAY_RECORDS_IN_BYTES;    // Адрес первого байта буфера, где сохранены настройки
static const uint ADDR_LAST_SET = ((ADDR_SECTOR_SETTINGS + SIZE_SECTOR_SETTINGS) - 1);  // Адрес последнего байта буфера, где сохранены настройки


// Признак того, что запись в этоу область флэш уже производилась. Если нулевое слово области (данных, ресурсов или настроек) имеет это значение, запись уже была произведена как минимум один раз
static const uint MARK_OF_FILLED = 0x123456;
static const uint startDataInfo = ADDR_SECTOR_DATA_0;


static RecordConfig *records = (RecordConfig *)ADDR_ARRAY_RECORDS;     // Для упрощения операций с записями


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Для настроек
static int CalculateFreeMemory(void);
static RecordConfig* FindRecordConfigForWrite(void);
static void WriteWord(uint address, uint word);
static void WriteBuffer(uint address, uint *buffer, int size);
static void PrepareSectorForData(void);
static void ReadBuffer(uint addressSrc, uint *bufferDest, int size);
static void EraseSector(uint startAddress);
static uint GetSector(uint startAddress);
static RecordConfig* LastFilledRecord(void);    // Возвращает адрес последней записи с сохранёнными настройками или 0, если нет сохранённых настроек

// Для данных
static ArrayDatas* CurrentArray(void);          // Возвращает адрес актуального массива с адресами данных


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FLASH_SaveSettings(void)
{
    if(gBF.alreadyLoadSettings == 0)
    {
        return;
    }

    CLEAR_FLASH_FLAGS
    
    RecordConfig *record = FindRecordConfigForWrite();
    if(record == 0)                                                 // Если нет места для записи настроек
    {
        set.common.countErasedFlashSettings++;
        EraseSector(ADDR_SECTOR_SETTINGS);                          // Стираем сектор
        WriteWord(ADDR_SECTOR_SETTINGS, MARK_OF_FILLED);            // и маркируем
        record = FIRST_RECORD;                                      // Теперь-то уж точно найдём место для записи
    }

    set.common.workingTimeInSecs += gTimerMS / 1000;

    WriteWord((uint)(&record->addr), (record == FIRST_RECORD) ? ADDR_FIRST_SET : (record - 1)->addr + (record - 1)->size);

    WriteWord((uint)(&record->size), sizeof(set));

    WriteBuffer(record->addr, (uint*)(&set), record->size / 4);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FLASH_LoadSettings(void)
{
    CLEAR_FLASH_FLAGS;

    if(READ_WORD(ADDR_SECTOR_SETTINGS) != MARK_OF_FILLED)       // Если первый байт сектора не отмаркирован - первое включение прибора
    {
        set.common.countErasedFlashSettings = 0;
        set.common.countEnables = 0;
        set.common.countErasedFlashData = 0;
        set.common.workingTimeInSecs = 0;
        EraseSector(ADDR_SECTOR_SETTINGS);                      // На всякий случай стираем сектор
        WriteWord(ADDR_SECTOR_SETTINGS, MARK_OF_FILLED);        // И маркируем
        PrepareSectorForData();                                 // Также готовим сектор для сохранения данных
        return false;
    }

    RecordConfig *record = LastFilledRecord();

    if(record == 0)                                             // По какой-то причине сохранённых настроек может не оказаться. Например, сектор был промаркирован при предыдущем включении,
    {                                                           // но прибор выключили выключателем на задней стенке, а не кнопкой на передней панели, вследствие чего настройки не сохранились
        return false;
    }

    int numWordsForCopy = record->size / 4;

    if(sizeof(set) != record->size)                         // Если новая структура настроек не соответствует сохранённой
    {
        numWordsForCopy = SIZE_NONRESET_SETTINGS / 4;           // то будем копировать только несбрасываемые настройки (у них всегда один размер и они в начале структуры)
    }

    ReadBuffer(record->addr, (uint*)(&set), numWordsForCopy);
    set.common.countEnables++;
    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static RecordConfig* LastFilledRecord(void)
{
    for(int i = MAX_NUM_RECORDS - 1; i >= 0; --i)           // Просматриваем массив записей от последнего элемента к первому
    {                                                       // Никаких дополнительных проверок не требуется, потому что при стирании сектора все значения в нём равны 0xffffffff
        if(records[i].addr != MAX_UINT)
        {
            return &records[i];
        }
    }

    return 0;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static RecordConfig* FindRecordConfigForWrite(void)
{
    RecordConfig *record = LastFilledRecord();
    if(record == &records[MAX_NUM_RECORDS - 1] ||   // Если все записи заняты
       CalculateFreeMemory() < sizeof(set))         // или памяти осталось меньше, чем нужно для сохранения настроек
    {
        return 0;
    }
    
    if(record == 0)
    {
        return &records[0];
    }

    return ++record;                                // Возвращаем адрес следующей за последней заполненной записи
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteWord(uint address, uint word)
{
    CLEAR_FLASH_FLAGS

    HAL_FLASH_Unlock();
    if(HAL_FLASH_Program(TYPEPROGRAM_WORD, address, (uint64_t)word) != HAL_OK)
    {
        LOG_ERROR("Не могу записать в память");
    }
    HAL_FLASH_Lock();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteBuffer(uint address, uint *buffer, int size)
{
    HAL_FLASH_Unlock();
    for(int i = 0; i < size; i++)
    {
        if(HAL_FLASH_Program(TYPEPROGRAM_WORD, address, (uint64_t)(buffer[i])) != HAL_OK)
        {
            LOG_ERROR("Не могу записать в флеш-память");
        }
        address += sizeof(uint);
    }
    HAL_FLASH_Lock();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void PrepareSectorForData(void)
{
    EraseSector(ADDR_SECTOR_DATA_0);
    for(int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        WriteWord(startDataInfo + i * 4, 0);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void ReadBuffer(uint addressSrc, uint *bufferDest, int size)
{
    for(int i = 0; i < size; i++)
    {
        bufferDest[i] = *((uint*)addressSrc);
        addressSrc += 4;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void EraseSector(uint startAddress)
{
    CLEAR_FLASH_FLAGS

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef flashITD;
    flashITD.TypeErase = TYPEERASE_SECTORS;
    flashITD.Sector = GetSector(startAddress);
    flashITD.NbSectors = 1;
    flashITD.VoltageRange = VOLTAGE_RANGE_3;

    uint32_t error = 0;

    while(gSoundIsBeep) // WARN Здесь ждём, пока бикалка не закончит. Костыль, надо разобраться, почему они не могут вместе работать
    {
    };

    HAL_FLASHEx_Erase(&flashITD, &error);

    HAL_FLASH_Lock();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static uint GetSector(uint startAddress)
{
    typedef struct
    {
        uint number;
        uint startAddress;
    } StructSector;

    static const StructSector sectors[24] =
    {
        {FLASH_SECTOR_0, ADDR_SECTOR_BOOT_0},
        {FLASH_SECTOR_1, ADDR_SECTOR_BOOT_1},
        {FLASH_SECTOR_2, ADDR_FLASH_SECTOR_2},
        {FLASH_SECTOR_3, ADDR_FLASH_SECTOR_3},
        {FLASH_SECTOR_4, ADDR_FLASH_SECTOR_4},
        {FLASH_SECTOR_5, ADDR_SECTOR_PROGRAM_0},
        {FLASH_SECTOR_6, ADDR_SECTOR_PROGRAM_1},
        {FLASH_SECTOR_7, ADDR_SECTOR_PROGRAM_2},
        {FLASH_SECTOR_8, ADDR_SECTOR_TEMP_1},
        {FLASH_SECTOR_9, ADDR_SECTOR_TEMP_2},
        {FLASH_SECTOR_10, ADDR_SECTOR_RESOURCES},
        {FLASH_SECTOR_11, ADDR_SECTOR_SETTINGS},
        {FLASH_SECTOR_12, ADDR_FLASH_SECTOR_12},
        {FLASH_SECTOR_13, ADDR_FLASH_SECTOR_13},
        {FLASH_SECTOR_14, ADDR_FLASH_SECTOR_14},
        {FLASH_SECTOR_15, ADDR_FLASH_SECTOR_15},
        {FLASH_SECTOR_16, ADDR_SECTOR_DATA_DATA},
        {FLASH_SECTOR_17, ADDR_SECTOR_DATA_0},
        {FLASH_SECTOR_18, ADDR_SECTOR_DATA_1},
        {FLASH_SECTOR_19, ADDR_SECTOR_DATA_2},
        {FLASH_SECTOR_20, ADDR_SECTOR_DATA_3},
        {FLASH_SECTOR_21, ADDR_SECTOR_DATA_4},
        {FLASH_SECTOR_22, ADDR_SECTOR_DATA_5},
        {FLASH_SECTOR_23, ADDR_SECTOR_DATA_6}
    };

    for(int i = 0; i < 24; i++)
    {
        if(sectors[i].startAddress == startAddress)
        {
            return sectors[i].number;
        }
    }

    LOG_ERROR("Неправильный адрес сектора");

    return (uint)-1;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int CalculateFreeMemory(void)
{
    RecordConfig *lastFilledRecord = LastFilledRecord();    // Находим запись с последними сохранёнными настройками

    if(lastFilledRecord == 0)                               // Если все записи свободны
    {
        return MEMORY_FOR_SETTINGS;
    }

    if(lastFilledRecord == &records[MAX_NUM_RECORDS - 1])   // Если все записи заняты
    {
        return 0;                                           // то свободной памяти нет
    }

    int retValue = ADDR_LAST_SET - (lastFilledRecord->addr + lastFilledRecord->size);

    return retValue < 0 ? 0 : retValue;     // Возвращаем 0, если размер получился отрицательный - каким-то образом последняя запись оказалась за пределами сектора
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static uint FindAddressNextDataInfo(void)
{
    uint addressNextInfo = startDataInfo + MAX_NUM_SAVED_WAVES * 4;

    while (*((uint*)addressNextInfo) != MAX_UINT)
    {
        addressNextInfo = *((uint*)addressNextInfo) + MAX_NUM_SAVED_WAVES * 4;
    }

    return addressNextInfo;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static uint FindActualDataInfo(void)
{
    return FindAddressNextDataInfo() - MAX_NUM_SAVED_WAVES * 4;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_GetDataInfo(bool existData[MAX_NUM_SAVED_WAVES])
{
    uint address = FindActualDataInfo();

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        existData[i] = READ_WORD(address + i * 4) != 0;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FLASH_ExistData(int num)
{
    ArrayDatas *array = CurrentArray();
    return array->address[num] != MAX_UINT;     // Признаком того, что данные не записаны в этот элемент, является равенство его (-1)
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_DeleteData(int num)
{
    /*
        Для того, чтобы удалить данные, нужно сделать следующее:
        1. Если в DATA_DATA нет места для записи нового массива:
            1.1 Запомнить содержимое текущего массива.
            1.2 Cтереть сектор DATA_DATA
            1.3 Записать в начало сектора DATA_DATA массив, который был текущим, со следующими изменениями:
                - в MAX_NUM_SAVED_WAVES и array[num] должно быть (-1)
            1.4 Выход
        2. 
    */

    uint address = FindActualDataInfo();
    WriteWord(address + num * 4, 0);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
int CalculateSizeData(DataSettings *ds)
{
    int size = sizeof(DataSettings);

    int pointsInChannel = NumBytesInChannel(ds);

    if (ds->enableChA == 1)
    {
        size += pointsInChannel;
    }

    if (ds->enableChB == 1)
    {
        size += pointsInChannel;
    }

    return size;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static uint FreeMemory(void)
{
    return ADDR_SECTOR_DATA_0 + 128 * 1024 - FindAddressNextDataInfo() - 1 - 4 * MAX_NUM_SAVED_WAVES - 3000;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteBufferBytes(uint address, uint8 *buffer, int size)
{
    HAL_FLASH_Unlock();
    for(int i = 0; i < size; i++)
    {
        HAL_FLASH_Program(TYPEPROGRAM_BYTE, address, (uint64_t)(buffer[i]));
        address++;
    }
    HAL_FLASH_Lock();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void CompactMemory(void)
{
    Display_ClearFromWarnings();
    Display_ShowWarning(MovingData);
    Display_Update();
    uint dataInfoRel = FindActualDataInfo() - ADDR_SECTOR_DATA_0;

    EraseSector(ADDR_SECTOR_DATA_1);
    WriteBufferBytes(ADDR_SECTOR_DATA_1, (uint8*)ADDR_SECTOR_DATA_0, 1024 * 128);
    PrepareSectorForData();

    uint addressDataInfo = ADDR_SECTOR_DATA_1 + dataInfoRel;

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        uint addrDataOld = READ_WORD(addressDataInfo + i * 4);
        if (addrDataOld != 0)
        {
            uint addrDataNew = addrDataOld + 1024 * 128;
            DataSettings *ds = (DataSettings*)addrDataNew;
            addrDataNew += sizeof(DataSettings);
            uint8 *dataA = 0;
            uint8 *dataB = 0;
            if (ds->enableChA == 1)
            {
                dataA = (uint8*)addrDataNew;
                addrDataNew += NumBytesInChannel(ds);
            }
            if (ds->enableChB == 1)
            {
                dataB = (uint8*)addrDataNew;
            }
            FLASH_SaveData(i, ds, dataA, dataB);
        }
    }
    Display_ClearFromWarnings();
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_SaveData(int num, DataSettings *ds, uint8 *dataA, uint8 *dataB)
{
    /*
        0. Если в num уже записаны данные, то удаляем их
        1. Узнаём количество оставшейся памяти
        2. Если не хватает памяти для записи данных - уплотняем массив (для этой цели используем вспомогательные сектора TEMP_1 и TEMP_2)
        4. Записываем в сектора данных данные в порядке DataSettings, dataChanA, dataChanB
        5. Записываем в массив из DATA_DATA адрес наших записанных данных
    */

// 0
    if (FLASH_ExistData(num))
    {
        FLASH_DeleteData(num);
    }





    int size = CalculateSizeData(ds);

// 2
    if (FreeMemory() < (uint)size)
    {
        CompactMemory();
    }

// 3
    uint addrDataInfo = FindActualDataInfo();          // Находим начало действующего информационного массива

// 4
    uint addrForWrite = addrDataInfo + MAX_NUM_SAVED_WAVES * 4;             // Это - адрес, по которому будет храниться адрес следующего информационного массива
    uint valueForWrite = addrForWrite + 4 + size;                           // Это - адрес следующего информационного массива
    WriteWord(addrForWrite, valueForWrite);

// 5
    uint address = addrDataInfo + MAX_NUM_SAVED_WAVES * 4 + 4;              // Адрес, по которому будет сохранён сигнал с настройками
    uint addressNewData = address;
    WriteBufferBytes(address, (uint8*)ds, sizeof(DataSettings));            // Сохраняем настройки сигнала
    address += sizeof(DataSettings);
    
    int pointsInChannel = NumBytesInChannel(ds);

    if (ds->enableChA == 1)
    {
        WriteBufferBytes(address, (uint8*)dataA, pointsInChannel);       // Сохраняем первый канал
        address += pointsInChannel;
    }

    if (ds->enableChB == 1)
    {
        WriteBufferBytes(address, (uint8*)dataB, pointsInChannel);       // Сохраняем второй канал
        address += pointsInChannel;
    }

// 6
    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        uint addressForWrite = address + i * 4;
        if (i == num)
        {
            WriteWord(addressForWrite, addressNewData);
        }
        else
        {
            WriteWord(addressForWrite, READ_WORD(addrDataInfo + i * 4));
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static ArrayDatas* CurrentArray(void)
{
    ArrayDatas *array = (ArrayDatas*)ADDR_SECTOR_DATA_DATA;
    while (array->address[MAX_NUM_SAVED_WAVES] != MAX_UINT)
    {
        ++array;
    }
    return array;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FLASH_GetData(int num, DataSettings **ds, uint8 **dataA, uint8 **dataB)
{
    *ds = 0;
    *dataA = 0;
    *dataB = 0;
    
    return false;

    uint addrDataInfo = FindActualDataInfo();
    if (READ_WORD(addrDataInfo + 4 * num) == 0)
    {
        return false;
    }

    uint addrDS = READ_WORD(addrDataInfo + 4 * num);

    uint addrData0 = 0;
    uint addrData1 = 0;

    *ds = (DataSettings*)addrDS;
    
    if ((*ds)->enableChA == 1)
    {
        addrData0 = addrDS + sizeof(DataSettings);
    }

    if ((*ds)->enableChB == 1)
    {
        if (addrData0 != 0)
        {
            addrData1 = addrData0 + NumBytesInChannel(*ds);
        }
        else
        {
            addrData1 = addrDS + sizeof(DataSettings);
        }
    }

    *dataA = (uint8*)addrData0;
    *dataB = (uint8*)addrData1;
    
    return true;
}

#undef CLEAR_FLASH_FLAGS
#undef FIRST_RECORD
#undef READ_WORD
#undef RECORD_EXIST
#undef MEMORY_FOR_SETTINGS
