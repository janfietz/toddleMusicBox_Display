
/**
 * @file    ws2811.h
 * @brief   WS2811 Driver macros and structures.
 *
 * @addtogroup WS2811
 * @{
 */

#ifndef _WS2811_H_
#define _WS2811_H_

#if HAL_USE_WS2811 || defined(__DOXYGEN__)

#include "led.h"
#include "color.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/
#define WS2811_BIT_PWM_WIDTH 210 //2.5 us
#define WS2811_ZERO_PWM_WIDTH 41 //0.5 us
#define WS2811_ONE_PWM_WIDTH 100 //1.2 us

#define WS2812_BIT_PWM_WIDTH 105
#define WS2812_ZERO_PWM_WIDTH 30
#define WS2812_ONE_PWM_WIDTH 59

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/
/**
 * @brief   WS2811 interrupt priority level setting.
 */
#if !defined(WS2811_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define WS2811_IRQ_PRIORITY         10
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/


/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  WS2811_UNINIT = 0,                   /**< Not initialized.                   */
  WS2811_STOP = 1,                     /**< Stopped.                           */
  WS2811_READY = 2,                    /**< Ready.                             */
  WS2811_ACTIVE = 3,                   /**< Active.                            */
} ws2811state_t;
/**
 * @brief   Type of a structure representing an ws2811Driver driver.
 */
typedef struct ws2811Driver ws2811Driver;
/**
 * @brief   Driver configuration structure.
 * @note    It could be empty on some architectures.
 */
typedef struct {
  uint16_t ledCount;
  struct LedSetting* ledSettings;
  uint8_t portmask;
  uint32_t periphalset;
  uint32_t periphalclear;
  uint32_t bitwidth;
  uint32_t zerowidth;
  uint32_t onewidth;
  PWMConfig pwmMasterConfig;
  PWMDriver *pwmMaster;
  PWMConfig pwmConfig;
  PWMDriver *pwmd;
  stm32_dma_stream_t *dmastp_reset;
  stm32_dma_stream_t *dmastp_one;
  stm32_dma_stream_t *dmastp_zero;
} ws2811Config;


/**
 * @brief   Structure representing an ws2811 driver.
 */
struct ws2811Driver {
  /**
   * @brief   Driver state.
   */
	ws2811state_t                state;
  /**
   * @brief   Current configuration data.
   */
  const ws2811Config           *config;
  /* End of the mandatory fields.*/
  uint8_t dma_source;
  uint8_t *framebuffer;
  struct Color* colors;
  bool updateframebuffer;
  binary_semaphore_t bsemUpdateFrame;
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
  void ws2811Init(void);
  void ws2811ObjectInit(ws2811Driver *ws2811p);
  void ws2811Start(ws2811Driver *ws2811p, const ws2811Config *config);
  void ws2811Stop(ws2811Driver *ws2811p);
  void ws2811SetColor(ws2811Driver *ws2811p, int ledNum, const struct Color *color);
  void ws2811Update(ws2811Driver *ws2811p);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_ws2811 */

#endif /* _WS2811_H_ */

/** @} */
