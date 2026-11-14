/**
 ******************************************************************************
 * @file    oc32_hal_uart.c
 * @author
 * @brief   OC32 HAL UART module driver.
 ******************************************************************************
 */



/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_uart.h"

#ifdef HAL_UART_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static HAL_DMA_ChTypedef Ut0DmaCh = 0U;
static HAL_DMA_ChTypedef Ut1DmaCh = 0U;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  UART TX Setup
 * @param  Uart: pointer to a MODULE_UART_typedef structure, specifies which UART
 * @param  Length: length of data in bytes
 * @param  Data: pointer to the tx data buffer
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Uart is not UART_0/UART_1
 * @note   Blocks until any pending UART interrupt (UTIE) has completed.
 */
__sram__ HAL_StatusTypedef UartTxSetup(MODULE_UART_Typedef *Uart, uint32_t Length, uint32_t *Data)
{

  if (Uart != UART_0 && Uart != UART_1)
    return HAL_ERROR;

  while (Uart->UTCON.UTIE == HAL_ENABLE)
    ;
  Uart->UTTBC = Length;
  Uart->UTTBUF = *Data;
  Uart->UTTDA = (uint32_t)Data;

  return HAL_OK;
}

/**
 * @brief  UART RX Setup
 * @param  Uart: pointer to a MODULE_UART_typedef structure, specifies which UART
 * @param  Length: length of data in bytes
 * @param  Data: pointer to the rx data buffer (may be NULL; in that case
 *               UTRDA is set to 0, which is harmless when DMA is not enabled,
 *               e.g. for short (<= 4 byte) transfers where rx result is read
 *               back from UTRBUF instead of via DMA)
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Uart is not UART_0/UART_1
 * @note   Blocks until any pending UART interrupt (UTIE) has completed.
 */
__sram__ HAL_StatusTypedef UartRxSetup(MODULE_UART_Typedef *Uart, uint32_t Length, uint32_t *Data)
{
  if (Uart != UART_0 && Uart != UART_1)
    return HAL_ERROR;

  while (Uart->UTCON.UTIE == HAL_ENABLE)
    ;
  Uart->UTRBC = Length;
  Uart->UTRDA = (uint32_t)Data;

  return HAL_OK;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the UART peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_UART_Inittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or Init->Uart is not UART_0/UART_1
 */
HAL_StatusTypedef HAL_UART_Init(HAL_UART_InitTypedef *Init)
{
  if (Init == NULL)
    return HAL_ERROR;

  /* record dma channel per uart instance */
  if (Init->Uart == UART_0)
  {
    Ut0DmaCh = Init->DmaCh;
  }
  else if (Init->Uart == UART_1)
  {
    Ut1DmaCh = Init->DmaCh;
  }
  else
  {
    return HAL_ERROR;
  }

  /* setup parity mode */
  Init->Uart->UTCON.UTPAR = Init->Parity;

  /* setup bits */
  Init->Uart->UTCON.UTM = Init->Bits;

  /* setup duplex */
  Init->Uart->UTCON.UTHD = Init->Duplex;

  /* setup pins */
  Init->Uart->UTCON.UTPORTS = Init->Pins;

  /* Baud must be non-zero (SystemClock / Baud would divide-by-zero otherwise) */
  if (Init->Baud == 0U)
    return HAL_ERROR;

  /* calculates cycles per bit */
  uint32_t bitcycles = SystemClock / Init->Baud;
  /* round up */
  if ((SystemClock % Init->Baud) > (Init->Baud >> 1))
    bitcycles++;

  /* UTBR min value is 0 (= 8 cycles/bit), so fewer than 8 cycles per
     bit cannot generate the requested baud rate. */
  if (bitcycles < 8U)
    return HAL_ERROR;

  /* setup baudrate */
  Init->Uart->UTCON.UTBR = bitcycles / 8;
  Init->Uart->UTCON.UTBRA = bitcycles % 8;

  return HAL_OK;
}

/**
 * @brief  DeInitializes the UART peripheral.
 * @param  Init: pointer to a HAL_UART_Inittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Uart is not UART_0/UART_1
 * @note   Resets UTCON to 0 then clears TX and RX done flags.
 */
HAL_StatusTypedef HAL_UART_DeInit(HAL_UART_InitTypedef *Init)
{
  if (Init == NULL || (Init->Uart != UART_0 && Init->Uart != UART_1))
    return HAL_ERROR;

  WRITE_SR(&Init->Uart->UTCON, 0U);
  Init->Uart->UTCON.UTTCLR = HAL_ENABLE;
  Init->Uart->UTCON.UTRCLR = HAL_ENABLE;
  HAL_DMA_ChTypedef DmaCh = (Init->Uart == UART_0) ? Ut0DmaCh : Ut1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Init->Uart == UART_0) ? OC32_DMA_UART0 : OC32_DMA_UART1;
  DmaUpdate(Init->Uart->UTCON.UTTDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  return HAL_OK;
}

/**
 * @brief  Tx packet
 * @param  Packet: pointer to a HAL_UART_Packettypedef structure, specifies the tx data packet.
 *                 Packet->Data must not be NULL.
 * @retval HAL_OK: on success (in both polling and interrupt mode)
 * @retval HAL_ERROR: on failure
 * @note   In half-duplex mode, waits for any pending RX DMA to complete first.
 *         In interrupt mode (Packet->IntE != 0), returns immediately; completion is
 *         handled by HAL_UART_IRQHandler. In polling mode, blocks until transfer completes.
 *         Caller must not mask the UART interrupt during the call, or the busy-wait
 *         on UTTDMAE / UTTBV will not terminate.
 */
__sram__ HAL_StatusTypedef HAL_UART_TX(HAL_UART_PacketTypedef *Packet)
{
  if (Packet == NULL || Packet->Data == NULL || (Packet->Uart != UART_0 && Packet->Uart != UART_1))
    return HAL_ERROR;

  /* wait rx if it is half-duplex */
  if (Packet->Uart->UTCON.UTHD == UART_HALF_DUPLEX)
  {
    while (Packet->Uart->UTCON.UTRDMAE == HAL_ENABLE)
      ;
  }

  /* wait previous packet ends */
  while (Packet->Uart->UTCON.UTTDMAE == HAL_ENABLE)
    ;

  HAL_DMA_ChTypedef DmaCh = (Packet->Uart == UART_0) ? Ut0DmaCh : Ut1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Packet->Uart == UART_0) ? OC32_DMA_UART0 : OC32_DMA_UART1;
  /* setup dma if length is greater than 4 bytes */
  if ((Packet->Length > 4U) || (Packet->IntE == HAL_ENABLE))
  {
    if (DmaUpdate(Packet->Uart->UTCON.UTRDMAE, DmaCh, DmaModule, OC32_DMA_BUSY) != HAL_OK)
      return HAL_ERROR;

    /* setup tx dma */
    Packet->Uart->UTCON.UTTDMAE = HAL_ENABLE;
  }

  if (UartTxSetup(Packet->Uart, Packet->Length, Packet->Data) != HAL_OK)
    return HAL_ERROR;

  /* setup interrupt enable */
  Packet->Uart->UTCON.UTIE = Packet->IntE;

  /* kick start tx */
  Packet->Uart->UTCON.UTTKS = HAL_ENABLE;

  /* return if it is interrupt mode */
  if (Packet->Uart->UTCON.UTIE == HAL_ENABLE)
  {
    return HAL_OK;
  }

  /* if interrupt disabled, wait for data transfer to complete */
  while (Packet->Uart->UTCON.UTTBV == HAL_FLAG_INVALID)
    ;
  /* clear done flag */
  Packet->Uart->UTCON.UTTCLR = HAL_ENABLE;
  Packet->Uart->UTCON.UTTDMAE = HAL_DISABLE;
  return DmaUpdate(Packet->Uart->UTCON.UTRDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
}

/**
 * @brief  Rx packet
 * @param  Packet: pointer to a HAL_UART_Packettypedef structure, specifies the rx data packet.
 *                 Packet->Data must not be NULL.
 * @retval HAL_OK: on success (in both polling and interrupt mode)
 * @retval HAL_ERROR: on failure
 * @note   In half-duplex mode, waits for any pending TX DMA to complete first.
 *         In interrupt mode (Packet->IntE != 0), returns immediately; completion is
 *         handled by HAL_UART_IRQHandler. In polling mode, blocks until transfer completes.
 *         Caller must not mask the UART interrupt during the call, or the busy-wait
 *         on UTRDMAE / UTRBV will not terminate.
 */
__sram__ HAL_StatusTypedef HAL_UART_RX(HAL_UART_PacketTypedef *Packet)
{
  if (Packet == NULL || Packet->Data == NULL || (Packet->Uart != UART_0 && Packet->Uart != UART_1))
    return HAL_ERROR;

  /* wait tx if it is half-duplex */
  if (Packet->Uart->UTCON.UTHD == UART_HALF_DUPLEX)
  {
    while (Packet->Uart->UTCON.UTTDMAE == HAL_ENABLE) 
      ;
  }

  /* wait previous packet ends */
  while (Packet->Uart->UTCON.UTRDMAE == HAL_ENABLE)
    ;

  /* setup dma if length is greater than 4 bytes */
  HAL_DMA_ChTypedef DmaCh = (Packet->Uart == UART_0) ? Ut0DmaCh : Ut1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Packet->Uart == UART_0) ? OC32_DMA_UART0 : OC32_DMA_UART1;
  if ((Packet->Length > 4U) || (Packet->IntE == HAL_ENABLE))
  {
    if (DmaUpdate(Packet->Uart->UTCON.UTTDMAE, DmaCh, DmaModule, OC32_DMA_BUSY) != HAL_OK)
      return HAL_ERROR;

    /* setup rx dma */
    Packet->Uart->UTCON.UTRDMAE = HAL_ENABLE;
  }

  if (UartRxSetup(Packet->Uart, Packet->Length, Packet->Data) != HAL_OK)
    return HAL_ERROR;

  /* setup interrupt enable */
  Packet->Uart->UTCON.UTIE = Packet->IntE;

  /* enable rx */
  Packet->Uart->UTCON.UTRE = HAL_ENABLE;

  /* return if it is interrupt mode */
  if (Packet->Uart->UTCON.UTIE == HAL_ENABLE)
    return HAL_OK;

  /* if interrupt disabled, wait for data transfer to complete */
  while (Packet->Uart->UTCON.UTRBV == HAL_FLAG_INVALID)
    ;

  /* copy the rx data in buffer for non-dma mode */
  if (Packet->Uart->UTCON.UTRDMAE == HAL_DISABLE)
    Packet->Data[0] = Packet->Uart->UTRBUF;

  HAL_StatusTypedef status = HAL_OK;
  if (Packet->Uart->UTCON.UTRERR == HAL_FLAG_ERROR)
    status = HAL_ERROR;

  Packet->Uart->UTCON.UTRE = HAL_DISABLE;
  Packet->Uart->UTCON.UTRCLR = HAL_ENABLE;
  Packet->Uart->UTCON.UTRDMAE = HAL_DISABLE;
  DmaUpdate(Packet->Uart->UTCON.UTTDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  return status;
}

/**
 * @brief  Handles UART interrupt request at user side.
 * @param  Uart: pointer to a MODULE_UART_typedef structure, specifies which UART
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__sram__ __weak__ HAL_StatusTypedef HAL_UART_UserIRQ(MODULE_UART_Typedef *Uart)
{
  return HAL_OK;
}

/**
 * @brief  Handles UART interrupt request.
 * @param  Uart: pointer to a MODULE_UART_typedef structure, specifies which UART
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Uart is not UART_0/UART_1
 * @note   Invoked from the global interrupt handler. On TX completion (UTTBV set) or
 *         RX completion (UTRBV set), clears the respective done flag, disables the
 *         associated DMA, and updates the DMA channel state to IDLE.
 *         Disables UTIE when the DMA channel is idle, allowing the next transfer to proceed.
 */
__sram__ HAL_StatusTypedef HAL_UART_IRQHandler(MODULE_UART_Typedef *Uart)
{
  if (Uart != UART_0 && Uart != UART_1)
    return HAL_ERROR;

  HAL_DMA_ChTypedef DmaCh = (Uart == UART_0) ? Ut0DmaCh : Ut1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Uart == UART_0) ? OC32_DMA_UART0 : OC32_DMA_UART1;
  HAL_StatusTypedef status = HAL_OK;

  if (Uart->UTCON.UTTBV == HAL_FLAG_VALID)
  {
    status = HAL_UART_UserIRQ(Uart);
    Uart->UTCON.UTTCLR = HAL_ENABLE;
    Uart->UTCON.UTTDMAE = HAL_DISABLE;
    /* if Rx is still in progress, keep the DMA state as busy */
    DmaUpdate(Uart->UTCON.UTRDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }
  else if (Uart->UTCON.UTRBV == HAL_FLAG_VALID)
  {
    if (Uart->UTCON.UTRERR == HAL_FLAG_ERROR)
    {
      status = HAL_ERROR;
    }
    else
    {
      status = HAL_UART_UserIRQ(Uart);
    }

    Uart->UTCON.UTRCLR = HAL_ENABLE;
    Uart->UTCON.UTRE = HAL_DISABLE;
    Uart->UTCON.UTRDMAE = HAL_DISABLE;
    /* if Tx is still in progress, keep the DMA state as busy */
    DmaUpdate(Uart->UTCON.UTTDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }

  /* disable interrupt only if the DMA channel is idle: both Tx and Rx are done */
  if (DMAChState[DmaCh] == OC32_DMA_IDLE)
    Uart->UTCON.UTIE = HAL_DISABLE;

  return status;
}

#endif /* HAL_UART_ENABLE */