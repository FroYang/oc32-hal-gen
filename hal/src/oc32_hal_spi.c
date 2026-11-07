/**
 ******************************************************************************
 * @file    oc32_hal_spi.c
 * @author
 * @brief   OC32 HAL SPI module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal.h"

#ifdef HAL_SPI_ENABLE
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static HAL_DMA_ChTypedef Spi0DmaCh = 0U;
static HAL_DMA_ChTypedef Spi1DmaCh = 0U;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the SPI peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_SPI_InitTypeDef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or Init->Spi is not SPI_0/SPI_1
 */
HAL_StatusTypeDef HAL_SPI_Init(HAL_SPI_InitTypeDef *Init)
{
  if (Init == NULL)
    return HAL_ERROR;

  /* record dma channel per spi instance */
  if (Init->Spi == SPI_0)
  {
    Spi0DmaCh = Init->DmaCh;
  }
  else if (Init->Spi == SPI_1)
  {
    Spi1DmaCh = Init->DmaCh;
  }
  else
  {
    return HAL_ERROR;
  }

  /* setup clk mode */
  Init->Spi->SPICON.SPICIH = Init->Mode & 0x1U;

  /* setup data mode */
  Init->Spi->SPICON.SPIDSP = (Init->Mode >> 1U) & 0x1U;

  /* setup master clock divider */
  Init->Spi->SPICON.SPIMCD = Init->ClkDiv;

  /* setup master/slave mode */
  Init->Spi->SPICON.SPIS = Init->Slave;

  /* setup master CS output or not */
  Init->Spi->SPICON.SPIMSOE = Init->CsOut;

  /* setup duplex mode */
  Init->Spi->SPICON.SPIHD = Init->Duplex;

  /* select half-duplex pin */
  Init->Spi->SPICON.SPISIOP = Init->HalfPin;

  /* setup first bit */
  Init->Spi->SPICON.SPILF = Init->FirstBit;

  /* setup pins */
  Init->Spi->SPICON.SPIPORTS = Init->Pins;

  return HAL_OK;
}

/**
 * @brief  DeInitializes the SPI peripheral.
 * @param  Spi: pointer to a MODULE_SPI_TypeDef structure, specifies which SPI
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Spi is not SPI_0/SPI_1
 * @note   Resets SPICON to 0 then clears the TX busy flag.
 */
HAL_StatusTypeDef HAL_SPI_DeInit(MODULE_SPI_TypeDef *Spi)
{
  if (Spi != SPI_0 && Spi != SPI_1)
    return HAL_ERROR;

  *(volatile uint32_t *)&Spi->SPICON = 0U;
  Spi->SPICON.SPITCLR = 1U;
  return HAL_OK;
}

/**
 * @brief  SPI Tx Setup
 * @param  Spi: pointer to a MODULE_SPI_TypeDef structure, specifies which SPI
 * @param  Length: length of data in 32-bit words (max 4; TransferOnce enforces this)
 * @param  Data: pointer to the tx data buffer (must not be NULL on the tx path)
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Spi is not SPI_0/SPI_1
 * @note   Blocks until any pending SPI interrupt (SPIIE) has completed.
 */
__sram__ HAL_StatusTypeDef SpiTxSetup(MODULE_SPI_TypeDef *Spi, uint32_t Length, uint32_t *Data)
{

  if (Spi != SPI_0 && Spi != SPI_1)
    return HAL_ERROR;

  while (Spi->SPICON.SPIIE)
    ;
  Spi->SPITBC = Length;
  Spi->SPITBUF = *Data;
  Spi->SPITDA = (uint32_t)Data;

  return HAL_OK;
}

/**
 * @brief  SPI RX Setup
 * @param  Spi: pointer to a MODULE_SPI_TypeDef structure, specifies which SPI
 * @param  Length: length of data in 32-bit words
 * @param  Data: pointer to the rx data buffer (may be NULL; in that case
 *               SPIRDA is set to 0, which is harmless when DMA is not enabled,
 *               e.g. inside HAL_SPI_TransferOnce where rx result is read back
 *               from SPIRBUF instead of via DMA)
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Spi is not SPI_0/SPI_1
 * @note   Blocks until any pending SPI interrupt (SPIIE) has completed.
 */
__sram__ HAL_StatusTypeDef SpiRxSetup(MODULE_SPI_TypeDef *Spi, uint32_t Length, uint32_t *Data)
{
  if (Spi != SPI_0 && Spi != SPI_1)
    return HAL_ERROR;

  while (Spi->SPICON.SPIIE)
    ;
  Spi->SPITBC = Length;
  Spi->SPIRDA = (uint32_t)Data;

  return HAL_OK;
}


/**
 * @brief  full/half duplex transfer once
 * @param  Spi: pointer to a MODULE_SPI_TypeDef structure, specifies which SPI
 * @param  Data: pointer to a HAL_SPI_PacketTypedef structure, specifies the data packet.
 *               Data->RxData may be NULL; rx result is returned via Result.Data (read from SPIRBUF).
 * @retval HAL_ResultTypedef:
 *         .Data  = received rx data word (in full/half-duplex rx path), 0 otherwise
 *         .Error = HAL_OK on success, HAL_ERROR on failure
 * @note   This function blocks until transfer completes (no DMA, no interrupt).
 *         Caller must not mask the SPI interrupt during the call, or the busy-wait
 *         on SPITBV will not terminate.
 */
__sram__ HAL_ResultTypedef HAL_SPI_TransferOnce(MODULE_SPI_TypeDef *Spi, HAL_SPI_PacketTypedef *Data)
{
  HAL_ResultTypedef Result = {0U, HAL_ERROR};

  if (Data == NULL || (Spi != SPI_0 && Spi != SPI_1))
    return Result;  

  /* setup tx config */
  if ((Spi->SPICON.SPIHD == SPI_HALF_DUPLEX && Data->HalfRx == SPI_HALF_TX) || (Spi->SPICON.SPIHD == SPI_FULL_DUPLEX))
  {
    if ((SpiTxSetup(Spi, Data->Length, Data->TxData) != HAL_OK) || (Data->Length > 4U) || Data->TxData == NULL)
    {
      return Result;
    }
  } 
  
  /* setup rx config */
  if ((Spi->SPICON.SPIHD == SPI_HALF_DUPLEX && Data->HalfRx == SPI_HALF_RX) || (Spi->SPICON.SPIHD == SPI_FULL_DUPLEX))
  {
    if ((SpiRxSetup(Spi, Data->Length, Data->RxData) != HAL_OK) || (Data->Length > 4U))
    {
      return Result;
    }
  }

  /* kick start */
  Spi->SPICON.SPIKS = 1;
  while (Spi->SPICON.SPITBV == 0)
    ;
  /* clear done flag */
  Spi->SPICON.SPITCLR = 1;
  Result.Data = Spi->SPIRBUF;
  Result.Error = HAL_OK;
  return Result;
}

/**
 * @brief  Tx/Rx packet
 * @param  Spi: pointer to a MODULE_SPI_TypeDef structure, specifies which SPI
 * @param  Packet: pointer to a HAL_SPI_PacketTypedef structure, specifies the tx/rx data packet.
 *                 Packet->TxData may be NULL only when TX is not active (half-duplex RX only).
 *                 Packet->RxData may be NULL only when RX is not active (half-duplex TX only).
 * @retval HAL_OK: on success (in both polling and interrupt mode)
 * @retval HAL_ERROR: on failure
 * @note   In interrupt mode (Packet->IntE != 0), returns immediately; completion is
 *         handled by HAL_SPI_IRQHandler. In polling mode, blocks until transfer completes.
 *         Caller must not mask the SPI interrupt during the call, or the busy-wait
 *         on SPIDMAE / SPITBV will not terminate.
 */
__sram__ HAL_StatusTypeDef HAL_SPI_TransferPacket(MODULE_SPI_TypeDef *Spi, HAL_SPI_PacketTypedef *Packet)
{
  if (Packet == NULL || 
     (Packet->TxData == NULL && (Spi->SPICON.SPIHD == SPI_FULL_DUPLEX || Packet->HalfRx == SPI_HALF_TX)) || 
     (Packet->RxData == NULL && (Spi->SPICON.SPIHD == SPI_FULL_DUPLEX || Packet->HalfRx == SPI_HALF_RX)) || 
     (Spi != SPI_0 && Spi != SPI_1))
    return HAL_ERROR;

  /* wait previous packet ends */
  while (Spi->SPICON.SPIDMAE)
    ;

  HAL_DMA_ChTypedef DmaCh = (Spi == SPI_0) ? Spi0DmaCh : Spi1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Spi == SPI_0) ? OC32_DMA_SPI0 : OC32_DMA_SPI1;
  if (HAL_DMA_Update(Spi->SPICON.SPIDMAE, DmaCh, DmaModule, OC32_DMA_BUSY) != HAL_OK)
    return HAL_ERROR;

  Spi->SPICON.SPIIE = Packet->IntE;

  /* setup dma */
  Spi->SPICON.SPIDMAE = 1U;

  if (SpiTxSetup(Spi, Packet->Length, Packet->TxData) != HAL_OK)
    return HAL_ERROR;

  if (SpiRxSetup(Spi, Packet->Length, Packet->RxData) != HAL_OK)
    return HAL_ERROR;

  /* setup half duplex tx/rx */
  Spi->SPICON.SPIHDR = Packet->HalfRx;

  /* kick start tx */
  Spi->SPICON.SPIKS = 1U;

  /* return if it is interrupt mode */
  if (Spi->SPICON.SPIIE)
  {
    return HAL_OK;
  }
  else
  {
    while (Spi->SPICON.SPITBV == 0)
      ;
    /* clear done flag */
    Spi->SPICON.SPITCLR = 1U;
    Spi->SPICON.SPIDMAE = 0U;
    return HAL_DMA_Update(Spi->SPICON.SPIDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }
}

/**
 * @brief  Handles SPI interrupt request at user side.
 * @param  Spi: pointer to a MODULE_SPI_TypeDef structure, specifies which SPI
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__sram__ __weak__ HAL_StatusTypeDef HAL_SPI_UserIRQ(MODULE_SPI_TypeDef *Spi)
{
  return HAL_OK;
}

/**
 * @brief  Handles SPI interrupt request.
 * @param  Spi: pointer to a MODULE_SPI_TypeDef structure, specifies which SPI
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Spi is not SPI_0/SPI_1
 * @note   Invoked from the global interrupt handler. On transfer completion (SPITBV set),
 *         clears SPITCLR, disables DMA (SPIDMAE=0), and updates the DMA channel state to IDLE.
 *         Disables SPIIE when the DMA channel is idle, allowing the next transfer to proceed.
 */
__sram__ HAL_StatusTypeDef HAL_SPI_IRQHandler(MODULE_SPI_TypeDef *Spi)
{
  if (Spi != SPI_0 && Spi != SPI_1)
    return HAL_ERROR;

  HAL_DMA_ChTypedef DmaCh = (Spi == SPI_0) ? Spi0DmaCh : Spi1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Spi == SPI_0) ? OC32_DMA_SPI0 : OC32_DMA_SPI1;
  HAL_StatusTypeDef status = HAL_OK;

  if (Spi->SPICON.SPITBV)
  {
    status = HAL_SPI_UserIRQ(Spi);
    Spi->SPICON.SPITCLR = 1U;
    Spi->SPICON.SPIDMAE = 0U;
    HAL_DMA_Update(Spi->SPICON.SPIDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }

  if (DMAChState[DmaCh] == OC32_DMA_IDLE)
    Spi->SPICON.SPIIE = 0U;

  return status;
}

#endif /* HAL_SPI_ENABLE */