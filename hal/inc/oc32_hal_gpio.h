/**
 ******************************************************************************
 * @file    oc32_hal_gpio.h
 * @author
 * @brief   Header file of GPIO HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_GPIO_H
#define __OC32_HAL_GPIO_H

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

    /** @addtogroup GPIO
     * @{
     */

    /* Exported types ------------------------------------------------------*/

    /**
     * @brief  Pin enum definition
     */
    typedef enum
    {
        PIN0 = 0x00000001U,
        PIN1 = 0x00000002U,
        PIN2 = 0x00000004U,
        PIN3 = 0x00000008U,
        PIN4 = 0x00000010U,
        PIN5 = 0x00000020U,
        PIN6 = 0x00000040U,
        PIN7 = 0x00000080U,
        PIN8 = 0x00000100U,
        PIN9 = 0x00000200U,
        PIN10 = 0x00000400U,
        PIN11 = 0x00000800U,
        PIN12 = 0x00001000U,
        PIN13 = 0x00002000U,
        PIN14 = 0x00004000U,
        PIN15 = 0x00008000U,
        PIN16 = 0x00010000U,
        PIN17 = 0x00020000U,
        PIN18 = 0x00040000U,
        PIN19 = 0x00080000U,
        PIN20 = 0x00100000U,
        PIN21 = 0x00200000U,
        PIN22 = 0x00400000U,
        PIN23 = 0x00800000U,
        PIN24 = 0x01000000U,
        PIN25 = 0x02000000U,
        PIN26 = 0x04000000U,
        PIN27 = 0x08000000U,
        PIN28 = 0x10000000U,
        PIN29 = 0x20000000U,
        PIN30 = 0x40000000U,
        PIN31 = 0x80000000U
    } HAL_GPIOPinTypedef;

    /**
     * @brief  Mode enum definition
     */
    typedef enum
    {
        GPIO_INPUT = 0U,
        GPIO_OUTPUT = 1U,
    } HAL_GPIOModeTypedef;

    /**
     * @brief  state enum definition
     */
    typedef enum
    {
        GPIO_STATE_LO = 0u,
        GPIO_STATE_HI = 1u
    } HAL_GPIOStateTypedef;

    /**
     * @brief  slew enum definition
     */
    typedef enum
    {
        GPIO_SLEW_HI = 0u,
        GPIO_SLEW_LO = 1u
    } HAL_GPIOSlewTypedef;

    /**
     * @brief  pull enum definition
     */
    typedef enum
    {
        GPIO_PULL_NONE = 0u,
        GPIO_PULL_UP = 1u,
        GPIO_PULL_DOWN = 2u
    } HAL_GPIOPullTypedef;

    /**
     * @brief  exint enum definition
     */
    typedef enum
    {
        GPIO_EI_NONE = 0u,
        GPIO_EI_RISE = 1u,
        GPIO_EI_FALL = 2U,
        GPIO_EI_BOTH = 3U
    } HAL_GPIOExIntTypedef;

    /**
     * @brief  filter enum definition
     */
    typedef enum
    {
        GPIO_FILTER_NONE = 0u,
        GPIO_FILTER_LV0 = 1u,
        GPIO_FILTER_LV1 = 2U,
        GPIO_FILTER_LV2 = 3U
    } HAL_GPIOFilterTypedef;

    /**
     * @brief  GPIO Init structure definition
     */
    typedef struct
    {
        GROUP_PORT_TypeDef *Port; /*!< Specifies the GPIO port to be configured */
        HAL_GPIOPinTypedef Pin;   /*!< Specifies the GPIO pins to be configured */
        HAL_GPIOModeTypedef Mode; /*!< Specifies the operating mode for the selected pins */

        /* output config */
        HAL_GPIOStateTypedef State; /*!< Specifies the GPIO pins output state */
        HAL_GPIOSlewTypedef Slew;   /*!< Specifies the Slewrate control */

        /* input config */
        HAL_GPIOPullTypedef Pull;     /*!< Specifies the Pull-up activation */
        HAL_GPIOExIntTypedef ExInt;   /*!< Specifies the External Interrupt control */
        HAL_GPIOFilterTypedef Filter; /*!< Specifies the Filter control */
    } HAL_GPIOInitTypeDef;

    /**
     * @}
     */

    /* Exported constants --------------------------------------------------*/

    /**
     * @}
     */

    /* Exported functions ----------------------------------------------------*/

    /** @addtogroup GPIO_Exported_Functions
     * @{
     */

    /* Initialization and de-initialization functions *************************/
    HAL_StatusTypeDef HAL_GPIO_Init(HAL_GPIOInitTypeDef *Init);
    HAL_StatusTypeDef HAL_GPIO_DeInit(HAL_GPIOInitTypeDef *Init);

    /**
     * @}
     */

    /* Private macros --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_GPIO_H */
