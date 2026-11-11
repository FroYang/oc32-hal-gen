/**
 ******************************************************************************
 * @file    oc32_hal_vts.h
 * @author
 * @brief   Header file of VTS HAL module.
 *
 ******************************************************************************
 */

#ifndef __OC32_HAL_VTS_H
#define __OC32_HAL_VTS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

    /* extern ------------------------------------------------------------*/
    extern uint32_t SysReqBuf[4];

  /** @addtogroup OC32_HAL_Driver
   * @{
   */

  /* Exported types ------------------------------------------------------*/


  /**
   * @}
   */

  /** @addtogroup VTS
   * @{
   */
#ifdef HAL_VTS_ENABLE

  /* Initialization and de-initialization functions *************************/
  void HAL_VTS_Init(HAL_EnableTypedef AlarmReset);
  void HAL_VTS_DeInit(void);
  /* ---- interrupt handling -------------------------------------------- */
  HAL_StatusTypedef HAL_VTS_IRQHandler(void);
  __weak__ HAL_StatusTypedef HAL_VTS_UserIRQ(void);
#endif /* HAL_VTS_ENABLE */
  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_VTS_H */
