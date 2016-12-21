#pragma once


#include "Settings/SettingsTypes.h"


void CursorsUpdate(void);           // Вызываем эту функцию для каждого измерения, чтобы обновить положие курсоров, если они должны обновляться автоматически.
bool IsRegSetActiveOnCursors(void); // Возвращает true, если вращение ручки УСТАНОВКА будет перемещать курсоры/курсоры
