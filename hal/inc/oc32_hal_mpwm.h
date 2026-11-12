/**
 ******************************************************************************
 * @file    oc32_hal_mpwm.h
 * @author
 * @brief   Header file of MPWM HAL module.
 *
 ******************************************************************************
 */

#ifndef __OC32_HAL_MPWM_H
#define __OC32_HAL_MPWM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

  /* extern ------------------------------------------------------------*/
  extern uint32_t DMAChState[OC32_DMA_CH_TOTAL];
  extern __sram__ HAL_StatusTypedef DmaUpdate(HAL_EnableTypedef DMAE, HAL_DMA_ChTypedef DmaCh, HAL_DMA_MapTypedef Module, HAL_DMA_StateTypedef State);

  /** @addtogroup OC32_HAL_Driver
   * @{
   */

  /* Exported types ------------------------------------------------------*/
  /**
   * @brief  MPWM mode definition
   */
  typedef enum
  {
    MPWM_MODE_PWM = 0U, /*!< PWM mode */
    MPWM_MODE_LED = 1U  /*!< LED mode */
  } HAL_MPWM_ModeTypedef;

  /**
   * @brief  MPWM base timer enum definition
   */
  typedef enum
  {
    MPWM_BASE_TIMER0 = 0U,
    MPWM_BASE_TIMER1 = 1U,
    MPWM_BASE_TIMER2 = 2U,
    MPWM_BASE_TIMER3 = 3U,
    MPWM_BASE_TIMER4 = 4U,
    MPWM_BASE_TIMER5 = 5U,
    MPWM_BASE_TIMER6 = 6U,
    MPWM_BASE_TIMER7 = 7U
  } HAL_MPWM_BaseTimerTypedef;

  /**
   * @brief  MPWM way enum definition
   */
  typedef enum
  {
    MPWM_WAY0 = 0U,
    MPWM_WAY1 = 1U,
    MPWM_WAY2 = 2U,
    MPWM_WAY3 = 3U,
    MPWM_WAY4 = 4U,
    MPWM_WAY5 = 5U,
    MPWM_WAY6 = 6U,
    MPWM_WAY7 = 7U
  } HAL_MPWM_WayTypedef;

  /**
   * @brief  MPWM Init structure definition
   */
  typedef struct
  {
    MODULE_MPWM_Typedef *Mpwm;       /*!< Specifies the MPWM */
    HAL_EnableTypedef IntE;          /*!< Specifies the interrupt mode */
    HAL_MPWM_BaseTimerTypedef Timer; /*!< Specifies the base timer */
    HAL_EnableTypedef InvOutE;       /*!< Specifies the inverted output enable */
  } HAL_MPWM_InitTypedef;

  /**
   * @brief  MPWM Way structure definition
   */
  typedef struct
  {
    MODULE_MPWM_Typedef *Mpwm; /*!< Specifies the MPWM */
    HAL_MPWM_WayTypedef Way;   /*!< Specifies the way */
    uint32_t Duty;             /*!< Specifies the duty cycle */
  } HAL_MPWM_WayDutyTypedef;

  /**
   * @brief  MPWM LED packet structure definition
   */
  typedef struct
  {
    MODULE_MPWM_Typedef *Mpwm; /*!< Specifies the MPWM */
    uint32_t *Data;            /*!< DMA data buffer (SRAM) */
    uint32_t Length;           /*!< Number of data in bytes, max 1023 bytes. */
  } HAL_MPWM_PacketTypedef;

  /**
   * @}
   */

  /** @addtogroup MPWM
   * @{
   */
#ifdef HAL_MPWM_ENABLE

  /* Initialization and de-initialization functions *************************/
  HAL_StatusTypedef HAL_MPWM_Init(HAL_MPWM_InitTypedef *Init);
  HAL_StatusTypedef HAL_MPWM_DeInit(HAL_MPWM_InitTypedef *Init);
  __sram__ HAL_StatusTypedef HAL_MPWM_UpdateDuty(HAL_MPWM_WayDutyTypedef *Way);
  __sram__ HAL_StatusTypedef HAL_MPWM_SerialLed(HAL_MPWM_PacketTypedef *Packet);
  __sram__ HAL_StatusTypedef HAL_MPWM_IRQHandler(MODULE_MPWM_Typedef *Mpwm);
  __sram__ __weak__ HAL_StatusTypedef HAL_MPWM_UserIRQ(MODULE_MPWM_Typedef *Mpwm);
#endif /* HAL_MPWM_ENABLE */
  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_MPWM_H */
