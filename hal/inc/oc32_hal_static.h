/**
 ******************************************************************************
 * @file    oc32_hal_static.h
 * @author
 * @brief   Header file of OC32 HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_STATIC_H
#define __OC32_HAL_STATIC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

  /**
   * @brief  SYSC syscall number definition
   */
  typedef enum
  {
    SYSC_PID = 0u,      /* fused Product ID (DEVID/REVID) */
    SYSC_LID = 1u,      /* fused Lot ID */
    SYSC_TSN = 2u,      /* fused Test Serial Number (96-bit UID) */
    SYSC_REGTRIM0 = 3u, /* fused reg: BPCON/IOSCCON/LVDCON/VTSCON */
    SYSC_REGTRIM1 = 4u, /* fused reg:  */
    SYSC_REGTRIM2 = 5u, /* fused reg:  */
    SYSC_REGTRIM3 = 6u, /* fused reg:  */
    SYSC_VTTABLE = 7u   /* fused reg: VTTABLE */
  } SYSC_SyscallNumTypedef;

  /**
   * @brief  Trigger SYSC instruction to read fused Product ID into SysReqBuf.
   */
  static inline void HAL_SYS_GetPid(void)
  {
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_PID) : "memory");
  }

  /**
   * @brief  Trigger SYSC instruction to read fused Test Serial Number into SysReqBuf.
   */
  static inline void HAL_SYS_GetTsn(void)
  {
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_TSN) : "memory");
  }

  /**
   * @brief  Trigger SYSC instruction to read fused Register Trim Value into SysReqBuf.
   */
  static inline void HAL_SYS_GetTrim0(void)
  {
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_REGTRIM0) : "memory");
  }

  /**
   * @brief  Trigger SYSC instruction to read fused Register Trim Value into SysReqBuf.
   */
  static inline void HAL_SYS_GetTrim1(void)
  {
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_REGTRIM1) : "memory");
  }

  /**
   * @brief  Trigger SYSC instruction to read fused Register Trim Value into SysReqBuf.
   */
  static inline void HAL_SYS_GetTrim2(void)
  {
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_REGTRIM2) : "memory");
  }

  /**
   * @brief  Trigger SYSC instruction to read fused Register Trim Value into SysReqBuf.
   */
  static inline void HAL_SYS_GetTrim3(void)
  {
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_REGTRIM3) : "memory");
  }

  /**
   * @brief  Trigger SYSC instruction to read fused VTTABLE into SysReqBuf.
   */
  static inline void HAL_SYS_GetVtTable(void)
  {
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_VTTABLE) : "memory");
  }


#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_STATIC_H */
