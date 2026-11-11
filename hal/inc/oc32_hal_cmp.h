/**
 ******************************************************************************
 * @file    oc32_hal_cmp.h
 * @author
 * @brief   Header file of CMP HAL module.
 *
 ******************************************************************************
 */

#ifndef __OC32_HAL_CMP_H
#define __OC32_HAL_CMP_H

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
 * @brief  CMP Module definition
 */
  typedef enum
  {
    CMP_0 = 0U,
    CMP_1 = 1U
  } MODULE_CMP_Typedef;

/**
 * @brief  CMP Hysteresis control definition
 */
  typedef enum
  {
    CMP_HYS_NONE = 0U,
    CMP_HYS_X1 = 1U,
    CMP_HYS_X2 = 2U,
    CMP_HYS_X4 = 3U
  } HAL_CMP_HysteresisTypedef;

/**
 * @brief  CMP input filter definition
 */
  typedef enum
  {
    CMP_FILTER_NONE = 0U,
    CMP_FILTER_X1 = 1U,
    CMP_FILTER_X2 = 2U,
    CMP_FILTER_X3 = 3U
  } HAL_CMP_FilterTypedef;

/**
 * @brief  CMP offset trim definition
 */
  typedef enum
  {
    CMP_OFFSET_TRIM_P0 = 0U,
    CMP_OFFSET_TRIM_P1 = 1U,
    CMP_OFFSET_TRIM_P2 = 2U,
    CMP_OFFSET_TRIM_P3 = 3U,  
    CMP_OFFSET_TRIM_P4 = 4U,  
    CMP_OFFSET_TRIM_P5 = 5U,
    CMP_OFFSET_TRIM_P6 = 6U,
    CMP_OFFSET_TRIM_P7 = 7U,
    CMP_OFFSET_TRIM_M0 = 8U,
    CMP_OFFSET_TRIM_M1 = 9U,
    CMP_OFFSET_TRIM_M2 = 10U,
    CMP_OFFSET_TRIM_M3 = 11U,
    CMP_OFFSET_TRIM_M4 = 12U,  
    CMP_OFFSET_TRIM_M5 = 13U,
    CMP_OFFSET_TRIM_M6 = 14U,
    CMP_OFFSET_TRIM_M7 = 15U
  } HAL_CMP_OffsetTypedef;

/**
 * @brief  CMP window timer definition
 */
  typedef enum
  {
    CMP_WIN_OFF = 0U,
    CMP_WIN_T1 = 1U,
    CMP_WIN_T2 = 2U,
    CMP_WIN_T3 = 3U,  
    CMP_WIN_T4 = 4U,  
    CMP_WIN_T5 = 5U,
    CMP_WIN_T6 = 6U,
    CMP_WIN_T7 = 7U
  } HAL_CMP_WinTimerTypedef;

/**
 * @brief  CMP input source definition
 */
  typedef enum
  {
    CMP_INPUT_0 = 0U,
    CMP_INPUT_1 = 1U,
    CMP_INPUT_2 = 2U,
    CMP_INPUT_3 = 3U,  
    CMP_INPUT_4 = 4U,  
    CMP_INPUT_5 = 5U,
    CMP_INPUT_6 = 6U,
    CMP_INPUT_7 = 7U
  } HAL_CMP_InputTypedef;

/**
 * @brief  CMP reference source definition
 */
  typedef enum
  {
    CMP_REF_0 = 0U,
    CMP_REF_1 = 1U,
    CMP_REF_2 = 2U,
    CMP_REF_3 = 3U,  
    CMP_REF_4 = 4U,  
    CMP_REF_5 = 5U,
    CMP_REF_6 = 6U,
    CMP_REF_7 = 7U
  } HAL_CMP_RefTypedef;

/**
 * @brief  CMP Init structure definition
 */
typedef struct {
    MODULE_CMP_Typedef Cmp;  /*!< Specifies the CMP */
    HAL_CMP_HysteresisTypedef Hysteresis; /*!< Specifies the Hysteresis */
    HAL_CMP_FilterTypedef Filter; /*!< Specifies the Filter */
    HAL_CMP_InputTypedef Input; /*!< Specifies the Input */
    HAL_CMP_RefTypedef Ref; /*!< Specifies the Ref */
    HAL_EnableTypedef RiseFlag; /*!< Specifies the output RiseFlag Enable */
    HAL_EnableTypedef FallFlag; /*!< Specifies the output FallFlag Enable */
    HAL_EnableTypedef OutE; /*!< Specifies the Output Enable */
    HAL_EnableTypedef IntE; /*!< Specifies the Interrupt Enable */
    HAL_CMP_OffsetTypedef Offset; /*!< Specifies the Offset */
    HAL_CMP_WinTimerTypedef WinTimer; /*!< Specifies the Timer */
    uint32_t WinStart; /*!< Specifies the Window Start value */
    uint32_t WinEnd; /*!< Specifies the Window End value */
} HAL_CMP_InitTypedef;

  /**
   * @}
   */

  /** @addtogroup CMP
   * @{
   */
#ifdef HAL_CMP_ENABLE

  
  /* Initialization and de-initialization functions *************************/
  HAL_StatusTypedef HAL_CMP_Init(HAL_CMP_InitTypedef *Init);
  HAL_StatusTypedef HAL_CMP_DeInit(HAL_CMP_InitTypedef *Init);
  __sram__ HAL_ResultTypedef HAL_CMP_OutState(MODULE_CMP_Typedef Cmp);


  /* ---- interrupt handling -------------------------------------------- */
  __sram__ HAL_StatusTypedef HAL_CMP_IRQHandler(MODULE_CMP_Typedef Cmp);
  __weak__ __sram__ HAL_StatusTypedef HAL_CMP_UserIRQ(MODULE_CMP_Typedef Cmp);

#endif /* HAL_CMP_ENABLE */

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_CMP_H */
