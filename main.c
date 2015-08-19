/*
 ChibiOS - Copyright (C) 2006-2014 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "ch.h"
#include "hal.h"
#include "ws281x.h"

#include "chprintf.h"
#include "shell.h"

#include "float.h"
#include "math.h"

#include "usbcfg.h"

#include <stdlib.h>

#include "ledconf.h"
#include "color.h"
#include "blink.h"
#include "fadeout.h"
#include "effect.h"
#include "effect_wandering.h"
#include "effect_simplecolor.h"
#include "effect_randomcolor.h"
#include "effect_fade.h"


/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;




static ws2811Driver ws2811;
static ws2811Config ws2811_cfg =
{
    LEDCOUNT,
    LED1,
    0b00000010,
    (uint32_t)(&(GPIOA->BSRR.H.set)),
    (uint32_t)(&(GPIOA->BSRR.H.clear)),
    WS2812_BIT_PWM_WIDTH,
    WS2812_ZERO_PWM_WIDTH,
    WS2812_ONE_PWM_WIDTH,
    {
        168000000 / 2 / WS2812_BIT_PWM_WIDTH,
        (LEDCOUNT * 24) + 20,
        NULL,
        {
            { PWM_OUTPUT_ACTIVE_HIGH, NULL },
            { PWM_OUTPUT_DISABLED, NULL },
            { PWM_OUTPUT_DISABLED, NULL },
            { PWM_OUTPUT_DISABLED, NULL }
        },
        TIM_CR2_MMS_2, /* master mode selection */
        0,
    },
    &PWMD2,
    {
        168000000 / 2,
        WS2812_BIT_PWM_WIDTH,
        NULL,
        {
            { PWM_OUTPUT_ACTIVE_HIGH, NULL },
            { PWM_OUTPUT_ACTIVE_HIGH, NULL },
            { PWM_OUTPUT_ACTIVE_HIGH, NULL },
            { PWM_OUTPUT_ACTIVE_HIGH, NULL }
        },
        0,
        TIM_DIER_UDE | TIM_DIER_CC3DE | TIM_DIER_CC1DE,
    },
    &PWMD1,
    STM32_DMA2_STREAM5,
    STM32_DMA2_STREAM1,
    STM32_DMA2_STREAM6,
};

static systime_t lastPatternSelect;
struct Color colors[LEDCOUNT];
struct Color resetColor;
static blink_executecallback_t blink[LEDCOUNT];
static fade_executecallback_t faders[LEDCOUNT];
struct Pattern1Cfg
{
    bool randomized;
    int templatesCount;
    int currentTemplate;
    struct Color template[LEDCOUNT];
};

static void testLedPattern(struct Pattern1Cfg* cfg)
{
    int i;
    for (i = ws2811_cfg.ledCount - 1; i > 0; i--)
    {
        ColorCopy(&colors[i-1], &colors[i]);
    }
    if (cfg->randomized == true)
    {
        ColorRandom(&colors[i]);
    }
    else
    {
        ColorCopy(&cfg->template[cfg->currentTemplate], &colors[0]);
        ++cfg->currentTemplate;
        if (cfg->currentTemplate >= cfg->templatesCount)
        {
            cfg->currentTemplate = 0;
        }
    }
}

static struct effect_list effects;
static void UpdateColors(void)
{
    int i;
    systime_t current = chVTGetSystemTime();
    for (i = 0; i < ws2811_cfg.ledCount; i++)
    {
        struct Color c;
        if (effects.p_next != NULL)
        {
            struct effect_t* effect = effects.p_next;
            while (effect != NULL)
            {
                msg_t result = effect->update(i, current, effect->effectcfg, effect->effectdata, &colors[i], &colors[i]);
                if (result == 1 && (effect->p_next != NULL))
                {
                    struct effect_t* nextEffect = effect->p_next;
                    nextEffect->reset(i, current, nextEffect->effectcfg, nextEffect->effectdata);
                }
                effect = effect->p_next;
            }

            ColorCopy(&colors[i], &c);
        }
        else
        {
            ColorCopy(&colors[i], &c);
            if (blink[i] != NULL)
            {
                blink[i](i, &c, &c, current);
            }
            if (faders[i] != NULL)
            {
                faders[i](i, &c, &c, current);
            }
        }

        ws2811SetColor(&ws2811, i, &c);
    }
    ws2811Update(&ws2811);
}

static void ResetEffects(void)
{
    int i;
    systime_t current = chVTGetSystemTime();
    for (i = 0; i < ws2811_cfg.ledCount; i++)
    {
        struct effect_t* effect = effects.p_next;
        while (effect != NULL)
        {
            msg_t result = effect->reset(i, current, effect->effectcfg, effect->effectdata);
            effect = effect->p_next;
        }
    }
}

static struct EffectWanderingCfg wandering_fullstrip_cfg =
{
    .ledbegin = 0,
    .ledend = 58
};

static struct EffectWanderingData wandering_fullstrip_data;

static struct EffectSimpleColorCfg effSimpleColor_cfg =
{
    .color = {0, 0, 0},
};

static struct EffectSimpleColorData effSimpleColor_data;

static struct EffectRandomColorCfg effRandomColor_cfg =
{
    .interval = MS2ST(1000),
};

static struct EffectRandomColorData effRandomColor_data;

static struct EffectFadeCfg effFade_cfg =
{
    .period = MS2ST(750),
};
static struct EffectFadeData effFade_data;

static struct effect_t effFade =
{
       .effectcfg = &effFade_cfg,
       .effectdata = &effFade_data,
       .update = (effect_update)&EffectFadeUpdate,
       .reset = (effect_reset)&EffectFadeReset,
       .p_next = NULL,
};



static struct effect_t effSimpleColor =
{
       .effectcfg = &effSimpleColor_cfg,
       .effectdata = &effSimpleColor_data,
       .update = (effect_update)&EffectSimpleUpdate,
       .reset = (effect_reset)&EffectSimpleReset,
       .p_next = &effFade,
};

static struct effect_t effWandering =
{
       .effectcfg = &wandering_fullstrip_cfg,
       .effectdata = &wandering_fullstrip_data,
       .update = (effect_update)&EffectWanderingUpdate,
       .reset = (effect_reset)&EffectWanderingReset,
       .p_next = &effSimpleColor,
};

static struct Pattern1Cfg patternCfg =
{
    .randomized = false,
    .templatesCount = 4,
    .currentTemplate = 0,
    .template[0] = {255, 0, 0},
    .template[1] = {0, 0, 0},
    .template[2] = {0, 0, 255},
    .template[3] = {0, 0, 0},
};

static struct effect_t effRandomColor =
{
       .effectcfg = &effRandomColor_cfg,
       .effectdata = &effRandomColor_data,
       .update = (effect_update)&EffectRandomUpdate,
       .reset = (effect_reset)&EffectRandomReset,
       .p_next = &effFade,
};

static int16_t activeTestPattern = 0;
static THD_WORKING_AREA(waThread1, 512);
static THD_FUNCTION(Thread1, arg)
{
    (void) arg;
    chRegSetThreadName("thread");

    effects.p_next = NULL;

    int16_t pattern = 0;
    systime_t time = chVTGetSystemTime();
    while (TRUE)
    {
        systime_t current = chVTGetSystemTime();

        if (activeTestPattern != pattern)
        {
            if(pattern == 2 || pattern == 3 || pattern == 5)
            {
                effects.p_next = NULL;
            }
            else if(pattern == 4)
            {
                int i;
                for (i = 0; i < ws2811_cfg.ledCount; i++)
                {
                    blink[i] = NULL;
                }
            }

            pattern = activeTestPattern;

            if(pattern == 2 || pattern == 3)
            {

                ColorRandom(&effSimpleColor_cfg.color);

                wandering_fullstrip_cfg.speed = rand() % 100;
                wandering_fullstrip_cfg.turn = (rand() % 2) > 0;

                effFade_cfg.period = MS2ST(rand() % 750);

                effWandering.p_next = &effSimpleColor;
                effects.p_next = &effWandering;

                ResetEffects();

            }
            else if (pattern == 5)
            {
                wandering_fullstrip_cfg.speed = rand() % 100;
                wandering_fullstrip_cfg.turn = (rand() % 2) > 0;

                effFade_cfg.period = MS2ST(rand() % 750);

                effWandering.p_next = &effRandomColor;
                effects.p_next = &effWandering;

                ResetEffects();

            }
            else if(pattern == 4)
            {
                struct Color white = {100, 100, 100};
                int i;
                for (i = 0; i < ws2811_cfg.ledCount; i++)
                {
                    ColorCopy(&white, &colors[i]);
                    BlinkStart(i, true, current);
                    blink[i] = &BlinkExecute;
                }
            }
            else if(pattern == 0xFF || pattern == 0xFF - 1)
            {
                int i;
                for (i = 0; i < ws2811_cfg.ledCount; i++)
                {
                    ColorCopy(&resetColor, &colors[i]);
                }
            }
            else
            {
                struct Color black = {0, 0, 0};
                int i;
                for (i = 0; i < ws2811_cfg.ledCount; i++)
                {
                    ColorCopy(&black, &colors[i]);
                }
            }
        }

        if(pattern == 1)
        {
            patternCfg.randomized = false;
            testLedPattern(&patternCfg);
            chThdSleepMilliseconds(40);
        }
        else if(pattern == 2)
        {

        }
        else if(pattern == 3)
        {

        }
        else if(pattern == 4)
        {
        }
        else if(pattern == 5)
        {

        }
        else if(pattern == 0xff)
        {

        }
        else if(pattern == 0xFF - 1)
        {

        }
        else
        {
            patternCfg.randomized = true;
            testLedPattern(&patternCfg);
            chThdSleepMilliseconds(40);
        }

        UpdateColors();

        if (lastPatternSelect == 0 || chVTTimeElapsedSinceX(lastPatternSelect) > MS2ST(60000))
        {
            if (chVTTimeElapsedSinceX(time) > MS2ST(30000))
            {
                activeTestPattern++;
                if (activeTestPattern > 5)
                {
                    activeTestPattern = 0;
                }

                time = chVTGetSystemTime();
            }
        }

        chThdSleepMilliseconds(10);
    }
}


#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)
#define TEST_WA_SIZE    THD_WORKING_AREA_SIZE(256)

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
  size_t n, size;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: mem\r\n");
    return;
  }
  n = chHeapStatus(NULL, &size);
  chprintf(chp, "core free memory : %u bytes\r\n", chCoreGetStatusX());
  chprintf(chp, "heap fragments   : %u\r\n", n);
  chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
  static const char *states[] = {CH_STATE_NAMES};
  thread_t *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: threads\r\n");
    return;
  }
  chprintf(chp, "    addr    stack prio refs     state\r\n");
  tp = chRegFirstThread();
  do {
    chprintf(chp, "%08lx %08lx %4lu %4lu %9s\r\n",
             (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
             (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
             states[tp->p_state]);
    tp = chRegNextThread(tp);
  } while (tp != NULL);
}

static void cmd_pattern(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc!=1) {
        chprintf(chp, "Usage: pattern [0|1|2]\r\n");
        chprintf(chp, "       Select a LED test pattern [pattern]\r\n");
        return;
    }
    int pattern = atoi(argv[0]);
    chprintf(chp, "Pattern: %d Started\r\n",pattern);
    activeTestPattern = pattern;

    lastPatternSelect = chVTGetSystemTime();
}

static void cmd_blink(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc!=1) {
        chprintf(chp, "Usage: blink [led]\r\n");
        chprintf(chp, "       Select a LED to blink [LED]\r\n");
        return;
    }
    int led = atoi(argv[0]);
    if (led < LEDCOUNT)
    {
        if (blink[led] == NULL)
        {
            BlinkStart(led, true, chVTGetSystemTime());
            blink[led] = &BlinkExecute;
            chprintf(chp, "Blink: %d started\r\n", led);
        }
        else
        {
            blink[led] = NULL;
            chprintf(chp, "Blink: %d stoped\r\n", led);
        }
    }

}

static void cmd_reset(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc == 0)
    {
        /* create random color */
        ColorRandom(&resetColor);
    }
    else if (argc == 3)
    {
        resetColor.R = (uint8_t)atoi(argv[0]);
        resetColor.G = (uint8_t)atoi(argv[1]);
        resetColor.B = (uint8_t)atoi(argv[2]);
    }

    if (activeTestPattern == 0xFF)
    {
        activeTestPattern = 0xFF - 1;
    }
    else
    {
        activeTestPattern = 0xFF;
    }

    lastPatternSelect = chVTGetSystemTime();

    chprintf(chp, "Reset: {%d %d %d}\r\n", resetColor.R, resetColor.G, resetColor.B);
}


static const ShellCommand commands[] = {
  {"mem", cmd_mem},
  {"threads", cmd_threads},
  {"pattern", cmd_pattern},
  {"blink", cmd_blink},
  {"reset", cmd_reset},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};

/*
 * Application entry point.
 */

int main(void)
{
    thread_t *shelltp = NULL;

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();
    /*
     * Shell manager initialization.
     */
    shellInit();

    /*
     * Initializes a serial-over-USB CDC driver.
     */
    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);

    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1000);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    /*
     * Starts ws2811 driver
     */
    ws2811ObjectInit(&ws2811);
    ws2811Start(&ws2811, &ws2811_cfg);
    palSetGroupMode(GPIOA, 0b00000010, 0, PAL_MODE_OUTPUT_PUSHPULL|PAL_STM32_OSPEED_HIGHEST|PAL_STM32_PUDR_FLOATING);


    activeTestPattern = 2;
    /*
     * Creates the example thread.
     */
    chThdCreateStatic(waThread1, sizeof(waThread1), LOWPRIO, Thread1, NULL);

    /*
     * Normal main() thread activity, in this demo it just performs
     * a shell respawn upon its termination.
     */
    while (TRUE)
    {
        if (!shelltp) {
          if (SDU1.config->usbp->state == USB_ACTIVE) {
            /* Spawns a new shell.*/
            shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
          }
        }
        else {
          /* If the previous shell exited.*/
          if (chThdTerminatedX(shelltp)) {
            /* Recovers memory of the previous shell.*/
            chThdRelease(shelltp);
            shelltp = NULL;
          }
        }
        chThdSleepMilliseconds(500);
    }
}
