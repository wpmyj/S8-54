#pragma once


void TimerZ80_Init(void);

// Start the clock generator
void TimerZ80_StartTicks(void);

// Wait n cycles after the previous call Timer_StartTicks() or Timer_WaitForNTacts()
void TimerZ80_WaitForNTacts(int n);
