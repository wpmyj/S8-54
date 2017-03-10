#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspConsole;


// ОТЛАДКА - КОНСОЛЬ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Governor mgConsole_NumStrings =
{
    Item_Governor, &mspConsole, 0,
    {
        "Число строк", "Number strings",
        "",
        ""
    },
    &CONSOLE_NUM_STRINGS, 0, 33
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_SizeFont =
{
    Item_Choice, &mspConsole, 0,
    {
        "Размер шрифта", "Size font",
        "",
        ""
    },
    {
        {"5", "5"},
        {"8", "8"}
    },
    &set.debug.sizeFont
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_ModeStop =
{
    Item_Choice, &mspConsole, 0,
    {
        "Реж. останова", "Mode stop",
        "Предоставляет возможность приостановки вывода в консоль путём нажатия на кнопку ПУСК/СТОП",
        "It provides the ability to pause the output to the console by pressing the ПУСК/СТОП button"
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.modePauseConsole
};

const Page mspConsole_Registers;

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_ShowAll =
{
    Item_Choice, &mspConsole_Registers, 0,
    {
        "Показывать все", "Show all",
        "Показывать все значения, засылаемые в регистры",
        "To show all values transferred in registers"
    },
    {
        {"Нет", "No"},
        {"Да", "Yes"}
    },
    (int8*)&set.debug.show.all
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_Registers(void)
{
    return set.debug.show.all;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RD_FL =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "RD_FL", "RD_FL",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.flag
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RShiftA =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "U см. 1к", "U shift 1ch",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.rShift[A]
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RShiftB =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "U см. 2к", "U shift 2ch",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.range[B]
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_TrigLev =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "U синхр.", "U trig.",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.trigLev
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RangeA =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "ВОЛЬТ/ДЕЛ 1", "Range 1",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.range[A]
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RangeB =
{
    Item_Choice, &mspConsole_Registers, IsActive_Registers,
    {
        "ВОЛЬТ/ДЕЛ 2", "Range 2",
        "",
        ""
    },
    {
        {DISABLE_RU, DISABLE_EN},
        {ENABLE_RU, ENABLE_EN}
    },
    (int8*)&set.debug.show.range[B]
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Page mspConsole_Registers =
{
    Item_Page, &mspConsole, 0,
    {
        "РЕГИСТРЫ", "REGISTERS",
        "",
        ""
    },
    Page_DebugShowRegisters,
    {
        (void*)&mcConsole_Registers_ShowAll,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Показывать все
        (void*)&mcConsole_Registers_RD_FL,        // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - RD_FL
        (void*)&mcConsole_Registers_RShiftA,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 1к
        (void*)&mcConsole_Registers_RShiftB,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 2к
        (void*)&mcConsole_Registers_TrigLev,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U синхр
        (void*)&mcConsole_Registers_RangeA,       // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 1
        (void*)&mcConsole_Registers_RangeB,       // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 2
        (void*)&mcConsole_Registers_TrigParam,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. синхр.
        (void*)&mcConsole_Registers_ChanParamA,   // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 1
        (void*)&mcConsole_Registers_ChanParamB,   // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2
        (void*)&mcConsole_Registers_TBase,        // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВРЕМЯ/ДЕЛ
        (void*)&mcConsole_Registers_TShift        // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Т см.
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Page mspConsole =
{
    Item_Page, &mpDebug, 0,
    {
        "КОНСОЛЬ", "CONSOLE",
        "",
        ""
    },
    Page_DebugConsole,
    {
        (void*)&mgConsole_NumStrings,   // ОТЛАДКА - КОНСОЛЬ - Число строк
        (void*)&mcConsole_SizeFont,     // ОТЛАДКА - КОНСОЛЬ - Размер шрифта
        (void*)&mcConsole_ModeStop,     // ОТЛАДКА - КОНСОЛЬ - Реж. останова
        (void*)&mspConsole_Registers,   // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ
        (void*)&mbConsole_SizeSettings  // ОТЛАДКА - КОНСОЛЬ - Разм. настроек

    }
};
