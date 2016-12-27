#pragma once


#define CURS_SOURCE         (set.cursors.source)
#define CURS_SOURCE_A       (CURS_SOURCE == A)
#define CURS_SOURCE_B       (CURS_SOURCE == B)

#define CURsU_CNTRL_CH(ch)  (set.cursors.cntrlU[ch])
#define CURsU_CNTRL         (CURsU_CNTRL_CH(CURS_SOURCE))
#define CURsU_CNTRL_1       (CURsU_CNTRL == CursCntrl_1)
#define CURsU_CNTRL_2       (CURsU_CNTRL == CursCntrl_2)
#define CURsU_CNTRL_1_2     (CURsU_CNTRL == CursCntrl_1_2)
#define CURsU_DISABLED      (CURsU_CNTRL == CursCntrl_Disable)
#define CURsU_ENABLED       (!CURsU_DISABLED)

#define CURsT_CNTRL_CH(ch)  (set.cursors.cntrlT[ch])
#define CURsT_CNTRL         (CURsT_CNTRL_CH(CURS_SOURCE))
#define CURsT_CNTRL_1       (CURsT_CNTRL == CursCntrl_1)
#define CURsT_CNTRL_2       (CURsT_CNTRL == CursCntrl_2)
#define CURsT_CNTRL_1_2     (CURsT_CNTRL == CursCntrl_1_2)
#define CURsT_DISABLED      (CURsT_CNTRL == CursCntrl_Disable)
#define CURsT_ENABLED       (!CURsT_DISABLED)

#define CURsU_POS(ch, num)  (set.cursors.posCurU[ch][num])


float       sCursors_GetCursPosU(Channel ch, int numCur);                         // Получить позицию курсора напряжения.
bool        sCursors_NecessaryDrawCursors(void);                                    // Возвращает true,если нужно рисовать курсоры.
const char* sCursors_GetCursVoltage(Channel source, int numCur, char buffer[20]);   // Получить строку курсора напряжения.
const char* sCursors_GetCursorTime(Channel source, int numCur, char buffer[20]);    // Получить строку курсора времени.
const char* sCursors_GetCursorPercentsU(Channel source, char buffer[20]);           // Получить строку процентов курсоров напряжения.
const char* sCursors_GetCursorPercentsT(Channel source, char buffer[20]);           // Получить строку процентов курсоров времени.
