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
#include "effect_fadingpixels.h"
#include "effect_volume.h"
#include "effect_buttons.h"
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
static bool nextEffect = false;
static bool noEffect = false;
static bool showControls = false;

static bool showVolume = false;
static bool showVolumeEffect = false;
static uint8_t newVolume = 0;
static systime_t startVolumeEffect = 0;

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

    .speed = MS2ST(100),
    .speedVariance = MS2ST(100),
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
/* FadingPixels                                                             */
/*===========================================================================*/
static struct EffectFadingPixelsCfg effFadingPixels_cfg =
{
    .spawninterval = MS2ST(300),
    .fadeperiod = MS2ST(2000),
    .color = {0, 0, 0},
    .randomColor = true,
    .number = 1,
};

static struct EffectFadingPixelsData effFadingPixels_data =
{
    .lastspawn = 0,
    .pixelColors = colorEffects1,
    .fadeStates = fadeEffects1,
};

static struct Effect effFadingPixels =
{
    .effectcfg = &effFadingPixels_cfg,
    .effectdata = &effFadingPixels_data,
    .update = &EffectFadingPixelsUpdate,
    .reset = &EffectFadingPixelsReset,
    .p_next = NULL,
};

/*===========================================================================*/
/* Volume                                                                    */
/*===========================================================================*/
static struct EffectVolumeCfg effVolume_cfg =
{
    .fontColor = {0x00, 0x8D, 0x6A},
    .minColor = {0x00, 0xAE, 0x00},
    .maxColor = {0x4D, 0x00, 0x00},
    .showNumber = true,
    .showBackground = false,
    .speed = MS2ST(30),
    .volume = 0,
};

static struct EffectVolumeData effVolume_data =
{
    .lastStepupdate = 0,
    .step = 0,
};

static struct Effect effVolume =
{
    .effectcfg = &effVolume_cfg,
    .effectdata = &effVolume_data,
    .update = &EffectVolumeUpdate,
    .reset = &EffectVolumeReset,
    .p_next = NULL,
};

/*===========================================================================*/
/* Buttons                                                                   */
/*===========================================================================*/
static struct EffectButtonsCfg effButtons_cfg =
{
    .play = {
        .x = 0,
        .y = 0,
        .color = {0x51, 0xBD, 0x1f},
    },
    .vol_up = {
        .x = 10,
        .y = 4,
        .color = {0x00, 0xFF, 0xBF},
    },
    .vol_down = {
        .x = 0,
        .y = 4,
        .color = {0x00, 0x8D, 0x6A},
    },
    .next = {
        .x = 10,
        .y = 2,
        .color = {0xFF, 0x00, 0x2D},
    },
    .prev = {
        .x = 0,
        .y = 2,
        .color = {0x9D, 0x00, 0x1C},
    },
    .special = {
        .x = 10,
        .y = 0,
        .color = {0xFF, 0x5F, 0x00},
    },

    .playMode = EFFECT_BUTTON_MODE_EMPTYPLAYLIST,
    .colorModeEmptyPlayList = {0x29, 0x00, 0x02},
    .colorModePause = {0x28, 0x5F, 0x0F},
    .colorModeStop = {0xE4, 0x24, 0x2E},

    .blendperiod = MS2ST(500),
};

static struct EffectButtonsData effButtons_data =
{
    .lastPlayMode = EFFECT_BUTTON_MODE_EMPTYPLAYLIST,
    .lastBlendStep = 1.0f,
    .lastPlayModeColor = {0x29, 0x00, 0x02},
    .lastUpdate = 0,
};

static struct Effect effButtons =
{
    .effectcfg = &effButtons_cfg,
    .effectdata = &effButtons_data,
    .update = &EffectButtonsUpdate,
    .reset = &EffectButtonsReset,
    .p_next = NULL,
};


/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static void Draw(void)
{
    int i;
    systime_t current = chVTGetSystemTime();
    /* clear buffer */
    memset(display.pixels, 0, sizeof(struct Color) * LEDCOUNT);

    if (effects.p_next != NULL)
    {
        EffectUpdate(effects.p_next, 0, 0, current, &display);
    }

    /* render volume for a 5s */
    if (showVolume == true)
    {
        startVolumeEffect = current;
        showVolume = false;
        effVolume_cfg.volume = newVolume;
        showVolumeEffect = true;
        if (chVTTimeElapsedSinceX(startVolumeEffect) > MS2ST(15000))
        {
            EffectReset(&effVolume, 0, 0, current);
        }
    }

    if (showVolumeEffect == true)
    {
        if (chVTTimeElapsedSinceX(startVolumeEffect) <= MS2ST(5000))
        {
            EffectUpdate(&effVolume, 0, 0, current, &display);
        }
        else
        {
            showVolumeEffect = false;
        }
    }

    //draw buttons
    if (showControls == true)
    {
        EffectUpdate(&effButtons, 0, 0, current, &display);
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
        if (noEffect == true)
        {
            effects.p_next = NULL;
        }
        if (nextEffect == true)
        {
            noEffect = false;
            nextEffect = false;

            activeEffect++;
            if (activeEffect > 3)
            {
                activeEffect = 1;
            }

            time = chVTGetSystemTime();
            (void)time;
        }

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
                ColorRandom(&effFallingPixels_cfg.color);
                effFallingPixels_cfg.randomRed = (rand() % 2) > 0;
                effFallingPixels_cfg.randomGreen = (rand() % 2) > 0;
                effFallingPixels_cfg.randomBlue = (rand() % 2) > 0;
            }
            else if (effectNumber == 3)
            {
                effFadingPixels_cfg.number = 1 + (rand() % 3);
                ColorRandom(&effFadingPixels_cfg.color);
                effFadingPixels_cfg.randomColor = (rand() % 2) > 0;

                effects.p_next = &effFadingPixels;
            }
            ResetEffects();
        }

        Draw();

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

void EffectControlNewPlayMode(uint8_t mode)
{
    if (mode <= EFFECT_BUTTON_MODE_EMPTYPLAYLIST)
        effButtons_cfg.playMode = mode;
}

void EffectControlNextEffect(void)
{
    nextEffect = true;
}

void EffectControlNoEffect(void)
{
    noEffect = true;
}

void EffectControlHideControls(void)
{
    showControls = false;
}

void EffectControlShowControls(void)
{
    showControls = true;
}

void EffectControlVolume(uint8_t volume)
{
    showVolume = true;
    newVolume = volume;
}
/** @} */
