
/**
 * @file    fadeout.h
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#ifndef _FADEOUT_H_
#define _FADEOUT_H_

#include "ch.h"
#include "ledconf.h"
#include "color.h"
/*===========================================================================*/
/* Effect constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Effect pre-compile time settings.                                         */
/*===========================================================================*/
/**
 * @brief   WS2811 interrupt priority level setting.
 */
#if !defined(FADE_LED_COUNT) || defined(__DOXYGEN__)
#define FADE_LED_COUNT         LEDCOUNT
#endif
/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/
typedef void (*fade_executecallback_t)(uint16_t led, struct Color* inColor, struct Color* outColor, systime_t time);

#ifdef __cplusplus
extern "C" {
#endif
  void FadeStart(uint16_t led, systime_t fadeperiod, systime_t time);
  void FadeExecute(uint16_t led, struct Color* inColor, struct Color* outColor, systime_t time);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_ws2811 */


/** @} */
