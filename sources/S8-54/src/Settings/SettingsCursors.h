#pragma once


float       sCursors_GetCursPosU(Channel ch, int numCur);                         // Получить позицию курсора напряжения.
bool        sCursors_NecessaryDrawCursors(void);                                    // Возвращает true,если нужно рисовать курсоры.
const char* sCursors_GetCursVoltage(Channel source, int numCur, char buffer[20]);   // Получить строку курсора напряжения.
const char* sCursors_GetCursorTime(Channel source, int numCur, char buffer[20]);    // Получить строку курсора времени.
const char* sCursors_GetCursorPercentsU(Channel source, char buffer[20]);           // Получить строку процентов курсоров напряжения.
const char* sCursors_GetCursorPercentsT(Channel source, char buffer[20]);           // Получить строку процентов курсоров времени.
