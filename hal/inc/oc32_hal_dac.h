/**
 ******************************************************************************
 * @file    oc32_hal_dac.h
 * @author
 * @brief   Header file of DAC HAL module.
 *
 ******************************************************************************
 */

#ifndef __OC32_HAL_DAC_H
#define __OC32_HAL_DAC_H

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
   * @brief  DAC dma mode definition
   */
  typedef enum
  {
    DAC_DMA_ONCE = 0U, /*!< Convert once then stop */
    DAC_DMA_LOOP = 1U  /*!< Loop convert from buffer top to bottom */
  } HAL_DAC_DmaTypedef;

  /**
   * @brief  DAC input mode definition
   */
  typedef enum
  {
    DAC_IN_LAT = 0U, /*!< Latch mode */
    DAC_IN_IMM = 1U  /*!< Immediately mode */
  } HAL_DAC_InputTypedef;

  /**
   * @brief  DAC Init structure definition
   */
  typedef struct
  {
    MODULE_DAC_Typedef *Dac;   /*!< Specifies the DAC */
    uint32_t ClkDiv;           /*!< Specifies the Clock Divider */
    HAL_DAC_InputTypedef Mode; /*!< Specifies the DAC input mode */
  } HAL_DAC_InitTypedef;

  /**
   * @brief  DAC data packet structure for DMA / interrupt transfers
   */
  typedef struct
  {
    MODULE_DAC_Typedef *Dac; /*!< Specifies the DAC */
    uint16_t *Data;          /*!< DMA data buffer (SRAM). Must be aligned to a 32-bit boundary */
    uint32_t Count;          /*!< Number of times of DAC conversions. Non-DMA max 2, DMA max 1023. */
    HAL_EnableTypedef IntE;  /*!< Specifies the interrupt */
    HAL_DAC_DmaTypedef Mode; /*!< Specifies the DMA mode */
  } HAL_DAC_PacketTypedef;

  /**
   * @}
   */

  /** @addtogroup DAC
   * @{
   */
#ifdef HAL_DAC_ENABLE

  /* Initialization and de-initialization functions *************************/
  HAL_StatusTypedef HAL_DAC_Init(HAL_DAC_InitTypedef *Init);
  HAL_StatusTypedef HAL_DAC_DeInit(HAL_DAC_InitTypedef *Init);
  __sram__ HAL_StatusTypedef HAL_DAC_Convert(HAL_DAC_PacketTypedef *Packet);
  __sram__ HAL_StatusTypedef HAL_DAC_IRQHandler(MODULE_DAC_Typedef *Dac);
  __sram__ __weak__ HAL_StatusTypedef HAL_DAC_UserIRQ(MODULE_DAC_Typedef *Dac);
#endif /* HAL_DAC_ENABLE */
  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_DAC_H */
