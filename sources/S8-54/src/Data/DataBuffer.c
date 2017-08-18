#include "DataBuffer.h"


static uint8 buffer[4][16 * 1024] __attribute__ ((section("CCM_DATA")));

uint8 *dataIN[NumChannels] = {buffer[0], buffer[1]};

uint8 *dataOUT[NumChannels] = {buffer[2], buffer[3]};
