
/**
 * @file    display.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "displayconf.h"
#include "color.h"
#include <stdint.h>
#include <stdbool.h>

/*===========================================================================*/
/* Effect constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Effect pre-compile time settings.                                         */
/*===========================================================================*/

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

#ifdef __cplusplus
extern "C" {
#endif
    void DisplayDraw(int16_t x, int16_t y, const struct Color* color);
    void DisplayPaint(void);

    void DisplayLedToCoord(uint16_t led, int16_t* x, int16_t* y);
    bool DisplayCoordToLed(int16_t x, int16_t y, uint16_t* led);

    extern void SetLedColor(uint16_t led, const struct Color* color);
    extern void SetUpdateLed(void);
#ifdef __cplusplus
}
#endif

#endif /* _DISPLAY_H_ */


/** @} */
