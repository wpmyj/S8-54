#include "Menu.h"
#include "MenuItemsLogic.h"
#include "Globals.h"
#include "MenuFunctions.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Settings/Settings.h"
#include "Hardware/Hardware.h"
#include "Hardware/Sound.h"
#include "Hardware/RTC.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Pages/PageDisplay.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Структура используется для анимации элементов ГУИ Choice и Governor

typedef enum
{
    NONE,
    INCREASE,
    DECREASE
} DIRECTION;

typedef struct
{
    void *address;  // Адрес элемента. Если 0 - не движется
    uint timeStart; // Время начала анимации в миллисекундах
    DIRECTION dir;  // Направление изменения значения
} TimeStruct;

static TimeStruct tsChoice = {0, 0, NONE};
static TimeStruct tsGovernor = {0, 0, NONE};

int8 gCurDigit = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------------------------------------------------
void Choice::StartChange(int delta)
{
    if (tsChoice.address != 0)
    {
        return;
    }
    Sound_GovernorChangedValue();
    if (HINT_MODE_ENABLE)
    {
        SetItemForHint(this);
    }
    else if (!ItemIsAcitve(this))
    {
        CHOICE_RUN_FUNC_CHANGED(this, false);
    }
    else
    {
        tsChoice.address = this;
        tsChoice.timeStart = gTimeMS;
        tsChoice.dir = delta > 0 ? INCREASE : DECREASE;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float Choice::Step()
{
    static const float speed = 0.1f;
    static const int numLines = 12;
    if (tsChoice.address == this)
    {
        float delta = speed * (gTimeMS - tsChoice.timeStart);
        if (delta == 0.0f)
        {
            delta = 0.001f; // Таймер в несколько первых кадров может показать, что прошло 0 мс, но мы возвращаем большее число, потому что ноль будет говорить о том, что движения нет
        }
        int8 index = *cell;
        if (tsChoice.dir == INCREASE)
        {
            if (delta <= numLines)
            {
                return delta;
            }
            CircleIncreaseInt8(&index, 0, (int8)NumSubItems() - 1);
        }
        else if (tsChoice.dir == DECREASE)
        {
            delta = -delta;

            if (delta >= -numLines)
            {
                return delta;
            }
            CircleDecreaseInt8(&index, 0, (int8)NumSubItems() - 1);
        }
        *cell = index;
        tsChoice.address = 0;
        CHOICE_RUN_FUNC_CHANGED(this, ItemIsAcitve(this));
        NEED_FINISH_DRAW = 1;
        tsChoice.dir = NONE;
        return 0.0f;
    }
    return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Choice::ChangeIndex(int delta)
{
    int index = *cell;
    if (delta < 0)
    {
        ++index;
        if (index == NumSubItems())
        {
            index = 0;
        }
    }
    else
    {
        --index;
        if (index < 0)
        {
            index = NumSubItems() - 1;
        }
    }
    *cell = (int8)index;
    CHOICE_RUN_FUNC_CHANGED(this, ItemIsAcitve(this));
    Sound_GovernorChangedValue();
    NEED_FINISH_DRAW = 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Choice::NumSubItems()
{
    int i = 0;
    for (; i < MAX_NUM_SUBITEMS_IN_CHOICE; i++)
    {
        if (names[i][LANG] == 0)
        {
            return i;
        }
    }
    return i;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::StartChange(int delta)
{
    Sound_GovernorChangedValue();
    if (delta > 0 && tsGovernor.address == this && tsGovernor.dir == INCREASE)
    {
        *cell = NextValue();
    }
    else if (delta < 0 && tsGovernor.address == this && tsGovernor.dir == DECREASE)
    {
        *cell = PrevValue();
    }
    else
    {
        tsGovernor.timeStart = gTimeMS;
        tsGovernor.address = this;
    }
    tsGovernor.dir = delta > 0 ? INCREASE : DECREASE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int16 Governor::NextValue()
{
    return ((*cell) < maxValue) ? (*cell) + 1 : minValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int16 Governor::PrevValue()
{
    return ((*cell) > minValue) ? (*cell) - 1 : maxValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float Governor::Step()
{
    static const float speed = 0.05f;
    static const int numLines = 10;
    float delta = 0.0f;
    if (tsGovernor.address == this)
    {
        delta = speed * (gTimeMS - tsGovernor.timeStart);
        if (tsGovernor.dir == DECREASE)
        {
            delta *= -1.0f;
            if (delta == 0.0f)
            {
                return -0.001f;
            }
            if (delta < -numLines)
            {
                tsGovernor.dir = NONE;
                *cell = PrevValue();
                if (funcOfChanged)
                {
                    funcOfChanged();
                }
                delta = 0.0f;
                tsGovernor.address = 0;
            }
        }
        else if (tsGovernor.dir == INCREASE)
        {
            if (delta == 0.0f)
            {
                return 0.001f;
            }
            if (delta > numLines)
            {
                tsGovernor.dir = NONE;
                *cell = NextValue();
                if (funcOfChanged)
                {
                    funcOfChanged();
                }
                delta = 0.0f;
                tsGovernor.address = 0;
            }
        }
    }
    return delta;
}



































































//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Governor_ChangeValue(Governor *governor, int delta)
{
    int16 oldValue = *governor->cell;
    *governor->cell += (int16)(math.Sign(delta) * math.Pow10(gCurDigit));
    LIMITATION(*governor->cell, governor->minValue, governor->maxValue);
    if (*governor->cell != oldValue)
    {
        if (governor->funcOfChanged)
        {
            governor->funcOfChanged();
        }
        Sound_GovernorChangedValue();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void IPaddress_ChangeValue(IPaddress *ip, int delta)
{
    int numByte = 0;
    int numPos = 0;

    IPaddress_GetNumPosIPvalue(&numByte, &numPos);

    int oldValue = 0;

    if (numByte < 4)
    {
        uint8 *bytes = ip->ip0;
        oldValue = bytes[numByte];
    }
    else
    {
        oldValue = *ip->port;
    }

    int newValue = oldValue + math.Sign(delta) * math.Pow10(numPos);
    LIMITATION(newValue, 0, numByte == 4 ? 65535 : 255);

    if (oldValue != newValue)
    {
        if (numByte == 4)
        {
            *ip->port = (int16)newValue;
        }
        else
        {
            ip->ip0[numByte] = (int8)newValue;
        }
        Sound_GovernorChangedValue();
        display.ShowWarning(NeedRebootDevice);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void MACaddress_ChangeValue(MACaddress *mac, int delta)
{
    uint8 *value = mac->mac0 + gCurDigit;
    *value += delta > 0 ? 1 : -1;
    Sound_GovernorChangedValue();
    display.ShowWarning(NeedRebootDevice);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void IPaddress_GetNumPosIPvalue(int *numIP, int *selPos)
{
    if (gCurDigit < 12)
    {
        *numIP = gCurDigit / 3;
        *selPos = 2 - (gCurDigit - (*numIP * 3));
    }
    else
    {
        *numIP = 4;
        *selPos = 4 - (gCurDigit - 12);
    }


}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Governor_NextPosition(Governor *governor)
{
    if (OpenedItem() == governor)
    {
        CircleIncreaseInt8(&gCurDigit, 0, (int8)(Governor_NumDigits(governor) - 1));
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Governor_NumDigits(Governor *governor)
{
    int min = math.NumDigitsInNumber(math.FabsInt(governor->minValue));
    int max = math.NumDigitsInNumber(math.FabsInt(governor->maxValue));
    if (min > max)
    {
        max = min;
    }
    return max;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void IPaddress_NextPosition(IPaddress *ipEthernet_IP)
{
    CircleIncreaseInt8(&gCurDigit, 0, ipEthernet_IP->port == 0 ? 11 : 16);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Time_SetOpened(Time *item)
{
    PackedTime time = RTC_GetPackedTime();
    *(item->seconds) = (int8)time.seconds;
    *(item->minutes) = (int8)time.minutes;
    *(item->hours) = (int8)time.hours;
    *(item->day) = (int8)time.day;
    *(item->month) = (int8)time.month;
    *(item->year) = (int8)time.year ;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Time_SetNewTime(Time *time)
{
    RTC_SetTimeAndData(*time->day, *time->month, *time->year, *time->hours, *time->minutes, *time->seconds);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Time_SelectNextPosition(Time *time)
{
    CircleIncreaseInt8(time->curField, 0, 7);
    painter.ResetFlash();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Time_IncCurrentPosition(Time *time)
{
    Sound_GovernorChangedValue();
    int8 *value[] = {0, time->day, time->month, time->year, time->hours, time->minutes, time->seconds};
    int8 position = *time->curField;
    if (position != iSET && position != iEXIT)
    {
        static const int8 max[] = {0, 31, 12, 99, 23, 59, 59};
        static const int8 min[] = {0, 1, 1, 15, 0, 0, 0};
        *(value[position]) = (*(value[position]))++;
        if (*value[position] > max[position])
        {
            *value[position] = min[position];
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Time_DecCurrentPosition(Time *time)
{
    Sound_GovernorChangedValue();
    static const int8 max[] = {0, 31, 12, 99, 23, 59, 59};
    static const int8 min[] = {0, 1, 1, 15, 0, 0, 0};
    int8 *value[] = {0, time->day, time->month, time->year, time->hours, time->minutes, time->seconds};
    int8 position = *time->curField;
    if (position != iSET && position != iEXIT)
    {
        (*(value[position]))--;
        if (*value[position] < min[position])
        {
            *value[position] = max[position];
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void GovernorColor_ChangeValue(GovernorColor *governor, int delta)
{
    ColorType *ct = governor->colorType;
    if (ct->currentField == 0)
    {
        ct->BrightnessChange(delta);
        Sound_GovernorChangedValue();
    }
    else
    {
        ct->ComponentChange(delta);
        Sound_GovernorChangedValue();
    }

    PageService_InitGlobalColors();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void SBPage_SetCurrent(const Page *page)
{
    if (ItemIsAcitve(page))
    {
        if (SHOW_STRING_NAVI_TEMP)
        {
            menu.TemporaryEnableStrNavi();
        }
        SetCurrentItem(page, true);
        OpenItem(page, !ItemIsOpened(page));
    }
}
