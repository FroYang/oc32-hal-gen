/**
 ******************************************************************************
 * @file    oc32_hal_lvd.c
 * @author
 * @brief   OC32 HAL LVD (Low Voltage Detection) module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_lvd.h"

/**
 * @brief  DeInitializes the LVD peripheral.
 * @retval HAL_OK: on success
 * @note   Disables all LVD actions and clears the flag.
 */
HAL_StatusTypedef HAL_LVD_DeInit(void)
{
  LVDCON->LVDRE = HAL_DISABLE;
  LVDCON->LVDWE = HAL_DISABLE;
  LVDCON->LVDIE = HAL_DISABLE;
  LVDCON->LVDE = HAL_DISABLE;
  LVDCON->LVDECLR = HAL_ENABLE;
  return HAL_OK;
}

#ifdef HAL_LVD_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/**
 * @brief  Initializes the LVD peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_LVD_Inittypedef structure that contains
 *         the configuration information for the specified LVD peripheral.
 * @retval HAL_OK: on success
 * @note   Disables all LVD actions, sets the trigger voltage, enables LVD,
 *         clears the stale flag, then configures the action mode (reset /
 *         interrupt / wake).
 */
HAL_StatusTypedef HAL_LVD_Init(HAL_LVD_InitTypedef *Init)
{
  if (Init == NULL)
    return HAL_ERROR;
    
  /* clear all configure */
  HAL_LVD_DeInit();

  /* setup trigger voltage then enable it */
  LVDCON->LVDVP = Init->TrigV;
  LVDCON->LVDE = HAL_ENABLE;

  /* clear stale flag may caused by noise before */
  LVDCON->LVDECLR = HAL_ENABLE;

  if (Init->Mode == LVD_MODE_RST)
  {
    LVDCON->LVDRE = HAL_ENABLE;
  }
  else
  {
    if ((Init->Mode == LVD_MODE_INT) || (Init->Mode == LVD_MODE_INT_AND_WAKE))
    {
      LVDCON->LVDIE = HAL_ENABLE;
    }

    if ((Init->Mode == LVD_MODE_WAKE) || (Init->Mode == LVD_MODE_INT_AND_WAKE))
      LVDCON->LVDWE = HAL_ENABLE;
  }

  return HAL_OK;
}

/**
 * @brief  Handles LVD interrupt request at user side.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ HAL_StatusTypedef HAL_LVD_UserIRQ(void)
{
  return HAL_OK;
}

/**
 * @brief  Handles LVD interrupt request.
 * @retval Return value of HAL_LVD_UserIRQ
 * @note   Invoked from the global interrupt handler. DeInitializes the LVD,
 *         then calls HAL_LVD_UserIRQ.
 */
HAL_StatusTypedef HAL_LVD_IRQHandler(void)
{
  
  HAL_StatusTypedef status = HAL_LVD_UserIRQ();
  HAL_LVD_DeInit();
  return status;
}

#endif /* HAL_LVD_ENABLE */