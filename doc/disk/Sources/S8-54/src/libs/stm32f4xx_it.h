#pragma once


#ifdef __cplusplus
extern "C" {
#endif

    
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void EXTI0_IRQHandler(void);
void SPI1_IRQHandler(void);

void OTG_FS_IRQHandler(void);
    
void OTG_HS_IRQHandler(void);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);     // Используется для чтения АЦП рандомизатора

#ifdef __cplusplus
}
#endif
