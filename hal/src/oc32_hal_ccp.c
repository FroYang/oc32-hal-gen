/**
 ******************************************************************************
 * @file    oc32_hal_timer.c
 * @author
 * @brief   OC32 HAL TIMER module driver.
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
 * @brief  check whether the timer address right
 * @param  Timer: pointer to a MODULE_TIMER_TypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef TimerModuleCheck(MODULE_TIMER_TypeDef *Timer)
{
  if (Timer != TIMER_0 && Timer != TIMER_1 &&
      Timer != TIMER_2 && Timer != TIMER_3 &&
      Timer != TIMER_4 && Timer != TIMER_5 &&
      Timer != TIMER_6 && Timer != TIMER_7)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}


/**
 * @brief  Initializes the TIMER peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_TIMER_BaseInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_TIMER_Init(HAL_TIMER_BaseInitTypeDef *Init)
{
  if (Init == NULL || TimerModuleCheck(Init->Timer) != HAL_OK)
    return HAL_ERROR;

  /* setup reload value & prescaler */
  Init->Timer->TARL = (Init->ReloadVal & 0xFFFFU) | ((Init->PreScaler & 0xFFFFU) << 16U);
  Init->Timer->TCON.TCM = Init->CountMode;
  Init->Timer->TCON.TOFM = Init->OvfMode;
  Init->Timer->TCON.TCS = Init->CountSource;
  Init->Timer->TCON.TECC = Init->CountExCon;
  Init->Timer->TCON.TUM = Init->UpdateMode;
  Init->Timer->TECON.TACT = Init->AdcTrig;
  Init->Timer->TECON.TSBR = Init->ReadReg;
  Init->Timer->TCON.TOIE = Init->IntE;
  Init->Timer->TECON.TCSC = Init->StopCon;

  Init->Timer->TCNT = 0U;

  /* enable the timer */
  Init->Timer->TCON.TE = 1U;

  return HAL_OK;
}


/**
 * @brief  Initializes the TIMER peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_TIMER_EncodeInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_TIMER_EncodeInit(HAL_TIMER_EncodeInitTypeDef *Init)
{
  if (Init == NULL || TimerModuleCheck(Init->Timer) != HAL_OK)
    return HAL_ERROR;

  Init->Timer->TCON.TENC0 = Init->Enc0Pin;
  Init->Timer->TCON.TENC1 = Init->Enc1Pin;
  Init->Timer->TCON.TENCM = Init->Mode;
  Init->Timer->TCON.TENCP = Init->Polarity;

  return HAL_OK;
}


/**
 * @brief  Initializes the TIMER peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_TIMER_ResetInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_TIMER_ResetInit(HAL_TIMER_ResetInitTypeDef *Init)
{
  if (Init == NULL || TimerModuleCheck(Init->Timer) != HAL_OK)
    return HAL_ERROR;

  Init->Timer->TCON.TRGE = Init->GenEvent;
  Init->Timer->TCON.TERS = Init->ExSource;
  Init->Timer->TCON.TERC = Init->ExCon;

  return HAL_OK;
}


/**
 * @brief  Reset the timer
 * @param  Timer: pointer to a MODULE_TIMER_TypeDef structure.
 * @retval HAL status
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_ResetNow(MODULE_TIMER_TypeDef *Timer)
{
  if (TimerModuleCheck(Timer) != HAL_OK)
    return HAL_ERROR;

  Timer->TCON.TRS = 1U;

  return HAL_OK;
}


/**
 * @brief  Initializes the TIMER peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_TIMER_EncodeInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_TIMER_PauseInit(HAL_TIMER_PauseInitTypeDef *Init)
{
  if (Init == NULL || TimerModuleCheck(Init->Timer) != HAL_OK)
    return HAL_ERROR;

  Init->Timer->TECON.TPGE = Init->GenEvent;
  Init->Timer->TECON.TEPS = Init->ExSource;
  Init->Timer->TECON.TEPC = Init->ExCon;

  return HAL_OK;
}


/**
 * @brief  Initializes the TIMER peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_TIMER_EventInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_TIMER_EventInit(HAL_TIMER_EventInitTypeDef *Init)
{
  if (Init == NULL || TimerModuleCheck(Init->Timer) != HAL_OK)
    return HAL_ERROR;

  Init->Timer->TECON.TPEC = Init->OvfPost & 0xFFU;
  Init->Timer->TECON.TEIE = Init->IntE;
  Init->Timer->TECON.TETS = Init->Source;
  Init->Timer->TECON.TETC = Init->ExCon;
  
  return HAL_OK;
}


/**
 * @brief  Trigger an event
 * @param  Timer: pointer to a MODULE_TIMER_TypeDef structure.
 * @retval HAL status
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_EventTrig(MODULE_TIMER_TypeDef *Timer)
{
  if (TimerModuleCheck(Timer) != HAL_OK)
    return HAL_ERROR;

  Timer->TECON.TET = 1U;

  return HAL_OK;
}


/**
 * @brief  Wait timer overflow
 * @param  Timer: pointer to a MODULE_TIMER_TypeDef structure.
 * @retval HAL status
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_WaitOv(MODULE_TIMER_TypeDef *Timer)
{
  if (TimerModuleCheck(Timer) != HAL_OK)
    return HAL_ERROR;

  while (!Timer->TCON.TOV)
    ;

  Timer->TCON.TOCLR = 1U;  

  return HAL_OK;
}


/**
 * @brief  Wait timer event
 * @param  Timer: pointer to a MODULE_TIMER_TypeDef structure.
 * @retval HAL status
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_WaitEv(MODULE_TIMER_TypeDef *Timer)
{
  if (TimerModuleCheck(Timer) != HAL_OK)
    return HAL_ERROR;

  while (!Timer->TECON.TEF)
    ;

  Timer->TECON.TECLR = 1U;  

  return HAL_OK;
}

/**
 * @brief  DeInitializes the TIMER peripheral.
 * @param  TIMERCON: pointer to a HAL_TIMER_BaseInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_TIMER_DeInit(MODULE_TIMER_TypeDef *Timer)
{
  if (TimerModuleCheck(Timer) != HAL_OK)
    return HAL_ERROR;

  *(volatile uint32_t *)&Timer->TCON = 0U;
  *(volatile uint32_t *)&Timer->TECON = 0U;
  Timer->TARL = 0U;
  Timer->TCNT = 0U;
  Timer->TCON.TOCLR = 1U;
  Timer->TECON.TECLR = 1U;

  return HAL_OK;
}



/**
 * @brief  Handles TIMER interrupt request.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_UserIRQ(MODULE_TIMER_TypeDef *Timer)
{
  return HAL_OK;
}

/**
 * @brief  Handles TIMER interrupt request.
 */
__weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_IRQHandler(MODULE_TIMER_TypeDef *Timer)
{
  if (Timer->TCON.TOV)
    Timer->TCON.TOCLR = 1U;

  if (Timer->TECON.TEF)
    Timer->TECON.TECLR = 1U;

  return HAL_TIMER_UserIRQ(Timer);
}