/**
 ******************************************************************************
 * @file    oc32_hal_ccp.c
 * @author
 * @brief   OC32 HAL CCP module driver.
 ******************************************************************************
 */



/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_ccp.h"

#ifdef HAL_CCP_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  check whether the ccp address is valid
 * @param  CCP: pointer to a MODULE_CCP_typedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if CCP is not CCP_0..CCP_5
 */
__sram__ HAL_StatusTypedef CCPModuleCheck(MODULE_CCP_Typedef *CCP)
{
  if (CCP != CCP_0 && CCP != CCP_1 &&
      CCP != CCP_2 && CCP != CCP_3 &&
      CCP != CCP_4 && CCP != CCP_5)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
 * @brief  Initializes the CCP peripheral in base mode.
 * @param  Init: pointer to a HAL_CCP_BaseInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or CCP is not CCP_0..CCP_5
 * @note   Enables the CCP (CCPE=1) as the last step.
 */
HAL_StatusTypedef HAL_CCP_Init(HAL_CCP_BaseInitTypedef *Init)
{
  if (Init == NULL || CCPModuleCheck(Init->CCP) != HAL_OK)
    return HAL_ERROR;

  /* setup the ccp */
  Init->CCP->CCPCON.CCPIE = Init->IntE;
  Init->CCP->CCPCON.CCPM = Init->Mode;
  Init->CCP->CCPCON.CCPT = Init->Timer;
  Init->CCP->CCPCON.CCPUM = Init->UpdateMode;
  Init->CCP->CCPOUT.CCPSBR = Init->ReadReg;

  /* enable the ccp */
  Init->CCP->CCPCON.CCPE = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Initializes the CCP peripheral in capture mode.
 * @param  Init: pointer to a HAL_CCP_CaptureInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or CCP is not CCP_0..CCP_5
 */
HAL_StatusTypedef HAL_CCP_CaptureInit(HAL_CCP_CaptureInitTypedef *Init)
{
  if (Init == NULL || CCPModuleCheck(Init->CCP) != HAL_OK)
    return HAL_ERROR;

  Init->CCP->CCPCON.CCPCIC = Init->CapCon;
  Init->CCP->CCPCON.CCPCI = Init->CapIn;
  Init->CCP->CCPVAL.CCPPSC = Init->Prescaler;

  return HAL_OK;
}

/**
 * @brief  Initializes the CCP peripheral in compare mode.
 * @param  Init: pointer to a HAL_CCP_CompareInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or CCP is not CCP_0..CCP_5
 */
HAL_StatusTypedef HAL_CCP_CompareInit(HAL_CCP_CompareInitTypedef *Init)
{
  if (Init == NULL || CCPModuleCheck(Init->CCP) != HAL_OK)
    return HAL_ERROR;

  Init->CCP->CCPVAL.CCPCV = Init->ComVal;
  Init->CCP->CCPOUT.CCPROC = Init->ComCon;
  Init->CCP->CCPOUT.CCPCOL = Init->ComOut;
  Init->CCP->CCPOUT.CCPPOE = Init->PosOutE;
  Init->CCP->CCPOUT.CCPPIO = Init->PosOutInv;
  Init->CCP->CCPOUT.CCPNOE = Init->NegOutE;
  Init->CCP->CCPOUT.CCPNIO = Init->NegOutInv;

  return HAL_OK;
}

/**
 * @brief  Initializes the CCP peripheral in PWM mode.
 * @param  Init: pointer to a HAL_CCP_PwmInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or CCP is not CCP_0..CCP_5
 */
HAL_StatusTypedef HAL_CCP_PwmInit(HAL_CCP_PwmInitTypedef *Init)
{
  if (Init == NULL || CCPModuleCheck(Init->CCP) != HAL_OK)
    return HAL_ERROR;

  Init->CCP->CCPVAL.CCPCV = Init->Duty;
  Init->CCP->CCPVAL.CCPDT = Init->DeadTime;
  Init->CCP->CCPOUT.CCPPFM = Init->PwmCon;
  Init->CCP->CCPOUT.CCPPOE = Init->PosOutE;
  Init->CCP->CCPOUT.CCPPIO = Init->PosOutInv;
  Init->CCP->CCPOUT.CCPNOE = Init->NegOutE;
  Init->CCP->CCPOUT.CCPNIO = Init->NegOutInv;

  return HAL_OK;
}

/**
 * @brief  Initializes the CCP peripheral in gate mode.
 * @param  Init: pointer to a HAL_CCP_GateInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or CCP is not CCP_0..CCP_5
 */
HAL_StatusTypedef HAL_CCP_GateInit(HAL_CCP_GateInitTypedef *Init)
{
  if (Init == NULL || CCPModuleCheck(Init->CCP) != HAL_OK)
    return HAL_ERROR;

  Init->CCP->CCPGAT.CCPGSV = Init->GateStart & 0xFFFFU;
  Init->CCP->CCPGAT.CCPGEV = Init->GateEnd & 0xFFFFU;
  Init->CCP->CCPOUT.CCPGM = Init->GateMode;
  Init->CCP->CCPOUT.CCPGUE = Init->GateUpCountE;
  Init->CCP->CCPOUT.CCPGDE = Init->GateDownCountE;
  Init->CCP->CCPOUT.CCPGINV = Init->GateInv;
  Init->CCP->CCPOUT.CCPGFM = Init->GateFlag;

  return HAL_OK;
}

/**
 * @brief  Initializes the CCP peripheral in force mode.
 * @param  Init: pointer to a HAL_CCP_ForceInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL, CCP is not CCP_0..CCP_5, or ForceChannel is invalid
 */
HAL_StatusTypedef HAL_CCP_ForceInit(HAL_CCP_ForceInitTypedef *Init)
{
  if (Init == NULL || CCPModuleCheck(Init->CCP) != HAL_OK)
    return HAL_ERROR;

  if (Init->ForceChannel == CCP_FORCE_CH0)
  {
    Init->CCP->CCPCON.CCPFC0I = Init->ForceInSource;
    Init->CCP->CCPCON.CCPFC0L = Init->ForceCon;
    Init->CCP->CCPCON.CCPFC0E = Init->ForceE;
  }
  else if (Init->ForceChannel == CCP_FORCE_CH1)
  {
    Init->CCP->CCPCON.CCPFC1I = Init->ForceInSource;
    Init->CCP->CCPCON.CCPFC1L = Init->ForceCon;
    Init->CCP->CCPCON.CCPFC1E = Init->ForceE;
  }
  else
  {
    return HAL_ERROR;
  }

  Init->CCP->CCPCON.CCPVFE = Init->ForceLvdE;
  Init->CCP->CCPOUT.CCPFNO = Init->ForceNoState;
  Init->CCP->CCPOUT.CCPFPO = Init->ForcePoState;

  return HAL_OK;
}

/**
 * @brief  handle capture overflow
 * @param  CCP: pointer to a MODULE_CCP_typedef structure.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_CCP_HandleCov(MODULE_CCP_Typedef *CCP)
{
  return HAL_OK;
}

/**
 * @brief  Wait ccp capture/compare/pwm flag
 * @param  CCP: pointer to a MODULE_CCP_typedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if CCP is not CCP_0..CCP_5
 * @note   Blocks until the CCP flag (CCPCF) is set. If capture overflow (CCPCOV)
 *         occurred, calls HAL_CCP_HandleCov before clearing the flag (CCPCCLR).
 *         This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_CCP_WaitCf(MODULE_CCP_Typedef *CCP)
{
  if (CCPModuleCheck(CCP) != HAL_OK)
    return HAL_ERROR;

  while (!CCP->CCPCON.CCPCF)
    ;

  if (CCP->CCPCON.CCPCOV)
    HAL_CCP_HandleCov(CCP);

  CCP->CCPCON.CCPCCLR = HAL_ENABLE;
  return HAL_OK;
}

/**
 * @brief  Wait ccp gate flag
 * @param  CCP: pointer to a MODULE_CCP_typedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if CCP is not CCP_0..CCP_5
 * @note   Blocks until the gate flag (CCPGF) is set, then clears it (CCPGCLR).
 *         This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_CCP_WaitGf(MODULE_CCP_Typedef *CCP)
{
  if (CCPModuleCheck(CCP) != HAL_OK)
    return HAL_ERROR;

  while (!CCP->CCPCON.CCPGF)
    ;

  CCP->CCPCON.CCPGCLR = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Wait ccp force flag
 * @param  CCP: pointer to a MODULE_CCP_typedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if CCP is not CCP_0..CCP_5
 * @note   Blocks until the force flag (CCPFF) is set, then clears it (CCPFCLR).
 *         This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_CCP_WaitFf(MODULE_CCP_Typedef *CCP)
{
  if (CCPModuleCheck(CCP) != HAL_OK)
    return HAL_ERROR;

  while (CCP->CCPCON.CCPFF == HAL_FLAG_INVALID)
    ;

  CCP->CCPCON.CCPFCLR = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  update the ccp duty and gate value
 * @param  Update: pointer to a HAL_CCP_Updatetypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if CCP is not CCP_0..CCP_5
 * @note   Temporarily switches to user-trigger update mode, writes the new
 *         duty/gate values, triggers a user update (CCPUT), then restores
 *         the original update mode.
 */
__weak__ HAL_StatusTypedef HAL_CCP_UpdateNow(HAL_CCP_UpdateTypedef *Update)
{
  if (CCPModuleCheck(Update->CCP) != HAL_OK)
    return HAL_ERROR;

  /* save the current update mode */
  uint32_t updatemode = Update->CCP->CCPCON.CCPUM;
  /* switch to user trigger mode */
  Update->CCP->CCPCON.CCPUM = CCP_UPDATE_TRIG;
  HAL_CCP_Update(Update);
  Update->CCP->CCPCON.CCPUT = 1U;
  /* switch back to the original update mode */
  Update->CCP->CCPCON.CCPUM = updatemode;

  return HAL_OK;
}

/**
 * @brief  DeInitializes the CCP peripheral.
 * @param  CCP: pointer to a MODULE_CCP_typedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if CCP is not CCP_0..CCP_5
 * @note   Resets all CCP registers to 0, then clears capture/gate/force flags.
 */
HAL_StatusTypedef HAL_CCP_DeInit(MODULE_CCP_Typedef *CCP)
{
  if (CCPModuleCheck(CCP) != HAL_OK)
    return HAL_ERROR;

  WRITE_SR(&CCP->CCPCON, 0U);
  WRITE_SR(&CCP->CCPVAL, 0U);
  WRITE_SR(&CCP->CCPOUT, 0U);
  WRITE_SR(&CCP->CCPGAT, 0U);
  CCP->CCPCON.CCPCCLR = HAL_ENABLE;
  CCP->CCPCON.CCPGCLR = HAL_ENABLE;
  CCP->CCPCON.CCPFCLR = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Handles CCP interrupt request at user side.
 * @param  CCP: pointer to a MODULE_CCP_typedef structure.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_CCP_UserIRQ(MODULE_CCP_Typedef *CCP)
{
  return HAL_OK;
}

/**
 * @brief  Handles CCP interrupt request.
 * @param  CCP: pointer to a MODULE_CCP_typedef structure.
 * @retval Return value of HAL_CCP_UserIRQ
 * @note   Invoked from the global interrupt handler. Clears capture/overflow flag
 *         (CCPCF/CCPCOV → CCPCCLR), gate flag (CCPGF → CCPGCLR), and force flag
 *         (CCPFF → CCPFCLR) if set.
 */
__sram__ HAL_StatusTypedef HAL_CCP_IRQHandler(MODULE_CCP_Typedef *CCP)
{
  HAL_StatusTypedef status = HAL_CCP_UserIRQ(CCP);

  if ((CCP->CCPCON.CCPCF == HAL_FLAG_VALID) || (CCP->CCPCON.CCPCOV == HAL_FLAG_VALID))
    CCP->CCPCON.CCPCCLR = HAL_ENABLE;

  if (CCP->CCPCON.CCPGF == HAL_FLAG_VALID)
    CCP->CCPCON.CCPGCLR = HAL_ENABLE;

  if (CCP->CCPCON.CCPFF == HAL_FLAG_VALID)
    CCP->CCPCON.CCPFCLR = HAL_ENABLE;

  return status;
}

#endif /* HAL_CCP_ENABLE */
