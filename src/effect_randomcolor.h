
/**
 * @file    effect_randomcolor.h
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#ifndef _EFFECT_RANDOMCOLOR_H_
#define _EFFECT_RANDOMCOLOR_H_

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
struct EffectRandomColorCfg
{
    systime_t interval;
};

struct EffectRandomColorData
{
    systime_t lastupdate;
    bool reset;
    struct Color color;
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
    msg_t EffectRandomUpdate(int16_t x, int16_t y, systime_t time, void* effectcfg, void* effectdata, const struct Color* color, struct effect_t* next);
    void EffectRandomReset(int16_t x, int16_t y, systime_t time, void* effectcfg, void* effectdata, struct effect_t* next);
#ifdef __cplusplus
}
#endif

#endif /* _EFFECT_RANDOMCOLOR_H_ */


/** @} */
