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
  * @param  Mode: Mode
  * @param  TrigV: Trigger voltage
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LVD_Init(HAL_LVDModeTypedef Mode, HAL_LVDTrigVTypedef TrigV)
{
    /* clear all configure */
    LVDCON = 0;

    /* setup trigger voltage then enable it */
    LVDCON->LVDVP = TrigV;
    LVDCON->LVDE = 1U;

    /* clear stale flag may caused by noise before */
    LVDCON->LVDECLR = 1U;

    /* setup control */
    WRITE_SR(LVDCON, (Mode << 1U) | 1U);

    return HAL_OK;
}

/**
  * @brief  DeInitializes the LVD peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LVD_DeInit(void)
{
    LVDCON = 0;
    LVDCON->LVDECLR = 1U;    
    return HAL_OK;
}

#endif /* HAL_LVD_ENABLE */
