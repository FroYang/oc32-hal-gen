/**
 ******************************************************************************
 * @file    oc32_hal_lvd.c
 * @author
 * @brief   OC32 HAL LVD (Low Voltage Detection) module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal.h"

#ifdef HAL_LVD_ENABLE

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the LVD peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_LVDInitTypeDef structure that contains
 *         the configuration information for the specified LVD peripheral.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_LVD_Init(HAL_LVDInitTypeDef *Init)
{
  /* clear all configure */
  LVDCON->LVDRE = 0U;
  LVDCON->LVDWE = 0U;
  LVDCON->LVDIE = 0U;
  LVDCON->LVDE = 0U;

  /* setup trigger voltage then enable it */
  LVDCON->LVDVP = Init->TrigV;
  LVDCON->LVDE = 1U;

  /* clear stale flag may caused by noise before */
  LVDCON->LVDECLR = 1U;

  if (Init->Mode == LVD_MODE_RST)
  {
    LVDCON->LVDRE = 1U;
  }
  else
  {
    if ((Init->Mode == LVD_MODE_INT) || (Init->Mode == LVD_MODE_INT_AND_WAKE))
    {
      LVDCON->LVDIE = 1U;
      HIE->SYSHIE = 1U;

      /* highest interrupt priority always */
      HIPL0->SYSHIPL0 = 1U; /* Bit 0 of priority */
      HIPL1->SYSHIPL1 = 1U; /* Bit 1 of priority */
    }

    if ((Init->Mode == LVD_MODE_WAKE) || (Init->Mode == LVD_MODE_INT_AND_WAKE))
      LVDCON->LVDWE = 1U;
  }

  return HAL_OK;
}

/**
 * @brief  DeInitializes the LVD peripheral.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_LVD_DeInit(void)
{
  LVDCON->LVDRE = 0U;
  LVDCON->LVDWE = 0U;
  LVDCON->LVDIE = 0U;
  HIE->SYSHIE = 0U;
  HIPL0->SYSHIPL0 = 0U;
  HIPL1->SYSHIPL1 = 0U;
  LVDCON->LVDE = 0U;
  LVDCON->LVDECLR = 1U;
  return HAL_OK;
}

#endif /* HAL_LVD_ENABLE */

/**
 * @brief  Handles LVD interrupt request.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ void HAL_LVD_IRQHandler()
{
  if (HRF->LVDRF)
    HRF->LVDRFCLR = 1U;

  if (LVDCON->LVDEF)
    LVDCON->LVDECLR = 1U;
}