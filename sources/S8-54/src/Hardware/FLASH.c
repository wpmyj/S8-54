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

    В секторе ADDR_DATA_DATA хранятся массивы с адресами MAX_NUM_SAVED_WAVES данных (если адрес == -1, то данные стёрты). Его размер MAX_NUM_SAVED_WAVES + 1
    При этом в элементе с индексом MAX_NUM_SAVED_WAVES хранится маркер, указываеющий на то, является ли текущий массив активным. Если там -1, то пользуемся им, если нет - переходим к следующему
    Для хранения собственно данных предназначены сектора ADDR_DATA_0...6. Данные хранятся так - сначала DataSettings, а потом каналы в естественном порядке.
    Данные хранятся по бесконечному кругу. Вначале пишутся по адресу ADDR_DATA_0, следующие за ними и так далее. Когда доходит до последнего байта ADDR_DATA_6, стирается ADDR_DATA_0 сектор
    и пишется в него. Когда полностью заполняется ADDR_DATA_0 сектор, стирается ADDR_DATA_1
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
#define ADDR_FLASH_SECTOR_8     ((uint)0x08080000)  // 128k
#define ADDR_FLASH_SECTOR_9     ((uint)0x080A0000)  // 128k
#define ADDR_SECTOR_RESOURCES   ((uint)0x080C0000)  // 128k Графические и звуковые ресурсы
#define ADDR_SECTOR_SETTINGS    ((uint)0x080E0000)  // 128k Настройки
#define ADDR_FLASH_SECTOR_12    ((uint)0x08100000)  // 16k
#define ADDR_FLASH_SECTOR_13    ((uint)0x08104000)  // 16k
#define ADDR_FLASH_SECTOR_14    ((uint)0x08108000)  // 16k
#define ADDR_FLASH_SECTOR_15    ((uint)0x0810C000)  // 16k
#define ADDR_DATA_DATA   ((uint)0x08110000)  // 64k  Здесь будем сохранять массивы адресов с нашими данными
#define ADDR_DATA_0             ((uint)0x08120000)  // 128k |
#define ADDR_DATA_1             ((uint)0x08140000)  // 128k |
#define ADDR_DATA_2             ((uint)0x08160000)  // 128k |
#define ADDR_DATA_3             ((uint)0x08180000)  // 128k | Здесь будут храниться собственно данные
#define ADDR_DATA_4             ((uint)0x081A0000)  // 128k |
#define ADDR_DATA_5             ((uint)0x081C0000)  // 128k |
#define ADDR_DATA_6             ((uint)0x081E0000)  // 128k /

#define SIZE_SECTOR_128         (128 * 1024)

#define SIZE_SECTOR_SETTINGS    (128 * 1024)        // Размер сектора, куда сохраняются настройки, в байтах

#define DATA_START              ADDR_DATA_0
#define DATA_END                (ADDR_DATA_6 + 128 * 1024)  // На самом указывает на первый байт после области

// Структура для записи массива указателей на данные
typedef struct
{
    uint lastRecord;                        // Здесь адрес, по которому записаны последние данные
    uint addr[MAX_NUM_SAVED_WAVES + 1];
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
static const uint startDataInfo = ADDR_DATA_1;


static RecordConfig *records = (RecordConfig *)ADDR_ARRAY_RECORDS;     // Для упрощения операций с записями


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Для настроек
static int CalculateFreeMemory(void);
static RecordConfig* FindRecordConfigForWrite(void);
static void WriteWord(uint address, uint word);
static void WriteBufferWords(uint address, void *buffer, int numWords);
static void PrepareSectorForData(void);
static void ReadBuffer(uint addressSrc, uint *bufferDest, int size);
static void EraseSector(uint startAddress);
static uint GetSector(uint startAddress);
static RecordConfig* LastFilledRecord(void);    // Возвращает адрес последней записи с сохранёнными настройками или 0, если нет сохранённых настроек

// Для данных
static ArrayDatas* CurrentArray(void);          // Возвращает адрес актуального массива с адресами данных
static ArrayDatas* NextArray(void);             // Возвращает адрес массива, находящегося за актуальным. Если сектор DATA_DATA исчерпан, этот адрес будет меньше, чем текущий
static uint SaveDataChannel(uint address, Channel ch, DataSettings *ds, uint8 *data);   // Сохраняте данные канала по указанному адресу. Возвращает адрес первого байта, следующего после сохранённых данных
static int SizeDataInAddress(uint address);                     // Возвращает размер данных, записанных по адресу address
static bool IsFirstAddressSectorData(uint address);             // Возвращает true, если это первый адрес сектора ADDR_DATA_0...6
static bool IsPlacedDataInSector(uint address, int size);       // Возвращает true, если данные размером size могут быть размещены, начиная с адреса address, в одном секторе
static uint StartAddressSector(uint address);                   // Возвращает первый адрес сектора, которому принадлежить адрес address
static uint LastAddressSector(uint address);                    // Возвращает послединй адрес сектора, которому принадлежит адрес address


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

    WriteBufferWords(record->addr, &set, record->size / 4);
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
static void WriteBufferWords(uint address, void *buffer, int numWords)
{
    HAL_FLASH_Unlock();
    for(int i = 0; i < numWords; i++)
    {
        if(HAL_FLASH_Program(TYPEPROGRAM_WORD, address, (uint64_t)(((uint*)buffer)[i])) != HAL_OK)
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
    EraseSector(ADDR_DATA_1);
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
        {FLASH_SECTOR_8, ADDR_FLASH_SECTOR_8},
        {FLASH_SECTOR_9, ADDR_FLASH_SECTOR_9},
        {FLASH_SECTOR_10, ADDR_SECTOR_RESOURCES},
        {FLASH_SECTOR_11, ADDR_SECTOR_SETTINGS},
        {FLASH_SECTOR_12, ADDR_FLASH_SECTOR_12},
        {FLASH_SECTOR_13, ADDR_FLASH_SECTOR_13},
        {FLASH_SECTOR_14, ADDR_FLASH_SECTOR_14},
        {FLASH_SECTOR_15, ADDR_FLASH_SECTOR_15},
        {FLASH_SECTOR_16, ADDR_DATA_DATA},
        {FLASH_SECTOR_17, ADDR_DATA_0},
        {FLASH_SECTOR_18, ADDR_DATA_1},
        {FLASH_SECTOR_19, ADDR_DATA_2},
        {FLASH_SECTOR_20, ADDR_DATA_3},
        {FLASH_SECTOR_21, ADDR_DATA_4},
        {FLASH_SECTOR_22, ADDR_DATA_5},
        {FLASH_SECTOR_23, ADDR_DATA_6}
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
void FLASH_GetDataInfo(bool existData[MAX_NUM_SAVED_WAVES])
{
    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        existData[i] = FLASH_ExistData(i);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FLASH_ExistData(int num)
{
    return CurrentArray()->addr[num] != MAX_UINT;     // Признаком того, что данные не записаны в этот элемент, является равенство его (-1)
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_DeleteData(int num)
{
    /*
        Для того, чтобы удалить данные, нужно сделать следующее:
        1. Если array[num] == MAX_UINT:
            - выход
        2. Сохранить в array_temp текущий массив со следующими изменениями:
            - array_temp[MAX_NUM_SAVED_WAVES] = MAX_UINT; array_temp[num] = MAX_UINT
        3. Если в DATA_DATA нет места для записи нового массива:
            3.1 Cтереть сектор DATA_DATA
            3.2 Записать в начало DATA_DATA array_temp
            3.3 Выход
        4. Записать в array[MAX_NUM_SAVED_WAVES] 0
        5. Разместить за текущим массивом array_temp
    */

    if (!FLASH_ExistData(num))  // Если здесь данные не сохранены
    {
        return;
    }

    ArrayDatas *array = CurrentArray();
    ArrayDatas array_temp = *array;
    array_temp.addr[MAX_NUM_SAVED_WAVES] = MAX_UINT;
    array_temp.addr[num] = MAX_UINT;

    ArrayDatas *next = NextArray();     // По этому адресу будет сохранён следующий массив

    if (next < array)                   // Если этот адрес меньше адреса следующего массива, значит, в DATA_DATA нет места сохранения нового массива. Будем стирать
    {
        EraseSector(ADDR_DATA_DATA);
        WriteBufferWords(ADDR_DATA_DATA, &array_temp, sizeof(ArrayDatas) / 4);
    }
    else
    {
        WriteWord((uint)&array->addr[MAX_NUM_SAVED_WAVES], 0);
        WriteBufferWords((uint)next, &array_temp, sizeof(ArrayDatas) / 4);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteBufferBytes(uint address, void *buffer, int size)
{
    HAL_FLASH_Unlock();
    for(int i = 0; i < size; i++)
    {
        HAL_FLASH_Program(TYPEPROGRAM_BYTE, address, (uint64_t)(((uint8*)buffer)[i]));
        address++;
    }
    HAL_FLASH_Lock();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static int SizeDataInAddress(uint address)
{
    DataSettings *ds = (DataSettings*)address;

    return sizeof(DataSettings) + NumBytesInData(ds);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_SaveData(int num, DataSettings *ds, uint8 *dataA, uint8 *dataB)
{
    int size = sizeof(DataSettings) + NumBytesInData(ds);                   // Количество байт, которые нужно записать

    ArrayDatas array = *CurrentArray();

    uint address = DATA_START;                                              // Первая запись должна быть проведена по начальному адресу области записи данных

    if (array.lastRecord != MAX_UINT)
    {
        address = array.lastRecord + SizeDataInAddress(array.lastRecord);   // А если записи уже были - находим адрес первого байта за последней записью
    }

    if (address + sizeof(DataSettings) + NumBytesInData(ds) > DATA_END)     // Если не хватает места в 
    {
        address = DATA_START;                                               // То записывать будем с начала
    }

    if (IsFirstAddressSectorData(address))                                  // Если это первый адрес сектора
    {
        EraseSector(address);                                               // то сотрём его перед записью
    }

    if (!IsPlacedDataInSector(address, size))                               // Если не хватает места для рамещения в одном секторе
    {
        EraseSector(StartAddressSector(address) + SIZE_SECTOR_128);         // То стираем следующий сектор
    }
    
    array.addr[num] = address;
    array.lastRecord = address;

    ArrayDatas *newArray = NextArray();

    WriteWord((uint)(&(CurrentArray()->addr[MAX_NUM_SAVED_WAVES])), 0);     // Записываем признак того, что текущий массив уже не последний

    if (newArray < CurrentArray())                                          // Если адрес следующего массива меньше адреса текущего
    {
        EraseSector(ADDR_DATA_DATA);                                        // то пошли на следующий круг - стираем сектор
    }

    WriteBufferBytes((uint)newArray, &array, sizeof(array));                // Сохраняем новый массив

    WriteBufferBytes(array.addr[num], ds, sizeof(DataSettings));            // Сохраняем DataSettings
    address = array.addr[num] + sizeof(DataSettings);
    address = SaveDataChannel(address, A, ds, dataA);                       // Сохраняем данные каналов
              SaveDataChannel(address, B, ds, dataB);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsFirstAddressSectorData(uint address)
{
    const uint addr[7] = {ADDR_DATA_0, ADDR_DATA_1, ADDR_DATA_2, ADDR_DATA_3, ADDR_DATA_4, ADDR_DATA_5, ADDR_DATA_6};

    for (int i = 0; i < 7; i++)
    {
        if (address == addr[i])
        {
            return true;
        }
    }

    return false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsPlacedDataInSector(uint address, int size)
{
    return (address + size) <= (LastAddressSector(address) + 1);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint StartAddressSector(uint address)
{
    const uint addr[7] = {ADDR_DATA_0, ADDR_DATA_1, ADDR_DATA_2, ADDR_DATA_3, ADDR_DATA_4, ADDR_DATA_5, ADDR_DATA_6};

    for (int i = 0; i < 7; i++)
    {
        if (addr[i] + SIZE_SECTOR_128 > address)
        {
            return addr[i];
        }
    }

    LOG_ERROR("Неправильный сектор");

    return 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint LastAddressSector(uint address)
{
    const uint addr[7] = {ADDR_DATA_0, ADDR_DATA_1, ADDR_DATA_2, ADDR_DATA_3, ADDR_DATA_4, ADDR_DATA_5, ADDR_DATA_6};

    for (int i = 0; i < 7; i++)
    {
        if (addr[i] + SIZE_SECTOR_128 > address)
        {
            return addr[i] + SIZE_SECTOR_128 - 1;
        }
    }

    LOG_ERROR("Неправильный сектор");

    return 0;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static ArrayDatas* CurrentArray(void)
{
    ArrayDatas *array = (ArrayDatas*)ADDR_DATA_DATA;
    while (array->addr[MAX_NUM_SAVED_WAVES] != MAX_UINT)
    {
        ++array;
    }
    return array;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static ArrayDatas* NextArray(void)
{
    ArrayDatas* current = CurrentArray();
    ArrayDatas* next = current + 1;

    if (ADDR_DATA_DATA - (uint)next < sizeof(ArrayDatas))    // Если в DATA_DATA не осталось места для ещё одного массива
    {
        next = (ArrayDatas*)ADDR_DATA_DATA;
    }

    return next;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint SaveDataChannel(uint address, Channel ch, DataSettings *ds, uint8 *data)
{
    if ((ch == A && ds->enableChA == 0) ||
        (ch == B && ds->enableChB == 0))
    {
        return address;
    }

    int numBytes = NumBytesInChannel(ds);
    WriteBufferBytes(address, data, numBytes);
    return address + numBytes;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
bool FLASH_GetData(int num, DataSettings **ds, uint8 **dataA, uint8 **dataB)
{
    *ds = 0;
    *dataA = 0;
    *dataB = 0;

    ArrayDatas array = *CurrentArray();
    uint address = array.addr[num];
    if (address == MAX_UINT)
    {
        return false;
    }

    *ds = (DataSettings*)address;

    address += sizeof(DataSettings);
    if ((*ds)->enableChA)
    {
        *dataA = (uint8*)address;
        address += NumBytesInChannel(*ds);
    }
    if ((*ds)->enableChB)
    {
        *dataB = (uint8*)address;
    }
    
    return true;
}

#undef CLEAR_FLASH_FLAGS
#undef FIRST_RECORD
#undef READ_WORD
#undef RECORD_EXIST
#undef MEMORY_FOR_SETTINGS
