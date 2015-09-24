/**
 * @file    ledconf.h
 * @brief
 *
 * @{
 */

#ifndef _LEDCONF_H_
#define _LEDCONF_H_

#include "led.h"

#define LEDCOUNT 55

#define LED_VOLDOWN 0
#define LED_VOLUP 10
#define LED_PREV 22
#define LED_NEXT 32
#define LED_PLAYPAUSE 44
#define LED_FUNC_01 54

extern struct LedSetting LED1[LEDCOUNT];


#endif /* _LEDCONF_H_ */

/** @} */
