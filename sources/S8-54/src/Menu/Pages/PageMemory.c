#include "PageMemory.h"
#include "Data/Data.h"
#include "Data/DataBuffer.h"
#include "Display/Grid.h"
#include "Display/Symbols.h"
#include "FlashDrive/FlashDrive.h"
#include "FPGA/FPGA.h"
#include "Hardware/FLASH.h"
#include "Hardware/Sound.h"
#include "Menu/FileManager.h"
#include "Menu/Menu.h"
#include "Menu/MenuFunctions.h"
#include "Menu/MenuItemsLogic.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Dictionary.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void PressSB_FM_Tab(void);

extern const      Page pMemory;
extern const    Choice cPoints;                                 ///< ПАМЯТЬ - Точки
static bool    IsActive_Points(void);
       void   OnChanged_Points(bool active);
extern const     Page ppLast;                                   ///< ПАМЯТЬ - Последние
static void     OnPress_Last(void);
static void      OnDraw_Last(void);
static void    OnRegSet_Last(int angle);
extern const   SButton bLast_Exit;                              ///< ПАМЯТЬ - Последние - Выход
static void     OnPress_Last_Exit(void);
extern const   SButton bLast_Next;                              ///< ПАМЯТЬ - Последние - Следующий
static void     OnPress_Last_Next(void);
static void        Draw_Last_Next(int x, int y);
extern const   SButton bLast_Prev;                              ///< ПАМЯТЬ - Последние - Предыдущий
static void     OnPress_Last_Prev(void);
static void        Draw_Last_Prev(int x, int y);
extern const   SButton bLast_SaveToROM;                         ///< ПАМЯТЬ - Последние - Внутр ЗУ
static void     OnPress_Last_SaveToROM(void);
static void        Draw_Last_SaveToROM(int x, int y);
extern const   SButton bLast_SaveToDrive;                       ///< ПАМЯТЬ - Последние - Сохранить
static void     OnPress_Last_SaveToDrive(void);
static void        Draw_Last_SaveToDrive(int x, int y);
extern const     Page ppInternal;                               ///< ПАМЯТЬ - ВНУТР ЗУ
static void     OnPress_Internal(void);
static void      OnDraw_Internal(void);
static void    OnRegSet_Internal(int delta);
extern const   SButton bInternal_Exit;                          ///< ПАМЯТЬ - ВНУТР ЗУ - Выход
static void     OnPress_Internal_Exit(void);
extern const   SButton bInternal_ShowAlways;                    ///< ПАМЯТЬ - ВНУТР ЗУ - Показывать всегда
static void     OnPress_Internal_ShowAlways(void);
static void        Draw_Internal_ShowAlways(int x, int y);
static void        Draw_Internal_ShowAlways_Yes(int x, int y);
static void        Draw_Internal_ShowAlways_No(int x, int y);
extern const   SButton bInternal_ModeShow;                      ///< ПАМЯТЬ - ВНУТР ЗУ - Вид сигнала
static void     OnPress_Internal_ModeShow(void);
static void        Draw_Internal_ModeShow(int x, int y);
static void        Draw_Internal_ModeShow_Direct(int x, int y); 
static void        Draw_Internal_ModeShow_Saved(int x, int y);
static void        Draw_Internal_ModeShow_Both(int x, int y);
extern const   SButton bInternal_Delete;                        ///< ПАМЯТЬ - ВНУТР ЗУ - Удалить
static void     OnPress_Internal_Delete(void);
static void        Draw_Internal_Delete(int x, int y);
/*
static const   SButton bInternal_EraseAll;                      ///< ПАМЯТЬ - ВНУТР ЗУ - Стереть все
static void     OnPress_Internal_EraseAll(void);
static void        Draw_Internal_EraseAll(int x, int y);
*/
extern const   SButton bInternal_Scale;                         ///< ПАМЯТЬ - ВНУТР ЗУ - Месштаб
static void     OnPress_Internal_Scale(void);
static void        Draw_Internal_Scale(int x, int y);
static void        Draw_Internal_Scale_Recalculated(int x, int y);
static void        Draw_Internal_Scale_Original(int x, int y);
extern const   SButton bInternal_SaveToMemory;                  ///< ПАМЯТЬ - ВНУТР ЗУ - Сохранить
static void     OnPress_Internal_SaveToMemory(void);
static void        Draw_Internal_SaveToMemory(int x, int y);
extern const   SButton bInternal_SaveToDrive;                   ///< ПАМЯТЬ - ВНУТР ЗУ - Сохранить на флешку
static void     OnPress_Internal_SaveToDrive(void);
static void        Draw_Internal_SaveToDrive(int x, int y);
extern const     Page ppDrive;                                  ///< ПАМЯТЬ - ВНЕШН ЗУ
extern const    Choice cDrive_Name;                             ///< ПАМЯТЬ - ВНЕШН ЗУ - Имя файла
extern const    Choice cDrive_SaveAs;                           ///< ПАМЯТЬ - ВНЕШН ЗУ - Сохранять как
extern const    Choice cDrive_ModeBtnMemory;                    ///< ПАМЯТЬ - ВНЕШН ЗУ - Режим кн ПАМЯТЬ
extern const    Choice cDrive_Autoconnect;                      ///< ПАМЯТЬ - ВНЕШН ЗУ - Автоподлючение
extern const    Page pppDrive_Manager;                          ///< ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ
static bool    IsActive_Drive_Manager(void);
       void     OnPress_Drive_Manager(void);
extern const   SButton bDrive_Manager_Exit;                     ///< ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Выход
static void     OnPress_Drive_Manager_Exit(void);
extern const   SButton bDrive_Manager_Tab;                      ///< ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Tab
static void        Draw_Drive_Manager_Tab(int x, int y);
extern const   SButton bDrive_Manager_LevelUp;                  ///< ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Выйти из каталога
static void        Draw_Drive_Manager_LevelUp(int x, int y);
extern const   SButton bDrive_Manager_LevelDown;                ///< ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Войти в каталог
static void        Draw_Drive_Manager_LevelDown(int x, int y);
extern const    Page pppDrive_Mask;                             ///< ПАМЯТЬ - ВНЕШН ЗУ - МАСКА
static bool    IsActive_Drive_Mask(void);
static void     OnPress_Drive_Mask(void);
static void    OnRegSet_Drive_Mask(int angle);
extern const   SButton bDrive_Mask_Exit;                        ///< ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Выход
extern const   SButton bDrive_Mask_Delete;                      ///< ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Удалить
static void     OnPress_Drive_Mask_Delete(void);
static void        Draw_Drive_Mask_Delete(int x, int y);
extern const   SButton bDrive_Mask_Backspace;                   ///< ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Backspace
static void     OnPress_Drive_Mask_Backspace(void);
static void        Draw_Drive_Mask_Backspace(int x, int y);
extern const   SButton bDrive_Mask_Insert;                      ///< ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Вставить
static void     OnPress_Drive_Mask_Insert(void);
static void        Draw_Drive_Mask_Insert(int x, int y);
extern const      Page pSetName;                                ///< УСТАНОВКА ИМЕНИ
static void    OnRegSet_SetName(int angle);
extern const   SButton bSetName_Exit;                           ///< УСТАНОВКА ИМЕНИ - Выход
static void     OnPress_SetName_Exit(void);
extern const   SButton bSetName_Delete;                         ///< УСТАНОВКА ИМЕНИ - Удалить
static void     OnPress_SetName_Delete(void);
static void        Draw_SetName_Delete(int x, int y);
extern const   SButton bSetName_Backspace;                      ///< УСТАНОВКА ИМЕНИ - Backspace
static void     OnPress_SetName_Backspace(void);
static void        Draw_SetName_Backspace(int x, int y);
extern const   SButton bSetName_Insert;                         ///< УСТАНОВКА ИМЕНИ - Вставить
static void     OnPress_SetName_Insert(void);
static void        Draw_SetName_Insert(int x, int y);
extern const   SButton bSetName_Save;                           ///< УСТАНОВКА ИМЕНИ - Сохранить
static void     OnPress_SetName_Save(void);
static void        Draw_SetName_Save(int x, int y);

static void DrawSetMask(void);  // Эта функция рисует, когда выбран режим задания маски.
static void DrawFileMask(int x, int y);
static void DrawSetName(void);  // Эта функция рисует, когда нужно задать имя файла для сохранения

static void DrawMemoryWave(int num, bool exist);
static void SaveSignalToIntMemory(void);
       void OnMemExtSetMaskNameRegSet(int angle, int maxIndex);

extern Page mainPage;

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
        (void *)&cPoints,       // ПАМЯТЬ - Точки
        (void *)&ppLast,        // ПАМЯТЬ - Последние
        (void *)&ppInternal,    // ПАМЯТЬ - ВНУТР ЗУ
        (void *)&ppDrive        // ПАМЯТЬ - ВНЕШН ЗУ
    }
};

// ПАМЯТЬ - Точки ------------------------------------------------------------------------------------------------------------------------------------
static const Choice cPoints =
{
    Item_Choice, &pMemory, IsActive_Points,
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
    (int8 *)&FPGA_ENUM_POINTS, OnChanged_Points
};

static bool IsActive_Points(void)
{
    return SET_PEAKDET_DIS;
}

void OnChanged_Points(bool active)
{
    // Если включен пиковый детектор, то не можем переключать память
    if (SET_PEAKDET_EN && !active)
    {
        display.ShowWarning(WrongModePeackDet);
        return;
    }

    // Блокируем включение 32к длины записи, если включен второй канал
    if (FPGA_POINTS_32k && SET_ENABLED_B)
    {
        display.ShowWarning(DisableChannel2);
        FPGA_ENUM_POINTS = FNP_16k;
    }

    int width = grid.Width();
    
    FPGA_Reset();
    
    if (SET_PEAKDET_EN)
    {
        width *= 2;
    }
    if (TPOS_IS_LEFT)
    {
        SHIFT_IN_MEMORY = 0;
    }
    else if (TPOS_IS_CENTER)
    {
        SHIFT_IN_MEMORY = (int16)(SET_BYTES_IN_CHANNEL / 2 - width / 2);
    }
    else if (TPOS_IS_RIGHT)
    {
        SHIFT_IN_MEMORY = (int16)(SET_BYTES_IN_CHANNEL - width - 2);
    }
    
    FPGA_Reset();
    FPGA_SetTShift(SET_TSHIFT);
    FPGA_Reset();
}

// ПАМЯТЬ - ПОСЛЕДНИЕ --------------------------------------------------------------------------------------------------------------------------------
static const Page ppLast =
{
    Item_Page, &pMemory, 0,
    {
        "ПОСЛЕДНИЕ", "LATEST",
        "Переход в режим работы с последними полученными сигналами",
        "Transition to an operating mode with the last received signals"
    },
    PageSB_Memory_Last,
    {
        (void *)&bLast_Exit,        // ПАМЯТЬ - ПОСЛЕДНИЕ - Выход
        (void *)0,
        (void *)&bLast_Next,        // ПАМЯТЬ - ПОСЛЕДНИЕ - Следующий
        (void *)&bLast_Prev,        // ПАМЯТЬ - ПОСЛЕДНИЕ - Предыдущий
        (void *)&bLast_SaveToROM,   // ПАМЯТЬ - ПОСЛЕДНИЕ - Внутр ЗУ
        (void *)&bLast_SaveToDrive  // ПАМЯТЬ - ПОСЛЕДНИЕ - Сохранить
    },
    true, OnPress_Last, OnDraw_Last, OnRegSet_Last
};

static void OnPress_Last(void)
{
    NUM_RAM_SIGNAL = 0;
    RUN_FPGA_BEFORE_SB = FPGA_IS_RUNNING ? 1 : 0;
    FPGA_Stop(false);
    MODE_WORK = ModeWork_RAM;
}

static void OnDraw_Last(void)
{
    char buffer[20];

    int width = 40;
    int height = 10;
    painter.FillRegion(grid.Right() - width, GRID_TOP, width, height, gColorBack);
    painter.DrawRectangle(grid.Right() - width, GRID_TOP, width, height, gColorFill);
    painter.DrawText(grid.Right() - width + 2, GRID_TOP + 1, Int2String(NUM_RAM_SIGNAL + 1, false, 3, buffer));
    painter.DrawText(grid.Right() - width + 17, GRID_TOP + 1, "/");
    painter.DrawText(grid.Right() - width + 23, GRID_TOP + 1, Int2String(DS_NumElementsInStorage(), false, 3, buffer));
}

static void OnRegSet_Last(int angle)
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
static const SButton bLast_Exit =
{
    Item_SmallButton, &ppLast,
    COMMON_BEGIN_SB_EXIT,
    OnPress_Last_Exit,
    DrawSB_Exit
};

static void OnPress_Last_Exit(void)
{
    MODE_WORK = ModeWork_Dir;
    if (RUN_FPGA_BEFORE_SB)
    {
        FPGA_Start();
        RUN_FPGA_BEFORE_SB = 0;
    }
    OnPressSB_Exit();
}

// ПАМЯТЬ - ПОСЛЕДНИЕ - Следующий --------------------------------------------------------------------------------------------------------------------
static const SButton bLast_Next =
{
    Item_SmallButton, &ppLast, 0,
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
    CircleIncreaseInt16(&NUM_RAM_SIGNAL, 0, (int16)(DS_NumElementsInStorage() - 1));
}

static void Draw_Last_Next(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 2, '\x64');
    painter.SetFont(TypeFont_8);
}

// ПАМЯТЬ - ПОСЛЕДНИЕ - Предыдущий -------------------------------------------------------------------------------------------------------------------
static const SButton bLast_Prev =
{
    Item_SmallButton, &ppLast, 0,
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
    CircleDecreaseInt16(&NUM_RAM_SIGNAL, 0, (int16)(DS_NumElementsInStorage() - 1));
}

static void Draw_Last_Prev(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_BACKSPACE);
    painter.SetFont(TypeFont_8);
}

// ПАМЯТЬ - ПОСЛЕДНИЕ - Внутр ЗУ ---------------------------------------------------------------------------------------------------------------------
static const SButton bLast_SaveToROM =
{
    Item_SmallButton, &ppLast, 0,
    {
        "Внутр ЗУ", "Internal storage",
        "Нажмите эту кнопку, чтобы сохранить сигнал во внутреннем запоминающем устройстве",
        "Press this button to keep a signal in an internal memory"
    },
    OnPress_Last_SaveToROM,
    Draw_Last_SaveToROM
};

static void OnPress_Last_SaveToROM(void)
{
    SBPage_SetCurrent(&ppInternal);
    MODE_WORK = ModeWork_ROM;   // Находимся в режиме внутренного ЗУ
    EXIT_FROM_ROM_TO_RAM = 1;   // Этот признак мы устанавливаем для того, чтобы:
                                // 1 - по нажатии кнопки Выход со страницы "ВНУТР ЗУ" выходить в "Последние", а не в основное меню;
                                // 2 - для того, чтобы на странице "ВНУТР ЗУ" выводить не последний считанный сигнал, а выбранный на странице 
                                // "Последние";
                                // 3 - чтобы в Data_Load() устанавливать соответствующие указатели.
}

static void Draw_Last_SaveToROM(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_ROM);
    painter.SetFont(TypeFont_8);
}

// ПАМЯТЬ - ПОСЛЕДНИЕ - Сохранить --------------------------------------------------------------------------------------------------------------------
static const SButton bLast_SaveToDrive =
{
    Item_SmallButton, &ppLast, 0,
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
    EXIT_FROM_SETNAME_TO = RETURN_TO_LAST_MEM;
    Memory_SaveSignalToFlashDrive();
}

void Memory_SaveSignalToFlashDrive(void)
{
    if (FDRIVE_IS_CONNECTED)
    {
        if (FILE_NAMING_MODE_MANUAL)
        {
            display.SetAddDrawFunction(DrawSetName);
        }
        else
        {
            NEED_SAVE_TO_FLASHDRIVE = 1;
        }
    }
    else
    {
        EXIT_FROM_SETNAME_TO = 0;
    }
}

static void DrawSetName(void)
{
    int x0 = grid.Left() + 40;
    int y0 = GRID_TOP + 60;
    int width = grid.Width() - 80;
    int height = 80;

    painter.DrawRectangle(x0, y0, width, height, gColorFill);
    painter.FillRegion(x0 + 1, y0 + 1, width - 2, height - 2, gColorBack);

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

    int x = painter.DrawTextC(x0 + deltaX, y0 + 65, FILE_NAME, gColorFill);
    painter.FillRegion(x, y0 + 65, 5, 8, COLOR_FLASH_10);
}

static void Draw_Last_SaveToDrive(int x, int y)
{
    if (FDRIVE_IS_CONNECTED)
    {
        painter.SetFont(TypeFont_UGO2);
        painter.Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_FLASH_DRIVE_BIG);
        painter.SetFont(TypeFont_8);
    }
}

// ПАМЯТЬ - ВНУТР ЗУ ---------------------------------------------------------------------------------------------------------------------------------
static const Page ppInternal =
{
    Item_Page, &pMemory, 0,
    {
        "ВНУТР ЗУ", "INT STORAGE",
        "Переход в режим работы с внутренней памятью",
        "Transition to an operating mode with internal memory"
    },
    PageSB_Memory_Internal,
    {
        (void *)&bInternal_Exit,            // ПАМЯТЬ - ВНУТР ЗУ - Выход
        (void *)&bInternal_ShowAlways,      // ПАМЯТЬ - ВНУТР ЗУ - Показывать всегда
        (void *)&bInternal_ModeShow,        // ПАМЯТЬ - ВНУТР ЗУ - Вид сигнала
        //(void *)0,
        //(void *)&bInternal_EraseAll,
        //(void *)&bInternal_Scale,           // ПАМЯТЬ - ВНУТР ЗУ - Масштаб
        (void *)&bInternal_Delete,          // ПАМЯТЬ - ВНУТР ЗУ - Удалить
        (void *)&bInternal_SaveToMemory,    // ПАМЯТЬ - ВНУТР ЗУ - Сохранить
        (void *)&bInternal_SaveToDrive      // ПАМЯТЬ - ВНУТР ЗУ - Сохранить на флешку
    },
    true, OnPress_Internal, OnDraw_Internal, OnRegSet_Internal
};

static void OnPress_Internal(void)
{
    MODE_WORK = ModeWork_ROM;
}

static void OnDraw_Internal(void)
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

    int x = grid.Left() + 2 + num * 12;
    int y = grid.FullBottom() - 10;
    int width = 12;
    painter.FillRegion(x, y, width, 10, num == NUM_ROM_SIGNAL ? COLOR_FLASH_10 : gColorBack);
    painter.DrawRectangle(x, y, width, 10, gColorFill);
    painter.SetColor(num == NUM_ROM_SIGNAL ? COLOR_FLASH_01 : gColorFill);
    if (exist)
    {
        painter.DrawText(x + 2, y + 1, Int2String(num + 1, false, 2, buffer));
    }
    else
    {
        painter.DrawText(x + 3, y + 1, "\x88");
    }
}

static void OnRegSet_Internal(int delta)
{
    Sound_RegulatorSwitchRotate();
    if (delta < 0)
    {
        CircleDecreaseInt8(&NUM_ROM_SIGNAL, 0, MAX_NUM_SAVED_WAVES - 1);
    }
    else if (delta > 0)
    {
        CircleIncreaseInt8(&NUM_ROM_SIGNAL, 0, MAX_NUM_SAVED_WAVES - 1);
    }
    painter.ResetFlash();
}

// ПАМЯТЬ - ВНУТР ЗУ - Выход -------------------------------------------------------------------------------------------------------------------------
static const SButton bInternal_Exit =
{
    Item_SmallButton, &ppInternal, 0,
    {
        "Выход", "Exit",
        "Кнопка для выхода в предыдущее меню",
        "Button to return to the previous menu"
    },
    OnPress_Internal_Exit,
    DrawSB_Exit
};

static void FuncForInternalExit(void)
{
    SBPage_SetCurrent(&ppLast);
}

static void OnPress_Internal_Exit(void)
{
    if (EXIT_FROM_ROM_TO_RAM)
    {
        MODE_WORK = ModeWork_RAM;
        EXIT_FROM_ROM_TO_RAM = 0;
        Menu_RunAfterUpdate(FuncForInternalExit);
    }
    else
    {
        MODE_WORK = ModeWork_Dir;
        if (RUN_FPGA_BEFORE_SB)
        {
            FPGA_Start();
            RUN_FPGA_BEFORE_SB = 0;
        }
        OnPressSB_Exit();
        //ShortPressOnPageItem(PagePointerFromName(PageSB_Memory_Internal), 0);
    }
}

// ПАМЯТЬ - ВНУТР ЗУ - Показывать всегда -------------------------------------------------------------------------------------------------------------
static const SButton bInternal_ShowAlways =
{
    Item_SmallButton, &ppInternal, 0,
    {
        "Показывать всегда", "To show always",
        "Позволяет всегда показывать выбранный сохранённый сигнал поверх текущего",
        "Allows to show always the chosen kept signal over the current"
    },
    OnPress_Internal_ShowAlways,
    Draw_Internal_ShowAlways,
    {
        {
            Draw_Internal_ShowAlways_Yes,
            "показывать выбранный сигнал из внутренней памяти поверх текущего",
            "to show the chosen signal from internal memory over the current"
        },
        {
            Draw_Internal_ShowAlways_No,
            "сигнал из внутренней памяти виден только в режиме работы с внутренним запоминающим устройством",
            "the signal from internal memory is visible only in an operating mode with an internal memory"
        }
    }
};

static void OnPress_Internal_ShowAlways(void)
{
    ALWAYS_SHOW_ROM_SIGNAL = ALWAYS_SHOW_ROM_SIGNAL ? 0 : 1;
}

static void Draw_Internal_ShowAlways(int x, int y)
{
    if (ALWAYS_SHOW_ROM_SIGNAL == 0)
    {
        Draw_Internal_ShowAlways_No(x, y);
    }
    else
    {
        Draw_Internal_ShowAlways_Yes(x, y);
    }
}

static void Draw_Internal_ShowAlways_Yes(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, '\x66');
    painter.SetFont(TypeFont_8);
}

static void Draw_Internal_ShowAlways_No(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, '\x68');
    painter.SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНУТР ЗУ - Вид сигнала -------------------------------------------------------------------------------------------------------------------
static const SButton bInternal_ModeShow =
{
    Item_SmallButton, &ppInternal, 0,
    {
        "Вид сигнала", "Type of a signal",
        "Показывать записанный или текущий сигнал в режиме ВНУТР ЗУ",
        "Show recorded or current signal in mode Internal Memory"
    },
    OnPress_Internal_ModeShow,
    Draw_Internal_ModeShow,
    {
        {
            Draw_Internal_ModeShow_Direct,
            "на дисплее текущий сигнал",
            "on the display current signal"
        },
        {
            Draw_Internal_ModeShow_Saved,
            "на дисплее сохранённый сигнал",
            "on the display the kept signal"
        },
        {
            Draw_Internal_ModeShow_Both,
            "на дисплее оба сигнала",
            "on the display both signals"
        }
    }
};

static void OnPress_Internal_ModeShow(void)
{
    CircleIncreaseInt8((int8 *)&SHOW_IN_INT, 0, 2);
}

static void Draw_Internal_ModeShow(int x, int y)
{
    if (SHOW_IN_INT_DIRECT)
    {
        Draw_Internal_ModeShow_Direct(x, y);
    }
    else if (SHOW_IN_INT_SAVED)
    {
        Draw_Internal_ModeShow_Saved(x, y);
    }
    else
    {
        Draw_Internal_ModeShow_Both(x, y);
    }
}

static void Draw_Internal_ModeShow_Direct(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, '\x6a');
    painter.SetFont(TypeFont_8);
}


static void Draw_Internal_ModeShow_Saved(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, '\x6c');
    painter.SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Draw_Internal_ModeShow_Both(int x, int y)
{
    painter.DrawText(x + 1, y + 5, "ОБА");
    painter.DrawVLine(x + 1, y + 2, y + 14, gColorBack);
    painter.DrawVLine(x + 2, y + 6, y + 11, gColorFill);
}

/*
// ПАМЯТЬ - ВНУТР ЗУ - Стереть всё -------------------------------------------------------------------------------------------------------------------
static const SButton bInternal_EraseAll =
{
    Item_SmallButton, &ppInternal, 0,
    {
        "Стереть всё", "Erase all",
        "Стирает все данные из области хранения данных, включая область иноформации. Тотальное форматирование",
        "It erases all data from the storage area, including the area inoformatsiya. The total format"
    },
    OnPress_Internal_EraseAll,
    Draw_Internal_EraseAll,
    {
        {
            Draw_Internal_EraseAll,
            "Стереть все данные",
            "Erase all data"
        }
    }
};

static void OnPress_Internal_EraseAll(void)
{
    display.FuncOnWaitStart("Стираю. Подождите", "Erase. Wait", false);
    FLASH_DeleteAllData();
    display.FuncOnWaitStop();
}

static void Draw_Internal_EraseAll(int x, int y)
{
    painter.DrawText(x + 5, y + 5, "E");
}
*/

// ПАМЯТЬ - ВНУТР ЗУ - Масштаб -----------------------------------------------------------------------------------------------------------------------
static const SButton bInternal_Scale =
{
    Item_SmallButton, &ppInternal, 0,
    {
        "Масштаб", "Scale",
        "Приводить или нет записанный сигнал к текущим установкам",
        "Whether or not to record the recorded signal to the current settings"
    },
    OnPress_Internal_Scale,
    Draw_Internal_Scale,
    {
        {
            Draw_Internal_Scale_Recalculated,
            "Сигнал приведён к текущим установкам осциллографа",
            "The signal is given to the current oscilloscope settings"
        },
        {
            Draw_Internal_Scale_Original,
            "Сигнал рисуется в таком виде, в котором считан",
            "The signal is drawn in the form in which the readout"
        }
    }
};

void OnPress_Internal_Scale(void)
{
    CircleIncreaseInt8((int8 *)&MEM_DATA_SCALE, 0, 1);
}

void Draw_Internal_Scale(int x, int y)
{
    if (MEM_DATA_SCALE_RECALC)
    {
        Draw_Internal_Scale_Recalculated(x, y);
    }
    else
    {
        Draw_Internal_Scale_Original(x, y);
    }
}

void Draw_Internal_Scale_Recalculated(int x, int y)
{
    painter.DrawText(x + 8, y + 2, DICT(DM));
    painter.SetFont(TypeFont_5);
    painter.DrawText(x + 5, y + 9, DICT(DABS));
    painter.SetFont(TypeFont_8);
}

void Draw_Internal_Scale_Original(int x, int y)
{
    painter.DrawText(x + 8, y + 2, DICT(DM));
    painter.SetFont(TypeFont_5);
    painter.DrawText(x + 5, y + 9, DICT(DREL));
    painter.SetFont(TypeFont_8);
}


// ПАМЯТЬ - ВНУТР ЗУ - Сохранить в памяти ------------------------------------------------------------------------------------------------------------
static const SButton bInternal_SaveToMemory =
{
    Item_SmallButton, &ppInternal, 0,
    {
        "Сохранить в памяти", "Save to memory",
        "Сохранить сигнал во внутреннем запоминующем устройстве",
        "To keep a signal in an internal memory"
    },
    OnPress_Internal_SaveToMemory,
    Draw_Internal_SaveToMemory
};

static void OnPress_Internal_SaveToMemory(void)
{
    display.FuncOnWaitStart(DICT(DStoredInMemory), false);
    SaveSignalToIntMemory();
    display.FuncOnWaitStop();
}

static void SaveSignalToIntMemory(void)
{
    // Заносим в указатели DS, DATA_A, DATA_B данные из ОЗУ или последний считанный сигнал, в зависимости от того, из какого режима зашли в 
    // "ПАМЯТЬ-ВНУТР ЗУ"
    Data_ReadFromRAM(EXIT_FROM_ROM_TO_RAM ? NUM_RAM_SIGNAL : 0, 0, false);

    if (DS)                                             // Если есть что сохранять
    {
        FLASH_SaveData(NUM_ROM_SIGNAL, DS, IN_A, IN_B);   // То сохраняем данные из DS, DATA_A, DATA_B на место NUM_ROM_SIGNAL в ППЗУ
        display.ShowWarning(SignalIsSaved);
    }
}

static void Draw_Internal_SaveToMemory(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    painter.SetFont(TypeFont_8);
}


// ПАМЯТЬ - ВНУТР ЗУ - Удалить ----------------------------------------------------------------------------------------------------------------------
static const SButton bInternal_Delete =
{
    Item_SmallButton, &ppInternal, 0,
    {
        "Удалить", "Delete",
        "Удаляет выбранный сигнал из внутреннего запоминающего устройства",
        "Removes the selected signal from the internal storage device"
    },
    OnPress_Internal_Delete,
    Draw_Internal_Delete
};

static void OnPress_Internal_Delete(void)
{
    display.FuncOnWaitStart(DICT(DDeleteFromMemory), false);
    FLASH_DeleteData(NUM_ROM_SIGNAL);
    display.FuncOnWaitStop();
}

static void Draw_Internal_Delete(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    painter.SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНУТР ЗУ - Сохранить на флешку -----------------------------------------------------------------------------------------------------------
static const SButton bInternal_SaveToDrive =
{
    Item_SmallButton, &ppInternal, 0,
    {
        "Сохранить на флешку", "Save to disk",
        "Сохраняет сигнал на флешку",
        "Save signal to flash drive"
    },
    OnPress_Internal_SaveToDrive,
    Draw_Internal_SaveToDrive
};

static void OnPress_Internal_SaveToDrive(void)
{
    EXIT_FROM_SETNAME_TO = RETURN_TO_INT_MEM;
    Memory_SaveSignalToFlashDrive();
}

static void Draw_Internal_SaveToDrive(int x, int y)
{
    if (FDRIVE_IS_CONNECTED)
    {
        painter.SetFont(TypeFont_UGO2);
        painter.Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_FLASH_DRIVE_BIG);
        painter.SetFont(TypeFont_8);
    }
}

// ПАМЯТЬ - ВНЕШН ЗУ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppDrive =
{
    Item_Page, &pMemory, 0,
    {
        "ВНЕШН ЗУ", "EXT STORAGE",
        "Работа с внешним запоминающим устройством.",
        "Work with external storage device."
    },
    Page_Memory_Drive,
    {
        (void *)&pppDrive_Manager,      // ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ
        (void *)&cDrive_Name,           // ПАМЯТЬ - ВНЕШН ЗУ - Имя файла
        (void *)&pppDrive_Mask,         // Память - ВНЕШН ЗУ - МАСКА
        (void *)&cDrive_SaveAs,         // ПАМЯТЬ - ВНЕШН ЗУ - Сохранять как
        (void *)&cDrive_ModeBtnMemory,  // ПАМЯТЬ - ВНЕШН ЗУ - Реж кн ПАМЯТЬ
        (void *)&cDrive_Autoconnect     // ПАМЯТЬ - ВНЕШН ЗУ - Автоподключение
    }
};

// ПАМЯТЬ - ВНЕШН ЗУ - Имя файла ---------------------------------------------------------------------------------------------------------------------
static const Choice cDrive_Name =
{
    Item_Choice, &ppDrive, 0,
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
    (int8 *)&FILE_NAMING_MODE
};

// ПАМЯТЬ - ВНЕШН ЗУ - Сохранять как ---------------------------------------------------------------------------------------------------------------
static const Choice cDrive_SaveAs =
{
    Item_Choice, &ppDrive, 0,
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
    (int8 *)&MODE_SAVE
};

// ПАМЯТЬ - ВНЕШН ЗУ - Реж кн ПАМЯТЬ ---------------------------------------------------------------------------------------------------------------
static const Choice cDrive_ModeBtnMemory =
{
    Item_Choice, &ppDrive, 0,
    {
        "Реж кн ПАМЯТЬ",    "Mode btn MEMORY",
        "",
        ""
    },
    {
        {"Меню",            "Menu"},
        {"Сохранение",      "Save"}
    },
    (int8 *)&MODE_BTN_MEMORY
};

// ПАМЯТЬ - ВНЕШН ЗУ - Автоподключение -------------------------------------------------------------------------------------------------------------
static const Choice cDrive_Autoconnect =
{
    Item_Choice, &ppDrive, 0,
    {
        "Автоподкл.",   "AutoConnect",
        "Eсли \"Вкл\", при подключении внешнего накопителя происходит автоматический переход на страницу ПАМЯТЬ - Внешн ЗУ",
        "If \"Enable\", when you connect an external drive is automatically transferred to the page MEMORY - Ext.Storage"
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&FLASH_AUTO_CONNECT
};

// ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page pppDrive_Manager =
{
    Item_Page, &ppDrive, IsActive_Drive_Manager,
    {
        "КАТАЛОГ", "DIRECTORY",
        "Открывает доступ к файловой системе подключенного накопителя",
        "Provides access to the file system of the connected drive"
    },
    PageSB_Memory_Drive_Manager,
    {
        (void *)&bDrive_Manager_Exit,       // ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Выход
        (void *)&bDrive_Manager_Tab,        // ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Tab
        (void *)0,
        (void *)0,
        (void *)&bDrive_Manager_LevelUp,    // ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Выйти из каталога
        (void *)&bDrive_Manager_LevelDown   // ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Войти в каталог
    },
    true, OnPress_Drive_Manager, 0, FM_RotateRegSet
};

static bool IsActive_Drive_Manager(void)
{
    return FDRIVE_IS_CONNECTED;
}

void OnPress_Drive_Manager(void)
{
    if (FDRIVE_IS_CONNECTED)
    {
        FDrive_Mount();
        display.SetDrawMode(DrawMode_Auto, FM_Draw);
        FM_NEED_REDRAW = FM_REDRAW_FULL;
    }
    else
    {
        display.ShowWarning(WarnNeedForFlashDrive);
    }
}

// ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Выход ------------------------------------------------------------------------------------------------------------
static const SButton bDrive_Manager_Exit =
{
    Item_SmallButton, &pppDrive_Manager, 0,
    {
        "Выход", "Exit",
        "Кнопка для выхода в предыдущее меню",
        "Button to return to the previous menu"
    },
    OnPress_Drive_Manager_Exit,
    DrawSB_Exit
};

static void OnPress_Drive_Manager_Exit(void)
{
    display.SetDrawMode(DrawMode_Auto, 0);
    OnPressSB_Exit();
}

// ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Tab --------------------------------------------------------------------------------------------------------------
static const SButton bDrive_Manager_Tab =
{
    Item_SmallButton, &pppDrive_Manager, 0,
    {
        "Tab", "Tab",
        "Переход между каталогами и файлами",
        "The transition between the directories and files"
    },
    PressSB_FM_Tab,
    Draw_Drive_Manager_Tab
};

static void Draw_Drive_Manager_Tab(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_TAB);
    painter.SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Выйти из каталога ------------------------------------------------------------------------------------------------
static const SButton bDrive_Manager_LevelUp =
{
    Item_SmallButton, &pppDrive_Manager, 0,
    {
        "Выйти из каталого", "Leave from directory",
        "Переход в родительский каталог",
        "Transition to the parental catalog"
    },
    PressSB_FM_LevelUp,
    Draw_Drive_Manager_LevelUp
};

static void Draw_Drive_Manager_LevelUp(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, '\x48');
    painter.SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ - Войти в каталог --------------------------------------------------------------------------------------------------
static const SButton bDrive_Manager_LevelDown =
{
    Item_SmallButton, &pppDrive_Manager, 0,
    {
        "Войти в каталог", "Enter in directory",
        "Переход в выбранный каталог",
        "Transition to the chosen catalog"
    },
    PressSB_FM_LevelDown,
    Draw_Drive_Manager_LevelDown
};

static void Draw_Drive_Manager_LevelDown(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 2, '\x4a');
    painter.SetFont(TypeFont_8);
}

// Память - ВНЕШН ЗУ - МАСКА ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page pppDrive_Mask =
{
    Item_Page, &ppDrive, IsActive_Drive_Mask,
    {
        "МАСКА", "MASK",
        "Режим ввода маски для автоматического именования файлов",
        "Input mode mask for automatic file naming"
    },
    PageSB_Memory_Drive_Mask,
    {
        (void *)&bDrive_Mask_Exit,      // ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Выход
        (void *)&bDrive_Mask_Delete,    // ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Удалить
        (void *)0,
        (void *)0,
        (void *)&bDrive_Mask_Backspace, // ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Backspace
        (void *)&bDrive_Mask_Insert     // ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Вставить
    },
    true, OnPress_Drive_Mask, 0, OnRegSet_Drive_Mask
};

static bool IsActive_Drive_Mask(void)
{
    return FILE_NAMING_MODE_MASK;
}

static void OnPress_Drive_Mask(void)
{
    display.SetAddDrawFunction(DrawSetMask);
}

static void DrawSetMask(void)
{
    int x0 = grid.Left() + 40;
    int y0 = GRID_TOP + 20;
    int width = grid.Width() - 80;
    int height = 160;

    painter.DrawRectangle(x0, y0, width, height, gColorFill);
    painter.FillRegion(x0 + 1, y0 + 1, width - 2, height - 2, gColorBack);

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

    static const char * const strings[] =
    {
        "%y - год, %m - месяц, %d - день",
        "%H - часы, %M - минуты, %S - секунды",
        "%nN - порядковый номер, где",
        "n - минимальное количество знаков для N"
    };

    deltaY--;
    painter.SetColor(gColorFill);
    for (int i = 0; i < sizeof(strings) / 4; i++)
    {
        painter.DrawText(x0 + deltaX, y0 + 100 + deltaY * i, strings[i]);
    }
}

static void DrawFileMask(int x, int y)
{
    char *ch = FILE_NAME_MASK;

    painter.SetColor(gColorFill);
    while (*ch != '\0')
    {
        if (*ch >= 32)
        {
            x = painter.DrawChar(x, y, *ch);
        }
        else
        {
            if (*ch == 0x07)
            {
                x = painter.DrawChar(x, y, '%');
                x = painter.DrawChar(x, y, (char)(0x30 | *(ch + 1)));
                x = painter.DrawChar(x, y, 'N');
                ch++;
            }
            else
            {
                x = painter.DrawText(x, y, symbolsAlphaBet[*ch + 0x40]);
            }
        }
        ch++;
    }
    painter.FillRegion(x, y, 5, 8, COLOR_FLASH_10);
}

static void OnRegSet_Drive_Mask(int angle)
{
    OnMemExtSetMaskNameRegSet(angle, sizeof(symbolsAlphaBet) / 4);
}

// ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Выход --------------------------------------------------------------------------------------------------------------
static const SButton bDrive_Mask_Exit =
{
    Item_SmallButton, &pppDrive_Mask,
    COMMON_BEGIN_SB_EXIT,
    OnPressSB_Exit,
    DrawSB_Exit
};

// ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Удалить ------------------------------------------------------------------------------------------------------------
static const SButton bDrive_Mask_Delete =
{
    Item_SmallButton, &pppDrive_Mask, 0,
    {
        "Удалить", "Delete",
        "Удаляет все введённые символы",
        "Deletes all entered symbols"
    },
    OnPress_Drive_Mask_Delete,
    Draw_Drive_Mask_Delete
};

static void OnPress_Drive_Mask_Delete(void)
{
    FILE_NAME_MASK[0] = '\0';
}

static void Draw_Drive_Mask_Delete(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    painter.SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Backspace ----------------------------------------------------------------------------------------------------------
static const SButton bDrive_Mask_Backspace =
{
    Item_SmallButton, &pppDrive_Mask, 0,
    {
        "Backspace", "Backspace",
        "Удаляет последний введённый символ",
        "Deletes the last entered symbol"
    },
    OnPress_Drive_Mask_Backspace,
    Draw_Drive_Mask_Backspace
};

static void OnPress_Drive_Mask_Backspace(void)
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

static void Draw_Drive_Mask_Backspace(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_BACKSPACE);
    painter.SetFont(TypeFont_8);
}

// ПАМЯТЬ - ВНЕШН ЗУ - МАСКА - Вставить -----------------------------------------------------------------------------------------------------------
static const SButton bDrive_Mask_Insert =
{
    Item_SmallButton, &pppDrive_Mask, 0,
    {
        "Вставить", "Insert",
        "Вставляет выбранный символ",
        "Inserts the chosen symbol"
    },
    OnPress_Drive_Mask_Insert,
    Draw_Drive_Mask_Insert
};

static void OnPress_Drive_Mask_Insert(void)
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

static void Draw_Drive_Mask_Insert(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_INSERT);
    painter.SetFont(TypeFont_8);
}

// Страница вызывается для ввода имени файла /////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page pSetName =
{
    Item_Page, 0, 0,
    {
        "", "",
        "",
        ""
    },
    PageSB_Memory_SetName,
    {
        (void *)&bSetName_Exit,         // ВВОД ИМЕНИ ФАЙЛА - Выход
        (void *)&bSetName_Delete,       // ВВОД ИМЕНИ ФАЙЛА - Удалить
        (void *)0,
        (void *)&bSetName_Backspace,    // ВВОД ИМЕНИ ФАЙЛА - Backspace
        (void *)&bSetName_Insert,       // ВВОД ИМЕНИ ФАЙЛА - Вставить
        (void *)&bSetName_Save          // ВВОД ИМЕНИ ФАЙЛА - Сохранить
    },
    true, 0, 0, OnRegSet_SetName
};

static void OnRegSet_SetName(int angle)
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

    painter.ResetFlash();
    if (INDEX_SYMBOL > maxIndex)
    {
        INDEX_SYMBOL = (int8)(maxIndex - 1);
    }
    func[Math_Sign(angle) + 1](&INDEX_SYMBOL, 0, (int8)(maxIndex - 1));
    Sound_RegulatorSwitchRotate();

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const SButton bSetName_Exit =   // Кнопк для выхода из режима задания имени сохраняемому сигналу. Одновременно кнопка отказа от сохранения
{
    Item_SmallButton, &pSetName, 0,
    {
        "Выход", "Exit",
        "Отказ от сохранения",
        "Failure to save"
    },
    OnPress_SetName_Exit,
    DrawSB_Exit
};


static void OnPress_SetName_Exit(void)
{
    OnPressSB_Exit();
    if (EXIT_FROM_SETNAME_TO == RETURN_TO_DISABLE_MENU)
    {
        ShortPressOnPageItem(PagePointerFromName(PageSB_Memory_SetName), 0);
    }
    else if (EXIT_FROM_SETNAME_TO == RETURN_TO_LAST_MEM)
    {
    }
    else if (EXIT_FROM_SETNAME_TO == RETURN_TO_INT_MEM)
    {
    }
    EXIT_FROM_SETNAME_TO = RETURN_TO_DISABLE_MENU;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const SButton bSetName_Delete =
{
    Item_SmallButton, &pSetName, 0,
    {
        "Удалить", "Delete",
        "Удаляет все введённые символы",
        "Deletes all entered characters"
    },
    OnPress_SetName_Delete,
    Draw_SetName_Delete
};

static void OnPress_SetName_Delete(void)
{
    FILE_NAME[0] = '\0';
}

static void Draw_SetName_Delete(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    painter.SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const SButton bSetName_Backspace =
{
    Item_SmallButton, &pSetName, 0,
    {
        "Backspace", "Backspace",
        "Удаляет последний символ",
        "Delete the last character"
    },
    OnPress_SetName_Backspace,
    Draw_SetName_Backspace
};

static void OnPress_SetName_Backspace(void)
{
    int size = strlen(FILE_NAME);
    if (size > 0)
    {
        FILE_NAME[size - 1] = '\0';
    }
}

static void Draw_SetName_Backspace(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_BACKSPACE);
    painter.SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const SButton bSetName_Insert =
{
    Item_SmallButton, &pSetName, 0,
    {
        "Вставить", "Insert",
        "Вводит очередной символ",
        "Print the next character"
    },
    OnPress_SetName_Insert,
    Draw_SetName_Insert
};

static void OnPress_SetName_Insert(void)
{
    int size = strlen(FILE_NAME);
    if (size < MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        FILE_NAME[size] = symbolsAlphaBet[INDEX_SYMBOL][0];
        FILE_NAME[size + 1] = '\0';
    }
}

static void Draw_SetName_Insert(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 2, y + 2, '\x26');
    painter.SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static const SButton bSetName_Save =
{
    Item_SmallButton, &pSetName, 0,
    {
        "Сохранить", "Save",
        "Сохранение на флеш под заданным именем",
        "Saving to flashdrive with the specified name"
    },
    OnPress_SetName_Save,
    Draw_SetName_Save
};

static void OnPress_SetName_Save(void)
{
    if (FDRIVE_IS_CONNECTED)
    {
        OnPress_SetName_Exit();
        NEED_SAVE_TO_FLASHDRIVE = 1;
    }
}

static void Draw_SetName_Save(int x, int y)
{
    if (FDRIVE_IS_CONNECTED)
    {
        painter.SetFont(TypeFont_UGO2);
        painter.Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_FLASH_DRIVE_BIG);
        painter.SetFont(TypeFont_8);
    }
}

/*

void DrawSB_MemLastSelect(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 3, y + 2, set.memory.strMemoryLast.isActiveModeSelect ? '\x2a' : '\x28');
    painter.SetFont(TypeFont_8);
}

void PressSB_MemLastSelect(void)
{
    set.memory.strMemoryLast.isActiveModeSelect = !set.memory.strMemoryLast.isActiveModeSelect;
}


void DrawSB_MemExtNewFolder(int x, int y)
{
    painter.SetFont(TypeFont_UGO2);
    painter.Draw4SymbolsInRect(x + 1, y, '\x46');
    painter.SetFont(TypeFont_8);
}

extern const Page pMemory;
extern const Page ppDrive;

*/
