/**
 ******************************************************************************
 * @file    oc32_hal_timer.c
 * @author
 * @brief   OC32 HAL TIMER module driver.
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_timer.h"

#ifdef HAL_TIMER_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  check whether the timer address is valid
 * @param  Timer: pointer to a MODULE_TIMER_typedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Timer is not TIMER_0..TIMER_7
 */
HAL_StatusTypedef TimerModuleCheck(MODULE_TIMER_Typedef *Timer)
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
 * @brief  Initializes the TIMER peripheral in base (count) mode.
 * @param  Init: pointer to a HAL_TIMER_BaseInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or Timer is not TIMER_0..TIMER_7
 * @note   Enables the timer (TE=1) as the last step.
 */
HAL_StatusTypedef HAL_TIMER_Init(HAL_TIMER_BaseInitTypedef *Init)
{
  if (Init == NULL || TimerModuleCheck(Init->Timer) != HAL_OK)
    return HAL_ERROR;

  /* setup reload value & prescaler */
  HAL_TIMER_Update((HAL_TIMER_UpdateTypedef *)Init);
  Init->Timer->TCON.TCM = Init->CountMode;
  Init->Timer->TCON.TOFM = Init->OvfMode;
  Init->Timer->TCON.TCS = Init->CountSource;
  Init->Timer->TCON.TECC = Init->CountExCon;
  Init->Timer->TCON.TUM = Init->UpdateMode;
  Init->Timer->TECON.TACT = Init->AdcTrig;
  Init->Timer->TECON.TSBR = Init->ReadReg;
  Init->Timer->TCON.TOIE = Init->IntE;
  Init->Timer->TECON.TCSC = Init->StopCon;

  /* enable the timer */
  Init->Timer->TCON.TE = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Initializes the TIMER peripheral in encoder mode.
 * @param  Init: pointer to a HAL_TIMER_EncodeInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or Timer is not TIMER_0..TIMER_7
 */
HAL_StatusTypedef HAL_TIMER_EncodeInit(HAL_TIMER_EncodeInitTypedef *Init)
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
 * @brief  Initializes the TIMER reset feature.
 * @param  Init: pointer to a HAL_TIMER_ResetInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or Timer is not TIMER_0..TIMER_7
 */
HAL_StatusTypedef HAL_TIMER_ResetInit(HAL_TIMER_ResetInitTypedef *Init)
{
  if (Init == NULL || TimerModuleCheck(Init->Timer) != HAL_OK)
    return HAL_ERROR;

  Init->Timer->TCON.TRGE = Init->GenEventE;
  Init->Timer->TCON.TERS = Init->ExSource;
  Init->Timer->TCON.TERC = Init->ExCon;

  return HAL_OK;
}

/**
 * @brief  update the timer
 * @param  Update: pointer to a HAL_TIMER_Updatetypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Timer is not TIMER_0..TIMER_7
 * @note   Temporarily switches to user-trigger update mode, writes the new
 *         reload/prescaler/counter, triggers a user update (TUT), then restores
 *         the original update mode.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_TIMER_UpdateNow(HAL_TIMER_UpdateTypedef *Update)
{
  if (TimerModuleCheck(Update->Timer) != HAL_OK)
    return HAL_ERROR;

  /* save the current update mode */
  uint32_t updatemode = Update->Timer->TCON.TUM;
  /* switch to user trigger mode */
  Update->Timer->TCON.TUM = TIMER_UPDATE_TRIG;
  HAL_TIMER_Update(Update);
  Update->Timer->TCON.TUT = HAL_ENABLE;
  /* switch back to the original update mode */
  Update->Timer->TCON.TUM = updatemode;

  return HAL_OK;
}

/**
 * @brief  Reset the timer
 * @param  Timer: pointer to a MODULE_TIMER_typedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Timer is not TIMER_0..TIMER_7
 */
__weak__ __sram__ HAL_StatusTypedef HAL_TIMER_ResetNow(MODULE_TIMER_Typedef *Timer)
{
  if (TimerModuleCheck(Timer) != HAL_OK)
    return HAL_ERROR;

  Timer->TCON.TRS = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Initializes the TIMER pause feature.
 * @param  Init: pointer to a HAL_TIMER_PauseInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or Timer is not TIMER_0..TIMER_7
 */
HAL_StatusTypedef HAL_TIMER_PauseInit(HAL_TIMER_PauseInitTypedef *Init)
{
  if (Init == NULL || TimerModuleCheck(Init->Timer) != HAL_OK)
    return HAL_ERROR;

  Init->Timer->TECON.TPGE = Init->GenEventE;
  Init->Timer->TECON.TEPS = Init->ExSource;
  Init->Timer->TECON.TEPC = Init->ExCon;

  return HAL_OK;
}

/**
 * @brief  Initializes the TIMER event trigger feature.
 * @param  Init: pointer to a HAL_TIMER_EventInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or Timer is not TIMER_0..TIMER_7
 */
HAL_StatusTypedef HAL_TIMER_EventInit(HAL_TIMER_EventInitTypedef *Init)
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
 * @brief  Trigger an event immediately.
 * @param  Timer: pointer to a MODULE_TIMER_typedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Timer is not TIMER_0..TIMER_7
 */
__weak__ __sram__ HAL_StatusTypedef HAL_TIMER_EventTrig(MODULE_TIMER_Typedef *Timer)
{
  if (TimerModuleCheck(Timer) != HAL_OK)
    return HAL_ERROR;

  Timer->TECON.TET = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Wait timer overflow
 * @param  Timer: pointer to a MODULE_TIMER_typedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Timer is not TIMER_0..TIMER_7
 * @note   Blocks until overflow flag (TOV) is set, then clears it (TOCLR).
 *         Caller must not mask the timer interrupt during the call.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_TIMER_WaitOv(MODULE_TIMER_Typedef *Timer)
{
  if (TimerModuleCheck(Timer) != HAL_OK)
    return HAL_ERROR;

  while (Timer->TCON.TOV == HAL_FLAG_INVALID)
    ;

  Timer->TCON.TOCLR = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Wait timer event
 * @param  Timer: pointer to a MODULE_TIMER_typedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Timer is not TIMER_0..TIMER_7
 * @note   Blocks until event flag (TEF) is set, then clears it (TECLR).
 *         Caller must not mask the timer interrupt during the call.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_TIMER_WaitEv(MODULE_TIMER_Typedef *Timer)
{
  if (TimerModuleCheck(Timer) != HAL_OK)
    return HAL_ERROR;

  while (Timer->TECON.TEF == HAL_FLAG_INVALID)
    ;

  Timer->TECON.TECLR = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  DeInitializes the TIMER peripheral.
 * @param  Init: pointer to a HAL_TIMER_BaseInitTypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Timer is not TIMER_0..TIMER_7
 * @note   Resets TCON/TECON to 0, clears TARL/TCNT, and clears TOV/TEF flags.
 */
HAL_StatusTypedef HAL_TIMER_DeInit(HAL_TIMER_BaseInitTypedef *Init)
{
  if (TimerModuleCheck(Init->Timer) != HAL_OK)
    return HAL_ERROR;

  WRITE_SR(&Init->Timer->TCON, 0U);
  WRITE_SR(&Init->Timer->TECON, 0U);
  Init->Timer->TARL = 0U;
  Init->Timer->TCNT = 0U;
  Init->Timer->TCON.TOCLR = HAL_ENABLE;
  Init->Timer->TECON.TECLR = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Handles TIMER interrupt request at user side.
 * @param  Timer: pointer to a MODULE_TIMER_typedef structure.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_TIMER_UserIRQ(MODULE_TIMER_Typedef *Timer)
{
  return HAL_OK;
}

/**
 * @brief  Handles TIMER interrupt request.
 * @param  Timer: pointer to a MODULE_TIMER_typedef structure.
 * @retval Return value of HAL_TIMER_UserIRQ
 * @note   Invoked from the global interrupt handler. Clears overflow flag (TOV)
 *         and event flag (TEF) if set.
 */
__sram__ HAL_StatusTypedef HAL_TIMER_IRQHandler(MODULE_TIMER_Typedef *Timer)
{
  HAL_StatusTypedef status = HAL_TIMER_UserIRQ(Timer);

  if (Timer->TCON.TOV == HAL_FLAG_VALID)
    Timer->TCON.TOCLR = HAL_ENABLE;

  if (Timer->TECON.TEF == HAL_FLAG_VALID)
    Timer->TECON.TECLR = HAL_ENABLE;

  return status;
}

#endif /* HAL_TIMER_ENABLE */
