/**
  ******************************************************************************
  * @file    oc32_hal_wdt.c
  * @author
  * @brief   OC32 HAL WDT (Watchdog Timer) module driver.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal.h"

#ifdef HAL_WDT_ENABLE

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the WDT peripheral according to the specified parameters.
  * @param  hwdt: pointer to a WDT_HandleTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_WDT_Init(WDT_HandleTypeDef *hwdt)
{
    /* TODO: Implement WDT initialization
     * 1. Validate WDT handle
     * 2. Configure WDT prescaler
     * 3. Configure WDT auto-reload value
     * 4. Enable WDT
     */

    hwdt->State = HAL_WDT_STATE_READY;

    /* Initialize the WDT MSP */
    HAL_WDT_MspInit(hwdt);

    return HAL_OK;
}

/**
  * @brief  DeInitializes the WDT peripheral.
  * @param  hwdt: pointer to a WDT_HandleTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_WDT_DeInit(WDT_HandleTypeDef *hwdt)
{
    /* TODO: Implement WDT de-initialization
     * 1. Disable WDT
     * 2. Reset WDT registers to default
     */

    hwdt->State = HAL_WDT_STATE_RESET;

    /* DeInitialize the WDT MSP */
    HAL_WDT_MspDeInit(hwdt);

    return HAL_OK;
}

/**
  * @brief  Initializes the WDT MSP.
  * @param  hwdt: pointer to a WDT_HandleTypeDef structure.
  * @note   This is a weak implementation that can be overridden by the user.
  */
__weak void HAL_WDT_MspInit(WDT_HandleTypeDef *hwdt)
{
    /* NOTE: This function should be implemented in the user file.
     * TODO: Implement WDT MSP initialization
     */
}

/**
  * @brief  DeInitializes the WDT MSP.
  * @param  hwdt: pointer to a WDT_HandleTypeDef structure.
  * @note   This is a weak implementation that can be overridden by the user.
  */
__weak void HAL_WDT_MspDeInit(WDT_HandleTypeDef *hwdt)
{
    /* NOTE: This function should be implemented in the user file.
     * TODO: Implement WDT MSP de-initialization
     */
}

/**
  * @brief  Starts the WDT.
  * @param  hwdt: pointer to a WDT_HandleTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_WDT_Start(WDT_HandleTypeDef *hwdt)
{
    /* TODO: Start WDT
     * Enable WDT by writing key value to WDT control register
     */
    __HAL_WDT_START(hwdt);

    return HAL_OK;
}

/**
  * @brief  Stops the WDT.
  * @param  hwdt: pointer to a WDT_HandleTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_WDT_Stop(WDT_HandleTypeDef *hwdt)
{
    /* TODO: Stop WDT
     * Disable WDT
     */
    __HAL_WDT_DISABLE(hwdt);

    return HAL_OK;
}

/**
  * @brief  Refreshes the WDT.
  * @param  hwdt: pointer to a WDT_HandleTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_WDT_Refresh(WDT_HandleTypeDef *hwdt)
{
    /* TODO: Refresh (reload) WDT counter
     * Write reload key to WDT control register
     */
    __HAL_WDT_RELOAD(hwdt);

    return HAL_OK;
}

/**
  * @brief  Handles WDT interrupt request.
  * @param  hwdt: pointer to a WDT_HandleTypeDef structure.
  */
void HAL_WDT_IRQHandler(WDT_HandleTypeDef *hwdt)
{
    /* TODO: Handle WDT interrupt
     * 1. Check for WDT timeout flag
     * 2. Clear flag
     * 3. Call timeout callback
     */
}

/**
  * @brief  WDT timeout callback.
  * @param  hwdt: pointer to a WDT_HandleTypeDef structure.
  * @note   This is a weak implementation that can be overridden by the user.
  */
__weak void HAL_WDT_TimeoutCallback(WDT_HandleTypeDef *hwdt)
{
    /* NOTE: This function should be implemented in the user file.
     * TODO: Implement WDT timeout callback
     */
}

#endif /* HAL_WDT_ENABLE */
