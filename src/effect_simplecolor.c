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

msg_t EffectSimpleUpdate(int16_t x, int16_t y, systime_t time,
        void* effectcfg, void* effectdata, const struct Color* color, struct effect_t* next)
{
    (void) color;
    struct EffectSimpleColorCfg* cfg = (struct EffectSimpleColorCfg*) effectcfg;
    struct EffectSimpleColorData* data = (struct EffectSimpleColorData*) effectdata;

    return EffectUpdate(next, x, y, time, &cfg->color);
}

void EffectSimpleReset(int16_t x, int16_t y, systime_t time, void* effectcfg, void* effectdata, struct effect_t* next)
{
    (void)effectcfg;
    (void)effectdata;
    struct EffectSimpleColorData* data = (struct EffectSimpleColorData*) effectdata;
    data->reset = true;

    EffectReset(next, x, y, time);
}

/** @} */
