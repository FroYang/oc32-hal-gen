/**
 ******************************************************************************
 * @file    oc32_hal_flash.h
 * @author
 * @brief   Header file of FLASH HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_FLASH_H
#define __OC32_HAL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"
#include "CV32S6015MSR.h"

/** @addtogroup OC32_HAL_Driver
 * @{
 */

/** @addtogroup FLASH
 * @{
 */

/* Exported types ------------------------------------------------------*/

/* Exported constants --------------------------------------------------*/

/** @defgroup FLASH_Exported_Constants FLASH Exported Constants
 * @{
 */

/** @defgroup FLASH_command FLASH command/data
 * @{
 */
/* flash command */
#define FLASH_WRITE_STATUS    ((uint32_t)0x01U)
#define FLASH_PROGRAM         ((uint32_t)0x02U)
#define FLASH_READ            ((uint32_t)0x03U)
#define FLASH_READ_STATUS     ((uint32_t)0x05U)
#define FLASH_WRITE_ENABLE    ((uint32_t)0x06U)
#define FLASH_SECTOR_ERASE    ((uint32_t)0x20U)
#define FLASH_NV_WRITE_ENABLE ((uint32_t)0x50U)
#define FLASH_PAGE_ERASE      ((uint32_t)0x81U)
#define FLASH_QUAD_READ       ((uint32_t)0xEBU)

/* flash register config */
#define FLASH_SET_STATUS2_QE ((uint32_t)0x00000200U)

/* flash spec */
#define FLASH_PAGE_SIZE       ((uint32_t)256U)
#define FLASH_SECTOR_SIZE     ((uint32_t)4096U)


/**
 * @}
 */

/** @defgroup FLASH_config FLASH config
 * @{
 */
#define FLASH_SCLK_HIGH ((uint32_t)50000000U)
#define FLASH_CLK_DIV2  ((uint32_t)0U)
#define FLASH_CLK_DIV3  ((uint32_t)1U)
#define FLASH_CLK_DIV4  ((uint32_t)2U)
#define FLASH_CLK_DIV5  ((uint32_t)3U)

/* config for core accessing */
#define FLASH_CORE_1_LINE          ((uint32_t)0U)
#define FLASH_CORE_2_LINE          ((uint32_t)1U)
#define FLASH_CORE_4_LINE          ((uint32_t)2U)
#define FLASH_CORE_CLC_1_LINE      ((uint32_t)0U)
#define FLASH_CORE_CLC_2_LINE      ((uint32_t)0U)
#define FLASH_CORE_CLC_4_LINE      ((uint32_t)4U)
#define FLASH_CORE_CAC_1_LINE      ((uint32_t)23U)
#define FLASH_CORE_CAC_2_LINE      ((uint32_t)15U)
#define FLASH_CORE_CAC_4_LINE      ((uint32_t)7U)
#define FLASH_CORE_TSHSL_SCLK_LOW  ((uint32_t)0U)
#define FLASH_CORE_TSHSL_SCLK_HIGH ((uint32_t)1U)
#define FLASH_CORE_TSHSL_MAX       ((uint32_t)15U)

/* config for user accessing */
#define FLASH_USER_LENGTH_1B         ((uint32_t)0U)
#define FLASH_USER_LENGTH_2B         ((uint32_t)1U)
#define FLASH_USER_LENGTH_4B         ((uint32_t)2U)
#define FLASH_USER_LENGTH_16B        ((uint32_t)3U)
#define FLASH_USER_LENGTH_64B        ((uint32_t)4U)

#define FLASH_USER_OP_INST           ((uint32_t)0U)
#define FLASH_USER_OP_INST_ADR       ((uint32_t)1U)
#define FLASH_USER_OP_INST_RDATA     ((uint32_t)2U)
#define FLASH_USER_OP_INST_WDATA     ((uint32_t)3U)
#define FLASH_USER_OP_INST_ADR_RDATA ((uint32_t)4U)
#define FLASH_USER_OP_INST_ADR_WDATA ((uint32_t)5U)
#define FLASH_USER_OP_RDATA          ((uint32_t)6U)
#define FLASH_USER_OP_WDATA          ((uint32_t)7U)

#define FLASH_USER_READ_MAX          ((uint32_t)512U)

/**
 * @}
 */

/** @defgroup FLASH_Timeout FLASH Timeout
 * @{
 */
#define FLASH_TIMEOUT_VALUE ((uint32_t)500000U)
/**
 * @}
 */


/**
 * @}
 */

/* Exported macros -------------------------------------------------------*/

/* Exported functions ----------------------------------------------------*/

/** @addtogroup FLASH_Exported_Functions
 * @{
 */

/* Flash clock update functions *************************/
__sram__ HAL_StatusTypeDef HAL_FLASH_ClkUpdate(void);

#ifdef HAL_FLASH_ENABLE
/* Flash quad read for core functions *************************/
__sram__ HAL_StatusTypeDef HAL_FLASH_CPUQuadRead(void);

/* Initialization and de-initialization functions *************************/
__sram__ HAL_StatusTypeDef HAL_FLASH_Init(void);

/* Read operation functions ********************************************/
__sram__ HAL_StatusTypeDef HAL_FLASH_Read(uint32_t *Data, uint32_t Address, uint32_t Length);
__sram__ HAL_StatusTypeDef HAL_FLASH_ReadLines(uint32_t *Data, uint32_t Address, uint32_t Lines, HAL_DMAChTypedef DMA);

/* Program operation functions ********************************************/
__sram__ HAL_StatusTypeDef HAL_FLASH_Program(uint32_t *Data, uint32_t Address, uint32_t Length);

/* Page Erase operation ***************************************************/
__sram__ HAL_StatusTypeDef HAL_FLASH_Erase(uint32_t Address, uint32_t Length);

/* Update CRC of a zone **************************************************/
__sram__ HAL_StatusTypeDef HAL_FLASH_UpdateCrc(uint32_t Bank, uint32_t Zone);

#endif

#ifdef HAL_FLASH_VERIFY_ENABLE
/* CRC verify and recovery **************************************************/
__sram__ HAL_StatusTypeDef HAL_FLASH_VerifyCode(void);
#endif

/**
 * @}
 */

/* Private macros --------------------------------------------------------*/

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_FLASH_H */
