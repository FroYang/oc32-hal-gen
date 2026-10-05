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
extern "C" {
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
 * @brief  GPIO Init structure definition
 */
typedef struct {
    uint32_t Pin;  /*!< Specifies the GPIO pins to be configured */
    uint32_t Mode; /*!< Specifies the operating mode for the selected pins */

    /* output config */
    uint32_t State; /*!< Specifies the GPIO pins output state */
    uint32_t Slew;  /*!< Specifies the Slewrate control */

    /* input config */
    uint32_t Pull;   /*!< Specifies the Pull-up activation */
    uint32_t ExInt;  /*!< Specifies the External Interrupt control */
    uint32_t Filter; /*!< Specifies the Filter control */
} HAL_GPIOInitTypeDef;

/**
 * @}
 */

/* Exported constants --------------------------------------------------*/

/** @defgroup GPIO_Exported_Constants GPIO Exported Constants
 * @{
 */

/** @defgroup GPIO_port_define GPIO port define
 * @{
 */
#define GPIO_PORT_A 0x00000000u /*!< PORT A, always be */

#ifdef HAL_PORT_B_ENABLE
#define GPIO_PORT_B 0x00000001u /*!< PORT B */
#endif

#ifdef HAL_PORT_C_ENABLE
#define GPIO_PORT_C 0x00000002u /*!< PORT C */
#endif

#ifdef HAL_PORT_D_ENABLE
#define GPIO_PORT_D 0x00000003u /*!< PORT D */
#endif

/**
 * @}
 */

/** @defgroup HAL_GPIO_State_define GPIO state define
 * @{
 */
typedef enum {
    LO = 0u,
    HI = 1u
} HAL_GPIO_State;

/**
 * @}
 */

/** @defgroup GPIO_mode_define GPIO mode define
 * @{
 */
#define GPIO_INPUT  0x00000000u /*!< Input Floating Mode */
#define GPIO_OUTPUT 0x00000001u /*!< Output Push-pull */
/**
 * @}
 */

/** @defgroup GPIO_mode_define GPIO mode define
 * @{
 */
#define GPIO_EI_NONE 0x00000000u /*!< External Interrupt Disable */
#define GPIO_EI_RISE 0x00000001u /*!< External Interrupt at Rising edge */
#define GPIO_EI_FALL 0x00000002u /*!< External Interrupt at Falling edge */
#define GPIO_EI_BOTH 0x00000003u /*!< External Interrupt at Rising/Falling edge */
/**
 * @}
 */

/** @defgroup GPIO_pull_define GPIO pull define
 * @{
 */
#define GPIO_PULL_OFF 0x00000000u /*!< No Pull-up or Pull-down activation */
#define GPIO_PULL_UP  0x00000001u /*!< Pull-up activation */
/**
 * @}
 */

/** @defgroup GPIO_filter_define GPIO filter define
 * @{
 */
#define GPIO_FILTER_OFF 0x00000000u /*!< No Filter (PAFS0=0, PAFS1=0) */
#define GPIO_FILTER_LV0 0x00000001u /*!< Filter level 0 (PAFS0=1, PAFS1=0) */
#define GPIO_FILTER_LV1 0x00000002u /*!< Filter level 1 (PAFS0=0, PAFS1=1) */
#define GPIO_FILTER_LV2 0x00000003u /*!< Filter level 2 (PAFS0=1, PAFS1=1) */
/**
 * @}
 */

/** @defgroup GPIO_LowSlew_define GPIO Low Slewrate control define
 * @{
 */
#define GPIO_LOWSLEW_OFF 0x00000000u /*!< Low Slewrate control off */
#define GPIO_LOWSLEW_ON  0x00000001u /*!< Low Slewrate control on  */
/**
 * @}
 */

#define PIN0  ((uint32_t)0x00000001)
#define PIN1  ((uint32_t)0x00000002)
#define PIN2  ((uint32_t)0x00000004)
#define PIN3  ((uint32_t)0x00000008)
#define PIN4  ((uint32_t)0x00000010)
#define PIN5  ((uint32_t)0x00000020)
#define PIN6  ((uint32_t)0x00000040)
#define PIN7  ((uint32_t)0x00000080)
#define PIN8  ((uint32_t)0x00000100)
#define PIN9  ((uint32_t)0x00000200)
#define PIN10 ((uint32_t)0x00000400)
#define PIN11 ((uint32_t)0x00000800)
#define PIN12 ((uint32_t)0x00001000)
#define PIN13 ((uint32_t)0x00002000)
#define PIN14 ((uint32_t)0x00004000)
#define PIN15 ((uint32_t)0x00008000)
#define PIN16 ((uint32_t)0x00010000)
#define PIN17 ((uint32_t)0x00020000)
#define PIN18 ((uint32_t)0x00040000)
#define PIN19 ((uint32_t)0x00080000)
#define PIN20 ((uint32_t)0x00100000)
#define PIN21 ((uint32_t)0x00200000)
#define PIN22 ((uint32_t)0x00400000)
#define PIN23 ((uint32_t)0x00800000)
#define PIN24 ((uint32_t)0x01000000)
#define PIN25 ((uint32_t)0x02000000)
#define PIN26 ((uint32_t)0x04000000)
#define PIN27 ((uint32_t)0x08000000)
#define PIN28 ((uint32_t)0x10000000)
#define PIN29 ((uint32_t)0x20000000)
#define PIN30 ((uint32_t)0x40000000)
#define PIN31 ((uint32_t)0x80000000)
/**
 * @}
 */

/**
 * @}
 */

/* Exported functions ----------------------------------------------------*/

/** @addtogroup GPIO_Exported_Functions
 * @{
 */

/* Initialization and de-initialization functions *************************/
HAL_StatusTypeDef HAL_GPIO_Init(GROUP_PORT_TypeDef *Port, HAL_GPIOInitTypeDef *Init);
HAL_StatusTypeDef HAL_GPIO_DeInit(GROUP_PORT_TypeDef *Port, uint32_t Pin);

/* IO operation functions *************************************************/
HAL_GPIO_State HAL_GPIO_ReadPin(GROUP_PORT_TypeDef *Port, uint32_t Pin);
void HAL_GPIO_WritePin(GROUP_PORT_TypeDef *Port, uint32_t Pin, HAL_GPIO_State State);
void HAL_GPIO_TogglePin(GROUP_PORT_TypeDef *Port, uint32_t Pin);

/**
 * @}
 */

/* Private macros --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_GPIO_H */
