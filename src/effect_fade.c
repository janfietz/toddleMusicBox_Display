/**
 * @file    effect_fade.c
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#include "effect_fade.h"
#include "ledconf.h"
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

msg_t EffectFadeUpdate(uint16_t led, systime_t time, void* effectcfg, void* effectdata, const struct Color* in, struct Color* out)
{

    (void)led;
    (void)time;
    (void)in;
    (void)effectdata;

    struct EffectFadeCfg* cfg = (struct EffectFadeCfg*) effectcfg;

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

    out->R = in->R * val;
    out->G = in->G * val;
    out->B = in->B * val;

    return 0;
}

void EffectFadeReset(uint16_t led, systime_t time, void* effectcfg, void* effectdata)
{
    (void)effectdata;

    struct EffectFadeCfg* cfg = (struct EffectFadeCfg*) effectcfg;

    if (led < LEDCOUNT)
    {
        effectFadeStates[led].fadesequence = cfg->period;
        effectFadeStates[led].lastUpdate = time;
    }
}

/** @} */
