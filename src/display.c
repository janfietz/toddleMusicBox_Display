/**
 * @file    display.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "display.h"
#include <stdlib.h>

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/
void DisplayLedToCoord(uint16_t led, int16_t* x, int16_t* y)
{
    *x = led % DISPLAY_WIDTH;
    *y = led / DISPLAY_WIDTH;
}

bool DisplayCoordToLed(int16_t x, int16_t y, uint16_t* led)
{
    /* check if out of bounds */
    if (x < 0)
        return false;
    else if (x >= DISPLAY_WIDTH)
        return false;

    if (y < 0)
        return false;
    else if (y >= DISPLAY_HEIGHT)
        return false;

    *led = x + (y * DISPLAY_WIDTH);
    return true;
}

void DisplayDraw(int16_t x, int16_t y, const struct Color* color)
{
    /* convert 2d space coordinate to led number */
    uint16_t lednum = 0;
    if (DisplayCoordToLed(x, y, &lednum) == true)
    {
        SetLedColor(lednum, color);
    }
}

void DisplayPaint(void)
{
    SetUpdateLed();


}

/** @} */
