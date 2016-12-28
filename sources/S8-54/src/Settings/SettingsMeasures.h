#pragma once


#define MEAS_COMPRESS_GRID  (set.measures.modeViewSignals == ModeViewSignals_Compress)  // —жимать ли сетку при выводе измерений
#define MEASURE(n)          (set.measures.measures[n])
#define SHOW_MEASURES       (set.measures.show)
#define MARKED_MEAS         (set.measures.marked)


#define NUM_MEASURES        (set.measures.number)
#define NUM_MEASURES_1_5    (NUM_MEASURES == MN_1_5)
#define NUM_MEASURES_2_5    (NUM_MEASURES == MN_2_5)
#define NUM_MEASURES_3_5    (NUM_MEASURES == MN_3_5)
#define NUM_MEASURES_6_1    (NUM_MEASURES == MN_6_1)
#define NUM_MEASURES_6_2    (NUM_MEASURES == MN_6_2)

#define SOURCE_MEASURE      (set.measures.source)
#define SOURCE_MEASURE_A    (SOURCE_MEASURE == A)
#define SOURCE_MEASURE_B    (SOURCE_MEASURE == B)
#define SOURCE_MEASURE_A_B  (SOURCE_MEASURE == A_B)

#define POS_MEAS_CUR_U(n)   (set.measures.posCurU[n])
#define POS_MEAS_CUR_U_0    (POS_MEAS_CUR_U(0))
#define POS_MEAS_CUR_U_1    (POS_MEAS_CUR_U(1))

#define POS_MEAS_CUR_T(n)   (set.measures.posCurT[n])
#define POS_MEAS_CUR_T_0    (POS_MEAS_CUR_T(0))
#define POS_MEAS_CUR_T_1    (POS_MEAS_CUR_T(1))
