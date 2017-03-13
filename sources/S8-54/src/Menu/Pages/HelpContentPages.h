#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    TypePage_Content,       // На этой странице расположены ссылки на другие страницы
    TypePage_Description    // На этой странице нет ссылок, только описание
} TypePageHelp;


#define MAX_PAGES 3


typedef struct  
{
    TypePageHelp    type;
    void*           parent;             // Адрес родительской страницы
    pFuncBV         funcNotUsed;        // Оставлено для соместимости с типом Page - нужно для отрисовки.   // WARN избавиться от этого рудимента
    char*           titleHint[4];       // Название страницы на русском и английском языке, а затем содержимое на русском и английском (для случая TypePage_Description)
    void*           pages[MAX_PAGES];   // Массив содержит адреса ссылаемых страниц в случае TypePage_Content
} PageHelp;

extern const PageHelp helpMenu;

static const PageHelp helpMenuCommon =
{
    TypePage_Description,
    (void*)&helpMenu, 0,
    {
        "Общее описание принципов меню",
        "General description of the principles of the menu",
        "Кнопки на панели управления имеют два типа нажатия - короткое, длительностью менее 0.5 сек и длинное, длительностьи более 0.5 сек. "
        "Меню представляет собой древовидную структуру элементов. "
        "Главная страница меню открывается коротким либо длинным нажатием кнопки МЕНЮ. "
        "Ко всем элементам главной страницы имеется непосрдественный доступ с помощью нажатия одной из кнопок на панели управления:\n"
        "\"ДИСПЛЕЙ\" - ДИСПЛЕЙ\n"
        "\"КАНАЛ 1\" - КАНАЛ 1\n"
        "\"КАНАЛ 2\" - КАНАЛ 2\n"
        "\"СИНХР.\" - СИНХР\n"
        "\"РАЗВEРТКА\" - РАЗВ\n"
        "\"КУРСОРЫ\" - КУРСОРЫ\n"
        "\"ПАМЯТЬ\" - ПАМЯТЬ\n"
        "\"ИЗМЕРЕНИЯ\" - ИЗМЕР\n"
        "\"СЕРВИС\" - СЕРВИС\n"
        "\"ПОМОЩЬ\" - ПОМОЩЬ\n"
        "В каждом элементе главной страницы собраны настройки соответствующей категории"
        ,
        ""
    }
};

static const PageHelp helpMenuControls =
{
    TypePage_Description,
    (void*)&helpMenu, 0,
    {
        "Описание органов управлениея",
        "Description of the controls",
        "",
        ""
    }
};

extern const PageHelp helpMain;

static const PageHelp helpSCPI =
{
    TypePage_Description,
    (void*)&helpMain, 0,
    {
        "Работа с SCPI",
        "Working with SCPI",
        "",
        ""
    }
};

const PageHelp helpMenu =
{
    TypePage_Content,
    (void*)&helpMain, 0,
    {
        "Работа с меню",
        "Working with menus"
    },
    {
        (void*)&helpMenuCommon,
        (void*)&helpMenuControls
    }
};

const PageHelp helpMain =
{
    TypePage_Content,
    0, 0,
    {
        "ПОМОЩЬ",
        "HELP"
    },
    {
        (void*)&helpMenu,
        (void*)&helpSCPI
    }
};
