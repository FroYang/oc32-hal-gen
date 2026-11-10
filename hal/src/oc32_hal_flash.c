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
        FLCCON->FLTSHSL = FLASH_CORE_TSHSL_HIGH;
    }
    else
    {
        /* sclk/2 */
        FLCCON->FLCC = FLASH_CLK_DIV2;
        FLUCON->FLUC = FLASH_CLK_DIV2;
        FLCCON->FLTSHSL = FLASH_CORE_TSHSL_LOW;
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
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: on timeout
 */
__sram__ HAL_StatusTypeDef FlashKick(void)
{
    uint32_t timeout;

    FLUCON->FLUKS = HAL_ENABLE;
    timeout = FLASH_TIMEOUT_VALUE;
    while ((FLUCON->FLUOPF == HAL_FLAG_INVALID) && (timeout-- > 0))
        ;
    FLUCON->FLUOCLR = HAL_ENABLE;
    if (timeout == 0u)
        return HAL_ERROR;
    return HAL_OK;
}

/**
 * @brief  Sets SPI flash Write Enable
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: on timeout
 */
__sram__ HAL_StatusTypeDef FlashWREN(void)
{
    /* write enable command */
    FLUCON->FLUI = FLASH_WRITE_ENABLE;
    return FlashKick();
}

/**
 * @brief  Reads lines of data (4 words per line) from the specified address using DMA.
 * @param  Packet: pointer to a HAL_Flash_PacketTypeDef structure that contains
 *         the configuration information for the data packet.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if flashloader operation times out
 * @note   Internal helper; reads into Packet->Data via DMA flash channel.
 *         Packet->Length must be a multiple of 16 words and Packet->Data
 *         must be 32-bit aligned.
 */
__sram__ HAL_StatusTypeDef FlashReadLines(HAL_Flash_PacketTypeDef *Packet)
{
    /* translate length to the configure value (power of 2) */
    uint32_t n = 0;
    while (((Packet->Length / 16U) >> n) > 1U)
        n++;

    WRITE_SR(FLUADR, Packet->Address);
    FLUCON->FLUDC = FLASH_USER_LENGTH_16B;

    /* using core-accessing config, no need to config at user side*/
    FLUCON->FLURUCS = HAL_ENABLE;

    HAL_DMA_Update(0U, Packet->DmaCh, OC32_DMA_FLASH, OC32_DMA_BUSY);

    /* DMA mode */
    FLUCON->FLURRE = HAL_ENABLE;
    /* setup lines */
    FLUCON->FLURRC = n - 1;
    /* setup DMA address */
    WRITE_SR(FLURRDA, (uint32_t)Packet->Data);

    if (FlashKick() != HAL_OK)
        goto error;

    FLUCON->FLURUCS = HAL_DISABLE;
    FLUCON->FLURRE = HAL_DISABLE;
    HAL_DMA_Update(0U, Packet->DmaCh, OC32_DMA_FLASH, OC32_DMA_IDLE);
    return HAL_OK;

error:
    FLUCON->FLURUCS = HAL_DISABLE;
    FLUCON->FLURRE = HAL_DISABLE;
    HAL_DMA_Update(0U, Packet->DmaCh, OC32_DMA_FLASH, OC32_DMA_IDLE);
    return HAL_ERROR;
}

/**
 * @brief  Sets up SPI flash Quad-Read mode for CPU instruction fetch
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: on timeout
 * @note   Writes the QE bit in Status Register 2, then configures the
 *         core-accessing flash controller for 4-line (Quad) reads.
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
 * @retval HAL_OK: on success
 * @note   Disables all interrupts (HAL_HIC_DeInit), configures the flashloader
 *         for standard read mode, updates the flash clock divider based on
 *         SystemClock, and sets default read latency / data length / operation type.
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_Init(void)
{
    /* disable interrupt at first */
    HAL_HIC_DeInit();

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
 * @brief  Reads data from the specified flash address.
 * @param  Packet: pointer to a HAL_Flash_PacketTypeDef structure that contains
 *         the configuration information for the data packet.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Packet/Data/Length is invalid, address out of range,
 *         or flashloader operation times out
 * @note   Uses DMA for aligned multi-line reads; falls back to byte-by-byte
 *         user-accessing reads for small or unaligned transfers.
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_Read(HAL_Flash_PacketTypeDef *Packet)
{
    if (Packet == NULL || Packet->Data == NULL)
        return HAL_ERROR;
    if (Packet->Length == 0U)
        return HAL_ERROR;
    if ((Packet->Address + Packet->Length) > OC32_FLASH_SIZE)
        return HAL_ERROR;

    /* DMA condition check
     * 1. length is multiple of lines(16 bytes)
     * 2. buffer address is word aligned
     * 3. lines is 2/4/8/16/32/64/128/256 */
    if (((Packet->Length % 16U) != 0) || (((uintptr_t)Packet->Data & 0x3U) != 0))
        goto normal_read;

    uint32_t Lines = Packet->Length / 16U;
    if ((Lines < 2U) || ((Lines & (Lines - 1U)) != 0U) || (Lines > 256U))
        goto normal_read;

    /* dma_read */
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
    FLUCON->FLUSS = FLASH_CS_ENABLE;
    if (FlashKick() != HAL_OK)
        goto error;

    /* receive flash data */
    FLUCON->FLUT = FLASH_USER_OP_RDATA;
    uint32_t i;
    for (i = 0; i < Packet->Length; i += 4U)
    {
        if (FlashKick() != HAL_OK)
            goto error;

        /* may have 1~3 bytes garbage */
        Packet->Data[i] = READ_SR(FLUDATA0);
    }

    /* drive CS high */
    FLUCON->FLUSS = FLASH_CS_DISABLE;
    return HAL_OK;

error:
    FLUCON->FLUSS = FLASH_CS_DISABLE;
    return HAL_ERROR;
}

/**
 * @brief  Programs a word at the specified flash address.
 * @param  Packet: pointer to a HAL_Flash_PacketTypeDef structure that contains
 *         the configuration information for the data packet.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Packet/Data is NULL, data spans multiple pages,
 *         or flashloader operation times out
 * @note   All data in Packet->Data must reside within a single 256-byte page.
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_Program(HAL_Flash_PacketTypeDef *Packet)
{
    if (Packet == NULL || Packet->Data == NULL)
        return HAL_ERROR;

    /* check whether all of data locate in a same page */
    if (((Packet->Address + Packet->Length) & 0xFFFFFF00U) != (Packet->Address & 0xFFFFFF00U))
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
    FLUCON->FLUSS = FLASH_CS_ENABLE;
    if (FlashKick() != HAL_OK)
        goto error;

    /* send flash data.
     * Length is in bytes; Data is a uint32_t[] word array. Byte order is
     * LSB-first: in 1-byte mode FLUDATA0 contributes its lowest byte, and
     * 4-byte mode streams byte0 (LSB) through byte3. */
    FLUCON->FLUT = FLASH_USER_OP_WDATA;
    uint32_t i;
    uint32_t full_words = Packet->Length / 4U; /* complete 4-byte words */
    uint32_t rem_bytes = Packet->Length % 4U;  /* 0..3 trailing bytes  */
    uint32_t tail;

    /* fast path: send the full words 4 bytes at a time.
     * For Length < 4, full_words is 0 so this loop is skipped and the
     * remainder path below handles the whole transfer. */
    FLUCON->FLUDC = FLASH_USER_LENGTH_4B;
    for (i = 0; i < full_words; i++)
    {
        WRITE_SR(FLUDATA0, Packet->Data[i]);
        if (FlashKick() != HAL_OK)
            goto error;
    }

    /* send the remaining 1..3 bytes of the last word, 1 byte each */
    if (rem_bytes != 0U)
    {
        FLUCON->FLUDC = FLASH_USER_LENGTH_1B;
        tail = Packet->Data[full_words];
        for (i = 0; i < rem_bytes; i++)
        {
            WRITE_SR(FLUDATA0, tail);
            if (FlashKick() != HAL_OK)
                goto error;
            tail >>= 8U;
        }
    }

    /* drive CS high */
    FLUCON->FLUSS = FLASH_CS_DISABLE;

    /* wait busy */
    FLUCON->FLUI = FLASH_READ_STATUS;
    FLUCON->FLUT = FLASH_USER_OP_INST_RDATA;
    FLUCON->FLUWIPC = HAL_ENABLE;

    if (FlashKick() != HAL_OK)
        goto error;

    return HAL_OK;

error:
    FLUCON->FLUSS = FLASH_CS_DISABLE;
    return HAL_ERROR;
}

/**
 * @brief  Erases the specified flash address range.
 * @param  Packet: pointer to a HAL_Flash_PacketTypeDef structure that contains
 *         the configuration information for the data packet.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Packet is NULL, address/length is not page-aligned,
 *         range is out of bounds, or flashloader operation times out
 * @note   Uses sector erase (4 KB) wherever possible, falling back to
 *         page erase (256 B) for partial sectors.
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_Erase(HAL_Flash_PacketTypeDef *Packet)
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
        FLUCON->FLUWIPC = HAL_ENABLE;
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
 * @brief  CRC init for flash operations
 * @note   Internal helper; configures the CRC peripheral with default
 *         settings for flash integrity checking.
 */
__sram__ void FlashCrcInit(void)
{
    HAL_CRC_InitTypeDef crc_init = {
        .Poly = CRC_POLY_DEFAULT,
        .InitVal = 0xFFFFFFFFU,
        .Bits = CRC_32_BIT,
        .RefIn = 1U,
        .RefOut = 1U,
        .CplOut = 1U,
        .Rvb = 0,
        .DmaCh = OC32_DMA_CH0,
    };
    HAL_CRC_Init(&crc_init);
}

/**
 * @brief  Reads a flash zone and updates the CRC32 stored at the zone boundary.
 * @param  Zone: pointer to a HAL_Flash_ZoneTypeDef structure that contains
 *         the bank/zone configuration.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Zone is NULL, a read/CRC error occurs, or the
 *         CRC program write fails
 * @note   Reads the zone in 1 KB blocks via DMA, computes the running CRC,
 *         then programs the final CRC value into the last 4 bytes of the zone
 *         (offset 1020). Uses a 1 KB static DMA buffer; caller must ensure
 *         sufficient memory before calling.
 */
HAL_StatusTypeDef HAL_FLASH_UpdateCrc(HAL_Flash_ZoneTypeDef *Zone)
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

    HAL_CRC_PacketTypedef crc_packet = {
        .Data = flashbuf,
        .Length = 1024U,
        .IntE = HAL_DISABLE,
        .IDM = CRC_IDM_CAPTURE_DMAIN,
    };

    uint32_t crc32;
    for (i = 0; i < Size; i++)
    {
        /* exclude the word of crc32 data */
        if (i == (Size - 1))
        {
            crc_packet.Length = 1020U;
        }

        if (HAL_CRC_Packet(&crc_packet).Error == HAL_ERROR)
        {
            HAL_CRC_DeInit();
            return HAL_ERROR;
        }

        /* 1k per kick, overwrite the buffer every kick */
        HAL_Flash_PacketTypeDef pkt = {
            .Data = flashbuf,
            .Address = Address + i * 1024U,
            .Length = 1024U,
            .DmaCh = OC32_DMA_CH0,
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
    HAL_Flash_PacketTypeDef crc_pkt = {
        .Data = &crc32,
        .Address = Address + 1020U,
        .Length = 4U,
        .DmaCh = OC32_DMA_CH0,
    };
    if (HAL_FLASH_Program(&crc_pkt) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

#ifdef HAL_FLASH_VERIFY_ENABLE
/**
 * @brief  Reads a flash region and verifies CRC in 1 KB blocks.
 * @param  Address: starting address of the region
 * @param  Size: size of the region in bytes
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if a read or CRC operation fails
 * @note   Internal helper; uses a 1 KB static DMA buffer. The final CRC result
 *         is discarded (caller inspects CRCOUT directly after each block).
 */
__sram__ HAL_StatusTypeDef FlashReadZone(uint32_t Address, uint32_t Size)
{
    uint32_t i;

    /* DMA buffer for flash reads */
    static uint32_t flashbuf[256] __attribute__((aligned(4)));

    HAL_CRC_PacketTypedef crc_packet = {
        .Data = flashbuf,
        .Length = 1024U,
        .IntE = HAL_DISABLE,
        .IDM = CRC_IDM_CAPTURE_DMAIN,
    };

    for (i = 0; i < (Size >> 10U); i++)
    {
        if (HAL_CRC_Packet(&crc_packet).Error == HAL_ERROR)
            return HAL_ERROR;

        /* 1k per kick, overwrite the data every kick */
        HAL_Flash_PacketTypeDef pkt = {
            .Data = flashbuf,
            .Address = Address + i * 1024U,
            .Length = 1024U,
            .DmaCh = OC32_DMA_CH0,
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
 * @brief  Copies a zone from the good bank to the bad bank.
 * @param  Bank0Err: non-zero if bank 0 has an error
 * @param  Bank1Err: non-zero if bank 1 has an error
 * @param  Size: zone size in bytes (OC32_CODE_ZONE0_SIZE or OC32_CODE_ZONE1_SIZE)
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if both or neither bank has an error, or a read/program fails
 * @note   Erases the destination zone, then copies the source zone page-by-page
 *         (256 B per page) into the destination. Uses a static DMA buffer.
 */
__sram__ HAL_StatusTypeDef FlashCopyZone(uint32_t Bank0Err, uint32_t Bank1Err, uint32_t Size)
{
    /* DMA buffer for flash reads */
    static uint32_t flashbuf[64] __attribute__((aligned(4)));

    /* source address, destination address */
    uint32_t saddr, daddr;
    if ((Bank0Err == HAL_FLAG_ERROR) && (Bank1Err == HAL_FLAG_GOOD))
    {
        daddr = 0;
        saddr = OC32_CODE_BANK_SIZE;
    }
    else if ((Bank0Err == HAL_FLAG_GOOD) && (Bank1Err == HAL_FLAG_ERROR))
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
    HAL_Flash_PacketTypeDef erase_pkt = {
        .Data = NULL,
        .Address = daddr,
        .Length = Size,
        .DmaCh = OC32_DMA_CH0,
    };
    if (HAL_FLASH_Erase(&erase_pkt) != HAL_OK)
        return HAL_ERROR;

    for (uint32_t page = 0; page < (Size / FLASH_PAGE_SIZE); page++)
    {
        /* read a page, then program a page */
        HAL_Flash_PacketTypeDef read_pkt = {
            .Data = flashbuf,
            .Address = saddr,
            .Length = FLASH_PAGE_SIZE,
            .DmaCh = OC32_DMA_CH0,
        };
        if (HAL_FLASH_Read(&read_pkt) != HAL_OK)
            return HAL_ERROR;

        HAL_Flash_PacketTypeDef prog_pkt = {
            .Data = flashbuf,
            .Address = daddr,
            .Length = FLASH_PAGE_SIZE,
            .DmaCh = OC32_DMA_CH0,
        };
        if (HAL_FLASH_Program(&prog_pkt) != HAL_OK)
            return HAL_ERROR;

        saddr += FLASH_PAGE_SIZE;
        daddr += FLASH_PAGE_SIZE;
    }

    return HAL_OK;
}

/**
 * @brief  Verifies the CRC of user program code in flash; recovers it if error.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if CRC error is detected and cannot be recovered after
 *         HAL_FLASH_VERIFY_RETRY attempts
 *
 * NOTICE:
 * RUN THIS FUNCTION WITHOUT ANY OTHER HARDWARE MODULE FUNCTION RUNNING.
 * IT WILL OVERRIDE THE DMA & CRC CONFIG WITHOUT ANY RECOVERY.
 * IT WILL REQUEST 1K DATA BUFFER FOR FLASH OPERATION; PLEASE MAKE SURE
 * ENOUGH MEMORY IS AVAILABLE BEFORE CALLING IT.
 */
__sram__ HAL_StatusTypeDef HAL_FLASH_VerifyCode(void)
{

    FlashCrcInit();

    uint32_t tries = 0U;
    while (++tries < HAL_FLASH_VERIFY_RETRY)
    {
        /* Bank0 Boot: 30K */
        FlashReadZone(0, OC32_CODE_ZONE0_SIZE);
        /* check CRC */
        uint32_t b0z0err = (READ_SR(CRCOUT) == CRC_GOOD_DEFAULT) ? HAL_FLAG_GOOD : HAL_FLAG_ERROR;

        /* Bank0 User: 98K */
        FlashReadZone(OC32_CODE_ZONE0_SIZE, OC32_CODE_ZONE1_SIZE);
        /* check CRC */
        uint32_t b0z1err = (READ_SR(CRCOUT) == CRC_GOOD_DEFAULT) ? HAL_FLAG_GOOD : HAL_FLAG_ERROR;

        /* Bank1 Boot: 30K */
        FlashReadZone(OC32_CODE_BANK_SIZE, OC32_CODE_ZONE0_SIZE);
        /* check CRC */
        uint32_t b1z0err = (READ_SR(CRCOUT) == CRC_GOOD_DEFAULT) ? HAL_FLAG_GOOD : HAL_FLAG_ERROR;

        /* Bank1 User: 98K */
        FlashReadZone(OC32_CODE_BANK_SIZE + OC32_CODE_ZONE0_SIZE, OC32_CODE_ZONE1_SIZE);
        /* check CRC */
        uint32_t b1z1err = (READ_SR(CRCOUT) == CRC_GOOD_DEFAULT) ? HAL_FLAG_GOOD : HAL_FLAG_ERROR;

        /* return ok if no error */
        if (!b0z0err && !b1z0err && !b0z1err && !b1z1err)
        {
            return HAL_OK;
        }

        /* flash copy */
        if (((b0z0err == HAL_FLAG_ERROR) && (b1z0err == HAL_FLAG_ERROR)) || ((b0z1err == HAL_FLAG_ERROR) && (b1z1err == HAL_FLAG_ERROR)))
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
    }

    HAL_CRC_DeInit();
    return HAL_ERROR;
}
#endif

#endif /* HAL_FLASH_ENABLE */