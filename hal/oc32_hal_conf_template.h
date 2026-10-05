/**
 ******************************************************************************
 * @file    oc32_hal_conf.h
 * @author
 * @brief   Configuration header file of __HAL_CONFIG_NAME__ HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_CONF_H
#define __OC32_HAL_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "__HAL_CONFIG_NAME__MSR.h"

/* Exported constants ----------------------------------------------------*/

/** @defgroup HAL_Clocks HAL Clocks
 * @{
 */
#define OC32_XHOSC_FREQ ((uint32_t)__HAL_CONFIG_XHOSC_FREQ__) /* External oscillator frequency in Hz */
#define OC32_IHOSC_FREQ ((uint32_t)__HAL_CONFIG_IHOSC_FREQ__) /* Internal oscillator frequency in Hz */
#define OC32_ILOSC_FREQ ((uint32_t)__HAL_CONFIG_ILOSC_FREQ__) /* Internal oscillator frequency in Hz */
#define OC32_SCLK_FREQ ((uint32_t)__HAL_CONFIG_SCLK_FREQ__)   /* System clock frequency in MHz */
/**
 * @}
 */

/** @defgroup HAL_FLASH_Config FLASH Configuration
 * @{
 */
#define OC32_FLASH_SIZE ((uint32_t)__HAL_CONFIG_FLASH_SIZE__)           /* Flash comment */
#define OC32_CODE_ZONE0_SIZE ((uint32_t)__HAL_CONFIG_ZONE0_SIZE__)      /* Zone0 comment */
#define OC32_CODE_ZONE1_SIZE ((uint32_t)__HAL_CONFIG_ZONE1_SIZE__)      /* Zone1 comment */
#define OC32_CODE_BANK_SIZE OC32_CODE_ZONE0_SIZE + OC32_CODE_ZONE1_SIZE /* Bank comment */
  /**
   * @}
   */

  /** @defgroup HAL_DMA_Config DMA Configuration
   * @{
   */
  typedef enum
  {
    __HAL_CONFIG_DMA_CH_ENUM__
  } HAL_DMAChTypedef;

  __HAL_CONFIG_DMA_CH_MAP__
  /**
   * @}
   */

  /** @defgroup HAL_INT_Priority Int Configuration
   * @{
   */
  typedef enum
  {
    __HAL_CONFIG_INT_PRI_ENUM__
  } HAL_INTPriTypedef;

/**
 * @}
 */

/* ########################### System Configuration ######################### */
/**
 * @brief This is the HAL system configuration section
 */
#define HAL_VDD_VALUE __HAL_CONFIG_VDD_VALUE__             /*!< Value of VDD in mv */
#define HAL_TICK_INT_PRIORITY __HAL_CONFIG_TICK_PRIORITY__ /*!< tick interrupt priority: small num -> low priority */

  typedef enum
  {
    HAL_TICK_FREQ_10HZ = 100U,
    HAL_TICK_FREQ_100HZ = 10U,
    HAL_TICK_FREQ_1KHZ = 1U,
    HAL_TICK_FREQ_DEFAULT = __HAL_CONFIG_TICK_FREQ__
  } HAL_TickFreqTypeDef;

  extern volatile uint32_t uwTick;
  extern HAL_TickFreqTypeDef uwTickFreq;

  __HAL_CONFIG_UART_DEBUG__

#ifdef HAL_UART_DEBUG_ENABLE
  __HAL_CONFIG_UART_DEBUG_BR__
  __HAL_CONFIG_UART_DEBUG_SUB__
#endif

  /** @defgroup HAL_ENABLE Module Enable
   * @{
   */

  /* use defined enable modules */
  __HAL_CONFIG_USER_MODULES__

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_CONF_H */
