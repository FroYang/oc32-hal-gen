/**
 ******************************************************************************
 * @file    oc32_hal_amp.h
 * @author
 * @brief   Header file of AMP HAL module.
 *
 ******************************************************************************
 */

#ifndef __OC32_HAL_AMP_H
#define __OC32_HAL_AMP_H

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

  /* Exported types ------------------------------------------------------*/

/**
 * @brief  AMP Module definition
 */
  typedef enum
  {
    AMP_0 = 0U,
    AMP_1 = 1U,
    AMP_2 = 2U
  } MODULE_AMP_Typedef;

/**
 * @brief  AMP Gain control definition
 */
  typedef enum
  {
    AMP_GAIN_OPEN = 0U,
    AMP_GAIN_X1 = 1U,
    AMP_GAIN_X2 = 2U,
    AMP_GAIN_X4 = 3U,
    AMP_GAIN_X8 = 4U,
    AMP_GAIN_X16 = 5U,
    AMP_GAIN_X32 = 6U,
    AMP_GAIN_X64 = 7U
  } HAL_AMP_GainTypedef;

/**
 * @brief  AMP offset trim definition
 */
  typedef enum
  {
    AMP_OFFSET_TRIM_NONE = 0U,
    AMP_OFFSET_TRIM_1 = 1U,
    AMP_OFFSET_TRIM_2 = 2U,
    AMP_OFFSET_TRIM_3 = 3U,
    AMP_OFFSET_TRIM_4 = 4U,
    AMP_OFFSET_TRIM_5 = 5U,
    AMP_OFFSET_TRIM_6 = 6U,
    AMP_OFFSET_TRIM_7 = 7U
  } HAL_AMP_OffsetTypedef;

/**
 * @brief  AMP Init structure definition
 */
typedef struct {
    MODULE_AMP_Typedef Amp;  /*!< Specifies the AMP */
    HAL_AMP_GainTypedef Gain; /*!< Specifies the Gain */
    HAL_AMP_OffsetTypedef Offset; /*!< Specifies the Offset */
} HAL_AMP_InitTypedef;

  /**
   * @}
   */

  /** @addtogroup AMP
   * @{
   */
#ifdef HAL_AMP_ENABLE

  /* Initialization and de-initialization functions *************************/
  HAL_StatusTypedef HAL_AMP_Init(HAL_AMP_InitTypedef *Init);
  HAL_StatusTypedef HAL_AMP_DeInit(HAL_AMP_InitTypedef *Init);
#endif /* HAL_AMP_ENABLE */
  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_AMP_H */
