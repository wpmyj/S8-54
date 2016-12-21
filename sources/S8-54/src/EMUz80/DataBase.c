#include "defines.h"
#include "DataBase.h"
#include "RAM48.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
    Действовать будем так.
    Есть несколько точек входа. Декомпиляцю всегда начинаем с этих адресов.
    Все 65536 адресов делим на 15 отрезков по 8 кБ. 15 - потому что каждый отрезок перекрывает два соседних

         |       |         |         |         |         |          |         |
         |   1   |    3    |    5    |    7    |    9    |    11    |   13    |
         |       |         |         |         |         |          |         |
    +----+---+---+----+----+----+----+----+----+----+----+-----+----+----+----+----+
    |        |        |         |         |         |          |         |         |
    |    0   |   2    |    4    |    6    |    8    |   10     |   12    |   14    |
    |        |        |         |         |         |          |         |         |

    Далее расчитываем границы блоков

    n - чётное:
        addressBegin = n * 8 * 1024
        addressEnd = (n + 1) * 8 * 1024 - 1
    n - нечётное:
        addressBegin = n * 8 * 1024 + 4 * 1024
        addressEnd = (n + 1) * 8 * 1024 + 4 * 1024 - 1

    
*/

#define SIZE_SEGMENT (8 * 1024)

typedef struct
{
    uint16 address;
    int8 data[SIZE_SEGMENT];    // 0 - ещё не заполнено
                                // > 0 - количество байт, занимаемых командов
                                // -1 - второй байт команды
                                // -2 - третий байт команды
                                // -3 - четвёртый байт команды
                                // -4 - пятый байт команды
                                // -5 - этот байт не занят комндой. В нём мусор или данные
                                // -6 - этот байт не занят командой. В нём данные
} Segment8k;


static Segment8k *segment = 0;  // Здесь будет структура, описывающая сегмент
static Section *sections = 0;   // Здесь будет массив структур, описывающих секции

static uint16 CalculateStartAddress(uint16 address);
static void FillSegment(uint16 startAddress);
static void FillSections(void);
static void FillSection(uint16 address);
static bool IncludedSection(uint16 address);    // Возвращает true, если команда по адресу address уже учтена в какой-либо секции
static void AddToSections(OutStruct *params);   // Добавляет в секцию команду, описанную структурой params
static void MergeSections(void);
static void AddSection(Section section);
static void MoveSectionsRight(int position);    // Сдвинуть все секции, начиная с позиции position, вправо
static void MoveSectionLeft(int position);      // Сдвинуть все секции, начиная с позиции position, влево
static bool IsCompactedSections(void);          // Возвращает true, если нет граничащих секций (т.е. таких смежных секций, у которых граничат начало и конец)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataBase_Init(Section sections_[NUM_SECTIONS])
{
    sections = sections_;
    
    segment = malloc(sizeof(Segment8k));
    segment->address = 1;
    FillSections();
    FillSegment(0);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void DataBase_GetData(uint16 address, OutStruct params[20])
{
    uint16 startAddress = CalculateStartAddress(address);
    FillSegment(startAddress);

    int writedParams = 0;

    int curAddress = (int)address - (int)startAddress;

    while (writedParams < 20 && (curAddress < SIZE_SEGMENT))
    {
        if (segment->data[curAddress] > 0)
        {
            Decode(curAddress + startAddress, &params[writedParams]);
            writedParams++;
        }
        curAddress++;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
uint16 CalculateStartAddress(uint16 address)
{
    return address / (4 * 1024);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FillSegment(uint16 startAddress)
{
    if (startAddress == segment->address)
    {
        return;
    }

    memset(segment->data, 0, SIZE_SEGMENT);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FillSections(void)
{
    memset(sections, 0, sizeof(Section) * NUM_SECTIONS);
    FillSection(0x00);
    FillSection(0x08);
    FillSection(0x10);
    FillSection(0x18);
    FillSection(0x20);
    FillSection(0x28);
    FillSection(0x30);
    FillSection(0x38);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
void FillSection(uint16 address)
{
    if (IncludedSection(address))
    {
        return;
    }

    OutStruct params;
    Decode(address, &params);

    AddToSections(&params);

    for (int i = 0; i < params.numAddresses; i++)
    {
        FillSection((uint16)params.addresses[i]);
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static bool IncludedSection(uint16 address)
{
    if (address < 0x0f && sections[0].end != 0) // для первой секции - особое условие, потому что для неё нельзя применить проверку на нулевоей адрес
    {
        return true;
    }

    for (int i = 1; i < NUM_SECTIONS; i++)
    {
        if (sections[i].end == 0)   // Если закончили просмотр всех секций - выход
        {
            return false;
        }

if (sections[i].start <= address && sections[i].end >= address)
{
    return true;
}
    }

    return false;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void AddToSections(OutStruct *params)
{
    uint16 address = (uint16)params->address;
    int numBytes = params->numOpCodes;

    Section section;
    section.start = address;
    section.end = address + (uint16)numBytes - 1;

    AddSection(section);

    while (!IsCompactedSections())
    {
        MergeSections();
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void AddSection(Section section)
{
    if (section.start == 0x00)
    {
        sections[0] = section;
    }
    else
    {
        for (int i = 0; i < NUM_SECTIONS; i++)
        {
            if (sections[i].end == 0)
            {
                sections[i] = section;
                break;
            }
            else if (sections[i + 1].start == 0)
            {
                sections[i + 1] = section;
                break;
            }
            else if (sections[i].end < section.start && sections[i + 1].start >= section.end)
            {

            }
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void MoveSectionsRight(int position)
{
    int lastPosition = position;
    while (sections[lastPosition + 1].start != 0)
    {
        lastPosition++;
    }

    for (; lastPosition >= position; lastPosition--)
    {
        sections[lastPosition + 1] = sections[lastPosition];
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void MoveSectionLeft(int position)
{
    do
    {
        sections[position - 1] = sections[position];
        sections[position].start = 0;
        sections[position].end = 0;
    } while (sections[++position].start);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static void MergeSections(void)
{
    for (int i = 1; i < NUM_SECTIONS; i++)
    {
        if (sections[i].start == 0)
        {
            return;
        }
        if ((sections[i - 1].end + 1) == sections[i].start)
        {
            sections[i - 1].end = sections[i].end;
            MoveSectionLeft(i + 1);
            return;
        }

        if (sections[i + 1].start == 0)
        {
            return;
        }
        if (sections[i].end + 1 == sections[i + 1].start)
        {
            sections[+1].start = sections[i].start;
            MoveSectionLeft(i + 1);
            return;
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsCompactedSections(void)
{
    for (int i = 1; i < NUM_SECTIONS; i++)
    {
        if (sections[i].start == 0)
        {
            return true;
        }
        if (sections[i - 1].end + 1 == sections[i].start)
        {
            return false;
        }
    }
    return true;
}
