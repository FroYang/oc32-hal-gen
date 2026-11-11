/**
 * @file    oc32_hal.c
 * @author
 * @brief   OC32 HAL module driver.
 *          This file provides firmware functions to manage the following
 *          functionalities of the OC32 peripherals:
 *           + Initialization and de-initialization functions
 *           + Clock Core functions
 *           + tick functions
 *           + Delay functions
 *           + System configuration functions
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal.h"
#include "oc32_hal_static.h"

/**
 * @brief OC32 HAL Driver version number
 */
#define __OC32_HAL_VERSION_MAIN (0x01U) /*!< [31:24] main version */
#define __OC32_HAL_VERSION_SUB1 (0x00U) /*!< [23:16] sub1 version */
#define __OC32_HAL_VERSION_SUB2 (0x00U) /*!< [15:8]  sub2 version */
#define __OC32_HAL_VERSION_RC (0x00U)   /*!< [7:0]  release candidate */
#define __OC32_HAL_VERSION ((__OC32_HAL_VERSION_MAIN << 24) | (__OC32_HAL_VERSION_SUB1 << 16) | (__OC32_HAL_VERSION_SUB2 << 8) | (__OC32_HAL_VERSION_RC))

/**
 * @}
 */

/** @defgroup HAL_Private_Variables HAL Private Variables
 * @{
 */

/* the tick counter */
volatile uint32_t uwTick;

/* Syscall request buffer: filled by __syscall_handler via SYSC instruction */
uint32_t SysReqBuf[4];

/** VTS table index */
uint32_t VtIndex;

/* DMA Channel State
 *
 * Per-channel busy/idle bookkeeping for the shared DMA hardware. Each
 * peripheral declares the DMA channel it wants via its Init struct's DmaCh
 * field (set by the user). The HAL layer does NOT arbitrate
 * channel ownership between modules: the user is responsible for assigning
 * non-conflicting channels and for serializing access to any channel that
 * two modules both claim. DMAChState[] only tracks whether the channel is
 * currently in use so that HAL_DMA_Update() can reject a second BUSY claim
 * on the same channel. Flash operations are exclusive with respect to the
 * DMA/CRC configuration (see HAL_FLASH_VerifyCode). */
uint32_t DMAChState[OC32_DMA_CH_TOTAL];

/* Fuse data symbols defined in crti-hw.S (.boot section), linked by compiler */
extern const uint32_t __FUSE_PID[4];
extern const uint32_t __FUSE_LID[4];
extern const uint32_t __FUSE_TSN[4];
extern const uint32_t __FUSE_REGTRIM0[4];
extern const uint32_t __FUSE_REGTRIM1[4];
extern const uint32_t __FUSE_REGTRIM2[4];
extern const uint32_t __FUSE_REGTRIM3[4];
extern const uint32_t __FUSE_VTTABLE[32];


/**
 * @}
 */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Minimal memcpy for bare-metal (-nostdlib) environments.
 * @note   Compiled only when NO_STDLIB is defined (pass -DNO_STDLIB for
 *         bare-metal builds that do not link the C library). Marked weak so
 *         that a stronger definition (e.g. from a hand-rolled runtime or a
 *         later-stage library) automatically overrides this one.
 */
#ifdef NO_STDLIB
__weak__ void *memcpy(void *dest, const void *src, uint32_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    while (n--)
    {
        *d++ = *s++;
    }
    return dest;
}
#endif /* NO_STDLIB */

/**
 * @brief  Minimal memset for bare-metal (-nostdlib) environments.
 * @note   Compiled only when NO_STDLIB is defined (pass -DNO_STDLIB for
 *         bare-metal builds that do not link the C library). Marked weak so
 *         that a stronger definition (e.g. from a hand-rolled runtime or a
 *         later-stage library) automatically overrides this one.
 */
#ifdef NO_STDLIB
__weak__ void *memset(void *dest, int c, uint32_t n)
{
    uint8_t *d = (uint8_t *)dest;

    while (n--)
    {
        *d++ = (uint8_t)c;
    }

    return dest;
}
#endif /* NO_STDLIB */

/**
 * @brief  update DMA channel state
 * @param  DMAE: current DMA enable state (read from the module's DMA-enable register)
 * @param  DmaCh: DMA channel number
 * @param  Module: DMA module to map to the channel
 * @param  State: DMA state to record for the channel
 * @retval HAL_OK: on success (or if DMAE indicates the channel is already enabled)
 * @retval HAL_ERROR: if DmaCh is invalid or the channel is already busy
 *
 * @note   This helper only tracks per-channel busy/idle and maps a channel to
 *         one module. It does NOT arbitrate ownership between modules: the
 *         caller (user) assigns which channel each module uses and is
 *         responsible for keeping a channel from being claimed by two modules
 *         at the same time. Flash/CRC internal operations default to CH0,
 *         so user-configured modules that also use CH0 must not run them
 *         concurrently (see HAL_FLASH_VerifyCode's exclusivity note).
 */
__sram__ HAL_StatusTypedef HAL_DMA_Update(HAL_EnableTypedef DMAE, HAL_DMA_ChTypedef DmaCh, HAL_DMA_MapTypedef Module, HAL_DMA_StateTypedef State)
{
    /* indicates that the DMA is busy now, should not update the state */
    if (DMAE == HAL_ENABLE)
        return HAL_OK;

    /* error if try to set the busy channel to busy state
    allow: busy -> idle; idle -> idle/busy */
    if ((State == OC32_DMA_BUSY) && (DMAChState[DmaCh] == OC32_DMA_BUSY))
        return HAL_ERROR;

    switch (DmaCh)
    {
    case OC32_DMA_CH0:
        DMACON->DMACH0 = Module;
        break;
    case OC32_DMA_CH1:
        DMACON->DMACH1 = Module;
        break;
    case OC32_DMA_CH2:
        DMACON->DMACH2 = Module;
        break;
    case OC32_DMA_CH3:
        DMACON->DMACH3 = Module;
        break;
    default:
        return HAL_ERROR;
    }

    DMAChState[DmaCh] = State;
    return HAL_OK;
}

/**
 * @brief  System trap handler invoked by crti-hw.S when the SYSC trap vector
 *         is taken. This is a temporary placeholder: it performs no action and
 *         just returns 0. (The fused-data read into SysReqBuf[4] is actually
 *         handled by __syscall_handler, not here.)
 * @retval 0 on success
 */
int __systrap_handler(void)
{

    return 0;
}

/**
 * @brief  System call handler invoked by crtihw.S when SYSC instruction is executed.
 *         Reads fused data from the appropriate source into SysReqBuf[4].
 * @param  Num: syscall number (0=PID, 1=LID, 2=TSN, 3=REGTRIM0, 4=REGTRIM1, 5=REGTRIM2)
 * @retval 0 on success, -1 on invalid number
 */
int __syscall_handler(uint32_t Num)
{
    const uint32_t *fuseptr;

    switch (Num)
    {
    case SYSC_PID:
        fuseptr = __FUSE_PID;
        break;

    case SYSC_LID:
        fuseptr = __FUSE_LID;
        break;

    case SYSC_TSN:
        fuseptr = __FUSE_TSN;
        break;

    case SYSC_REGTRIM0:
        fuseptr = __FUSE_REGTRIM0;
        break;

    case SYSC_REGTRIM1:
        fuseptr = __FUSE_REGTRIM1;
        break;

    case SYSC_REGTRIM2:
        fuseptr = __FUSE_REGTRIM2;
        break;

    case SYSC_REGTRIM3:
        fuseptr = __FUSE_REGTRIM3;
        break;

    case SYSC_VTTABLE:
        fuseptr = __FUSE_VTTABLE + (VtIndex & 0x00FCU);
        break;

    default:
        return -1;
    }

    memcpy(SysReqBuf, fuseptr, sizeof(SysReqBuf));
    return 0;
}


/**
 * @brief This function configures the source of the time base.
 *        The time source is configured  to have 1ms time base with a dedicated
 *        Tick interrupt priority.
 * @note In the default implementation, WDT1 timer is the source of time base.
 *       It is used to generate interrupts at regular time intervals.
 *       Care must be taken if HAL_Delay() is called from a peripheral ISR process,
 *       The SysTick interrupt must have higher priority (numerically higher)
 *       than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
 * @retval HAL_OK: on success
 */
HAL_StatusTypedef HAL_TICK_Init(void)
{
    HAL_WDT_InitTypedef Init = {
        .Wdt = WDT_1,
        .Mode = WDT_MODE_INT,
        .Period = HAL_TICK_FREQ_DEFAULT * 1000U};

    HAL_WDT_Init(&Init);

    return HAL_OK;
}

/**
 * @brief  This function is called to setup global HIC(hardware interrupt controller)
 * @param  Init: pointer to a HAL_HICInittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL
 * @note   Always enables the WDT channel (vector 17) in HIE so that the
 *         system tick timer is active, regardless of Init->ChEnable.
 */
HAL_StatusTypedef HAL_HIC_Init(HAL_HICInitTypedef *Init)
{
    if (Init == NULL)
        return HAL_ERROR;

    /* e.g: ChEnable = OC32_HIC_SERR | OC32_HIC_CCP0 | OC32_HIC_TIMER6 .... */
    /* always set WDT HIE for system tick timer */
    WRITE_SR(HIE, Init->ChEnable | OC32_HIC_WDT);

    /* e.g: ChPriorityBit0 = HPL_Bit0(OC32_HIC_PRI1, OC32_HIC_CH_SERR) | HPL_Bit0(OC32_HIC_PRI0, OC32_HIC_CH_UART1) | .... */
    /* e.g: ChPriorityBit1 = HPL_Bit1(OC32_HIC_PRI1, OC32_HIC_CH_SERR) | HPL_Bit1(OC32_HIC_PRI0, OC32_HIC_CH_UART1) | .... */
    WRITE_SR(HIPL0, Init->ChPriorityBit0);
    WRITE_SR(HIPL1, Init->ChPriorityBit1);

    return HAL_OK;
}

/**
 * @brief  This function is called to setup global HIC(hardware interrupt controller)
 * @note   Clears HIE entirely, including the WDT bit. As a result the system
 *         tick (WDT interrupt, see HAL_HIC_Init) is disabled and HAL_Delay
 *         stops advancing. Re-enable via HAL_HIC_Init after this call if the
 *         tick is needed again.
 */
void HAL_HIC_DeInit(void)
{
    WRITE_SR(HIE, 0);
    WRITE_SR(HIPRM, 0);
    WRITE_SR(HIPL0, 0);
    WRITE_SR(HIPL1, 0);
}

/**
 * @brief  This function provides minimum delay (in milliseconds) based on tick increment.
 * @param  Delay: specifies the delay time length, in milliseconds.
 */
void HAL_Delay(uint32_t Delay)
{
    uint32_t tickstart = HAL_TICK_GetVal();

    while ((HAL_TICK_GetVal() - tickstart) < Delay)
    {
    }
}

/**
 * @brief  Handles POR interrupt request.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ HAL_StatusTypedef HAL_POR_UserIRQ(void)
{
    return HAL_OK;
}

/**
 * @brief  Handles Debug Reset interrupt request.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ HAL_StatusTypedef HAL_DBGR_UserIRQ(void)
{
    return HAL_OK;
}

/**
 * @brief  Handles HRF(Hardware Reset Flags) interrupt request.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if no recognized reset flag is set
 * @note   Clears POR/DBGR flags and dispatches to HAL_POR_UserIRQ /
 *         HAL_DBGR_UserIRQ. Also handles LVD and WDT reset flags.
 */
HAL_StatusTypedef HAL_HRF_IRQHandler(void)
{
    /* clear flag */
    if (HRF->PORF == HAL_FLAG_VALID)
    {
        HRF->PORFCLR = HAL_ENABLE;
        return HAL_POR_UserIRQ();
    }

    if (HRF->DBGRF == HAL_FLAG_VALID)
    {
        HRF->DBGRFCLR = HAL_ENABLE;
        return HAL_DBGR_UserIRQ();
    }

    /* lvd reset flag or lvd event */
#ifdef HAL_LVD_ENABLE
    if (HRF->LVDRF == HAL_FLAG_VALID)
        return HAL_LVD_IRQHandler();
#endif /* HAL_LVD_ENABLE */

#ifdef HAL_WDT_ENABLE
    if (HRF->WDT0RF == HAL_FLAG_VALID)
        return HAL_WDT_IRQHandler();
    /* WDT1 is tick timer, reset disable */
#endif /* HAL_WDT_ENABLE */

#ifdef HAL_VTS_ENABLE
    if (HRF->VTSAF == HAL_FLAG_VALID)
        return HAL_VTS_IRQHandler();
#endif /* HAL_VTS_ENABLE */

    return HAL_ERROR;
}

#ifdef HAL_UART_DEBUG_ENABLE
/**
 * @brief  uart tx char for debug
 */
void uart_tx_char(char c)
{

    uint32_t txdata = (uint32_t)c;
    HAL_UART_PacketTypedef Packet;
    Packet.Length = 1U;
    Packet.Data = &txdata;
    Packet.IntE = HAL_DISABLE;
#ifdef HAL_UART0_DEBUG
    HAL_UART_TX(UART_0, &Packet);
#endif

#ifdef HAL_UART1_DEBUG
    HAL_UART_TX(UART_1, &Packet);
#endif
}

/**
 * @brief  Minimal puts for bare-metal (-nostdlib) environments.
 */
__weak__ int puts(const char *s)
{
    while (*s)
    {
        uart_tx_char(*s++);
    }
    uart_tx_char('\n');
    return 1;
}

static int txchar(char c)
{
    uart_tx_char(c);
    return 1;
}

static int txstring(const char *s)
{
    int count = 0;
    while (*s)
    {
        count += txchar(*s++);
    }
    return count;
}

static int txuint(unsigned int n)
{
    int count = 0;

    if (n >= 10)
    {
        count += txuint(n / 10);
    }
    count += txchar('0' + (n % 10));

    return count;
}

static int txint(int n)
{
    int count = 0;

    if (n < 0)
    {
        count += txchar('-');
        if (n == (int)0x80000000)
        {
            count += txstring("2147483648");
            return count;
        }
        n = -n;
    }
    count += txuint((unsigned int)n);

    return count;
}

static char get_hex_char(unsigned int n)
{
    const char *hex_chars = "0123456789abcdef";
    return hex_chars[n & 0xF];
}

static char get_hex_char_upper(unsigned int n)
{
    const char *hex_chars = "0123456789ABCDEF";
    return hex_chars[n & 0xF];
}

static int txhex(unsigned int n)
{
    char buf[9];
    int i = 7;
    int count = 0;

    do
    {
        buf[i--] = get_hex_char(n);
        n >>= 4;
    } while (n > 0);

    while (i >= 0)
    {
        buf[i--] = '0';
    }

    count += txstring(buf);

    return count;
}

static int txhex_upper(unsigned int n)
{
    char buf[9];
    int i = 7;
    int count = 0;

    do
    {
        buf[i--] = get_hex_char_upper(n);
        n >>= 4;
    } while (n > 0);

    while (i >= 0)
    {
        buf[i--] = '0';
    }

    count += txstring(buf);

    return count;
}

static int txptr(void *ptr)
{
    int count = 0;
    count += txstring("0x");
    count += txhex((unsigned int)(unsigned long)ptr);
    return count;
}

/**
 * @brief  Minimal printf for bare-metal (-nostdlib) environments.
 * @note   Supported conversion specifiers: %d, %u, %x, %X, %s, %c, %p, %%.
 *         All values are taken as 32-bit (int / unsigned int / pointer).
 *         64-bit types (int64_t/uint64_t) and length modifiers (%ld, %hd,
 *         %ll, etc.) are NOT supported and will print incorrect results.
 */
__weak__ int printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    int count = 0;

    while (*format)
    {
        if (*format == '%')
        {
            format++;

            switch (*format)
            {
            case 'd':
                count += txint(va_arg(args, int));
                break;

            case 'u':
                count += txuint(va_arg(args, unsigned int));
                break;

            case 'x':
                count += txhex(va_arg(args, unsigned int));
                break;

            case 'X':
                count += txhex_upper(va_arg(args, unsigned int));
                break;

            case 's':
                count += txstring(va_arg(args, char *));
                break;

            case 'c':
                count += txchar(va_arg(args, int));
                break;

            case 'p':
                count += txptr(va_arg(args, void *));
                break;

            case '%':
                count += txchar('%');
                break;

            default:
                count += txchar('%');
                count += txchar(*format);
                break;
            }
        }
        else
        {
            count += txchar(*format);
        }

        format++;
    }

    va_end(args);
    return count;
}

/**
 * @brief  This function is called to setup global debug uart
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if UART init fails
 */
HAL_StatusTypedef HAL_DEBUG_Init(void)
{
    HAL_UART_InitTypedef Init;
#ifdef HAL_UART0_DEBUG
    Init.Uart = UART_0;
#endif
#ifdef HAL_UART1_DEBUG
    Init.Uart = UART_1;
#endif
    Init.Parity = HAL_UART_DEBUG_PARITY;
    Init.Bits = UART_8_BIT_PAR;
    Init.Baud = HAL_UART_DEBUG_BR;
    Init.Duplex = HAL_UART_DEBUG_DUPLEX;
    Init.Pins = HAL_UART_DEBUG_PINS;
    Init.DmaCh = OC32_DMA_CH0;

    return HAL_UART_Init(&Init);
}
#endif /* HAL_UART_DEBUG_ENABLE */

/**
 * @brief  This function is called to setup global system: clock, tick, debug uart, etc.
 * @note   This function is declared as __weak  to be overwritten  in case of user.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if RCM init, flash verify, debug uart, or tick init fails
 */
__weak__ HAL_StatusTypedef HAL_SYS_Init(void)
{
    /* setup system clock */
    if (HAL_RCM_Init() != HAL_OK)
        return HAL_ERROR;

#ifndef HAL_LVD_ENABLE
/* turn off lvd if not enabled */
    HAL_LVD_DeInit();
#endif /* HAL_LVD_ENABLE */

#ifdef HAL_QUAD_READ_ENABLE
    /* setup flash quad read mode for cpu instruction fetch */
    HAL_FLASH_CPUQuadRead();
#endif /* HAL_QUAD_READ_ENABLE */

#ifdef HAL_FLASH_VERIFY_ENABLE
    /* verify flash code integrity */
    if (HAL_FLASH_VerifyCode() != HAL_OK)
        return HAL_ERROR;
#endif /* HAL_FLASH_VERIFY_ENABLE */

#ifdef HAL_UART_DEBUG_ENABLE
    /* setup debug uart */
    if (HAL_DEBUG_Init() != HAL_OK)
        return HAL_ERROR;
#endif /* HAL_UART_DEBUG_ENABLE */

    /* setup tick timer */
    /* NOTE: HAL_TICK_Init only configures the WDT (INT mode) tick parameters
     * and does NOT enable the WDT interrupt in HIE. The system tick only
     * actually runs once the user calls HAL_HIC_Init() (which sets the WDT
     * HIE bit). Until then, uwTick stays 0 and HAL_Delay will not advance. */
    if (HAL_TICK_Init() != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}