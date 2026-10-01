/**
  ******************************************************************************
  * @file    oc32_hal_tim.h
  * @author
  * @brief   Header file of TIM HAL module.
  ******************************************************************************
  */

#ifndef __OC32_HAL_TIM_H
#define __OC32_HAL_TIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"
#include "CV32S6015MSR.h"

#ifdef HAL_TIM_ENABLE

/** @addtogroup OC32_HAL_Driver
  * @{
  */

/** @addtogroup TIM
  * @{
  */

/* Exported types ------------------------------------------------------*/

/**
  * @brief  TIM Time Base Configuration Structure definition
  */
typedef struct {
    uint32_t Prescaler;         /*!< Specifies the prescaler value */
    uint32_t CounterMode;       /*!< Specifies the counter mode */
    uint32_t Period;            /*!< Specifies the period value */
    uint32_t ClockDivision;     /*!< Specifies the clock division */
    uint32_t RepetitionCounter; /*!< Specifies the repetition counter value */
    uint32_t AutoReloadPreload; /*!< Specifies the auto-reload preload */
} TIM_Base_InitTypeDef;

/**
  * @brief  TIM Output Compare Configuration Structure definition
  */
typedef struct {
    uint32_t OCMode;        /*!< Specifies the TIM mode */
    uint32_t Pulse;         /*!< Specifies the pulse value */
    uint32_t OCPolarity;    /*!< Specifies the output polarity */
    uint32_t OCNPolarity;   /*!< Specifies the complementary output polarity */
    uint32_t OCFastMode;    /*!< Specifies the Fast mode state */
    uint32_t OCIdleState;   /*!< Specifies the TIM Output pin state during Idle state */
    uint32_t OCNIdleState;  /*!< Specifies the TIM complementary Output pin state during Idle */
} TIM_OC_InitTypeDef;

/**
  * @brief  TIM Input Capture Configuration Structure definition
  */
typedef struct {
    uint32_t ICPolarity;  /*!< Specifies the active edge */
    uint32_t ICSelection; /*!< Specifies the input */
    uint32_t ICPrescaler; /*!< Specifies the Input Prescaler */
    uint32_t ICFilter;    /*!< Specifies the input capture filter */
} TIM_IC_InitTypeDef;

/**
  * @brief  TIM One Pulse Mode Configuration Structure definition
  */
typedef struct {
    uint32_t OCMode;        /*!< Specifies the TIM mode */
    uint32_t Pulse;         /*!< Specifies the pulse value */
    uint32_t OCPolarity;    /*!< Specifies the output polarity */
    uint32_t OCNPolarity;   /*!< Specifies the complementary output polarity */
    uint32_t OCIdleState;   /*!< Specifies the TIM Output pin state during Idle state */
    uint32_t OCNIdleState;  /*!< Specifies the TIM complementary Output pin state during Idle */
    uint32_t ICPolarity;    /*!< Specifies the active edge of the input signal */
    uint32_t ICSelection;   /*!< Specifies the input */
    uint32_t ICFilter;      /*!< Specifies the input capture filter */
} TIM_OnePulse_InitTypeDef;

/**
  * @brief  TIM Encoder Configuration Structure definition
  */
typedef struct {
    uint32_t EncoderMode;  /*!< Specifies the active edge */
    uint32_t IC1Polarity;  /*!< Specifies the active edge of the input signal */
    uint32_t IC1Selection; /*!< Specifies the input */
    uint32_t IC1Prescaler; /*!< Specifies the Input Prescaler */
    uint32_t IC1Filter;    /*!< Specifies the input capture filter */
    uint32_t IC2Polarity;  /*!< Specifies the active edge of the input signal */
    uint32_t IC2Selection; /*!< Specifies the input */
    uint32_t IC2Prescaler; /*!< Specifies the Input Prescaler */
    uint32_t IC2Filter;    /*!< Specifies the input capture filter */
} TIM_Encoder_InitTypeDef;

/**
  * @brief  TIM Hall Sensor Configuration Structure definition
  * @note   For TIM1 to TIM11 only
  */
typedef struct {
    uint32_t IC1Polarity;  /*!< Specifies the active edge of the input signal */
    uint32_t IC1Selection; /*!< Specifies the input */
    uint32_t IC1Prescaler; /*!< Specifies the Input Prescaler */
    uint32_t IC1Filter;    /*!< Specifies the input capture filter */
    uint32_t CommutationDelay; /*!< Specifies the capture compare value to be loaded into the Capture Compare Register */
} TIM_HallSensor_InitTypeDef;

/**
  * @brief  TIM Handle Structure definition
  */
typedef struct __TIM_HandleTypeDef {
    TIM_TypeDef *Instance;                /*!< Register base address */
    TIM_Base_InitTypeDef Init;            /*!< TIM Time Base required configuration */
    DMA_HandleTypeDef *hdma[7];           /*!< DMA Handles array */
    HAL_LockTypeDef Lock;                 /*!< TIM locking object */
    __IO HAL_StateTypeDef State;          /*!< TIM state information */
    __IO uint32_tErrorCode;               /*!< TIM Error code */
    void (* PeriodElapsedCallback)(struct __TIM_HandleTypeDef *htim) __weak;
    void (* PeriodElapsedHalfCpltCallback)(struct __TIM_HandleTypeDef *htim) __weak;
    void (* TriggerCallback)(struct __TIM_HandleTypeDef *htim) __weak;
    void (* TriggerHalfCpltCallback)(struct __TIM_HandleTypeDef *htim) __weak;
    void (* IC_CaptureCallback)(struct __TIM_HandleTypeDef *htim, uint32_t Channel) __weak;
    void (* OC_DelayExpiredCallback)(struct __TIM_HandleTypeDef *htim, uint32_t Channel) __weak;
    void (* PWM_PulseFinishedCallback)(struct __TIM_HandleTypeDef *htim, uint32_t Channel) __weak;
    void (* PWM_PulseFinishedHalfCpltCallback)(struct __TIM_HandleTypeDef *htim, uint32_t Channel) __weak;
    void (* EncoderCallback)(struct __TIM_HandleTypeDef *htim) __weak;
    void (* HallErrorCallback)(struct __TIM_HandleTypeDef *htim) __weak;
    void (* ErrorCallback)(struct __TIM_HandleTypeDef *htim) __weak;
    void (* MspInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (* MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
} TIM_HandleTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------*/

/** @defgroup TIM_Exported_Constants TIM Exported Constants
  * @{
  */

/** @defgroup TIM_Counter_Mode TIM Counter Mode
  * @{
  */
#define TIM_COUNTERMODE_UP                 0x00000000U
#define TIM_COUNTERMODE_DOWN               TIM_CNTDIR_DOWN
#define TIM_COUNTERMODE_CENTERALIGNED1     ((uint32_t)TIM_CNTDIR_CENTERALIGNED1)
#define TIM_COUNTERMODE_CENTERALIGNED2     ((uint32_t)TIM_CNTDIR_CENTERALIGNED2)
#define TIM_COUNTERMODE_CENTERALIGNED3     ((uint32_t)TIM_CNTDIR_CENTERALIGNED3)
/**
  * @}
  */

/** @defgroup TIM_ClockDivision TIM Clock Division
  * @{
  */
#define TIM_CLOCKDIVISION_DIV1             0x00000000U
#define TIM_CLOCKDIVISION_DIV2             ((uint32_t)TIM_CR1_DTS_0)
#define TIM_CLOCKDIVISION_DIV4             ((uint32_t)TIM_CR1_DTS_1)
/**
  * @}
  */

/** @defgroup TIM_AutoReloadPreload TIM Auto-Reload Preload
  * @{
  */
#define TIM_AUTORELOAD_PRELOAD_DISABLE     0x00000000U
#define TIM_AUTORELOAD_PRELOAD_ENABLE      TIM_CR1_ARPE
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_and_PWM_modes TIM Output Compare and PWM Modes
  * @{
  */
#define TIM_OCMODE_TIMING                  0x00000000U
#define TIM_OCMODE_ACTIVE                  ((uint32_t)TIM_CCMR1_OC1M_0)
#define TIM_OCMODE_INACTIVE                ((uint32_t)TIM_CCMR1_OC1M_1)
#define TIM_OCMODE_TOGGLE                  ((uint32_t)(TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0))
#define TIM_OCMODE_PWM1                    ((uint32_t)(TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1))
#define TIM_OCMODE_PWM2                    ((uint32_t)(TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0))
#define TIM_OCMODE_FORCED_ACTIVE           ((uint32_t)TIM_CCMR1_OC1M_2)
#define TIM_OCMODE_FORCED_INACTIVE         ((uint32_t)TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1)
/**
  * @}
  */

/** @defgroup TIM_Output_Polarity TIM Output Polarity
  * @{
  */
#define TIM_OCPOLARITY_HIGH                0x00000000U
#define TIM_OCPOLARITY_LOW                 TIM_CCER_CC1P
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Polarity TIM Input Capture Polarity
  * @{
  */
#define TIM_ICPOLARITY_RISING             0x00000000U
#define TIM_ICPOLARITY_FALLING            TIM_CCER_CC1P
#define TIM_ICPOLARITY_BOTH_EDGE          (TIM_CCER_CC1P | TIM_CCER_CC1NP)
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Selection TIM Input Capture Selection
  * @{
  */
#define TIM_ICSELECTION_DIRECTTI          ((uint32_t)TIM_CCMR1_CC1S_0)
#define TIM_ICSELECTION_INDIRECTTI        ((uint32_t)TIM_CCMR1_CC1S_1)
#define TIM_ICSELECTION_TRC               ((uint32_t)TIM_CCMR1_CC1S)
/**
  * @}
  */

/** @defgroup TIM_Clear_Input_Selection TIM Clear Input Selection
  * @{
  */
#define TIM_CLEARINPUT_DISABLE            0x00000000U
#define TIM_CLEARINPUTsource_ETR          ((uint32_t)TIM_SMCR_ETPC_0)
/**
  * @}
  */

/** @defgroup TIM_Flag_Define TIM Flag Define
  * @{
  */
#define TIM_FLAG_UPDATE        TIM_SR_UIF
#define TIM_FLAG_CC1           TIM_SR_CC1F
#define TIM_FLAG_CC2           TIM_SR_CC2F
#define TIM_FLAG_CC3           TIM_SR_CC3F
#define TIM_FLAG_CC4           TIM_SR_CC4F
#define TIM_FLAG_COM           TIM_SR_COMF
#define TIM_FLAG_TRIGGER       TIM_SR_TIF
#define TIM_FLAG_BREAK         TIM_SR_BIF
#define TIM_FLAG_CC1OF         TIM_SR_CC1OF
#define TIM_FLAG_CC2OF         TIM_SR_CC2OF
#define TIM_FLAG_CC3OF         TIM_SR_CC3OF
#define TIM_FLAG_CC4OF         TIM_SR_CC4OF
/**
  * @}
  */

/** @defgroup TIM_IT_Define TIM Interrupt Define
  * @{
  */
#define TIM_IT_UPDATE      TIM_DIER_UIE
#define TIM_IT_CC1         TIM_DIER_CC1IE
#define TIM_IT_CC2         TIM_DIER_CC2IE
#define TIM_IT_CC3         TIM_DIER_CC3IE
#define TIM_IT_CC4         TIM_DIER_CC4IE
#define TIM_IT_COM         TIM_DIER_COMIE
#define TIM_IT_TRIGGER     TIM_DIER_TIE
#define TIM_IT_BREAK       TIM_DIER_BIE
/**
  * @}
  */

/** @defgroup TIM_DMA_Base_Address TIM DMA Base Address
  * @{
  */
#define TIM_DMABASE_CR1                0x0000U
#define TIM_DMABASE_CR2                0x0001U
#define TIM_DMABASE_SMCR               0x0002U
#define TIM_DMABASE_DIER               0x0003U
#define TIM_DMABASE_SR                 0x0004U
#define TIM_DMABASE_EGR                0x0005U
#define TIM_DMABASE_CCMR1              0x0006U
#define TIM_DMABASE_CCMR2              0x0007U
#define TIM_DMABASE_CCER               0x0008U
#define TIM_DMABASE_CNT                0x0009U
#define TIM_DMABASE_PSC                0x000AU
#define TIM_DMABASE_ARR                0x000BU
#define TIM_DMABASE_RCR                0x000CU
#define TIM_DMABASE_CCR1               0x000DU
#define TIM_DMABASE_CCR2               0x000EU
#define TIM_DMABASE_CCR3               0x000FU
#define TIM_DMABASE_CCR4               0x0010U
#define TIM_DMABASE_BDTR               0x0011U
#define TIM_DMABASE_DCR                0x0012U
#define TIM_DMABASE_DMAR               0x0013U
/**
  * @}
  */

/** @defgroup TIM_DMA_Burst_Length TIM DMA Burst Length
  * @{
  */
#define TIM_DMABURSTLENGTH_1TRANSFER   0x0000U
#define TIM_DMABURSTLENGTH_2TRANSFERS  0x0100U
#define TIM_DMABURSTLENGTH_3TRANSFERS  0x0200U
#define TIM_DMABURSTLENGTH_4TRANSFERS  0x0300U
#define TIM_DMABURSTLENGTH_5TRANSFERS  0x0400U
#define TIM_DMABURSTLENGTH_6TRANSFERS  0x0500U
#define TIM_DMABURSTLENGTH_7TRANSFERS  0x0600U
#define TIM_DMABURSTLENGTH_8TRANSFERS  0x0700U
#define TIM_DMABURSTLENGTH_9TRANSFERS  0x0800U
#define TIM_DMABURSTLENGTH_10TRANSFERS 0x0900U
#define TIM_DMABURSTLENGTH_11TRANSFERS 0x0A00U
#define TIM_DMABURSTLENGTH_12TRANSFERS 0x0B00U
#define TIM_DMABURSTLENGTH_13TRANSFERS 0x0C00U
#define TIM_DMABURSTLENGTH_14TRANSFERS 0x0D00U
#define TIM_DMABURSTLENGTH_15TRANSFERS 0x0E00U
#define TIM_DMABURSTLENGTH_16TRANSFERS 0x0F00U
#define TIM_DMABURSTLENGTH_17TRANSFERS 0x1000U
#define TIM_DMABURSTLENGTH_18TRANSFERS 0x1100U
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -------------------------------------------------------*/

/** @defgroup TIM_Exported_Macros TIM Exported Macros
  * @{
  */
#define __HAL_TIM_ENABLE(__HANDLE__)                   ((__HANDLE__)->Instance->CR1 |= TIM_CR1_CEN)
#define __HAL_TIM_DISABLE(__HANDLE__)                  ((__HANDLE__)->Instance->CR1 &= ~TIM_CR1_CEN)
#define __HAL_TIM_GET_FLAG(__HANDLE__, __FLAG__)       (((__HANDLE__)->Instance->SR & (__FLAG__)) == (__FLAG__))
#define __HAL_TIM_CLEAR_FLAG(__HANDLE__, __FLAG__)     ((__HANDLE__)->Instance->SR = ~(__FLAG__))
#define __HAL_TIM_GET_SR(__HANDLE__)                   ((__HANDLE__)->Instance->SR)
#define __HAL_TIM_ENABLE_IT(__HANDLE__, __IT__)        ((__HANDLE__)->Instance->DIER |= (__IT__))
#define __HAL_TIM_DISABLE_IT(__HANDLE__, __IT__)       ((__HANDLE__)->Instance->DIER &= ~(__IT__))
#define __HAL_TIM_GET_ITSOURCE(__HANDLE__, __IT__)     (((__HANDLE__)->Instance->DIER & (__IT__)) == (__IT__))
#define __HAL_TIM_CLEAR_IT(__HANDLE__, __IT__)         ((__HANDLE__)->Instance->SR = ~(__IT__))
#define __HAL_TIM_IS_TIM_COUNTING_DOWN(__HANDLE__)     (((__HANDLE__)->Instance->CNTDIR & TIM_CNTDIR_DOWN) == TIM_CNTDIR_DOWN)
#define __HAL_TIM_SET_COUNTER(__HANDLE__, __COUNT__)   ((__HANDLE__)->Instance->CNT = (__COUNT__))
#define __HAL_TIM_GET_COUNTER(__HANDLE__)              ((__HANDLE__)->Instance->CNT)
#define __HAL_TIM_SET_AUTORELOAD(__HANDLE__, __VALUE__) ((__HANDLE__)->Instance->ARR = (__VALUE__))
#define __HAL_TIM_GET_AUTORELOAD(__HANDLE__)            ((__HANDLE__)->Instance->ARR)
#define __HAL_TIM_SET_COMPARE(__HANDLE__, __CHANNEL__, __VALUE__) \
    (((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCR1 = (__VALUE__)) : \
    ((__CHANNEL__) == TIM_CHANNEL_2) ? ((__HANDLE__)->Instance->CCR2 = (__VALUE__)) : \
    ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCR3 = (__VALUE__)) : \
    ((__HANDLE__)->Instance->CCR4 = (__VALUE__)))
#define __HAL_TIM_GET_COMPARE(__HANDLE__, __CHANNEL__) \
    (((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCR1) : \
    ((__CHANNEL__) == TIM_CHANNEL_2) ? ((__HANDLE__)->Instance->CCR2) : \
    ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCR3) : \
    ((__HANDLE__)->Instance->CCR4))
/**
  * @}
  */

/* Exported functions ----------------------------------------------------*/

/** @addtogroup TIM_Exported_Functions_Group1
  * @{
  */
/* Initialization and de-initialization functions *************************/
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) __weak;
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim) __weak;
HAL_StatusTypeDef HAL_TIM_PWM_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_PWM_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) __weak;
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim) __weak;
HAL_StatusTypeDef HAL_TIM_IC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_IC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim) __weak;
void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim) __weak;
HAL_StatusTypeDef HAL_TIM_OC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_OC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim) __weak;
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef *htim) __weak;
HAL_StatusTypeDef HAL_TIM_OnePulse_Init(TIM_HandleTypeDef *htim, uint32_t OnePulseMode);
HAL_StatusTypeDef HAL_TIM_OnePulse_DeInit(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Encoder_Init(TIM_HandleTypeDef *htim, TIM_Encoder_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIM_Encoder_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim) __weak;
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef *htim) __weak;

/* Clock Source configuration -------------------------------------------*/
HAL_StatusTypeDef HAL_TIM_ConfigClockSource(TIM_HandleTypeDef *htim, TIM_ClockSourceTypeDef *sClockSourceConfig);
HAL_StatusTypeDef HAL_TIM_ConfigCLKDis_selection(TIM_HandleTypeDef *htim, uint32_t ClockSource);

/**
  * @}
  */

/** @addtogroup TIM_Exported_Functions_Group2
  * @{
  */
/* IO operation functions *************************************************/
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Base_Stop_DMA(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);

/* Callback handling functions ********************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) __weak;
void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim) __weak;
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim) __weak;
void HAL_TIM_TriggerHalfCpltCallback(TIM_HandleTypeDef *htim) __weak;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim, uint32_t Channel) __weak;
void HAL_TIM_OC_DelayExpiredCallback(TIM_HandleTypeDef *htim, uint32_t Channel) __weak;
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim, uint32_t Channel) __weak;
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim, uint32_t Channel) __weak;
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim) __weak;
void HAL_TIM.CommUTationCallback(TIM_HandleTypeDef *htim) __weak;
/**
  * @}
  */

/** @addtogroup TIM_Exported_Functions_Group3
  * @{
  */
/* Peripheral Control functions *******************************************/
HAL_StatusTypeDef HAL_TIM_ConfigOcpolairty(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfigOC);
HAL_StatusTypeDef HAL_TIM_ConfigChannelOutputMaster(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t OutputType);
HAL_StatusTypeDef HAL_TIM_ConfigClockSource(TIM_HandleTypeDef *htim, TIM_ClockSourceTypeDef *sClockSourceConfig);
HAL_StatusTypeDef HAL_TIM_ConfigTI1Input(TIM_HandleTypeDef *htim, uint32_t TI1InputSelection);
HAL_StatusTypeDef HAL_TIM_SetICPrescalerValue(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t ICInputPrescaler);
HAL_StatusTypeDef HAL_TIM_ConfigOCRCDMA(TIM_HandleTypeDef *htim, TIM_OCRCDISConfigTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIM_EnableCommutationDMA(TIM_HandleTypeDef *htim, uint32_t CommitmentDMARequest, uint32_t CommitmentDMABase, uint32_t CommitmentDMABurstLength);
HAL_StatusTypeDef HAL_TIM_DisableCommutationDMA(TIM_HandleTypeDef *htim, uint32_t CommitmentDMARequest);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutationEvent(TIM_HandleTypeDef *htim, uint32_t InputTrigger, uint32_t CommitmentEventActivation);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutationEvent_DMA(TIM_HandleTypeDef *htim, uint32_t InputTrigger, uint32_t CommitmentEventActivation);
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(TIM_HandleTypeDef *htim, TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig);
HAL_StatusTypeDef HAL_TIMEx_ActiveCommutation(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_DeActiveCommuration(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_ConfigOcnMor(TIM_HandleTypeDef *htim, uint32_t OCMor, uint32_t BreakInput, uint32_t BreakPolarity, uint32_t BreakFilter, uint32_t Aotosoutput, uint32_t DClockStatus, uint32_t OffStateRunMode, uint32_t OffStateIDLEMode, uint32_t LockLevel, uint32_t DeadTime);
HAL_StatusTypeDef HAL_TIMEx_EnableBreakInput(TIM_HandleTypeDef *htim, uint32_t BreakInput);
HAL_StatusTypeDef HAL_TIMEx_DisableBreakInput(TIM_HandleTypeDef *htim, uint32_t BreakInput);
HAL_StatusTypeDef HAL_TIMEx_EnableMODMM(TIM_HandleTypeDef *htim, uint32_t MODMMSrc, uint32_t MODMMDisasbleMask);
HAL_StatusTypeDef HAL_TIMEx_DisableMODMM(TIM_HandleTypeDef *htim, uint32_t MODMMSrc, uint32_t MODMMDisasbleMask);
HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim, uint32_t Remap);
HAL_StatusTypeDef HAL_TIMEx_Disable睡M(TIM_HandleTypeDef *htim, uint32_t睡M);
HAL_StatusTypeDef HAL_TIMEx_Config睡M(TIM_HandleTypeDef *htim, uint32_t睡Msrc, uint32_t睡MEnMask);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(TIM_HandleTypeDef *htim, TIM_HallSensor_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim) __weak;
void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim) __weak;

/* Callback handling functions ********************************************/
void HAL_TIMEx_ComMutatiionCallback(TIM_HandleTypeDef *htim) __weak;
void HAL_TIMEx_ComMutatiionHalfCpltCallback(TIM_HandleTypeDef *htim) __weak;
void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim) __weak;
void HAL_TIMEx_Break2Callback(TIM_HandleTypeDef *htim) __weak;
/**
  * @}
  */

/**
  * @}
  */

/* Private macros --------------------------------------------------------*/
/** @defgroup TIM_Private_Macros TIM Private Macros
  * @{
  */
#define IS_TIM_CLOCKSOURCE(__CLOCK__) (((__CLOCK__) == TIM_CLOCKSOURCE_INTERNAL) || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_EXTI) || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_PXMI) || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_CLKDISABLED))
#define IS_TIM_CLOCKDIVISION_DIV(__DIV__) (((__DIV__) == TIM_CLOCKDIVISION_DIV1) || \
                                           ((__DIV__) == TIM_CLOCKDIVISION_DIV2) || \
                                           ((__DIV__) == TIM_CLOCKDIVISION_DIV4))
#define IS_TIM_COUNTER_MODE(__MODE__) (((__MODE__) == TIM_COUNTERMODE_UP)              || \
                                       ((__MODE__) == TIM_COUNTERMODE_DOWN)            || \
                                       ((__MODE__) == TIM_COUNTERMODE_CENTERALIGNED1)  || \
                                       ((__MODE__) == TIM_COUNTERMODE_CENTERALIGNED2)  || \
                                       ((__MODE__) == TIM_COUNTERMODE_CENTERALIGNED3))
#define IS_TIM_AUTORELOAD_PRELOAD(PRELOAD) (((PRELOAD) == TIM_AUTORELOAD_PRELOAD_DISABLE) || \
                                            ((PRELOAD) == TIM_AUTORELOAD_PRELOAD_ENABLE))
#define IS_TIM_FAST_STATE(__STATE__) (((__STATE__) == TIM_OCFast_DISABLE) || \
                                      ((__STATE__) == TIM_OCFast_ENABLE))
#define IS_TIM_OC_MODE(__MODE__) (((__MODE__) == TIM_OCMODE_TIMING)            || \
                                  ((__MODE__) == TIM_OCMODE_ACTIVE)            || \
                                  ((__MODE__) == TIM_OCMODE_INACTIVE)          || \
                                  ((__MODE__) == TIM_OCMODE_TOGGLE)            || \
                                  ((__MODE__) == TIM_OCMODE_PWM1)              || \
                                  ((__MODE__) == TIM_OCMODE_PWM2)              || \
                                  ((__MODE__) == TIM_OCMODE_FORCED_ACTIVE)     || \
                                  ((__MODE__) == TIM_OCMODE_FORCED_INACTIVE))
#define IS_TIM_OC_POLARITY(__POLARITY__) (((__POLARITY__) == TIM_OCPOLARITY_HIGH) || \
                                          ((__POLARITY__) == TIM_OCPOLARITY_LOW))
#define IS_TIM_IC_POLARITY(__POLARITY__) (((__POLARITY__) == TIM_ICPOLARITY_RISING) || \
                                          ((__POLARITY__) == TIM_ICPOLARITY_FALLING) || \
                                          ((__POLARITY__) == TIM_ICPOLARITY_BOTH_EDGE))
#define IS_TIM_IC_SELECTION(__SELECTION__) (((__SELECTION__) == TIM_ICSELECTION_DIRECTTI) || \
                                            ((__SELECTION__) == TIM_ICSELECTION_INDIRECTTI) || \
                                            ((__SELECTION__) == TIM_ICSELECTION_TRC))
#define IS_TIM_CLEARINPUT_SOURCE(__SOURCE__) (((__SOURCE__) == TIM_CLEARINPUT_DISABLE) || \
                                              ((__SOURCE__) == TIM_CLEARINPUTsource_ETR))
/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_TIM_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_TIM_H */
