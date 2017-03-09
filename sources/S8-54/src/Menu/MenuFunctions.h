#pragma once

#include "MenuItems.h"
#include "Panel/Controls.h"


int Choice_NumSubItems(Choice *choice);                         // Возвращает количество вариантов выбора в элементе по адресу choice.

bool            ItemIsAcitve(void *item);                       // Вовзращает true, если элемент меню item является ативным, т.е. может быть нажат.
int             NumItemsInPage(const Page * const page);        // Возвращает количество элементов в странице по адресу page.
int             NumCurrentSubPage(Page *page);                  // Возвращает номер текущей подстранцы элемента по адресу page.
int             NumSubPages(const Page *page);                  // Dозвращает число подстраниц в странице по адресу page.
TypeItem        TypeMenuItem(const void *address);              // Возвращает тип элемента меню по адресу address.
TypeItem        TypeOpenedItem(void);                           // Возвращает тип открытого элемента меню.
void*           OpenedItem(void);                               // Возвращает адрес открытого элемента меню.
void*           CurrentItem(void);                              // Возвращает адрес текущего элемента меню (текущим, как правило, является элемент, кнопка которого была нажата последней.
void*           Item                                            // Возвращает адрес элемента меню заданной страницы.
                     (const Page *page,                         // страница, элемент которой нужно узнать
                     int numElement                             // порядковый номер элемента, который нужно узнать
                     );
bool            CurrentItemIsOpened(NamePage namePage);         // Возвращает true, если текущий элемент страницы с именем namePage открыт.
int8            PosCurrentItem(const Page *page);               // Возвращает позицию текущего элемента странцы page.
void            SetCurrentItem                                  // Сделать/разделать текущим пункт страницы.
                                (const void *item,              // адрес элемента, активность которого нужно установить/снять
                                bool active                     // true - сделать элемент текущим, false - расделать элемент текущим. В этом случае текущим становится заголовок страницы
                                );
Page*           Keeper(const void *item);                       // Возвращает адрес элемента, которому принадлежит элемент по адресу item.
NamePage        GetNamePage(const Page *page);                  // Возвращает имя страницы page.
NamePage        GetNameOpenedPage(void);
void            OpenPageAndSetItCurrent(NamePage namePage);
bool            ChangeOpenedItem(void *item, int delta);
void            ChangeItem(void *item, int delta);              // Уменьшает или увеличивает значение Governor, GovernorColor или Choice по адресу item в зависимости от знака delta
const char*     NameCurrentSubItem(Choice *choice);             // Возвращает имя текущего варианта выбора элемента choice, как оно записано в исходном коде программы.
const char*     NameSubItem(Choice *choice, int i);             // Возвращает имя варианта выбора элемента choice в позиции i как оно записано в исходном коде программы.
const char*     NameNextSubItem(Choice *choice);                // Возвращает имя следующего варианта выбора элемента choice, как оно записано в исходном коде программы.
const char*     NamePrevSubItem(Choice *choice);
int             HeightOpenedItem(void *item);                   // Возвращает высоту в пикселях открытого элемента Choice или NamePage по адресу item.
const char*     TitleItem(void *item);                          // Возвращает название элемента по адресу item, как оно выглядит на дисплее прибора.
int             PosItemOnTop(Page *page);                       // Возвращает позицию первого элемента страницы по адресу page на экране. Если текущая подстраница 0, это будет 0, если текущая подстраница 1, это будет 5 и т.д.
bool            IsFunctionalButton(PanelButton button);         // Возвращает true, если button - функциональная клавиша [1..5].
void            CloseOpenedItem(void);                          // Закрыть открытый элемент меню.
void            OpenItem(const void *item, bool open);          // Открыть/закрыть элемент меню по адрему item.
bool            ItemIsOpened(void *item);                       // Возвращает true, если элемент меню по адрему item открыт.
void            ChangeSubPage(const Page *page, int delta);
void            PrintStatePage(Page *page);
void            ShortPressOnPageItem(Page *page, int numItem);  // Вызывает функцию короткого нажатия кнопки над итемом numItem страницы page;
Page*           PagePointerFromName(NamePage namePage);
bool            IsPageSB(const void *item);
SmallButton*    SmallButonFromPage(Page *page, int numButton);
void            CallFuncOnPressButton(void *button);
void            CallFuncOnDrawButton(Button *button, int x, int y);
