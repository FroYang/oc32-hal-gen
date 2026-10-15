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
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_UART_Init(HAL_UART_InitTypeDef *Init)
{
  if (Init == NULL)
    return HAL_ERROR;

  /* setup interrupt priority */
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
 * @retval HAL status
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
 * @param  Length: length of data
 * @param  Data: data to be sent
 * @retval HAL status
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
 * @param  Length: length of data
 * @retval HAL status
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
 * @brief  UART TX once
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 */
__sram__ void UartTxOnce(MODULE_UART_TypeDef *Uart)
{
  /* kcik start tx */
  Uart->UTCON.UTTKS = 1;
  while (Uart->UTCON.UTTBV == 0)
    ;
  /* clear done flag */
  Uart->UTCON.UTTCLR = 1;
}

/**
 * @brief  UART RX once
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @retval Rx data
 */
__sram__ HAL_ResultTypedef UartRxOnce(MODULE_UART_TypeDef *Uart)
{
  HAL_ResultTypedef Result;

  Uart->UTCON.UTRE = 1U;

  while (Uart->UTCON.UTRBV == 0)
    ;
  Uart->UTCON.UTRCLR = 1U;
  Uart->UTCON.UTRE = 0U;

  Result.Error = HAL_OK;
  Result.Data = Uart->UTRBUF;
  return Result;
}

/**
 * @brief  Tx one byte
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @param  Data: data to be sent
 * @retval HAL status
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
 * @retval Rx data
 */
__sram__ HAL_ResultTypedef HAL_UART_RX_OneByte(MODULE_UART_TypeDef *Uart)
{
  HAL_ResultTypedef Result = {0};
  /* dont care the address pointer */
  if (UartRxSetup(Uart, 1U, NULL) != HAL_OK)
  {
    Result.Error = HAL_ERROR;
    return Result;
  }

  return UartRxOnce(Uart);
}

/**
 * @brief  Tx one word
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @param  Data: data to be sent
 * @retval HAL status
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
 * @retval Rx data
 */
__sram__ HAL_ResultTypedef HAL_UART_RX_OneWord(MODULE_UART_TypeDef *Uart)
{
  HAL_ResultTypedef Result = {0};
  /* dont care the address pointer */
  if (UartRxSetup(Uart, 4U, NULL) != HAL_OK)
  {
    Result.Error = HAL_ERROR;
    return Result;
  }

  return UartRxOnce(Uart);
}

/**
 * @brief  update DMA channel state
 * @param  DMAE: specifies whether dma is enable
 * @param  DmaCh: pointer to a HAL_DMA_ChTypedef structure, specifies the dma channel
 * @param  Module: pointer to a HAL_DMA_MapTypedef structure, specifies the dma module
 * @param  State: pointer to a HAL_DMA_StateTypedef structure, specifies the dma state to be set
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef UartDMAUpdate(uint32_t DMAE, HAL_DMA_ChTypedef DmaCh, HAL_DMA_MapTypedef Module, HAL_DMA_StateTypedef State)
{
  if (DMAE)
    return HAL_OK;

  return HAL_DMA_Update(DmaCh, Module, State);
}

/**
 * @brief  Tx packet
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @param  Packet: pointer to a HAL_UART_PacketTypedef structure, specifies the tx data packet
 * @retval HAL status
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
  if (UartDMAUpdate(Uart->UTCON.UTRDMAE, DmaCh, DmaModule, OC32_DMA_BUSY) != HAL_OK)
    return HAL_ERROR;

  Uart->UTCON.UTIE = Packet->IntE;

  /* setup tx dma */
  Uart->UTCON.UTTDMAE = 1U;

  if (UartTxSetup(Uart, Packet->Length, Packet->Data) != HAL_OK)
    return HAL_ERROR;

  /* kcik start tx */
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
    return UartDMAUpdate(Uart->UTCON.UTRDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }
}

/**
 * @brief  Rx packet
 * @param  Uart: pointer to a MODULE_UART_TypeDef structure, specifies which UART
 * @param  Packet: pointer to a HAL_UART_PacketTypedef structure, specifies the rx data packet
 * @retval HAL status
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

  if (UartDMAUpdate(Uart->UTCON.UTTDMAE, DmaCh, DmaModule, OC32_DMA_BUSY) != HAL_OK)
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

    return UartDMAUpdate(Uart->UTCON.UTTDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }
}

/**
 * @brief  Handles Uart interrupt request at user side.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__sram__ __weak__ HAL_StatusTypeDef HAL_UART_UserIRQ(MODULE_UART_TypeDef *Uart)
{
  return HAL_OK;
}

/**
 * @brief  Handles UART interrupt request.
 */
__sram__ HAL_StatusTypeDef HAL_UART_IRQHandler(MODULE_UART_TypeDef *Uart)
{
  if (Uart != UART_0 && Uart != UART_1)
    return HAL_ERROR;

  HAL_DMA_ChTypedef DmaCh = (Uart == UART_0) ? Ut0DmaCh : Ut1DmaCh;
  HAL_DMA_MapTypedef DmaModule = (Uart == UART_0) ? OC32_DMA_UART0 : OC32_DMA_UART1;

  if (Uart->UTCON.UTTBV)
  {
    Uart->UTCON.UTTCLR = 1U;
    Uart->UTCON.UTTDMAE = 0U;
    UartDMAUpdate(Uart->UTCON.UTRDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }

  if (Uart->UTCON.UTRBV)
  {
    Uart->UTCON.UTRCLR = 1U;
    Uart->UTCON.UTRE = 0U;
    Uart->UTCON.UTRDMAE = 0U;
    UartDMAUpdate(Uart->UTCON.UTTDMAE, DmaCh, DmaModule, OC32_DMA_IDLE);
  }

  if (DMAChState[DmaCh] == OC32_DMA_IDLE)
    Uart->UTCON.UTIE = 0U;

  return HAL_UART_UserIRQ(Uart);
}

#endif /* HAL_UART_ENABLE */