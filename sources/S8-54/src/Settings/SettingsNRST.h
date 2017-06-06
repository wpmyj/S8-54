#pragma once


#define NRST_RSHIFT_ADD(ch, range, mode)    (set.nrst_RShiftAdd[ch][range][mode])
#define NRST_RSHIFT_ADD_A(range, mode)      (NRST_RSHIFT_ADD(A, range, mode))
#define NRST_RSHIFT_ADD_B(range, mode)      (NRST_RSHIFT_ADD(B, range, mode))

#define NRST_STRETCH_ADC_TYPE               (set.nrst_StretchADCtype)
#define NRST_STRETCH_ADC_TYPE_IS_DISABLE    (NRST_STRETCH_ADC_TYPE == StretchADC_Disable)
#define NRST_STRETCH_ADC_TYPE_IS_HAND       (NRST_STRETCH_ADC_TYPE == StretchADC_Hand)

#define NRST_STRETCH_ADC(ch, type)          (set.nrst_StretchADC[ch][type])
#define NRST_STRETCH_ADC_A(type)            (NRST_STRETCH_ADC(A, type))
#define NRST_STRETCH_ADC_B(type)            (NRST_STRETCH_ADC(B, type))

#define NRST_BALANCE_ADC_TYPE               (set.nrst_BalanceADCtype)
#define NRST_BALANCE_ADC_TYPE_IS_HAND       (NRST_BALANCE_ADC_TYPE == BalanceADC_Hand)

#define NRST_BALANCE_ADC(ch)                (set.nrst_BalanceADC[ch])
#define NRST_BALANCE_ADC_A                  (NRST_BALANCE_ADC(A))
#define NRST_BALANCE_ADC_B                  (NRST_BALANCE_ADC(B))

#define NRST_CORRECTION_TIME                (set.nrst_CorrectionTime)

#define NRST_ADD_STRETCH_20mV(ch)           (set.nrst_AddStretch20mV[ch])
#define NRST_ADD_STRETCH_20mV_A             (NRST_ADD_STRETCH_20mV(A))
#define NRST_ADD_STRETCH_20mV_B             (NRST_ADD_STRETCH_20mV(B))

#define NRST_ADD_STRETCH_50mV(ch)           (set.nrst_AddStretch50mV[ch])
#define NRST_ADD_STRETCH_50mV_A             (NRST_ADD_STRETCH_50mV(A))
#define NRST_ADD_STRETCH_50mV_B             (NRST_ADD_STRETCH_50mV(B))

#define NRST_ADD_STRETCH_100mV(ch)          (set.nrst_AddStretch100mV[ch])
#define NRST_ADD_STRETCH_100mV_A            (NRST_ADD_STRETCH_100mV(A))
#define NRST_ADD_STRETCH_100mV_B            (NRST_ADD_STRETCH_100mV(B))

#define NRST_ADD_STRETCH_2V(ch)             (set.nrst_AddStretch2V[ch])
#define NRST_ADD_STRETCH_2V_A               (NRST_ADD_STRETCH_2V(A))
#define NRST_ADD_STRETCH_2V_B               (NRST_ADD_STRETCH_2V(B))

#define NRST_NUM_SMOOTH_FOR_RAND            (set.nrst_NumSmoothForRand)

#define NRST_NUM_AVE_FOR_RAND               (set.nrst_NumAveForRand)
