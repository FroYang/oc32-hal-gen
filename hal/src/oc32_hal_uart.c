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
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the UART mode according to the specified parameters.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{
    /* TODO: Implement UART initialization
     * 1. Validate UART handle
     * 2. Enable UART clock
     * 3. Configure baud rate (UBRG register)
     * 4. Configure data bits (UCSRnC: USBSEL, UMODE)
     * 5. Configure stop bits (UCSRnC: USSEL)
     * 6. Configure parity (UCSRnA/B: UPE, UPM1:0)
     * 7. Configure mode (TXEN, RXEN)
     * 8. Configure oversampling (U2X)
     * 9. Initialize TX/RX buffers and state
     */

    huart->gState = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;

    /* Initialize the UART MSP */
    HAL_UART_MspInit(huart);

    return HAL_OK;
}

/**
  * @brief  DeInitializes the UART peripheral.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart)
{
    /* TODO: Implement UART de-initialization
     * 1. Disable UART interrupts
     * 2. Disable UART peripheral (UCSRnB: RXEN, TXEN)
     * 3. Reset UART registers to default
     * 4. Disable UART clock
     */

    huart->gState = HAL_UART_STATE_RESET;
    huart->RxState = HAL_UART_STATE_RESET;

    /* DeInitialize the UART MSP */
    HAL_UART_MspDeInit(huart);

    return HAL_OK;
}

/**
  * @brief  Initializes the UART MSP.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @note   This is a weak implementation that can be overridden by the user.
  */
__weak void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    /* NOTE: This function should be implemented in the user file.
     * TODO: Implement UART MSP initialization
     * 1. Configure GPIO pins (TX, RX)
     * 2. Configure NVIC for UART interrupt
     * 3. Enable UART clock
     */
}

/**
  * @brief  DeInitializes the UART MSP.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @note   This is a weak implementation that can be overridden by the user.
  */
__weak void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    /* NOTE: This function should be implemented in the user file.
     * TODO: Implement UART MSP de-initialization
     */
}

/**
  * @brief  Sends an amount of data in blocking mode.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @param  pData: pointer to data buffer.
  * @param  Size: amount of data to be sent.
  * @param  Timeout: timeout duration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /* TODO: Implement blocking UART transmit
     * 1. Check parameters
     * 2. Loop through data bytes
     * 3. Wait for TXE flag before writing each byte
     * 4. Wait for TC flag at the end
     */
    uint32_t tickstart = HAL_SYS_GetTick();

    while (Size > 0U)
    {
        /* Wait for TXE flag */
        if (HAL_SYS_GetTick() - tickstart >= Timeout)
        {
            return HAL_TIMEOUT;
        }
        while ((huart->Instance->USR & UART_USR_TXE) == 0U)
        {
        }
        huart->Instance->UDR = *pData;
        pData++;
        Size--;
    }

    /* Wait for TC flag */
    if (HAL_SYS_GetTick() - tickstart >= Timeout)
    {
        return HAL_TIMEOUT;
    }
    while ((huart->Instance->USR & UART_USR_TC) == 0U)
    {
    }

    return HAL_OK;
}

/**
  * @brief  Receives an amount of data in blocking mode.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @param  pData: pointer to data buffer.
  * @param  Size: amount of data to be received.
  * @param  Timeout: timeout duration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /* TODO: Implement blocking UART receive
     * 1. Check parameters
     * 2. Loop through data bytes
     * 3. Wait for RXNE flag before reading each byte
     */
    uint32_t tickstart = HAL_SYS_GetTick();

    while (Size > 0U)
    {
        /* Wait for RXNE flag */
        if (HAL_SYS_GetTick() - tickstart >= Timeout)
        {
            return HAL_TIMEOUT;
        }
        while ((huart->Instance->USR & UART_USR_RXNE) == 0U)
        {
        }
        *pData = huart->Instance->UDR;
        pData++;
        Size--;
    }

    return HAL_OK;
}

/**
  * @brief  Sends an amount of data in interrupt mode.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @param  pData: pointer to data buffer.
  * @param  Size: amount of data to be sent.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    /* TODO: Implement interrupt-driven UART transmit
     * 1. Check parameters
     * 2. Store TX buffer and size
     * 3. Enable TXE interrupt
     * 4. Set gState to BUSY_TX
     */
    if (huart->gState == HAL_UART_STATE_BUSY_RX)
    {
        return HAL_BUSY;
    }

    huart->gState = HAL_UART_STATE_BUSY_TX;
    huart->pTxBuffPtr = pData;
    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    /* Enable TXE and TC interrupts */
    huart->Instance->UCSRnB |= UART_UCSRB_TXIE;

    return HAL_OK;
}

/**
  * @brief  Receives an amount of data in interrupt mode.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @param  pData: pointer to data buffer.
  * @param  Size: amount of data to be received.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    /* TODO: Implement interrupt-driven UART receive
     * 1. Check parameters
     * 2. Store RX buffer and size
     * 3. Enable RXNE interrupt
     * 4. Set RxState to BUSY_RX
     */
    if (huart->RxState == HAL_UART_STATE_BUSY_TX)
    {
        return HAL_BUSY;
    }

    huart->RxState = HAL_UART_STATE_BUSY_RX;
    huart->pRxBuffPtr = pData;
    huart->RxXferSize = Size;
    huart->RxXferCount = Size;

    /* Enable RXNE interrupt */
    huart->Instance->UCSRnB |= UART_UCSRB_RXIE;

    return HAL_OK;
}

/**
  * @brief  Sends an amount of data in DMA mode.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @param  pData: pointer to data buffer.
  * @param  Size: amount of data to be sent.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    /* TODO: Implement DMA-driven UART transmit
     * 1. Check parameters
     * 2. Configure DMA for TX
     * 3. Enable DMA transfer complete interrupt
     * 4. Start DMA transfer
     * 5. Enable UART TXE interrupt
     */
    return HAL_OK;
}

/**
  * @brief  Receives an amount of data in DMA mode.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @param  pData: pointer to data buffer.
  * @param  Size: amount of data to be received.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    /* TODO: Implement DMA-driven UART receive
     * 1. Check parameters
     * 2. Configure DMA for RX
     * 3. Enable DMA transfer complete interrupt
     * 4. Start DMA transfer
     * 5. Enable UART RXNE interrupt
     */
    return HAL_OK;
}

/**
  * @brief  Pauses the DMA Transfer.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart)
{
    /* TODO: Pause DMA transfer */
    return HAL_OK;
}

/**
  * @brief  Resumes the DMA Transfer.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart)
{
    /* TODO: Resume DMA transfer */
    return HAL_OK;
}

/**
  * @brief  Stops the DMA Transfer.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart)
{
    /* TODO: Stop DMA transfer and disable interrupts */
    return HAL_OK;
}

/**
  * @brief  Handles UART interrupt request.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  */
void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    /* TODO: Handle UART interrupt requests
     * 1. Check for RXNE interrupt
     * 2. Check for TXE interrupt
     * 3. Check for TC interrupt
     * 4. Check for errors (OERR, FERR, PE)
     * 5. Call appropriate callbacks
     */
}

/**
  * @brief  TX complete callback.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @note   This is a weak implementation that can be overridden by the user.
  */
__weak void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    /* NOTE: This function should be implemented in the user file.
     * TODO: Implement TX complete callback
     */
}

/**
  * @brief  RX complete callback.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @note   This is a weak implementation that can be overridden by the user.
  */
__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    /* NOTE: This function should be implemented in the user file.
     * TODO: Implement RX complete callback
     */
}

/**
  * @brief  UART error callback.
  * @param  huart: pointer to a UART_HandleTypeDef structure.
  * @note   This is a weak implementation that can be overridden by the user.
  */
__weak void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    /* NOTE: This function should be implemented in the user file.
     * TODO: Implement error callback
     */
}

#endif /* HAL_UART_ENABLE */
