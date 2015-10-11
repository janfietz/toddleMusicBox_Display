/**
 * @file    board_drivers.h
 * @brief
 *
 * @{
 */

#ifndef _BOARD_DRIVERS_H_
#define _BOARD_DRIVERS_H_

#include "ch.h"
#include "hal.h"
#include "ws281x.h"

static ws2811Config ws2811_cfg;

void BoardDriverInit(void);
void BoardDriverStart(void);
void BoardDriverShutdown(void);

#endif /* _LEDCONF_H_ */

/** @} */
