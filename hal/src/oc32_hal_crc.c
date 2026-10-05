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

  if (Init->Mode == CRC_MODE_INT)
  {
    CRCCON->CRCIE = 1U;
    HIE->FLCRCHIE = 1U;
    HIPL0->FLCRCHIPL0 = (Init->IntPri & 1u);        /* Bit 0 of priority */
    HIPL1->FLCRCHIPL1 = ((Init->IntPri >> 1) & 1u); /* Bit 1 of priority */
  }

  return HAL_OK;
}

/**
 * @brief  DeInitializes the CRC peripheral.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CRC_DeInit()
{
  CRCPOLY = 0;
  CRCREG = 0;
  CRCCON = 0;
  HIE->FLCRCHIE = 0;
  HIPL0->FLCRCHIPL0 = 0;
  HIPL1->FLCRCHIPL1 = 0;

  return HAL_OK;
}

/**
 * @brief  Handles CRC interrupt request.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ void HAL_CRC_IRQHandler(void)
{
  CRCCON->CRCCCLR = 1U;
}



/**
 * @brief  Get CRC result
 * @retval CRC result
 */
uint32_t HAL_CRC_GetResult(void)
{
  /* wait caculating */
  while (!CRCCON->CRCCF)
    ;
  CRCCON->CRCCCLR = 1U;
  return READ_SR(CRCOUT);
}


/**
 * @brief  Caculates 1 byte of data
 * @param  Data: data to be caculate
 * @retval CRC result
 */
uint32_t CRCOnce(uint32_t Data)
{
  CRCCON->CRCIDM = CRC_IDM_WRITE_CRCIN;
  CRCIN = Data;
  /* kick start */
  CRCCON->CRCKS = 1U;
  return HAL_CRC_GetResult();
}


/**
 * @brief  Caculates 1 byte of data
 * @param  Data: data to be caculate
 * @retval CRC result
 */
uint32_t HAL_CRC_OneByte(uint32_t Data)
{
  WRITE_SR(CRCBC, 1U);
  return CRCOnce(Data);
}



/**
 * @brief  Caculates 1 word of data
 * @param  Data: data to be caculate
 * @retval CRC result
 */
uint32_t HAL_CRC_OneWord(uint32_t Data)
{
  WRITE_SR(CRCBC, 4U);
  return CRCOnce(Data);
}


/**
 * @brief  Caculates 1 packet
 * @param  Packet: pointer to a HAL_CRCPacketTypedef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_CRC_Packet(HAL_CRCPacketTypedef *Packet)
{
  if (Packet == NULL || Packet->Data == NULL)
    return HAL_ERROR;

  if (Packet->Mode == CRC_IDM_WRITE_CRCIN)
  {
    return HAL_ERROR;
  }

  if (Packet->Mode == CRC_IDM_READ_DMA)
  {
    
    /* setup DMA Channel */
    switch (Packet->DMA)
    {
    case OC32_DMA_CH0:
        DMACON->DMACH0 = OC32_DMA_FLASH;
        break;
    case OC32_DMA_CH1:
        DMACON->DMACH1 = OC32_DMA_FLASH;
        break;
    case OC32_DMA_CH2:
        DMACON->DMACH2 = OC32_DMA_FLASH;
        break;
    case OC32_DMA_CH3:
        DMACON->DMACH3 = OC32_DMA_FLASH;
        break;
    default:
        return HAL_ERROR;
    }

    /* setup DMA address */
    WRITE_SR(CRCDA, &(Packet->Data));

  }
  else
  {
    CRCCON->CRCPDC = Packet->DMA;
  }

  CRCCON->CRCIDM = Packet->Mode;
  WRITE_SR(CRCBC, Packet->Length);

  /* kickstart */
  CRCCON->CRCKS = 1U;

  return HAL_OK;
}


#endif /* HAL_CRC_ENABLE */