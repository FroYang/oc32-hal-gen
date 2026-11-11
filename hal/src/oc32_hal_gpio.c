/**
 ******************************************************************************
 * @file    oc32_hal_gpio.c
 * @author
 * @brief   OC32 HAL GPIO module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_gpio.h"

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the GPIO peripheral.
 * @param  Init: pointer to a HAL_GPIO_Inittypedef structure that contains
 *         the configuration information for the specified GPIO peripheral.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init->Port is not PORT_A/PORT_B
 * @note   For output mode: sets data, slew rate, and output enable.
 *         For input mode: configures pull, noise filter, and interrupt
 *         edge detection (RIE/FIE), and clears stale interrupt flags (IFC).
 */
HAL_StatusTypedef HAL_GPIO_Init(HAL_GPIO_InitTypedef *Init)
{
    if (Init->Port != PORT_A && Init->Port != PORT_B)
        return HAL_ERROR;

    if (Init->Direction == GPIO_OUTPUT)
    {
        if (Init->State == GPIO_STATE_HI)
        {
            Init->Port->D = Init->Port->D | Init->Pin;
        }
        else
        {
            Init->Port->D = Init->Port->D & ~(Init->Pin);
        }

        if (Init->Slew == GPIO_SLEW_LO)
        {
            Init->Port->LS = Init->Port->LS | Init->Pin;
        }
        else
        {
            Init->Port->LS = Init->Port->LS & ~(Init->Pin);
        }

        Init->Port->PU = Init->Port->PU & ~(Init->Pin);
        Init->Port->IE = Init->Port->IE & ~(Init->Pin);
        Init->Port->OE = Init->Port->OE | Init->Pin;
    }
    else /* GPIO_INPUT */
    {
        Init->Port->OE = Init->Port->OE & ~(Init->Pin);

        if (Init->Pull == GPIO_PULL_UP)
        {
            Init->Port->PU = Init->Port->PU | Init->Pin;
        }
        else
        {
            Init->Port->PU = Init->Port->PU & ~(Init->Pin);
        }

        /* Configure noise filter (PAFS0 = LSB, PAFS1 = bit 1) */
        if ((Init->Filter == GPIO_FILTER_LV0) || (Init->Filter == GPIO_FILTER_LV2))
        {
            Init->Port->FS0 = Init->Port->FS0 | Init->Pin;
        }
        else
        {
            Init->Port->FS0 = Init->Port->FS0 & ~(Init->Pin);
        }

        if ((Init->Filter == GPIO_FILTER_LV1) || (Init->Filter == GPIO_FILTER_LV2))
        {
            Init->Port->FS1 = Init->Port->FS1 | Init->Pin;
        }
        else
        {
            Init->Port->FS1 = Init->Port->FS1 & ~(Init->Pin);
        }

        Init->Port->IE = Init->Port->IE | Init->Pin;

        /* Configure interrupt edge detection */
        if ((Init->ExInt == GPIO_EI_RISE) || (Init->ExInt == GPIO_EI_BOTH))
        {
            Init->Port->RIE = Init->Port->RIE | Init->Pin;
        }
        else
        {
            Init->Port->RIE = Init->Port->RIE & ~(Init->Pin);
        }

        if ((Init->ExInt == GPIO_EI_FALL) || (Init->ExInt == GPIO_EI_BOTH))
        {
            Init->Port->FIE = Init->Port->FIE | Init->Pin;
        }
        else
        {
            Init->Port->FIE = Init->Port->FIE & ~(Init->Pin);
        }

        /* clear stale flags for clean start up */
        Init->Port->IFC = Init->Pin;
    }

    return HAL_OK;
}

/**
 * @brief  DeInitializes the GPIO peripheral.
 * @param  Init: pointer to a HAL_GPIO_Inittypedef structure that contains
 *         the configuration information for the specified GPIO peripheral.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init->Port is not PORT_A/PORT_B
 * @note   Disables output, pull, interrupt, and filter for the specified pin.
 */
HAL_StatusTypedef HAL_GPIO_DeInit(HAL_GPIO_InitTypedef *Init)
{
    if (Init->Port != PORT_A && Init->Port != PORT_B)
        return HAL_ERROR;

    Init->Port->OE = Init->Port->OE & ~(Init->Pin);
    Init->Port->D = Init->Port->D & ~(Init->Pin);

    Init->Port->IE = Init->Port->IE & ~(Init->Pin);
    Init->Port->PU = Init->Port->PU & ~(Init->Pin);
    Init->Port->LS = Init->Port->LS & ~(Init->Pin);
    Init->Port->RIE = Init->Port->RIE & ~(Init->Pin);
    Init->Port->FIE = Init->Port->FIE & ~(Init->Pin);
    Init->Port->FS0 = Init->Port->FS0 & ~(Init->Pin);
    Init->Port->FS1 = Init->Port->FS1 & ~(Init->Pin);
    Init->Port->IFC = Init->Pin;
    return HAL_OK;
}

/**
 * @brief  Handles GPIO interrupt request at user side.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 * 
 * NOTICE: USER MUST OVERRIDE THIS FUNCTION, AND CLEAR THE INTERRUPT FLAGS.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_GPIO_UserIRQ(MODULE_PORT_Typedef *Port)
{
  return HAL_OK;
}

/**
 * @brief  Handles VTS interrupt request.
 * @retval Return value of HAL_VTS_UserIRQ
 * @note   Invoked from the global interrupt handler. DeInitializes the VTS,
 *         then calls HAL_VTS_UserIRQ.
 */
__sram__ HAL_StatusTypedef HAL_GPIO_IRQHandler(MODULE_PORT_Typedef *Port)
{
    if ((Port == NULL) || (Port != PORT_A && Port != PORT_B))
        return HAL_ERROR;
        
  return HAL_GPIO_UserIRQ(Port);
}