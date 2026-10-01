/**
 ******************************************************************************
 * @file    oc32_hal_conf.h
 * @author
 * @brief   Configuration header file of OC32 HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_CONF_H
#define __OC32_HAL_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Exported constants ----------------------------------------------------*/

/** @defgroup HAL_Clocks HAL Clocks
 * @{
 */
#define OC32_XHOSC_FREQ ((uint32_t)4)  /* External oscillator frequency in MHz */
#define OC32_IHOSC_FREQ ((uint32_t)24) /* Internal oscillator frequency in MHz */
#define OC32_SCLK_FREQ ((uint32_t)99)  /* System clock frequency in MHz */
/**
 * @}
 */



 
/** @defgroup HAL_FLASH_Config FLASH Configuration
 * @{
 */
#define OC32_FLASH_SIZE ((uint32_t)0x40000U)                            /* 256K Flash */
#define OC32_CODE_ZONE0_SIZE ((uint32_t)0x7800U)                        /* 30 KB Boot zone */
#define OC32_CODE_ZONE1_SIZE ((uint32_t)0x18800U)                       /* 98 KB User zone */
#define OC32_CODE_BANK_SIZE OC32_CODE_ZONE0_SIZE + OC32_CODE_ZONE1_SIZE /* 2 banks in a 256k flash*/
   /**
    * @}
    */

   /** @defgroup HAL_DMA_Config DMA Configuration
    * @{
    */
   typedef enum
   {
      OC32_DMA_CH0 = 0U,
      OC32_DMA_CH1 = 1U,
      OC32_DMA_CH2 = 2U,
      OC32_DMA_CH3 = 3U
   } HAL_DMAChTypedef;

#define OC32_DMA_SPI0 ((uint32_t)0U)
#define OC32_DMA_SPI1 ((uint32_t)1U)
#define OC32_DMA_IIC ((uint32_t)2U)
#define OC32_DMA_UART0 ((uint32_t)3U)
#define OC32_DMA_UART1 ((uint32_t)4U)
#define OC32_DMA_ADC0 ((uint32_t)5U)
#define OC32_DMA_MPWM0 ((uint32_t)6U)
#define OC32_DMA_MPWM1 ((uint32_t)7U)
#define OC32_DMA_MPWM2 ((uint32_t)8U)
#define OC32_DMA_FLASH ((uint32_t)9U)
#define OC32_DMA_DAC0 ((uint32_t)10U)
#define OC32_DMA_ADC1 ((uint32_t)11U)
#define OC32_DMA_DAC1 ((uint32_t)12U)
#define OC32_DMA_reserved0 ((uint32_t)13U)
#define OC32_DMA_reserved1 ((uint32_t)14U)
#define OC32_DMA_CRC ((uint32_t)15U)
/**
 * @}
 */

/* ########################### System Configuration ######################### */
/**
 * @brief This is the HAL system configuration section
 */
#define HAL_VDD_VALUE 5000U      /*!< Value of VDD in mv */
#define HAL_TICK_INT_PRIORITY 0U /*!< tick interrupt priority: small num -> low priority */
#define HAL_FLASH_VERIFY_ENABLE
#define HAL_FLASH_VERIFY_RETRY 3U
#define HAL_UART_DEBUG_ENABLE

#ifdef HAL_UART_DEBUG_ENABLE
#define HAL_UART_DEBUG_BR 115200 /* 9600/14400 .... ~ 1000000 */
/* #define HAL_UART0_DEBUG */
#define HAL_UART1_DEBUG
#endif

typedef enum
{
  HAL_TICK_FREQ_10HZ         = 100U,
  HAL_TICK_FREQ_100HZ        = 10U,
  HAL_TICK_FREQ_1KHZ         = 1U,
  HAL_TICK_FREQ_DEFAULT      = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;

extern volatile uint32_t uwTick;
extern uint32_t uwTickPrio;
extern HAL_TickFreqTypeDef uwTickFreq;

/** @defgroup HAL_ENABLE Module Enable
 * @{
 */

/* use defined enable modules */
#define HAL_FLASH_ENABLE
#define HAL_LVD_ENABLE

   /* enable depends on model list:
      #define HAL_PORT_B_ENABLE
      #define HAL_PORT_C_ENABLE
      #define HAL_PORT_D_ENABLE
      #define HAL_USB_ENABLE
      #define HAL_RCM_CDR_ENABLE */

   /**
    * @}
    */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_CONF_H */
