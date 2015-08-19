/**
 * @file    effect_simplecolor.c
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#include "effect_randomcolor.h"

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

msg_t EffectRandomUpdate(uint16_t led, systime_t time, void* effectcfg, void* effectdata, const struct Color* in, struct Color* out)
{
    (void)led;
    (void)time;
    struct EffectRandomColorCfg* cfg = (struct EffectRandomColorCfg*) effectcfg;
    struct EffectRandomColorData* data = (struct EffectRandomColorData*) effectdata;

    if (data->reset == true)
    {
        ColorCopy(&data->color, out);
        data->reset = false;
        return 1;
    }

    systime_t diff = time - data->lastupdate;
    if (diff < cfg->interval)
    {
        ColorCopy(in, out);
        return 0;
    }

    ColorRandom(&data->color);
    ColorCopy(&data->color, out);

    data->lastupdate += cfg->interval;

    return 0;
}

void EffectRandomReset(uint16_t led, systime_t time, void* effectcfg, void* effectdata)
{
    (void)led;
    (void)time;
    (void)effectcfg;
    (void)effectdata;
    struct EffectRandomColorData* data = (struct EffectRandomColorData*) effectdata;
    data->lastupdate = time;
    data->reset = true;
    ColorRandom(&data->color);
}

/** @} */
