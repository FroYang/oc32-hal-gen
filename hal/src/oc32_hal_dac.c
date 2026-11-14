/**
 ******************************************************************************
 * @file    oc32_hal_dac.c
 * @author
 * @brief   OC32 HAL DAC (Digital to Analog Converter) module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_dac.h"

#ifdef HAL_DAC_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static HAL_DMA_ChTypedef Dac0DmaCh = 0U;
static HAL_DMA_ChTypedef Dac1DmaCh = 1U;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  DeInitializes the DAC peripheral.
 * @retval HAL_OK: on success
 * @note   Disables all DAC actions and clears the flag.
 */
HAL_StatusTypedef HAL_DAC_DeInit(HAL_DAC_InitTypedef *Init)
{
  if ((Init == NULL) || ((Init->Dac != DAC_0) && (Init->Dac != DAC_1)))
    return HAL_ERROR;

  Init->Dac->DACBUF = 0U;

  WRITE_SR(&Init->Dac->DACCON, 0U);

  return HAL_OK;
}

/**
 * @brief  Initializes the DAC peripheral according to the specified parameters.
 * @param  Init: Pointer to a structure that contains the DAC configuration parameters.
 * @retval HAL_OK: on success
 */
HAL_StatusTypedef HAL_DAC_Init(HAL_DAC_InitTypedef *Init)
{
  if ((Init == NULL) || ((Init->Dac != DAC_0) && (Init->Dac != DAC_1)))
    return HAL_ERROR;

  Init->Dac->DACCON.DACCCD = Init->ClkDiv;
  Init->Dac->DACCON.DACIM = Init->Mode;

  Init->Dac->DACBUF = 0U;

  Init->Dac->DACCON.DACE = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Converts DAC data (polling or interrupt mode)
 * @param  Packet: pointer to a HAL_DAC_PacketTypedef structure
 * @retval HAL_OK: on success (in both polling and interrupt mode)
 * @retval HAL_ERROR: on failure
 * @note   In interrupt mode (Packet->IntE != 0), returns immediately; completion is
 *         handled by HAL_DAC_IRQHandler. In polling mode, blocks until conversion completes.
 */
__sram__ HAL_StatusTypedef HAL_DAC_Convert(HAL_DAC_PacketTypedef *Packet)
{
  if ((Packet == NULL) || ((Packet->Dac != DAC_0) && (Packet->Dac != DAC_1)))
    return HAL_ERROR;

  /* wait previous packet ends */
  while (Packet->Dac->DACCON.DACDMAE)
    ;

  /* setup dma if more than 2 conversions or interrupt mode enabled */
  HAL_DMA_ChTypedef DmaCh = (Packet->Dac == DAC_0) ? Dac0DmaCh : Dac1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Packet->Dac == DAC_0) ? OC32_DMA_DAC0 : OC32_DMA_DAC1;
  if ((Packet->Count > 2U) || (Packet->IntE == HAL_ENABLE))
  {
    if (DmaUpdate(Packet->Dac->DACCON.DACDMAE, DmaCh, DmaModule, OC32_DMA_BUSY) != HAL_OK)
      return HAL_ERROR;

    /* setup dma */
    Packet->Dac->DACCON.DACDMAE = HAL_ENABLE;
    Packet->Dac->DACCON.DACDMAL = Packet->Mode;
  }

  /* setup data */
  Packet->Dac->DACDC = Packet->Count;
  Packet->Dac->DACBUF = *Packet->Data;
  Packet->Dac->DACDA = (uint32_t)Packet->Data;

  /* setup interrupt enable */
  Packet->Dac->DACCON.DACIE = Packet->IntE;

  /* return if it is interrupt mode or loop mode (no completion flags) */
  if ((Packet->Dac->DACCON.DACIE == HAL_ENABLE) || (Packet->Dac->DACCON.DACDMAL == DAC_DMA_LOOP))
  {
    return HAL_OK;
  }

  /* if interrupt disabled, wait for data transfer to complete */
  while (Packet->Dac->DACCON.DACCF == HAL_FLAG_INVALID)
    ;

  /* clear done flag */
  Packet->Dac->DACCON.DACCCLR = HAL_ENABLE;
  Packet->Dac->DACCON.DACDMAE = HAL_DISABLE;
  return DmaUpdate(Packet->Dac->DACCON.DACDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
}

/**
 * @brief  Handles DAC interrupt request at user side.
 * @param  Dac: pointer to a MODULE_DAC_typedef structure, specifies which DAC
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__sram__ __weak__ HAL_StatusTypedef HAL_DAC_UserIRQ(MODULE_DAC_Typedef *Dac)
{
  return HAL_OK;
}

/**
 * @brief  Handles DAC interrupt request.
 * @param  Dac: pointer to a MODULE_DAC_typedef structure, specifies which DAC
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Dac is not DAC_0/DAC_1
 * @note   Invoked from the global interrupt handler. On conversion completion (DACCF set),
 *         clears DACCCLR, disables DMA (DACDMAE=0), and updates the DMA channel state to IDLE.
 *         Disables DACIE when the DMA channel is idle, allowing the next transfer to proceed.
 */
__sram__ HAL_StatusTypedef HAL_DAC_IRQHandler(MODULE_DAC_Typedef *Dac)
{
  if ((Dac != DAC_0) && (Dac != DAC_1))
    return HAL_ERROR;

  HAL_DMA_ChTypedef DmaCh = (Dac == DAC_0) ? Dac0DmaCh : Dac1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Dac == DAC_0) ? OC32_DMA_DAC0 : OC32_DMA_DAC1;
  HAL_StatusTypedef status = HAL_OK;

  status = HAL_DAC_UserIRQ(Dac);
  Dac->DACCON.DACCCLR = HAL_ENABLE;
  Dac->DACCON.DACDMAE = HAL_DISABLE;
  DmaUpdate(Dac->DACCON.DACDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  Dac->DACCON.DACIE = HAL_DISABLE;
  return status;
}

#endif /* HAL_DAC_ENABLE */