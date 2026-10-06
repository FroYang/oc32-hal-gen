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
     * @brief  RCM Oscillator type structure definition
     */
    typedef enum
    {
        RCM_OSC_ILOSC = 0x0U,
        RCM_OSC_IHOSC = 0x1U,
        RCM_OSC_XLOSC = 0x2U,
        RCM_OSC_XHOSC = 0x3U
    } HAL_RCMOscTypedef;


    /**
     * @brief  RCM Init configuration structure definition
     */
    typedef struct
    {
        HAL_RCMOscTypedef OscType;  /*!< osc */
        uint32_t OutFreq;           /*!< output freq */
    } HAL_RCMInitTypeDef;

    /**
     * @brief  RCM Oscillator trim definition
     */
    typedef enum
    {
        RCM_OSC_TRIM_OFF = 0x0U,
        RCM_OSC_TRIM_ON = 0x1U,
    } HAL_RCMOscTrimTypedef;

    /**
     * @brief  RCM Oscillator trim lock definition
     */
    typedef enum
    {
        RCM_OSC_TRIM_LOCK_OFF = 0x0U,
        RCM_OSC_TRIM_LOCK_ON = 0x1U,
    } HAL_RCMOscTrimLockTypedef;


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
#define RCM_CLOCKSWITCH_TIMEOUT ((uint32_t)100U)
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
    __sram__ HAL_StatusTypeDef HAL_RCM_Init(HAL_RCMInitTypeDef *Init);
    __weak__ void HAL_PLL_IRQHandler(void);

#ifdef HAL_USB_ENABLE
    HAL_StatusTypeDef HAL_RCM_UsbTrim(HAL_RCMOscTrimTypedef Trim);
    HAL_StatusTypeDef HAL_RCM_UsbTrimLock(HAL_RCMOscTrimLockTypedef Lock);
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
