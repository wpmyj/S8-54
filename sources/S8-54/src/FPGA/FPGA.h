#pragma once
#include "defines.h"
#include "FPGA/FPGAtypes.h"
#include "Globals.h"
#include "Panel/Controls.h"
#include "Settings/Settings.h"


extern uint16 gPost;
extern int16 gPred;
extern StateWorkFPGA fpgaStateWork;


void FPGA_Init(void);

void FPGA_SetNumSignalsInSec(int numSigInSec);  // Установить количество считываемых сигналов в секунду.

void FPGA_Update(void);

void FPGA_OnPressStartStop(void);               // Запускает цикл сбора информации.

void FPGA_Start(void);                          // Запуск процесса сбора информации.

void FPGA_WriteStartToHardware(void);

void FPGA_Stop(bool pause);                     // Прерывает процесс сбора информации.

void FPGA_Reset(void);                          // Можно делать при изменении каких-то настроек. Например, при изменении числа точек (ПАМЯТЬ-Точки) если не вызвать, то будут артефакты изображения

bool FPGA_IsRunning(void);                      // Возвращает true, если прибор находится не в процессе сбора информации.

void FPGA_ClearData(void);                      // Удаляет данные. Нужно для режима рандомизаотра, где информация каждого цикла не является самостоятельной.

void FPGA_SetNumberMeasuresForGates(int number);// Установить количество измерений, по которым будут рассчитываться ворота в режиме рандомизатора.

void FPGA_SwitchingTrig(void);                  // Принудительно запустить синхронизацию.

void FPGA_TemporaryPause(void);                  // Установить временную паузу после изменения ручек - чтобы смещённый сигнал зафиксировать на некоторое время

void FPGA_FillDataPointer(DataSettings *dp);

void FPGA_FindAndSetTrigLevel(void);            // Найти и установить уровень синхронизации по последнему считанному сигналу
    
void FPGA_LoadSettings(void);                                   // Загрузить настройки в аппаратную часть из глобальной структуры SSettings.

void FPGA_SetModeCouple(Channel ch, ModeCouple modeCoupe);    // Установить режим канала по входу.

void FPGA_SetRange(Channel ch, Range range);                  // Установить масштаб по напряжению.

bool FPGA_RangeIncrease(Channel ch);                          // Увеличить масштаб по напряжению.

bool FPGA_RangeDecrease(Channel ch);                          // Уменьшить масштаб по напряжению.

void FPGA_SetTBase(TBase tBase);                                // Установить масштаб по времени.

void FPGA_TBaseDecrease(void);                                  // Уменьшить масштаб по времени.

void FPGA_TBaseIncrease(void);                                  // Увеличить масштаб по времени.

void FPGA_SetRShift(Channel ch, uint16 rShift);               // Установить относительное смещение по напряжению.

void FPGA_SetTShift(int tShift);                                // Установить относительное смещение по времени.

void FPGA_ChangePostValue(int delta);

void FPGA_SetDeltaTShift(int16 shift);                          // Установить добавочное смещение по времени для режима рандомизатора. В каждой развёртке это смещение должно быть разное.

void FPGA_SetPeackDetMode(PeackDetMode peackDetMode);           // Включить/выключить режим пикового детектора.

void FPGA_SetCalibratorMode(CalibratorMode calibratorMode);     // Включить/выключить калибратор.

void FPGA_EnableRecorderMode(bool enable);

void FPGA_SetTrigLev(TrigSource ch, uint16 trigLev);          // Установить относительный уровень синхронизации.

void FPGA_SetTrigSource(TrigSource trigSource);                 // Установить источник синхронизации.

void FPGA_SetTrigPolarity(TrigPolarity polarity);               // Установить полярность синхронизации.

void FPGA_SetTrigInput(TrigInput trigInput);                    // Установить режим входа синхронизации.

void FPGA_SetResistance(Channel ch, Resistance resistance);

void FPGA_SetBandwidth(Channel ch);

void FPGA_ReadPoint(void);                                      // Функция чтения точки при поточечном выводе. Вызывается из внешнего прерывания.

const char* FPGA_GetTShiftString(int16 tShiftRel, char buffer[20]); // Возвращает установленное смещение по времени в текстовом виде, пригодном для вывода на экран.

typedef enum
{
    RecordFPGA,
    RecordAnalog,
    RecordDAC
} TypeRecord;

#define CS1 ((uint16*)0)
#define CS2 ((uint16*)1)
#define CS3 ((uint16*)2)
#define CS4 ((uint16*)3)

#define dacRShiftA ((uint16*)0)
#define dacRShiftB ((uint16*)1)
#define dacTrigLev ((uint16*)2)


void FPGA_Write(TypeRecord type, uint16 *address, uint data, bool restart);
