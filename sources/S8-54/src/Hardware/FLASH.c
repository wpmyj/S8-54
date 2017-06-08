// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "FLASH.h"
#include "Globals.h"
#include "Log.h"
#include "Hardware/Hardware.h"
#include "Hardware/Timer.h"
#include "Hardware/Sound.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Программа и константные данные
#define ADDR_SECTOR_BOOT_0      ((uint)0x08000000)          ///< 16k +
#define ADDR_SECTOR_BOOT_1      ((uint)0x08004000)          ///< 16k | Загрузчик
#define ADDR_SECTOR_BOOT_2      ((uint)0x08008000)          ///< 16k /
#define ADDR_FLASH_SECTOR_3     ((uint)0x0800C000)          ///< 16k
#define ADDR_FLASH_SECTOR_4     ((uint)0x08010000)          ///< 64k
#define ADDR_SECTOR_PROGRAM_0   ((uint)0x08020000)          ///< 128k Основная программа
#define ADDR_SECTOR_PROGRAM_1   ((uint)0x08040000)          ///< 128k Основная программа
#define ADDR_SECTOR_PROGRAM_2   ((uint)0x08060000)          ///< 128k Основная программа
#define ADDR_FLASH_SECTOR_8     ((uint)0x08080000)          ///< 128k
#define ADDR_FLASH_SECTOR_9     ((uint)0x080A0000)          ///< 128k
#define ADDR_SECTOR_RESOURCES   ((uint)0x080C0000)          ///< 128k Графические и звуковые ресурсы
#define ADDR_SECTOR_SETTINGS    ((uint)0x080E0000)          ///< 128k Настройки
#define ADDR_FLASH_SECTOR_12    ((uint)0x08100000)          ///< 16k
#define ADDR_FLASH_SECTOR_13    ((uint)0x08104000)          ///< 16k
#define ADDR_FLASH_SECTOR_14    ((uint)0x08108000)          ///< 16k
#define ADDR_FLASH_SECTOR_15    ((uint)0x0810C000)          ///< 16k
#define ADDR_DATA_DATA          ((uint)0x08110000)          ///< 64k  Здесь будем сохранять массивы адресов с нашими данными
#define ADDR_DATA_0             ((uint)0x08120000)          ///< 128k +
#define ADDR_DATA_1             ((uint)0x08140000)          ///< 128k |
#define ADDR_DATA_2             ((uint)0x08160000)          ///< 128k |
#define ADDR_DATA_3             ((uint)0x08180000)          ///< 128k | Здесь будут храниться собственно данные
#define ADDR_DATA_4             ((uint)0x081A0000)          ///< 128k |
#define ADDR_DATA_5             ((uint)0x081C0000)          ///< 128k |
#define ADDR_DATA_TEMP          ((uint)0x081E0000)          ///< 128k /

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

static const uint startDataInfo = ADDR_DATA_1;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#undef CLEAR_FLASH_FLAGS
#define CLEAR_FLASH_FLAGS                                                                   \
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP     |  /* end of operation flag              */   \
                            FLASH_FLAG_OPERR  |  /* operation error flag               */   \
                            FLASH_FLAG_WRPERR |  /* write protected error flag         */   \
                            FLASH_FLAG_PGAERR |  /* programming alignment error flag   */   \
                            FLASH_FLAG_PGPERR |  /* programming parallelism error flag */   \
                            FLASH_FLAG_PGSERR);  /* programming sequence error flag    */

#define READ_BYTE(address) (*((volatile uint8*)address))

#define READ_HALF_WORD(address) (*((volatile uint16*)address))

#define READ_WORD(address) (*((volatile uint*)address))


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Для настроек

// Для данных
static void PrepareSectorForData(void);
static ArrayDatas* CurrentArray(void);              // Возвращает адрес актуального массива с адресами данных
static uint AddressSectorForData(int num);          // Возвращает адрес сектора, в котором сохранены данные с номером num
static uint AddressForData(int num);                // Возвращает адрес, по которому должны быть сохранены данные num
static void SaveArrayDatas(ArrayDatas array);       // Перезаписать текущий ArrayDatas
static void SaveData(int num, DataSettings *ds, uint8 *dataA, uint8 *dataB);        // Сохранение данных. Хранилище должно быть подготовлено - данные 
                                                    // предварительно стёрты, т.е. везде записано FF, стирать ничего не нужно, только записывать

static uint AddressSectorForAddress(uint address);  // Возвращает адрес сектора, которому принадлежить адрес address

static void WriteWord(uint address, uint word);
static void WriteBufferWords(uint address, void *buffer, int numWords);
static void WriteBufferBytes(uint address, void *buffer, int numBytes);
static void ReadBufferBytes(uint addressSrc, void *bufferDest, int size);
static bool EraseSector(uint startAddress);
static uint GetSector(uint startAddress);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FLASH_LoadSettings(void)
{
    uint16 numBytes = READ_HALF_WORD(ADDR_SECTOR_SETTINGS);

    if (numBytes == 0xffff)         // Если в первых байтах сектора для настроек записаны все единицы, значит, сохранение настроек ещё не 
    {                               // производилось - первое сохранение настроек.
        PrepareSectorForData();     // Поэтому подготавливаем сектор для сохранения сигналов
        return;                     // и выходим
    }

    // Находим область сектора с сохранёнными настройками. Настройки записываются последовательно друг за другом по адресам, кратным 1024
    int i = 0;
    int address = ADDR_SECTOR_SETTINGS;
    for (; i < 128; i++)
    {
        if (READ_HALF_WORD(address) == 0xffff)
        {
            break;
        }
        address += 1024;
    }

    // Если i == 128, то сектор заполнен полностью - нужно читать из последней области сектора.
    // Уменьшаем i на один и читаем.

    address = ADDR_SECTOR_SETTINGS + (i - 1) * 1024;

    // Читаем в Settings set количество байт, указанное в (int16)*address

    ReadBufferBytes(address, &set, READ_HALF_WORD(address));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_SaveSettings(void)
{
    // Записываем в Settings.size текущий размер структуры Settings
    set.size = sizeof(Settings);

    // Находим первый свободынй адрес записи.
    int address = ADDR_SECTOR_SETTINGS;
    while (READ_HALF_WORD(address) != 0xffff &&             // Пока по адресу, кратному 1024, не записаны настройки
           address < (ADDR_SECTOR_SETTINGS + 1024 * 128))   // и мы не вышли за пределы сектора настроек
    {
        address += 1024;                                    // переходим на следующую предполагаемую область записи
    }

    if (address == ADDR_SECTOR_SETTINGS + 1024 * 128)       // Если адрес указывает на следующий сектор
    {
        EraseSector(GetSector(ADDR_SECTOR_SETTINGS));       // То стираем к лешему заполненный сетор настроек
        address = ADDR_SECTOR_SETTINGS;
    }

    WriteBufferBytes(address, &set, sizeof(Settings));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PrepareSectorForData(void)
{
    EraseSector(ADDR_DATA_1);
    for(int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        WriteWord(startDataInfo + i * 4, 0);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ReadBufferBytes(uint addressSrc, void *bufferDest, int size)
{
    uint8 *src = (uint8*)addressSrc;
    uint8 *dest = (uint8*)bufferDest;

    for (int i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
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

    Sound_WaitCompletion();

    HAL_FLASHEx_Erase(&flashITD, &error);

    HAL_FLASH_Lock();

    return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint GetSector(uint startAddress)
{
    typedef struct
    {
        uint number;
        uint startAddress;
    } StructSector;

    static const StructSector sectors[] =
    {
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_GetDataInfo(bool existData[MAX_NUM_SAVED_WAVES])
{
    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        existData[i] = FLASH_ExistData(i);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
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

    int numNotWritted = num % 4;                // Здесь мы находим четверть, которую не нужно переписывать, потому что в ней хранятся наши данные, 
                                                // которые нужно стереть. Это будет остаток от деления на 4, потому что в каждом секторе хранится 
                                                // ровно 4 набора данных
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
        ADDR_FLASH_SECTOR_4,
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


//----------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteBufferBytes(uint address, void *buffer, int size)
{
    Sound_WaitCompletion();
    
    CLEAR_FLASH_FLAGS

    HAL_FLASH_Unlock();
    for(int i = 0; i < size; i++)
    {
        uint64_t data = ((uint8*)buffer)[i];
        HAL_FLASH_Program(TYPEPROGRAM_BYTE, address, data);
        address++;
    }
    HAL_FLASH_Lock();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
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

    if (ENABLED_A(ds))
    {
        WriteBufferBytes(address, dataA, sizeChannel);
        address += sizeChannel;
    }

    if (ENABLED_B(ds))
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

    if (ENABLED_A(*ds))
    {
        *dataA = (uint8*)addrData;
        addrData += NumBytesInChannel(*ds);
    }

    if (ENABLED_B(*ds))
    {
        *dataB = (uint8*)addrData;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool OTP_SaveSerialNumber(char *servialNumber)
{
    // Находим первую пустую строку длиной 16 байт в области OTP, начиная с начала
    uint8 *address = (uint8*)FLASH_OTP_BASE;

    while ((*address) != 0xff &&                    // *address != 0xff означает, что запись в эту строку уже производилась
           address < (uint8*)FLASH_OTP_END - 16)
    {
        address += 16;
    }

    if (address < (uint8*)FLASH_OTP_END - 16)
    {
        WriteBufferBytes((uint)address, (uint8*)servialNumber, strlen(servialNumber) + 1);
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int OTP_GetSerialNumber(char buffer[17])
{
    /// \todo улучшить - нельзя разбрасываться байтами. Каждая запись должна занимать столько места, сколько в ней символов, а не 16, как сейчас.

    const int allShotsMAX = 512 / 16;   // Максимальное число записей в OTP серийного номера.

    uint8* address = (uint8*)FLASH_OTP_END - 15;

    do
    {
        address -= 16;
    } while(*address == 0xff && address > (uint8*)FLASH_OTP_BASE);

    if (*address == 0xff)   // Не нашли строки с информацией, дойдя до начального адреса OTP
    {
        buffer[0] = 0;
        return allShotsMAX;
    }

    strcpy(buffer, (char*)address);

    return allShotsMAX - (address - (uint8*)FLASH_OTP_BASE) / 16 - 1;
}

#undef CLEAR_FLASH_FLAGS
#undef READ_WORD
#undef READ_HALF_WORD
