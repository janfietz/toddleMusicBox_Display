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

/* addition driver */

#include "iwdg_driver.h"

#include "board_drivers.h"

#include "chprintf.h"
#include "shell.h"

#include "float.h"
#include "math.h"
#include <string.h>

#include "usbcfg.h"

#include <stdlib.h>

#include "ledconf.h"
#include "effect_control.h"
#include "ws281x.h"
#include "mfrc522.h"

#define LED_ORANGE GPIOD_LED3
#define LED_GREEN GPIOD_LED4
#define LED_BLUE GPIOD_LED6
#define LED_RED GPIOD_LED5

#define MFRC522_RESET GPIOC_PIN5

#define USE_WDG FALSE


/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;
ws2811Driver ws2811;

MFRC522Driver RFID1;

static const IWDGConfig IWDGcfg = {
    .counter = 0x0A,
    .div = IWDG_DIV_32,
};

void SetUpdateLed(void)
{
    ws2811Update(&ws2811);
}

void SetLedColor(uint16_t led, const struct Color* color)
{
    ws2811SetColor(&ws2811, led, color);
}

static uint8_t txbuf[2];
static uint8_t rxbuf[2];

static MUTEX_DECL(CardIDMutex);
static struct MifareUID CardID;

void MFRC522WriteRegister(MFRC522Driver* mfrc522p, uint8_t addr, uint8_t val)
{
    (void)mfrc522p;
    spiSelect(&SPID1);
    txbuf[0] = (addr << 1) & 0x7E;
    txbuf[1] = val;
    spiSend(&SPID1, 2, txbuf);
    spiUnselect(&SPID1);
}

uint8_t MFRC522ReadRegister(MFRC522Driver* mfrc522p, uint8_t addr)
{
    (void)mfrc522p;
    spiSelect(&SPID1);
    txbuf[0] = ((addr << 1) & 0x7E) | 0x80;
    txbuf[1] = 0xff;
    spiExchange(&SPID1, 2, txbuf, rxbuf);
    spiUnselect(&SPID1);
    return rxbuf[1];
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

static void cmd_showcarduid(BaseSequentialStream *chp, int argc, char *argv[])
{


  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: rfidshow\r\n");
    return;
  }

  static struct MifareUID uid;
  chMtxLock(&CardIDMutex);
  uid.size = CardID.size;
  memcpy(uid.bytes, CardID.bytes, sizeof(CardID.bytes));
  chMtxUnlock(&CardIDMutex);
  uint8_t i;
  for(i = 0; i < CardID.size; i++)
  {
      chprintf(chp, "%02X", CardID.bytes[i]);
  }
  chprintf(chp, "[%d]", CardID.size);
  chprintf(chp, "\r\n");

}

static void cmd_playmode(BaseSequentialStream *chp, int argc, char *argv[])
{
  if (argc != 1) {
    chprintf(chp, "Usage: mode <0|1|2|3>\r\n");
    return;
  }

  EffectControlNewPlayMode((uint8_t)atoi(argv[0]));

  chprintf(chp, "\r\n");

}

static void cmd_nexteffect(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argv;
    if (argc > 0) {
      chprintf(chp, "Usage: nexteffect\r\n");
      return;
    }

    EffectControlNextEffect();
    chprintf(chp, "\r\n");
}

static void cmd_noeffect(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argv;
    if (argc > 0) {
      chprintf(chp, "Usage: noeffect\r\n");
      return;
    }

    EffectControlNoEffect();
    chprintf(chp, "\r\n");
}

static void cmd_hidecontrols(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argv;
    if (argc > 0) {
      chprintf(chp, "Usage: hidecontrols\r\n");
      return;
    }

    EffectControlHideControls();
    chprintf(chp, "\r\n");
}

static void cmd_showcontrols(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argv;
    if (argc > 0) {
      chprintf(chp, "Usage: showcontrols\r\n");
      return;
    }

    EffectControlShowControls();
    chprintf(chp, "\r\n");
}

static void cmd_volume(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc != 1) {
        chprintf(chp, "Usage: volume <0-100>\r\n");
        return;
    }

    EffectControlVolume((uint8_t)atoi(argv[0]));

    chprintf(chp, "\r\n");
}

static const ShellCommand commands[] = {
  {"mem", cmd_mem},
  {"threads", cmd_threads},
  {"uid", cmd_showcarduid},
  {"playmode", cmd_playmode},
  {"nexteffect", cmd_nexteffect},
  {"noeffect", cmd_noeffect},
  {"hidecontrols", cmd_hidecontrols},
  {"showcontrols", cmd_showcontrols},
  {"volume", cmd_volume},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};

/*
 * This is a periodic thread that reads uid from rfid periphal
 */
static THD_WORKING_AREA(waThread1, 256);
static THD_FUNCTION(Thread1, arg) {

  systime_t time;

  (void)arg;
  chRegSetThreadName("reader");

  /* Reader thread loop.*/
  time = chVTGetSystemTime();
  bool active = false;
  palClearPad(GPIOD, LED_ORANGE);

  while (true) {
      if (active == true)
      {
          palClearPad(GPIOD, LED_ORANGE);
      }
      else
      {
          palSetPad(GPIOD, LED_ORANGE);
      }
      active = !active;

      chMtxLock(&CardIDMutex);
      if (MifareCheck(&RFID1, &CardID) == MIFARE_OK) {
          palSetPad(GPIOD, LED_GREEN);
      } else {
          palClearPad(GPIOD, LED_GREEN);
          CardID.size = 0;
      }
      chMtxUnlock(&CardIDMutex);


    /* Waiting until the next 250 milliseconds time interval.*/
    chThdSleepUntil(time += MS2ST(100));
  }
}

/*
 * This is a periodic thread that reads uid from rfid periphal
 */
#if USE_WDG
static THD_WORKING_AREA(waWDGThread1, 128);
static THD_FUNCTION(WDGThread, arg) {

  (void)arg;
  chRegSetThreadName("watchdog");

  iwdgStart(&IWDGD, &IWDGcfg);
  while (true) {

    iwdgReset(&IWDGD);

    chThdSleep(MS2ST(5));
  }
}
#endif
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
    iwdgInit();

    chSysInit();

    /*
    * Creates watchdog.
    */
#if USE_WDG
    chThdCreateStatic(waWDGThread1, sizeof(waWDGThread1), NORMALPRIO, WDGThread, NULL);
#endif
    /*
     * Shell manager initialization.
     */
    shellInit();

    palSetPadMode(GPIOC, MFRC522_RESET, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUDR_FLOATING);
    BoardDriverInit();

    BoardDriverStart();

    /*
     * Creates the example thread.
     */
    EffectControlInitThread();
    EffectControlStartThread();

    /*
    * Creates rfid reader.
    */
    chThdCreateStatic(waThread1, sizeof(waThread1),
                    LOWPRIO, Thread1, NULL);

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
