/**
 ******************************************************************************
 * @file    oc32_hal_flash.c
 * @author
 * @brief   OC32 HAL FLASH module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal.h"

/* always need this function for rcm clock config */
__sram__ HAL_StatusTypeDef HAL_FLASH_ClkUpdate(void)
{
    /* clock divide: flash clock < 25MHz */
    if (SystemClock > FLASH_SCLK_HIGH)
    {
        /* sclk/4 */
        FLCCON->FLCC = FLASH_CLK_DIV4;
        FLUCON->FLUC = FLASH_CLK_DIV4;
        FLCCON->FLTSHSL = FLASH_CORE_TSHSL_SCLK_HIGH;
    }
    else
    {
        /* sclk/2 */
        FLCCON->FLCC = FLASH_CLK_DIV2;
        FLUCON->FLUC = FLASH_CLK_DIV2;
        FLCCON->FLTSHSL = FLASH_CORE_TSHSL_SCLK_LOW;
    }

    return HAL_OK;
}

#ifdef HAL_FLASH_ENABLE

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Kickstart flashloader operation
 * @retval HAL_OK on success, HAL_ERROR on timeout
 */
__sram__ HAL_StatusTypeDef FlashKick(void)
{
    uint32_t timeout;

    FLUCON->FLUKS = 1;
    timeout = FLASH_TIMEOUT_VALUE;
    while ((FLUCON->FLUOPF == 0) && (timeout-- > 0))
        ;
    FLUCON->FLUOCLR = 1;
    if (timeout == 0u)
        return HAL_ERROR;
    return HAL_OK;
}

/**
 * @brief  set SPI flash Write Enable
 * @retval HAL_OK on success, HAL_ERROR on timeout
 */
__sram__ HAL_StatusTypeDef FlashWREN(void)
{
    /* write enable command */
    FLUCON->FLUI = FLASH_WRITE_ENABLE;
    return FlashKick();
}

/**
 * @brief  Read lines of data(4 word per line) from the specified address.
 * @param  Packet: pointer to HAL_FlashPacketTypeDef structure that contains
 *         the configuration information for data packet.
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef FlashReadLines(HAL_FlashPacketTypeDef *Packet)
{
    /* translate length to the configure value (power of 2) */
    uint32_t n = 0;
    while (((Packet->Length / 16U) >> n) > 1U)
        n++;

    WRITE_SR(FLUADR, Packet->Address);
    FLUCON->FLUDC = FLASH_USER_LENGTH_16B;

    /* using core-accessing config, no need to config at user side*/
    FLUCON->FLURUCS = 1;

    /* setup DMA Channel */
    switch (Packet->DMA)
    {
    case OC32_DMA_CH0:
        DMACON->DMACH0 = OC32_DMA_FLASH;
        break;
    case OC32_DMA_CH1:
        DMACON->DMACH1 = OC32_DMA_FLASH;
        break;
    case OC32_DMA_CH2:
        DMACON->DMACH2 = OC32_DMA_FLASH;
        break;
    case OC32_DMA_CH3:
        DMACON->DMACH3 = OC32_DMA_FLASH;
        break;
    default:
        goto error;
    }
    /* DMA mode */
    FLUCON->FLURRE = 1;
    /* setup lines */
    FLUCON->FLURRC = n - 1;
    /* setup DMA address */
    WRITE_SR(FLURRDA, (uint32_t)Packet->Data);

    if (FlashKick() != HAL_OK)
        goto error;

    FLUCON->FLURUCS = 0;
    FLUCON->FLURRE = 0;
    return HAL_OK;

error:
    FLUCON->FLURUCS = 0;
    FLUCON->FLURRE = 0;
    return HAL_ERROR;
}

/**
 * @brief  setup SPI flash Quad-Read mode for CPU instruction read
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_CPUQuadRead(void)
{
    /* Write Enable for Non-volatile bit(QE in SR2) */
    FLUCON->FLUI = FLASH_NV_WRITE_ENABLE;
    if (FlashKick() != HAL_OK)
        return HAL_ERROR;

    /* Write Status Register2: QE=1*/
    WRITE_SR(FLUDATA0, FLASH_SET_STATUS2_QE);

    /* write status command */
    FLUCON->FLUI = FLASH_WRITE_STATUS;
    /* write 2 bytes: status 1 & status 2*/
    FLUCON->FLUDC = FLASH_USER_LENGTH_2B;
    /* operation type: inst+wr_data */
    FLUCON->FLUT = FLASH_USER_OP_INST_WDATA;

    if (FlashKick() != HAL_OK)
        return HAL_ERROR;

    /* setup Quad Read command for core read */
    FLCCON->FLCI = FLASH_QUAD_READ;
    /* setup data line for Quad Read */
    FLCCON->FLCDL = FLASH_CORE_4_LINE;
    /* setup read latency for Quad Read */
    FLCCON->FLCLC = FLASH_CORE_CLC_4_LINE;
    /* setup address+mode cycles */
    FLCCON->FLCAC = FLASH_CORE_CAC_4_LINE;

    return HAL_OK;
}

/**
 * @brief  Initializes the FLASH peripheral.
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_Init(void)
{

    /* standard read command */
    FLCCON->FLCI = FLASH_READ;
    FLUCON->FLUI = FLASH_READ;

    HAL_FLASH_ClkUpdate();

    /* (core accessing) read latency */
    FLCCON->FLCLC = FLASH_CORE_CLC_1_LINE;

    /* (core accessing) address+mode cycles */
    FLCCON->FLCAC = FLASH_CORE_CAC_1_LINE;

    /* (user accessing) default 32-bit data length */
    FLUCON->FLUDC = FLASH_USER_LENGTH_4B;

    /* (user accessing) default read operation */
    FLUCON->FLUT = FLASH_USER_OP_INST_ADR_RDATA;

    /* (user accessing) default config */
    WRITE_SR(FLUCON, READ_SR(FLUCON) & 0x0000FFFFU);

    return HAL_OK;
}

/**
 * @brief  Read data from the specified address.
 * @param  Packet: pointer to HAL_FlashPacketTypeDef structure that contains
 *         the configuration information for data packet.
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_Read(HAL_FlashPacketTypeDef *Packet)
{
    if (Packet == NULL || Packet->Data == NULL)
        return HAL_ERROR;
    if (Packet->Length == 0U)
        return HAL_ERROR;
    if ((Packet->Address + (Packet->Length << 2)) > OC32_FLASH_SIZE)
        return HAL_ERROR;

    /* DMA condition check
     * 1. length is multiple of lines
     * 2. buffer address is word aligned
     * 3. lines is 2/4/8/16/32/64/128/256 */
    if (((Packet->Length % 16U) != 0) || (((uintptr_t)Packet->Data & 0x3U) != 0))
        goto normal_read;

    uint32_t Lines = Packet->Length / 16U;
    if ((Lines < 2U) || ((Lines & (Lines - 1U)) != 0U) || (Lines > 256U))
        goto normal_read;

dma_read:
    if (FlashReadLines(Packet) != HAL_OK)
        return HAL_ERROR;
    else
        return HAL_OK;

normal_read:
    if (Packet->Length > FLASH_USER_READ_MAX)
        return HAL_ERROR;

    /* send flash cmd + address */
    WRITE_SR(FLUADR, Packet->Address);
    FLUCON->FLUI = FLASH_READ;
    FLUCON->FLUDC = FLASH_USER_LENGTH_4B;
    FLUCON->FLUT = FLASH_USER_OP_INST_ADR;

    /* drive CS low */
    FLUCON->FLUSS = 0;
    if (FlashKick() != HAL_OK)
        goto error;

    /* receive flash data */
    FLUCON->FLUT = FLASH_USER_OP_RDATA;
    uint32_t i;
    for (i = 0; i < Packet->Length; i++)
    {
        if (FlashKick() != HAL_OK)
            goto error;
        Packet->Data[i] = READ_SR(FLUDATA0);
    }

    /* drive CS high */
    FLUCON->FLUSS = 1;
    return HAL_OK;

error:
    FLUCON->FLUSS = 1;
    return HAL_ERROR;
}

/**
 * @brief  Program word at the specified address.
 * @param  Packet: pointer to HAL_FlashPacketTypeDef structure that contains
 *         the configuration information for data packet.
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_Program(HAL_FlashPacketTypeDef *Packet)
{
    if (Packet == NULL || Packet->Data == NULL)
        return HAL_ERROR;

    /* check whether all of data locate in a same page */
    if (((Packet->Address + (Packet->Length << 2)) & 0xFFFFFF00U) != (Packet->Address & 0xFFFFFF00U))
        return HAL_ERROR;

    /* write enable */
    if (FlashWREN() != HAL_OK)
        return HAL_ERROR;

    /* send flash cmd + address */
    WRITE_SR(FLUADR, Packet->Address);
    FLUCON->FLUI = FLASH_PROGRAM;
    FLUCON->FLUDC = FLASH_USER_LENGTH_4B;
    FLUCON->FLUT = FLASH_USER_OP_INST_ADR;

    /* drive CS low */
    FLUCON->FLUSS = 0;
    if (FlashKick() != HAL_OK)
        goto error;

    /* send flash data */
    FLUCON->FLUT = FLASH_USER_OP_WDATA;
    uint32_t i;
    for (i = 0; i < Packet->Length; i++)
    {
        WRITE_SR(FLUDATA0, Packet->Data[i]);
        if (FlashKick() != HAL_OK)
            goto error;
    }

    /* drive CS high */
    FLUCON->FLUSS = 1;

    /* wait busy */
    FLUCON->FLUI = FLASH_READ_STATUS;
    FLUCON->FLUDC = FLASH_USER_LENGTH_1B;
    FLUCON->FLUT = FLASH_USER_OP_INST_RDATA;
    FLUCON->FLUWIPC = 1u;

    if (FlashKick() != HAL_OK)
        goto error;

    return HAL_OK;

error:
    FLUCON->FLUSS = 1;
    return HAL_ERROR;
}

/**
 * @brief  Erase the specified address range.
 * @param  Packet: pointer to HAL_FlashPacketTypeDef structure that contains
 *         the configuration information for data packet.
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_Erase(HAL_FlashPacketTypeDef *Packet)
{
    if (Packet == NULL)
        return HAL_ERROR;

    uint32_t end_addr;
    uint32_t sector_start;

    /* address and length must be page-aligned (256 bytes) */
    if ((Packet->Address & (FLASH_PAGE_SIZE - 1U)) != 0U)
        return HAL_ERROR;
    if ((Packet->Length & (FLASH_PAGE_SIZE - 1U)) != 0U)
        return HAL_ERROR;
    if (Packet->Length == 0U)
        return HAL_ERROR;
    if ((Packet->Address + Packet->Length) > OC32_FLASH_SIZE)
        return HAL_ERROR;

    end_addr = Packet->Address + Packet->Length;

    while (Packet->Address < end_addr)
    {
        /* sector erase: only when the remaining range covers a complete sector */
        sector_start = Packet->Address & ~(FLASH_SECTOR_SIZE - 1U);
        if (FlashWREN() != HAL_OK)
            return HAL_ERROR;
        WRITE_SR(FLUADR, Packet->Address);
        FLUCON->FLUT = FLASH_USER_OP_INST_ADR;
        FLUCON->FLUWIPC = 1u;
        if ((Packet->Address == sector_start) && (end_addr >= (sector_start + FLASH_SECTOR_SIZE)))
        {
            FLUCON->FLUI = FLASH_SECTOR_ERASE;
            if (FlashKick() != HAL_OK)
                return HAL_ERROR;
            Packet->Address += FLASH_SECTOR_SIZE;
        }
        else
        {
            FLUCON->FLUI = FLASH_PAGE_ERASE;
            if (FlashKick() != HAL_OK)
                return HAL_ERROR;
            Packet->Address += FLASH_PAGE_SIZE;
        }
    }
    return HAL_OK;
}

/**
 * @brief  CRC init for flash
 */
void FlashCrcInit(void)
{
    HAL_CRCInitTypeDef crc_init = {
        .Poly = CRC_POLY_DEFAULT,
        .InitVal = 0xFFFFFFFFU,
        .Bits = CRC_32_BIT,
        .RefIn = 1U,
        .RefOut = 1U,
        .CplOut = 1U,
        .Rvb = 0,
        .Mode = CRC_MODE_NONE,
        .IntPri = 0,
    };
    HAL_CRC_Init(&crc_init);
}

/**
 * @brief  Read a flash zone and update the crc
 * @param  Zone: pointer to HAL_FlashZoneTypeDef structure that contains
 *         the configuration information for zone.
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_UpdateCrc(HAL_FlashZoneTypeDef *Zone)
{
    if (Zone == NULL)
        return HAL_ERROR;

    uint32_t i;

    /* DMA buffer for flash reads */
    static uint32_t flashbuf[256] __attribute__((aligned(4)));

    /* data start address */
    uint32_t Address = (Zone->BankNum == 0) ? 0 : OC32_CODE_BANK_SIZE;
    Address = (Zone->ZoneNum == 0) ? Address : Address + OC32_CODE_ZONE0_SIZE;

    /* data size */
    uint32_t Size = (Zone->ZoneNum == 0) ? OC32_CODE_ZONE0_SIZE : OC32_CODE_ZONE1_SIZE;
    /* convert to kb */
    Size = Size >> 10U;

    FlashCrcInit();

    HAL_CRCPacketTypedef crc_packet = {
        .Data = flashbuf,
        .Length = 1024U,
        .Mode = CRC_IDM_CAPTURE_DMAIN,
        .DMA = OC32_DMA_CH0,
    };

    uint32_t crc32;
    for (i = 0; i < Size; i++)
    {
        /* exclude the word of crc32 data */
        if (i == (Size - 1))
        {
            crc_packet = {
                .Length = 1020U,
            };
        }

        if (HAL_CRC_Packet(&crc_packet) != HAL_OK)
        {
            HAL_CRC_DeInit();
            return HAL_ERROR;
        }

        /* 1k per kick, overwrite the buffer every kick */
        HAL_FlashPacketTypeDef pkt = {
            .Data = flashbuf,
            .Address = Address + i * 1024U,
            .Length = 256U,
            .DMA = OC32_DMA_CH0,
        };
        if (FlashReadLines(&pkt) != HAL_OK)
        {
            HAL_CRC_DeInit();
            return HAL_ERROR;
        }

        crc32 = HAL_CRC_GetResult();
    }

    HAL_CRC_DeInit();
    /* program crc */
    Address = Address + 1020U;
    HAL_FlashPacketTypeDef crc_pkt = {
        .Data = &crc32,
        .Address = Address + 1020U,
        .Length = 1U,
        .DMA = OC32_DMA_CH0,
    };
    if (HAL_FLASH_Program(&crc_pkt) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;

}

#ifdef HAL_FLASH_VERIFY_ENABLE
/**
 * @brief  Read a flash region into the given buffer
 * @param  Address: Zone Address
 * @param  Size: Zone size
 * @retval HAL status
 */
__sram__ static HAL_StatusTypeDef FlashReadZone(uint32_t Address, uint32_t Size)
{
    uint32_t i;

    /* DMA buffer for flash reads */
    static uint32_t flashbuf[256] __attribute__((aligned(4)));

    HAL_CRCPacketTypedef crc_packet = {
        .Data = flashbuf,
        .Length = 1024U,
        .Mode = CRC_IDM_CAPTURE_DMAIN,
        .DMA = OC32_DMA_CH0,
    };

    for (i = 0; i < (Size >> 10U); i++)
    {
        if (HAL_CRC_Packet(&crc_packet) != HAL_OK)
            return HAL_ERROR;

        /* 1k per kick, overwrite the data every kick */
        HAL_FlashPacketTypeDef pkt = {
            .Data = &flashbuf,
            .Address = Address + i * 1024U,
            .Length = 256U,
            .DMA = OC32_DMA_CH0,
        };
        if (FlashReadLines(&pkt) != HAL_OK)
        {
            return HAL_ERROR;
        }
        HAL_CRC_GetResult();
    }

    return HAL_OK;
}

/**
 * @brief  copy zone from good bank to bad bank
 * @param  Bank0Err: Bank0 error flag
 * @param  Bank1Err: Bank1 error flag
 * @param  Size: Zone size
 * @retval HAL status
 */
__sram__ static HAL_StatusTypeDef FlashCopyZone(uint32_t Bank0Err, uint32_t Bank1Err, uint32_t Size)
{
    /* DMA buffer for flash reads */
    static uint32_t flashbuf[64] __attribute__((aligned(4)));

    /* source address, destination address */
    uint32_t saddr, daddr;
    if ((Bank0Err == 1) && (Bank1Err == 0))
    {
        daddr = 0;
        saddr = OC32_CODE_BANK_SIZE;
    }
    else if ((Bank0Err == 0) && (Bank1Err == 1))
    {
        saddr = 0;
        daddr = OC32_CODE_BANK_SIZE;
    }
    else
    {
        return HAL_ERROR;
    }

    if (Size == OC32_CODE_ZONE1_SIZE)
    {
        saddr = saddr + OC32_CODE_ZONE0_SIZE;
        daddr = daddr + OC32_CODE_ZONE0_SIZE;
    }

    /* erase error zone */
    HAL_FlashPacketTypeDef erase_pkt = {
        .Data = NULL,
        .Address = daddr,
        .Length = Size,
        .DMA = OC32_DMA_CH0,
    };
    if (HAL_FLASH_Erase(&erase_pkt) != HAL_OK)
        return HAL_ERROR;

    for (uint32_t page = 0; page < (Size / FLASH_PAGE_SIZE); page++)
    {
        /* read a page, then program a page */
        HAL_FlashPacketTypeDef read_pkt = {
            .Data = &flashbuf,
            .Address = saddr,
            .Length = FLASH_PAGE_SIZE >> 2,
            .DMA = OC32_DMA_CH0,
        };
        if (HAL_FLASH_Read(&read_pkt) != HAL_OK)
            return HAL_ERROR;

        HAL_FlashPacketTypeDef prog_pkt = {
            .Data = &flashbuf,
            .Address = daddr,
            .Length = FLASH_PAGE_SIZE >> 2,
            .DMA = OC32_DMA_CH0,
        };
        if (HAL_FLASH_Program(&prog_pkt) != HAL_OK)
            return HAL_ERROR;

        saddr += FLASH_PAGE_SIZE;
        daddr += FLASH_PAGE_SIZE;
    }

    return HAL_OK;
}

/**
 * @brief  Verify the CRC of user program code in flash, recover it if error.
 * @retval HAL status
 *
 * NOTICE:
 * RUN THIS FUNCTION WITHOUT ANY OTHER HARDWARE MODULE FUNCTION RUNNING
 * IT WILL OVERRIDE THE DMA & CRC CONFIG WITHOUT ANY RECOVERY
 * IT WILL REQUEST 1K DATA BUFFER FOR FLASH OPERATION, PLEASE MAKE SURE
 * ENOUGH MEMORY BEFORE CALLING IT
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_VerifyCode(void)
{

    FlashCrcInit();

    uint32_t tries;
    do
    {
        /* Bank0 Boot: 30K */
        FlashReadZone(0, OC32_CODE_ZONE0_SIZE);
        /* check CRC */
        uint32_t b0z0err = (READ_SR(CRCOUT) == CRC_GOOD_DEFAULT) ? 0 : 1;

        /* Bank0 User: 98K */
        FlashReadZone(OC32_CODE_ZONE0_SIZE, OC32_CODE_ZONE1_SIZE);
        /* check CRC */
        uint32_t b0z1err = (READ_SR(CRCOUT) == CRC_GOOD_DEFAULT) ? 0 : 1;

        /* Bank1 Boot: 30K */
        FlashReadZone(OC32_CODE_BANK_SIZE, OC32_CODE_ZONE0_SIZE);
        /* check CRC */
        uint32_t b1z0err = (READ_SR(CRCOUT) == CRC_GOOD_DEFAULT) ? 0 : 1;

        /* Bank1 User: 98K */
        FlashReadZone(OC32_CODE_BANK_SIZE + OC32_CODE_ZONE0_SIZE, OC32_CODE_ZONE1_SIZE);
        /* check CRC */
        uint32_t b1z1err = (READ_SR(CRCOUT) == CRC_GOOD_DEFAULT) ? 0 : 1;

        /* return ok if no error */
        if (!b0z0err && !b1z0err && !b0z1err && !b1z1err)
        {
            return HAL_OK;
        }

        /* flash copy */
        if (((b0z0err == 1) && (b1z0err == 1)) || ((b0z1err == 1) && (b1z1err == 1)))
        {
            /* can not repair, should not arrive here
             * do nothing, goto retry again
             * maybe some noise error during verify */
            continue;
        }

        if (b0z0err != b1z0err)
        {
            FlashCopyZone(b0z0err, b1z0err, OC32_CODE_ZONE0_SIZE);
        }

        if (b0z1err != b1z1err)
        {
            FlashCopyZone(b0z1err, b1z1err, OC32_CODE_ZONE1_SIZE);
        }

        /* go verify again after recovery */

    } while (++tries < HAL_FLASH_VERIFY_RETRY);

    HAL_CRC_DeInit();
    return HAL_ERROR;
}
#endif

#endif /* HAL_FLASH_ENABLE */