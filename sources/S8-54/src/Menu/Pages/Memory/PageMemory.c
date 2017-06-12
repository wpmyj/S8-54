// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "PageMemory.h"
#include "Menu/Pages/Definition.h"
#include "Menu/FileManager.h"
#include "Menu/MenuFunctions.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGAtypes.h"
#include "FPGA/Data.h"
#include "FPGA/DataStorage.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/font/Font.h"
#include "Display/Painter.h"
#include "Display/Grid.h"
#include "Display/Symbols.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "FlashDrive/FlashDrive.h"
#include "Hardware/FLASH.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Log.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;

extern void PressSB_FM_Tab(void);

static const Choice mcLengthMemory;
static bool IsActive_MemoryLength(void);
       void OnChange_MemoryLength(bool active);


static const Page mspLast;
static void OnPress_Last(void);
static void OnDraw_Last(void);
static void OnRot_Last(int angle);

static const SmallButton sbLast_Exit;
static void OnPress_Last_Exit(void);

static const SmallButton sbLast_Next;
static void OnPress_Last_Next(void);
static void Draw_Last_Next(int x, int y);

static const SmallButton sbLast_Prev;
static void OnPress_Last_Prev(void);
static void Draw_Last_Prev(int x, int y);

static const SmallButton sbLast_IntEnter;
static void OnPress_Last_IntEnter(void);
static void Draw_Last_IntEnter(int x, int y);

static const SmallButton sbLast_SaveToDrive;
static void OnPress_Last_SaveToDrive(void);
static void Draw_Last_SaveToDrive(int x, int y);


static const Page mspInt;
static void OnPress_Int(void);
static void OnDraw_Int(void);
static void OnReg_Int(int delta);

static const SmallButton sbInt_Exit;
static void OnPress_Int_Exit(void);

static const SmallButton sbInt_ShowSignalsAlways;
static void OnPress_Int_ShowSignalAlways(void);
static void Draw_Int_ShowSignalAlways(int x, int y);
static void Draw_Int_ShowSignalAllways_Yes(int x, int y);
static void Draw_Int_ShowSignalAllways_No(int x, int y);

static const SmallButton sbInt_ModeShow;
static void OnPress_Int_ModeShow(void);
static void Draw_Int_ModeShow(int x, int y);
static void Draw_Int_ModeShow_Direct(int x, int y);
static void Draw_Int_ModeShow_Saved(int x, int y);
static void Draw_Int_ModeShow_Both(int x, int y);

static const SmallButton sbInt_EraseAll;
static void OnPress_Int_EraseAll(void);
static void Draw_Int_EraseAll(int x, int y);

static const SmallButton sbInt_SaveToMemory;
static void OnPress_Int_SaveToMemory(void);
static void Draw_Int_SaveToMemory(int x, int y);

static const SmallButton sbInt_SaveToDrive;
static void OnPress_Int_SaveToDrive(void);
static void Draw_Int_SaveToDrive(int x, int y);


static const Page mspDrive;
static const Choice mcDrive_Name;
static const Choice mcDrive_ModeSave;
static const Choice mcDrive_ModeBtnMemory;
static const Choice mcDrive_Autoconnect;


static const Page mspDrive_Manager;
static bool FuncOfActiveExtMemFolder(void);
       void OnPressMemoryExtFileManager(void);

static const SmallButton sbExitFileManager;
static void PressSB_FM_Exit(void);

static const SmallButton sbFileManagerTab;
static void DrawSB_FM_Tab(int x, int y);

static const SmallButton sbFileManagerLevelUp;
static void DrawSB_FM_LevelUp(int x, int y);

static const SmallButton sbFileManagerLevelDown;
static void DrawSB_FM_LevelDown(int x, int y);


static const Page mspSetMask;
static bool IsActiveMemoryExtSetMask(void);
static void OnPressMemoryExtMask(void);
static void OnMemExtSetMaskRegSet(int angle);

static const SmallButton sbExitSetMask;
static void PressSB_SetMask_Exit(void);

static const SmallButton sbSetMaskDelete;
static void PressSB_SetMask_Delete(void);
static void DrawSB_SetMask_Delete(int x, int y);

static const SmallButton sbSetMaskBackspace;
static void PressSB_SetMask_Backspace(void);
static void DrawSB_SetMask_Backspace(int x, int y);

static const SmallButton sbSetMaskInsert;
static void PressSB_SetMask_Insert(void);
static void DrawSB_SetMask_Insert(int x, int y);

    const Page mpSetName;
static void OnMemExtSetNameRegSet(int angle);
static const SmallButton sbExitSetName;
static void PressSB_SetName_Exit(void);
static const SmallButton sbSetNameDelete;
static void PressSB_SetName_Delete(void);
static void DrawSB_SetName_Delete(int x, int y);
static const SmallButton sbSetNameBackspace;
static void PressSB_SetName_Backspace(void);
static void DrawSB_SetName_Backspace(int x, int y);
static const SmallButton sbSetNameInsert;
static void PressSB_SetName_Insert(void);
static void DrawSB_SetName_Insert(int x, int y);
static const SmallButton sbSetNameSave;
static void PressSB_MemExtSetNameSave(void);
static void DrawSB_MemExtSetNameSave(int x, int y);


static void DrawSetMask(void);  // Эта функция рисует, когда выбран режим задания маски.
static void DrawFileMask(int x, int y);
static void DrawSetName(void);  // Эта функция рисует, когда нужно задать имя файла для сохранения

static void DrawMemoryWave(int num, bool exist);
static void SaveSignalToIntMemory(void);
       void OnMemExtSetMaskNameRegSet(int angle, int maxIndex);


// ПЯМЯТЬ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page pMemory =
{
    Item_Page, &mainPage, 0,
    {
        "ПАМЯТЬ", "MEMORY",
        "Работа с внешней и внутренней памятью.",
        "Working with external and internal memory."
    },
    Page_Memory,
    {
        (void*)&mcLengthMemory, // ПАМЯТЬ - Точки
        (void*)&mspLast,        // ПАМЯТЬ - Последние
        (void*)&mspInt,         // ПАМЯТЬ - ВНУТР ЗУ
        (void*)&mspDrive        // ПАМЯТЬ - ВНЕШН ЗУ
    }
};

// ПАМЯТЬ - Точки ------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcLengthMemory =
{
    Item_Choice, &pMemory, IsActive_MemoryLength,
    {
        "Длина памяти", "Mem length"
        ,
        "Выбор количества отсчётов для сохраняемых сигналов. "
        "При увеличении количества отсчётов уменьшается количество сохранённых в памяти сигналов."
        ,
        "Choice of number of counting for the saved signals. "
        "At increase in number of counting the quantity of the signals kept in memory decreases."
    },
    {
        {"512", "512"},
        {"1к",  "1k"},
        {"2к",  "2k"},
        {"4к",  "4k"},
        {"8к",  "8k"},
        {"16к", "16k"}
        //,{"32к", "32k"}
    },
    (int8*)&FPGA_NUM_POINTS, OnChange_MemoryLength
};

static bool IsActive_MemoryLength(void)
{
    return SET_PEACKDET_DIS;
}

void OnChange_MemoryLength(bool active)
{
    // Если включен пиковый детектор, то не можем переключать память
    if (SET_PEACKDET_EN && !active)
    {
        Display_ShowWarning(WrongModePeackDet);
        return;
    }

    // Блокируем включение 32к длины записи, если включен второй канал
    if (FPGA_NUM_POINTS_32k && SET_ENABLED_B)
    {
        Display_ShowWarning(DisableChannel2);
        FPGA_NUM_POINTS = FNP_16k;
    }

    int width = GridWidth();
    
    FPGA_Reset();
    if (sMemory_NumBytesInChannel(false) == 281)
    {
        SHIFT_IN_MEMORY = 0;
    }
    else
    {
        if (SET_PEACKDET_EN)
        {
            width *= 2;
        }
        if (TPOS_LEFT)
        {
            SHIFT_IN_MEMORY = 0;
        }
        else if (TPOS_CENTER)
        {
            SHIFT_IN_MEMORY = (int16)(sMemory_NumBytesInChannel(false) / 2 - width / 2);
        }
        else if (TPOS_RIGHT)
        {
            SHIFT_IN_MEMORY = (int16)(sMemory_NumBytesInChannel(false) - width - 2);
        }
    }
    FPGA_Reset();
    FPGA_SetTShift(SET_TSHIFT);
    FPGA_Reset();
    //LOG_WRITE("shift %d, num %d, width %d", SHIFT_IN_MEMORY, sMemory_NumBytesInChannel(false), width);
}

// ПАМЯТЬ - Последние --------------------------------------------------------------------------------------------------------------------------------
static const Page mspLast =
{
    Item_Page, &pMemory, 0,
    {
        "ПОСЛЕДНИЕ", "LATEST",
        "Переход в режим работы с последними полученными сигналами",
        "Transition to an operating mode with the last received signals"
    },
    Page_SB_MemLatest,
    {
        (void*)&sbLast_Exit,        // ПАМЯТЬ - ПОСЛЕДНИЕ - Выход
        (void*)0,
        (void*)&sbLast_Next,        // ПАМЯТЬ - ПОСЛЕДНИЕ - Следующий
        (void*)&sbLast_Prev,        // ПАМЯТЬ - ПОСЛЕДНИЕ - Предыдущий
        (void*)&sbLast_IntEnter,    // ПАМЯТЬ - ПОСЛЕДНИЕ - Внутр ЗУ
        (void*)&sbLast_SaveToDrive  // ПАМЯТЬ - ПОСЛЕДНИЕ - Сохранить
    },
    OnPress_Last, OnDraw_Last, OnRot_Last
};

static void OnPress_Last(void)
{
    gMemory.currentNumLatestSignal = 0;
    gMemory.runningFPGAbeforeSmallButtons = FPGA_IsRunning() ? 1 : 0;
    FPGA_Stop(false);
    MODE_WORK = ModeWork_Latest;
}

static void OnDraw_Last(void)
{
    char buffer[20];

    int width = 40;
    int height = 10;
    Painter_FillRegionC(GridRight() - width, GRID_TOP, width, height, gColorBack);
    Painter_DrawRectangleC(GridRight() - width, GRID_TOP, width, height, gColorFill);
    Painter_DrawText(GridRight() - width + 2, GRID_TOP + 1, Int2String(gMemory.currentNumLatestSignal + 1, false, 3, buffer));
    Painter_DrawText(GridRight() - width + 17, GRID_TOP + 1, "/");
    Painter_DrawText(GridRight() - width + 23, GRID_TOP + 1, Int2String(DS_NumElementsInStorage(), false, 3, buffer));
}

static void OnRot_Last(int angle)
{
    if (DS_NumElementsInStorage() > 1)
    {
        Sound_RegulatorSwitchRotate();
    }
    if (Math_Sign(angle) > 0)
    {
        OnPress_Last_Next();
    }
    else
    {
        OnPress_Last_Prev();
    }
}

// ПАМЯТЬ - ПОСЛЕДНИЕ - Выход ------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbLast_Exit =
{
    Item_SmallButton, &mspLast,
    COMMON_BEGIN_SB_EXIT,
    OnPress_Last_Exit,
    DrawSB_Exit
};

static void OnPress_Last_Exit(void)
{
    MODE_WORK = ModeWork_Direct;
    if (gMemory.runningFPGAbeforeSmallButtons == 1)
    {
        FPGA_Start();
        gMemory.runningFPGAbeforeSmallButtons = 0;
    }
    Display_RemoveAddDrawFunction();
}

// ПАМЯТЬ - ПОСЛЕДНИЕ - Следующий --------------------------------------------------------------------------------------------------------------------
static const SmallButton sbLast_Next =
{
    Item_SmallButton, &mspLast, 0,
    {
        "Следующий", "Next",
        "Перейти к следующему сигналу",
        "Go to the next signal"
    },
    OnPress_Last_Next,
    Draw_Last_Next
};

static void OnPress_Last_Next(void)
{
    CircleIncreaseInt16(&gMemory.currentNumLatestSignal, 0, (int16)(DS_NumElementsInStorage() - 1));
}

static void Draw_Last_Next(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 2, '\x64');
    Painter_SetFont(TypeFont_8);
}

// ПАМЯТЬ - ПОСЛЕДНИЕ - Предыдущий -------------------------------------------------------------------------------------------------------------------
static const SmallButton sbLast_Prev =
{
    Item_SmallButton, &mspLast, 0,
    {
        "Предыдущий", "Previous",
        "Перейти к предыдущему сигналу",
        "Go to the previous signal"
    },
    OnPress_Last_Prev,
    Draw_Last_Prev
};

static void OnPress_Last_Prev(void)
{
    CircleDecreaseInt16(&gMemory.currentNumLatestSignal, 0, (int16)(DS_NumElementsInStorage() - 1));
}

static void Draw_Last_Prev(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 2, '\x20');
    Painter_SetFont(TypeFont_8);
}

// ПАМЯТЬ - ПОСЛЕДНИЕ - Внутр ЗУ ---------------------------------------------------------------------------------------------------------------------
static const SmallButton sbLast_IntEnter =
{
    Item_SmallButton, &mspLast, 0,
    {
        "Внутр ЗУ", "Internal storage",
        "Нажмите эту кнопку, чтобы сохранить сигнал во внутреннем запоминающем устройстве",
        "Press this button to keep a signal in an internal memory"
    },
    OnPress_Last_IntEnter,
    Draw_Last_IntEnter
};

static void OnPress_Last_IntEnter(void)
{
    OpenPageAndSetItCurrent(Page_SB_MemInt);
    MODE_WORK = ModeWork_EEPROM;
    Data_GetFromIntMemory();
    gMemory.exitFromIntToLast = 1;
}

static void Draw_Last_IntEnter(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x40');
    Painter_SetFont(TypeFont_8);
}

// ПАМЯТЬ - ПОСЛЕДНИЕ - Сохранить --------------------------------------------------------------------------------------------------------------------
static const SmallButton sbLast_SaveToDrive =
{
    Item_SmallButton, &mspLast, 0,
    {
        "Сохранить", "Save",
        "Кнопка становится доступна при присоединённом внешнем ЗУ. Позволяет сохранить сигнал на внешем ЗУ",
        "Click this button to save the signal on the external FLASH"
    },
    OnPress_Last_SaveToDrive,
    Draw_Last_SaveToDrive
};

static void OnPress_Last_SaveToDrive(void)
{
    gMemory.exitFromModeSetNameTo = RETURN_TO_LAST_MEM;
    Memory_SaveSignalToFlashDrive();
}

void Memory_SaveSignalToFlashDrive(void)
{
    if (gFlashDriveIsConnected)
    {
        if (FILE_NAMING_MODE_MANUAL)
        {
            OpenPageAndSetItCurrent(Page_SB_MemExtSetName);
            Display_SetAddDrawFunction(DrawSetName);
        }
        else
        {
            gMemory.needForSaveToFlashDrive = 1;
        }
    }
    else
    {
        gMemory.exitFromModeSetNameTo = 0;
    }
}

static void DrawSetName(void)
{
    int x0 = GridLeft() + 40;
    int y0 = GRID_TOP + 60;
    int width = GridWidth() - 80;
    int height = 80;

    Painter_DrawRectangleC(x0, y0, width, height, gColorFill);
    Painter_FillRegionC(x0 + 1, y0 + 1, width - 2, height - 2, gColorBack);

    int index = 0;
    int position = 0;
    int deltaX = 10;
    int deltaY0 = 5;
    int deltaY = 12;

    // Рисуем большие буквы английского алфавита
    while (symbolsAlphaBet[index][0] != ' ')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0);
        index++;
        position++;
    }

    // Теперь рисуем цифры и пробел
    position = 0;
    while (symbolsAlphaBet[index][0] != 'a')
    {
        DrawStr(index, x0 + deltaX + 50 + position * 7, y0 + deltaY0 + deltaY);
        index++;
        position++;
    }

    // Теперь рисуем малые буквы алфавита
    position = 0;
    while (symbolsAlphaBet[index][0] != '%')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0 + deltaY * 2);
        index++;
        position++;
    }

    int x = Painter_DrawTextC(x0 + deltaX, y0 + 65, FILE_NAME, gColorFill);
    Painter_FillRegionC(x, y0 + 65, 5, 8, COLOR_FLASH_10);
}

static void Draw_Last_SaveToDrive(int x, int y)
{
    if (gFlashDriveIsConnected)
    {
        Painter_SetFont(TypeFont_UGO2);
        Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Painter_SetFont(TypeFont_8);
    }
}

// ПАМЯТЬ - ВНУТР ЗУ ---------------------------------------------------------------------------------------------------------------------------------
static const Page mspInt =
{
    Item_Page, &pMemory, 0,
    {
        "ВНУТР ЗУ", "INT STORAGE",
        "Переход в режим работы с внутренней памятью",
        "Transition to an operating mode with internal memory"
    },
    Page_SB_MemInt,
    {
        (void*)&sbInt_Exit,                 // ПАМЯТЬ - ВНУТР ЗУ - Выход
        (void*)&sbInt_ShowSignalsAlways,    // ПАМЯТЬ - ВНУТР ЗУ - Показывать всегда
        (void*)&sbInt_ModeShow,             // ПАМЯТЬ - ВНУТР ЗУ - Вид сигнала
        //(void*)0,
        (void*)&sbInt_EraseAll,
        (void*)&sbInt_SaveToMemory,         // ПАМЯТЬ - ВНУТР ЗУ - Сохранить
        (void*)&sbInt_SaveToDrive           // ПАМЯТЬ - ВНУТР ЗУ - Сохранить на флешку
    },
    OnPress_Int, OnDraw_Int, OnReg_Int
};

static void OnPress_Int(void)
{
    OpenPageAndSetItCurrent(Page_SB_MemInt);
    MODE_WORK = ModeWork_EEPROM;
}

static void OnDraw_Int(void)
{
    // Теперь нарисуем состояние памяти

    bool exist[MAX_NUM_SAVED_WAVES] = {false};

    FLASH_GetDataInfo(exist);

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        DrawMemoryWave(i, exist[i]);
    }
}

static void DrawMemoryWave(int num, bool exist)
{
    char buffer[20];

    int x = GridLeft() + 2 + num * 12;
    int y = GridFullBottom() - 10;
    int width = 12;
    Painter_FillRegionC(x, y, width, 10, num == gMemory.currentNumIntSignal ? COLOR_FLASH_10 : gColorBack);
    Painter_DrawRectangleC(x, y, width, 10, gColorFill);
    Painter_SetColor(num == gMemory.currentNumIntSignal ? COLOR_FLASH_01 : gColorFill);
    if (exist)
    {
        Painter_DrawText(x + 2, y + 1, Int2String(num + 1, false, 2, buffer));
    }
    else
    {
        Painter_DrawText(x + 3, y + 1, "\x88");
    }
}

static void OnReg_Int(int delta)
{
    Sound_RegulatorSwitchRotate();
    if (delta < 0)
    {
        CircleDecreaseInt8(&gMemory.currentNumIntSignal, 0, MAX_NUM_SAVED_WAVES - 1);
    }
    else if (delta > 0)
    {
        CircleIncreaseInt8(&gMemory.currentNumIntSignal, 0, MAX_NUM_SAVED_WAVES - 1);
    }
    Data_GetFromIntMemory();
    Painter_ResetFlash();
}

// ПАМЯТЬ - ВНУТР ЗУ - Выход -------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbInt_Exit =
{
    Item_SmallButton, &mspInt, 0,
    {
        "Выход", "Exit",
        "Кнопка для выхода в предыдущее меню",
        "Button to return to the previous menu"
    },
    OnPress_Int_Exit,
    DrawSB_Exit
};

static void OnPress_Int_Exit(void)
{
    Data_GetFromIntMemory();
    if (gMemory.exitFromIntToLast == 1)
    {
        OpenPageAndSetItCurrent(Page_SB_MemLatest);
        MODE_WORK = ModeWork_Latest;
        gMemory.exitFromIntToLast = 0;
    }
    else
    {
        MODE_WORK = ModeWork_Direct;
        if (gMemory.runningFPGAbeforeSmallButtons == 1)
        {
            FPGA_Start();
            gMemory.runningFPGAbeforeSmallButtons = 0;
        }
        Display_RemoveAddDrawFunction();
        //ShortPressOnPageItem(PagePointerFromName(Page_SB_MemInt), 0);
    }
}

// ПАМЯТЬ - ВНУТР ЗУ - Показывать всегда -------------------------------------------------------------------------------------------------------------
static const SmallButton sbInt_ShowSignalsAlways =
{
    Item_SmallButton, &mspInt, 0,
    {
        "Показывать всегда", "To show always",
        "Позволяет всегда показывать выбранный сохранённый сигнал поверх текущего",
        "Allows to show always the chosen kept signal over the current"
    },
    OnPress_Int_ShowSignalAlways,
    Draw_Int_ShowSignalAlways,
    {
        {
            Draw_Int_ShowSignalAllways_Yes,
            "показывать выбранный сигнал из внутренней памяти поверх текущего",
            "to show the chosen signal from internal memory over the current"
        },
        {
            Draw_Int_ShowSignalAllways_No,
            "сигнал из внутренней памяти виден только в режиме работы с внутренним запоминающим устройством",
            "the signal from internal memory is visible only in an operating mode with an internal memory"
        }
    }
};

static void OnPress_Int_ShowSignalAlways(void)
{
    gMemory.alwaysShowMemIntSignal = (gMemory.alwaysShowMemIntSignal == 0) ? 1 : 0;
}

static void Draw_Int_ShowSignalAlways(int x, int y)
{
    if (gMemory.alwaysShowMemIntSignal == 0)
    {
        Draw_Int_ShowSignalAllways_No(x, y);
    }
    else
    {
        Draw_Int_ShowSignalAllways_Yes(x, y);
    }
}

static void Draw_Int_ShowSignalAllways_Yes(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x66');
    Painter_SetFont(TypeFont_8);
}

static void Draw_Int_ShowSignalAllways_No(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x68');
    Painter_SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНУТР ЗУ - Вид сигнала -------------------------------------------------------------------------------------------------------------------
static const SmallButton sbInt_ModeShow =
{
    Item_SmallButton, &mspInt, 0,
    {
        "Вид сигнала", "Type of a signal",
        "Показывать записанный или текущий сигнал в режиме ВНУТР ЗУ",
        "Show recorded or current signal in mode Internal Memory"
    },
    OnPress_Int_ModeShow,
    Draw_Int_ModeShow,
    {
        {
            Draw_Int_ModeShow_Direct,
            "на дисплее текущий сигнал",
            "on the display current signal"
        },
        {
            Draw_Int_ModeShow_Saved,
            "на дисплее сохранённый сигнал",
            "on the display the kept signal"
        },
        {
            Draw_Int_ModeShow_Both,
            "на дисплее оба сигнала",
            "on the display both signals"
        }
    }
};

static void OnPress_Int_ModeShow(void)
{
    CircleIncreaseInt8((int8*)&SHOW_IN_INT, 0, 2);
}

static void Draw_Int_ModeShow(int x, int y)
{
    if (SHOW_IN_INT_DIRECT)
    {
        Draw_Int_ModeShow_Direct(x, y);
    }
    else if (SHOW_IN_INT_SAVED)
    {
        Draw_Int_ModeShow_Saved(x, y);
    }
    else
    {
        Draw_Int_ModeShow_Both(x, y);
    }
}

static void Draw_Int_ModeShow_Direct(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x6a');
    Painter_SetFont(TypeFont_8);
}


static void Draw_Int_ModeShow_Saved(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x6c');
    Painter_SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Draw_Int_ModeShow_Both(int x, int y)
{
    Painter_DrawText(x + 1, y + 5, "ОБА");
    Painter_DrawVLineC(x + 1, y + 2, y + 14, gColorBack);
    Painter_DrawVLineC(x + 2, y + 6, y + 11, gColorFill);
}

// ПАМЯТЬ - ВНУТР ЗУ - Стереть всё -------------------------------------------------------------------------------------------------------------------
static const SmallButton sbInt_EraseAll =
{
    Item_SmallButton, &mspInt, 0,
    {
        "Стереть всё", "Erase all",
        "Стирает все данные из области хранения данных, включая область иноформации. Тотальное форматирование",
        "It erases all data from the storage area, including the area inoformatsiya. The total format"
    },
    OnPress_Int_EraseAll,
    Draw_Int_EraseAll,
    {
        {
            Draw_Int_EraseAll,
            "Стереть все данные",
            "Erase all data"
        }
    }
};

static void OnPress_Int_EraseAll(void)
{
    Display_FuncOnWaitStart("Стираю. Подождите", "Erase. Wait", false);
    FLASH_DeleteAllData();
    Display_FuncOnWaitStop();
}

static void Draw_Int_EraseAll(int x, int y)
{
    Painter_DrawText(x + 5, y + 5, "E");
}

// ПАМЯТЬ - ВНУТР ЗУ - Сохранить в памяти ------------------------------------------------------------------------------------------------------------
static const SmallButton sbInt_SaveToMemory =
{
    Item_SmallButton, &mspInt, 0,
    {
        "Сохранить в памяти", "Save to memory",
        "Сохранить сигнал во внутреннем запоминующем устройстве",
        "To keep a signal in an internal memory"
    },
    OnPress_Int_SaveToMemory,
    Draw_Int_SaveToMemory
};

static void OnPress_Int_SaveToMemory(void)
{
    Display_FuncOnWaitStart("Записываю в память", "I stored in memory", false);
    SaveSignalToIntMemory();
    Display_FuncOnWaitStop();
}

static void SaveSignalToIntMemory(void)
{
    if (gMemory.exitFromIntToLast == 1)
    {
        if (DS)
        {
            FLASH_SaveData(gMemory.currentNumIntSignal, DS, DATA_LAST(A), DATA_LAST(B));
            Data_GetFromIntMemory();
            Display_ShowWarning(SignalIsSaved);
        }
    }
    else
    {
        if (DS != 0)
        {
            FLASH_SaveData(gMemory.currentNumIntSignal, DS, DATA(A), DATA(B));
            FLASH_GetData(gMemory.currentNumIntSignal, &DS, &DATA_INT(A), &DATA_INT(B));
            Display_ShowWarning(SignalIsSaved);
        }
    }
}

static void Draw_Int_SaveToMemory(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x2c');
    Painter_SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНУТР ЗУ - Сохранить на флешку -----------------------------------------------------------------------------------------------------------
static const SmallButton sbInt_SaveToDrive =
{
    Item_SmallButton, &mspInt, 0,
    {
        "Сохранить на флешку", "Save to disk",
        "Сохраняет сигнал на флешку",
        "Save signal to flash drive"
    },
    OnPress_Int_SaveToDrive,
    Draw_Int_SaveToDrive
};

static void OnPress_Int_SaveToDrive(void)
{
    gMemory.exitFromModeSetNameTo = RETURN_TO_INT_MEM;
    Memory_SaveSignalToFlashDrive();
}

static void Draw_Int_SaveToDrive(int x, int y)
{
    if (gFlashDriveIsConnected)
    {
        Painter_SetFont(TypeFont_UGO2);
        Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Painter_SetFont(TypeFont_8);
    }
}

// ПАМЯТЬ - ВНЕШН ЗУ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspDrive =
{
    Item_Page, &pMemory, 0,
    {
        "ВНЕШН ЗУ", "EXT STORAGE",
        "Работа с внешним запоминающим устройством.",
        "Work with external storage device."
    },
    Page_MemoryExt,
    {
        (void*)&mspDrive_Manager,       // ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ
        (void*)&mcDrive_Name,           // ПАМЯТЬ - ВНЕШН ЗУ - Имя файла
        (void*)&mspSetMask,             // Память - ВНЕШН ЗУ - МАСКА
        (void*)&mcDrive_ModeSave,       // ПАМЯТЬ - ВНЕШН ЗУ - Сохранять как
        (void*)&mcDrive_ModeBtnMemory,  // ПАМЯТЬ - ВНЕШН ЗУ - Реж кн ПАМЯТЬ
        (void*)&mcDrive_Autoconnect     // ПАМЯТЬ - ВНЕШН ЗУ - Автоподключение
    }
};

// ПАМЯТЬ - ВНЕШН ЗУ - Имя файла ---------------------------------------------------------------------------------------------------------------------
static const Choice mcDrive_Name =
{
    Item_Choice, &mspDrive, 0,
    {
        "Имя файла", "File name"
        ,
        "Задаёт режим наименования файлов при сохранении на внешний накопитель:\n"
        "\"По маске\" - файлы именуются автоматически по заранее введённой маске (след. пункт меню),\n"
        "\"Вручную\" - каждый раз имя файла нужно задавать вручную"
        ,
        "Sets the mode name when saving files to an external drive:\n"
        "\"By mask\" - files are named automatically advance to the entered mask(seq.Menu),\n"
        "\"Manual\" - each time the file name must be specified manually"
    },
    {
        {"По маске",    "Mask"},
        {"Вручную",     "Manually"}
    },
    (int8*)&FILE_NAMING_MODE
};

// ПАМЯТЬ - ВНЕШН ЗУ - Сохранять как ---------------------------------------------------------------------------------------------------------------
static const Choice mcDrive_ModeSave =
{
    Item_Choice, &mspDrive, 0,
    {
        "Сохранять как",    "Save as"
        ,
        "Если выбран вариант \"Изображение\", сигнал будет сохранён в текущем каталоге в графическом файле с расширением BMP\n"
        "Если выбран вариант \"Текст\", сигнал будет сохранён в текущем каталоге в текстовом виде в файле с раширением TXT"
        ,
        "If you select \"Image\", the signal will be stored in the current directory in graphic file with the extension BMP\n"
        "If you select \"Text\", the signal will be stored in the current directory as a text file with the extension TXT"
    },
    {
        {"Изображение",     "Image"},
        {"Текст",           "Text"}
    },
    (int8*)&MODE_SAVE
};

// ПАМЯТЬ - ВНЕШН ЗУ - Реж кн ПАМЯТЬ ---------------------------------------------------------------------------------------------------------------
static const Choice mcDrive_ModeBtnMemory =
{
    Item_Choice, &mspDrive, 0,
    {
        "Реж кн ПАМЯТЬ",    "Mode btn MEMORY",
        "",
        ""
    },
    {
        {"Меню",            "Menu"},
        {"Сохранение",      "Save"}
    },
    (int8*)&MODE_BTN_MEMORY
};

// ПАМЯТЬ - ВНЕШН ЗУ - Автоподключение -------------------------------------------------------------------------------------------------------------
static const Choice mcDrive_Autoconnect =
{
    Item_Choice, &mspDrive, 0,
    {
        "Автоподкл.",   "AutoConnect",
        "Eсли \"Вкл\", при подключении внешнего накопителя происходит автоматический переход на страницу ПАМЯТЬ - Внешн ЗУ",
        "If \"Enable\", when you connect an external drive is automatically transferred to the page MEMORY - Ext.Storage"
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&FLASH_AUTO_CONNECT
};

// ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspDrive_Manager =
{
    Item_Page, &mspDrive, FuncOfActiveExtMemFolder,
    {
        "КАТАЛОГ", "DIRECTORY",
        "Открывает доступ к файловой системе подключенного накопителя",
        "Provides access to the file system of the connected drive"
    },
    Page_SB_FileManager,
    {
        (void*)&sbExitFileManager,      // ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Выход
        (void*)&sbFileManagerTab,       // ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Tab
        (void*)0,
        (void*)0,
        (void*)&sbFileManagerLevelUp,   // ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Выйти из каталога
        (void*)&sbFileManagerLevelDown  // ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Войти в каталог
    },
    OnPressMemoryExtFileManager, 0, FM_RotateRegSet
};

static bool FuncOfActiveExtMemFolder(void)
{
    return gFlashDriveIsConnected;
}

void OnPressMemoryExtFileManager(void)
{
    if (gFlashDriveIsConnected)
    {
        FDrive_Mount();
        OpenPageAndSetItCurrent(Page_SB_FileManager);
        Display_SetDrawMode(DrawMode_Auto, FM_Draw);
        gBF.needRedrawFileManager = 1;
    }
    else
    {
        Display_ShowWarning(WarnNeedForFlashDrive);
    }
}

// ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Выход ------------------------------------------------------------------------------------------------------------
static const SmallButton sbExitFileManager =
{
    Item_SmallButton, &mspDrive_Manager, 0,
    {
        "Выход", "Exit",
        "Кнопка для выхода в предыдущее меню",
        "Button to return to the previous menu"
    },
    PressSB_FM_Exit,
    DrawSB_Exit
};

static void PressSB_FM_Exit(void)
{
    Display_SetDrawMode(DrawMode_Auto, 0);
    Display_RemoveAddDrawFunction();
}

// ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Tab --------------------------------------------------------------------------------------------------------------
static const SmallButton sbFileManagerTab =
{
    Item_SmallButton, &mspDrive_Manager, 0,
    {
        "Tab", "Tab",
        "Переход между каталогами и файлами",
        "The transition between the directories and files"
    },
    PressSB_FM_Tab,
    DrawSB_FM_Tab
};

static void DrawSB_FM_Tab(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_TAB);
    Painter_SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Выйти из каталога ------------------------------------------------------------------------------------------------
static const SmallButton sbFileManagerLevelUp =
{
    Item_SmallButton, &mspDrive_Manager, 0,
    {
        "Выйти из каталого", "Leave from directory",
        "Переход в родительский каталог",
        "Transition to the parental catalog"
    },
    PressSB_FM_LevelUp,
    DrawSB_FM_LevelUp
};

static void DrawSB_FM_LevelUp(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x48');
    Painter_SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Войти в каталог --------------------------------------------------------------------------------------------------
static const SmallButton sbFileManagerLevelDown =
{
    Item_SmallButton, &mspDrive_Manager, 0,
    {
        "Войти в каталог", "Enter in directory",
        "Переход в выбранный каталог",
        "Transition to the chosen catalog"
    },
    PressSB_FM_LevelDown,
    DrawSB_FM_LevelDown
};

static void DrawSB_FM_LevelDown(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 2, '\x4a');
    Painter_SetFont(TypeFont_8);
}

// Память - ВНЕШН ЗУ - МАСКА ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mspSetMask =
{
    Item_Page, &mspDrive, IsActiveMemoryExtSetMask,
    {
        "МАСКА", "MASK",
        "Режим ввода маски для автоматического именования файлов",
        "Input mode mask for automatic file naming"
    },
    Page_SB_MemExtSetMask,
    {
        (void*)&sbExitSetMask,      // ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Выход
        (void*)&sbSetMaskDelete,    // ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Удалить
        (void*)0,
        (void*)0,
        (void*)&sbSetMaskBackspace, // ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Backspace
        (void*)&sbSetMaskInsert     // ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Вставить
    },
    0, OnPressMemoryExtMask, OnMemExtSetMaskRegSet
};

static bool IsActiveMemoryExtSetMask(void)
{
    return FILE_NAMING_MODE_MASK;
}

static void OnPressMemoryExtMask(void)
{
    OpenPageAndSetItCurrent(Page_SB_MemExtSetMask);
    Display_SetAddDrawFunction(DrawSetMask);
}

static void DrawSetMask(void)
{
    int x0 = GridLeft() + 40;
    int y0 = GRID_TOP + 20;
    int width = GridWidth() - 80;
    int height = 160;

    Painter_DrawRectangleC(x0, y0, width, height, gColorFill);
    Painter_FillRegionC(x0 + 1, y0 + 1, width - 2, height - 2, gColorBack);

    int index = 0;
    int position = 0;
    int deltaX = 10;
    int deltaY0 = 5;
    int deltaY = 12;

    // Рисуем большие буквы английского алфавита
    while (symbolsAlphaBet[index][0] != ' ')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0);
        index++;
        position++;
    }

    // Теперь рисуем цифры и пробел
    position = 0;
    while (symbolsAlphaBet[index][0] != 'a')
    {
        DrawStr(index, x0 + deltaX + 50 + position * 7, y0 + deltaY0 + deltaY);
        index++;
        position++;
    }

    // Теперь рисуем малые буквы алфавита
    position = 0;
    while (symbolsAlphaBet[index][0] != '%')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0 + deltaY * 2);
        index++;
        position++;
    }

    // Теперь рисуем спецсимволы
    position = 0;
    while (index < (sizeof(symbolsAlphaBet) / 4))
    {
        DrawStr(index, x0 + deltaX + 26 + position * 20, y0 + deltaY0 + deltaY * 3);
        index++;
        position++;
    }

    DrawFileMask(x0 + deltaX, y0 + 65);

    static const char* strings[] =
    {
        "%y - год, %m - месяц, %d - день",
        "%H - часы, %M - минуты, %S - секунды",
        "%nN - порядковый номер, где",
        "n - минимальное количество знаков для N"
    };

    deltaY--;
    Painter_SetColor(gColorFill);
    for (int i = 0; i < sizeof(strings) / 4; i++)
    {
        Painter_DrawText(x0 + deltaX, y0 + 100 + deltaY * i, strings[i]);
    }
}

static void DrawFileMask(int x, int y)
{
    char *ch = FILE_NAME_MASK;

    Painter_SetColor(gColorFill);
    while (*ch != '\0')
    {
        if (*ch >= 32)
        {
            x = Painter_DrawChar(x, y, *ch);
        }
        else
        {
            if (*ch == 0x07)
            {
                x = Painter_DrawChar(x, y, '%');
                x = Painter_DrawChar(x, y, (char)(0x30 | *(ch + 1)));
                x = Painter_DrawChar(x, y, 'N');
                ch++;
            }
            else
            {
                x = Painter_DrawText(x, y, symbolsAlphaBet[*ch + 0x40]);
            }
        }
        ch++;
    }
    Painter_FillRegionC(x, y, 5, 8, COLOR_FLASH_10);
}

static void OnMemExtSetMaskRegSet(int angle)
{
    OnMemExtSetMaskNameRegSet(angle, sizeof(symbolsAlphaBet) / 4);
}

// ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Выход --------------------------------------------------------------------------------------------------------------
static const SmallButton sbExitSetMask =
{
    Item_SmallButton, &mspSetMask,
    COMMON_BEGIN_SB_EXIT,
    PressSB_SetMask_Exit,
    DrawSB_Exit
};

static void PressSB_SetMask_Exit(void)
{
    Display_RemoveAddDrawFunction();
}

// ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Удалить ------------------------------------------------------------------------------------------------------------
static const SmallButton sbSetMaskDelete =
{
    Item_SmallButton, &mspSetMask, 0,
    {
        "Удалить", "Delete",
        "Удаляет все введённые символы",
        "Deletes all entered symbols"
    },
    PressSB_SetMask_Delete,
    DrawSB_SetMask_Delete
};

static void PressSB_SetMask_Delete(void)
{
    FILE_NAME_MASK[0] = '\0';
}

static void DrawSB_SetMask_Delete(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Painter_SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Backspace ----------------------------------------------------------------------------------------------------------
static const SmallButton sbSetMaskBackspace =
{
    Item_SmallButton, &mspSetMask, 0,
    {
        "Backspace", "Backspace",
        "Удаляет последний введённый символ",
        "Deletes the last entered symbol"
    },
    PressSB_SetMask_Backspace,
    DrawSB_SetMask_Backspace
};

static void PressSB_SetMask_Backspace(void)
{
    int size = strlen(FILE_NAME_MASK);
    if (size > 0)
    {
        if (size > 1 && FILE_NAME_MASK[size - 2] == 0x07)
        {
            FILE_NAME_MASK[size - 2] = '\0';
        }
        else
        {
            FILE_NAME_MASK[size - 1] = '\0';
        }
    }
}

static void DrawSB_SetMask_Backspace(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x20');
    Painter_SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Вставить -----------------------------------------------------------------------------------------------------------
static const SmallButton sbSetMaskInsert =
{
    Item_SmallButton, &mspSetMask, 0,
    {
        "Вставить", "Insert",
        "Вставляет выбранный символ",
        "Inserts the chosen symbol"
    },
    PressSB_SetMask_Insert,
    DrawSB_SetMask_Insert
};

static void PressSB_SetMask_Insert(void)
{
    int index = INDEX_SYMBOL;
    int size = strlen(FILE_NAME_MASK);
    if (size == MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        return;
    }
    if (index < 0x41)
    {
        FILE_NAME_MASK[size] = symbolsAlphaBet[index][0];
        FILE_NAME_MASK[size + 1] = '\0';
    }
    else
    {
        index -= 0x40;
        if (index == 0x07)  // Для %nN - отдельный случай
        {
            if (size < MAX_SYMBOLS_IN_FILE_NAME - 2 && size > 0)
            {
                if (FILE_NAME_MASK[size - 1] >= 0x30 && FILE_NAME_MASK[size - 1] <= 0x39) // Если ранее введено число
                {
                    FILE_NAME_MASK[size] = FILE_NAME_MASK[size - 1] - 0x30;
                    FILE_NAME_MASK[size - 1] = 0x07;
                    FILE_NAME_MASK[size + 1] = '\0';
                }
            }
        }
        else
        {
            FILE_NAME_MASK[size] = (char)index;
            FILE_NAME_MASK[size + 1] = '\0';
        }
    }
}

static void DrawSB_SetMask_Insert(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_INSERT);
    Painter_SetFont(TypeFont_8);
}

// Страница вызывается для ввода имени файла /////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpSetName =
{
    Item_Page, 0, 0,
    {
        "", "",
        "",
        ""
    },
    Page_SB_MemExtSetName,
    {
        (void*)&sbExitSetName,
        (void*)&sbSetNameDelete,
        (void*)0,
        (void*)&sbSetNameBackspace,
        (void*)&sbSetNameInsert,
        (void*)&sbSetNameSave
    },
    0, 0, OnMemExtSetNameRegSet
};

static void OnMemExtSetNameRegSet(int angle)
{
    OnMemExtSetMaskNameRegSet(angle, sizeof(symbolsAlphaBet) / 4 - 7);
}

void OnMemExtSetMaskNameRegSet(int angle, int maxIndex)
{
    int8(*func[3])(int8 *, int8, int8) =
    {
        CircleDecreaseInt8,
        CircleDecreaseInt8,
        CircleIncreaseInt8
    };

    Painter_ResetFlash();
    if (INDEX_SYMBOL > maxIndex)
    {
        INDEX_SYMBOL = (int8)(maxIndex - 1);
    }
    func[Math_Sign(angle) + 1](&INDEX_SYMBOL, 0, (int8)(maxIndex - 1));
    Sound_RegulatorSwitchRotate();

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbExitSetName =   // Кнопк для выхода из режима задания имени сохраняемому сигналу. Одновременно кнопка отказа от сохранения
{
    Item_SmallButton, &mpSetName, 0,
    {
        "Выход", "Exit",
        "Отказ от сохранения",
        "Failure to save"
    },
    PressSB_SetName_Exit,
    DrawSB_Exit
};


static void PressSB_SetName_Exit(void)
{
    Display_RemoveAddDrawFunction();
    if (gMemory.exitFromModeSetNameTo == RETURN_TO_DISABLE_MENU)
    {
        ShortPressOnPageItem(PagePointerFromName(Page_SB_MemExtSetName), 0);
    }
    else if (gMemory.exitFromModeSetNameTo == RETURN_TO_LAST_MEM)
    {
        OpenPageAndSetItCurrent(Page_SB_MemLatest);
    }
    else if (gMemory.exitFromModeSetNameTo == RETURN_TO_INT_MEM)
    {
        OpenPageAndSetItCurrent(Page_SB_MemInt);
    }
    gMemory.exitFromModeSetNameTo = RETURN_TO_DISABLE_MENU;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbSetNameDelete =
{
    Item_SmallButton, &mpSetName, 0,
    {
        "Удалить", "Delete",
        "Удаляет все введённые символы",
        "Deletes all entered characters"
    },
    PressSB_SetName_Delete,
    DrawSB_SetName_Delete
};

static void PressSB_SetName_Delete(void)
{
    FILE_NAME[0] = '\0';
}

static void DrawSB_SetName_Delete(int x, int y) //-V524
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Painter_SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbSetNameBackspace =
{
    Item_SmallButton, &mpSetName, 0,
    {
        "Backspace", "Backspace",
        "Удаляет последний символ",
        "Delete the last character"
    },
    PressSB_SetName_Backspace,
    DrawSB_SetName_Backspace
};

static void PressSB_SetName_Backspace(void)
{
    int size = strlen(FILE_NAME);
    if (size > 0)
    {
        FILE_NAME[size - 1] = '\0';
    }
}

static void DrawSB_SetName_Backspace(int x, int y) //-V524
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x20');
    Painter_SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbSetNameInsert =
{
    Item_SmallButton, &mpSetName, 0,
    {
        "Вставить", "Insert",
        "Вводит очередной символ",
        "Print the next character"
    },
    PressSB_SetName_Insert,
    DrawSB_SetName_Insert
};

static void PressSB_SetName_Insert(void)
{
    int size = strlen(FILE_NAME);
    if (size < MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        FILE_NAME[size] = symbolsAlphaBet[INDEX_SYMBOL][0];
        FILE_NAME[size + 1] = '\0';
    }
}

static void DrawSB_SetName_Insert(int x, int y) //-V524
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 2, '\x26');
    Painter_SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbSetNameSave =
{
    Item_SmallButton, &mpSetName, 0,
    {
        "Сохранить", "Save",
        "Сохранение на флеш под заданным именем",
        "Saving to flashdrive with the specified name"
    },
    PressSB_MemExtSetNameSave,
    DrawSB_MemExtSetNameSave
};

static void PressSB_MemExtSetNameSave(void)
{
    if (gFlashDriveIsConnected)
    {
        PressSB_SetName_Exit();
        gMemory.needForSaveToFlashDrive = 1;
    }
}

static void DrawSB_MemExtSetNameSave(int x, int y)
{
    if (gFlashDriveIsConnected)
    {
        Painter_SetFont(TypeFont_UGO2);
        Painter_Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Painter_SetFont(TypeFont_8);
    }
}

/*

void DrawSB_MemLastSelect(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 3, y + 2, set.memory.strMemoryLast.isActiveModeSelect ? '\x2a' : '\x28');
    Painter_SetFont(TypeFont_8);
}

void PressSB_MemLastSelect(void)
{
    set.memory.strMemoryLast.isActiveModeSelect = !set.memory.strMemoryLast.isActiveModeSelect;
}


void DrawSB_MemExtNewFolder(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 1, y, '\x46');
    Painter_SetFont(TypeFont_8);
}

extern const Page pMemory;
extern const Page mspDrive;

*/
