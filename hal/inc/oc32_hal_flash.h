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
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

    /** @addtogroup OC32_HAL_Driver
     * @{
     */

    /** @addtogroup FLASH
     * @{
     */

    /* Exported types ------------------------------------------------------*/

    /**
     * @brief CS Enable structure definition
     */
    typedef enum
    {
        FLASH_CS_ENABLE = 0x00U,
        FLASH_CS_DISABLE = 0x01U
    } FLASH_CSTypeDef;

    /**
     * @brief flash command structure definition
     */
    typedef enum
    {
        FLASH_WRITE_STATUS = 0x01U,
        FLASH_PROGRAM = 0x02U,
        FLASH_READ = 0x03U,
        FLASH_READ_STATUS = 0x05U,
        FLASH_WRITE_ENABLE = 0x06U,
        FLASH_SECTOR_ERASE = 0x20U,
        FLASH_NV_WRITE_ENABLE = 0x50U,
        FLASH_PAGE_ERASE = 0x81U,
        FLASH_QUAD_READ = 0xEBU
    } FLASH_CommandTypeDef;

    /**
     * @brief flash clock divider structure definition
     */
    typedef enum
    {
        FLASH_CLK_DIV2 = 0U,
        FLASH_CLK_DIV3 = 1U,
        FLASH_CLK_DIV4 = 2U,
        FLASH_CLK_DIV5 = 3U
    } FLASH_ClkDivTypeDef;

    /* flash timing: clock cycles latency between address -> data out
       user need to confirm the timing parameters base on flash spec
       - 1-line mode: 0 clock cycles
       - 2-line mode: 0 clock cycles
       - 4-line mode: 4 clock cycles */
    /**
     * @brief flash core-read clock latency cycles structure definition
     */
    typedef enum
    {
        FLASH_CORE_CLC_1_LINE = 0U,
        FLASH_CORE_CLC_2_LINE = 0U,
        FLASH_CORE_CLC_4_LINE = 4U
    } FLASH_CoreClcTypeDef;

    /* flash timing: clock cycles of command + address + mode
       - 1-line mode: 23 + 1 clock cycles
       - 2-line mode: 15 + 1 clock cycles
       - 4-line mode: 7 + 1 clock cycles */
    /**
     * @brief flash core-read command & addresscycles structure definition
     */
    typedef enum
    {
        FLASH_CORE_CAC_1_LINE = 23U,
        FLASH_CORE_CAC_2_LINE = 15U,
        FLASH_CORE_CAC_4_LINE = 7U
    } FLASH_CoreCacTypeDef;

    /* flash timing: CS deselect minimum time */
    /**
     * @brief flash core-read CS deselect minimum time structure definition
     */
    typedef enum
    {
        FLASH_CORE_TSHSL_LOW = 0U,
        FLASH_CORE_TSHSL_HIGH = 1U,
        FLASH_CORE_TSHSL_MAX = 15U
    } FLASH_CoreTshslTypeDef;

    /**
     * @brief flash core line mode structure definition
     */
    typedef enum
    {
        FLASH_CORE_1_LINE = 0U,
        FLASH_CORE_2_LINE = 1U,
        FLASH_CORE_4_LINE = 2U
    } FLASH_CoreLineTypeDef;

    /* user accessing data length */
    /**
     * @brief flash accessing data length structure definition
     */
    typedef enum
    {
        FLASH_USER_LENGTH_1B = 0U,
        FLASH_USER_LENGTH_2B = 1U,
        FLASH_USER_LENGTH_4B = 2U,
        FLASH_USER_LENGTH_16B = 3U
    } FLASH_UserLengthTypeDef;

    /* user accessing operation */
    /**
     * @brief flash accessing operation structure definition
     */
    typedef enum
    {
        FLASH_USER_OP_INST = 0U,
        FLASH_USER_OP_INST_ADR = 1U,
        FLASH_USER_OP_INST_RDATA = 2U,
        FLASH_USER_OP_INST_WDATA = 3U,
        FLASH_USER_OP_INST_ADR_RDATA = 4U,
        FLASH_USER_OP_INST_ADR_WDATA = 5U,
        FLASH_USER_OP_RDATA = 6U,
        FLASH_USER_OP_WDATA = 7U
    } FLASH_UserOpTypeDef;

    /**
     * @brief  Flash Data structure definition
     */
    typedef struct
    {
        uint32_t *Data;          /*!< Specifies Data buffer */
        uint32_t Address;        /*!< Specifies Data Address */
        uint32_t Length;         /*!< Specifies Data Length */
        HAL_DMA_ChTypedef DmaCh; /*!< Specifies DMA channel for transfer */
    } HAL_Flash_PacketTypeDef;

    /**
     * @brief  Flash Zone structure definition
     */
    typedef struct
    {
        uint32_t BankNum; /*!< Specifies Flash bank number */
        uint32_t ZoneNum; /*!< Specifies Zone number in a bank */
    } HAL_Flash_ZoneTypeDef;

/* Exported constants --------------------------------------------------*/

/** @defgroup FLASH_Exported_Constants FLASH Exported Constants
 * @{
 */

/** @defgroup FLASH_command FLASH command/data
 * @{
 */

/* flash register config */
#define FLASH_SET_STATUS2_QE ((uint32_t)0x00000200U)

/* flash spec */
#define FLASH_PAGE_SIZE ((uint32_t)256U)
#define FLASH_SECTOR_SIZE ((uint32_t)4096U)

/**
 * @}
 */

/** @defgroup FLASH_config FLASH config
 * @{
 */
#define FLASH_SCLK_HIGH ((uint32_t)50000000U)
#define FLASH_USER_READ_MAX ((uint32_t)1024U)

/**
 * @}
 */

/** @defgroup FLASH_Timeout FLASH Timeout
 * @{
 */
#define FLASH_TIMEOUT_VALUE ((uint32_t)500000U)
#define HAL_FLASH_VERIFY_RETRY 3U
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
    __sram__ HAL_StatusTypeDef HAL_FLASH_Read(HAL_Flash_PacketTypeDef *Packet);

    /* Program operation functions ********************************************/
    __sram__ HAL_StatusTypeDef HAL_FLASH_Program(HAL_Flash_PacketTypeDef *Packet);

    /* Erase operation ***************************************************/
    __sram__ HAL_StatusTypeDef HAL_FLASH_Erase(HAL_Flash_PacketTypeDef *Packet);

    /* Update CRC of a zone **************************************************/
    HAL_StatusTypeDef HAL_FLASH_UpdateCrc(HAL_Flash_ZoneTypeDef *Zone);

#endif /* HAL_FLASH_ENABLE */

#ifdef HAL_FLASH_VERIFY_ENABLE
    /* CRC verify and recovery **************************************************/
    __sram__ HAL_StatusTypeDef HAL_FLASH_VerifyCode(void);
#endif /* HAL_FLASH_VERIFY_ENABLE */

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
