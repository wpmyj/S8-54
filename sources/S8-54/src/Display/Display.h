#pragma once
#include "DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Tables.h"
#include "Painter.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Cообщения, выводимые на экран
typedef enum
{
    LimitChan1_Volts,               ///< Предел масштаба по напряжениию первого канала.
    LimitChan2_Volts,               ///< Предел масштаба по напряжению второго канала.
    LimitSweep_Time,                ///< Предел масштаба по времени.
    EnabledPeakDet,                 ///< Включён пиковый детектор.
    LimitChan1_RShift,              ///< Предел смещения по напряжению первого канала.
    LimitChan2_RShift,              ///< Предел смещения по напряжению второго канала.
    LimitSweep_Level,               ///< Предел уровня синхронизации.
    LimitSweep_TShift,              ///< Предел смещения по времени.
    TooSmallSweepForPeakDet,        ///< Слишком быстрая развёртка для пикового детектора.
    TooFastScanForRecorder,         ///< Слишком быстрая развёртка для поточечного вывода.
    FileIsSaved,                    ///< Файл сохранён на флешку.
    SignalIsSaved,                  ///< Сигнал сохранён в ППЗ.
    SignalIsDeleted,                ///< Из ППЗУ удалён сигнал.
    MenuDebugEnabled,               ///< Включено меню ОТЛАДКА.
    TimeNotSet,                     ///< Время не установлено.
    SignalNotFound,                 ///< Сигнал не найден (автопоиск).
    SetTPosToLeft,                  ///< Рекомендация.
    NeedRebootDevice,               ///< Требуется перезагрузка.
    ImpossibleEnableMathFunction,   ///< Невозможно включить математическую функцию.
    ImpossibleEnableFFT,            ///< Невозможно включить вычисление БПФ.
    WrongFileSystem,                ///< Невозможно подключить флешку.
    WrongModePeackDet,              ///< Невозможно включить пиковый детектор.
    DisableChannel2,                ///< Требуется выключить канал 2.
    TooLongMemory,                  ///< Сообщение о невозможности включения режима 32к.
    NoPeakDet32k,                   ///< Пиковый детектор не работает при длине памяти 32к.
    NoPeakDet16k,                   ///< Пиковый детектор не работает при длине памяти 16к.
    Warn50Ohms,                     ///< Предупреждение об ограничение входного уровня на 50 Омах.
    WarnNeedForFlashDrive,          ///< Требуется подключенная флешка.
    FirmwareSaved,                  ///< Прошивка сохранена на флешку.
    FullyCompletedOTP,              ///< 
    Warning_Count
} Warning;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @defgroup Display
 *  @brief Работа с дисплеем
 *  @details При поточечном режиме вывода текущие точки берутся из DS_GetData(0), а последний полный сигнал из DS_GetData(1).
 *  @{
 */

class Display
{
public:
    /// Инициализация
    void Init(void);
    /// Эту функцию нужно вызывать каждый кадр - отрисовка
    void Update(void);
    /// Эту функцию нужно вызывать при вращении ручки смещения по вертикали для временной отрисовки вспомогательных маркеров
    void RotateRShift(Channel ch);
    /// Эту функцию нужно вызывать при вращении ручки уровня синхронизации для временной отрисовки вспомогательных маркеров
    void RotateTrigLev(void);

    void EnableTrigLabel(bool enable);

    void SetAddDrawFunction(pFuncVV func);

    pFuncVV GetAddDrawFunction(void);

    void RemoveAddDrawFunction(void);

    void Clear(void);

    void ShiftScreen(int delta);

    void ChangedRShiftMarkers(bool active);

    void AddStringToIndicating(const char *string);

    void OneStringUp(void);

    void OneStringDown(void);

    void SetPauseForConsole(bool pause);

    void SetOrientation(DisplayOrientation orientation);
    /// Вывести сообщения отладочной консоли
    void DrawConsole(void);
    /// @brief Установить функцию и режим отрисовки экрана.
    /// @details Возможны три варианта.
    /// 1. DrawMode_Hand - в этом случае будет вызываться функция func(), определяемая пользователем, с частотой 25 раз в секунду.
    /// Используется в случаях, когда не выполняется главный цикл.
    /// 2. DrawMode_Auto и func == 0 - в этом случае будет выполняться функция Update() в главном цикле.
    /// 3. DrawMode_Auto и func != 0 - в этом случае будет выполняться функция func(), определяемая пользователем, но в теле
    /// главного цикла, будучи вызываемой из Update() вместо Update().
    void SetDrawMode(DrawMode mode, pFuncVV func);

    void ClearFromWarnings(void);

    void ShowWarning(Warning warning);

    // Группа функций для вывода сообщения о занятом приборе
    void FuncOnWaitStart(const char *text, bool eraseBackground);

    void FuncOnWaitStop(void);
};

extern Display display;

/** @}
 */
