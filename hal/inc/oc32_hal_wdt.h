/**
  ******************************************************************************
  * @file    oc32_hal_wdt.h
  * @author
  * @brief   Header file of WDT HAL module.
  ******************************************************************************
  */

#ifndef __OC32_HAL_WDT_H
#define __OC32_HAL_WDT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"
#include "CV32S6015MSR.h"

#ifdef HAL_WDT_ENABLE

/** @addtogroup OC32_HAL_Driver
  * @{
  */

/** @addtogroup WDT
  * @{
  */

/* Exported types ------------------------------------------------------*/

/**
  * @brief  WDT Init Structure definition
  */
typedef struct {
    uint32_t Prescaler;   /*!< Watchdog prescaler */
    uint32_t ReloadValue; /*!< Watchdog reload value */
    uint32_t Window;      /*!< Watchdog window value */
} WDT_InitTypeDef;

/**
  * @brief  WDT Handle Structure definition
  */
typedef struct __WDT_HandleTypeDef {
    WDT_TypeDef *Instance;
    WDT_InitTypeDef Init;
    HAL_LockTypeDef Lock;
    __IO HAL_StateTypeDef State;
    __IO uint32_t ErrorCode;
    void (* MspInitCallback)(struct __WDT_HandleTypeDef *hw) __weak;
    void (* MspDeInitCallback)(struct __WDT_HandleTypeDef *hw) __weak;
} WDT_HandleTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------*/
#define WDT_TIMEOUT_VALUE  ((uint32_t)50000U)

/**
  * @}
  */

/* Exported macros -------------------------------------------------------*/
#define __HAL_WDT_START()     (WDT->CR = WDT_CR_START)
#define __HAL_WDT_RELOAD()    (WDT->CR = WDT_CR_RELOAD)
#define __HAL_WDT_ENABLE()    (WDT->CR |= WDT_CR_EN)
#define __HAL_WDT_DISABLE()   (WDT->CR &= ~WDT_CR_EN)
#define __HAL_WDT_GET_FLAG()  (WDT->SR & WDT_SR_RF)

/* Exported functions ----------------------------------------------------*/
HAL_StatusTypeDef HAL_WDT_Init(WDT_HandleTypeDef *hwdt);
HAL_StatusTypeDef HAL_WDT_DeInit(WDT_HandleTypeDef *hwdt);
void HAL_WDT_MspInit(WDT_HandleTypeDef *hwdt) __weak;
void HAL_WDT_MspDeInit(WDT_HandleTypeDef *hwdt) __weak;
HAL_StatusTypeDef HAL_WDT_Start(WDT_HandleTypeDef *hwdt);
HAL_StatusTypeDef HAL_WDT_Stop(WDT_HandleTypeDef *hwdt);
HAL_StatusTypeDef HAL_WDT_Refresh(WDT_HandleTypeDef *hwdt);
void HAL_WDT_IRQHandler(WDT_HandleTypeDef *hwdt);
void HAL_WDT_TimeoutCallback(WDT_HandleTypeDef *hwdt) __weak;

/**
  * @}
  */

#endif /* HAL_WDT_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_WDT_H */
