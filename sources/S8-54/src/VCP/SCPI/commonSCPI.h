#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup SCPI
 *  @{
 */

void Process_IDN(uint8 *buffer);

void Process_RUN(uint8 *buffer);

void Process_STOP(uint8 *buffer);

void Process_RESET(uint8 *buffer);

void Process_AUTOSCALE(uint8 *buffer);

void Process_REQUEST(uint8 *buffer);

/** @}
 */
