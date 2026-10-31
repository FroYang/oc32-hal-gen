/**
 ******************************************************************************
 * @file    oc32_hal_ccp.h
 * @author
 * @brief   Header file of CCP HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_CCP_H
#define __OC32_HAL_CCP_H

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

  /** @addtogroup CCP
   * @{
   */

  /* Exported types ------------------------------------------------------*/

  /**
   * @brief  CCP mode enum definition
   */
  typedef enum
  {
    CCP_CAPTURE_MODE = 0x0U,
    CCP_COMPARE_MODE = 0x1U,
    CCP_PWM_EDGE_MODE = 0x2U,
    CCP_PWM_CENTER_MODE = 0x3U
  } HAL_CCP_ModeTypedef;

  /**
   * @brief  CCP capture condition enum definition
   */
  typedef enum
  {
    CCP_CAP_RISE = 0x0U,
    CCP_CAP_FALL = 0x1U,
    CCP_CAP_BOTH = 0x2U
  } HAL_CCP_CaptureConTypedef;

  /**
   * @brief  CCP capture input enum definition
   */
  typedef enum
  {
    CCP_CAP_PIN0 = 0x0U,
    CCP_CAP_PIN1 = 0x1U,
    CCP_CAP_PIN2 = 0x2U,
    CCP_CAP_PIN3 = 0x3U,
    CCP_CAP_PIN4 = 0x4U,
    CCP_CAP_CMP0 = 0x5U,
    CCP_CAP_CMP1 = 0x6U,
    CCP_CAP_PINS_XOR = 0x7U
  } HAL_CCP_CaptureInputTypedef;

  /**
   * @brief  CCP base timer enum definition
   */
  typedef enum
  {
    CCP_BASE_TIMER0 = 0U,
    CCP_BASE_TIMER1 = 1U,
    CCP_BASE_TIMER2 = 2U,
    CCP_BASE_TIMER3 = 3U,
    CCP_BASE_TIMER4 = 4U,
    CCP_BASE_TIMER5 = 5U,
    CCP_BASE_TIMER6 = 6U,
    CCP_BASE_TIMER7 = 7U
  } HAL_CCP_BaseTimerTypedef;

  /**
   * @brief  CCP update mode enum definition
   */
  typedef enum
  {
    CCP_UPDATE_DIR = 0U,
    CCP_UPDATE_TRIG = 1U,
    CCP_UPDATE_EVENT = 2U,
    CCP_UPDATE_LOCK = 3U
  } HAL_CCP_UpdateModeTypedef;

  /**
   * @brief  CCP force channel enum definition
   */
  typedef enum
  {
    CCP_FORCE_CH0 = 0x0U,
    CCP_FORCE_CH1 = 0x1U,
  } HAL_CCP_ForceChannelTypedef;

  /**
   * @brief  CCP force input enum definition
   */
  typedef enum
  {
    CCP_FORCE_IN_CMP0 = 0x0U,
    CCP_FORCE_IN_CMP1 = 0x1U,
    CCP_FORCE_IN_PIN3 = 0x2U,
    CCP_FORCE_IN_PIN4 = 0x3U
  } HAL_CCP_ForceInSourceTypedef;

  /**
   * @brief  CCP force condition enum definition
   */
  typedef enum
  {
    CCP_FORCE_IN_HI = 0U,
    CCP_FORCE_IN_LO = 1U
  } HAL_CCP_ForceConTypedef;

  /**
   * @brief  CCP force by input enum definition
   */
  typedef enum
  {
    CCP_FORCE_IN_OFF = 0U,
    CCP_FORCE_IN_ON = 1U
  } HAL_CCP_ForceInTypedef;

  /**
   * @brief  CCP force by Lvd enum definition
   */
  typedef enum
  {
    CCP_FORCE_LVD_OFF = 0U,
    CCP_FORCE_LVD_ON = 1U
  } HAL_CCP_ForceLvdTypedef;

  /**
   * @brief  CCP output enable enum definition
   */
  typedef enum
  {
    CCP_OUTPUT_OFF = 0U,
    CCP_OUTPUT_ON = 1U
  } HAL_CCP_OutputTypedef;

  /**
   * @brief  CCP force state enum definition
   */
  typedef enum
  {
    CCP_FORCE_LO = 0x0U,
    CCP_FORCE_HI = 0x1U
  } HAL_CCP_ForceStateTypedef;

  /**
   * @brief  CCP invert enum definition
   */
  typedef enum
  {
    CCP_OUTPUT_INV_OFF = 0x0U,
    CCP_OUTPUT_INV_ON = 0x1U,
  } HAL_CCP_OutputInvertTypedef;

  /**
   * @brief  CCP output control enum definition
   */
  typedef enum
  {
    CCP_OUTPUT_LOGIC = 0U,
    CCP_OUTPUT_CLR = 1U,
    CCP_OUTPUT_SET = 2U,
    CCP_OUTPUT_HOLD = 3U,
  } HAL_CCP_OutputConTypedef;

  /**
   * @brief  CCP gate mode enum definition
   */
  typedef enum
  {
    CCP_GATE_AND = 0U,
    CCP_GATE_OR = 1U,
    CCP_GATE_XOR = 2U,
    CCP_GATE_REPLACE = 3U
  } HAL_CCP_GateModeTypedef;

  /**
   * @brief  CCP gate invert enum definition
   */
  typedef enum
  {
    CCP_GATE_INV_OFF = 0x0U,
    CCP_GATE_INV_ON = 0x1U,
  } HAL_CCP_GateInvTypedef;

  /**
   * @brief  CCP gate enable enum definition
   */
  typedef enum
  {
    CCP_GATE_OFF = 0x0U,
    CCP_GATE_ON = 0x1U
  } HAL_CCP_GateTypedef;

  /**
   * @brief  CCP gate flag mode enum definition
   */
  typedef enum
  {
    CCP_GATE_FLAG_START = 0x0U,
    CCP_GATE_FLAG_END = 0x1U
  } HAL_CCP_GateFlagTypedef;

  /**
   * @brief  CCP pwm flag mode enum definition
   */
  typedef enum
  {
    CCP_PWM_FLAG_RISE = 0U,
    CCP_PWM_FLAG_FALL = 1U
  } HAL_CCP_PwmFlagTypedef;

  /**
   * @brief  CCP select buffer for read enum definition
   */
  typedef enum
  {
    CCP_READ_REG_DIR = 0,
    CCP_READ_REG_BUF = 1U
  } HAL_CCP_ReadRegTypedef;

  /**
   * @brief  CCP compare logic output state enum definition
   */
  typedef enum
  {
    CCP_COM_LOGIC_HI = 0U,
    CCP_COM_LOGIC_LO = 1U,
    CCP_COM_LOGIC_TOGGLE = 2U
  } HAL_CCP_CompareOutputTypedef;

  /**
   * @brief  CCP base config Init structure definition
   */
  typedef struct
  {
    MODULE_CCP_TypeDef *CCP;              /*!< Specifies the CCP */
    HAL_MODULE_INTTypedef IntE;           /*!< Specifies the interrupt mode */
    HAL_CCP_ModeTypedef Mode;             /*!< Specifies the ccp mode */
    HAL_CCP_BaseTimerTypedef Timer;       /*!< Specifies the base timer */
    HAL_CCP_UpdateModeTypedef UpdateMode; /*!< Specifies the update mode */
    HAL_CCP_ReadRegTypedef ReadReg;       /*!< Specifies the read way */
  } HAL_CCP_BaseInitTypeDef;

  /**
   * @brief  CCP capture config Init structure definition
   */
  typedef struct
  {
    MODULE_CCP_TypeDef *CCP;           /*!< Specifies the CCP */
    HAL_CCP_CaptureConTypedef CapCon;  /*!< Specifies the conditon */
    HAL_CCP_CaptureInputTypedef CapIn; /*!< Specifies the input */
    uint32_t Prescaler;                /*!< Specifies the input prescaler */
  } HAL_CCP_CaptureInitTypeDef;

  /**
   * @brief  CCP compare config Init structure definition
   */
  typedef struct
  {
    MODULE_CCP_TypeDef *CCP;               /*!< Specifies the CCP */
    uint32_t ComVal;                       /*!< Specifies the compare value */
    HAL_CCP_OutputConTypedef ComCon;       /*!< Specifies the conditon */
    HAL_CCP_CompareOutputTypedef ComOut;   /*!< Specifies the output */
    HAL_CCP_OutputTypedef PosOutE;         /*!< Specifies the positive output enable */
    HAL_CCP_OutputInvertTypedef PosOutInv; /*!< Specifies the positive output invert */
    HAL_CCP_OutputTypedef NegOutE;         /*!< Specifies the negative output enable */
    HAL_CCP_OutputInvertTypedef NegOutInv; /*!< Specifies the negative output invert */
  } HAL_CCP_CompareInitTypeDef;

  /**
   * @brief  CCP pwm config Init structure definition
   */
  typedef struct
  {
    MODULE_CCP_TypeDef *CCP;               /*!< Specifies the CCP */
    uint32_t Duty;                         /*!< Specifies the duty */
    uint32_t DeadTime;                     /*!< Specifies the deadtime */
    HAL_CCP_PwmFlagTypedef PwmCon;         /*!< Specifies the conditon */
    HAL_CCP_OutputTypedef PosOutE;         /*!< Specifies the positive output enable */
    HAL_CCP_OutputInvertTypedef PosOutInv; /*!< Specifies the positive output invert */
    HAL_CCP_OutputTypedef NegOutE;         /*!< Specifies the negative output enable */
    HAL_CCP_OutputInvertTypedef NegOutInv; /*!< Specifies the negative output invert */
  } HAL_CCP_PwmInitTypeDef;

  /**
   * @brief  CCP gate config Init structure definition
   */
  typedef struct
  {
    MODULE_CCP_TypeDef *CCP;            /*!< Specifies the CCP */
    uint32_t GateStart;                 /*!< Specifies the start value */
    uint32_t GateEnd;                   /*!< Specifies the end value */
    HAL_CCP_GateModeTypedef GateMode;   /*!< Specifies the gate mode */
    HAL_CCP_GateTypedef GateUpCountE;   /*!< Specifies the gate enable */
    HAL_CCP_GateTypedef GateDownCountE; /*!< Specifies the gate enable */
    HAL_CCP_GateInvTypedef GateInv;     /*!< Specifies the gate invert */
    HAL_CCP_GateFlagTypedef GateFlag;   /*!< Specifies the gate flag mode */
  } HAL_CCP_GateInitTypeDef;

  /**
   * @brief  CCP force config Init structure definition
   */
  typedef struct
  {
    MODULE_CCP_TypeDef *CCP;                    /*!< Specifies the CCP */
    HAL_CCP_ForceChannelTypedef ForceChannel;   /*!< Specifies the force channel */
    HAL_CCP_ForceInSourceTypedef ForceInSource; /*!< Specifies the force input source */
    HAL_CCP_ForceConTypedef ForceCon;           /*!< Specifies the force condition */
    HAL_CCP_ForceInTypedef ForceE;              /*!< Specifies the force enable */
    HAL_CCP_ForceLvdTypedef ForceLvdE;          /*!< Specifies the force lvd */
    HAL_CCP_ForceStateTypedef ForcePoState;     /*!< Specifies the positive output force state */
    HAL_CCP_ForceStateTypedef ForceNoState;     /*!< Specifies the negative output force state */
  } HAL_CCP_ForceInitTypeDef;

  /**
   * @brief  CCP update structure definition
   */
  typedef struct
  {
    MODULE_CCP_TypeDef *CCP; /*!< Specifies the CCP */
    uint32_t Duty;           /*!< Specifies the duty cycle */
    uint32_t GateStart;      /*!< Specifies the gate start value */
    uint32_t GateEnd;        /*!< Specifies the gate end value */
  } HAL_CCP_UpdateTypeDef;

  /* Exported constants --------------------------------------------------*/

  /* Exported macros -------------------------------------------------------*/

  /* Exported functions ----------------------------------------------------*/

#ifdef HAL_CCP_ENABLE
  static inline uint32_t HAL_CCP_CaptureValue(MODULE_CCP_TypeDef *CCP);
  static inline void HAL_CCP_Update(HAL_CCP_UpdateTypeDef *Update);

  HAL_StatusTypeDef HAL_CCP_Init(HAL_CCP_BaseInitTypeDef *Init);
  HAL_StatusTypeDef HAL_CCP_GateInit(HAL_CCP_GateInitTypeDef *Init);
  HAL_StatusTypeDef HAL_CCP_CaptureInit(HAL_CCP_CaptureInitTypeDef *Init);
  HAL_StatusTypeDef HAL_CCP_CompareInit(HAL_CCP_CompareInitTypeDef *Init);
  HAL_StatusTypeDef HAL_CCP_PwmInit(HAL_CCP_PwmInitTypeDef *Init);
  HAL_StatusTypeDef HAL_CCP_GateInit(HAL_CCP_GateInitTypeDef *Init);
  HAL_StatusTypeDef HAL_CCP_ForceInit(HAL_CCP_ForceInitTypeDef *Init);
  HAL_StatusTypeDef HAL_CCP_UpdateNow(HAL_CCP_UpdateTypeDef *Update);
  HAL_StatusTypeDef HAL_CCP_WaitCf(MODULE_CCP_TypeDef *CCP);
  HAL_StatusTypeDef HAL_CCP_WaitGf(MODULE_CCP_TypeDef *CCP);
  HAL_StatusTypeDef HAL_CCP_WaitFf(MODULE_CCP_TypeDef *CCP);
  HAL_StatusTypeDef HAL_CCP_DeInit(MODULE_CCP_TypeDef *CCP);

  __weak__ __sram__ HAL_StatusTypeDef HAL_CCP_UserIRQ(MODULE_CCP_TypeDef *CCP);
  __weak__ __sram__ HAL_StatusTypeDef HAL_CCP_IRQHandler(MODULE_CCP_TypeDef *CCP);
#endif /* HAL_CCP_ENABLE */

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_CCP_H */
