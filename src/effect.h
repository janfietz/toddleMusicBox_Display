
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
/**
 * @brief
 *
 * @param[in]
 * @param[in]
 */
typedef msg_t (*effect_update)(uint16_t led, systime_t time, void* effectcfg, void* effectdata, const struct Color* in, struct Color* out);
typedef msg_t (*effect_reset)(uint16_t led, systime_t time, void* effectcfg, void* effectdata);

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


#endif /* _EFFECT_WANDERING_H_ */


/** @} */
