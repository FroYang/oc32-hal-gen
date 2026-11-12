/**
 ******************************************************************************
 * @file    oc32_hal_def.h
 * @author
 * @brief   Header file of OC32 HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_DEF_H
#define __OC32_HAL_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

  /* Exported types ------------------------------------------------------*/
  /**
   * @brief HAL Enable structure definition
   */
  typedef enum
  {
    HAL_DISABLE = 0U,
    HAL_ENABLE = 1U
  } HAL_EnableTypedef;

  /**
   * @brief  Flag error/valid type definitions.
   *
   * Two separate 2-value enums are used deliberately to keep register
   * semantics readable at the call site:
   *  - HAL_FlagErrorTypedef   (GOOD=0 / ERROR=1) is for error-type
   *    status flags (e.g. IICHNACK, IICCLTO, HRF->PORF):
   *    `X == HAL_FLAG_ERROR` reads as "error condition present".
   *  - HAL_FlagValidTypedef   (INVALID=0 / VALID=1) is for operation
   *    done/busy flags (e.g. IICTBV, IICRBV, CRCCF, FLUOPF):
   *    `X == HAL_FLAG_INVALID` reads as "not done yet".
   *
   * Both enums have value 0 as the first member, but they are intended
   * to be used in their respective contexts only — do not mix them.
   */
  typedef enum
  {
    HAL_FLAG_GOOD = 0U,
    HAL_FLAG_ERROR = 1U
  } HAL_FlagErrorTypedef;

  /**
   * @brief  Flag valid/invalid type definition (operation done/busy flags).
   */
  typedef enum
  {
    HAL_FLAG_INVALID = 0U,
    HAL_FLAG_VALID = 1U
  } HAL_FlagValidTypedef;

  /**
   * @brief HAL Status structure definition
   */
  typedef enum
  {
    HAL_OK = 0U,
    HAL_ERROR = 1U,
    HAL_BUSY = 2U,
    HAL_TIMEOUT = 3U
  } HAL_StatusTypedef;

  typedef struct
  {
    uint32_t Data;           /*!< Specifies Data buffer */
    HAL_StatusTypedef Error; /*!< Specifies the status */
  } HAL_ResultTypedef;

/* Exported macros -----------------------------------------------------*/

/** @defgroup HAL_Private_Macros HAL Private Macros
 * @{
 */
#define UNUSED(X) (void)X /* Avoid compiler warnings for unused variables */

/**
 * @}
 */

/** @defgroup HAL_INT_Priority_Macros HAL Private Macros
 * @{
 */
#define HPL_Bit0(pri, ch) ((pri & 0x1) << ch)        /* get int priority config bit 0 */
#define HPL_Bit1(pri, ch) (((pri & 0x2) >> 1) << ch) /* get int priority config bit 1 */

/**
 * @}
 */

/** @defgroup HAL_Private_Attributes HAL Private Attributes
 * @{
 */
#define __weak__ __attribute__((weak))
#define __sram__ __attribute__((section(".stext"))) /*!< Place it in SRAM, stext(sram text) is OC32 specific */

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_DEF_H */
