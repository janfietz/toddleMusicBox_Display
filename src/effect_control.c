/**
 * @file    effect_control.c
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#include "effect_control.h"
#include "ch.h"
#include "hal.h"

#include "display.h"
#include "displayconf.h"
#include "ledconf.h"
#include "effect_randompixels.h"
#include "effect_fallingpixels.h"
#include <string.h>
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

static THD_WORKING_AREA(waEffectControlThread, 1024);

static struct effect_list effects;

static systime_t lastPatternSelect;

struct Color colorEffects1[LEDCOUNT];
struct EffectFadeState fadeEffects1[LEDCOUNT];

struct Color colorBuffer1[LEDCOUNT];
struct DisplayBuffer display =
{
        .width = DISPLAY_WIDTH,
        .height = DISPLAY_HEIGHT,
        .pixels = colorBuffer1,
};

struct Color resetColor;
static int16_t activeEffect = 0;

/*===========================================================================*/
/* RandmoPixels                                                              */
/*===========================================================================*/
static struct EffectRandomPixelsCfg effRandomPixels_cfg =
{
    .spawninterval = MS2ST(300),
    .color = {0, 0, 0},
    .randomRed = true,
    .randomGreen = true,
    .randomBlue = true,
};

static struct EffectRandomPixelsData effRandomPixels_data =
{
    .lastspawn = 0,
    .pixelColors = colorEffects1,
};

static struct Effect effRandomPixels =
{
    .effectcfg = &effRandomPixels_cfg,
    .effectdata = &effRandomPixels_data,
    .update = &EffectRandomPixelsUpdate,
    .reset = &EffectRandomPixelsReset,
    .p_next = NULL,
};

/*===========================================================================*/
/* FallingPixels                                                             */
/*===========================================================================*/
static struct EffectFallingPixelsCfg effFallingPixels_cfg =
{
    .spawninterval = MS2ST(300),
    .fadeperiod = MS2ST(1000),
    .color = {0, 0, 0},
    .randomRed = true,
    .randomGreen = true,
    .randomBlue = true,
};

static struct EffectFallingPixelsData effFallingPixels_data =
{
    .lastspawn = 0,
    .pixelColors = colorEffects1,
    .fadeStates = fadeEffects1,
};

static struct Effect effFallingPixels =
{
    .effectcfg = &effFallingPixels_cfg,
    .effectdata = &effFallingPixels_data,
    .update = &EffectFallingPixelsUpdate,
    .reset = &EffectFallingPixelsReset,
    .p_next = NULL,
};


/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static void Draw(void)
{
    int i;
    systime_t current = chVTGetSystemTime();
    if (effects.p_next != NULL)
    {
        /* clear buffer */
        memset(display.pixels, 0, sizeof(struct Color) * LEDCOUNT);

        EffectUpdate(effects.p_next, 0, 0, current, &display);
    }

    for (i = 0; i < LEDCOUNT; i++)
    {
        SetLedColor(i, &display.pixels[i]);
    }
    SetUpdateLed();
}

static void ResetEffects(void)
{
    systime_t current = chVTGetSystemTime();
    struct Effect* effect = effects.p_next;
    EffectReset(effect, 0, 0, current);
}

static THD_FUNCTION(EffectControlThread, arg)
{
    (void) arg;
    chRegSetThreadName("effectcontrol");

    effects.p_next = NULL;

    int16_t effectNumber = -1;
    systime_t time = chVTGetSystemTime();
    while (TRUE)
    {
        if (activeEffect != effectNumber)
        {
            if(effectNumber == 1)
            {

            }

            effectNumber = activeEffect;

            if(effectNumber == 1)
            {
                effects.p_next = &effRandomPixels;
            }
            else if (effectNumber == 2)
            {
                effects.p_next = &effFallingPixels;
            }
            ResetEffects();
        }

        Draw();

        if (lastPatternSelect == 0 || chVTTimeElapsedSinceX(lastPatternSelect) > MS2ST(60000))
        {
            if (chVTTimeElapsedSinceX(time) > MS2ST(15000))
            {
                activeEffect++;
                if (activeEffect > 2)
                {
                    activeEffect = 1;
                }

                time = chVTGetSystemTime();
            }
        }

        chThdSleepMilliseconds(10);
    }
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief
 *
 */

void ResetWithColor(struct Color* color)
{
    if (activeEffect == 0xFF)
    {
        activeEffect = 0xFF - 1;
    }
    else
    {
        activeEffect = 0xFF;
    }

    ColorCopy(color, &resetColor);

    lastPatternSelect = chVTGetSystemTime();
}



void EffectControlInitThread(void)
{
    activeEffect = 2;
}

void EffectControlStartThread(void)
{
    chThdCreateStatic(waEffectControlThread, sizeof(waEffectControlThread), LOWPRIO, EffectControlThread, NULL);
}
/** @} */
