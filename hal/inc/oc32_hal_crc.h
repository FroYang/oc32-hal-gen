/**
 ******************************************************************************
 * @file    oc32_hal_crc.h
 * @author
 * @brief   Header file of CRC HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_CRC_H
#define __OC32_HAL_CRC_H

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

  /** @addtogroup CRC
   * @{
   */

  /* Exported types ------------------------------------------------------*/

  typedef enum
  {
    CRC_MODE_NONE = 0x0U,
    CRC_MODE_INT = 0x1U
  } HAL_CRCModeTypedef;

  typedef enum
  {
    CRC_IDM_WRITE_CRCIN = 0x0U,
    CRC_IDM_CAPTURE_DMAIN = 0x1U,
    CRC_IDM_CAPTURE_DMAOUT = 0x2U,
    CRC_IDM_READ_DMA = 0x3U
  } HAL_CRCIDMTypedef;


  typedef enum
  {
    CRC_5_BIT = 4U,
    CRC_8_BIT = 7U,
    CRC_16_BIT = 15U,
    CRC_32_BIT = 31U
  } HAL_CRCBitsTypedef;  


  typedef struct
  {
    uint32_t Poly;            /*!< Specifies the crc poly */
    uint32_t InitVal;         /*!< Specifies the initial value */
    uint32_t Bits;          /*!< Specifies the bit length */
    uint32_t RefIn;           /*!< Specifies the input reverse bits in a byte */
    uint32_t RefOut;          /*!< Specifies the output reverse bits in whole word */
    uint32_t CplOut;          /*!< Specifies the output complement */
    uint32_t Rvb;             /*!< Specifies the output byte ordering reverse */
    HAL_CRCModeTypedef Mode;  /*!< Specifies the mode(normal or interrupt) */
    HAL_INTPriTypedef IntPri; /*!< Specifies the interrupt priority */
  } HAL_CRCInitTypeDef;

  typedef struct
  {
    uint32_t *Data;        /*!< Specifies Data buffer */
    uint32_t Length;       /*!< Specifies Data Length in bytes */
    HAL_CRCIDMTypedef Mode; /*!< Specifies the input data mode */
    HAL_DMAChTypedef DMA;  /*!< Specifies DMA channel for transfer */
  } HAL_CRCPacketTypedef;

  /* Exported constants --------------------------------------------------*/

  #define CRC_POLY_DEFAULT 0x04C11DB7U
  #define CRC_GOOD_DEFAULT 0x2144DF1CU


  /* Exported macros -------------------------------------------------------*/

  /* Exported functions ----------------------------------------------------*/

#ifdef HAL_CRC_ENABLE
  HAL_StatusTypeDef HAL_CRC_Init(HAL_CRCInitTypeDef *Init);
  HAL_StatusTypeDef HAL_CRC_DeInit(void);
  __weak__ void HAL_CRC_IRQHandler(void);
  uint32_t HAL_CRC_OneByte(uint32_t Data);
  uint32_t HAL_CRC_OneWord(uint32_t Data);
  HAL_StatusTypeDef HAL_CRC_Packet(HAL_CRCPacketTypedef *Packet);
  uint32_t HAL_CRC_GetResult(void);
#endif /* HAL_CRC_ENABLE */

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_CRC_H */
