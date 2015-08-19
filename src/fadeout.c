/**
 * @file    blinl.c
 * @brief   Blink effect code.
 *
 * @addtogroup effects
 * @{
 */

#include "hal.h"
#include "fadeout.h"
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
struct FadeState
{
    systime_t fadesequence;
    systime_t fadeperiod;
    systime_t lastUpdate;
};

static struct FadeState fadeStates[FADE_LED_COUNT];
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

void FadeStart(uint16_t led, systime_t fadeperiod, systime_t time)
{
    if (led < FADE_LED_COUNT)
    {
        fadeStates[led].fadesequence = fadeperiod;
        fadeStates[led].fadeperiod = fadeperiod;
        fadeStates[led].lastUpdate = time;
    }
}
void FadeExecute(uint16_t led, struct Color* inColor, struct Color* outColor, systime_t time)
{
    float val = 1.0f;
    if (led < FADE_LED_COUNT)
    {
        systime_t diff = time - fadeStates[led].lastUpdate;
        if (diff > fadeStates[led].fadesequence)
        {
            fadeStates[led].fadesequence = 0;
        }
        else
        {
            fadeStates[led].fadesequence -= diff;
        }
        fadeStates[led].lastUpdate = time;

        val = sinf(((float)fadeStates[led].fadesequence/(float)fadeStates[led].fadeperiod) * (M_PI / 2.0));
    }

    outColor->R = inColor->R * val;
    outColor->G = inColor->G * val;
    outColor->B = inColor->B * val;
}

/** @} */
