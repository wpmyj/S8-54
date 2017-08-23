#pragma once
#include "Settings/Settings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsService Settings Service
 *  @{
 */

#define RECORDER_MODE                   (set.serv_Recorder)

#define LANG                            (set.com_Lang)
#define LANG_RU                         (LANG == Russian)
#define LANG_EN                         (LANG == English)

#define TIME_SHOW_LEVELS                (set.disp_TimeShowLevels)

#define FREQ_METER_ENABLED              (set.freq_Enable)
#define FREQ_METER_TIMECOUNTING         (set.freq_TimeCounting)
#define FREQ_METER_FREQ_CLC             (set.freq_FreqClc)
#define FREQ_METER_NUM_PERIODS          (set.freq_NumberPeriods)

#define CALIBRATOR_MODE                 (set.serv_CalibratorMode)

#define SOUND_ENABLED                   (set.serv_SoundEnable)
#define SOUND_VOLUME                    (set.serv_SoundVolume)

#define COLOR_SCHEME                    (set.serv_ColorScheme)
#define COLOR_SCHEME_IS_WHITE_LETTERS   (COLOR_SCHEME == ColorScheme_WhiteLetters)

#define REC_PLACE_OF_SAVING             (set.rec_PlaceOfSaving)
#define REC_NUM_CURSOR                  (set.rec_NumCursor)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float   sService_MathGetFFTmaxDBabs(void);

/** @}  @}
 */
