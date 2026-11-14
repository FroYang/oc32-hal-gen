/**
 ******************************************************************************
 * @file    oc32_hal_cmp.c
 * @author
 * @brief   OC32 HAL CMP (comparator) module driver.
 ******************************************************************************
 */



/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_cmp.h"

#ifdef HAL_CMP_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  DeInitializes the CMP peripheral.
 * @retval HAL_OK: on success
 * @note   Disables all CMP actions and clears the flag.
 */
HAL_StatusTypedef HAL_CMP_DeInit(HAL_CMP_InitTypedef *Init)
{
  if (Init == NULL)
    return HAL_ERROR;

  if (Init->Cmp == CMP_0)
  {
    WRITE_SR(CMP0TW, 0);
    WRITE_SR(CMP0CON, 0);
  }
  else if (Init->Cmp == CMP_1)
  {
    WRITE_SR(CMP1TW, 0);
    WRITE_SR(CMP1CON, 0);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;

}

/**
 * @brief  Initializes the CMP peripheral according to the specified parameters.
 * @param  Init: Pointer to a structure that contains the CMP configuration parameters.
 * @retval HAL_OK: on success
 */
HAL_StatusTypedef HAL_CMP_Init(HAL_CMP_InitTypedef *Init)
{
  if (Init == NULL)
    return HAL_ERROR;

  uint32_t RegVal = HAL_ENABLE << CMPE_POS;
  RegVal |= (Init->Hysteresis & CMPHS_BIT) << CMPHS_POS;
  RegVal |= (Init->Filter & CMPFS_BIT) << CMPFS_POS;
  RegVal |= (Init->Input & CMPIS_BIT) << CMPIS_POS;
  RegVal |= (Init->Ref & CMPRS_BIT) << CMPRS_POS;
  RegVal |= (Init->RiseFlag & CMPRFE_BIT) << CMPRFE_POS;
  RegVal |= (Init->FallFlag & CMPFFE_BIT) << CMPFFE_POS;
  RegVal |= (Init->OutE & CMPOE_BIT) << CMPOE_POS;
  RegVal |= (Init->IntE & CMPIE_BIT) << CMPIE_POS;
  RegVal |= (Init->Offset & CMPOT_BIT) << CMPOT_POS;
  RegVal |= (Init->WinTimer & CMPTM_BIT) << CMPTM_POS;

  uint32_t WinVal = (Init->WinStart & CMPTWS_BIT) << CMPTWS_POS;
  WinVal |= (Init->WinEnd & CMPTWE_BIT) << CMPTWE_POS;

  if (Init->Cmp == CMP_0)
  {
    WRITE_SR(CMP0TW, WinVal);
    WRITE_SR(CMP0CON, RegVal);
  }
  else if (Init->Cmp == CMP_1)
  {
    WRITE_SR(CMP1TW, WinVal);
    WRITE_SR(CMP1CON, RegVal);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}


/**
 * @brief  return the CMP output state
 * @retval HAL_OK: on success
 */
__sram__ HAL_ResultTypedef HAL_CMP_OutState(MODULE_CMP_Typedef Cmp)
{
  HAL_ResultTypedef Result = {0U, HAL_ERROR};
  if (Cmp == CMP_0)
  {
    Result.Data = CMP0CON->CMPS;
  }
  else if (Cmp == CMP_1)
  {
    Result.Data = CMP1CON->CMPS;
  }
  else
  {
    return Result;
  }

  Result.Status = HAL_OK;
  return Result;
}

/**
 * @brief  Handles CMP interrupt request at user side.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_CMP_UserIRQ(MODULE_CMP_Typedef Cmp)
{
  return HAL_OK;
}

/**
 * @brief  Handles CMP interrupt request.
 * @retval Return value of HAL_CMP_UserIRQ
 * @note   Invoked from the global interrupt handler. DeInitializes the CMP,
 *         then calls HAL_CMP_UserIRQ.
 */
__sram__ HAL_StatusTypedef HAL_CMP_IRQHandler(MODULE_CMP_Typedef Cmp)
{

  if (Cmp == CMP_0)
  {
    HAL_StatusTypedef status = HAL_CMP_UserIRQ(Cmp);
    CMP0CON->CMPECLR = HAL_ENABLE;
    return status;
  }
  else if (Cmp == CMP_1)
  {
    HAL_StatusTypedef status = HAL_CMP_UserIRQ(Cmp);
    CMP1CON->CMPECLR = HAL_ENABLE;
    return status;
  }
  else
  {
    return HAL_ERROR;
  }  
}

#endif /* HAL_CMP_ENABLE */