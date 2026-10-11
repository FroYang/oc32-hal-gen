/**
 ******************************************************************************
 * @file    oc32_hal_crc.c
 * @author
 * @brief   OC32 HAL CRC module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal.h"

#ifdef HAL_CRC_ENABLE
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static int CrcDmaCh;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the CRC peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_CRCInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CRC_Init(HAL_CRCInitTypeDef *Init)
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
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CRC_DeInit(void)
{
  WRITE_SR(CRCPOLY, 0);
  WRITE_SR(CRCREG, 0);
  WRITE_SR(CRCCON, 0);
  CrcDmaCh = 0;

  return HAL_OK;
}

/**
 * @brief  Caculates once
 * @param  Data: data to be caculate
 * @retval CRC result
 */
__sram__ uint32_t CrcOnce(uint32_t Data)
{
  CRCCON->CRCIDM = CRC_IDM_WRITE_CRCIN;
  CRCIN = Data;
  /* kick start */
  CRCCON->CRCKS = 1U;
  /* wait caculating */
  while (!CRCCON->CRCCF)
    ;
  CRCCON->CRCCCLR = 1U;
  return READ_SR(CRCOUT);
}

/**
 * @brief  Caculates 1 byte of data (LSB first default)
 * @param  Data: data to be caculate
 * @retval CRC result
 */
__sram__ uint32_t HAL_CRC_OneByte(uint32_t Data)
{
  /* wait previous packet ends */
  while (CRCCON->CRCIE)
    ;

  WRITE_SR(CRCBC, 1U);
  return CrcOnce(Data);
}

/**
 * @brief  Caculates 1 word of data (LSB first default)
 * @param  Data: data to be caculate
 * @retval CRC result
 */
__sram__ uint32_t HAL_CRC_OneWord(uint32_t Data)
{
  /* wait previous packet ends */
  while (CRCCON->CRCIE)
    ;

  WRITE_SR(CRCBC, 4U);
  return CrcOnce(Data);
}

/**
 * @brief  Caculates 1 packet (LSB first default)
 * @param  Packet: pointer to a HAL_CRCPacketTypedef structure.
 * @retval CRC result
 */
__sram__ HAL_ResultTypedef HAL_CRC_Packet(HAL_CRCPacketTypedef *Packet)
{
  HAL_ResultTypedef Result = {0};

  Result.Error = HAL_ERROR;
  if (Packet == NULL || Packet->Data == NULL || Packet->IDM == CRC_IDM_WRITE_CRCIN)
    return Result;

  /* wait previous packet ends */
  while (CRCCON->CRCIE)
    ;

  /* setup interrupt */
  if (Packet->Mode == CRC_MODE_INT)
  {
    CRCCON->CRCIE = 1U;
  }

  if (Packet->IDM == CRC_IDM_READ_DMA)
  {

    if (HAL_DMA_Update(CrcDmaCh, OC32_DMA_CRC, OC32_DMA_BUSY) != HAL_OK)
      return Result;

    /* setup DMA address */
    WRITE_SR(CRCDA, (uint32_t)Packet->Data);
  }
  else
  {
    CRCCON->CRCPDC = CrcDmaCh;
  }

  CRCCON->CRCIDM = Packet->IDM;
  WRITE_SR(CRCBC, Packet->Length);

  /* kickstart */
  CRCCON->CRCKS = 1U;

  /* if interrupt mode, return value is not the correct result, user should get it in IRQ
   * if normal mode, return value is the crc result */
  if (CRCCON->CRCIE)
  {
    Result.Error = HAL_OK;
    return Result;
  }
    
  /* wait caculating */
  while (!CRCCON->CRCCF)
    ;
  CRCCON->CRCCCLR = 1U;
  Result.Error = HAL_DMA_Update(CrcDmaCh, OC32_DMA_CRC, OC32_DMA_IDLE);
  Result.Data = HAL_CRC_GetResult();
  return Result;
}

/**
 * @brief  Handles CRC interrupt request.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__sram__ __weak__ HAL_StatusTypeDef HAL_CRC_UserIRQ(void)
{
  return HAL_OK;
}

/**
 * @brief  Handles CRC interrupt request.
 */
__sram__ HAL_StatusTypeDef HAL_CRC_IRQHandler(void)
{
  CRCCON->CRCCCLR = 1U;
  CRCCON->CRCIE = 0;
  HAL_DMA_Update(CrcDmaCh, OC32_DMA_CRC, OC32_DMA_IDLE);
  return HAL_CRC_UserIRQ();
}

#endif /* HAL_CRC_ENABLE */