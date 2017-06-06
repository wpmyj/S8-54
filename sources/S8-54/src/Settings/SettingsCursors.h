#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsCursors Settings Cursors
 *  @{
 */

#define CURSORS_SHOW_FREQ           (set.curs_ShowFreq)
#define CURS_MOVEMENT               (set.curs_Movement)
#define CURS_MOVEMENT_IS_PERCENTS   (CURS_MOVEMENT == CursMovement_Percents)

#define CURS_SOURCE                 (set.curs_Source)
#define CURS_SOURCE_A               (CURS_SOURCE == A)
#define CURS_SOURCE_B               (CURS_SOURCE == B)

#define CURsU_CNTRL_CH(ch)          (set.curs_CntrlU[ch])
#define CURsU_CNTRL                 (CURsU_CNTRL_CH(CURS_SOURCE))
#define CURsU_CNTRL_1               (CURsU_CNTRL == CursCntrl_1)
#define CURsU_CNTRL_2               (CURsU_CNTRL == CursCntrl_2)
#define CURsU_CNTRL_1_2             (CURsU_CNTRL == CursCntrl_1_2)
#define CURsU_DISABLED              (CURsU_CNTRL == CursCntrl_Disable)
#define CURsU_ENABLED               (!CURsU_DISABLED)

#define CURsT_CNTRL_CH(ch)          (set.curs_CntrlT[ch])
#define CURsT_CNTRL                 (CURsT_CNTRL_CH(CURS_SOURCE))
#define CURsT_CNTRL_1               (CURsT_CNTRL == CursCntrl_1)
#define CURsT_CNTRL_2               (CURsT_CNTRL == CursCntrl_2)
#define CURsT_CNTRL_1_2             (CURsT_CNTRL == CursCntrl_1_2)
#define CURsT_DISABLED              (CURsT_CNTRL == CursCntrl_Disable)
#define CURsT_ENABLED               (!CURsT_DISABLED)

#define CURsU_POS(ch, num)          (set.curs_PosCurU[ch][num])
#define CURsT_POS(ch, num)          (set.curs_PosCurT[ch][num])

#define dUperc(ch)                  (set.curs_DeltaU100percents[ch])
#define dTperc(ch)                  (set.curs_DeltaT100percents[ch])

#define CURS_ACTIVE                 (set.curs_Active)
#define CURS_ACTIVE_U               (CURS_ACTIVE == CursActive_U)
#define CURS_ACTIVE_T               (CURS_ACTIVE == CursActive_T)

#define CURS_LOOK_MODE(ch)          (set.curs_LookMode[ch])
#define CURS_LOOK_U(ch)             (CURS_LOOK_MODE(ch) == CursLookMode_Voltage)
#define CURS_LOOK_T(ch)             (CURS_LOOK_MODE(ch) == CursLookMode_Time)
#define CURS_LOOK_BOTH(ch)          (CURS_LOOK_MODE(ch) == CursLookMode_Both)

#define CURS_SHOW                   (set.curs_ShowCursors)


 /// Получить позицию курсора напряжения
float sCursors_GetCursPosU(Channel ch, int numCur);
/// Возвращает true,если нужно рисовать курсоры
bool  sCursors_NecessaryDrawCursors(void);
/// Получить строку курсора напряжения
const char* sCursors_GetCursVoltage(Channel source, int numCur, char buffer[20]);
/// Получить строку курсора времени
const char* sCursors_GetCursorTime(Channel source, int numCur, char buffer[20]);
/// Получить строку процентов курсоров напряжения
const char* sCursors_GetCursorPercentsU(Channel source, char buffer[20]);
/// Получить строку процентов курсоров времени
const char* sCursors_GetCursorPercentsT(Channel source, char buffer[20]);

/** @}  @}
 */
