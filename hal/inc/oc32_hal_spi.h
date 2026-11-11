/**
 ******************************************************************************
 * @file    oc32_hal_spi.h
 * @author
 * @brief   Header file of SPI HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_SPI_H
#define __OC32_HAL_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

  /* extern ------------------------------------------------------------*/
  extern uint32_t DMAChState[OC32_DMA_CH_TOTAL];
  extern __sram__ HAL_StatusTypedef HAL_DMA_Update(HAL_EnableTypedef DMAE, HAL_DMA_ChTypedef DmaCh, HAL_DMA_MapTypedef Module, HAL_DMA_StateTypedef State);

  /** @addtogroup OC32_HAL_Driver
   * @{
   */

  /* Exported types ------------------------------------------------------*/

  /**
   * @brief  SPI Mode definition
   */
  typedef enum
  {
    SPI_CIH_DSS = 0U, /*!< Clock idle high, data sample from second edge */
    SPI_CIL_DSS = 1U, /*!< Clock idle low, data sample from second edge */
    SPI_CIH_DSF = 2U, /*!< Clock idle high, data sample from first edge */
    SPI_CIL_DSF = 3U  /*!< Clock idle low, data sample from first edge */
  } HAL_SPI_ModeTypedef;

  /**
   * @brief  SPI Slave definition
   */
  typedef enum
  {
    SPI_MASTER = 0x0U,
    SPI_SLAVE = 0x1U
  } HAL_SPI_SlaveTypedef;

  /**
   * @brief  SPI CS Out definition
   */
  typedef enum
  {
    SPI_CS_OUT_OFF = 0x0U, /*!< Master mode CS output disabled, user should manually control the CS by software. */
    SPI_CS_OUT_ON = 0x1U,  /*!< Master mode CS output enabled */
  } HAL_SPI_CsOutTypedef;

  /**
   * @brief  SPI Duplex definition
   */
  typedef enum
  {
    SPI_FULL_DUPLEX = 0x0U, /*!< tx and rx at the same clock */
    SPI_HALF_DUPLEX = 0x1U  /*!< tx or rx only */
  } HAL_SPI_DuplexTypedef;

  /**
   * @brief  SPI Half-TX-RX definition
   */
  typedef enum
  {
    SPI_HALF_TX = 0x0U,
    SPI_HALF_RX = 0x1U
  } HAL_SPI_HalfTxRxTypedef;

  /**
   * @brief  SPI Half-Pin definition
   */
  typedef enum
  {
    SPI_HALF_PIN_SEPARATE = 0x0U, /*!< separate tx pin and rx pin */
    SPI_HALF_PIN_SHARE = 0x1U     /*!< enable tx pin only, tx and rx use the same one */
  } HAL_SPI_HalfPinTypedef;

  /**
   * @brief  SPI First Bit definition
   */
  typedef enum
  {
    SPI_MSB_FIRST = 0x0U,
    SPI_LSB_FIRST = 0x1U
  } HAL_SPI_FirstBitTypedef;

  /**
   * @brief  SPI Pin definition
   */
  typedef enum
  {
    SPI_PINS_0 = 0x0U,
    SPI_PINS_1 = 0x1U
  } HAL_SPI_PinsTypedef;

  /**
   * @brief  SPI Init structure definition
   */
  typedef struct
  {
    MODULE_SPI_Typedef *Spi;          /*!< Specifies the SPI */
    HAL_SPI_ModeTypedef Mode;         /*!< Specifies the SPI mode */
    uint32_t ClkDiv;                  /*!< Specifies the SPI clock divider */
    HAL_SPI_SlaveTypedef Slave;       /*!< Specifies the SPI slave */
    HAL_EnableTypedef CsOutE;       /*!< Specifies the SPI cs out enable */
    HAL_SPI_DuplexTypedef Duplex;     /*!< Specifies the Half-duplex */
    HAL_EnableTypedef HalfPinShareE;   /*!< Specifies the Half-duplex pin share enable */
    HAL_SPI_FirstBitTypedef FirstBit; /*!< Specifies the First Bit */
    HAL_SPI_PinsTypedef Pins;         /*!< Specifies the pins */
    HAL_DMA_ChTypedef DmaCh;          /*!< Specifies DMA channel for transfer */
  } HAL_SPI_InitTypedef;

  /**
   * @brief  SPI packet structure definition
   */
  typedef struct
  {
    HAL_SPI_HalfTxRxTypedef HalfRx; /*!< Specifies the half-duplex tx/rx mode */
    uint32_t *TxData;               /*!< Specifies the tx data buffer */
    uint32_t *RxData;               /*!< Specifies the rx data buffer */
    uint32_t Length;                /*!< Specifies Data Length in bytes */
    HAL_EnableTypedef IntE;         /*!< Specifies the interrupt */
  } HAL_SPI_PacketTypedef;

  /**
   * @}
   */

  /** @addtogroup SPI
   * @{
   */
#ifdef HAL_SPI_ENABLE
  HAL_StatusTypedef HAL_SPI_Init(HAL_SPI_InitTypedef *Init);
  HAL_StatusTypedef HAL_SPI_DeInit(MODULE_SPI_Typedef *Spi);
  __sram__ HAL_StatusTypedef HAL_SPI_Transfer(MODULE_SPI_Typedef *Spi, HAL_SPI_PacketTypedef *Packet);
  __sram__ HAL_StatusTypedef HAL_SPI_IRQHandler(MODULE_SPI_Typedef *Spi);
  __sram__ __weak__ HAL_StatusTypedef HAL_SPI_UserIRQ(MODULE_SPI_Typedef *Spi);
#endif /* HAL_SPI_ENABLE */
  /**
   * @}
   */

  /* Exported constants --------------------------------------------------*/

  /** @defgroup SPI_Exported_Constants SPI Exported Constants
   * @{
   */

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_SPI_H */
