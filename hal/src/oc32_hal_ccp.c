/**
 ******************************************************************************
 * @file    oc32_hal_ccp.c
 * @author
 * @brief   OC32 HAL CCP module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the CCP peripheral according to the specified parameters.
 * @param  CCP: pointer to a MODULE_CCP_TypeDef structure.
 * @retval capture value
 */
static inline uint32_t HAL_CCP_CaptureValue(MODULE_CCP_TypeDef *CCP)
{
  /* setup reload value */
  return CCP->CCPVAL.CCPCV;
}

/**
 * @brief  Initializes the CCP peripheral according to the specified parameters.
 * @param  Update: pointer to a HAL_CCP_UpdateTypeDef structure.
 * @retval HAL status
 */
static inline void HAL_CCP_Update(HAL_CCP_UpdateTypeDef *Update)
{
  /* setup reload value */
  Update->CCP->CCPVAL.CCPCV = (Update->Duty & 0xFFFFU);
  Update->CCP->CCPGAT.CCPGSV = (Update->GateStart & 0xFFFFU);
  Update->CCP->CCPGAT.CCPGEV = (Update->GateEnd & 0xFFFFU);
  return;
}

/**
 * @brief  check whether the ccp address right
 * @param  CCP: pointer to a MODULE_CCP_TypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef CCPModuleCheck(MODULE_CCP_TypeDef *CCP)
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
 * @brief  Initializes the CCP peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_CCP_BaseInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CCP_Init(HAL_CCP_BaseInitTypeDef *Init)
{
  if (Init == NULL || CCPModuleCheck(Init->CCP) != HAL_OK)
    return HAL_ERROR;

  /* setup the ccp */
  Init->CCP->CCPCON.CCPM = Init->Mode;
  Init->CCP->CCPCON.CCPT = Init->Timer;
  Init->CCP->CCPCON.CCPUM = Init->UpdateMode;
  Init->CCP->CCPOUT.CCPSBR = Init->ReadReg;

  /* enable the ccp */
  Init->CCP->CCPCON.CCPE = 1U;

  return HAL_OK;
}

/**
 * @brief  Initializes the CCP peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_CCP_CaptureInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CCP_CaptureInit(HAL_CCP_CaptureInitTypeDef *Init)
{
  if (Init == NULL || CCPModuleCheck(Init->CCP) != HAL_OK)
    return HAL_ERROR;

  Init->CCP->CCPCON.CCPCIC = Init->CapCon;
  Init->CCP->CCPCON.CCPCI = Init->CapIn;
  Init->CCP->CCPVAL.CCPPSC = Init->Prescaler;

  return HAL_OK;
}

/**
 * @brief  Initializes the CCP peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_CCP_CompareInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CCP_CompareInit(HAL_CCP_CompareInitTypeDef *Init)
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
 * @brief  Initializes the CCP peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_CCP_PwmInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CCP_PwmInit(HAL_CCP_PwmInitTypeDef *Init)
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
 * @brief  Initializes the CCP peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_CCP_GateInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CCP_GateInit(HAL_CCP_GateInitTypeDef *Init)
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
 * @brief  Initializes the CCP peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_CCP_ForceInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CCP_ForceInit(HAL_CCP_ForceInitTypeDef *Init)
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
 * @param  CCP: pointer to a MODULE_CCP_TypeDef structure.
 * @retval HAL status
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_CCP_HandleCov(MODULE_CCP_TypeDef *CCP)
{
  return HAL_OK;
}

/**
 * @brief  Wait ccp capture/compare/pwm flag
 * @param  CCP: pointer to a MODULE_CCP_TypeDef structure.
 * @retval HAL status
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_CCP_WaitCf(MODULE_CCP_TypeDef *CCP)
{
  if (CCPModuleCheck(CCP) != HAL_OK)
    return HAL_ERROR;

  while (!CCP->CCPCON.CCPCF)
    ;

  if (CCP->CCPCON.CCPCOV)
    HAL_CCP_HandleCov(CCP);

  CCP->CCPCON.CCPCCLR = 1U;
  return HAL_OK;
}

/**
 * @brief  Wait ccp gate flag
 * @param  CCP: pointer to a MODULE_CCP_TypeDef structure.
 * @retval HAL status
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_CCP_WaitGf(MODULE_CCP_TypeDef *CCP)
{
  if (CCPModuleCheck(CCP) != HAL_OK)
    return HAL_ERROR;

  while (!CCP->CCPCON.CCPGF)
    ;

  CCP->CCPCON.CCPGCLR = 1U;

  return HAL_OK;
}

/**
 * @brief  Wait ccp force flag
 * @param  CCP: pointer to a MODULE_CCP_TypeDef structure.
 * @retval HAL status
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_CCP_WaitFf(MODULE_CCP_TypeDef *CCP)
{
  if (CCPModuleCheck(CCP) != HAL_OK)
    return HAL_ERROR;

  while (!CCP->CCPCON.CCPFF)
    ;

  CCP->CCPCON.CCPFCLR = 1U;

  return HAL_OK;
}

/**
 * @brief  update the ccp duty and gate value
 * @param  Update: pointer to a HAL_CCP_UpdateTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CCP_UpdateNow(HAL_CCP_UpdateTypeDef *Update)
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
 * @param  CCP: pointer to a MODULE_CCP_TypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CCP_DeInit(MODULE_CCP_TypeDef *CCP)
{
  if (CCPModuleCheck(CCP) != HAL_OK)
    return HAL_ERROR;

  *(volatile uint32_t *)&CCP->CCPCON = 0U;
  *(volatile uint32_t *)&CCP->CCPVAL = 0U;
  *(volatile uint32_t *)&CCP->CCPOUT = 0U;
  *(volatile uint32_t *)&CCP->CCPGAT = 0U;
  CCP->CCPCON.CCPCCLR = 1U;
  CCP->CCPCON.CCPGCLR = 1U;
  CCP->CCPCON.CCPFCLR = 1U;

  return HAL_OK;
}

/**
 * @brief  Handles CCP interrupt request.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_CCP_UserIRQ(MODULE_CCP_TypeDef *CCP)
{
  return HAL_OK;
}

/**
 * @brief  Handles CCP interrupt request.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_CCP_IRQHandler(MODULE_CCP_TypeDef *CCP)
{
  HAL_StatusTypeDef status = HAL_CCP_UserIRQ(CCP);

  if ((CCP->CCPCON.CCPCF) || (CCP->CCPCON.CCPCOV))
    CCP->CCPCON.CCPCCLR = 1U;

  if (CCP->CCPCON.CCPGF)
    CCP->CCPCON.CCPGCLR = 1U;

  if (CCP->CCPCON.CCPFF)
    CCP->CCPCON.CCPFCLR = 1U;

  return status;
}