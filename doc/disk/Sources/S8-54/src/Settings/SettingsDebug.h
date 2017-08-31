#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsDegug Settings Debug
 *  @{
 */

#define CONSOLE_NUM_STRINGS     (set.dbg_NumStrings)
#define CONSOLE_SIZE_FONT       (set.dbg_SizeFont ? 8 : 5)
/// Если truе, то включён режим остновки консоли, в котором нажатие ПУСК/СТОП приостанавливает вывод в консоль.
#define MODE_PAUSE_CONSOLE      (set.dbg_ModePauseConsole)
#define NUM_MEASURES_FOR_GATES  (set.dbg_NumMeasuresForGates)
#define TIME_COMPENSATION       (set.dbg_TimeCompensation)
#define SHOW_RAND_INFO          (set.dbg_ShowRandInfo)
#define SHOW_RAND_STAT          (set.dbg_ShowRandStat)
#define MODE_EMS                (set.dbg_ModeEMS)
#define SHOW_STAT               (set.dbg_ShowStats)
#define PRETRIGGERED            (set.dbg_Pretriggered)
#define BANDWIDTH_DEBUG(ch)     (set.dbg_Bandwidth[ch])

#define DBG_SHOW_ALL            (set.dbg_ShowAll)
#define DBG_SHOW_FLAG           (set.dbg_ShowFlag)


float   GetStretchADC(Channel ch);

void    SetStretchADC(Channel ch, float kStretch);

/** @}  @}
 */
