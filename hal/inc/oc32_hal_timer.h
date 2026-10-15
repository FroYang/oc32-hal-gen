/**
 ******************************************************************************
 * @file    oc32_hal_timer.h
 * @author
 * @brief   Header file of TIMER HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_TIMER_H
#define __OC32_HAL_TIMER_H

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

  /** @addtogroup TIMER
   * @{
   */

  /* Exported types ------------------------------------------------------*/

  /**
   * @brief  TIMER count mode enum definition
   */
  typedef enum
  {
    TIMER_COUNT_UP = 0x0U,
    TIMER_COUNT_DOWN = 0x1U,
    TIMER_COUNT_TOGGLE = 0x2U,
    TIMER_COUNT_ENCODE = 0x3U
  } HAL_TIMER_CountModeTypedef;

  /**
   * @brief  TIMER count stop condition enum definition
   */
  typedef enum
  {
    TIMER_STOP_NONE = 0x0U,
    TIMER_STOP_OVER = 0x1U,
    TIMER_STOP_EVENT = 0x2U,
    TIMER_STOP_BOTH = 0x3U
  } HAL_TIMER_StopConTypedef;

  /**
   * @brief  TIMER overflow flag mode enum definition
   */
  typedef enum
  {
    TIMER_OVF_NONE = 0x0U,
    TIMER_OVF_OVER = 0x1U,
    TIMER_OVF_UNDER = 0x2U,
    TIMER_OVF_BOTH = 0x3U
  } HAL_TIMER_OvflagModeTypedef;

  /**
   * @brief  TIMER count source enum definition
   */
  typedef enum
  {
    TIMER_SOURCE_SCLK = 0x0U,
    TIMER_SOURCE_PIN0 = 0x1U,
    TIMER_SOURCE_PIN1 = 0x2U,
    TIMER_SOURCE_PIN2 = 0x3U,
    TIMER_SOURCE_CMP0 = 0x4U,
    TIMER_SOURCE_CMP1 = 0x5U,
    TIMER_SOURCE_T0 = 0x6U,
    TIMER_SOURCE_T1 = 0x7U
  } HAL_TIMER_CountSourceTypedef;

  /**
   * @brief  TIMER count ex-condition enum definition
   */
  typedef enum
  {
    TIMER_EXCON_RISE = 0U,
    TIMER_EXCON_FALL = 1U,
    TIMER_EXCON_BOTH = 2U
  } HAL_TIMER_CountExConTypedef;

  /**
   * @brief  TIMER update mode enum definition
   */
  typedef enum
  {
    TIMER_UPDATE_DIR = 0U,
    TIMER_UPDATE_TRIG = 1U,
    TIMER_UPDATE_EVENT = 2U,
    TIMER_UPDATE_LOCK = 3U
  } HAL_TIMER_UpdateModeTypedef;

  /**
   * @brief  TIMER encode input pin enum definition
   */
  typedef enum
  {
    TIMER_ENCODE_PIN0 = 0U,
    TIMER_ENCODE_PIN1 = 1U,
    TIMER_ENCODE_PIN2 = 2U
  } HAL_TIMER_EncPinTypedef;

  /**
   * @brief  TIMER encode mode enum definition
   */
  typedef enum
  {
    TIMER_ENCODE_IN0 = 0U,
    TIMER_ENCODE_IN1 = 1U,
    TIMER_ENCODE_BOTH = 2U
  } HAL_TIMER_EncModeTypedef;

  /**
   * @brief  TIMER encode polarity enum definition
   */
  typedef enum
  {
    TIMER_ENCODE_DEFAULT = 0U,
    TIMER_ENCODE_INVERT = 1U
  } HAL_TIMER_EncPolarityTypedef;

  /**
   * @brief  TIMER reset gen event enum definition
   */
  typedef enum
  {
    TIMER_RESET_GEN_NONE = 0x0U,
    TIMER_RESET_GEN_EVENT = 0x1U
  } HAL_TIMER_ResetGenTypedef;

  /**
   * @brief  TIMER ex-reset enum definition
   */
  typedef enum
  {
    TIMER_EXRESET_NONE = 0U,
    TIMER_EXRESET_T0 = 1U,
    TIMER_EXRESET_T1 = 2U,
    TIMER_EXRESET_CCP0 = 3U,
    TIMER_EXRESET_CCP1 = 4U,
    TIMER_EXRESET_CCP2 = 5U,
    TIMER_EXRESET_CCP3 = 6U,
    TIMER_EXRESET_CCP4 = 7U,
    TIMER_EXRESET_CCP5 = 8U,
    TIMER_EXRESET_ADC0 = 9U,
    TIMER_EXRESET_ADC1 = 10U,
    TIMER_EXRESET_CMP0 = 11U,
    TIMER_EXRESET_CMP1 = 12U,
    TIMER_EXRESET_PIN0 = 13U,
    TIMER_EXRESET_PIN1 = 14U,
    TIMER_EXRESET_PIN2 = 15U
  } HAL_TIMER_ExResetTypedef;

  /**
   * @brief  TIMER ex-reset condition enum definition
   */
  typedef enum
  {
    TIMER_EXRESET_RISE = 0U,
    TIMER_EXRESET_FALL = 1U,
    TIMER_EXRESET_HIGH = 2U,
    TIMER_EXRESET_LOW = 3U
  } HAL_TIMER_ExResetConTypedef;

  /**
   * @brief  TIMER pause gen event enum definition
   */
  typedef enum
  {
    TIMER_PAUSE_GEN_NONE = 0x0U,
    TIMER_PAUSE_GEN_EVENT = 0x1U
  } HAL_TIMER_PauseGenTypedef;

  /**
   * @brief  TIMER ex-pause enum definition
   */
  typedef enum
  {
    TIMER_EXPAUSE_NONE = 0U,
    TIMER_EXPAUSE_CMP0 = 1U,
    TIMER_EXPAUSE_CMP1 = 2U,
    TIMER_EXPAUSE_ADC0 = 3U,
    TIMER_EXPAUSE_ADC1 = 4U,
    TIMER_EXPAUSE_PIN0 = 5U,
    TIMER_EXPAUSE_PIN1 = 6U,
    TIMER_EXPAUSE_PIN2 = 7U
  } HAL_TIMER_ExPauseTypedef;

  /**
   * @brief  TIMER ex-pause condition enum definition
   */
  typedef enum
  {
    TIMER_EXPAUSE_HIGH = 0,
    TIMER_EXPAUSE_LOW = 1U
  } HAL_TIMER_ExPauseConTypedef;

  /**
   * @brief  TIMER event trigger source enum definition
   */
  typedef enum
  {
    TIMER_EVENT_OVF = 0,
    TIMER_EVENT_T0 = 1U,
    TIMER_EVENT_T1 = 2U,
    TIMER_EVENT_CCP0 = 3U,
    TIMER_EVENT_CCP1 = 4U,
    TIMER_EVENT_CCP2 = 5U,
    TIMER_EVENT_CCP3 = 6U,
    TIMER_EVENT_CCP4 = 7U,
    TIMER_EVENT_CCP5 = 8U,
    TIMER_EVENT_ADC0 = 9U,
    TIMER_EVENT_ADC1 = 10U,
    TIMER_EVENT_CMP0 = 11U,
    TIMER_EVENT_CMP1 = 12U,
    TIMER_EVENT_PIN0 = 13U,
    TIMER_EVENT_PIN1 = 14U,
    TIMER_EVENT_PIN2 = 15U
  } HAL_TIMER_EventSourceTypedef;

  /**
   * @brief  TIMER ex-event condition enum definition
   */
  typedef enum
  {
    TIMER_EXEVENT_RISE = 0,
    TIMER_EXEVENT_FALL = 1U
  } HAL_TIMER_ExEventConTypedef;

  /**
   * @brief  TIMER select buffer for read enum definition
   */
  typedef enum
  {
    TIMER_READ_REG_DIR = 0,
    TIMER_READ_REG_BUF = 1U
  } HAL_TIMER_ReadRegTypedef;

  /**
   * @brief  TIMER adc trigger condition enum definition
   */
  typedef enum
  {
    TIMER_ADC_TRIG_NONE = 0,
    TIMER_ADC_TRIG_OVER = 1U,
    TIMER_ADC_TRIG_UNDER = 2U,
    TIMER_ADC_TRIG_EVENT = 3U
  } HAL_TIMER_AdcTrigConTypedef;

  /**
   * @brief  TIMER base config Init structure definition
   */
  typedef struct
  {
    MODULE_TIMER_TypeDef *Timer;              /*!< Specifies the timer */
    uint32_t ReloadVal;                       /*!< Specifies the reload value */
    uint32_t PreScaler;                       /*!< Specifies the prescaler */
    HAL_MODULE_INTTypedef IntE;               /*!< Specifies the interrupt mode */
    HAL_TIMER_CountModeTypedef CountMode;     /*!< Specifies the count mode */
    HAL_TIMER_StopConTypedef StopCon;         /*!< Specifies the stop condition */
    HAL_TIMER_OvflagModeTypedef OvfMode;      /*!< Specifies the overflow flag mode */
    HAL_TIMER_CountSourceTypedef CountSource; /*!< Specifies the count source */
    HAL_TIMER_CountExConTypedef CountExCon;   /*!< Specifies the count ex-source condition */
    HAL_TIMER_UpdateModeTypedef UpdateMode;   /*!< Specifies the update mode */
    HAL_TIMER_ReadRegTypedef ReadReg;         /*!< Specifies the read way */
    HAL_TIMER_AdcTrigConTypedef AdcTrig;      /*!< Specifies the adc trig way */
  } HAL_TIMER_BaseInitTypeDef;

  /**
   * @brief  TIMER encode config Init structure definition
   */
  typedef struct
  {
    MODULE_TIMER_TypeDef *Timer;           /*!< Specifies the timer */
    HAL_TIMER_EncPinTypedef Enc0Pin;       /*!< Specifies the pin */
    HAL_TIMER_EncPinTypedef Enc1Pin;       /*!< Specifies the pin */
    HAL_TIMER_EncModeTypedef Mode;         /*!< Specifies the count mode */
    HAL_TIMER_EncPolarityTypedef Polarity; /*!< Specifies the count polarity */
  } HAL_TIMER_EncodeInitTypeDef;

  /**
   * @brief  TIMER reset config Init structure definition
   */
  typedef struct
  {
    MODULE_TIMER_TypeDef *Timer;        /*!< Specifies the timer */
    HAL_TIMER_ResetGenTypedef GenEvent; /*!< Specifies the reset event generation */
    HAL_TIMER_ExResetTypedef ExSource;  /*!< Specifies the reset ex-source */
    HAL_TIMER_ExResetConTypedef ExCon;  /*!< Specifies the reset ex-source conditon */
  } HAL_TIMER_ResetInitTypeDef;

  /**
   * @brief  TIMER pause config Init structure definition
   */
  typedef struct
  {
    MODULE_TIMER_TypeDef *Timer;        /*!< Specifies the timer */
    HAL_TIMER_PauseGenTypedef GenEvent; /*!< Specifies the pause event generation */
    HAL_TIMER_ExPauseTypedef ExSource;  /*!< Specifies the reset ex-source */
    HAL_TIMER_ExPauseConTypedef ExCon;  /*!< Specifies the reset ex-source conditon */
  } HAL_TIMER_PauseInitTypeDef;

  /**
   * @brief  TIMER event config Init structure definition
   */
  typedef struct
  {
    MODULE_TIMER_TypeDef *Timer;         /*!< Specifies the timer */
    uint32_t OvfPost;                    /*!< Specifies the overflag event post counter */
    HAL_MODULE_INTTypedef IntE;          /*!< Specifies the event interrupt mode */
    HAL_TIMER_EventSourceTypedef Source; /*!< Specifies the event source */
    HAL_TIMER_ExEventConTypedef ExCon;   /*!< Specifies the event ex-source condition */
  } HAL_TIMER_EventInitTypeDef;

  /* Exported constants --------------------------------------------------*/

  /* Exported macros -------------------------------------------------------*/

  /* Exported functions ----------------------------------------------------*/

#ifdef HAL_TIMER_ENABLE
  HAL_StatusTypeDef HAL_TIMER_Init(HAL_TIMER_BaseInitTypeDef *Init);
  HAL_StatusTypeDef HAL_TIMER_DeInit(MODULE_TIMER_TypeDef *Timer);
  HAL_StatusTypeDef HAL_TIMER_ResetInit(HAL_TIMER_ResetInitTypeDef *Init);
  __weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_ResetNow(MODULE_TIMER_TypeDef *Timer);
  HAL_StatusTypeDef HAL_TIMER_PauseInit(HAL_TIMER_PauseInitTypeDef *Init);
  __weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_EventInit(HAL_TIMER_EventInitTypeDef *Init);
  __weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_WaitOv(MODULE_TIMER_TypeDef *Timer);
  __weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_WaitEv(MODULE_TIMER_TypeDef *Timer);
  __weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_UserIRQ(MODULE_TIMER_TypeDef *Timer);
  __weak__ __sram__ HAL_StatusTypeDef HAL_TIMER_IRQHandler(MODULE_TIMER_TypeDef *Timer);
#endif /* HAL_TIMER_ENABLE */

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_TIMER_H */
