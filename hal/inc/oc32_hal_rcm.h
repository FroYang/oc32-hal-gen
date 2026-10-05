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
extern "C" {
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
 * @brief  RCM Oscillator configuration structure definition
 */
typedef struct {
    uint32_t OscType;  /*!< IOSC / XHOSC */
    uint32_t IoscHigh;  /*!< High / Low Speed */
    uint32_t PllState;  /*!< On / Off */
    uint32_t PllMode;  /*!< Fast lock / Low jitter */
    uint32_t PllFM;   /*!< Factor of Multiply: 0~255 */
    uint32_t PllFD;   /*!< Factor of Divide: 2^n, n=0~7 */

#ifdef HAL_RCM_CDR_ENABLE
    uint32_t PllRef;  /*!< OSC / USB CDR */
    uint32_t CdrTrim; /*!< On / Off */
#endif
} HAL_RCMInitTypeDef;




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

/** @defgroup RCM_Oscillator_Type Oscillator Type
 * @{
 */
#define RCM_OSCTYPE_IOSC  ((uint32_t)0x00000000U)
#define RCM_OSCTYPE_XHOSC  ((uint32_t)0x00000001U)
/**
 * @}
 */

/** @defgroup RCM_Oscillator_Type Oscillator Type
 * @{
 */
#define RCM_IOSCSPEED_LOW  ((uint32_t)0x00000000U)
#define RCM_IOSCSPEED_HIGH  ((uint32_t)0x00000001U)
/**
 * @}
 */ 

/** @defgroup RCM_PLL_Config PLL Config
 * @{
 */
#define RCM_PLL_OFF ((uint32_t)0x00000000U)
#define RCM_PLL_ON  ((uint32_t)0x00000001U)
/**
 * @}
 */


/** @defgroup RCM_PLL_Mode PLL Mode
 * @{
 */
#define RCM_PLLMODE_LOWJITTER ((uint32_t)0x00000000U)
#define RCM_PLLMODE_FASTLOCK ((uint32_t)0x00000001U)
/**
 * @}
 */

#ifdef HAL_RCM_CDR_ENABLE
/** @defgroup RCM_PLL_Clock_Reference PLL Clock Reference
 * @{
 */
#define RCM_PLLREF_OSC ((uint32_t)0x00000000U)
#define RCM_PLLREF_CDR ((uint32_t)0x00000001U)
/**
 * @}
 */
#endif

#ifdef HAL_USB_ENABLE
/** @defgroup RCM_CDR_Trim CDR Trim
 * @{
 */
#define RCM_USBTRIM_OFF ((uint32_t)0x00000000U)
#define RCM_USBTRIM_ON ((uint32_t)0x00000001U)
/**
 * @}
 */


/** @defgroup RCM_TRIM_Lock Trim Lock
 * @{
 */
#define RCM_TRIMLOCK_ON ((uint32_t)0x00000000U)
#define RCM_TRIMLOCK_OFF ((uint32_t)0x00000001U)
/**
 * @}
 */
#endif

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
 *   FLMCG  – Flash interface clock mask  (bit 15)
 *   ADCMCG – ADC clock mask               (bit 18)
 *   DACMCG – DAC clock mask               (bit 20)
 *   USBMCG – USB clock mask               (bit 17)
 *   CRCMCG – CRC clock mask               (bit 16)
 *
 * Masks are active-low: clearing a bit enables the clock, setting it disables.
 * For peripherals without dedicated mask bits, these macros are no-ops.
 * ------------------------------------------------------------------------ */

/* Peripherals without individual clock gates (always clocked by HCLK) */

/* Peripherals with dedicated CLKCON mask bits */
/* FLASH clock gate (FLMCG) */
#define __HAL_RCM_FLASH_CLK_ENABLE()   (CLKCON->FLMCG = 0)
#define __HAL_RCM_FLASH_CLK_DISABLE()  (CLKCON->FLMCG = 1)

/* ADC clock gate (ADCMCG) */
#define __HAL_RCM_ADC_CLK_ENABLE()     (CLKCON->ADCMCG = 0)
#define __HAL_RCM_ADC_CLK_DISABLE()    (CLKCON->ADCMCG = 1)

/* DAC clock gate (DACMCG) */
#define __HAL_RCM_DAC_CLK_ENABLE()     (CLKCON->DACMCG = 0)
#define __HAL_RCM_DAC_CLK_DISABLE()    (CLKCON->DACMCG = 1)

/* USB clock gate (USBMCG) */
#define __HAL_RCM_USB_CLK_ENABLE()     (CLKCON->USBMCG = 0)
#define __HAL_RCM_USB_CLK_DISABLE()    (CLKCON->USBMCG = 1)

/* CRC clock gate (CRCMCG) */
#define __HAL_RCM_CRC_CLK_ENABLE()     (CLKCON->CRCMCG = 0)
#define __HAL_RCM_CRC_CLK_DISABLE()    (CLKCON->CRCMCG = 1)


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

#ifdef HAL_USB_ENABLE
HAL_StatusTypeDef HAL_RCM_TrimLockOn(void);
HAL_StatusTypeDef HAL_RCM_TrimLockOff(void);
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
