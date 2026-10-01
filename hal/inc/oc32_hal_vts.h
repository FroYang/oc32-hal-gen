/**
  ******************************************************************************
  * @file    oc32_hal_vts.h
  * @author
  * @brief   Header file of VTS (Voltage Temperature Sensor) HAL module.
  ******************************************************************************
  */

#ifndef __OC32_HAL_VTS_H
#define __OC32_HAL_VTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"
#include "CV32S6015MSR.h"

#ifdef HAL_VTS_ENABLE

/** @addtogroup OC32_HAL_Driver
  * @{
  */

/** @addtogroup VTS
  * @{
  */

typedef struct __VTS_HandleTypeDef {
    VTS_TypeDef *Instance;
    HAL_LockTypeDef Lock;
    __IO HAL_StateTypeDef State;
    void (* MspInitCallback)(struct __VTS_HandleTypeDef *hvts);
    void (* MspDeInitCallback)(struct __VTS_HandleTypeDef *hvts);
} VTS_HandleTypeDef;

HAL_StatusTypeDef HAL_VTS_Init(VTS_HandleTypeDef *hvts);
HAL_StatusTypeDef HAL_VTS_DeInit(VTS_HandleTypeDef *hvts);
void HAL_VTS_MspInit(VTS_HandleTypeDef *hvts) __weak;
void HAL_VTS_MspDeInit(VTS_HandleTypeDef *hvts) __weak;
HAL_StatusTypeDef HAL_VTS_Start(VTS_HandleTypeDef *hvts);
HAL_StatusTypeDef HAL_VTS_Stop(VTS_HandleTypeDef *hvts);
HAL_StatusTypeDef HAL_VTS_Start_IT(VTS_HandleTypeDef *hvts);
HAL_StatusTypeDef HAL_VTS_Stop_IT(VTS_HandleTypeDef *hvts);
int32_t HAL_VTS_GetTemperature(void);
void HAL_VTS_IRQHandler(VTS_HandleTypeDef *hvts);
void HAL_VTS_ConvCpltCallback(VTS_HandleTypeDef *hvts) __weak;

#define __HAL_VTS_ENABLE()      (VTS->CR |= VTS_CR_EN)
#define __HAL_VTS_DISABLE()     (VTS->CR &= ~VTS_CR_EN)
#define __HAL_VTS_ENABLE_IT()   (VTS->CR |= VTS_CR_IE)
#define __HAL_VTS_DISABLE_IT()  (VTS->CR &= ~VTS_CR_IE)
#define __HAL_VTS_GET_FLAG()    (VTS->SR & VTS_SR_RDY)
#define __HAL_VTS_CLEAR_FLAG()  (VTS->SR = ~VTS_SR_RDY)

#endif /* HAL_VTS_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_VTS_H */
