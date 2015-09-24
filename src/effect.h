
/**
 * @file    effect.h
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#ifndef _EFFECT_H_
#define _EFFECT_H_

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
struct effect_t;

/**
 * @brief
 *
 * @param[in]
 * @param[in]
 */
typedef msg_t (*effect_update)(int16_t x, int16_t y, systime_t time, void* effectcfg, void* effectdata, const struct Color* color, struct effect_t* next);
typedef void (*effect_reset)(int16_t x, int16_t y, systime_t time, void* effectcfg, void* effectdata, struct effect_t* next);

struct effect_t
{
    void* effectcfg;
    void* effectdata;
    effect_update update;
    effect_reset reset;
    struct effect_t* p_next;
};


/**
 * @brief   Generic effects single link list, it works like a stack.
 */
struct effect_list {
    struct effect_t*              p_next;    /**< @brief Next in the list/queue.     */
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
    msg_t EffectUpdate(struct effect_t* effect, int16_t x, int16_t y, systime_t time, const struct Color* color);
    void EffectReset(struct effect_t* effect, int16_t x, int16_t y, systime_t time);
#ifdef __cplusplus
}
#endif

#endif /* _EFFECT_WANDERING_H_ */


/** @} */
