
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
    msg_t EffectRandomUpdate(uint16_t led, systime_t time, void* effectcfg, void* effectdata, const struct Color* in, struct Color* out);
    void EffectRandomReset(uint16_t led, systime_t time, void* effectcfg, void* effectdata);
#ifdef __cplusplus
}
#endif

#endif /* _EFFECT_RANDOMCOLOR_H_ */


/** @} */
