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
extern "C" {
#endif

/* Includes ------------------------------------------------------------*/
#include "CV32S6015MSR.h"

/* Exported types ------------------------------------------------------*/

/**
  * @brief HAL Lock structure definition
  */
typedef enum {
    HAL_UNLOCKED = 0x00U,
    HAL_LOCKED   = 0x01U
} HAL_LockTypeDef;

/**
  * @brief HAL Status structure definition
  */
typedef enum {
    HAL_OK  = 0x00U,
    HAL_ERROR = 0x01U,
    HAL_BUSY = 0x02U,
    HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;


/* Exported macros -----------------------------------------------------*/

/** @defgroup HAL_Private_Macros HAL Private Macros
  * @{
  */
#define UNUSED(X) (void)X      /* Avoid compiler warnings for unused variables */

/**
  * @}
  */


/** @defgroup HAL_Private_Attributes HAL Private Attributes
  * @{
  */
#define __weak__     __attribute__((weak))
#define __sram__     __attribute__((section(".stext")))

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_DEF_H */
