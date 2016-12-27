#pragma once


#define CURS_SOURCE         (set.cursors.source)
#define CURS_SOURCE_A       (set.cursors.source == A)
#define CURS_SOURCE_B       (set.cursors.source == B)

#define CNTRL_CURSU_CH(ch)  (set.cursors.cntrlU[ch])
#define CNTRL_CURSU         (CNTRL_CURSU_CH(CURS_SOURCE))
#define CNTRL_CURSU_1       (CNTRL_CURSU == CursCntrl_1)
#define CNTRL_CURSU_2       (CNTRL_CURSU == CursCntrl_2)
#define CNTRL_CURSU_1_2     (CNTRL_CURSU == CursCntrl_1_2)
#define CURSU_DISABLED      (CNTRL_CURSU == CursCntrl_Disable)
#define CURSU_ENABLED       (!CURSU_DISABLED)

#define CNTRL_CURST_CH(ch)  (set.cursors.cntrlT[ch])
#define CNTRL_CURST         (CNTRL_CURST_CH(CURS_SOURCE))
#define CNTRL_CURST_1       (CNTRL_CURST == CursCntrl_1)
#define CNTRL_CURST_2       (CNTRL_CURST == CursCntrl_2)
#define CNTRL_CURST_1_2     (CNTRL_CURST == CursCntrl_1_2)
#define CURST_DISABLED      (CNTRL_CURST == CursCntrl_Disable)
#define CURST_ENABLED       (!CURST_DISABLED)


float       sCursors_GetCursPosU(Channel ch, int numCur);                         // Получить позицию курсора напряжения.
bool        sCursors_NecessaryDrawCursors(void);                                    // Возвращает true,если нужно рисовать курсоры.
const char* sCursors_GetCursVoltage(Channel source, int numCur, char buffer[20]);   // Получить строку курсора напряжения.
const char* sCursors_GetCursorTime(Channel source, int numCur, char buffer[20]);    // Получить строку курсора времени.
const char* sCursors_GetCursorPercentsU(Channel source, char buffer[20]);           // Получить строку процентов курсоров напряжения.
const char* sCursors_GetCursorPercentsT(Channel source, char buffer[20]);           // Получить строку процентов курсоров времени.
