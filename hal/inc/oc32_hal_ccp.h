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
#include "oc32_hal_ccp.h"

  /* extern ------------------------------------------------------------*/
  extern uint32_t DMAChState[OC32_DMA_CH_TOTAL];
  extern __sram__ HAL_StatusTypedef DmaUpdate(HAL_EnableTypedef DMAE, HAL_DMA_ChTypedef DmaCh, HAL_DMA_MapTypedef Module, HAL_DMA_StateTypedef State);

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
   * @brief  CCP force state enum definition
   */
  typedef enum
  {
    CCP_FORCE_LO = 0x0U,
    CCP_FORCE_HI = 0x1U
  } HAL_CCP_ForceStateTypedef;


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
    MODULE_CCP_Typedef *CCP;              /*!< Specifies the CCP */
    HAL_EnableTypedef IntE;               /*!< Specifies the interrupt mode */
    HAL_CCP_ModeTypedef Mode;             /*!< Specifies the ccp mode */
    HAL_CCP_BaseTimerTypedef Timer;       /*!< Specifies the base timer */
    HAL_CCP_UpdateModeTypedef UpdateMode; /*!< Specifies the update mode */
    HAL_CCP_ReadRegTypedef ReadReg;       /*!< Specifies the read way */
  } HAL_CCP_BaseInitTypedef;

  /**
   * @brief  CCP capture config Init structure definition
   */
  typedef struct
  {
    MODULE_CCP_Typedef *CCP;           /*!< Specifies the CCP */
    HAL_CCP_CaptureConTypedef CapCon;  /*!< Specifies the condition */
    HAL_CCP_CaptureInputTypedef CapIn; /*!< Specifies the input */
    uint32_t Prescaler;                /*!< Specifies the input prescaler */
  } HAL_CCP_CaptureInitTypedef;

  /**
   * @brief  CCP compare config Init structure definition
   */
  typedef struct
  {
    MODULE_CCP_Typedef *CCP;               /*!< Specifies the CCP */
    uint32_t ComVal;                       /*!< Specifies the compare value */
    HAL_CCP_OutputConTypedef ComCon;       /*!< Specifies the condition */
    HAL_CCP_CompareOutputTypedef ComOut;   /*!< Specifies the output */
    HAL_EnableTypedef PosOutE;         /*!< Specifies the positive output enable */
    HAL_EnableTypedef PosOutInv; /*!< Specifies the positive output invert */
    HAL_EnableTypedef NegOutE;         /*!< Specifies the negative output enable */
    HAL_EnableTypedef NegOutInv; /*!< Specifies the negative output invert */
  } HAL_CCP_CompareInitTypedef;

  /**
   * @brief  CCP pwm config Init structure definition
   */
  typedef struct
  {
    MODULE_CCP_Typedef *CCP;               /*!< Specifies the CCP */
    uint32_t Duty;                         /*!< Specifies the duty */
    uint32_t DeadTime;                     /*!< Specifies the deadtime */
    HAL_CCP_PwmFlagTypedef PwmCon;         /*!< Specifies the condition */
    HAL_EnableTypedef PosOutE;         /*!< Specifies the positive output enable */
    HAL_EnableTypedef PosOutInv; /*!< Specifies the positive output invert */
    HAL_EnableTypedef NegOutE;         /*!< Specifies the negative output enable */
    HAL_EnableTypedef NegOutInv; /*!< Specifies the negative output invert */
  } HAL_CCP_PwmInitTypedef;

  /**
   * @brief  CCP gate config Init structure definition
   */
  typedef struct
  {
    MODULE_CCP_Typedef *CCP;            /*!< Specifies the CCP */
    uint32_t GateStart;                 /*!< Specifies the start value */
    uint32_t GateEnd;                   /*!< Specifies the end value */
    HAL_CCP_GateModeTypedef GateMode;   /*!< Specifies the gate mode */
    HAL_EnableTypedef GateUpCountE;   /*!< Specifies the gate enable */
    HAL_EnableTypedef GateDownCountE; /*!< Specifies the gate enable */
    HAL_EnableTypedef GateInv;     /*!< Specifies the gate invert */
    HAL_CCP_GateFlagTypedef GateFlag;   /*!< Specifies the gate flag mode */
  } HAL_CCP_GateInitTypedef;

  /**
   * @brief  CCP force config Init structure definition
   */
  typedef struct
  {
    MODULE_CCP_Typedef *CCP;                    /*!< Specifies the CCP */
    HAL_CCP_ForceChannelTypedef ForceChannel;   /*!< Specifies the force channel */
    HAL_CCP_ForceInSourceTypedef ForceInSource; /*!< Specifies the force input source */
    HAL_CCP_ForceConTypedef ForceCon;           /*!< Specifies the force condition */
    HAL_EnableTypedef ForceE;              /*!< Specifies the force enable */
    HAL_EnableTypedef ForceLvdE;          /*!< Specifies the force lvd enable */
    HAL_CCP_ForceStateTypedef ForcePoState;     /*!< Specifies the positive output force state */
    HAL_CCP_ForceStateTypedef ForceNoState;     /*!< Specifies the negative output force state */
  } HAL_CCP_ForceInitTypedef;

  /**
   * @brief  CCP update structure definition
   */
  typedef struct
  {
    MODULE_CCP_Typedef *CCP; /*!< Specifies the CCP */
    uint32_t Duty;           /*!< Specifies the duty cycle */
    uint32_t GateStart;      /*!< Specifies the gate start value */
    uint32_t GateEnd;        /*!< Specifies the gate end value */
  } HAL_CCP_UpdateTypedef;

  /* Exported constants --------------------------------------------------*/

  /* Exported macros -------------------------------------------------------*/

  /* Exported functions ----------------------------------------------------*/

#ifdef HAL_CCP_ENABLE
  HAL_StatusTypedef HAL_CCP_Init(HAL_CCP_BaseInitTypedef *Init);
  HAL_StatusTypedef HAL_CCP_CaptureInit(HAL_CCP_CaptureInitTypedef *Init);
  HAL_StatusTypedef HAL_CCP_CompareInit(HAL_CCP_CompareInitTypedef *Init);
  HAL_StatusTypedef HAL_CCP_PwmInit(HAL_CCP_PwmInitTypedef *Init);
  HAL_StatusTypedef HAL_CCP_GateInit(HAL_CCP_GateInitTypedef *Init);
  HAL_StatusTypedef HAL_CCP_ForceInit(HAL_CCP_ForceInitTypedef *Init);
  __weak__ __sram__ HAL_StatusTypedef HAL_CCP_UpdateNow(HAL_CCP_UpdateTypedef *Update);
  __weak__ __sram__ HAL_StatusTypedef HAL_CCP_HandleCov(MODULE_CCP_Typedef *CCP);
  __weak__ __sram__ HAL_StatusTypedef HAL_CCP_WaitCf(MODULE_CCP_Typedef *CCP);
  __weak__ __sram__ HAL_StatusTypedef HAL_CCP_WaitGf(MODULE_CCP_Typedef *CCP);
  __weak__ __sram__ HAL_StatusTypedef HAL_CCP_WaitFf(MODULE_CCP_Typedef *CCP);
  __weak__ __sram__ HAL_StatusTypedef HAL_CCP_UserIRQ(MODULE_CCP_Typedef *CCP);
  __sram__ HAL_StatusTypedef HAL_CCP_IRQHandler(MODULE_CCP_Typedef *CCP);

  /**
   * @brief  Returns the CCP capture value.
   * @param  CCP: pointer to a MODULE_CCP_typedef structure.
   * @retval Capture value from CCPCV
   * @note   Internal helper.
   */
  static inline uint32_t HAL_CCP_CaptureValue(MODULE_CCP_Typedef *CCP)
  {
    /* setup reload value */
    return CCP->CCPVAL.CCPCV;
  }

  /**
   * @brief  Updates the CCP duty and gate values for a given CCP.
   * @param  Update: pointer to a HAL_CCP_Updatetypedef structure.
   * @note   Internal helper; writes CCPCV (duty) and CCPGAT (gate start/end).
   */
  static inline void HAL_CCP_Update(HAL_CCP_UpdateTypedef *Update)
  {
    /* setup reload value */
    Update->CCP->CCPVAL.CCPCV = (Update->Duty & 0xFFFFU);
    Update->CCP->CCPGAT.CCPGSV = (Update->GateStart & 0xFFFFU);
    Update->CCP->CCPGAT.CCPGEV = (Update->GateEnd & 0xFFFFU);
    return;
  }

#endif /* HAL_CCP_ENABLE */

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_CCP_H */
