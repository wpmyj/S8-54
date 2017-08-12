#pragma once
#include "defines.h"
#include "FPGA/FPGAtypes.h"
#include "Globals.h"
#include "Panel/Controls.h"
#include "Settings/Settings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @defgroup FPGA
 *  @brief Работа с альтерой
 *  @{
 */

extern uint16 gPost;
extern int16 gPred;
extern StateWorkFPGA fpgaStateWork;


/// Инициализация
void FPGA_Init(void);
/// Установить количество считываемых сигналов в секунду
void FPGA_SetENumSignalsInSec(int numSigInSec);
/// Функция должна вызываться каждый кадр
void FPGA_Update(void);
/// Запускает цикл сбора информации
void FPGA_OnPressStartStop(void);
/// Запуск процесса сбора информации
void FPGA_Start(void);

void FPGA_WriteStartToHardware(void);
/// Прерывает процесс сбора информации
void FPGA_Stop(bool pause);
/// Можно делать при изменении каких-то настроек. Например, при изменении числа точек (ПАМЯТЬ-Точки) если не вызвать, то будут артефакты изображения
void FPGA_Reset(void);
/// Возвращает true, если прибор находится не в процессе сбора информации
bool FPGA_IsRunning(void);
/// Удаляет данные. Нужно для режима рандомизаотра, где информация каждого цикла не является самостоятельной
void FPGA_ClearData(void);
/// Установить количество измерений, по которым будут рассчитываться ворота в режиме рандомизатора
void FPGA_SetNumberMeasuresForGates(int number);
/// Принудительно запустить синхронизацию
void FPGA_SwitchingTrig(void);
/// Установить временную паузу после изменения ручек - чтобы смещённый сигнал зафиксировать на некоторое время
void FPGA_TemporaryPause(void);

/// Найти и установить уровень синхронизации по последнему считанному сигналу
void FPGA_FindAndSetTrigLevel(void);
/// Загрузить настройки в аппаратную часть из глобальной структуры SSettings
void FPGA_LoadSettings(void);
/// Установить режим канала по входу
void FPGA_SetModeCouple(Channel ch, ModeCouple modeCoupe);
/// Установить масштаб по напряжению
void FPGA_SetRange(Channel ch, Range range);
/// Увеличить масштаб по напряжению
bool FPGA_RangeIncrease(Channel ch);
/// Уменьшить масштаб по напряжению
bool FPGA_RangeDecrease(Channel ch);
/// Установить масштаб по времени
void FPGA_SetTBase(TBase tBase);
/// Уменьшить масштаб по времени
void FPGA_TBaseDecrease(void);
/// Увеличить масштаб по времени
void FPGA_TBaseIncrease(void);
/// Установить относительное смещение по напряжению
void FPGA_SetRShift(Channel ch, uint16 rShift);
/// Установить относительное смещение по времени
void FPGA_SetTShift(int tShift);

void FPGA_ChangePostValue(int delta);
/// Установить добавочное смещение по времени для режима рандомизатора. В каждой развёртке это смещение должно быть разное
void FPGA_SetDeltaTShift(int16 shift);
/// Включить/выключить режим пикового детектора
void FPGA_SetPeackDetMode(PeackDetMode peackDetMode);
/// Включить/выключить калибратор.
void FPGA_SetCalibratorMode(CalibratorMode calibratorMode);

void FPGA_EnableRecorderMode(bool enable);
/// Установить относительный уровень синхронизации
void FPGA_SetTrigLev(TrigSource ch, uint16 trigLev);
/// Установить источник синхронизации
void FPGA_SetTrigSource(TrigSource trigSource);
/// Установить полярность синхронизации
void FPGA_SetTrigPolarity(TrigPolarity polarity);
/// Установить режим входа синхронизации
void FPGA_SetTrigInput(TrigInput trigInput);

void FPGA_SetResistance(Channel ch, Resistance resistance);

void FPGA_SetTPos(TPos tPos);

void FPGA_SetBandwidth(Channel ch);
/// Функция чтения точки при поточечном выводе. Вызывается из внешнего прерывания
void FPGA_ReadPoint(void);
/// Возвращает установленное смещение по времени в текстовом виде, пригодном для вывода на экран
const char *FPGA_GetTShiftString(int16 tShiftRel, char buffer[20]);

typedef enum
{
    RecordFPGA,
    RecordAnalog,
    RecordDAC
} TypeRecord;

#define CS1 ((uint16 *)0)
#define CS2 ((uint16 *)1)
#define CS3 ((uint16 *)2)
#define CS4 ((uint16 *)3)

#define dacRShiftA ((uint16 *)0)
#define dacRShiftB ((uint16 *)1)
#define dacTrigLev ((uint16 *)2)


void FPGA_Write(TypeRecord type, uint16 *address, uint data, bool restart);

/** @}
 */
