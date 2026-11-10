/**
 ******************************************************************************
 * @file    oc32_hal_rcm.h
 * @author
 * @brief   Header file of RCM (Reset and Clock Manager) HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_RCM_H
#define __OC32_HAL_RCM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

    /** @addtogroup OC32_HAL_Driver
     * @{
     */

    /** @addtogroup RCM
     * @{
     */

    /* Exported types ------------------------------------------------------*/



/**
 * @}
 */

/* Exported constants --------------------------------------------------*/

/** @defgroup RCM_Exported_Constants RCM Exported Constants
 * @{
 */

/** @defgroup RCM_ClockSwitch_timeout
 * @{
 */
/**
 * RCM_ClockSwitchWait() busy-loop timeout constant.
 *
 * Number of volatile CSF poll iterations before returning HAL_TIMEOUT.
 * Scaled for the oscillator the system clock can still run on when
 * RCM_ClockSwitchWait() is invoked from HAL_RCM_Init(), i.e. before
 * HAL_TICK_Init() has started the WDT tick (uwTick == 0).
 *
 * Nominal target:
 *   @ 24 MHz (IHOSC)  ~200 ms   (the wait always begins on IHOSC, before
 *   any source switch, so the 24 MHz figure is the sizing reference)
 *
 * 200 ms comfortably covers a normal PLL/oscillator switch; the legacy
 * tick-based RCM_CLOCKSWITCH_TIMEOUT (100 ticks at 1 kHz = 100 ms) was
 * unusable before the tick interrupt was up and running, and this value
 * supersedes it as a software safety net against a stuck CSF.
 */
#define RCM_CSWAIT_LOOP_MAX ((uint32_t)4800000U)
/**
 * @}
 */


/**
 * @}
 */

/* Exported macros -------------------------------------------------------*/

/** @defgroup RCM_Exported_Macros RCM Exported Macros
 * @{
 */

/* ------------------------------------------------------------------------
 * OC32 CLKCON-based clock enable/disable macros
 *
 * OC32 does not have per-peripheral clock gates like STM32's RCC (APB2ENR/APB1ENR).
 * The CLKCON register only provides functional-block mask bits:
 *   FLMCG  – Flashloader Module Clock Gating       (bit 15)
 *   CRCMCG – CRC Module Clock Gating               (bit 16)
 *   USBMCG – USB Module Clock Gating               (bit 17)
 *   ADCMCG – ADC Module Clock Gating               (bit 18)
 *   DACMCG – DAC Module Clock Gating               (bit 20)
 *
 * Masks are active-low: clearing a bit enables the clock, setting it disables.
 * For peripherals without dedicated mask bits, these macros are no-ops.
 * ------------------------------------------------------------------------ */

/* Peripherals without individual clock gates (always clocked by SCLK) */

/* Peripherals with dedicated CLKCON mask bits */
/* FLASH clock gate (FLMCG) */
#define __HAL_RCM_FLASH_CLK_ENABLE() (CLKCON->FLMCG = 0)
#define __HAL_RCM_FLASH_CLK_DISABLE() (CLKCON->FLMCG = 1)

/* ADC clock gate (ADCMCG) */
#define __HAL_RCM_ADC_CLK_ENABLE() (CLKCON->ADCMCG = 0)
#define __HAL_RCM_ADC_CLK_DISABLE() (CLKCON->ADCMCG = 1)

/* DAC clock gate (DACMCG) */
#define __HAL_RCM_DAC_CLK_ENABLE() (CLKCON->DACMCG = 0)
#define __HAL_RCM_DAC_CLK_DISABLE() (CLKCON->DACMCG = 1)

/* USB clock gate (USBMCG) */
#define __HAL_RCM_USB_CLK_ENABLE() (CLKCON->USBMCG = 0)
#define __HAL_RCM_USB_CLK_DISABLE() (CLKCON->USBMCG = 1)

/* CRC clock gate (CRCMCG) */
#define __HAL_RCM_CRC_CLK_ENABLE() (CLKCON->CRCMCG = 0)
#define __HAL_RCM_CRC_CLK_DISABLE() (CLKCON->CRCMCG = 1)

    /**
     * @}
     */

    /* Exported functions ----------------------------------------------------*/

    /** @addtogroup RCM_Exported_Functions
     * @{
     */

    /* Initialization and de-initialization functions */
    HAL_StatusTypeDef HAL_RCM_DeInit(void);
    HAL_StatusTypeDef HAL_RCM_Init(void);
    HAL_StatusTypeDef HAL_PLL_IRQHandler(void);
    __weak__ HAL_StatusTypeDef HAL_PLL_UserIRQ(void);

#ifdef HAL_USB_ENABLE
    HAL_StatusTypeDef HAL_RCM_UsbTrim(HAL_EnableTypeDef Trim);
    HAL_StatusTypeDef HAL_RCM_UsbTrimLock(HAL_EnableTypeDef Lock);
#endif

    /* Frequency query functions */
    uint32_t HAL_RCM_GetSysClockFreq(void);

    /**
     * @}
     */

    /* Private macros --------------------------------------------------------*/
    /** @defgroup RCM_Private_Macros RCM Private Macros
     * @{
     */

    /**
     * @}
     */

    /**
     * @}
     */

    /**
     * @}
     */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_RCM_H */
