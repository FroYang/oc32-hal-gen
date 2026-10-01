/**
 ******************************************************************************
 * @file    oc32_hal_gpio.c
 * @author
 * @brief   OC32 HAL GPIO module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the GPIO peripheral.
 * @param  Port: GROUP_PA/GROUP_PB/.. select the Group port register.
 * @param  Init: pointer to a HAL_GPIOInitTypeDef structure that contains
 *         the configuration information for the specified GPIO peripheral.
 * @retval None
 */
HAL_StatusTypeDef HAL_GPIO_Init(GROUP_PORT_TypeDef *Port, HAL_GPIOInitTypeDef *Init)
{

    if (Init->Mode == GPIO_OUTPUT)
    {
        if (Init->State == GPIO_HI)
        {
            Port->D = Port->D | Init->Pin;
        }
        else
        {
            Port->D = Port->D & ~Init->Pin;
        }

        if (Init->Slew == GPIO_LOWSLEW_ON)
        {
            Port->LS = Port->LS | Init->Pin;
        }
        else
        {
            Port->LS = Port->LS & ~Init->Pin;
        }

        Port->PU = Port->PU & ~Init->Pin;
        Port->IE = Port->IE & ~Init->Pin;
        Port->OE = Port->OE | Init->Pin;
    }
    else /* GPIO_INPUT */
    {
        Port->OE = Port->OE & ~Init->Pin;

        if (Init->Pull == GPIO_PULL_UP)
        {
            Port->PU = Port->PU | Init->Pin;
        }
        else
        {
            Port->PU = Port->PU & ~Init->Pin;
        }

        /* Configure noise filter (PAFS0 = LSB, PAFS1 = bit 1) */
        if ((Init->Filter == GPIO_FILTER_LV0) || (Init->Filter == GPIO_FILTER_LV2))
        {
            Port->FS0 = Port->FS0 | Init->Pin;
        }
        else
        {
            Port->FS0 = Port->FS0 & ~Init->Pin;
        }

        if ((Init->Filter == GPIO_FILTER_LV1) || (Init->Filter == GPIO_FILTER_LV2))
        {
            Port->FS1 = Port->FS1 | Init->Pin;
        }
        else
        {
            Port->FS1 = Port->FS1 & ~Init->Pin;
        }

        Port->IE = Port->IE | Init->Pin;

        /* Configure interrupt edge detection */
        if ((Init->ExInt == GPIO_EI_RISE) || (Init->ExInt == GPIO_EI_BOTH))
        {
            Port->RIE = Port->RIE | Init->Pin;
        }
        else
        {
            Port->RIE = Port->RIE & ~Init->Pin;
        }

        if ((Init->ExInt == GPIO_EI_FALL) || (Init->ExInt == GPIO_EI_BOTH))
        {
            Port->FIE = Port->FIE | Init->Pin;
        }
        else
        {
            Port->FIE = Port->FIE & ~Init->Pin;
        }

        /* clear stale flags for clean start up */
        Port->IFC = Init->Pin;
    }

    return HAL_OK;
}

/**
 * @brief  DeInitializes the GPIO peripheral.
 * @param  Port: GROUP_PA/GROUP_PB/.. select the Group port register.
 * @param  Pin: specifies the port pins to deinitialize.
 * @retval None
 */
HAL_StatusTypeDef HAL_GPIO_DeInit(GROUP_PORT_TypeDef *Port, uint32_t Pin)
{
    Port->OE = Port->OE & ~Pin;
    Port->D = Port->D & ~Pin;
    
    Port->IE = Port->IE & ~Pin;
    Port->PU = Port->PU & ~Pin;
    Port->LS = Port->LS & ~Pin;
    Port->RIE = Port->RIE & ~Pin;
    Port->FIE = Port->FIE & ~Pin;
    Port->FS0 = Port->FS0 & ~Pin;
    Port->FS1 = Port->FS1 & ~Pin;
    Port->IFC = Pin;
    return HAL_OK;
}

/**
 * @brief  Reads the specified input port pin.
 * @param  Port: GROUP_PA/GROUP_PB/.. select the Group port register.
 * @param  Pin: specifies the port bit to read.
 *         This parameter can be Pin_x where x can be (0..15).
 * @retval The input port pin value.
 */
HAL_GPIO_State HAL_GPIO_ReadPin(GROUP_PORT_TypeDef *Port, uint32_t Pin)
{
    return ((Port->D & Pin) != 0u) ? GPIO_HI : GPIO_LO;
}

/**
 * @brief  Sets or clears the selected data port bit.
 * @param  Port: GROUP_PA/GROUP_PB/.. select the Group port register.
 * @param  Pin: specifies the port bit to be written.
 * @param  State: specifies the value to be written to the selected bit.
 *         This parameter can be one of the State enum values.
 * @retval None
 */
void HAL_GPIO_WritePin(GROUP_PORT_TypeDef *Port, uint32_t Pin, HAL_GPIO_State State)
{
    if (State == GPIO_HI)
    {
        Port->D = Port->D | Pin;
    }
    else
    {
        Port->D = Port->D & ~Pin;
    }
}

/**
 * @brief  Toggles the specified GPIO pin.
 * @param  Port: GROUP_PA/GROUP_PB/.. select the Group port register.
 * @param  Pin: specifies the pin(s) to be toggled.
 * @retval None
 */
void HAL_GPIO_TogglePin(GROUP_PORT_TypeDef *Port, uint32_t Pin)
{
    Port->D ^= Pin;
}
