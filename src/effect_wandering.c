/**
 * @file    effect_wandering.c
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#include "effect_wandering.h"
#include <stdlib.h>
#include "fadeout.h"

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

msg_t EffectWanderingUpdate(uint16_t led, systime_t time, void* effectcfg, void* effectdata, const struct Color* in, struct Color* out)
{

    (void)led;
    (void)time;
    struct EffectWanderingCfg* cfg = (struct EffectWanderingCfg*) effectcfg;
    struct EffectWanderingData* data = (struct EffectWanderingData*) effectdata;

    ColorCopy(in, out);

    if (led != data->pos)
    {
        return 0;
    }

    systime_t diff = time - data->lastupdate;
    if (diff < MS2ST(cfg->speed))
    {
        return 0;
    }

    data->lastupdate += MS2ST(cfg->speed);

    if (data->pos >= (cfg->ledend))
    {
        if (cfg->turn == true)
        {
            data->traildir = data->traildir * -1;
            data->pos = cfg->ledend - 1;
        }
        else
        {
            if (data->pos + data->traildir > cfg->ledend)
            {
                data->pos = cfg->ledbegin;
            }
            else
            {
                data->pos += data->traildir;
            }

        }
    }
    else if (data->pos == cfg->ledbegin)
    {
        if (cfg->turn == true)
        {
            data->traildir = data->traildir * -1;
            data->pos = cfg->ledbegin + 1;
        }
        else
        {
            if (data->pos + data->traildir < 0)
            {
                data->pos = cfg->ledend;
            }
            else
            {
                data->pos = data->pos + data->traildir;
            }
        }
    }
    else
    {
        data->pos += + data->traildir;
    }

    return 1;
}

void EffectWanderingReset(uint16_t led, systime_t time, void* effectcfg, void* effectdata)
{
    (void)led;
    (void)time;
    struct EffectWanderingCfg* cfg = (struct EffectWanderingCfg*) effectcfg;
    struct EffectWanderingData* data = (struct EffectWanderingData*) effectdata;

    data->pos = cfg->ledbegin;
    data->traildir = 1;
    data->lastupdate = time;
}

/** @} */
