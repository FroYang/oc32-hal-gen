/**
 ******************************************************************************
 * @file    oc32_hal_spi.c
 * @author
 * @brief   OC32 HAL SPI module driver.
 ******************************************************************************
 */



/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_spi.h"

#ifdef HAL_SPI_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static HAL_DMA_ChTypedef Spi0DmaCh = 0U;
static HAL_DMA_ChTypedef Spi1DmaCh = 0U;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  SPI Tx Setup
 * @param  Spi: pointer to a MODULE_SPI_typedef structure, specifies which SPI
 * @param  Length: length of data in bytes
 * @param  Data: pointer to the tx data buffer (must not be NULL on the tx path)
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Spi is not SPI_0/SPI_1
 * @note   Blocks until any pending SPI interrupt (SPIIE) has completed.
 */
__sram__ HAL_StatusTypedef SpiTxSetup(MODULE_SPI_Typedef *Spi, uint32_t Length, uint32_t *Data)
{

  if (Spi != SPI_0 && Spi != SPI_1)
    return HAL_ERROR;

  while (Spi->SPICON.SPIIE == HAL_ENABLE)
    ;
  Spi->SPITBC = Length;
  Spi->SPITBUF = *Data;
  Spi->SPITDA = (uint32_t)Data;

  return HAL_OK;
}

/**
 * @brief  SPI RX Setup
 * @param  Spi: pointer to a MODULE_SPI_typedef structure, specifies which SPI
 * @param  Length: length of data in bytes
 * @param  Data: pointer to the rx data buffer (may be NULL; in that case
 *               SPIRDA is set to 0, which is harmless when DMA is not enabled,
 *               e.g. for short (<= 4 byte) transfers where rx result is read
 *               back from SPIRBUF instead of via DMA)
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Spi is not SPI_0/SPI_1
 * @note   Blocks until any pending SPI interrupt (SPIIE) has completed.
 */
__sram__ HAL_StatusTypedef SpiRxSetup(MODULE_SPI_Typedef *Spi, uint32_t Length, uint32_t *Data)
{
  if (Spi != SPI_0 && Spi != SPI_1)
    return HAL_ERROR;

  while (Spi->SPICON.SPIIE == HAL_ENABLE)
    ;
  Spi->SPITBC = Length;
  Spi->SPIRDA = (uint32_t)Data;

  return HAL_OK;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the SPI peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_SPI_Inittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL or Init->Spi is not SPI_0/SPI_1
 */
HAL_StatusTypedef HAL_SPI_Init(HAL_SPI_InitTypedef *Init)
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
  Init->Spi->SPICON.SPIMSOE = Init->CsOutE;

  /* setup duplex mode */
  Init->Spi->SPICON.SPIHD = Init->Duplex;

  /* select half-duplex pin */
  Init->Spi->SPICON.SPISIOP = Init->HalfPinShareE;

  /* setup first bit */
  Init->Spi->SPICON.SPILF = Init->FirstBit;

  /* setup pins */
  Init->Spi->SPICON.SPIPORTS = Init->Pins;

  return HAL_OK;
}

/**
 * @brief  DeInitializes the SPI peripheral.
 * @param  Spi: pointer to a MODULE_SPI_typedef structure, specifies which SPI
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Spi is not SPI_0/SPI_1
 * @note   Resets SPICON to 0 then clears the TX busy flag.
 */
HAL_StatusTypedef HAL_SPI_DeInit(MODULE_SPI_Typedef *Spi)
{
  if (Spi != SPI_0 && Spi != SPI_1)
    return HAL_ERROR;

  WRITE_SR(&Spi->SPICON, 0U);
  Spi->SPICON.SPITCLR = HAL_ENABLE;
  HAL_DMA_ChTypedef DmaCh = (Spi == SPI_0) ? Spi0DmaCh : Spi1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Spi == SPI_0) ? OC32_DMA_SPI0 : OC32_DMA_SPI1;
  HAL_DMA_Update(Spi->SPICON.SPIDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  return HAL_OK;
}

/**
 * @brief  Tx/Rx packet
 * @param  Spi: pointer to a MODULE_SPI_typedef structure, specifies which SPI
 * @param  Packet: pointer to a HAL_SPI_Packettypedef structure, specifies the tx/rx data packet.
 *                 Packet->TxData may be NULL only when TX is not active (half-duplex RX only).
 *                 Packet->RxData may be NULL only when RX is not active (half-duplex TX only).
 * @retval HAL_OK: on success (in both polling and interrupt mode)
 * @retval HAL_ERROR: on failure
 * @note   In interrupt mode (Packet->IntE != 0), returns immediately; completion is
 *         handled by HAL_SPI_IRQHandler. In polling mode, blocks until transfer completes.
 *         Caller must not mask the SPI interrupt during the call, or the busy-wait
 *         on SPIDMAE / SPITBV will not terminate.
 */
__sram__ HAL_StatusTypedef HAL_SPI_Transfer(MODULE_SPI_Typedef *Spi, HAL_SPI_PacketTypedef *Packet)
{
  if (Packet == NULL ||
      (Packet->TxData == NULL && (Spi->SPICON.SPIHD == SPI_FULL_DUPLEX || Packet->HalfRx == SPI_HALF_TX)) ||
      (Packet->RxData == NULL && (Spi->SPICON.SPIHD == SPI_FULL_DUPLEX || Packet->HalfRx == SPI_HALF_RX)) ||
      (Spi != SPI_0 && Spi != SPI_1))
    return HAL_ERROR;

  /* wait previous packet ends */
  while (Spi->SPICON.SPIDMAE)
    ;

  /* setup dma if length is greater than 4 bytes */
  HAL_DMA_ChTypedef DmaCh = (Spi == SPI_0) ? Spi0DmaCh : Spi1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Spi == SPI_0) ? OC32_DMA_SPI0 : OC32_DMA_SPI1;
  if ((Packet->Length > 4U) || (Packet->IntE == HAL_ENABLE))
  {
    if (HAL_DMA_Update(Spi->SPICON.SPIDMAE, DmaCh, DmaModule, OC32_DMA_BUSY) != HAL_OK)
      return HAL_ERROR;

    /* setup dma */
    Spi->SPICON.SPIDMAE = HAL_ENABLE;
  }

  if (SpiTxSetup(Spi, Packet->Length, Packet->TxData) != HAL_OK)
    return HAL_ERROR;

  if (SpiRxSetup(Spi, Packet->Length, Packet->RxData) != HAL_OK)
    return HAL_ERROR;

  /* setup half duplex tx/rx */
  Spi->SPICON.SPIHDR = Packet->HalfRx;

  /* setup interrupt enable */
  Spi->SPICON.SPIIE = Packet->IntE;

  /* kick start */
  Spi->SPICON.SPIKS = HAL_ENABLE;

  /* return if it is interrupt mode */
  if (Spi->SPICON.SPIIE == HAL_ENABLE)
  {
    return HAL_OK;
  }

  /* if interrupt disabled, wait for data transfer to complete */
  while (Spi->SPICON.SPITBV == HAL_FLAG_INVALID)
    ;

  /* copy the rx data in buffer for non-dma mode */
  if (Spi->SPICON.SPIDMAE == HAL_DISABLE)
    Packet->RxData[0] = Spi->SPIRBUF;

  /* clear done flag */
  Spi->SPICON.SPITCLR = HAL_ENABLE;
  Spi->SPICON.SPIDMAE = HAL_DISABLE;
  return HAL_DMA_Update(Spi->SPICON.SPIDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
}

/**
 * @brief  Handles SPI interrupt request at user side.
 * @param  Spi: pointer to a MODULE_SPI_typedef structure, specifies which SPI
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__sram__ __weak__ HAL_StatusTypedef HAL_SPI_UserIRQ(MODULE_SPI_Typedef *Spi)
{
  return HAL_OK;
}

/**
 * @brief  Handles SPI interrupt request.
 * @param  Spi: pointer to a MODULE_SPI_typedef structure, specifies which SPI
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Spi is not SPI_0/SPI_1
 * @note   Invoked from the global interrupt handler. On transfer completion (SPITBV set),
 *         clears SPITCLR, disables DMA (SPIDMAE=0), and updates the DMA channel state to IDLE.
 *         Disables SPIIE when the DMA channel is idle, allowing the next transfer to proceed.
 */
__sram__ HAL_StatusTypedef HAL_SPI_IRQHandler(MODULE_SPI_Typedef *Spi)
{
  if (Spi != SPI_0 && Spi != SPI_1)
    return HAL_ERROR;

  HAL_DMA_ChTypedef DmaCh = (Spi == SPI_0) ? Spi0DmaCh : Spi1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Spi == SPI_0) ? OC32_DMA_SPI0 : OC32_DMA_SPI1;
  HAL_StatusTypedef status = HAL_OK;

  if (Spi->SPICON.SPITBV == HAL_FLAG_VALID)
  {
    status = HAL_SPI_UserIRQ(Spi);
    Spi->SPICON.SPITCLR = HAL_ENABLE;
    Spi->SPICON.SPIDMAE = HAL_DISABLE;
    HAL_DMA_Update(Spi->SPICON.SPIDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }

  if (DMAChState[DmaCh] == OC32_DMA_IDLE)
    Spi->SPICON.SPIIE = HAL_DISABLE;

  return status;
}

#endif /* HAL_SPI_ENABLE */