/**
 ******************************************************************************
 * @file    oc32_hal_crc.c
 * @author
 * @brief   OC32 HAL CRC module driver.
 ******************************************************************************
 */



/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_crc.h"

#ifdef HAL_CRC_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static HAL_DMA_ChTypedef CrcDmaCh;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the CRC peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_CRC_Inittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL
 */
__sram__ HAL_StatusTypedef HAL_CRC_Init(HAL_CRC_InitTypedef *Init)
{
  if (Init == NULL)
    return HAL_ERROR;

  CRCPOLY = Init->Poly;
  CRCREG = Init->InitVal;
  CRCCON->CRCREGL = Init->Bits;
  CRCCON->CRCREFI = Init->RefIn;
  CRCCON->CRCREFO = Init->RefOut;
  CRCCON->CRCCPLO = Init->CplOut;
  CRCCON->CRCRVB = Init->Rvb;
  CrcDmaCh = Init->DmaCh;
  return HAL_OK;
}

/**
 * @brief  DeInitializes the CRC peripheral.
 * @retval HAL_OK: on success
 * @note   Resets CRCPOLY, CRCREG, and CRCCON to 0, and records the DMA channel
 *         state as idle.
 */
HAL_StatusTypedef HAL_CRC_DeInit(void)
{
  WRITE_SR(CRCPOLY, 0);
  WRITE_SR(CRCREG, 0);
  WRITE_SR(CRCCON, 0);
  CrcDmaCh = 0;

  return HAL_OK;
}

/**
 * @brief  Calculates CRC once for a single 32-bit value
 * @param  Data: data to be calculated
 * @retval CRC result
 * @note   Internal helper; kicks off the CRC computation and waits for completion.
 */
__sram__ uint32_t CrcOnce(uint32_t Data)
{
  CRCCON->CRCIDM = CRC_IDM_WRITE_CRCIN;
  CRCIN = Data;
  /* kick start */
  CRCCON->CRCKS = 1U;
  /* wait calculating */
  while (CRCCON->CRCCF == HAL_FLAG_INVALID)
    ;
  CRCCON->CRCCCLR = HAL_ENABLE;
  return READ_SR(CRCOUT);
}

/**
 * @brief  Calculates CRC of 1 byte of data (LSB first default)
 * @param  Data: data to be calculated
 * @retval CRC result
 * @note   Blocks until the CRC computation completes.
 */
__sram__ uint32_t HAL_CRC_OneByte(uint32_t Data)
{
  /* wait previous packet ends */
  while (CRCCON->CRCIE == HAL_ENABLE)
    ;

  WRITE_SR(CRCBC, 1U);
  return CrcOnce(Data);
}

/**
 * @brief  Calculates CRC of 1 word of data (LSB first default)
 * @param  Data: data to be calculated
 * @retval CRC result
 * @note   Blocks until the CRC computation completes.
 */
__sram__ uint32_t HAL_CRC_OneWord(uint32_t Data)
{
  /* wait previous packet ends */
  while (CRCCON->CRCIE == HAL_ENABLE)
    ;

  WRITE_SR(CRCBC, 4U);
  return CrcOnce(Data);
}

/**
 * @brief  Calculates CRC of a data packet (LSB first default)
 * @param  Packet: pointer to a HAL_CRC_Packettypedef structure.
 * @retval HAL_ResultTypedef:
 *         .Data  = CRC result (in polling mode)
 *         .Error = HAL_OK on success, HAL_ERROR on failure
 * @note   In interrupt mode (Packet->IntE != 0), Result.Data is not valid;
 *         the user should obtain the result in the IRQ handler.
 *         In polling mode, blocks until computation completes.
 */
__sram__ HAL_ResultTypedef HAL_CRC_Packet(HAL_CRC_PacketTypedef *Packet)
{
  HAL_ResultTypedef Result = {0U, HAL_ERROR};
  if (Packet == NULL || Packet->Data == NULL || Packet->IDM == CRC_IDM_WRITE_CRCIN)
    return Result;

  /* wait previous packet ends */
  while (CRCCON->CRCIE == HAL_ENABLE)
    ;

  /* setup interrupt */
  CRCCON->CRCIE = Packet->IntE;

  if (Packet->IDM == CRC_IDM_READ_DMA)
  {
    /* READ_DMA: CRC acts as the initiating peripheral. The channel is bound
     * in the DMA controller (OC32_DMA_CRC role) and the source address in
     * CRCDA. CRCPDC is NOT used in this mode. */
    if (DmaUpdate(0U, CrcDmaCh, OC32_DMA_CRC, OC32_DMA_BUSY) != HAL_OK)
      return Result;

    /* setup DMA source address */
    WRITE_SR(CRCDA, (uint32_t)Packet->Data);
  }
  else
  {
    /* CAPTURE_DMAIN/OUT: CRC passively monitors a DMA channel; CRCPDC
     * selects which channel's data it listens to. */
    CRCCON->CRCPDC = CrcDmaCh;
  }

  CRCCON->CRCIDM = Packet->IDM;
  WRITE_SR(CRCBC, Packet->Length);

  /* kickstart */
  CRCCON->CRCKS = HAL_ENABLE;

  /* if interrupt mode, return value is not the correct result, user should get it in IRQ
   * if normal mode, return value is the crc result */
  if (CRCCON->CRCIE == HAL_ENABLE)
  {
    Result.Status = HAL_OK;
    return Result;
  }

  /* wait calculating */
  while (CRCCON->CRCCF == HAL_FLAG_INVALID)
    ;
  CRCCON->CRCCCLR = HAL_ENABLE;
  Result.Status = DmaUpdate(0U, CrcDmaCh, OC32_DMA_CRC, OC32_DMA_IDLE);
  Result.Data = HAL_CRC_GetResult();
  return Result;
}

/**
 * @brief  Handles CRC interrupt request at user side.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_CRC_UserIRQ(void)
{
  return HAL_OK;
}

/**
 * @brief  Handles CRC interrupt request.
 * @retval Return value of HAL_CRC_UserIRQ
 * @note   Invoked from the global interrupt handler. Clears the CRC done flag,
 *         disables the CRC interrupt, and updates the DMA channel state to IDLE.
 */
__sram__ HAL_StatusTypedef HAL_CRC_IRQHandler(void)
{
  HAL_StatusTypedef status = HAL_CRC_UserIRQ();
  CRCCON->CRCCCLR = HAL_ENABLE;
  CRCCON->CRCIE = HAL_DISABLE;
  DmaUpdate(0U, CrcDmaCh, OC32_DMA_CRC, OC32_DMA_IDLE);
  return status;
}

#endif /* HAL_CRC_ENABLE */