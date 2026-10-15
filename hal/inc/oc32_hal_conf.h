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

#include "CV32S6015MSR.h"

/* Exported constants ----------------------------------------------------*/

/** @defgroup HAL_Clocks HAL Clocks
 * @{
 */
#define OC32_XHOSC_FREQ ((uint32_t)4000000U)  /* External oscillator frequency in Hz */
#define OC32_IHOSC_FREQ ((uint32_t)24000000U) /* Internal oscillator high frequency in Hz */
#define OC32_ILOSC_FREQ ((uint32_t)4000U)     /* Internal oscillator low frequency in Hz */
#define OC32_CDR_FREQ ((uint32_t)12000000U)   /* USB CDR output frequency in Hz */
#define OC32_SCLK_FREQ ((uint32_t)99000000U)  /* System clock frequency in Hz */
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
      OC32_DMA_CH3 = 3U,
      OC32_DMA_CH_TOTAL
   } HAL_DMA_ChTypedef;

   typedef enum
   {
      OC32_DMA_IDLE = 0U,
      OC32_DMA_BUSY = 1U
   } HAL_DMA_StateTypedef;

   typedef enum
   {
      OC32_DMA_SPI0 = 0U,
      OC32_DMA_SPI1 = 1U,
      OC32_DMA_IIC = 2U,
      OC32_DMA_UART0 = 3U,
      OC32_DMA_UART1 = 4U,
      OC32_DMA_ADC0 = 5U,
      OC32_DMA_MPWM0 = 6U,
      OC32_DMA_MPWM1 = 7U,
      OC32_DMA_MPWM2 = 8U,
      OC32_DMA_FLASH = 9U,
      OC32_DMA_DAC0 = 10U,
      OC32_DMA_ADC1 = 11U,
      OC32_DMA_DAC1 = 12U,
      OC32_DMA_reserved0 = 13U,
      OC32_DMA_reserved1 = 14U,
      OC32_DMA_CRC = 15U,
      OC32_DMA_NONE
   } HAL_DMA_MapTypedef;

   typedef enum
   {
      OC32_HIC_CH_SERR = 0U,
      OC32_HIC_CH_CMP0 = 1U,
      OC32_HIC_CH_CMP1 = 2U,
      OC32_HIC_CH_CCP0 = 3U,
      OC32_HIC_CH_CCP1 = 4U,
      OC32_HIC_CH_CCP2 = 5U,
      OC32_HIC_CH_CCP3 = 6U,
      OC32_HIC_CH_CCP4 = 7U,
      OC32_HIC_CH_CCP5 = 8U,
      OC32_HIC_CH_TIMER0 = 9U,
      OC32_HIC_CH_TIMER1 = 10U,
      OC32_HIC_CH_TIMER2 = 11U,
      OC32_HIC_CH_TIMER3 = 12U,
      OC32_HIC_CH_TIMER4 = 13U,
      OC32_HIC_CH_TIMER5 = 14U,
      OC32_HIC_CH_TIMER6 = 15U,
      OC32_HIC_CH_TIMER7 = 16U,
      OC32_HIC_CH_WDT = 17U,
      OC32_HIC_CH_ADC0 = 18U,
      OC32_HIC_CH_ADC1 = 19U,
      OC32_HIC_CH_UART0 = 20U,
      OC32_HIC_CH_UART1 = 21U,
      OC32_HIC_CH_SPI0 = 22U,
      OC32_HIC_CH_SPI1 = 23U,
      OC32_HIC_CH_IIC = 24U,
      OC32_HIC_CH_MPWM0 = 25U,
      OC32_HIC_CH_MPWM1 = 26U,
      OC32_HIC_CH_MPWM2 = 27U,
      OC32_HIC_CH_DAC = 28U,
      OC32_HIC_CH_CRC = 29U,
      OC32_HIC_CH_PORTA = 30U,
      OC32_HIC_CH_PORTB = 31U,
      OC32_HIC_CH_TOTAL
   } HAL_HIC_ChTypedef;

   typedef enum
   {
      OC32_HIC_SERR = 1U << 0,
      OC32_HIC_CMP0 = 1U << 1,
      OC32_HIC_CMP1 = 1U << 2,
      OC32_HIC_CCP0 = 1U << 3,
      OC32_HIC_CCP1 = 1U << 4,
      OC32_HIC_CCP2 = 1U << 5,
      OC32_HIC_CCP3 = 1U << 6,
      OC32_HIC_CCP4 = 1U << 7,
      OC32_HIC_CCP5 = 1U << 8,
      OC32_HIC_TIMER0 = 1U << 9,
      OC32_HIC_TIMER1 = 1U << 10,
      OC32_HIC_TIMER2 = 1U << 11,
      OC32_HIC_TIMER3 = 1U << 12,
      OC32_HIC_TIMER4 = 1U << 13,
      OC32_HIC_TIMER5 = 1U << 14,
      OC32_HIC_TIMER6 = 1U << 15,
      OC32_HIC_TIMER7 = 1U << 16,
      OC32_HIC_WDT = 1U << 17,
      OC32_HIC_ADC0 = 1U << 18,
      OC32_HIC_ADC1 = 1U << 19,
      OC32_HIC_UART0 = 1U << 20,
      OC32_HIC_UART1 = 1U << 21,
      OC32_HIC_SPI0 = 1U << 22,
      OC32_HIC_SPI1 = 1U << 23,
      OC32_HIC_IIC = 1U << 24,
      OC32_HIC_MPWM0 = 1U << 25,
      OC32_HIC_MPWM1 = 1U << 26,
      OC32_HIC_MPWM2 = 1U << 27,
      OC32_HIC_DAC = 1U << 28,
      OC32_HIC_CRC = 1U << 29,
      OC32_HIC_PORTA = 1U << 30,
      OC32_HIC_PORTB = 1U << 31,
      OC32_HIC_NONE
   } HAL_HIC_MapTypedef;

   /**
    * @}
    */

   /** @defgroup HAL_INT_Priority Int Configuration
    * @{
    */
   typedef enum
   {
      OC32_HIC_PRI0 = 0U,
      OC32_HIC_PRI1 = 1U,
      OC32_HIC_PRI2 = 2U,
      OC32_HIC_PRI3 = 3U
   } HAL_HIC_PriorityTypedef;

/**
 * @}
 */

/* ########################### System Configuration ######################### */
/**
 * @brief This is the HAL system configuration section
 */
#define HAL_VDD_VALUE 5000U      /*!< Value of VDD in mv */
#define HAL_FLASH_VERIFY_ENABLE
#define HAL_UART_DEBUG_ENABLE

#ifdef HAL_UART_DEBUG_ENABLE
#define HAL_UART_DEBUG_BR 115200 /* 9600/14400 .... ~ 1000000 */
/* #define HAL_UART0_DEBUG */
#define HAL_UART1_DEBUG
#endif

   typedef enum
   {
      HAL_TICK_FREQ_10Hz = 100U,
      HAL_TICK_FREQ_100Hz = 10U,
      HAL_TICK_FREQ_1KHz = 1U,
      HAL_TICK_FREQ_DEFAULT = HAL_TICK_FREQ_1KHz
   } HAL_TICK_FreqTypeDef;

   extern volatile uint32_t uwTick;
   extern HAL_TICK_FreqTypeDef uwTickFreq;

/** @defgroup HAL_ENABLE Module Enable
 * @{
 */

/* use defined enable modules */
#define HAL_FLASH_ENABLE
#define HAL_CRC_ENABLE
#define HAL_LVD_ENABLE
#define HAL_WDT_ENABLE
#define HAL_UART_ENABLE
#define HAL_TIMER_ENABLE

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
