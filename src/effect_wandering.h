
/**
 * @file    effect_wandering.h
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#ifndef _EFFECT_WANDERING_H_
#define _EFFECT_WANDERING_H_

#include "ch.h"
#include "color.h"
/*===========================================================================*/
/* Effect constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Effect pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/
struct EffectWanderingCfg
{
    int speed;
    int ledbegin;
    int ledend;
    bool turn;
};

struct EffectWanderingData
{
    int pos;
    int traildir;
    systime_t lastupdate;
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
    msg_t EffectWanderingUpdate(uint16_t led, systime_t time, void* effectcfg, void* effectdata, const struct Color* in, struct Color* out);
    void EffectWanderingReset(uint16_t led, systime_t time, void* effectcfg, void* effectdata);
#ifdef __cplusplus
}
#endif

#endif /* _EFFECT_WANDERING_H_ */


/** @} */
