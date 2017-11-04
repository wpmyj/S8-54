#pragma once
#include "MenuItems.h"
#include "Panel/Controls.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Menu
 *  @{
 *  @defgroup MenuFunctions Menu Functions
 *  @brief Вспомогательные функции для работы с меню
 *  @{
 */

/// Вовзращает true, если элемент меню item является ативным, т.е. может быть нажат
bool ItemIsAcitve(const void *item);
/// Возвращает тип элемента меню по адресу address
TypeItem TypeMenuItem(const void *address);
/// Возвращает тип открытого элемента меню
TypeItem TypeOpenedItem(void);
/// Возвращает адрес открытого элемента меню
void *OpenedItem(void);
/// Возвращает адрес текущего элемента меню (текущим, как правило, является элемент, кнопка которого была нажата последней
void *CurrentItem(void);
/// Возвращает true, если текущий элемент страницы с именем namePage открыт
bool CurrentItemIsOpened(NamePage namePage);
/// Возвращает позицию текущего элемента странцы page
int8 PosCurrentItem(const Page *page);
/// Сделать/разделать текущим пункт страницы
void SetCurrentItem(const void *item,               ///< адрес элемента, активность которого нужно установить/снять
                    bool active                     ///< true - сделать элемент текущим, false - расделать элемент текущим. В этом случае текущим становится заголовок страницы
                    );
/// Возвращает адрес элемента, которому принадлежит элемент по адресу item
Page *Keeper(const void *item);

NamePage GetNameOpenedPage(void);

bool ChangeOpenedItem(void *item, int delta);
/// Уменьшает или увеличивает значение Governor, GovernorColor или Choice по адресу item в зависимости от знака delta
void ChangeItem(void *item, int delta);
/// Возвращает имя варианта выбора элемента choice в позиции i как оно записано в исходном коде программы
const char *NameSubItem(Choice *choice, int i);

/// Возвращает высоту в пикселях открытого элемента Choice или NamePage по адресу item
int HeightOpenedItem(void *item);
/// Возвращает название элемента по адресу item, как оно выглядит на дисплее прибора
const char *TitleItem(void *item);
/// Возвращает позицию первого элемента страницы по адресу page на экране. Если текущая подстраница 0, это будет 0, если текущая подстраница 1, это будет 5 и т.д.
int PosItemOnTop(Page *page);
/// Возвращает true, если button - функциональная клавиша [1..5]
bool IsFunctionalButton(PanelButton button);
/// Закрыть открытый элемент меню
void CloseOpenedItem(void);
/// Открыть/закрыть элемент меню по адрему item
void OpenItem(const void *item, bool open);
/// Возвращает true, если элемент меню по адрему item открыт
bool ItemIsOpened(const void *item);

void ChangeSubPage(const Page *page, int delta);
/// Вызывает функцию короткого нажатия кнопки над итемом numItem страницы page
void ShortPressOnPageItem(Page *page, int numItem);

Page *PagePointerFromName(NamePage namePage);

bool IsPageSB(const void *item);

void CallFuncOnPressButton(void *button);

/** @}
 */
