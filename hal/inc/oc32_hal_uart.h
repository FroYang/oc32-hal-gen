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
extern "C" {
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"
#include "CV32S6015MSR.h"

#ifdef HAL_UART_ENABLE

/** @addtogroup OC32_HAL_Driver
  * @{
  */

/** @addtogroup UART
  * @{
  */

/* Exported types ------------------------------------------------------*/

/**
  * @brief UART Init Structure definition
  */
typedef struct {
    uint32_t BaudRate;          /*!< Baud rate */
    uint32_t WordLength;        /*!< Number of data bits */
    uint32_t StopBits;          /*!< Number of stop bits */
    uint32_t Parity;            /*!< Parity mode */
    uint32_t Mode;              /*!< Transmit/Receive mode */
    uint32_t HwFlowCtl;         /*!< Hardware flow control */
    uint32_t OverSampling;      /*!< Oversampling mode */
} UART_InitTypeDef;

/**
  * @brief HAL UART State structures definition
  */
typedef enum {
    HAL_UART_STATE_RESET             = 0x00U,
    HAL_UART_STATE_READY             = 0x20U,
    HAL_UART_STATE_BUSY              = 0x24U,
    HAL_UART_STATE_BUSY_TX           = 0x21U,
    HAL_UART_STATE_BUSY_RX           = 0x22U,
    HAL_UART_STATE_BUSY_TX_RX        = 0x23U,
    HAL_UART_STATE_TIMEOUT           = 0xA0U,
    HAL_UART_STATE_ERROR             = 0xE0U
} HAL_UART_StateTypeDef;

/**
  * @brief UART Handle Structure definition
  */
typedef struct __UART_HandleTypeDef {
    void *Instance;                         /*!< Register base address */
    UART_InitTypeDef Init;                  /*!< UART communication parameters */
    uint8_t *pTxBuffPtr;                    /*!< Pointer to UART Tx transfer Buffer */
    uint16_t TxFileSize;                    /*!< UART Tx transfer size */
    uint16_t TxCursor;                      /*!< UART Tx transfer cursor */
    uint8_t *pRxBuffPtr;                    /*!< Pointer to UART Rx transfer Buffer */
    uint16_t RxFileSize;                    /*!< UART Rx transfer size */
    uint16_t RxCursor;                      /*!< UART Rx transfer cursor */
    DMA_HandleTypeDef *hdmatx;              /*!< UART Tx DMA Handle parameters */
    DMA_HandleTypeDef *hdmarx;              /*!< UART Rx DMA Handle parameters */
    HAL_LockTypeDef Lock;                   /*!< Locking object */
    __IO HAL_UART_StateTypeDef gState;      /*!< UART state information related to global Handle management */
    __IO HAL_UART_StateTypeDef RxState;     /*!< UART state information related to Rx operations */
    __IO uint32_t ErrorCode;                /*!< UART Error code */
#if (HAL_UART_ENABLE > 0U)
    void (* TxISR)(struct __UART_HandleTypeDef *huart);   /*!< UART Tx IRQ Handler function pointer */
    void (* RxISR)(struct __UART_HandleTypeDef *huart);   /*!< UART Rx IRQ Handler function pointer */
#endif
} UART_HandleTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------*/

/** @defgroup UART_Exported_Constants UART Exported Constants
  * @{
  */

/** @defgroup UART_Word_Length Word Length
  * @{
  */
#define UART_WORDLENGTH_8B          0x00000000U
#define UART_WORDLENGTH_9B          ((uint32_t)UART_CR1_M)
/**
  * @}
  */

/** @defgroup UART_Stop_Bits Stop Bits
  * @{
  */
#define UART_STOPBITS_1             0x00000000U
#define UART_STOPBITS_2             ((uint32_t)UART_CR2_STOP_1)
/**
  * @}
  */

/** @defgroup UART_Parity Parity
  * @{
  */
#define UART_PARITY_NONE            0x00000000U
#define UART_PARITY_EVEN            ((uint32_t)UART_CR1_PCE | UART_CR1_PS)
#define UART_PARITY_ODD             ((uint32_t)UART_CR1_PCE)
/**
  * @}
  */

/** @defgroup UART_Mode UART Mode
  * @{
  */
#define UART_MODE_RX                ((uint32_t)UART_CR1_RE)
#define UART_MODE_TX                ((uint32_t)UART_CR1_TE)
#define UART_MODE_TX_RX             (UART_MODE_TX | UART_MODE_RX)
/**
  * @}
  */

/** @defgroup UART_Hardware_Flow_Control Hardware Flow Control
  * @{
  */
#define UART_HWCONTROL_NONE         0x00000000U
#define UART_HWCONTROL_RTS          UART_CR3_RTSE
#define UART_HWCONTROL_CTS          UART_CR3_CTSE
#define UART_HWCONTROL_RTS_CTS      (UART_CR3_RTSE | UART_CR3_CTSE)
/**
  * @}
  */

/** @defgroup UART_Over_Sampling Over Sampling
  * @{
  */
#define UART_OVERSAMPLING_16        0x00000000U
#define UART_OVERSAMPLING_8         UART_CR1_OVER8
/**
  * @}
  */

/** @defgroup UART_Flag UART Flags
  * @{
  */
#define UART_FLAG_CTS              ((uint32_t)UART_SR_CTS)
#define UART_FLAG_LBD              ((uint32_t)UART_SR_LBD)
#define UART_FLAG_TXE              ((uint32_t)UART_SR_TXE)
#define UART_FLAG_TC               ((uint32_t)UART_SR_TC)
#define UART_FLAG_RXNE             ((uint32_t)UART_SR_RXNE)
#define UART_FLAG_IDLE             ((uint32_t)UART_SR_IDLE)
#define UART_FLAG_ORE              ((uint32_t)UART_SR_ORE)
#define UART_FLAG_NE               ((uint32_t)UART_SR_NE)
#define UART_FLAG_FE               ((uint32_t)UART_SR_FE)
#define UART_FLAG_PE               ((uint32_t)UART_SR_PE)
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -------------------------------------------------------*/

/** @defgroup UART_Exported_Macros UART Exported Macros
  * @{
  */
#define __HAL_UART_GET_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR & (__FLAG__)) == (__FLAG__))
#define __HAL_UART_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->SR &= ~(__FLAG__))
#define __HAL_UART_ENABLE_IT(__HANDLE__, __IT__)  ((__HANDLE__)->Instance->CR1 |= (__IT__))
#define __HAL_UART_DISABLE_IT(__HANDLE__, __IT__) ((__HANDLE__)->Instance->CR1 &= ~(__IT__))
#define __HAL_UART_GET_IT(__HANDLE__, __IT__)     (((__HANDLE__)->Instance->CR1 & (__IT__)) == (__IT__))
#define __HAL_UART_CLEAR_IT(__HANDLE__, __IT__)   ((__HANDLE__)->Instance->SR &= ~(__IT__))
#define __HAL_UART_ENABLE(__HANDLE__)             ((__HANDLE__)->Instance->CR1 |= UART_CR1_UE)
#define __HAL_UART_DISABLE(__HANDLE__)            ((__HANDLE__)->Instance->CR1 &= ~UART_CR1_UE)
/**
  * @}
  */

/* Exported functions ----------------------------------------------------*/

/** @addtogroup UART_Exported_Functions
  * @{
  */

/* Initialization and de-initialization functions *************************/
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart);
void HAL_UART_MspInit(UART_HandleTypeDef *huart) __weak;
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart) __weak;

/* IO operation functions *************************************************/
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_IRQHandler(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) __weak;
HAL_StatusTypeDef HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart) __weak;
HAL_StatusTypeDef HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) __weak;
HAL_StatusTypeDef HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) __weak;
HAL_StatusTypeDef HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) __weak;

/**
  * @}
  */

/* Private macros --------------------------------------------------------*/
/** @defgroup UART_Private_Macros UART Private Macros
  * @{
  */
#define IS_UART_WORD_LENGTH(LENGTH) (((LENGTH) == UART_WORDLENGTH_8B) || \
                                     ((LENGTH) == UART_WORDLENGTH_9B))
#define IS_UART_STOPBITS(STOPBITS)  (((STOPBITS) == UART_STOPBITS_1) || \
                                     ((STOPBITS) == UART_STOPBITS_2))
#define IS_UART_PARITY(PARITY)      (((PARITY) == UART_PARITY_NONE) || \
                                     ((PARITY) == UART_PARITY_EVEN) || \
                                     ((PARITY) == UART_PARITY_ODD))
#define IS_UART_MODE(MODE)          ((((MODE) & 0x0000FFF3U) == 0x00000000U) && ((MODE) != 0x00000000U))
#define IS_UART_HWCONTROL(CONTROL)  (((CONTROL) == UART_HWCONTROL_NONE) || \
                                     ((CONTROL) == UART_HWCONTROL_RTS) || \
                                     ((CONTROL) == UART_HWCONTROL_CTS) || \
                                     ((CONTROL) == UART_HWCONTROL_RTS_CTS))
#define IS_UART_OVERSAMPLING(SAMPLING) (((SAMPLING) == UART_OVERSAMPLING_16) || \
                                        ((SAMPLING) == UART_OVERSAMPLING_8))
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_UART_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_UART_H */
