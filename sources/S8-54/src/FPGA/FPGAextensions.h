#pragma once

#include "defines.h"
#include "Settings/SettingsTypes.h"


void  FPGA_ProcedureCalibration(void);   // Запуск функции калибровки
void  FPGA_BalanceChannel(Channel ch);   // Провести процедуру балансировки
void  FPGA_AutoFind(void);               // Запуск процесса поиска сигнала
bool  FreqMeter_Init(void);
void  FreqMeter_Draw(int x, int y);
float FreqMeter_GetFreq(void);          // Получить значение частоты для вывода в нижней части экрана
void  FreqMeter_Update(uint16 flag);     // Функция вызывается из FPGA
