/**
 * @file    effect_simplecolor.c
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#include "effect_simplecolor.h"

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

/**
 * @brief
 *
 */

msg_t EffectSimpleUpdate(uint16_t led, systime_t time, void* effectcfg, void* effectdata, const struct Color* in, struct Color* out)
{
    (void)led;
    (void)time;
    struct EffectSimpleColorCfg* cfg = (struct EffectSimpleColorCfg*) effectcfg;
    struct EffectSimpleColorData* data = (struct EffectSimpleColorData*) effectdata;

    if (data->reset == true)
    {
        ColorCopy(&cfg->color, out);
        data->reset = false;
        return 1;
    }

    ColorCopy(in, out);

    return 0;
}

void EffectSimpleReset(uint16_t led, systime_t time, void* effectcfg, void* effectdata)
{
    (void)led;
    (void)time;
    (void)effectcfg;
    (void)effectdata;
    struct EffectSimpleColorData* data = (struct EffectSimpleColorData*) effectdata;
    data->reset = true;
}

/** @} */
