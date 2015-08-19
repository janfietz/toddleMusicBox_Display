/**
 * @file    blinl.c
 * @brief   Blink effect code.
 *
 * @addtogroup effects
 * @{
 */

#include "hal.h"
#include "blink.h"
#include "float.h"
#include "math.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/
struct BlinkState
{
    bool directionUp;
    uint8_t step;
    systime_t lastUpdate;
};

static struct BlinkState blinkStates[BLINK_LED_COUNT];
/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/


/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Blink execution funktion.
 *
 */

void BlinkStart(uint16_t led, bool dirUp, systime_t time)
{
    (void) dirUp;
    if (led < BLINK_LED_COUNT)
    {
        blinkStates[led].lastUpdate = time;
    }
}
void BlinkExecute(uint16_t led, struct Color* inColor, struct Color* outColor, systime_t time)
{
    float val = 1.0f;
    if (led < BLINK_LED_COUNT)
    {
        systime_t diff = time - blinkStates[led].lastUpdate;

        //val = (expf(sinf((float)ST2MS(diff)/2000.0*M_PI)) - 0.36787944) * 108.0;
        val = (expf(sinf((float)ST2MS(diff) * M_PI / 2000.0)) - 1.0) / 6.389056098;

        if (ST2MS(diff) > 4000)
            blinkStates[led].lastUpdate += MS2ST(4000);
    }

    outColor->R = inColor->R * val;
    outColor->G = inColor->G * val;
    outColor->B = inColor->B * val;
}

/** @} */
