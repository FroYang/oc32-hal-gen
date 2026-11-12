/**
 ******************************************************************************
 * @file    oc32_hal_iic.h
 * @author
 * @brief   Header file of IIC (I2C) HAL module.
 *
 *          The chip provides a single IIC peripheral (base 0x3FF08300). As a
 *          single-instance peripheral (like CRC), it is accessed directly via
 *          the device-header register macros — the control register
 *          (IICCON_Typedef / IICCON) and the standalone data registers
 *          (IICTBUF/IICRBUF/IICADR/IICDA/IICBC). No MODULE_IIC_Typedef block
 *          struct is introduced; that pattern is reserved for peripherals that
 *          have multiple on-chip instances (SPI/UART/WDT/CCP/TIMER).
 *
 *          Supported features (per the IIC datasheet section 14):
 *            - master and slave modes
 *            - Clock Stretching (IICSCL)
 *            - DMA transfers (IICDMAE, OC32_DMA_IIC)
 *            - 7-bit and 10-bit (+ Header) addressing
 *            - interrupt / polling operation
 ******************************************************************************
 */

#ifndef __OC32_HAL_IIC_H
#define __OC32_HAL_IIC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

  /* extern ------------------------------------------------------------*/
  extern uint32_t DMAChState[OC32_DMA_CH_TOTAL];
  extern __sram__ HAL_StatusTypedef DmaUpdate(HAL_EnableTypedef DMAE, HAL_DMA_ChTypedef DmaCh, HAL_DMA_MapTypedef Module, HAL_DMA_StateTypedef State);

  /** @addtogroup OC32_HAL_Driver
   * @{
   */

  /* Exported types ------------------------------------------------------*/

  /**
   * @brief  IIC master-mode transmit operation (IICTXC field)
   */
  typedef enum
  {
    IIC_TX_START = 0x0U, /*!< Send Start condition */
    IIC_TX_ADDR = 0x1U,  /*!< Send address (7-bit, or 10-bit Header when IICHE=1) */
    IIC_DATA = 0x2U,     /*!< Send/Receive data byte(s) */
    IIC_TX_STOP = 0x3U   /*!< Send Stop condition */
  } HAL_IIC_TxControlTypedef;

  /**
   * @brief  IIC mode
   */
  typedef enum
  {
    IIC_MASTER = 0x0U, /*!< Master mode (IICS = 0) */
    IIC_SLAVE = 0x1U   /*!< Slave mode (IICS = 1) */
  } HAL_IIC_SlaveTypedef;

  /**
   * @brief  IIC master read/write
   */
  typedef enum
  {
    IIC_WRITE = 0x0U, /*!< IIC write */
    IIC_READ = 0x1U   /*!< IIC read */
  } HAL_IIC_RWTypedef;

  /**
   * @brief  IIC master tx/rx direction
   */
  typedef enum
  {
    IIC_RX = 0x0U, /*!< Receive (IICTX = 0) */
    IIC_TX = 0x1U  /*!< Transmit (IICTX = 1) */
  } HAL_IIC_DataDirTypedef;

  /**
   * @brief  IIC address bit width
   */
  typedef enum
  {
    IIC_ADDR_7BIT = 0x0U, /*!< 7-bit addressing (no Header) */
    IIC_ADDR_10BIT = 0x1U /*!< 10-bit addressing with Header (IICHE = 1) */
  } HAL_IIC_AddrModeTypedef;

/* user can change the default value */
#define IIC_ADDR_10BIT_HEADER 0xF000U

  /**
   * @brief  IIC Pin definition
   */
  typedef enum
  {
    IIC_PINS0 = 0x0U,
    IIC_PINS1 = 0x1U
  } HAL_IIC_PinsTypedef;

  /**
   * @brief  IIC Bus State definition
   */
  typedef enum
  {
    IIC_BUS_IDLE = 0x0U,
    IIC_BUS_ACTIVE = 0x1U
  } HAL_IIC_BusStateTypedef;

  /**
   * @brief  IIC Slave Address Match definition
   */
  typedef enum
  {
    IIC_ADDR_MISMATCH = 0x0U,
    IIC_ADDR_MATCH = 0x1U
  } HAL_IIC_SlaveMatchTypedef;

  /**
   * @brief  IIC Transfer State definition
   */
  typedef enum
  {
    IIC_TRANS_IDLE = 0x0U,
    IIC_TRANS_START = 0x1U,
    IIC_TRANS_ADDR = 0x2U,
    IIC_TRANS_DATA = 0x3U,
    IIC_TRANS_STOP = 0x4U
  } HAL_IIC_TransferStateTypedef;

  /**
   * @brief  IIC Init structure definition
   */
  typedef struct
  {
    HAL_IIC_AddrModeTypedef AddrMode; /*!< Specifies address bit width */
    uint32_t ClkDiv;                  /*!< Specifies the IIC master clock divider */
    HAL_EnableTypedef Stretch;        /*!< Specifies Clock-Stretching enable (slave) */
    HAL_IIC_PinsTypedef Pins;         /*!< Specifies the pins */
    HAL_DMA_ChTypedef DmaCh;          /*!< Specifies the DMA channel for transfers */
  } HAL_IIC_InitTypedef;

  /**
   * @brief  IIC data packet structure for DMA / interrupt transfers
   */
  typedef struct
  {
    uint32_t DevAddr;       /*!< Device address */
    HAL_IIC_RWTypedef RW;   /*!< Specifies the data direction */
    uint32_t *Data;         /*!< DMA data buffer (SRAM). */
    uint32_t Length;        /*!< Number of bytes. Non-DMA max 4, DMA max 1023. */
    HAL_EnableTypedef IntE; /*!< Specifies the interrupt */
  } HAL_IIC_PacketTypedef;

  /**
   * @}
   */

  /** @addtogroup IIC
   * @{
   */
#ifdef HAL_IIC_ENABLE
  HAL_StatusTypedef HAL_IIC_Init(HAL_IIC_InitTypedef *Init);
  HAL_StatusTypedef HAL_IIC_DeInit(void);
  __sram__ HAL_StatusTypedef HAL_IIC_Transfer(HAL_IIC_PacketTypedef *Packet);

  /* ---- interrupt handling -------------------------------------------- */
  __sram__ HAL_StatusTypedef HAL_IIC_IRQHandler(void);
  __weak__ __sram__ HAL_StatusTypedef HAL_IIC_UserIRQ(void);
#endif /* HAL_IIC_ENABLE */
  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_IIC_H */
