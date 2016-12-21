#pragma once


bool FreqMeter_Init(void);

void FreqMeter_Draw(int x, int y);

// Получить значение частоты для вывода в нижней части экрана
float FreqMeter_GetFreq(void);

// Функция вызывается из FPGA
void FreqMeter_Update(uint16 flag);
