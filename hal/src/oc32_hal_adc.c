/**
 ******************************************************************************
 * @file    oc32_hal_adc.c
 * @author
 * @brief   OC32 HAL ADC (Analog-to-Digital Converter) module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_adc.h"

#ifdef HAL_ADC_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static HAL_DMA_ChTypedef Adc0DmaCh = 0U;
static HAL_DMA_ChTypedef Adc1DmaCh = 1U;

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Get the DMA-enable bit for a given event channel.
 * @param  Adc: pointer to a MODULE_ADC_Typedef structure, specifies which ADC
 * @param  Channel: the event channel (ADC_EVENT0..ADC_EVENT7) to query
 * @retval 1: if the channel's DMA-enable bit is set, 0: otherwise
 */
__sram__ uint32_t AdcDmaEnableVal(MODULE_ADC_Typedef *Adc, HAL_ADC_EventChannelTypedef Channel)
{

  if (Channel < ADC_EVENT4)
  {
    /* read out event0~3 dma enable bit */
    uint32_t Dc0Val = READ_SR(&Adc->ADCDC0);
    uint32_t Dc0ValWidth = ADCDC_BIT_WIDTH * Channel;
    Dc0Val &= (1U << ADCEDMAE_POS) << Dc0ValWidth;
    return (Dc0Val != 0U);
  }
  else
  {
    /* read out event4~7 dma enable bit */
    uint32_t Dc1Val = READ_SR(&Adc->ADCDC1);
    uint32_t Dc1ValWidth = ADCDC_BIT_WIDTH * (Channel - ADC_EVENT4);
    Dc1Val &= (1U << ADCEDMAE_POS) << Dc1ValWidth;
    return (Dc1Val != 0U);
  }
}

/**
 * @brief  Clear the DMA-enable bit for a given event channel.
 * @param  Adc: pointer to a MODULE_ADC_Typedef structure, specifies which ADC
 * @param  Channel: the event channel (ADC_EVENT0..ADC_EVENT7) to clear
 */
__sram__ void AdcDmaEnableClr(MODULE_ADC_Typedef *Adc, HAL_ADC_EventChannelTypedef Channel)
{
  if (Channel < ADC_EVENT4)
  {
    /* clear event0~3 dma enable bit */
    uint32_t Dc0Val = READ_SR(&Adc->ADCDC0);
    uint32_t Dc0ValWidth = ADCDC_BIT_WIDTH * Channel;
    Dc0Val &= ~((1U << ADCEDMAE_POS) << Dc0ValWidth);
    WRITE_SR(&Adc->ADCDC0, Dc0Val);
  }
  else
  {
    /* clear event4~7 dma enable bit */
    uint32_t Dc1Val = READ_SR(&Adc->ADCDC1);
    uint32_t Dc1ValWidth = ADCDC_BIT_WIDTH * (Channel - ADC_EVENT4);
    Dc1Val &= ~((1U << ADCEDMAE_POS) << Dc1ValWidth);
    WRITE_SR(&Adc->ADCDC1, Dc1Val);
  }
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  DeInitializes the ADC peripheral.
 * @retval HAL_OK: on success
 * @note   Disables all ADC actions and clears the flag.
 */
HAL_StatusTypedef HAL_ADC_DeInit(HAL_ADC_InitTypedef *Init)
{
  if ((Init == NULL) || ((Init->Adc != ADC_0) && (Init->Adc != ADC_1)))
    return HAL_ERROR;

  WRITE_SR(&Init->Adc->ADCCON, 0U);
  HAL_DMA_ChTypedef DmaCh = (Init->Adc == ADC_0) ? Adc0DmaCh : Adc1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Init->Adc == ADC_0) ? OC32_DMA_ADC0 : OC32_DMA_ADC1;
  return DmaUpdate(HAL_DISABLE, DmaCh, DmaModule, OC32_DMA_IDLE);
}

/**
 * @brief  Initializes the ADC peripheral according to the specified parameters.
 * @param  Init: Pointer to a structure that contains the ADC configuration parameters.
 * @retval HAL_OK: on success
 */
HAL_StatusTypedef HAL_ADC_Init(HAL_ADC_InitTypedef *Init)
{
  if (Init == NULL)
    return HAL_ERROR;

  /* record dma channel per adc instance */
  if (Init->Adc == ADC_0)
  {
    Adc0DmaCh = Init->DmaCh;
  }
  else if (Init->Adc == ADC_1)
  {
    Adc1DmaCh = Init->DmaCh;
  }
  else
  {
    return HAL_ERROR;
  }

  /* setup adc control */
  Init->Adc->ADCCON.ADCCCD = Init->ClkDiv;
  Init->Adc->ADCWW1.ADCRC = Init->Res;
  Init->Adc->ADCCON.ADCDMABM = Init->Mode;
  Init->Adc->ADCCON.ADCDMADA = Init->Align;
  Init->Adc->ADCCON.ADCRS = Init->Ref;
  Init->Adc->ADCCON.ADCESDLYP = Init->Pri;
  Init->Adc->ADCCON.ADCESDLYU = Init->Unit;

  /* setup the buffer address, allow NULL(it may not be DMA mode) */
  Init->Adc->ADCDA = (uint32_t)&Init->Buf;

  /* setup the watch windows */
  Init->Adc->ADCWW0.ADCWW0T = Init->Watch0Top;
  Init->Adc->ADCWW0.ADCWW0B = Init->Watch0Bottom;
  Init->Adc->ADCWW1.ADCWW1T = Init->Watch1Top;
  Init->Adc->ADCWW1.ADCWW1B = Init->Watch1Bottom;

  Init->Adc->ADCCON.ADCE = HAL_ENABLE;
  return HAL_OK;
}

/**
 * @brief  Start the ADC conversion by event control.
 * @param  Event: Pointer to a structure that contains the ADC configuration parameters.
 * @retval Result: HAL_ResultTypedef with Status and Data.
 * @note   In DMA mode (Count > 1) or interrupt mode (IntE enabled), this returns
 *         immediately after programming the transfer; in polling mode it blocks
 *         until the event's completion flag (ADCECF) is set.
 *         Result.Status is HAL_OK on success, HAL_ERROR on a trigger-overflow
 *         event, and HAL_SPECIAL if a watch window was hit.
 *         Result.Data carries the 12-bit conversion result only in non-DMA
 *         mode; it is not meaningful for multi-count DMA transfers.
 */
__sram__ HAL_ResultTypedef HAL_ADC_EventConvert(HAL_ADC_EventTypedef *Event)
{
  HAL_ResultTypedef Result = {0U, HAL_ERROR};

  if ((Event == NULL) || ((Event->Adc != ADC_0) && (Event->Adc != ADC_1)))
    return Result;

  /* wait previous event ends */
  while (AdcDmaEnableVal(Event->Adc, Event->Channel))
    ;

  HAL_DMA_ChTypedef DmaCh = (Event->Adc == ADC_0) ? Adc0DmaCh : Adc1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Event->Adc == ADC_0) ? OC32_DMA_ADC0 : OC32_DMA_ADC1;

  /* combine the dma config value */
  uint32_t DmaVal = 0U;
  if (Event->Count > (ADCEDC_BIT_MASK + 1U))
  {
    return Result;
  }
  else
  {
    if (Event->Count > 1U)
    {
      DmaVal = (Event->Count & ADCEDC_BIT_MASK) | (HAL_ENABLE << ADCEDMAE_POS);
      DmaUpdate(HAL_DISABLE, DmaCh, DmaModule, OC32_DMA_BUSY);
    }
  }

  /* setup adc signal input */
  uint32_t InputBank = HAL_DISABLE;
  if (Event->Input > ADC_INPUT15)
    InputBank = HAL_ENABLE;

  uint32_t Ww0Val = READ_SR(&Event->Adc->ADCWW0);
  Event->Adc->ADCECC[Event->Channel].ADCEIS = Event->Input;
  if (Event->Channel > ADC_EVENT4)
  {
    Ww0Val &= ~((1U << ADCE0IB_POS) << Event->Channel);
    Ww0Val |= (InputBank << ADCE0IB_POS) << Event->Channel;

    /* setup event0~3 dma control */
    uint32_t Dc0Val = READ_SR(&Event->Adc->ADCDC0);
    uint32_t DmaValWidth = ADCDC_BIT_WIDTH * Event->Channel;
    Dc0Val &= ~(ADCDC_BIT_MASK << DmaValWidth);
    Dc0Val |= (DmaVal << DmaValWidth);
    WRITE_SR(&Event->Adc->ADCDC0, Dc0Val);
  }
  else
  {
    Ww0Val &= ~((1U << ADCE4IB_POS) << (Event->Channel - ADC_EVENT4));
    Ww0Val |= (InputBank << ADCE4IB_POS) << (Event->Channel - ADC_EVENT4);

    /* setup event4~7 dma control */
    uint32_t Dc1Val = READ_SR(&Event->Adc->ADCDC1);
    uint32_t DmaValWidth = ADCDC_BIT_WIDTH * (Event->Channel - ADC_EVENT4);
    Dc1Val &= ~(ADCDC_BIT_MASK << DmaValWidth);
    Dc1Val |= (DmaVal << DmaValWidth);
    WRITE_SR(&Event->Adc->ADCDC1, Dc1Val);
  }

  /* setup the event control */
  Event->Adc->ADCECC[Event->Channel].ADCEES = Event->Source;
  Event->Adc->ADCECC[Event->Channel].ADCEPL = Event->Pri;
  Event->Adc->ADCECC[Event->Channel].ADCESDLY = Event->Delay;
  Event->Adc->ADCECC[Event->Channel].ADCEST = Event->Cycle;

  /* setup the watch control 0 */
  uint32_t Wc0Val = READ_SR(&Event->Adc->ADCWC0);
  Wc0Val &= ~((1U << ADCWE0E_POS) << Event->Channel);
  Wc0Val |= (Event->Watch0E << ADCWE0E_POS) << Event->Channel;

  /* setup the watch control 1 and interrupt enable */
  uint32_t Wc1Val = READ_SR(&Event->Adc->ADCWC1);
  Wc1Val &= ~((1U << ADCWE0E_POS) << Event->Channel);
  Wc1Val &= ~((1U << ADCE0IE_POS) << Event->Channel);
  Wc1Val |= (Event->Watch1E << ADCWE0E_POS) << Event->Channel;
  Wc1Val |= (Event->IntE << ADCE0IE_POS) << Event->Channel;
  WRITE_SR(&Event->Adc->ADCWC0, Wc0Val);
  WRITE_SR(&Event->Adc->ADCWC1, Wc1Val);

  /* kick start if it is user kick start */
  if (Event->Source == ADC_EVENT_USER)
  {
    Event->Adc->ADCCON.ADCUCS = HAL_ENABLE;
  }

  Result.Status = HAL_OK;
  /* return if it is interrupt enable */
  if (Event->IntE)
  {
    return Result;
  }

  /* if interrupt disable, wait for the convertion to finish */
  while (Event->Adc->ADCECC[Event->Channel].ADCECF == HAL_FLAG_INVALID)
    ;

  /* check if it is event trigger overflow */
  Wc0Val = READ_SR(&Event->Adc->ADCWC0);
  if ((Wc0Val & ((1U << ADCE0OV_POS) << Event->Channel)) != HAL_FLAG_GOOD)
  {
    Result.Status = HAL_ERROR;
    return Result;
  }

  /* check if it is hit the watch window.
     Note: ADCWE0F_POS / ADCWE0E_POS / ADCWE0CLR_POS are
     intentionally reused for both ADCWC0 and ADCWC1 because the two
     registers share identical bit positions (Watch-enable 0-7, clear/flag 16-31).
     No separate ADCW1* macro set is needed. */
  Wc1Val = READ_SR(&Event->Adc->ADCWC1);
  uint32_t WatchHit = Event->Channel * 2U;
  if (((Wc0Val & ((1U << ADCWE0F_POS) << WatchHit)) != HAL_FLAG_INVALID) ||
      ((Wc1Val & ((1U << ADCWE0F_POS) << WatchHit)) != HAL_FLAG_INVALID))
  {
    Result.Status = HAL_SPECIAL;
  }

  /* copy the result for non-dma mode,
     Result.Data has no meaning in dma mode */
  Result.Data = Event->Adc->ADCECC[Event->Channel].ADCED;

  /* clear done flag */
  Event->Adc->ADCECC[Event->Channel].ADCECCLR = HAL_ENABLE;
  AdcDmaEnableClr(Event->Adc, Event->Channel);
  DmaUpdate(HAL_DISABLE, DmaCh, DmaModule, OC32_DMA_IDLE);
  return Result;
}

/**
 * @brief  Handles ADC interrupt request at user side.
 * @param  Adc: pointer to a MODULE_ADC_typedef structure, specifies which ADC
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__sram__ __weak__ HAL_StatusTypedef HAL_ADC_UserIRQ(MODULE_ADC_Typedef *Adc)
{
  return HAL_OK;
}

/**
 * @brief  Handles ADC interrupt request.
 * @param  Adc: pointer to a MODULE_ADC_typedef structure, specifies which ADC
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Adc is not ADC_0/ADC_1
 * @note   Invoked from the global interrupt handler. The returned status word is a
 *         packed per-channel bitmask: bit Event of the return value holds the result
 *         of HAL_ADC_UserIRQ for that channel (HAL_ADC_UserIRQ(Adc) << Event), so a
 *         single return word reports the status of all 8 ADC event channels at once.
 *         A channel with no completion/error contributes 0 (HAL_OK). HAL_FLAG_ERROR is
 *         intentionally NOT used here because selecting it would require extra
 *         compare/branch instructions in the hot IRQ loop; the packed form is cheaper.
 */
__sram__ HAL_StatusTypedef HAL_ADC_IRQHandler(MODULE_ADC_Typedef *Adc)
{
  if (Adc != ADC_0 && Adc != ADC_1)
    return HAL_ERROR;

  HAL_DMA_ChTypedef DmaCh = (Adc == ADC_0) ? Adc0DmaCh : Adc1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Adc == ADC_0) ? OC32_DMA_ADC0 : OC32_DMA_ADC1;
  HAL_StatusTypedef status = HAL_OK;

  for (uint32_t Event = 0U; Event < ADC_EVENT_TOTAL; Event++)
  {
    /* call user IRQ handler at blow cases:
       - event trigger overflow (error case)
       - conversion done
       - watch window hit */
    uint32_t WatchHit = Event * 2U;
    uint32_t Wc0Val = READ_SR(&Adc->ADCWC0);
    uint32_t Wc1Val = READ_SR(&Adc->ADCWC1);
    /* ADCWE0F_POS / ADCWE0CLR_POS are reused for both ADCWC0 and ADCWC1
       because the two registers share identical bit positions (Watch-enable
       0-7, clear/flag 16-31). No separate ADCW1* macro set
       is needed. */
    if (((Wc0Val & ((1U << ADCE0OV_POS) << Event)) != HAL_FLAG_GOOD) ||
        (Adc->ADCECC[Event].ADCECF == HAL_FLAG_VALID) ||
        ((Wc0Val & ((1U << ADCWE0F_POS) << WatchHit)) != HAL_FLAG_INVALID) ||
        ((Wc1Val & ((1U << ADCWE0F_POS) << WatchHit)) != HAL_FLAG_INVALID))
    {
      /* user IRQ handler, merge the status.
         Packed per-channel encoding: bit Event of the return word holds the
         result of HAL_ADC_UserIRQ(Adc) for channel Event (shifted into place),
         so the single return value reports all 8 channel outcomes. HAL_OK (0)
         leaves the bit clear; a non-zero user result sets it. HAL_FLAG_ERROR is
         deliberately not used here (it would add a compare/branch in this hot
         IRQ path). */
      status |= HAL_ADC_UserIRQ(Adc) << Event;

      /* clear overflow and done flags */
      Adc->ADCECC[Event].ADCECCLR = HAL_ENABLE;

      /* clear watch 0 hit flags */
      Wc0Val |= ((HAL_ENABLE << ADCWE0CLR_POS) << WatchHit);

      /* clear watch 1 hit flags and interrupt enable */
      Wc1Val |= ((HAL_ENABLE << ADCWE0CLR_POS) << WatchHit);
      Wc1Val &= ~((1U << ADCE0IE_POS) << Event);
      WRITE_SR(&Adc->ADCWC0, Wc0Val);
      WRITE_SR(&Adc->ADCWC1, Wc1Val);
      
      /* clear dma enable */
      AdcDmaEnableClr(Adc, Event);
      DmaUpdate(HAL_DISABLE, DmaCh, DmaModule, OC32_DMA_IDLE);
    }
  }

  return status;
}

#endif /* HAL_ADC_ENABLE */