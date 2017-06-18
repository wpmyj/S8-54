#include "DataBuffer.h"


static uint8 buffer[4][16 * 1024] __attribute__ ((section("CCM_DATA")));

uint8 *inA = buffer[0];
uint8 *inB = buffer[1];

uint8 *outA = buffer[2];
uint8 *outB = buffer[3];
