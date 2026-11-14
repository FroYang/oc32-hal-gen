/**
 ******************************************************************************
 * @file    oc32_hal_adc.h
 * @author
 * @brief   Header file of ADC (Analog-to-Digital Converter) HAL module.
 *
 ******************************************************************************
 */

#ifndef __OC32_HAL_ADC_H
#define __OC32_HAL_ADC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

  /* extern ------------------------------------------------------------*/
  extern uint32_t DMAChState[OC32_DMA_CH_TOTAL];
  extern __sram__ HAL_StatusTypedef DmaUpdate(HAL_EnableTypedef DMAE, HAL_DMA_ChTypedef DmaCh, HAL_DMA_MapTypedef Module, HAL_DMA_StateTypedef State);

  /** @addtogroup OC32_HAL_Driver
   * @{
   */

  /* Exported types ------------------------------------------------------*/
  /**
   * @brief  ADC resolution definition
   */
  typedef enum
  {
    ADC_RES_12B = 0U, /*!< 12-bit resolution */
    ADC_RES_10B = 1U, /*!< 10-bit resolution */
    ADC_RES_9B = 2U,  /*!< 9-bit resolution */
    ADC_RES_8B = 3U   /*!< 8-bit resolution */
  } HAL_ADC_ResolutionTypedef;

  /**
   * @brief  ADC data alignment definition
   */
  typedef enum
  {
    ADC_ALIGN_LSB = 0U, /*!< LSB alignment */
    ADC_ALIGN_MSB = 1U  /*!< MSB alignment */
  } HAL_ADC_AlignTypedef;

  /**
   * @brief  ADC buffer mode definition
   */
  typedef enum
  {
    ADC_BUF_256BX2 = 0U, /*!< 256x2 buffer mode: 1st buffer for Event0, 2nd buffer for Event1~7(active one only) */
    ADC_BUF_128BX2 = 1U, /*!< 128x2 buffer mode: 1st buffer for Event0, 2nd buffer for Event1~7(active one only) */
    ADC_BUF_64BX2 = 2U,  /*!< 64x2 buffer mode: 1st buffer for Event0, 2nd buffer for Event1~7(active one only) */
    ADC_BUF_COMBINE = 3U /*!< Combined buffer mode: 32-bit buffer for each Event(2 samples) per line,
                              line0(1st & 2nd sample): Event0 , Event1 , ... Event7 - (32-bit per event)
                              line1(3rd & 4th sample): Event0 , Event1 , ... Event7 - (32-bit per event)
                              ... */
  } HAL_ADC_BufferTypedef;

  /**
   * @brief  ADC clock divide definition
   */
  typedef enum
  {
    ADC_CLK_DIV1 = 0U, /*!< Clock divide 1 */
    ADC_CLK_DIV2 = 1U, /*!< Clock divide 2 */
    ADC_CLK_DIV3 = 2U, /*!< Clock divide 3 */
    ADC_CLK_DIV4 = 3U, /*!< Clock divide 4 */
    ADC_CLK_DIV5 = 4U, /*!< Clock divide 5 */
    ADC_CLK_DIV6 = 5U, /*!< Clock divide 6 */
    ADC_CLK_DIV7 = 6U, /*!< Clock divide 7 */
    ADC_CLK_DIV8 = 7U  /*!< Clock divide 8 */
  } HAL_ADC_ClkDivTypedef;

  /**
   * @brief  ADC sample cycles definition
   */
  typedef enum
  {
    ADC_SAMP_CYC1 = 0U, /*!< Sample cycles 1 */
    ADC_SAMP_CYC2 = 1U, /*!< Sample cycles 2 */
    ADC_SAMP_CYC3 = 2U, /*!< Sample cycles 3 */
    ADC_SAMP_CYC4 = 3U, /*!< Sample cycles 4 */
    ADC_SAMP_CYC5 = 4U, /*!< Sample cycles 5 */
    ADC_SAMP_CYC6 = 5U, /*!< Sample cycles 6 */
    ADC_SAMP_CYC7 = 6U, /*!< Sample cycles 7 */
    ADC_SAMP_CYC8 = 7U  /*!< Sample cycles 8 */
  } HAL_ADC_SampleCycleTypedef;

  /**
   * @brief  ADC input source definition
   */
  typedef enum
  {
    ADC_INPUT0 = 0U,
    ADC_INPUT1 = 1U,
    ADC_INPUT2 = 2U,
    ADC_INPUT3 = 3U,
    ADC_INPUT4 = 4U,
    ADC_INPUT5 = 5U,
    ADC_INPUT6 = 6U,
    ADC_INPUT7 = 7U,
    ADC_INPUT8 = 8U,
    ADC_INPUT9 = 9U,
    ADC_INPUT10 = 10U,
    ADC_INPUT11 = 11U,
    ADC_INPUT12 = 12U,
    ADC_INPUT13 = 13U,
    ADC_INPUT14 = 14U,
    ADC_INPUT15 = 15U,
    ADC_INPUT16 = 16U,
    ADC_INPUT17 = 17U,
    ADC_INPUT18 = 18U,
    ADC_INPUT19 = 19U,
    ADC_INPUT20 = 20U,
    ADC_INPUT21 = 21U,
    ADC_INPUT22 = 22U,
    ADC_INPUT23 = 23U
  } HAL_ADC_InputTypedef;

  /**
   * @brief  ADC reference source definition
   */
  typedef enum
  {
    ADC_REF0 = 0U,
    ADC_REF1 = 1U,
    ADC_REF2 = 2U,
    ADC_REF3 = 3U,
    ADC_REF4 = 4U,
    ADC_REF5 = 5U,
    ADC_REF6 = 6U,
    ADC_REF7 = 7U,
    ADC_REF8 = 8U,
    ADC_REF9 = 9U,
    ADC_REF10 = 10U,
    ADC_REF11 = 11U,
    ADC_REF12 = 12U,
    ADC_REF13 = 13U,
    ADC_REF14 = 14U,
    ADC_REF15 = 15U
  } HAL_ADC_RefTypedef;

  /**
   * @brief  ADC Event definition
   */
  typedef enum
  {
    ADC_EVENT0 = 0U, /*!< Event0 */
    ADC_EVENT1 = 1U, /*!< Event1 */
    ADC_EVENT2 = 2U, /*!< Event2 */
    ADC_EVENT3 = 3U, /*!< Event3 */
    ADC_EVENT4 = 4U, /*!< Event4 */
    ADC_EVENT5 = 5U, /*!< Event5 */
    ADC_EVENT6 = 6U, /*!< Event6 */
    ADC_EVENT7 = 7U, /*!< Event7 */
    ADC_EVENT_TOTAL = 8U /*!< Event total */
  } HAL_ADC_EventChannelTypedef;

  /**
   * @brief  ADC event start delay priority definition
   */
  typedef enum
  {
    ADC_EVENT_SDLY_AFTER = 0U, /*!< Event enters priority queue after start delay */
    ADC_EVENT_SDLY_BEFORE = 1U /*!< Event enters priority queue before start delay */
  } HAL_ADC_EventSdlyPriorityTypedef;

  /**
   * @brief  ADC event start delay unit definition
   */
  typedef enum
  {
    ADC_EVENT_SDLY_X8 = 0U,   /*!< 8 clock cycle */
    ADC_EVENT_SDLY_X16 = 1U,  /*!< 16 clock cycle */
    ADC_EVENT_SDLY_X32 = 2U,  /*!< 32 clock cycle */
    ADC_EVENT_SDLY_X64 = 3U,  /*!< 64 clock cycle */
    ADC_EVENT_SDLY_X128 = 4U, /*!< 128 clock cycle */
    ADC_EVENT_SDLY_X256 = 5U, /*!< 256 clock cycle */
    ADC_EVENT_SDLY_X512 = 6U, /*!< 512 clock cycle */
    ADC_EVENT_SDLY_X1024 = 7U /*!< 1024 clock cycle */
  } HAL_ADC_EventSdlyUnitTypedef;

  /**
   * @brief  ADC event start delay count definition
   */
  typedef enum
  {
    ADC_EVENT_SDLY_CNT0 = 0U,   /*!< 0 */
    ADC_EVENT_SDLY_CNT1 = 1U,   /*!< 1 */
    ADC_EVENT_SDLY_CNT2 = 2U,   /*!< 2 */
    ADC_EVENT_SDLY_CNT3 = 3U,   /*!< 3 */
    ADC_EVENT_SDLY_CNT4 = 4U,   /*!< 4 */
    ADC_EVENT_SDLY_CNT5 = 5U,   /*!< 5 */
    ADC_EVENT_SDLY_CNT6 = 6U,   /*!< 6 */
    ADC_EVENT_SDLY_CNT7 = 7U,   /*!< 7 */
    ADC_EVENT_SDLY_CNT8 = 8U,   /*!< 8 */
    ADC_EVENT_SDLY_CNT9 = 9U,   /*!< 9 */
    ADC_EVENT_SDLY_CNT10 = 10U, /*!< 10 */
    ADC_EVENT_SDLY_CNT11 = 11U, /*!< 11 */
    ADC_EVENT_SDLY_CNT12 = 12U, /*!< 12 */
    ADC_EVENT_SDLY_CNT13 = 13U, /*!< 13 */
    ADC_EVENT_SDLY_CNT14 = 14U, /*!< 14 */
    ADC_EVENT_SDLY_CNT15 = 15U  /*!< 15 */
  } HAL_ADC_EventSdlyCountTypedef;

  /**
   * @brief  ADC event source definition
   */
  typedef enum
  {
    ADC_EVENT_DISABLE = 0U,
    ADC_EVENT_USER = 1U,
    ADC_EVENT_TIMER0 = 2U,
    ADC_EVENT_TIMER1 = 3U,
    ADC_EVENT_TIMER2 = 4U,
    ADC_EVENT_TIMER3 = 5U,
    ADC_EVENT_TIMER4 = 6U,
    ADC_EVENT_TIMER5 = 7U,
    ADC_EVENT_TIMER6 = 8U,
    ADC_EVENT_TIMER7 = 9U,
    ADC_EVENT_PIN0 = 10U,
    ADC_EVENT_PIN1 = 11U,
    ADC_EVENT_CMP0 = 12U,
    ADC_EVENT_CMP1 = 13U,
    ADC_EVENT_CMP2 = 14U,
    ADC_EVENT_ALWAYS = 15U,
  } HAL_ADC_EventSourceTypedef;

  /**
   * @brief  ADC event priority definition
   */
  typedef enum
  {
    ADC_EVENT_PRI0 = 0U, /*!< priority 0: lowest */
    ADC_EVENT_PRI1 = 1U, /*!< priority 1 */
    ADC_EVENT_PRI2 = 2U, /*!< priority 2 */
    ADC_EVENT_PRI3 = 3U, /*!< priority 3 */
    ADC_EVENT_PRI4 = 4U, /*!< priority 4 */
    ADC_EVENT_PRI5 = 5U, /*!< priority 5 */
    ADC_EVENT_PRI6 = 6U, /*!< priority 6 */
    ADC_EVENT_PRI7 = 7U  /*!< priority 7: highest */
  } HAL_ADC_EventPriorityTypedef;

  /**
   * @brief  ADC Init structure definition
   */
  typedef struct
  {
    MODULE_ADC_Typedef *Adc;              /*!< Specifies the ADC */
    HAL_ADC_ClkDivTypedef ClkDiv;         /*!< Specifies the Clock Divider */
    HAL_ADC_ResolutionTypedef Res;        /*!< Specifies the ADC resolution */
    HAL_ADC_BufferTypedef Mode;           /*!< Specifies the ADC buffer mode */
    HAL_ADC_AlignTypedef Align;           /*!< Specifies the ADC data alignment */
    HAL_ADC_RefTypedef Ref;               /*!< Specifies the ADC reference source */
    HAL_ADC_EventSdlyPriorityTypedef Pri; /*!< Specifies the ADC event start delay priority */
    HAL_ADC_EventSdlyUnitTypedef Unit;    /*!< Specifies the ADC event start delay unit */
    uint32_t *Buf;                        /*!< DMA data buffer (SRAM). Must be aligned to a 32-bit boundary */
    uint32_t Watch0Top;                   /*!< Watch 0 top value */
    uint32_t Watch0Bottom;                /*!< Watch 0 bottom value */
    uint32_t Watch1Top;                   /*!< Watch 1 top value */
    uint32_t Watch1Bottom;                /*!< Watch 1 bottom value */
    HAL_DMA_ChTypedef DmaCh;              /*!< Specifies DMA channel */
  } HAL_ADC_InitTypedef;

  /**
   * @brief  ADC Event structure definition
   */
  typedef struct
  {
    MODULE_ADC_Typedef *Adc;             /*!< Specifies the ADC */
    HAL_ADC_EventChannelTypedef Channel; /*!< Specifies the ADC event */
    HAL_ADC_InputTypedef Input;          /*!< Specifies the ADC input */
    HAL_ADC_EventSourceTypedef Source;   /*!< Specifies the ADC event source */
    HAL_ADC_EventPriorityTypedef Pri;    /*!< Specifies the ADC event priority */
    HAL_ADC_EventSdlyCountTypedef Delay; /*!< Specifies the ADC event start delay count */
    HAL_EnableTypedef IntE;              /*!< Specifies the interrupt */
    HAL_ADC_SampleCycleTypedef Cycle;    /*!< Specifies the ADC sample cycle */
    uint32_t Count;                      /*!< Number of times of ADC conversions. Non-DMA max 1, DMA max 128. */
    HAL_EnableTypedef Watch0E;           /*!< Specifies the watch 0 enable */
    HAL_EnableTypedef Watch1E;           /*!< Specifies the watch 1 enable */
  } HAL_ADC_EventTypedef;

  /**
   * @}
   */

  /** @addtogroup ADC
   * @{
   */
#ifdef HAL_ADC_ENABLE

  /* Initialization and de-initialization functions *************************/
  HAL_StatusTypedef HAL_ADC_Init(HAL_ADC_InitTypedef *Init);
  HAL_StatusTypedef HAL_ADC_DeInit(HAL_ADC_InitTypedef *Init);
  __sram__ HAL_ResultTypedef HAL_ADC_EventConvert(HAL_ADC_EventTypedef *Event);
  __sram__ HAL_StatusTypedef HAL_ADC_IRQHandler(MODULE_ADC_Typedef *Adc);
  __sram__ __weak__ HAL_StatusTypedef HAL_ADC_UserIRQ(MODULE_ADC_Typedef *Adc);
#endif /* HAL_ADC_ENABLE */
  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_ADC_H */
