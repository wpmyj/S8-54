#include "FLASH.h"
#include "Log.h"
#include "Hardware/Hardware.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
    Принцип хранения данных.
    Для хранения данных используется 6 основных секторов (ADDR_DATA_0...ADDR_DATA_5) и один вспомогательный (ADDR_DATA_TEMP).
    Места для хранения каждого номера строго фиксированы:
    0 - ADDR_DATA_0,
    1 - ADDR_DATA_0 + 32 * 1024,
    2 - ADDR_DATA_0 + 64 * 1024,
    3 - ADDR_DATA_0 + 96 * 1024,
    4 - ADDR_DATA_1,
    и т.д.
    Информация о состоянии хранилища хранится в ArrayDatas, который представляет собой массив.
*/
/*
    Принцип хранения несбрасываемых настроек.
    У несбрасываемых настроек есть одна особенность - там хранятся достаточно важные настройки, которые нельзя терять.
    В обычнх, сбрасываемых, настройках их хранить опасно. Если изменится размер структуры данных, стандартный алгоритм чтения не сможет прочитать их
    и загрузит настройки по умолчанию. Поэтому в сбрасываемых настройках должно быть соблюдено следующиее условие:
    порядок следования полей структуры не должен изменяться - новые настройки могут лишь добавляться в конец структуры.
    Весь сектор, предназначенный для хранения несбрасываемых настроек, поделен на участки размером 64 байта (могут быть и другие значения, кратные 16.
    пока достаточно 64. Для того, чтобы определить, сколько реально занимает участок, в его первом байте записан множитель, на который нужно умножить
    16, чтобы получить размер участка - для 64 байт - 4, для 128 - 8, и т.д.
    Располагая такой информацией, мы просто скачем по первым байтам участков и ищем тот, в который ничего ещё не было записано (в нём будет 0xff). 
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Программа и константные данные
#define ADDR_SECTOR_BOOT_0      ((uint)0x08000000)          // 16k +
#define ADDR_SECTOR_BOOT_1      ((uint)0x08004000)          // 16k | Загрузчик
#define ADDR_SECTOR_BOOT_2      ((uint)0x08008000)          // 16k /
#define ADDR_FLASH_SECTOR_3     ((uint)0x0800C000)          // 16k  TODO Здесь будут храниться идентификационные данные прибора - серийный номер, версия ПО
#define ADDR_SECTOR_NR_SETTINGS ((uint)0x08010000)          // 64k  Несбрасываемые настройки
#define SIZE_SECTOR_NR_SETTINGS (64 * 1024)                 // Размер сектора для хранения несбрасываемых настроек
#define SIZE_NR_SET_PARAGRAPH   (512)                       // Это размер одного параграфа, в котором, кроме настроек, сохранена ещё и его длина в первом слове
                                                            // (Сначала сделал 128 байт, но оказалось мало - уже как раз 128, поэтому ввёл четырёхкратный запас)
#define SIZE_NR_SETTINGS        (SIZE_NR_SET_PARAGRAPH - 4) // Размер структуры для хранения несбрасываемых настроек
#define ADDR_SECTOR_PROGRAM_0   ((uint)0x08020000)          // 128k Основная программа
#define ADDR_SECTOR_PROGRAM_1   ((uint)0x08040000)          // 128k Основная программа
#define ADDR_SECTOR_PROGRAM_2   ((uint)0x08060000)          // 128k Основная программа
#define ADDR_FLASH_SECTOR_8     ((uint)0x08080000)          // 128k
#define ADDR_FLASH_SECTOR_9     ((uint)0x080A0000)          // 128k
#define ADDR_SECTOR_RESOURCES   ((uint)0x080C0000)          // 128k Графические и звуковые ресурсы
#define ADDR_SECTOR_SETTINGS    ((uint)0x080E0000)          // 128k Настройки
#define ADDR_FLASH_SECTOR_12    ((uint)0x08100000)          // 16k
#define ADDR_FLASH_SECTOR_13    ((uint)0x08104000)          // 16k
#define ADDR_FLASH_SECTOR_14    ((uint)0x08108000)          // 16k
#define ADDR_FLASH_SECTOR_15    ((uint)0x0810C000)          // 16k
#define ADDR_DATA_DATA          ((uint)0x08110000)          // 64k  Здесь будем сохранять массивы адресов с нашими данными
#define ADDR_DATA_0             ((uint)0x08120000)          // 128k +
#define ADDR_DATA_1             ((uint)0x08140000)          // 128k |
#define ADDR_DATA_2             ((uint)0x08160000)          // 128k |
#define ADDR_DATA_3             ((uint)0x08180000)          // 128k | Здесь будут храниться собственно данные
#define ADDR_DATA_4             ((uint)0x081A0000)          // 128k |
#define ADDR_DATA_5             ((uint)0x081C0000)          // 128k |
#define ADDR_DATA_TEMP          ((uint)0x081E0000)          // 128k /

#define SIZE_SECTOR_SETTINGS    (128 * 1024)                // Размер сектора, куда сохраняются настройки, в байтах

#define SIZE_SECTOR_128         (128 * 1024)

typedef struct
{
    struct ElementData
    {
        uint address;               // Адрес, по которому хранятся данные канала. Для каждого номера этот адрес уникален, но, тем не менее,
                                    // присутствует в структуре - значение MAX_UINT означает, что данных нету.
                                    // Значение datas[0].address == 0 означает, что элемент стёрт и нужно переходить к следующему
        DataSettings ds;
    } datas[MAX_NUM_SAVED_WAVES];
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

#undef READ_BYTE
#define READ_BYTE(address) (*((volatile uint8*)address))

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


#define ADDR_ARRAY_RECORDS          ((ADDR_SECTOR_SETTINGS) + 4)
#define MAX_NUM_RECORDS             1024
#define SIZE_ARRAY_RECORDS_IN_BYTES ((MAX_NUM_RECORDS) * sizeof(RecordConfig))
static const uint ADDR_FIRST_SET    = ADDR_ARRAY_RECORDS + SIZE_ARRAY_RECORDS_IN_BYTES;     // Адрес первого байта буфера, где сохранены настройки
static const uint ADDR_LAST_SET     = ((ADDR_SECTOR_SETTINGS + SIZE_SECTOR_SETTINGS) - 1);  // Адрес последнего байта буфера, где сохранены настройки


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
static void WriteBufferBytes(uint address, void *buffer, int numBytes);
static void PrepareSectorForData(void);
static void ReadBufferBytes(uint addressSrc, void *bufferDest, int size);
static bool EraseSector(uint startAddress);
static uint GetSector(uint startAddress);
static RecordConfig* LastFilledRecord(void);        // Возвращает адрес последней записи с сохранёнными настройками или 0, если нет сохранённых настроек
static bool LoadNonResetSettings(void);
static void SaveNonResetSettings(void);

// Для данных
static ArrayDatas* CurrentArray(void);              // Возвращает адрес актуального массива с адресами данных
static uint AddressSectorForData(int num);          // Возвращает адрес сектора, в котором сохранены данные с номером num
static uint AddressForData(int num);                // Возвращает адрес, по которому должны быть сохранены данные num
static void SaveArrayDatas(ArrayDatas array);       // Перезаписать текущий ArrayDatas
static void SaveData(int num, DataSettings *ds, uint8 *dataA, uint8 *dataB);        // Сохранение данных. Хранилище должно быть подготовлено - данные предварительно стёрты,
                                                    // т.е. везде записано FF, стирать ничего не нужно, только записывать

static uint AddressSectorForAddress(uint address);  // Возвращает адрес сектора, которому принадлежить адрес address


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FLASH_LoadSettings(bool onlyNonReset)
{
    CLEAR_FLASH_FLAGS;

    uint value = READ_WORD(ADDR_SECTOR_SETTINGS);
    
    if (value != MARK_OF_FILLED)       // Если первый байт сектора не отмаркирован - первое включение прибора
    {
        set.common.countErasedFlashSettings = 0;
        set.common.countEnables = 0;
        set.common.countErasedFlashData = 0;
        set.common.workingTimeInSecs = 0;
        EraseSector(ADDR_SECTOR_SETTINGS);                      // На всякий случай стираем сектор
        EraseSector(ADDR_SECTOR_NR_SETTINGS);
        WriteWord(ADDR_SECTOR_SETTINGS, MARK_OF_FILLED);        // И маркируем
        PrepareSectorForData();                                 // Также готовим сектор для сохранения данных
        return;
    }

    RecordConfig *record = LastFilledRecord();
    if (record == 0)                                            // По какой-то причине сохранённых настроек может не оказаться. Например, сектор был промаркирован при предыдущем включении,
    {                                                           // но прибор выключили выключателем на задней стенке, а не кнопкой на передней панели, вследствие чего настройки не сохранились
        return;
    }

    // Сначала пытаемся загрузить несбрасываемые настройки из своего сектора

    bool nonResetLoaded = LoadNonResetSettings();

    if (onlyNonReset)
    {
        return;
    }

    if (nonResetLoaded)
    {
        if (sizeof(set) == record->size)
        {
            ReadBufferBytes(record->addr, &set, record->size);
        }
    }
    else                                                        // Если настройки сохранены по старому методу - в одном секторе
    {
        ReadBufferBytes(record->addr, &setNR, sizeof(setNR));   // То считываем только первую часть - где хранятся несбрасываемые настройки
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_SaveSettings(void)
{
    if(gBF.alreadyLoadSettings == 0)
    {
        return;
    }

    CLEAR_FLASH_FLAGS

    SaveNonResetSettings();
    
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool LoadNonResetSettings(void)
{
    // Первым делом проверим, есть ли такие настройки в специально предназначенном секторе

    if (READ_WORD(ADDR_SECTOR_NR_SETTINGS) != MAX_UINT)                         // Если в первом слове уже что-то записано, значит, настройки там сохранены
    {
        uint address = ADDR_SECTOR_NR_SETTINGS;
        uint lastAddress = ADDR_SECTOR_NR_SETTINGS + SIZE_SECTOR_NR_SETTINGS;
        while (address < lastAddress)
        {
            if (READ_WORD(address) == MAX_UINT)
            {
                break;
            }
            address += SIZE_NR_SET_PARAGRAPH;
        }
        address -= SIZE_NR_SET_PARAGRAPH;
        ReadBufferBytes(address + 4, &setNR, sizeof(setNR));
        return true;
    }
    return false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SaveNonResetSettings(void)
{
    if (READ_WORD(ADDR_SECTOR_NR_SETTINGS) != SIZE_NR_SETTINGS)     // Если настройки ещё не сохранялись либо же сохранённые настройки не того размера
    {
        EraseSector(ADDR_SECTOR_NR_SETTINGS);
    }

    // Теперь найдём адрес для сохранения

    uint address = ADDR_SECTOR_NR_SETTINGS;
    const uint LAST_ADDRESS = ADDR_SECTOR_NR_SETTINGS + SIZE_SECTOR_NR_SETTINGS;

    while (address < LAST_ADDRESS)
    {
        if (READ_WORD(address) == MAX_UINT)
        {
            break;
        }
        address += SIZE_NR_SET_PARAGRAPH;
    }

    if (address >= LAST_ADDRESS)                    // Нет места для сохранения
    {
        EraseSector(ADDR_SECTOR_NR_SETTINGS);       // Тогда обнуляем сектор
        address = ADDR_SECTOR_NR_SETTINGS;
    }

    WriteWord(address, SIZE_NR_SETTINGS);

    WriteBufferBytes(address + 4, (void*)(&setNR), sizeof(setNR));
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
        LOG_ERROR_TRACE("Не могу записать в память");
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
            LOG_ERROR_TRACE("Не могу записать в флеш-память");
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
static void ReadBufferBytes(uint addressSrc, void *bufferDest, int size)
{
    uint8 *src = (uint8*)addressSrc;
    uint8 *dest = (uint8*)bufferDest;

    for (int i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static bool EraseSector(uint startAddress)
{
    if (GetSector(startAddress) == MAX_UINT)    // если неизвестный сектор
    {
        return false;
    }

    CLEAR_FLASH_FLAGS

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef flashITD;
    flashITD.TypeErase = TYPEERASE_SECTORS;
    flashITD.Sector = GetSector(startAddress);
    flashITD.NbSectors = 1;
    flashITD.VoltageRange = VOLTAGE_RANGE_3;

    uint32_t error = 0;

    while(gBF.soundIsBeep) // WARN Здесь ждём, пока бикалка не закончит. Костыль, надо разобраться, почему они не могут вместе работать
    {
    };

    HAL_FLASHEx_Erase(&flashITD, &error);

    HAL_FLASH_Lock();

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static uint GetSector(uint startAddress)
{
    typedef struct
    {
        uint number;
        uint startAddress;
    } StructSector;

    static const StructSector sectors[] =
    {
        {FLASH_SECTOR_4, ADDR_SECTOR_NR_SETTINGS},  // Сюда сохраняются несбрасываемые настройки    64 кБ
        {FLASH_SECTOR_11, ADDR_SECTOR_SETTINGS},    // Сюда сохраняются сбрасываеме настройки       128 кБ
        {FLASH_SECTOR_16, ADDR_DATA_DATA},          // Массив адресов данных                        64 кБ
        {FLASH_SECTOR_17, ADDR_DATA_0},             // + 
        {FLASH_SECTOR_18, ADDR_DATA_1},             // |
        {FLASH_SECTOR_19, ADDR_DATA_2},             // |
        {FLASH_SECTOR_20, ADDR_DATA_3},             // | Здесь собственно данные                    6 х 128кБ = 768 кБ
        {FLASH_SECTOR_21, ADDR_DATA_4},             // |
        {FLASH_SECTOR_22, ADDR_DATA_5},             // |
        {FLASH_SECTOR_23, ADDR_DATA_TEMP},          // /
        {0, 0}
    };

    int i = 0;
    while (sectors[i].startAddress)
    {
        if (sectors[i].startAddress == startAddress)
        {
            return sectors[i].number;
        }
        i++;
    }

    LOG_ERROR_TRACE("Неправильный адрес сектора %d", startAddress);

    return MAX_UINT;
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
    return (CurrentArray()->datas[num].address != MAX_UINT);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_DeleteAllData(void)
{
    EraseSector(ADDR_DATA_DATA);
    EraseSector(ADDR_DATA_0);
    EraseSector(ADDR_DATA_1);
    EraseSector(ADDR_DATA_2);
    EraseSector(ADDR_DATA_3);
    EraseSector(ADDR_DATA_4);
    EraseSector(ADDR_DATA_5);
    EraseSector(ADDR_DATA_TEMP);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_DeleteData(int num)
{
    if (!FLASH_ExistData(num))
    {
        return;
    }

    /*
        Чтобы стереть данные канала, перепишем сектор с данными во вспомогательный сектор, сотрём наш сектор, а затем перепишем все байты обратно,
        кроме тех, которые заняты данными канала.
        Далее перезапишем новый ArrayDatas, в котором установим address в MAX_UINT.
    */

    EraseSector(ADDR_DATA_TEMP);

    uint addressSector = AddressSectorForData(num);

    WriteBufferWords(ADDR_DATA_TEMP, (void*)addressSector, SIZE_SECTOR_128 / 4);

    EraseSector(addressSector);

    int numNotWritted = num % 4;                // Здесь мы находим четверть, которую не нужно переписывать, потому что в ней хранятся наши данные, которые
                                                // нужно стереть. Это будет остаток от деления на 4, потому что в каждом секторе хранится ровно 4 набора данных
    uint sizeQuartPart = SIZE_SECTOR_128 / 4;   // Размер одной четвёртой части сектора в байтах

    for (int i = 0; i < 4; i++)
    {
        if (i != numNotWritted)
        {
            WriteBufferWords(addressSector + sizeQuartPart * i, (void*)(ADDR_DATA_TEMP + sizeQuartPart * i), sizeQuartPart / 4);
        }
    }
    
    ArrayDatas array = *CurrentArray();

    array.datas[num].address = MAX_UINT;                        // Пишем признак того, что данные num ещё не записаны
    memset(&array.datas[num].ds, 0xff, sizeof(DataSettings));   // Заполняем место для структуры DataSettings значением 0xff, чтобы корректно произошёл
                                                                // процесс записи в бубущем
    SaveArrayDatas(array);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint AddressSectorForData(int num)
{
    return AddressSectorForAddress(AddressForData(num));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint AddressForData(int num)
{
    return ADDR_DATA_0 + (SIZE_SECTOR_128) / 4 * num;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint AddressSectorForAddress(uint address)
{
    const uint addresses[] =
    {
        ADDR_SECTOR_BOOT_0,
        ADDR_SECTOR_BOOT_1,
        ADDR_SECTOR_BOOT_2,
        ADDR_FLASH_SECTOR_3,
        ADDR_SECTOR_NR_SETTINGS,
        ADDR_SECTOR_PROGRAM_0,
        ADDR_SECTOR_PROGRAM_1,
        ADDR_SECTOR_PROGRAM_2,
        ADDR_FLASH_SECTOR_8,
        ADDR_FLASH_SECTOR_9,
        ADDR_SECTOR_RESOURCES,
        ADDR_SECTOR_SETTINGS,
        ADDR_FLASH_SECTOR_12,
        ADDR_FLASH_SECTOR_13,
        ADDR_FLASH_SECTOR_14,
        ADDR_FLASH_SECTOR_15,
        ADDR_DATA_DATA,
        ADDR_DATA_0,
        ADDR_DATA_1,
        ADDR_DATA_2,
        ADDR_DATA_3,
        ADDR_DATA_4,
        ADDR_DATA_5,
        ADDR_DATA_TEMP,
        0
    };

    if (address < addresses[0] || address >= ADDR_DATA_TEMP + SIZE_SECTOR_128)
    {
        LOG_ERROR_TRACE("Адрес за пределами ППЗУ");
    }

    const uint *pointer = &addresses[1];

    while (*pointer)
    {
        if (address < *pointer)
        {
            pointer--;
            return *pointer;
        }
        pointer++;
    }

    return ADDR_DATA_TEMP;
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


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_SaveData(int num, DataSettings *ds, uint8 *dataA, uint8 *dataB)
{
    FLASH_DeleteData(num);              // Сначала сотрём данные по этому номеру

    SaveData(num, ds, dataA, dataB);    // Теперь сохраним данные этого номера
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static ArrayDatas* CurrentArray(void)
{
    ArrayDatas *array = (ArrayDatas*)ADDR_DATA_DATA;

    while (array->datas[0].address == 0)    // Нулевое значение означает, что элемент затёрт.
    {
        array++;                            // Поэтому переходим к следующему, чтобы проверить уже его актуальность
    }

    return array;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SaveArrayDatas(ArrayDatas array)
{
    uint address = (uint)CurrentArray() + sizeof(ArrayDatas);   // Адрес, по которому будем записывать array

    WriteWord((uint)&CurrentArray()->datas[0].address, 0);      // Запишем в текущий массив признак того, что он уже не текущий

    if (address + sizeof(ArrayDatas) >= ADDR_DATA_0)            // Если для записи array в секторе нет места
    {
        EraseSector(ADDR_DATA_DATA);                            // то сотрём этот сектор
        address = ADDR_DATA_DATA;                               // и записывать array будем в начале его
    }

    WriteBufferBytes(address, (void*)&array, sizeof(ArrayDatas));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SaveData(int num, DataSettings *ds, uint8 *dataA, uint8 *dataB)
{
    if (FLASH_ExistData(num))
    {
        LOG_ERROR_TRACE("Данные не стёрты");
    }

    uint address = AddressForData(num);
    int sizeChannel = NumBytesInChannel(ds);

    if (ds->enableChA)
    {
        WriteBufferBytes(address, dataA, sizeChannel);
        address += sizeChannel;
    }

    if (ds->enableChB)
    {
        WriteBufferBytes(address, dataB, sizeChannel);
    }

    ArrayDatas *array = CurrentArray();
    
    WriteWord((uint)&array->datas[num].address, AddressForData(num));
    WriteBufferBytes((uint)&array->datas[num].ds, (void*)ds, sizeof(DataSettings));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FLASH_GetData(int num, DataSettings **ds, uint8 **dataA, uint8 **dataB)
{
    *ds = 0;
    *dataA = 0;
    *dataB = 0;

    ArrayDatas array = *CurrentArray();

    uint addrData = array.datas[num].address;

    if (addrData == MAX_UINT)   // Если данных нет
    {
        return false;           // сообщаем об этом пользователю
    }

    *ds = &array.datas[num].ds;

    if ((*ds)->enableChA)
    {
        *dataA = (uint8*)addrData;
        addrData += NumBytesInChannel(*ds);
    }

    if ((*ds)->enableChB)
    {
        *dataB = (uint8*)addrData;
    }

    return true;
}

#undef CLEAR_FLASH_FLAGS
#undef FIRST_RECORD
#undef READ_WORD
#undef RECORD_EXIST
#undef MEMORY_FOR_SETTINGS
