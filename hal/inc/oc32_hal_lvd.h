/**
 ******************************************************************************
 * @file    oc32_hal_lvd.h
 * @author
 * @brief   Header file of LVD HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_LVD_H
#define __OC32_HAL_LVD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"
#include "CV32S6015MSR.h"

#ifdef HAL_LVD_ENABLE

  /** @addtogroup OC32_HAL_Driver
   * @{
   */

  /** @defgroup LVD_Trigger_Voltage
   * @{
   */
  typedef enum
  {
    LVD_TRIG_V2400 = 0U,
    LVD_TRIG_V2500 = 1U,
    LVD_TRIG_V2600 = 2U,
    LVD_TRIG_V2700 = 3U,
    LVD_TRIG_V2800 = 4U,
    LVD_TRIG_V2900 = 5U,
    LVD_TRIG_V3000 = 6U,
    LVD_TRIG_V3100 = 7U,
    LVD_TRIG_V3300 = 8U,
    LVD_TRIG_V3500 = 9U,
    LVD_TRIG_V3700 = 10U,
    LVD_TRIG_V3900 = 11U,
    LVD_TRIG_V4100 = 12U,
    LVD_TRIG_V4300 = 13U,
    LVD_TRIG_V4500 = 14U,
    LVD_TRIG_V4700 = 15U
  } HAL_LVDTrigVTypedef;

  /**
   * @}
   */

  /** @defgroup LVD_Mode
   * @{
   */
  typedef enum
  {
    LVD_MODE_NONE = 0x0U,
    LVD_MODE_RST = 0x1U,
    LVD_MODE_WAKE = 0x2U,
    LVD_MODE_INT = 0x4U
  } HAL_LVDModeTypedef;

  /**
   * @}
   */

  /** @addtogroup LVD
   * @{
   */

  HAL_StatusTypeDef HAL_LVD_Init(HAL_LVDModeTypedef Mode, HAL_LVDTrigVTypedef TrigV);
  HAL_StatusTypeDef HAL_LVD_DeInit(void);

  /**
   * @}
   */

  /* Exported constants --------------------------------------------------*/

  /** @defgroup LVD_Exported_Constants LVD Exported Constants
   * @{
   */

  /**
   * @}
   */

#endif /* HAL_LVD_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_LVD_H */
