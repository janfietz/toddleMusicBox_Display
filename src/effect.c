/**
 * @file    effect.c
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#include "effect.h"
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

/**
 * @brief
 *
 */

msg_t EffectUpdate(struct effect_t* effect, int16_t x, int16_t y, systime_t time, const struct Color* color)
{
    if (effect == NULL)
    {
        DisplayDraw(x, y, color);
        return 0;
    }

    return effect->update(x, y, time, effect->effectcfg, effect->effectdata, color, effect->p_next);
}

void EffectReset(struct effect_t* effect, int16_t x, int16_t y, systime_t time)
{
    if (effect != NULL)
    {
        effect->reset(x, y, time, effect->effectcfg, effect->effectdata, effect->p_next);
    }
}

/** @} */
