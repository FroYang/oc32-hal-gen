/**
 ******************************************************************************
 * @file    oc32_hal_vts.c
 * @author
 * @brief   OC32 HAL VTS (Voltage Temperature Sensor) module driver.
 ******************************************************************************
 */




/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_vts.h"
#include "oc32_hal_static.h"

#ifdef HAL_VTS_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/** VTS table index */
extern uint32_t VtIndex;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
 * @brief  DeInitializes the VTS peripheral.
 * @retval HAL_OK: on success
 * @note   Disables all VTS actions and clears the flag.
 */
void HAL_VTS_DeInit(void)
{
  ALMCON->VTSARE = HAL_DISABLE;
  VTSCON->VTSE = HAL_DISABLE;
  VtIndex = 0U;
}

/**
 * @brief  Initializes the VTS peripheral according to the specified parameters.
 * @param  AlarmReset: Specifies the alarm reset enable.
 * @retval HAL_OK: on success
 */
void HAL_VTS_Init(HAL_EnableTypedef AlarmReset)
{
  ALMCON->VTSARE = AlarmReset;
  VTSCON->VTSE = HAL_ENABLE;
  VtIndex = 0U;
}


/**
 * @brief  converts the VTS output to a temperature value by using the ADC.
 * @retval HAL_OK: on success
 */
HAL_ResultTypedef HAL_VTS_GetTemp(void)
{
  HAL_ResultTypedef Result = {0U, HAL_OK};
  VtIndex = 0U;

  /* read value from adc */

  /* convert to temperature using the table */
  HAL_SYS_GetVtTable();

  /* calculate temperature */
  Result.Data = SysReqBuf[0] >> (VtIndex & 0x3U);
  Result.Data &= 0x000000FFU;
  return Result;
}

/**
 * @brief  Handles VTS interrupt request at user side.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ HAL_StatusTypedef HAL_VTS_UserIRQ(void)
{
  return HAL_OK;
}

/**
 * @brief  Handles VTS interrupt request.
 * @retval Return value of HAL_VTS_UserIRQ
 * @note   Invoked from the global interrupt handler. DeInitializes the VTS,
 *         then calls HAL_VTS_UserIRQ.
 */
HAL_StatusTypedef HAL_VTS_IRQHandler(void)
{
  HAL_VTS_DeInit();
  return HAL_VTS_UserIRQ();
}

#endif /* HAL_VTS_ENABLE */