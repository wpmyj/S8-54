#pragma once
#include "Settings/DataSettings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DS              (Data_DS())
#define P_DS            (Data_pDS())
#define DATAA           (Data_ChDS(A))
#define DATAB           (Data_ChDS(B))
#define P_DATAA         (Data_pChDS(A))
#define P_DATAB         (Data_pChDS(B))

#define DS_MEM_INT      (Data_DSInt())
#define CHA_MEM_INT     (Data_ChInt(A))
#define CHB_MEM_INT     (Data_ChInt(B))

#define DS_MEM_LAST     (Data_DSLast())
#define P_DS_MEM_LAST   (Data_pDSLast())
#define CHA_MEM_LAST    (Data_ChLast(A))
#define CHB_MEM_LAST    (Data_ChLast(B))


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Data_GetFromIntMemory(void);       // Считать из ППЗУ информацию о настройках и указатели на данные

DataSettings*   Data_DS(void);          // Указатель на настройки рисуемых сигналов
DataSettings**  Data_pDS(void);         // Указатель на указатель на настройки рисуемых сигналов    TODO после рефакторинга этого не должно быть
uint8*  Data_ChDS(Channel ch);          // Указатель на данные канала рисуемого сигнала
uint8** Data_pChDS(Channel ch);         // Указатель на указатель на рисуемые данные TODO отрефакторить, чтобы не было

DataSettings*   Data_DSInt(void);       // Возвращает указатель на настройки сигнала из ППЗУ
DataSettings**  Data_pDSInt(void);      // Возвращает указатель на указатель на настройки сигнала из ППЗУ   TODO Отрефакторить - этой функции не должно быть
uint8*  Data_ChInt(Channel ch);         // Получить указатель на канал данных из ППЗУ
uint8** Data_pChInt(Channel ch);        // Возвращает указатель на указатель на канал данных на ППЗУ TODO Этой функции не должно остаться после рефакторинга

DataSettings*   Data_DSLast(void);
DataSettings**  Data_pDSLast(void);
uint8*  Data_ChLast(Channel ch);
uint8** Data_pChLast(Channel ch);

void Data_GetAverageFromDataStorage(void);
