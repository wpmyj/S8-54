#pragma once
#include "Display/Colors.h"
#include "Display/Display.h"
#include "defines.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Menu
 *  @{
 *  @defgroup MenuItems Menu Items
 *  @{
 */

extern int8 gCurDigit;

#define MAX_NUM_SUBITEMS_IN_CHOICE  12  ///< Максимальное количество вариантов выбора в элементе Choice.
#define MAX_NUM_ITEMS_IN_PAGE       16  ///< Максимальное количество пунктов на странице.
#define MENU_ITEMS_ON_DISPLAY       5   ///< Сколько пунктов меню помещается на экране по вертикали.
#define MAX_NUM_CHOICE_SMALL_BUTTON 6   ///< Максимальное количество вариантов маленькой кнопки + 1


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Разные виды пунктов меню
typedef enum
{
    Item_None,           
    Item_Choice,        ///< Пункт выбора - позволяет выбрать одно из нескольких заданных значений.
    Item_Button,        ///< Кнопка.
    Item_Page,          ///< Страница.
    Item_Governor,      ///< Регулятор - позволяет выбрать любое целое числовое значение из заранее заданного диапазаона.
    Item_Time,          ///< Позволяет ввести время.
    Item_IP,            ///< Позволяет ввести IP-адрес.
    Item_GovernorColor, ///< Позволяет выбрать цвет.
    Item_Formula,       ///< Позволяет выбрать знак и коэффициенты для математической формулы (умножение и сложение)
    Item_MAC,           ///< MAC-адрес
    Item_ChoiceReg,     ///< Элемент выбора, в котором выбор осуществляется не кнопкой, а ручкой
    Item_SmallButton,   ///< Кнопка для режима малых кнопок
    Item_NumberItems
} TypeItem;

/// Здесь ID всех представленных в меню страниц
typedef enum
{
    Page_Main,                      ///< Главная страница меню
    Page_Display,                   ///< ДИСПЕЙ
    Page_Display_Accum,             ///< ДИСПЛЕЙ - НАКОПЛЕНИЕ
    Page_Display_Average,           ///< ДИСПЛЕЙ - УСРЕДНЕНИЕ
    Page_Display_Grid,              ///< ДИСПЛЕЙ - СЕТКА
    Page_Display_Settings,          ///< ДИСПЛЕЙ - НАСТРОЙКИ
    Page_Display_Settings_Colors,   ///< ДИСПЛЕЙ - НАСТРОЙКИ - ЦВЕТА
    Page_ChannelA,                  ///< КАНАЛ 1
    Page_ChannelB,                  ///< КАНАЛ 2
    Page_Trig,                      ///< СИНХР
    Page_Trig_Search,               ///< СИНХР - ПОИСК
    Page_Time,                      ///< РАЗВЕРТКА
    Page_Cursors,                   ///< КУРСОРЫ
    PageSB_Cursors_Set,             ///< КУРСОРЫ - УСТАНОВИТЬ
    Page_Memory,                    ///< ПАМЯТЬ
    PageSB_Memory_Last,             ///< ПАМЯТЬ - ПОСЛЕДНИЕ
    PageSB_Memory_Internal,         ///< ПАМЯТЬ - ВНУТР ЗУ
    Page_Memory_Drive,              ///< ПАМЯТЬ - ВНЕШН ЗУ
    PageSB_Memory_Drive_Manager,    ///< ПАМЯТЬ - ВНЕШН ЗУ - КАТАЛОГ
    PageSB_Memory_Drive_Mask,       ///< ПАМЯТЬ - ВНЕШН ЗУ - МАСКА
    PageSB_Memory_SetName,          ///< \brief Появляется перед сохранением файла на флешку для задания имени файла при соответствующей опции 
                                    ///< ВНЕШН ЗУ - Имя файла
    Page_Measures,                  ///< ИЗМЕРЕНИЯ
    PageSB_Measures_Tune,           ///< ИЗМЕРЕНИЯ - НАСТРОИТЬ
    Page_Service,                   ///< СЕРВИС
    Page_Service_Calibrator,        ///< СЕРВИС - КАЛИБРАТОР
    Page_Service_Ethernet,          ///< СЕРВИС - ETHERNET
    Page_Service_FreqMeter,         ///< СЕРВИС - ЧАСТОТОМЕР
    Page_Service_Sound,             ///< СЕРВИС - ЗВУК
    Page_Service_RTC,               ///< СЕРВИС - ВРЕМЯ
    Page_Service_FFT,               ///< СЕРВИС - СПЕКТР
    PageSB_Service_FFT_Cursors,     ///< СЕРВИС - СПЕКТР - КУРСОРЫ
    PageSB_Service_Function,        ///< СЕРВИС - ФУНКЦИЯ
    PageSB_Service_Information,     ///< СЕРВИС - ИНФОРМАЦИЯ
    PageSB_Service_Recorder,        ///< СЕРВИС - РЕГИСТРАТОР
    PageSB_Help,                    ///< ПОМОЩЬ
    Page_Debug,                     ///< ОТЛАДКА
    Page_Debug_Console,             ///< ОТЛАДКА - КОНСОЛЬ
    Page_Debug_Console_Registers,   ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ
    Page_Debug_ADC,                 ///< ОТЛАДКА - АЦП
    Page_Debug_ADC_Balance,         ///< ОТЛАДКА - АЦП - БАЛАНС
    Page_Debug_ADC_Stretch,         ///< ОТЛАДКА - АЦП - РАСТЯЖКА
    Page_Debug_ADC_Shift,           ///< ОТЛАДКА - АЦП - ДОП СМЕЩ
    Page_Debug_ADC_AltShift,        ///< ОТЛАДКА - АЦП - ДОП СМЕЩ ПАМ
    Page_Debug_Rand,                ///< ОТЛАДКА - РАНД-ТОР
    Page_Debug_Channels,            ///< ОТЛАДКА - КАНАЛЫ
    PageSB_Debug_Settings,          ///< ОТЛАДКА - НАСТРОЙКИ
    PageSB_Debug_SerialNumber,      ///< ОТЛАДКА - С/Н
    Page_NumPages,
    Page_NoPage
} NamePage;

/// Общая часть для всех типов элементов меню
#define COMMON_PART_MENU_ITEM                                                                                   \
    TypeItem                type;           /* Тип итема */                                                     \
    const struct Page      *keeper;         /* Адрес страницы, которой принадлежит. Для Page_Main = 0 */        \
    pFuncBV                 funcOfActive;   /* Активен ли данный элемент */                                     \
    const char             *titleHint[4];   /* Название страницы на русском и английском языках. Также подсказка для режима помощи */

struct SButton;

/// Описывает страницу меню.
struct Page
{
    COMMON_PART_MENU_ITEM
    NamePage name;                          ///< Имя из перечисления NamePage
    void    *items[MAX_NUM_ITEMS_IN_PAGE];  ///< Здесь указатели на пункты этой страницы (в обычной странице)
                                            ///< для страницы малых кнопок  здесь хранятся 6 указателей на SButton : 0 - B_Menu, 1...5 - B_F1...B_F5
    bool     isPageSB;                      ///< Если true, то это страница малых кнопок
    pFuncVV  funcOnPress;                   ///< Будет вызываться при нажатии на свёрнутую страницу
    pFuncVV  funcOnDraw;                    ///< Будет вызываться после отрисовки кнопок
    pFuncVI  funcRegSetSB;                  ///< В странице малых кнопок вызывается при повороте ручки установка
    int NumSubPages() const;                ///< Dозвращает число подстраниц в странице по адресу page
    int NumItems() const;                   ///< Возвращает количество элементов в странице по адресу page
    int NumCurrentSubPage();                ///< Возвращает номер текущей подстранцы элемента по адресу page
    NamePage GetNamePage() const;           ///< Возвращает имя страницы page
    void SetCurrentSB() const;              ///< Установить текущей данную страницу с мылыми кнопками.
    
    void *Item(int numElement) const;       ///< Возвращает адрес элемента меню заданной страницы
    SButton* SmallButonFromPage(int numButton);
    /// \todo Возвращает позицию первого элемента страницы по адресу page на экране. Если текущая подстраница 0, это будет 0, если текущая 
    /// подстраница 1, это будет 5 и т.д.
    int PosItemOnTop();
    /// Вызывает функцию короткого нажатия кнопки над итемом numItem страницы page
    void ShortPressOnItem(int numItem);
    /// Возвращает позицию текущего элемента странцы page
    int8 PosCurrentItem() const;
    void ChangeSubPage(int delta) const;
    void Draw(int x, int y);
};

typedef struct Page Page;

/// Пустая позиция
typedef struct
{
    COMMON_PART_MENU_ITEM
} Empty;

/// Описывает кнопку.
typedef struct
{
    COMMON_PART_MENU_ITEM
    pFuncVV     funcOnPress;    ///< Функция, которая вызывается при нажатии на кнопку.
    pFuncVII    funcForDraw;    ///< Функция будет вызываться во время отрисовки кнопки.
    void CallFuncOnDraw(int x, int y);
    void Draw(int x, int y);
} Button;

typedef struct
{
    pFuncVII    funcDrawUGO;    ///< Указатель на функцию отрисовки изображения варианта кнопки
    const char * const helpUGO[2];     ///< Подпись к данному изображению.
} StructHelpSmallButton;

/// Описывает кнопку для дополнительного режима меню.
struct SButton
{
    COMMON_PART_MENU_ITEM
    pFuncVV                 funcOnPress;    ///< Эта функция вызвается для обработки нажатия кнопки.
    pFuncVII                funcOnDraw;     ///< Эта функция вызывается для отрисовки кнопки в месте с координатами x, y.
    StructHelpSmallButton   hintUGO[MAX_NUM_CHOICE_SMALL_BUTTON]; 
    void Draw(int x, int y);
};

/// Описывает регулятор.
typedef struct
{
    COMMON_PART_MENU_ITEM
    int16  *cell;
    int16   minValue;       ///< Минмальное значение, которое может принимать регулятор.
    int16   maxValue;       ///< Максимальное значение.
    pFuncVV funcOfChanged;  ///< Функция, которую нужно вызывать после того, как значение регулятора изменилось.
    pFuncVV funcBeforeDraw; ///< Функция, которая вызывается перед отрисовкой
    /// Запускает процессс анимации инкремента или декремента элемента меню типа Governor (в зависимости от знака delta).
    void StartChange(int detla);
    /// Возвращает следующее большее значение, которое может принять governor.
    int16 NextValue();
    /// Возвращает следующее меньшее значение, которое может принять governor.
    int16 PrevValue();
    /// Рассчитывате следующий кадр анимации.
    float Step();
    /// Изменяет значение в текущей позиции при раскрытом элементе.
    void ChangeValue(int delta);
    /// При открытом элементе переставляет курсор на следующую позицию.
    void NextPosition();
    /// Возвращает число знакомест в поле для ввода элемента governor. Зависит от максимального значения, которое может принимать governor.
    int  NumDigits();
    void Draw(int x, int y, bool opened);
    void DrawOpened(int x, int y);
    void DrawClosed(int x, int y);
    void DrawValue(int x, int y);
    void DrawLowPart(int x, int y, bool pressed, bool shade);
} Governor;

typedef struct
{
    COMMON_PART_MENU_ITEM
    uint8 *ip0;
    uint8 *ip1;
    uint8 *ip2;
    uint8 *ip3;
    pFuncVB funcOfChanged;
    uint16 *port;
    void NextPosition();                            ///< При открытом элементе переставляет курсор на следующую позицию.
    void ChangeValue(int delta);                    ///< Изменяет значение в текущей позиции при открытом элементе.
    void GetNumPosIPvalue(int *numIP, int *selPos); ///< Возвращает номер текущего байта (4 - номер порта) и номер текущей позиции в байте.
    void Draw(int x, int y, bool opened);
    void DrawOpened(int x, int y);
    void DrawClosed(int x, int y);
    void DrawValue(int x, int y);
    void DrawLowPart(int x, int y, bool pressed, bool shade);
} IPaddress;

typedef struct
{
    COMMON_PART_MENU_ITEM
    uint8 *mac0;
    uint8 *mac1;
    uint8 *mac2;
    uint8 *mac3;
    uint8 *mac4;
    uint8 *mac5;
    pFuncVB funcOfChanged;
    void ChangeValue(int delta);
    void Draw(int x, int y, bool opened);
    void DrawOpened(int x, int y);
    void DrawClosed(int x, int y);
    void DrawValue(int x, int y);
    void DrawLowPart(int x, int y, bool pressed, bool shade);
} MACaddress;

/// Описывает элемент меню для установки коэффициентов и знаков математической формулы
#define FIELD_SIGN_MEMBER_1_ADD 0
#define FIELD_SIGN_MEMBER_1_MUL 1
#define FIELD_SIGN_MEMBER_2_ADD 2
#define FIELD_SIGN_MEMBER_2_MUL 3
#define POS_SIGN_MEMBER_1       0
#define POS_KOEFF_MEMBER_1      1
#define POS_SIGN_MEMBER_2       2
#define POS_KOEFF_MEMBER_2      3
typedef struct
{
    COMMON_PART_MENU_ITEM
    int8   *function;       ///< Адрес ячейки, где хранится Function, из которой берётся знак операции
    int8   *koeff1add;      ///< Адрес коэффициента при первом члене для сложения
    int8   *koeff2add;      ///< Адрес коэффициента при втором члене для сложения
    int8   *koeff1mul;      ///< Адрес коэффициента при первом члене для умножения
    int8   *koeff2mul;      ///< Адрес коэффициента при втором члене для умножения
    int8   *curDigit;       ///< Текущий разряд : 0 - знак первого члена, 1 - коэффициент первого члена, 2 - знак второго члена, 3 - коэффициент второго члена
    pFuncVV funcOfChanged;  ///< Эта функция вызывается после изменения состояния элемента управления.
    void Draw(int x, int y, bool opened);
    void DrawClosed(int x, int y);
    void DrawLowPart(int x, int y, bool pressed, bool shade);
    void WriteText(int x, int y, bool opened);
} Formula;

typedef struct
{
    COMMON_PART_MENU_ITEM
    ColorType  *ct;                 ///< Структура для описания цвета.
    pFuncVV     funcOnChanged;      ///< Эту функцию нужно вызывать после изменения значения элемента.
    void ChangeValue(int delta);    ///< Изменить яркость цвета в governor.
    void Draw(int x, int y, bool opened);
private:
    void DrawOpened(int x, int y);
    void DrawClosed(int x, int y);
    void DrawValue(int x, int y, int delta);
} GovernorColor;

typedef struct
{
    COMMON_PART_MENU_ITEM
    const char * const  names[MAX_NUM_SUBITEMS_IN_CHOICE][2];   ///< Варианты выбора на русском и английском языках.
    int8 *               cell;                                  ///< Адрес ячейки, в которой хранится позиция текущего выбора.
    pFuncVB			    funcOnChanged;                          ///< Функция должна вызываться после изменения значения элемента.
    pFuncVII            funcForDraw;                            ///< Функция вызывается после отрисовки элемента. 
    void StartChange(int delta);
    float Step();                                               ///< Рассчитывает следующий кадр анимации.
    void ChangeIndex(int delta);                                ///< Изменяет значение choice в зависимости от величины и знака delta.
    int NumSubItems();                                          ///< Возвращает количество вариантов выбора в элементе по адресу choice
    void Draw(int x, int y, bool opened);
    void DrawOpened(int x, int y);
    void DrawClosed(int x, int y);
    /// Возвращает имя текущего варианта выбора элемента choice, как оно записано в исходном коде программы
    const char *NameCurrentSubItem();
    /// Возвращает имя следующего варианта выбора элемента choice, как оно записано в исходном коде программы
    const char *NameNextSubItem();
    const char *NamePrevSubItem();
    /// Возвращает имя варианта выбора элемента choice в позиции i как оно записано в исходном коде программы
    const char *NameSubItem(int i);
} Choice;


#define iEXIT   0
#define iDAY    1
#define iMONTH  2
#define iYEAR   3
#define iHOURS  4
#define iMIN    5
#define iSEC    6
#define iSET    7

/// Устанавливает и показывает время.
typedef struct
{
    COMMON_PART_MENU_ITEM
    int8 *   curField;   ///< Текущее поле установки. 0 - выход, 1 - сек, 2 - мин, 3 - часы, 4 - день, 5 - месяц, 6 - год, 7 - установить.
    int8 *   hours;
    int8 *   minutes;
    int8 *   seconds;
    int8 *   month;
    int8 *   day;
    int8 *   year;
    void SetOpened();
    void IncCurrentPosition();
    void SetNewTime();
    void SelectNextPosition();
    void DecCurrentPosition();
} Time;


/// Структура для хранения меню, вызываемого по нажатию ручки
typedef struct
{
    int16 i;
} StructFuncReg;

#define CHOICE_RUN_FUNC_CHANGED(c, val)     \
    if(c->funcOnChanged)                    \
    {                                       \
        c->funcOnChanged(val);              \
    }

/** @}  @}
 */
