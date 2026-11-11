/**
 ******************************************************************************
 * @file    oc32_hal_amp.c
 * @author
 * @brief   OC32 HAL AMP (amplifier) module driver.
 ******************************************************************************
 */



/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_amp.h"

#ifdef HAL_AMP_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  DeInitializes the AMP peripheral.
 * @retval HAL_OK: on success
 * @note   Disables all AMP actions and clears the flag.
 */
HAL_StatusTypedef HAL_AMP_DeInit(HAL_AMP_InitTypedef *Init)
{
  if (Init == NULL)
    return HAL_ERROR;

  uint32_t RegVal = READ_SR(AMPCON);
  RegVal = RegVal & ~(AMP_BIT_MASK << (Init->Amp * AMP_BIT_TOTAL));
  WRITE_SR(AMPCON, RegVal);

  return HAL_OK;
}

/**
 * @brief  Initializes the AMP peripheral according to the specified parameters.
 * @param  Init: Pointer to a structure that contains the AMP configuration parameters.
 * @retval HAL_OK: on success
 */
HAL_StatusTypedef HAL_AMP_Init(HAL_AMP_InitTypedef *Init)
{
  if (Init == NULL)
    return HAL_ERROR;

  /* combine the bit values */
  uint32_t AmpVal = 0U;
  AmpVal = (Init->Gain & AMPGC_BIT) << AMPGC_POS;
  AmpVal |= HAL_ENABLE << AMPE_POS;
  AmpVal |= (Init->Offset & AMPOT_BIT) << AMPOT_POS;

  /* update the register */
  uint32_t RegVal = READ_SR(AMPCON);
  RegVal = RegVal & ~(AMP_BIT_MASK << (Init->Amp * AMP_BIT_TOTAL));
  RegVal |= AmpVal << (Init->Amp * AMP_BIT_TOTAL);
  WRITE_SR(AMPCON, RegVal);
  return HAL_OK;
}

#endif /* HAL_AMP_ENABLE */