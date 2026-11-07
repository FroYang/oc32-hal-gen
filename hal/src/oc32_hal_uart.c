/**
 ******************************************************************************
 * @file    oc32_hal_uart.c
 * @author
 * @brief   OC32 HAL UART module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal.h"

#ifdef HAL_UART_ENABLE
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static HAL_DMA_ChTypedef Ut0DmaCh = 0U;
static HAL_DMA_ChTypedef Ut1DmaCh = 0U;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the UART peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_UART_InitTypeDef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or Init->Uart is not UART_0/UART_1
 */
HAL_StatusTypeDef HAL_UART_Init(HAL_UART_InitTypeDef *Init)
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

  /* calculates cycles per bit */
  uint32_t bitcycles = SystemClock / Init->Baud;
  /* round up */
  if ((SystemClock % Init->Baud) > (Init->Baud >> 1))
    bitcycles++;

  /* setup baudrate */
  Init->Uart->UTCON.UTBR = bitcycles / 8;
  Init->Uart->UTCON.UTBRA = bitcycles % 8;

  return HAL_OK;
}

/**
 * @brief  DeInitializes the UART peripheral.
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Uart is not UART_0/UART_1
 * @note   Resets UTCON to 0 then clears TX and RX done flags.
 */
HAL_StatusTypeDef HAL_UART_DeInit(MODULE_UART_TypeDef *Uart)
{
  if (Uart != UART_0 && Uart != UART_1)
    return HAL_ERROR;

  *(volatile uint32_t *)&Uart->UTCON = 0U;
  Uart->UTCON.UTTCLR = 1U;
  Uart->UTCON.UTRCLR = 1U;
  return HAL_OK;
}

/**
 * @brief  UART TX Setup
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @param  Length: length of data in words
 * @param  Data: pointer to the tx data buffer
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Uart is not UART_0/UART_1
 * @note   Blocks until any pending UART interrupt (UTIE) has completed.
 */
__sram__ HAL_StatusTypeDef UartTxSetup(MODULE_UART_TypeDef *Uart, uint32_t Length, uint32_t *Data)
{

  if (Uart != UART_0 && Uart != UART_1)
    return HAL_ERROR;

  while (Uart->UTCON.UTIE)
    ;
  Uart->UTTBC = Length;
  Uart->UTTBUF = *Data;
  Uart->UTTDA = (uint32_t)Data;

  return HAL_OK;
}

/**
 * @brief  UART RX Setup
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @param  Length: length of data in words
 * @param  Data: pointer to the rx data buffer (may be NULL; in that case
 *               UTRDA is set to 0, which is harmless when DMA is not enabled,
 *               e.g. inside HAL_UART_RX_OneByte/OneWord where rx result is read
 *               back from UTRBUF instead of via DMA)
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Uart is not UART_0/UART_1
 * @note   Blocks until any pending UART interrupt (UTIE) has completed.
 */
__sram__ HAL_StatusTypeDef UartRxSetup(MODULE_UART_TypeDef *Uart, uint32_t Length, uint32_t *Data)
{
  if (Uart != UART_0 && Uart != UART_1)
    return HAL_ERROR;

  while (Uart->UTCON.UTIE)
    ;
  Uart->UTRBC = Length;
  Uart->UTRDA = (uint32_t)Data;

  return HAL_OK;
}

/**
 * @brief  UART TX once (blocking single-word transfer)
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @note   Blocks until TX complete (UTTBV), then clears the done flag (UTTCLR).
 */
__sram__ void UartTxOnce(MODULE_UART_TypeDef *Uart)
{
  /* kick start tx */
  Uart->UTCON.UTTKS = 1;
  while (Uart->UTCON.UTTBV == 0)
    ;
  /* clear done flag */
  Uart->UTCON.UTTCLR = 1;
}

/**
 * @brief  UART RX once (blocking single-word receive)
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @retval Received data word (from UTRBUF)
 * @note   Blocks until RX data is valid (UTRBV), then clears the done flag (UTRCLR)
 *         and disables RX (UTRE=0).
 */
__sram__ uint32_t UartRxOnce(MODULE_UART_TypeDef *Uart)
{
  Uart->UTCON.UTRE = 1U;

  while (Uart->UTCON.UTRBV == 0)
    ;
  Uart->UTCON.UTRCLR = 1U;
  Uart->UTCON.UTRE = 0U;

  return Uart->UTRBUF;  
}

/**
 * @brief  Tx one byte
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @param  Data: data to be sent
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if setup fails
 * @note   Blocks until transfer completes (no DMA, no interrupt).
 */
__sram__ HAL_StatusTypeDef HAL_UART_TX_OneByte(MODULE_UART_TypeDef *Uart, uint32_t Data)
{
  if (UartTxSetup(Uart, 1U, &Data) != HAL_OK)
    return HAL_ERROR;

  UartTxOnce(Uart);
  return HAL_OK;
}

/**
 * @brief  Rx one byte
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @retval HAL_ResultTypedef:
 *         .Data  = received byte
 *         .Error = HAL_OK on success, HAL_ERROR on failure
 * @note   Blocks until transfer completes (no DMA, no interrupt).
 */
__sram__ HAL_ResultTypedef HAL_UART_RX_OneByte(MODULE_UART_TypeDef *Uart)
{
  HAL_ResultTypedef Result = {0U, HAL_ERROR};
  /* ignore the address pointer, DMA is not enabled in this path */
  if (UartRxSetup(Uart, 1U, NULL) != HAL_OK)
  {
    return Result;
  }

  Result.Data = UartRxOnce(Uart);
  Result.Error = HAL_OK;
  return Result;
}

/**
 * @brief  Tx one word
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @param  Data: data to be sent
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if setup fails
 * @note   Blocks until transfer completes (no DMA, no interrupt).
 */
__sram__ HAL_StatusTypeDef HAL_UART_TX_OneWord(MODULE_UART_TypeDef *Uart, uint32_t Data)
{
  if (UartTxSetup(Uart, 4U, &Data) != HAL_OK)
    return HAL_ERROR;

  UartTxOnce(Uart);
  return HAL_OK;
}

/**
 * @brief  Rx one word
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @retval HAL_ResultTypedef:
 *         .Data  = received word
 *         .Error = HAL_OK on success, HAL_ERROR on failure
 * @note   Blocks until transfer completes (no DMA, no interrupt).
 */
__sram__ HAL_ResultTypedef HAL_UART_RX_OneWord(MODULE_UART_TypeDef *Uart)
{
  HAL_ResultTypedef Result = {0U, HAL_ERROR};
  /* ignore the address pointer, DMA is not enabled in this path */
  if (UartRxSetup(Uart, 4U, NULL) != HAL_OK)
  {
    return Result;
  }

  Result.Data = UartRxOnce(Uart);
  Result.Error = HAL_OK;
  return Result;
}


/**
 * @brief  Tx packet
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @param  Packet: pointer to a HAL_UART_PacketTypedef structure, specifies the tx data packet.
 *                 Packet->Data must not be NULL.
 * @retval HAL_OK: on success (in both polling and interrupt mode)
 * @retval HAL_ERROR: on failure
 * @note   In half-duplex mode, waits for any pending RX DMA to complete first.
 *         In interrupt mode (Packet->IntE != 0), returns immediately; completion is
 *         handled by HAL_UART_IRQHandler. In polling mode, blocks until transfer completes.
 *         Caller must not mask the UART interrupt during the call, or the busy-wait
 *         on UTTDMAE / UTTBV will not terminate.
 */
__sram__ HAL_StatusTypeDef HAL_UART_TX_Packet(MODULE_UART_TypeDef *Uart, HAL_UART_PacketTypedef *Packet)
{
  if (Packet == NULL || Packet->Data == NULL || (Uart != UART_0 && Uart != UART_1))
    return HAL_ERROR;

  /* wait rx if it is half-duplex */
  if (Uart->UTCON.UTHD)
  {
    while (Uart->UTCON.UTRDMAE)
      ;
  }

  /* wait previous packet ends */
  while (Uart->UTCON.UTTDMAE)
    ;

  HAL_DMA_ChTypedef DmaCh = (Uart == UART_0) ? Ut0DmaCh : Ut1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Uart == UART_0) ? OC32_DMA_UART0 : OC32_DMA_UART1;
  if (HAL_DMA_Update(Uart->UTCON.UTRDMAE, DmaCh, DmaModule, OC32_DMA_BUSY) != HAL_OK)
    return HAL_ERROR;

  Uart->UTCON.UTIE = Packet->IntE;

  /* setup tx dma */
  Uart->UTCON.UTTDMAE = 1U;

  if (UartTxSetup(Uart, Packet->Length, Packet->Data) != HAL_OK)
    return HAL_ERROR;

  /* kick start tx */
  Uart->UTCON.UTTKS = 1U;

  /* return if it is interrupt mode */
  if (Uart->UTCON.UTIE)
  {
    return HAL_OK;
  }
  else
  {
    while (Uart->UTCON.UTTBV == 0)
      ;
    /* clear done flag */
    Uart->UTCON.UTTCLR = 1U;
    Uart->UTCON.UTTDMAE = 0U;
    return HAL_DMA_Update(Uart->UTCON.UTRDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }
}

/**
 * @brief  Rx packet
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @param  Packet: pointer to a HAL_UART_PacketTypedef structure, specifies the rx data packet.
 *                 Packet->Data must not be NULL.
 * @retval HAL_OK: on success (in both polling and interrupt mode)
 * @retval HAL_ERROR: on failure
 * @note   In half-duplex mode, waits for any pending TX DMA to complete first.
 *         In interrupt mode (Packet->IntE != 0), returns immediately; completion is
 *         handled by HAL_UART_IRQHandler. In polling mode, blocks until transfer completes.
 *         Caller must not mask the UART interrupt during the call, or the busy-wait
 *         on UTRDMAE / UTRBV will not terminate.
 */
__sram__ HAL_StatusTypeDef HAL_UART_RX_Packet(MODULE_UART_TypeDef *Uart, HAL_UART_PacketTypedef *Packet)
{
  if (Packet == NULL || Packet->Data == NULL || (Uart != UART_0 && Uart != UART_1))
    return HAL_ERROR;

  /* wait tx if it is half-duplex */
  if (Uart->UTCON.UTHD)
  {
    while (Uart->UTCON.UTTDMAE)
      ;
  }

  /* wait previous packet ends */
  while (Uart->UTCON.UTRDMAE)
    ;

  HAL_DMA_ChTypedef DmaCh = (Uart == UART_0) ? Ut0DmaCh : Ut1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Uart == UART_0) ? OC32_DMA_UART0 : OC32_DMA_UART1;

  if (HAL_DMA_Update(Uart->UTCON.UTTDMAE, DmaCh, DmaModule, OC32_DMA_BUSY) != HAL_OK)
    return HAL_ERROR;

  Uart->UTCON.UTIE = Packet->IntE;

  /* setup rx dma */
  Uart->UTCON.UTRDMAE = 1U;

  if (UartRxSetup(Uart, Packet->Length, Packet->Data) != HAL_OK)
    return HAL_ERROR;

  /* enable rx */
  Uart->UTCON.UTRE = 1U;

  /* return if it is interrupt mode */
  if (Uart->UTCON.UTIE)
    return HAL_OK;
  else
  {
    while (Uart->UTCON.UTRBV == 0)
      ;
    /* clear done flag */
    Uart->UTCON.UTRE = 0U;
    Uart->UTCON.UTRCLR = 1U;
    Uart->UTCON.UTRDMAE = 0U;

    return HAL_DMA_Update(Uart->UTCON.UTTDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }
}

/**
 * @brief  Handles UART interrupt request at user side.
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__sram__ __weak__ HAL_StatusTypeDef HAL_UART_UserIRQ(MODULE_UART_TypeDef *Uart)
{
  return HAL_OK;
}

/**
 * @brief  Handles UART interrupt request.
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Uart is not UART_0/UART_1
 * @note   Invoked from the global interrupt handler. On TX completion (UTTBV set) or
 *         RX completion (UTRBV set), clears the respective done flag, disables the
 *         associated DMA, and updates the DMA channel state to IDLE.
 *         Disables UTIE when the DMA channel is idle, allowing the next transfer to proceed.
 */
__sram__ HAL_StatusTypeDef HAL_UART_IRQHandler(MODULE_UART_TypeDef *Uart)
{
  if (Uart != UART_0 && Uart != UART_1)
    return HAL_ERROR;

  HAL_DMA_ChTypedef DmaCh = (Uart == UART_0) ? Ut0DmaCh : Ut1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Uart == UART_0) ? OC32_DMA_UART0 : OC32_DMA_UART1;
  HAL_StatusTypeDef status = HAL_OK;

  if (Uart->UTCON.UTTBV)
  {
    status = HAL_UART_UserIRQ(Uart);
    Uart->UTCON.UTTCLR = 1U;
    Uart->UTCON.UTTDMAE = 0U;
    /* if Rx is still in progress, keep the DMA state as busy */
    HAL_DMA_Update(Uart->UTCON.UTRDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }
  else if (Uart->UTCON.UTRBV)
  {
    status = HAL_UART_UserIRQ(Uart);
    Uart->UTCON.UTRCLR = 1U;
    Uart->UTCON.UTRE = 0U;
    Uart->UTCON.UTRDMAE = 0U;
    /* if Tx is still in progress, keep the DMA state as busy */
    HAL_DMA_Update(Uart->UTCON.UTTDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }

  /* disable interrupt only if the DMA channel is idle: both Tx and Rx are done */
  if (DMAChState[DmaCh] == OC32_DMA_IDLE)
    Uart->UTCON.UTIE = 0U;

  return status;
}

#endif /* HAL_UART_ENABLE */