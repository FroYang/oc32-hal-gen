/**
 ******************************************************************************
 * @file    oc32_hal_uart.h
 * @author
 * @brief   Header file of UART HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_UART_H
#define __OC32_HAL_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

  /** @addtogroup OC32_HAL_Driver
   * @{
   */

  /* Exported types ------------------------------------------------------*/

  /**
   * @brief  UART Bits definition
   */
  typedef enum
  {
    UART_8_BIT = 0U,
    UART_8_BIT_PAR = 1U,
    UART_9_BIT = 2U
  } HAL_UART_BitsTypedef;

  /**
   * @brief  UART Parity definition
   */
  typedef enum
  {
    UART_PAR_ODD = 0x0U,
    UART_PAR_EVEN = 0x1U,
    UART_PAR_0 = 0x2U,
    UART_PAR_1 = 0x3U
  } HAL_UART_ParityTypedef;

  /**
   * @brief  UART Duplex definition
   */
  typedef enum
  {
    UART_FULL_DUPLEX = 0x0U,
    UART_HALF_DUPLEX = 0x1U
  } HAL_UART_DuplexTypedef;

  /**
   * @brief  UART Pin definition
   */
  typedef enum
  {
    UART_PINS_0 = 0x0U,
    UART_PINS_1 = 0x1U
  } HAL_UART_PinsTypedef;

  /**
   * @brief  UART TX State definition
   */
  typedef enum
  {
    UART_TX_IDLE = 0x0U,
    UART_TX_BUSY = 0x1U
  } HAL_UART_TxStateTypedef;

  /**
   * @brief  UART Baudrate definition
   */
  typedef enum
  {
    UART_BAUD_9600 = 9600U,
    UART_BAUD_14400 = 14400U,
    UART_BAUD_38400 = 38400U,
    UART_BAUD_57600 = 57600U,
    UART_BAUD_115200 = 115200U,
    UART_BAUD_230400 = 230400U,
    UART_BAUD_460800 = 460800U,
    UART_BAUD_921600 = 921600U,
    UART_BAUD_1000000 = 1000000U
  } HAL_UART_BaudTypedef;

  /**
   * @brief  UART Init structure definition
   */
  typedef struct
  {
    MODULE_UART_TypeDef *Uart;     /*!< Specifies the Uart */
    HAL_UART_ParityTypedef Parity; /*!< Specifies the Parity mode */
    HAL_UART_BitsTypedef Bits;     /*!< Specifies the Data bits */
    HAL_UART_BaudTypedef Baud;     /*!< Specifies the Baudrate */
    HAL_UART_DuplexTypedef Duplex; /*!< Specifies the Half-duplex */
    HAL_UART_PinsTypedef Pins;     /*!< Specifies the pins */
    HAL_DMA_ChTypedef DmaCh;       /*!< Specifies DMA channel for transfer */
  } HAL_UART_InitTypeDef;

  /**
   * @brief  UART packet structure definition
   */
  typedef struct
  {
    uint32_t *Data;            /*!< Specifies Data buffer */
    uint32_t Length;           /*!< Specifies Data Length in bytes */
    HAL_MODULE_INTTypedef IntE; /*!< Specifies the interrupt */
  } HAL_UART_PacketTypedef;

  /**
   * @}
   */

  /** @addtogroup UART
   * @{
   */
#ifdef HAL_UART_ENABLE
  HAL_StatusTypeDef HAL_UART_Init(HAL_UART_InitTypeDef *Init);
  HAL_StatusTypeDef HAL_UART_DeInit(MODULE_UART_TypeDef *Uart);
  __sram__ HAL_StatusTypeDef HAL_UART_TX_OneByte(MODULE_UART_TypeDef *Uart, uint32_t Data);
  __sram__ HAL_StatusTypeDef HAL_UART_TX_OneWord(MODULE_UART_TypeDef *Uart, uint32_t Data);
  __sram__ HAL_StatusTypeDef HAL_UART_TX_Packet(MODULE_UART_TypeDef *Uart, HAL_UART_PacketTypedef *Packet);
  __sram__ HAL_ResultTypedef HAL_UART_RX_OneByte(MODULE_UART_TypeDef *Uart);
  __sram__ HAL_ResultTypedef HAL_UART_RX_OneWord(MODULE_UART_TypeDef *Uart);
  __sram__ HAL_StatusTypeDef HAL_UART_RX_Packet(MODULE_UART_TypeDef *Uart, HAL_UART_PacketTypedef *Packet);
  __sram__ HAL_StatusTypeDef HAL_UART_IRQHandler(MODULE_UART_TypeDef *Uart);
  __sram__ __weak__ HAL_StatusTypeDef HAL_UART_UserIRQ(MODULE_UART_TypeDef *Uart);
#endif /* HAL_UART_ENABLE */
  /**
   * @}
   */

  /* Exported constants --------------------------------------------------*/

  /** @defgroup UART_Exported_Constants UART Exported Constants
   * @{
   */

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_UART_H */
