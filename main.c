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

#include "board_drivers.h"

#include "chprintf.h"
#include "shell.h"

#include "float.h"
#include "math.h"
#include <string.h>

#include "usbcfg.h"

#include <stdlib.h>

#include "ledconf.h"
#include "display.h"
#include "color.h"
#include "blink.h"
#include "fadeout.h"
#include "effect_control.h"

#include "effect_wandering.h"
#include "effect_simplecolor.h"
#include "effect_randomcolor.h"
#include "effect_fade.h"
#include "effect_font_5x5.h"


/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;
ws2811Driver ws2811;


void SetUpdateLed(void)
{
    ws2811Update(&ws2811);
}

void SetLedColor(uint16_t led, const struct Color* color)
{
    ws2811SetColor(&ws2811, led, color);
}


#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

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



static void cmd_char(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc != 1)
    {
        chprintf(chp, "Usage: char [char]\r\n");
        chprintf(chp, "       Draw a char [char]\r\n");
        return;
    }


    if (activeTestPattern == 0xEF)
    {
        activeTestPattern = 0xEF - 1;
    }
    else
    {
        activeTestPattern = 0xEF;
    }

    char* input = argv[0];

    int i;
    for (i = 0; i < 16; i++)
    {
        char c = input[i];
        cmd_char_text[i] = c;
        if (c == 0)
        {
            break;
        }
    }

    lastPatternSelect = chVTGetSystemTime();
}

static void cmd_draw(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc != 2)
    {
        chprintf(chp, "Usage: draw [x] [y]\r\n");
        chprintf(chp, "       Draw a pixel [x] [y]\r\n");
        return;
    }


    if (activeTestPattern == 0xDF)
    {
        activeTestPattern = 0xDF - 1;
    }
    else
    {
        activeTestPattern = 0xDF;
    }

    pixel[0] = (int16_t)atoi(argv[0]);
    pixel[1] = (int16_t)atoi(argv[1]);

    lastPatternSelect = chVTGetSystemTime();
}

static void cmd_volume(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc != 1)
    {
        chprintf(chp, "Usage: vol [volume] \r\n");
        return;
    }


    if (activeTestPattern == 0xCF)
    {
        activeTestPattern = 0xCF - 1;
    }
    else
    {
        activeTestPattern = 0xCF;
    }

    volume = (int8_t)atoi(argv[0]);

    lastPatternSelect = chVTGetSystemTime();
}


static const ShellCommand commands[] = {
  {"mem", cmd_mem},
  {"threads", cmd_threads},
  {"pattern", cmd_pattern},
  {"blink", cmd_blink},
  {"reset", cmd_reset},
  {"char", cmd_char},
  {"draw", cmd_draw},
  {"vol", cmd_volume},
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


    BoardDriverInit();

    BoardDriverStart();

    /*
     * Creates the example thread.
     */
    EffectControlInitThread();
    EffectControlStartThread();

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
