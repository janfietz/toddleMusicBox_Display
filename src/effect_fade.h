
/**
 * @file    effect_fade.h
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#ifndef _EFFECT_FADE_H_
#define _EFFECT_FADE_H_

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
struct EffectFadeCfg
{
    systime_t period;
    bool turn;
};

struct EffectFadeData
{
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
    msg_t EffectFadeUpdate(uint16_t led, systime_t time, void* effectcfg, void* effectdata, const struct Color* in, struct Color* out);
    void EffectFadeReset(uint16_t led, systime_t time, void* effectcfg, void* effectdata);
#ifdef __cplusplus
}
#endif

#endif /* _EFFECT_WANDERING_H_ */


/** @} */
