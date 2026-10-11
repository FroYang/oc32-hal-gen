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
  } HAL_UARTBitsTypedef;

  /**
   * @brief  UART Mode definition
   */
  typedef enum
  {
    UART_MODE_NONE = 0x0U,
    UART_MODE_INT = 0x1U
  } HAL_UARTModeTypedef;

  /**
   * @brief  UART Parity definition
   */
  typedef enum
  {
    UART_PAR_ODD = 0x0U,
    UART_PAR_EVEN = 0x1U,
    UART_PAR_0 = 0x2U,
    UART_PAR_1 = 0x3U
  } HAL_UARTParityTypedef;

  /**
   * @brief  UART Duplex definition
   */
  typedef enum
  {
    UART_FULL_DUPLEX = 0x0U,
    UART_HALF_DUPLEX = 0x1U
  } HAL_UARTDuplexTypedef;

  /**
   * @brief  UART Pin definition
   */
  typedef enum
  {
    UART_PINS_0 = 0x0U,
    UART_PINS_1 = 0x1U
  } HAL_UARTPinsTypedef;

  /**
   * @brief  UART TX State definition
   */
  typedef enum
  {
    UART_TX_IDLE = 0x0U,
    UART_TX_BUSY = 0x1U
  } HAL_UARTTxStateTypedef;

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
  } HAL_UARTBaudTypedef;

  /**
   * @brief  UART Init structure definition
   */
  typedef struct
  {
    UTCON_TypeDef *UTCON;         /*!< Specifies the Uart */
    HAL_UARTParityTypedef Parity; /*!< Specifies the Parity mode */
    HAL_UARTBitsTypedef Bits;     /*!< Specifies the Data bits */
    HAL_UARTBaudTypedef Baud;     /*!< Specifies the Baudrate */
    HAL_UARTDuplexTypedef Duplex; /*!< Specifies the Half-duplex */
    HAL_UARTPinsTypedef Pins;     /*!< Specifies the pins */
    HAL_DMAChTypedef DmaCh;         /*!< Specifies DMA channel for transfer */
  } HAL_UARTInitTypeDef;

  /**
   * @brief  UART packet structure definition
   */
  typedef struct
  {
    uint32_t *Data;           /*!< Specifies Data buffer */
    uint32_t Length;          /*!< Specifies Data Length in bytes */
    HAL_UARTModeTypedef Mode; /*!< Specifies the Mode */
  } HAL_UARTPacketTypedef;



  /**
   * @}
   */

  /** @addtogroup UART
   * @{
   */
#ifdef HAL_UART_ENABLE
  HAL_StatusTypeDef HAL_UART_Init(HAL_UARTInitTypeDef *Init);
  HAL_StatusTypeDef HAL_UART_DeInit(UTCON_TypeDef *UTCON);
  __sram__ HAL_StatusTypeDef HAL_UART_TX_OneByte(UTCON_TypeDef *UTCON, uint32_t Data);
  __sram__ HAL_StatusTypeDef HAL_UART_TX_OneWord(UTCON_TypeDef *UTCON, uint32_t Data);
  __sram__ HAL_StatusTypeDef HAL_UART_TX_Packet(UTCON_TypeDef *UTCON, HAL_UARTPacketTypedef *Packet);
  __sram__ HAL_ResultTypedef HAL_UART_RX_OneByte(UTCON_TypeDef *UTCON);
  __sram__ HAL_ResultTypedef HAL_UART_RX_OneWord(UTCON_TypeDef *UTCON);
  __sram__ HAL_StatusTypeDef HAL_UART_RX_Packet(UTCON_TypeDef *UTCON, HAL_UARTPacketTypedef *Packet);
  __sram__ HAL_StatusTypeDef HAL_UART_IRQHandler(UTCON_TypeDef *UTCON);
  __sram__ __weak__ HAL_StatusTypeDef HAL_UART_UserIRQ(UTCON_TypeDef *UTCON);
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
