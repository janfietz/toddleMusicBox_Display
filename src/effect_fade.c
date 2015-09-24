/**
 * @file    effect_fade.c
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#include "effect_fade.h"
#include "ledconf.h"
#include "display.h"
#include <stdlib.h>
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
struct EffectFadeState
{
    systime_t fadesequence;
    systime_t lastUpdate;
};

static struct EffectFadeState effectFadeStates[LEDCOUNT];
/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/


/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief
 *
 */

msg_t EffectFadeUpdate(int16_t x, int16_t y, systime_t time, void* effectcfg, void* effectdata, const struct Color* color, struct effect_t* next)
{
    (void)effectdata;

    struct EffectFadeCfg* cfg = (struct EffectFadeCfg*) effectcfg;

    uint16_t led = 0;
    if (DisplayCoordToLed(x, y, &led) == true)
    {
        float val = 1.0f;
        if (led < LEDCOUNT)
        {
            systime_t diff = time - effectFadeStates[led].lastUpdate;
            if (diff > effectFadeStates[led].fadesequence)
            {
                effectFadeStates[led].fadesequence = 0;
            }
            else
            {
                effectFadeStates[led].fadesequence -= diff;
            }
            effectFadeStates[led].lastUpdate = time;

            val = sinf(((float)effectFadeStates[led].fadesequence/(float)cfg->period) * (M_PI / 2.0));
        }

        struct Color out =
        {
            color->R * val,
            color->G * val,
            color->B * val
        };

        return EffectUpdate(next, x, y, time, &out);
    }

    return 0;
}

void EffectFadeReset(int16_t x, int16_t y, systime_t time, void* effectcfg, void* effectdata, struct effect_t* next)
{
    (void)effectdata;

    struct EffectFadeCfg* cfg = (struct EffectFadeCfg*) effectcfg;

    uint16_t led = 0;
    if (DisplayCoordToLed(x, y, &led) == true)
    {
        if (led < LEDCOUNT)
        {
            effectFadeStates[led].fadesequence = cfg->period;
            effectFadeStates[led].lastUpdate = time;
        }

        EffectReset(next, x, y, time);
    }

}

/** @} */
