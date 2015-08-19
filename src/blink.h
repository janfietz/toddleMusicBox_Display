
/**
 * @file    blink.h
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#ifndef _BLINK_H_
#define _BLINK_H_

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
#if !defined(BLINK_LED_COUNT) || defined(__DOXYGEN__)
#define BLINK_LED_COUNT         LEDCOUNT
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
typedef void (*blink_executecallback_t)(uint16_t led, struct Color* inColor, struct Color* outColor, systime_t time);

#ifdef __cplusplus
extern "C" {
#endif
  void BlinkStart(uint16_t led, bool dirUp, systime_t time);
  void BlinkExecute(uint16_t led, struct Color* inColor, struct Color* outColor, systime_t time);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_ws2811 */


/** @} */
