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
static int Ut0DmaCh = 0;
static int Ut1DmaCh = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the UART peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_UARTInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_UART_Init(HAL_UARTInitTypeDef *Init)
{
  if (Init == NULL || (Init->UTCON != UT0CON && Init->UTCON != UT1CON))
    return HAL_ERROR;

  /* setup interrupt priority */
  if (Init->UTCON == UT0CON)
  {
    Ut0DmaCh = Init->DmaCh;
  }

  if (Init->UTCON == UT1CON)
  {
    Ut1DmaCh = Init->DmaCh;
  }

  /* setup parity mode */
  Init->UTCON->UTPAR = Init->Parity;

  /* setup bits */
  Init->UTCON->UTM = Init->Bits;

  /* setup duplex */
  Init->UTCON->UTHD = Init->Duplex;

  /* setup pins */
  Init->UTCON->UTPORTS = Init->Pins;

  /* calculates cycles per bit */
  uint32_t bitcycles = SystemClock / Init->Baud;
  /* round up */
  if ((SystemClock % Init->Baud) > (Init->Baud >> 1))
    bitcycles++;

  /* setup baudrate */
  Init->UTCON->UTBR = bitcycles / 8;
  Init->UTCON->UTBRA = bitcycles % 8;

  return HAL_OK;
}

/**
 * @brief  DeInitializes the UART peripheral.
 * @param  UTCON: pointer to a UTCON_TypeDef structure, specifies which UART
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_UART_DeInit(UTCON_TypeDef *UTCON)
{
  if (UTCON != UT0CON && UTCON != UT1CON)
    return HAL_ERROR;

  WRITE_SR(UTCON, 0);

  UTCON->UTTCLR = 1U;
  UTCON->UTRCLR = 1U;
  return HAL_OK;
}

/**
 * @brief  UART TX Setup
 * @param  UTCON: pointer to a UTCON_TypeDef structure, specifies which UART
 * @param  Length: length of data
 * @param  Data: data to be sent
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef UartTxSetup(UTCON_TypeDef *UTCON, uint32_t Length, uint32_t *Data)
{
  if (UTCON == UT0CON)
  {
    while (UTCON->UTIE)
      ;
    WRITE_SR(UT0TBC, Length);
    WRITE_SR(UT0TBUF, *Data);
    WRITE_SR(UT0TDA, (uint32_t)Data);
  }
  else if (UTCON == UT1CON)
  {
    while (UTCON->UTIE)
      ;
    WRITE_SR(UT1TBC, Length);
    WRITE_SR(UT1TBUF, *Data);
    WRITE_SR(UT1TDA, (uint32_t)Data);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
 * @brief  UART RX Setup
 * @param  UTCON: pointer to a UTCON_TypeDef structure, specifies which UART
 * @param  Length: length of data
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef UartRxSetup(UTCON_TypeDef *UTCON, uint32_t Length, uint32_t *Data)
{
  if (UTCON == UT0CON)
  {
    while (UTCON->UTIE)
      ;
    WRITE_SR(UT0RBC, Length);
    WRITE_SR(UT0RDA, (uint32_t)Data);
  }
  else if (UTCON == UT1CON)
  {
    while (UTCON->UTIE)
      ;
    WRITE_SR(UT1RBC, Length);
    WRITE_SR(UT1RDA, (uint32_t)Data);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
 * @brief  UART TX once
 * @param  UTCON: pointer to a UTCON_TypeDef structure, specifies which UART
 */
__sram__ void UartTxOnce(UTCON_TypeDef *UTCON)
{
  /* kcik start tx */
  UTCON->UTTKS = 1;
  while (UTCON->UTTBV == 0)
    ;
  /* clear done flag */
  UTCON->UTTCLR = 1;
}

/**
 * @brief  UART RX once
 * @param  UTCON: pointer to a UTCON_TypeDef structure, specifies which UART
 * @retval Rx data
 */
__sram__ HAL_ResultTypedef UartRxOnce(UTCON_TypeDef *UTCON)
{
  HAL_ResultTypedef Result = {0};

  UTCON->UTRE = 1U;

  while (UTCON->UTRBV == 0)
    ;
  UTCON->UTRCLR = 1U;
  UTCON->UTRE = 0;

  Result.Error = HAL_OK;

  if (UTCON == UT0CON)
  {
    Result.Data = UT0RBUF;
  }
  else if (UTCON == UT1CON)
  {
    Result.Data = UT1RBUF;
  }
  else
  {
    Result.Error = HAL_ERROR;
  }

  return Result;
}

/**
 * @brief  Tx one byte
 * @param  UTCON: pointer to a UTCON_TypeDef structure, specifies which UART
 * @param  Data: data to be sent
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_UART_TX_OneByte(UTCON_TypeDef *UTCON, uint32_t Data)
{
  if (UartTxSetup(UTCON, 1U, &Data) != HAL_OK)
    return HAL_ERROR;

  UartTxOnce(UTCON);
  return HAL_OK;
}

/**
 * @brief  Rx one byte
 * @param  UTCON: pointer to a UTCON_TypeDef structure, specifies which UART
 * @retval Rx data
 */
__sram__ HAL_ResultTypedef HAL_UART_RX_OneByte(UTCON_TypeDef *UTCON)
{
  HAL_ResultTypedef Result = {0};
  /* dont care the address pointer */
  if (UartRxSetup(UTCON, 1U, NULL) != HAL_OK)
  {
    Result.Error = HAL_ERROR;
    return Result;
  }

  return UartRxOnce(UTCON);
}

/**
 * @brief  Tx one word
 * @param  UTCON: pointer to a UTCON_TypeDef structure, specifies which UART
 * @param  Data: data to be sent
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_UART_TX_OneWord(UTCON_TypeDef *UTCON, uint32_t Data)
{
  if (UartTxSetup(UTCON, 4U, &Data) != HAL_OK)
    return HAL_ERROR;

  UartTxOnce(UTCON);
  return HAL_OK;
}

/**
 * @brief  Rx one word
 * @param  UTCON: pointer to a UTCON_TypeDef structure, specifies which UART
 * @retval Rx data
 */
__sram__ HAL_ResultTypedef HAL_UART_RX_OneWord(UTCON_TypeDef *UTCON)
{
  HAL_ResultTypedef Result = {0};
  /* dont care the address pointer */
  if (UartRxSetup(UTCON, 4U, NULL) != HAL_OK)
  {
    Result.Error = HAL_ERROR;
    return Result;
  }

  return UartRxOnce(UTCON);
}

/**
 * @brief  update DMA channel state
 * @param  DMAE: specifies whether dma is enable
 * @param  DmaCh: pointer to a HAL_DMAChTypedef structure, specifies the dma channel
 * @param  Module: pointer to a HAL_DMAMapTypedef structure, specifies the dma module
 * @param  State: pointer to a HAL_DMAStateTypedef structure, specifies the dma state to be set
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef UartDMAUpdate(uint32_t DMAE, HAL_DMAChTypedef DmaCh, HAL_DMAMapTypedef Module, HAL_DMAStateTypedef State)
{
  if (DMAE)
    return HAL_OK;

  return HAL_DMA_Update(DmaCh, Module, State);
}

/**
 * @brief  Tx packet
 * @param  UTCON: pointer to a UTCON_TypeDef structure, specifies which UART
 * @param  Packet: pointer to a HAL_UARTPacketTypedef structure, specifies the tx data packet
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_UART_TX_Packet(UTCON_TypeDef *UTCON, HAL_UARTPacketTypedef *Packet)
{
  if (Packet == NULL || Packet->Data == NULL || (UTCON != UT0CON && UTCON != UT1CON))
    return HAL_ERROR;

  /* wait rx if it is half-duplex */
  if (UTCON->UTHD)
  {
    while (UTCON->UTRDMAE)
      ;
  }

  /* wait previous packet ends */
  while (UTCON->UTTDMAE)
    ;

  if (UTCON == UT0CON)
  {
    if (UartDMAUpdate(UT0CON->UTRDMAE, Ut0DmaCh, OC32_DMA_UART0, OC32_DMA_BUSY) != HAL_OK)
      return HAL_ERROR;

    if (Packet->Mode == UART_MODE_INT)
    {
      UTCON->UTIE = 1U;
    }
  }

  if (UTCON == UT1CON)
  {
    if (UartDMAUpdate(UT1CON->UTRDMAE, Ut1DmaCh, OC32_DMA_UART1, OC32_DMA_BUSY) != HAL_OK)
      return HAL_ERROR;

    if (Packet->Mode == UART_MODE_INT)
    {
      UTCON->UTIE = 1U;
    }
  }

  /* setup tx dma */
  UTCON->UTTDMAE = 1U;

  if (UartTxSetup(UTCON, Packet->Length, Packet->Data) != HAL_OK)
    return HAL_ERROR;

  /* kcik start tx */
  UTCON->UTTKS = 1U;

  /* return if it is interrupt mode */
  if (UTCON->UTIE)
  {
    return HAL_OK;
  }
  else
  {
    while (UTCON->UTTBV == 0)
      ;
    /* clear done flag */
    UTCON->UTTCLR = 1U;
    UTCON->UTTDMAE = 0;
    if (UTCON == UT0CON)
    {
      return UartDMAUpdate(UT0CON->UTRDMAE, Ut0DmaCh, OC32_DMA_UART0, OC32_DMA_IDLE);
    }
    else if (UTCON == UT1CON)
    {
      return UartDMAUpdate(UT1CON->UTRDMAE, Ut1DmaCh, OC32_DMA_UART1, OC32_DMA_IDLE);
    }
    else
    {
      return HAL_ERROR;
    }
  }
}

/**
 * @brief  Rx packet
 * @param  UTCON: pointer to a UTCON_TypeDef structure, specifies which UART
 * @param  Packet: pointer to a HAL_UARTPacketTypedef structure, specifies the rx data packet
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_UART_RX_Packet(UTCON_TypeDef *UTCON, HAL_UARTPacketTypedef *Packet)
{
  if (Packet == NULL || Packet->Data == NULL || (UTCON != UT0CON && UTCON != UT1CON))
    return HAL_ERROR;

  /* wait tx if it is half-duplex */
  if (UTCON->UTHD)
  {
    while (UTCON->UTTDMAE)
      ;
  }

  /* wait previous packet ends */
  while (UTCON->UTRDMAE)
    ;

  if (UTCON == UT0CON)
  {
    if (UartDMAUpdate(UT0CON->UTTDMAE, Ut0DmaCh, OC32_DMA_UART0, OC32_DMA_BUSY) != HAL_OK)
      return HAL_ERROR;

    if (Packet->Mode == UART_MODE_INT)
    {
      UTCON->UTIE = 1U;
    }
  }

  if (UTCON == UT1CON)
  {
    if (UartDMAUpdate(UT1CON->UTTDMAE, Ut1DmaCh, OC32_DMA_UART1, OC32_DMA_BUSY) != HAL_OK)
      return HAL_ERROR;

    if (Packet->Mode == UART_MODE_INT)
    {
      UTCON->UTIE = 1U;
    }
  }

  /* setup rx dma */
  UTCON->UTRDMAE = 1U;

  if (UartRxSetup(UTCON, Packet->Length, Packet->Data) != HAL_OK)
    return HAL_ERROR;

  /* enable rx */
  UTCON->UTRE = 1U;

  /* return if it is interrupt mode */
  if (UTCON->UTIE)
    return HAL_OK;
  else
  {
    while (UTCON->UTRBV == 0)
      ;
    /* clear done flag */
    UTCON->UTRE = 0;
    UTCON->UTRCLR = 1U;
    UTCON->UTRDMAE = 0;
    if (UTCON == UT0CON)
    {
      return UartDMAUpdate(UT0CON->UTTDMAE, Ut0DmaCh, OC32_DMA_UART0, OC32_DMA_IDLE);
    }
    else if (UTCON == UT0CON)
    {
      return UartDMAUpdate(UT1CON->UTTDMAE, Ut1DmaCh, OC32_DMA_UART1, OC32_DMA_IDLE);
    }
    else
    {
      return HAL_ERROR;
    }
  }
}

/**
 * @brief  Handles Uart interrupt request at user side.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__sram__ __weak__ HAL_StatusTypeDef HAL_UART_UserIRQ(UTCON_TypeDef *UTCON)
{
  return HAL_OK;
}

/**
 * @brief  Handles UART interrupt request.
 */
__sram__ HAL_StatusTypeDef HAL_UART_IRQHandler(UTCON_TypeDef *UTCON)
{
  uint32_t DmaCh;
  uint32_t Module;
  if (UTCON == UT0CON)
  {
    DmaCh = Ut0DmaCh;
    Module = OC32_DMA_UART0;
  }
  else if (UTCON == UT1CON)
  {
    DmaCh = Ut1DmaCh;
    Module = OC32_DMA_UART1;
  }
  else
  {
    return HAL_ERROR;
  }

  if (UTCON->UTTBV)
  {
    UTCON->UTTCLR = 1U;
    UTCON->UTTDMAE = 0;
    UartDMAUpdate(UTCON->UTRDMAE, DmaCh, Module, OC32_DMA_IDLE);
  }

  if (UTCON->UTRBV)
  {
    UTCON->UTRCLR = 1U;
    UTCON->UTRE = 0;
    UTCON->UTRDMAE = 0;
    UartDMAUpdate(UTCON->UTTDMAE, DmaCh, Module, OC32_DMA_IDLE);
  }

  if (DMAChState[DmaCh] == OC32_DMA_IDLE)
    UTCON->UTIE = 0;

  return HAL_UART_UserIRQ(UTCON);
}

#endif /* HAL_UART_ENABLE */