/**
 * @file    board_drivers.c
 * @brief
 *
 * @{
 */

#include "board_drivers.h"
#include "ledconf.h"

#include "ch.h"
#include "hal.h"

#include "ws281x.h"
#include "usbcfg.h"
#include "mfrc522.h"

extern ws281xDriver ws281x;
extern SerialUSBDriver SDU1;
extern MFRC522Driver RFID1;

static ws281xConfig ws281x_cfg =
{
    LEDCOUNT,
    LED1,
    {
        12000000,
        WS2812_BIT_PWM_WIDTH,
        NULL,
        {
            { PWM_OUTPUT_ACTIVE_HIGH, NULL },
            { PWM_OUTPUT_ACTIVE_HIGH, NULL },
            { PWM_OUTPUT_ACTIVE_HIGH, NULL },
            { PWM_OUTPUT_ACTIVE_HIGH, NULL }
        },
        0,
        TIM_DIER_UDE | TIM_DIER_CC1DE,
    },
    &PWMD3,
    0,
    WS2812_ZERO_PWM_WIDTH,
    WS2812_ONE_PWM_WIDTH,
    STM32_DMA1_STREAM4,
    5
};

static MFRC522Config RFID1_cfg =
{

};

/*
 * SPI1 configuration structure.
 * Speed 5.25MHz, CPHA=1, CPOL=1, 8bits frames, MSb transmitted first.
 * The slave select line is the pin GPIOA_LRCK on the port GPIOA.
 */
static const SPIConfig SPI1cfg = {
  NULL,
  /* HW dependent part.*/
  GPIOC,
  GPIOC_PIN5,
  SPI_CR1_BR_0 | SPI_CR1_BR_1
};

void BoardDriverInit(void)
{
    sduObjectInit(&SDU1);
    ws281xObjectInit(&ws281x);
    MFRC522ObjectInit(&RFID1);
}

void BoardDriverStart(void)
{
    spiStart(&SPID1, &SPI1cfg);
    MFRC522Start(&RFID1, &RFID1_cfg);

    ws281xStart(&ws281x, &ws281x_cfg);
    palSetPadMode(GPIOB, 4, PAL_STM32_OSPEED_HIGHEST | PAL_MODE_ALTERNATE(2));

    /*
     * Initializes a serial-over-USB CDC driver.
     */
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
}

void BoardDriverShutdown(void)
{
    sduStop(&SDU1);
    ws281xStop(&ws281x);

    MFRC522Stop(&RFID1);
    spiStop(&SPID1);
}

/** @} */
