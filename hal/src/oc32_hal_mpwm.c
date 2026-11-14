/**
 ******************************************************************************
 * @file    oc32_hal_mpwm.c
 * @author
 * @brief   OC32 HAL MPWM (multi-PWM / serial-LED driver) module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_mpwm.h"

#ifdef HAL_MPWM_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static HAL_DMA_ChTypedef Mpwm0DmaCh = 0U;
static HAL_DMA_ChTypedef Mpwm1DmaCh = 1U;
static HAL_DMA_ChTypedef Mpwm2DmaCh = 2U;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  DeInitializes the MPWM peripheral.
 * @retval HAL_OK: on success
 * @note   Disables all MPWM actions and clears the flag.
 */
HAL_StatusTypedef HAL_MPWM_DeInit(HAL_MPWM_InitTypedef *Init)
{
  if ((Init == NULL) || ((Init->Mpwm != MPWM_0) && (Init->Mpwm != MPWM_1) && (Init->Mpwm != MPWM_2)))
    return HAL_ERROR;

  WRITE_SR(&Init->Mpwm->MPCON, 0U);
  HAL_DMA_ChTypedef DmaCh = (Init->Mpwm == MPWM_0) ? Mpwm0DmaCh : (Init->Mpwm == MPWM_1) ? Mpwm1DmaCh
                                                                                         : Mpwm2DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Init->Mpwm == MPWM_0) ? OC32_DMA_MPWM0 : (Init->Mpwm == MPWM_1) ? OC32_DMA_MPWM1
                                                                                                  : OC32_DMA_MPWM2;

  Init->Mpwm->MPCON.MPW0CLR = HAL_ENABLE;
  Init->Mpwm->MPCON.MPW1CLR = HAL_ENABLE;
  Init->Mpwm->MPCON.MPW2CLR = HAL_ENABLE;
  Init->Mpwm->MPCON.MPW3CLR = HAL_ENABLE;
  Init->Mpwm->MPCON.MPW4CLR = HAL_ENABLE;
  Init->Mpwm->MPCON.MPW5CLR = HAL_ENABLE;
  Init->Mpwm->MPCON.MPW6CLR = HAL_ENABLE;
  Init->Mpwm->MPCON.MPW7CLR = HAL_ENABLE;

  return DmaUpdate(HAL_DISABLE, DmaCh, DmaModule, OC32_DMA_IDLE);
}

/**
 * @brief  Initializes the MPWM peripheral according to the specified parameters.
 * @param  Init: Pointer to a structure that contains the MPWM configuration parameters.
 * @retval HAL_OK: on success
 */
HAL_StatusTypedef HAL_MPWM_Init(HAL_MPWM_InitTypedef *Init)
{
  if ((Init == NULL) || ((Init->Mpwm != MPWM_0) && (Init->Mpwm != MPWM_1) && (Init->Mpwm != MPWM_2)))
    return HAL_ERROR;

  Init->Mpwm->MPCON.MPIE = Init->IntE;
  Init->Mpwm->MPCON.MPT = Init->Timer;
  Init->Mpwm->MPCON.MPIO = Init->InvOutE;

  return HAL_OK;
}

/**
 * @brief  Update way duty cycle
 * @param  Way: pointer to a HAL_MPWM_WayDutyTypedef structure
 * @retval HAL_OK: on success (in both polling and interrupt mode)
 * @retval HAL_ERROR: on failure
 */
__sram__ HAL_StatusTypedef HAL_MPWM_UpdateDuty(HAL_MPWM_WayDutyTypedef *Way)
{
  if ((Way == NULL) || ((Way->Mpwm != MPWM_0) && (Way->Mpwm != MPWM_1) && (Way->Mpwm != MPWM_2)))
    return HAL_ERROR;

  /* update duty cycle */
  switch (Way->Way)
  {
  case MPWM_WAY0:
    Way->Mpwm->MPDTY01.MPW0DTY = Way->Duty;
    Way->Mpwm->MPCON.MPW0E = HAL_ENABLE;
    break;

  case MPWM_WAY1:
    Way->Mpwm->MPDTY01.MPW1DTY = Way->Duty;
    Way->Mpwm->MPCON.MPW1E = HAL_ENABLE;
    break;

  case MPWM_WAY2:
    Way->Mpwm->MPDTY23.MPW2DTY = Way->Duty;
    Way->Mpwm->MPCON.MPW2E = HAL_ENABLE;
    break;

  case MPWM_WAY3:
    Way->Mpwm->MPDTY23.MPW3DTY = Way->Duty;
    Way->Mpwm->MPCON.MPW3E = HAL_ENABLE;
    break;

  case MPWM_WAY4:
    Way->Mpwm->MPDTY45.MPW4DTY = Way->Duty;
    Way->Mpwm->MPCON.MPW4E = HAL_ENABLE;
    break;

  case MPWM_WAY5:
    Way->Mpwm->MPDTY45.MPW5DTY = Way->Duty;
    Way->Mpwm->MPCON.MPW5E = HAL_ENABLE;
    break;

  case MPWM_WAY6:
    Way->Mpwm->MPDTY67.MPW6DTY = Way->Duty;
    Way->Mpwm->MPCON.MPW6E = HAL_ENABLE;
    break;

  case MPWM_WAY7:
    Way->Mpwm->MPDTY67.MPW7DTY = Way->Duty;
    Way->Mpwm->MPCON.MPW7E = HAL_ENABLE;
    break;

  default:
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
 * @brief  send led packet using way0 and way1
 * @param  Packet: pointer to a HAL_MPWM_PacketTypedef structure
 * @retval HAL_OK: on success (in both polling and interrupt mode)
 * @retval HAL_ERROR: on failure
 * @note   User should setup timer base time meet serial led pulse duration in specification
 *         to ensure the led pulse duration is correct.
 */
__sram__ HAL_StatusTypedef HAL_MPWM_SerialLed(HAL_MPWM_PacketTypedef *Packet)
{
  if ((Packet == NULL) || ((Packet->Mpwm != MPWM_0) && (Packet->Mpwm != MPWM_1) && (Packet->Mpwm != MPWM_2)))
    return HAL_ERROR;

  HAL_DMA_ChTypedef DmaCh = (Packet->Mpwm == MPWM_0) ? Mpwm0DmaCh : (Packet->Mpwm == MPWM_1) ? Mpwm1DmaCh
                                                                                             : Mpwm2DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Packet->Mpwm == MPWM_0) ? OC32_DMA_MPWM0 : (Packet->Mpwm == MPWM_1) ? OC32_DMA_MPWM1
                                                                                                      : OC32_DMA_MPWM2;

  /* wait previous packet ends */
  while (DMAChState[DmaCh] != OC32_DMA_IDLE)
    ;

  if (DmaUpdate(HAL_DISABLE, DmaCh, DmaModule, OC32_DMA_BUSY) != HAL_OK)
    return HAL_ERROR;

  /* enalbe led serial mode(dma) */
  Packet->Mpwm->MPCON.MPLSM = HAL_ENABLE;

  /* setup data */
  Packet->Mpwm->MPLDC = Packet->Length;
  Packet->Mpwm->MPLDA = (uint32_t)Packet->Data;

  /* enable kick start */
  Packet->Mpwm->MPCON.MPW0E = HAL_ENABLE;
  Packet->Mpwm->MPCON.MPW1E = HAL_ENABLE;
  Packet->Mpwm->MPCON.MPLSKS = HAL_ENABLE;

  /* return if it is interrupt mode */
  if (Packet->Mpwm->MPCON.MPIE == HAL_ENABLE)
  {
    return HAL_OK;
  }

  /* if interrupt disabled, wait for data transfer to complete */
  while (Packet->Mpwm->MPCON.MPW0F == HAL_FLAG_INVALID)
    ;

  /* clear done flag */
  Packet->Mpwm->MPCON.MPW0CLR = HAL_ENABLE;
  return DmaUpdate(HAL_DISABLE, DmaCh, DmaModule, OC32_DMA_IDLE);
}

/**
 * @brief  Handles MPWM interrupt request at user side.
 * @param  Mpwm: pointer to a MODULE_MPWM_typedef structure, specifies which MPWM
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__sram__ __weak__ HAL_StatusTypedef HAL_MPWM_UserIRQ(MODULE_MPWM_Typedef *Mpwm)
{
  return HAL_OK;
}

/**
 * @brief  Handles MPWM interrupt request.
 * @param  Mpwm: pointer to a MODULE_MPWM_typedef structure, specifies which MPWM
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Mpwm is not MPWM_0/MPWM_1
 * @note   Invoked from the global interrupt handler. In serial-LED mode the transfer
 *         drives way0 and way1 together, so way0's flag (MPW0F) is the authoritative
 *         completion signal: when it is set, the shared DMA channel is released
 *         (state -> IDLE). The remaining way flags (MPW1F..MPW7F) are cleared
 *         independently without touching the channel, since it is shared per
 *         instance and is already released by the way0 completion.
 */
__sram__ HAL_StatusTypedef HAL_MPWM_IRQHandler(MODULE_MPWM_Typedef *Mpwm)
{
  if ((Mpwm != MPWM_0) && (Mpwm != MPWM_1) && (Mpwm != MPWM_2))
    return HAL_ERROR;

  HAL_StatusTypedef status;
  
  if (Mpwm->MPCON.MPW0F == HAL_FLAG_VALID)
  {
    status = HAL_MPWM_UserIRQ(Mpwm);
    HAL_DMA_ChTypedef DmaCh = (Mpwm == MPWM_0) ? Mpwm0DmaCh : (Mpwm == MPWM_1) ? Mpwm1DmaCh
                                                                               : Mpwm2DmaCh;
    HAL_DMA_MapTypedef DmaModule = (Mpwm == MPWM_0) ? OC32_DMA_MPWM0 : (Mpwm == MPWM_1) ? OC32_DMA_MPWM1
                                                                                        : OC32_DMA_MPWM2;
    Mpwm->MPCON.MPW0CLR = HAL_ENABLE;
    DmaUpdate(HAL_DISABLE, DmaCh, DmaModule, OC32_DMA_IDLE);
    return status;
  }

  status = HAL_MPWM_UserIRQ(Mpwm);
  if (Mpwm->MPCON.MPW1F == HAL_FLAG_VALID)
  {
    Mpwm->MPCON.MPW1CLR = HAL_ENABLE;
  }

  if (Mpwm->MPCON.MPW2F == HAL_FLAG_VALID)
  {
    Mpwm->MPCON.MPW2CLR = HAL_ENABLE;
  }

  if (Mpwm->MPCON.MPW3F == HAL_FLAG_VALID)
  {
    Mpwm->MPCON.MPW3CLR = HAL_ENABLE;
  }

  if (Mpwm->MPCON.MPW4F == HAL_FLAG_VALID)
  {
    Mpwm->MPCON.MPW4CLR = HAL_ENABLE;
  }

  if (Mpwm->MPCON.MPW5F == HAL_FLAG_VALID)
  {
    Mpwm->MPCON.MPW5CLR = HAL_ENABLE;
  }

  if (Mpwm->MPCON.MPW6F == HAL_FLAG_VALID)
  {
    Mpwm->MPCON.MPW6CLR = HAL_ENABLE;
  }

  if (Mpwm->MPCON.MPW7F == HAL_FLAG_VALID)
  {
    Mpwm->MPCON.MPW7CLR = HAL_ENABLE;
  }
  return status;

}

#endif /* HAL_MPWM_ENABLE */