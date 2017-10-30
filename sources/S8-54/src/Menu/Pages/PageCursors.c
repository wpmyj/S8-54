

#include "defines.h"
#include "PageCursors.h"
#include "Definition.h"
#include "Settings/Settings.h"
#include "Settings/SettingsTypes.h"
#include "Panel/Controls.h"
#include "Display/Colors.h"
#include "Display/Grid.h"
#include "Utils/Math.h"
#include "Utils/ProcessingSignal.h"
#include "Utils/GlobalFunctions.h"
#include "Log.h"
#include "Display/Painter.h"
#include "Hardware/Sound.h"
#include "../MenuFunctions.h"
#include <math.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mainPage;

static const  Choice cShow;                             ///< КУРСОРЫ - Показывать
static const  Choice cLookModeChanA;                    ///< КУРСОРЫ - Слежение канал 1
static const  Choice cLookModeChanB;                    ///< КУРСОРЫ - Слежение канал 2
static const  Choice cShowFreq;                         ///< КУРОСРЫ - 1/dT
static const   Page ppSet;                              ///< КУРСОРЫ - УСТАНОВИТЬ
static void  OnRegSet_Set(int angle);
static const SButton bSet_Exit;                         ///< КУРСОРЫ - УСТАНОВИТЬ - Выход
static const SButton bSet_Channel;                      ///< КУРСОРЫ - УСТАНОВИТЬ - Канал
static void   OnPress_Set_Channel(void);
static void      Draw_Set_Channel(int x, int y);
static void      Draw_Set_ChannelA(int x, int y);
static void      Draw_Set_ChannelB(int x, int y);
static const SButton bSet_U;                            ///< КУРСОРЫ - УСТАНОВИТЬ - Курсоры U
static void   OnPress_Set_U(void);
static void      Draw_Set_U(int x, int y);
static void      Draw_Set_U_disable(int x, int y);
static void      Draw_Set_U_disableBoth(int x, int y);
static void      Draw_Set_U_enableUpper(int x, int y);
static void      Draw_Set_U_enableLower(int x, int y);
static void      Draw_Set_U_enableBoth(int x, int y);
static const SButton bSet_T;                            ///< КУРСОРЫ - УСТАНОВИТЬ - Курсоры Т
static void   OnPress_Set_T(void);
static void      Draw_Set_T(int x, int y);
static void      Draw_Set_T_disable(int x, int y);
static void      Draw_Set_T_disableBoth(int x, int y);
static void      Draw_Set_T_enableLeft(int x, int y);
static void      Draw_Set_T_enableRight(int x, int y);
static void      Draw_Set_T_enableBoth(int x, int y);
static const SButton bSet_100;                          ///< КУРСОРЫ - УСТАНОВИТЬ - 100%
static void   OnPress_Set_100(void);
static void      Draw_Set_100(int x, int y);
static const SButton bSet_Movement;                     ///< КУРСОРЫ - УСТАНОВИТЬ - Перемещение
static void   OnPress_Set_Movement(void);
static void      Draw_Set_Movement(int x, int y);
static void      Draw_Set_Movement_Percents(int x, int y);
static void      Draw_Set_Movement_Points(int x, int y);

static void MoveCursUonPercentsOrPoints(int delta);
static void MoveCursTonPercentsOrPoints(int delta);

static void SetShiftCursPosU(Channel ch, int numCur, float delta);      ///< Изменить значение позиции курсора напряжения на delta точек.
static void SetShiftCursPosT(Channel ch, int numCurs, float delta);     ///< Изменить значение позиции курсора времени на delta точек.
static void SetCursPos100(Channel ch);                                  ///< Запомнить позиции курсоров, соответствующие 100%.
static void SetCursSource(Channel ch);                                  ///< Установить источник курсорных измерений.
static void IncCursCntrlU(Channel ch);                                  ///< Выбрать следующий курсор.
static void IncCursCntrlT(Channel ch);                                  ///< Выбрать следующий курсор.
static void SetCursPosU(Channel ch, int numCur, float pos);             ///< Установить позицию курсора напряжения.
//static void SetCursPosT(Channel ch, int numCur, float pos);             ///< Установить значение курсора по времени.

/// Рассчитывает условия отрисовки УГО малых кнопок управления выбором курсорами.
static void CalculateConditions(int16 pos0, int16 pos1, CursCntrl cursCntrl, bool *cond0, bool *cond1);
static void DrawMenuCursTime(int x, int y, bool left, bool right);
static void CalculateXY(int *x0, int *x1, int *y0, int *y1);
static int CalculateXforCurs(int x, bool left);
static int CalculateYforCurs(int y, bool top);
static void DrawMenuCursVoltage(int x, int y, bool top, bool bottom);


// КУРСОРЫ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page pCursors =
{
    Item_Page, &mainPage, 0,
    {
        "КУРСОРЫ", "CURSORS",
        "Курсорные измерения.",
        "Cursor measurements."
    },
    Page_Cursors,
    {
        (void *)&cShow,             // КУРСОРЫ - Показывать
        (void *)&cLookModeChanA,    // КУРСОРЫ - Слежение канал 1
        (void *)&cLookModeChanB,    // КУРСОРЫ - Слежение канал 2
        (void *)&cShowFreq,         // КУРОСРЫ - 1/dT
        (void *)&ppSet              // КУРСОРЫ - УСТАНОВИТЬ
    }
};

// КУРСОРЫ - Показывать ------------------------------------------------------------------------------------------------------------------------------
static const Choice cShow =
{
    Item_Choice, &pCursors, 0,
    {
        "Показывать", "Shown",
        "Включает/отключает курсоры.",
        "Enable/disable cursors."
    },
    {
        {"Нет", "No"},
        {"Да",  "Yes"}
    },
    (int8 *)&CURS_SHOW
};

// КУРСОРЫ - Слежение канал 1 -----------------------------------------------------------------------------------------------------------------------
static const Choice cLookModeChanA =
{
    Item_Choice, &pCursors, 0,
    {
        "Слежение \x8e, \x9e", "Tracking \x8e, \x9e"
        ,
        "Задаёт режим слежения за первым курсором времени и напряжения:\n"
        "1. \"Откл\" - курсор времени и курсор напряжения устанавливаются вручную.\n"
        "2. \"Напряжение\" - при ручном изменении положения курсора времени курсор напряжения автоматически отслеживают изменения сигнала.\n"
        "3. \"Время\" - при ручном изменении положения курсора напряжения курсор времени автоматически отслеживают изменения сигнала.\n"
        "4. \"Напряж и время\" - действует как один из предыдущих режимов, в зависимости от того, на какой курсор производилось последнее воздействие."
        ,
        /// \todo Перевод
        "Sets the mode tracking for second cursor:\n"
        "1. \"Disable\" - all cursors are set manually.\n"
        "2. \"Voltage\" - when manually changing the position of the cursor time cursors voltage automatically track changes in the signal.\n"
        "3. \"Time\" - when manually changing the position of the cursor voltage cursors time automatically track changes in the signal.\n"
        "4. \"Volt and time\" - acts as one of the previous modes, depending on which was carried out last effect cursors."
    },
    {
        {DISABLE_RU,        DISABLE_EN},
        {"Напряжение",      "Voltage"},
        {"Время",           "Time"},
        {"Напряж и время",  "Volt and time"}
    },
    (int8 *)&CURS_LOOK_MODE(A)
};

// КУРСОРЫ - Слежение канал 2 -----------------------------------------------------------------------------------------------------------------------
static const Choice cLookModeChanB =
{
    Item_Choice, &pCursors, 0,
    {
        "Слежение \x8f, \x9f", "Tracking \x8f, \x9f"
        ,
        "Задаёт режим слежения за вторым курсором времени и напряжения:\n"
        "1. \"Откл\" - курсор времени и курсор напряжения устанавливаются вручную.\n"
        "2. \"Напряжение\" - при ручном изменении положения курсора времени курсор напряжения автоматически отслеживают изменения сигнала.\n"
        "3. \"Время\" - при ручном изменении положения курсора напряжения курсор времени автоматически отслеживают изменения сигнала.\n"
        "4. \"Напряж и время\" - действует как один из предыдущих режимов, в зависимости от того, на какой курсор производилось последнее воздействие."
        ,
        "Sets the mode tracking cursors channel 2:\n"
        "1. \"Disable\" - all cursors are set manually.\n"
        "2. \"Voltage\" - when manually changing the position of the cursor time cursors voltage automatically track changes in the signal.\n"
        "3. \"Time\" - when manually changing the position of the cursor voltage cursors time automatically track changes in the signal.\n"
        "4. \"Volt and time\" - acts as one of the previous modes, depending on which was carried out last effect cursors."
    },
    {
        {DISABLE_RU,        DISABLE_EN},
        {"Напряжение",      "Voltage"},
        {"Время",           "Time"},
        {"Напряж. и время", "Volt. and time"}
    },
    (int8 *)&CURS_LOOK_MODE(B)
};

// КУРОСРЫ - 1/dT -----------------------------------------------------------------------------------------------------------------------------------
static const Choice cShowFreq =
{
    Item_Choice, &pCursors, 0,
    {
        "1/dT", "1/dT",
        "Если выбрано \"Вкл\", в правом верхнем углу выводится величина, обратная расстоянию между курсорами времени - частота сигнала, один период "
        "которого равен расстоянию между временными курсорами.",
        "If you select \"Enable\" in the upper right corner displays the inverse of the distance between cursors time - frequency signal, a period "
        "equal to the distance between the time cursors."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8 *)&CURSORS_SHOW_FREQ
};

// КУРСОРЫ - УСТАНОВИТЬ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page ppSet =
{
    Item_Page, &pCursors, 0,
    {
        "УСТАНОВИТЬ", "SET",
        "Переход в режим курсорных измерений",
        "Switch to cursor measures"
    },
    PageSB_Cursors_Set,
    {
        (void *)&bSet_Exit,     // КУРСОРЫ - УСТАНОВИТЬ - Выход
        (void *)&bSet_Channel,  // КУРСОРЫ - УСТАНОВИТЬ - Канал
        (void *)&bSet_U,        // КУРСОРЫ - УСТАНОВИТЬ - Курсоры U
        (void *)&bSet_T,        // КУРСОРЫ - УСТАНОВИТЬ - Курсоры Т
        (void *)&bSet_100,      // КУРСОРЫ - УСТАНОВИТЬ - 100%
        (void *)&bSet_Movement  // КУРСОРЫ - УСТАНОВИТЬ - Перемещение
    },
    true, 0, 0, OnRegSet_Set
};

static void OnRegSet_Set(int angle)
{
    if (CURS_ACTIVE_U)
    {
        MoveCursUonPercentsOrPoints(angle);
    }
    else
    {
        MoveCursTonPercentsOrPoints(angle);
    }
    Sound_RegulatorShiftRotate();
}

// КУРСОРЫ - УСТАНОВИТЬ - Выход --------------------------------------------------------------------------------------------------------------------
static const SButton bSet_Exit =
{
    Item_SmallButton, &ppSet, 0,
    {
        "Выход", "Exit",
        "Кнопка для выхода в предыдущее меню",
        "Button to return to the previous menu"
    },
    OnPressSB_Exit,
    DrawSB_Exit
};

// КУРСОРЫ - УСТАНОВИТЬ - Канал --------------------------------------------------------------------------------------------------------------------
static const SButton bSet_Channel =
{
    Item_SmallButton, &ppSet, 0,
    {
        "Канал", "Channel",
        "Выбор канала для курсорных измерений",
        "Channel choice for measurements"
    },
    OnPress_Set_Channel,
    Draw_Set_Channel,
    {
        {
            Draw_Set_ChannelA,
            "канал 1",
            "channel 1"
        },
        {
            Draw_Set_ChannelB,
            "канал 2",
            "channel 2"
        }
    }
};

static void OnPress_Set_Channel(void)
{
    Channel source = CURS_SOURCE_A ? B : A;
    SetCursSource(source);
}

static void Draw_Set_Channel(int x, int y)
{
    static const pFuncVII func[2] = {Draw_Set_ChannelA, Draw_Set_ChannelB};
    func[CURS_SOURCE](x, y);
}

static void Draw_Set_ChannelA(int x, int y)
{
    Painter_DrawText(x + 7, y + 5, "1");
}

static void Draw_Set_ChannelB(int x, int y)
{
    Painter_DrawText(x + 7, y + 5, "2");
}

// КУРСОРЫ - УСТАНОВИТЬ - Курсоры U ----------------------------------------------------------------------------------------------------------------
static const SButton bSet_U =      // Выбор курсора напряжения - курсор 1, курсор 2, оба курсора или отключены.
{
    Item_SmallButton, &ppSet, 0,
    {
        "Курсоры U", "Cursors U",
        "Выбор курсоров напряжения для индикации и управления",
        "Choice of cursors of voltage for indication and management"
    },
    OnPress_Set_U,
    Draw_Set_U,
    {
        {Draw_Set_U_disable,        "курсоры напряжения выключены", "cursors of tension are switched off"},
        {Draw_Set_U_disableBoth,    "курсоры напряжения включены", "cursors of tension are switched on"},
        {Draw_Set_U_enableUpper,    "курсоры напряжения включены, управление верхним курсором",
                                    "cursors of tension are switched on, control of the top cursor"},
        {Draw_Set_U_enableLower,    "курсоры напряжения включены, управление нижним курсором",
                                    "cursors of tension are switched on, control of the lower cursor"},
        {Draw_Set_U_enableBoth,     "курсоры напряжения включены, управление обоими курсорами",
                                    "cursors of tension are switched on, control of both cursors"}
    }
};

static void OnPress_Set_U(void)
{
    if (CURS_ACTIVE_U || CURsU_DISABLED)
    {
        IncCursCntrlU(CURS_SOURCE);
    }
    CURS_ACTIVE = CursActive_U;
}

static void Draw_Set_U(int x, int y)
{
    Channel source = CURS_SOURCE;
    if (CURsU_DISABLED)
    {
       Draw_Set_U_disable(x, y);
    }
    else
    {
        if (!CURS_ACTIVE_U)
        {
            Draw_Set_U_disableBoth(x, y);
        }
        else
        {
            bool condTop = false, condDown = false;
            CalculateConditions((int16)sCursors_GetCursPosU(source, 0), (int16)sCursors_GetCursPosU(source, 1), CURsU_CNTRL, &condTop, &condDown);
            if (condTop && condDown)
            {
                Draw_Set_U_enableBoth(x, y);
            }
            else if (condTop)
            {
                Draw_Set_U_enableUpper(x, y);
            }
            else
            {
                Draw_Set_U_enableLower(x, y);
            }
        }
    }
}

static void CalculateConditions(int16 pos0, int16 pos1, CursCntrl cursCntrl, bool *cond0, bool *cond1)
{
    bool zeroLessFirst = pos0 < pos1;
    *cond0 = cursCntrl == CursCntrl_1_2 || (cursCntrl == CursCntrl_1 && zeroLessFirst) || (cursCntrl == CursCntrl_2 && !zeroLessFirst);
    *cond1 = cursCntrl == CursCntrl_1_2 || (cursCntrl == CursCntrl_1 && !zeroLessFirst) || (cursCntrl == CursCntrl_2 && zeroLessFirst);
}

static void Draw_Set_U_disable(int x, int y)
{
    Painter_DrawText(x + 7, y + 5, "U");
}

static void Draw_Set_U_disableBoth(int x, int y)
{
    DrawMenuCursVoltage(x + 7, y + 5, false, false);
}

static void Draw_Set_U_enableUpper(int x, int y)
{
    DrawMenuCursVoltage(x + 7, y + 5, true, false);
}

static void Draw_Set_U_enableLower(int x, int y)
{
    DrawMenuCursVoltage(x + 7, y + 5, false, true);
}

static void Draw_Set_U_enableBoth(int x, int y)
{
    DrawMenuCursVoltage(x + 7, y + 5, true, true);
}

static void DrawMenuCursVoltage(int x, int y, bool top, bool bottom)
{
    x -= 65;
    y -= 21;
    int x0 = x, x1 = x, y0 = y, y1 = y;
    CalculateXY(&x0, &x1, &y0, &y1);
    for(int i = 0; i < (top ? 3 : 1); i++)
    {
        Painter_DrawHLine(y0 + i, x0, x1);
    }
    for(int i = 0; i < (bottom ? 3 : 1); i++)
    {
        Painter_DrawHLine(y1 - i, x0, x1);
    }
}

// КУРСОРЫ - УСТАНОВИТЬ - Курсоры Т ----------------------------------------------------------------------------------------------------------------
static const SButton bSet_T =      // Выбор курсора времени - курсор 1, курсор 2, оба курсора или отключены.
{
    Item_SmallButton, &ppSet, 0,
    {
        "Курсоры T", "Cursors T",
        "Выбор курсоров времени для индикации и управления",
        "Choice of cursors of time for indication and management"
    },
    OnPress_Set_T,
    Draw_Set_T,
    {
        {Draw_Set_T_disable, "курсоры времени выключены", "cursors of time are switched off"},
        {Draw_Set_T_disableBoth, "курсоры времени включены", "cursors of time are switched on"},
        {Draw_Set_T_enableLeft, "курсоры времени включены, управление левым курсором", "cursors of time are switched on, control of the left cursor"},
        {Draw_Set_T_enableRight, "курсоры времени включены, управление правым курсором", "cursors of time are switched on, control of the right cursor"},
        {Draw_Set_T_enableBoth, "курсоры времени включены, управление обоими курсорами", "cursors of time are switched on, control of both cursors"}
    }
};

static void OnPress_Set_T(void)
{
    if (CURS_ACTIVE_T || CURsT_DISABLED)
    {
        IncCursCntrlT(CURS_SOURCE);
    }
    CURS_ACTIVE = CursActive_T;
}

static void Draw_Set_T(int x, int y)
{
    if (CURsT_DISABLED)
    {
        Draw_Set_T_disable(x, y);
    }
    else
    {
        if (!CURS_ACTIVE_T)
        {
            Draw_Set_T_disableBoth(x, y);
        }
        else
        {
            bool condLeft = false, condDown = false;
            Channel source = CURS_SOURCE;
            CalculateConditions((int16)CURsT_POS(source, 0), (int16)CURsT_POS(source, 1), CURsT_CNTRL, &condLeft, &condDown);
            if (condLeft && condDown)
            {
                Draw_Set_T_enableBoth(x, y);
            }
            else if (condLeft)
            {
                Draw_Set_T_enableLeft(x, y);
            }
            else
            {
                Draw_Set_T_enableRight(x, y);
            }
        }
    }
}

static void Draw_Set_T_disable(int x, int y)
{
    Painter_DrawText(x + 7, y + 5, "T");
}

static void Draw_Set_T_disableBoth(int x, int y)
{
    DrawMenuCursTime(x, y, false, false);
}

static void Draw_Set_T_enableLeft(int x, int y)
{
    DrawMenuCursTime(x, y, true, false);
}

static void Draw_Set_T_enableRight(int x, int y)
{
    DrawMenuCursTime(x, y, false, true);
}

static void Draw_Set_T_enableBoth(int x, int y)
{
    DrawMenuCursTime(x, y, true, true);
}

static void DrawMenuCursTime(int x, int y, bool left, bool right)
{
    x -= 58;
    y -= 16;
    int x0 = x, x1 = x, y0 = y, y1 = y;
    CalculateXY(&x0, &x1, &y0, &y1);
    for(int i = 0; i < (left ? 3 : 1); i++)
    {
        Painter_DrawVLine(x0 + i, y0, y1);
    }
    for(int i = 0; i < (right ? 3 : 1); i++)
    {
        Painter_DrawVLine(x1 - i, y0, y1);
    }
}

static void CalculateXY(int *x0, int *x1, int *y0, int *y1)
{
    *x0 = CalculateXforCurs(*x0, true);
    *x1 = CalculateXforCurs(*x1, false);
    *y0 = CalculateYforCurs(*y0, true);
    *y1 = CalculateYforCurs(*y1, false);
}

static int CalculateXforCurs(int x, bool left)
{
    return left ? x + MI_WIDTH - 20 : x + MI_WIDTH - 5;
}

static int CalculateYforCurs(int y, bool top)
{
    return top ? y + MI_HEIGHT / 2 + 4 : y + MI_HEIGHT - 2;
}


// КУРСОРЫ - УСТАНОВИТЬ - 100% ---------------------------------------------------------------------------------------------------------------------
static const SButton bSet_100 =    // Установка 100 процентов в текущие места курсоров.
{
    Item_SmallButton, &ppSet, 0,
    {
        "100%", "100%",
        "Используется для процентных измерений. Нажатие помечает расстояние между активными курсорами как 100%",
        "It is used for percentage measurements. Pressing marks distance between active cursors as 100%"
    },
    OnPress_Set_100,
    Draw_Set_100
};

static void OnPress_Set_100(void)
{
    SetCursPos100(CURS_SOURCE);
}

static void Draw_Set_100(int x, int y)
{
    Painter_SetFont(TypeFont_5);
    Painter_DrawText(x + 2, y + 3, "100%");
    Painter_SetFont(TypeFont_8);
}

// КУРСОРЫ - УСТАНОВИТЬ - Перемещение --------------------------------------------------------------------------------------------------------------
static const SButton bSet_Movement =     // Переключение шага перемещения курсоров - по пикселям или по процентам.
{
    Item_SmallButton, &ppSet, 0,
    {
        "Перемещение", "Movement",
        "Выбор шага перемещения курсоров - проценты или точки",
        "Choice of a step of movement of cursors - percent or points"
    },
    OnPress_Set_Movement,
    Draw_Set_Movement,
    {
        {Draw_Set_Movement_Percents, "шаг перемещения курсоров кратен одному проценту", "the step of movement of cursors is multiple to one percent"},
        {Draw_Set_Movement_Points, "шаг перемещения курсора кратен одному пикселю", "the step of movement of the cursor is multiple to one pixel"}
    }
};

static void OnPress_Set_Movement(void)
{
    CircleIncreaseInt8((int8 *)&CURS_MOVEMENT, 0, 1);
}

static void Draw_Set_Movement(int x, int y)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        Draw_Set_Movement_Percents(x, y);
    }
    else
    {
        Draw_Set_Movement_Points(x, y);
    }
}

static void Draw_Set_Movement_Percents(int x, int y)
{
    Painter_DrawText(x + 6, y + 5, "\x83");
}

static void Draw_Set_Movement_Points(int x, int y)
{
    Painter_SetFont(TypeFont_5);
    Painter_DrawText(x + 4, y + 3, "тчк");
    Painter_SetFont(TypeFont_8);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_POS_U   200
#define MAX_POS_T   280

static void SetShiftCursPosU(Channel ch, int numCur, float delta)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        CURsU_POS(ch, numCur) = LimitationFloat(CURsU_POS(ch, numCur) - delta, 0, MAX_POS_U);   /// \todo одинаковые ветки
    }
    else
    {
        CURsU_POS(ch, numCur) = LimitationFloat(CURsU_POS(ch, numCur) - delta, 0, MAX_POS_U);
    }
}

static void SetCursPosU(Channel ch, int numCur, float pos)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        CURsU_POS(ch, numCur) = LimitationFloat(pos, 0, MAX_POS_U);                             /// \todo одинаковые ветки
    }
    else
    {
        CURsU_POS(ch, numCur) = LimitationFloat(pos, 0, MAX_POS_U);
    }
}

void SetShiftCursPosT(Channel ch, int numCur, float delta)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        // CURsT_POS(ch, numCur) = LimitationFloat(CURsT_POS(ch, numCur) + delta, 0, MAX_POS_T);   /// \todo одинаковые ветки
        SetCursPosT_temp(ch, numCur, LimitationFloat(CURsT_POS(ch, numCur) + delta, 0, MAX_POS_T));
    }
    else
    {
        // CURsT_POS(ch, numCur) = LimitationFloat(CURsT_POS(ch, numCur) + delta, 0, MAX_POS_T);
        SetCursPosT_temp(ch, numCur, LimitationFloat(CURsT_POS(ch, numCur) + delta, 0, MAX_POS_T));
    }
}

void SetCursPosT(Channel ch, int numCur, float pos)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        // CURsT_POS(ch, numCur) = LimitationFloat(pos, 0, MAX_POS_T);                             /// \todo одинаковые ветки
        SetCursPosT_temp(ch, numCur, LimitationFloat(pos, 0, MAX_POS_T));
    }
    else
    {
        // CURsT_POS(ch, numCur) = LimitationFloat(pos, 0, MAX_POS_T);
        SetCursPosT_temp(ch, numCur, LimitationFloat(pos, 0, MAX_POS_T));
    }
}

static void SetCursPos100(Channel ch)
{
    dUperc(ch) = (float)fabs(CURsU_POS(ch, 0) - CURsU_POS(ch, 1));
    dTperc(ch) = (float)fabs(CURsT_POS(ch, 0) - CURsT_POS(ch, 1));
}

static void SetCursSource(Channel ch)
{
    CURS_SOURCE = ch;
}

static void IncCursCntrlU(Channel ch)
{
    CircleIncreaseInt8((int8 *)&CURsU_CNTRL_CH(ch), 0, 3);
}

static void IncCursCntrlT(Channel ch)
{
    CircleIncreaseInt8((int8 *)&CURsT_CNTRL_CH(ch), 0, 3);
}

void CursorsUpdate(void)
{
    Channel source = CURS_SOURCE;

    float posT0 = 0.0f, posT1 = 0.0f;

    if(CURS_ACTIVE_T && (CURS_LOOK_U(0) || CURS_LOOK_BOTH(0)))
    {
        float posU0 = Processing_GetCursU(source, CURsT_POS(source, 0));
        SetCursPosU(source, 0, posU0);
    }
    if(CURS_ACTIVE_T && (CURS_LOOK_U(1) || CURS_LOOK_BOTH(1)))
    {
        float posU1 = Processing_GetCursU(source, CURsT_POS(source, 1));
        SetCursPosU(source, 1, posU1);
    }
    if(CURS_ACTIVE_U && (CURS_LOOK_T(0) || CURS_LOOK_BOTH(0)))
    {
        float posU0 = CURsU_POS(source, 0);
        posT0 = Processing_GetCursT(source, posU0, 0);
        SetCursPosT(source, 0, posT0);
    }
    if(CURS_ACTIVE_U && (CURS_LOOK_T(1) || CURS_LOOK_BOTH(1)))
    {
        float posU1 = CURsU_POS(source, 1);
        posT1 = Processing_GetCursT(source, posU1, 1);
        SetCursPosT(source, 1, posT1);
    }
}

static void MoveCursUonPercentsOrPoints(int delta)
{
    float value = (float)delta;

    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        value *= dUperc(CURS_SOURCE) / 100.0f;
    }

    if (CURsU_CNTRL_1 || CURsU_CNTRL_1_2)
    {
        SetShiftCursPosU(CURS_SOURCE, 0, value);
    }
    if (CURsU_CNTRL_2 || CURsU_CNTRL_1_2)
    {
        SetShiftCursPosU(CURS_SOURCE, 1, value);
    }
    CursorsUpdate();
}

static void MoveCursTonPercentsOrPoints(int delta)
{
    float value = (float)delta;

    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        value *= dTperc(CURS_SOURCE) / 100.0f;
    }

    if (CURsT_CNTRL_1 || CURsT_CNTRL_1_2)
    {
        SetShiftCursPosT(CURS_SOURCE, 0, value);
    }
    if (CURsT_CNTRL_2 || CURsT_CNTRL_1_2)
    {
        SetShiftCursPosT(CURS_SOURCE, 1, value);
    }
    CursorsUpdate();
}

bool IsRegSetActiveOnCursors(void)
{
    return ((GetNameOpenedPage() == PageSB_Cursors_Set) &&
        ((CURS_ACTIVE_U && CURsU_ENABLED) || (CURS_ACTIVE_T && CURsT_ENABLED)));
}
