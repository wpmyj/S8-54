#pragma once 
#include "defines.h"
#include "Panel/Controls.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void    Panel_Init(void);
void    Panel_Update(void);
void    Panel_EnableLEDRegSet(bool enable);
void    Panel_EnableLEDChannelA(bool enable);       // Включить/выключить светодиод КАНАЛ 1.
void    Panel_EnableLEDChannelB(bool enable);       // Включить/выключить светодиод КАНАЛ 2.
void    Panel_EnableLEDTrig(bool enable);           // Включить/выключить светодиод СИНХР
void    Panel_TransmitData(uint16 data);            // Передать даннные в мк панели управления.
void    Panel_Disable(void);                        // В отлюченном режиме панель лишь обновляет состояние переменной pressedButton, не выполняя больше никаких действий.
void    Panel_Enable(void);
PanelButton   Panel_WaitPressingButton(void);       // Ожидать нажатие клавиши.
bool    Panel_ProcessingCommandFromPIC(uint16 command);
uint16  Panel_NextData(void);
