
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
#include "effect.h"
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
    int8_t dir;
    bool turn;
};

struct EffectWanderingData
{
    int pos;
    int8_t dir_x;
    int8_t dir_y;
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
    msg_t EffectWanderingUpdate(int16_t x, int16_t y, systime_t time, void* effectcfg, void* effectdata, const struct Color* color, struct effect_t* next);
    void EffectWanderingReset(int16_t x, int16_t y, systime_t time, void* effectcfg, void* effectdata, struct effect_t* next);
#ifdef __cplusplus
}
#endif

#endif /* _EFFECT_WANDERING_H_ */


/** @} */
