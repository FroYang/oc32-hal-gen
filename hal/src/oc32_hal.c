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
volatile uint32_t uwTick;
HAL_TickFreqTypeDef uwTickFreq = HAL_TICK_FREQ_DEFAULT; /* 1KHz */

/* Syscall request buffer: filled by __syscall_handler via SYSC instruction */
static uint32_t requestbuf[4];

/* DMA Channel State */
uint32_t DMAChState[OC32_DMA_CH_TOTAL];

/* Fuse data symbols defined in crti-hw.S (.boot section), linked by compiler */
extern const uint32_t __FUSE_PID[4];
extern const uint32_t __FUSE_LID[4];
extern const uint32_t __FUSE_TSN[4];
extern const uint32_t __FUSE_REGTRIM0[4];
extern const uint32_t __FUSE_REGTRIM1[4];
extern const uint32_t __FUSE_REGTRIM2[4];

/**
 * @}
 */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Minimal memcpy for bare-metal (-nostdlib) environments.
 */
void *memcpy(void *dest, const void *src, uint32_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    while (n--)
    {
        *d++ = *s++;
    }
    return dest;
}

/**
 * @brief  Minimal memset for bare-metal (-nostdlib) environments.
 */
void *memset(void *dest, int c, uint32_t n)
{
    uint8_t *d = (uint8_t *)dest;

    while (n--)
    {
        *d++ = (uint8_t)c;
    }

    return dest;
}

#ifdef HAL_UART_DEBUG_ENABLE
/**
 * @brief  uart tx char for debug
 */
void uart_tx_char(char c)
{
#ifdef HAL_UART0_DEBUG
    HAL_UART_TX_OneByte(UT0CON, c);
#endif

#ifdef HAL_UART1_DEBUG
    HAL_UART_TX_OneByte(UT1CON, c);
#endif
}

/**
 * @brief  Minimal puts for bare-metal (-nostdlib) environments.
 */
int puts(const char *s)
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

int printf(const char *format, ...)
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

#endif

/**
 * @brief  update DMA channel state
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_DMA_Update(HAL_DMAChTypedef DmaCh, HAL_DMAMapTypedef Module, HAL_DMAStateTypedef State)
{
    /* error if try to set the busy channel */
    if (State && DMAChState[DmaCh])
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
 * @brief  System trap handler invoked by crtihw.S when SYSC instruction is executed.
 *         Reads fused data from the appropriate source into requestbuf[4].
 * @retval 0 on success, -1 on invalid number
 */
int __systrap_handler(void)
{

    return 0;
}

/**
 * @brief  System call handler invoked by crtihw.S when SYSC instruction is executed.
 *         Reads fused data from the appropriate source into requestbuf[4].
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

    default:
        return -1;
    }

    memcpy(requestbuf, fuseptr, sizeof(requestbuf));
    return 0;
}

/**
 * @brief  Trigger SYSC instruction to read fused Product ID into requestbuf.
 */
static void request_PID(void)
{
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_PID) : "memory");
}

/**
 * @brief  Trigger SYSC instruction to read fused Test Serial Number into requestbuf.
 */
static void request_TSN(void)
{
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_TSN) : "memory");
}

/**
 * @brief  Trigger SYSC instruction to read fused Register Trim Value into requestbuf.
 */
static void request_REGTRIM0(void)
{
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_REGTRIM0) : "memory");
}

/**
 * @brief  Trigger SYSC instruction to read fused Register Trim Value into requestbuf.
 */
static void request_REGTRIM1(void)
{
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_REGTRIM1) : "memory");
}

/**
 * @brief  Trigger SYSC instruction to read fused Register Trim Value into requestbuf.
 */
static void request_REGTRIM2(void)
{
    __asm__ __volatile__("SYSC  %0" : : "i"(SYSC_REGTRIM2) : "memory");
}

/**
 * @brief This function configures the source of the time base.
 *        The time source is configured  to have 1ms time base with a dedicated
 *        Tick interrupt priority.
 * @note This function is called  automatically at the beginning of program after
 *       reset by HAL_GPIO_Init() or at any time when clock is reconfigured.
 * @note In the default implementation, WDT1 timer is the source of time base.
 *       It is used to generate interrupts at regular time intervals.
 *       Care must be taken if HAL_Delay() is called from a peripheral ISR process,
 *       The SysTick interrupt must have higher priority (numerically higher)
 *       than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
 *       The function is declared as __weak  to be overwritten  in case of other
 *       implementation  in user file.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_TICK_Init(void)
{
    HAL_WDTInitTypeDef Init = {
        .WDTCON = WDT1CON,
        .Mode = WDT_MODE_INT,
        .Period = HAL_TICK_FREQ_DEFAULT * 1000U};

    HAL_WDT_Init(&Init);

    return HAL_OK;
}

/**
 * @brief  This function is called to setup global HIC(hardware interrupt controller)
 * @param  Init: pointer to a HAL_HICInitTypeDef structure.
 */
HAL_StatusTypeDef HAL_HIC_Init(HAL_HICInitTypeDef *Init)
{
    if (Init == NULL)
        return HAL_ERROR;

    /* e.g: ChEnable = OC32_HIC_SERR | OC32_HIC_CCP0 | OC32_HIC_TIMER6 .... */
    WRITE_SR(HIE, Init->ChEnable);

    /* e.g: ChPriorityBit0 = HPL_Bit0(OC32_INT_PRI1, OC32_HIC_CH_SERR) | HPL_Bit0(OC32_INT_PRI0, OC32_HIC_CH_UART1) | .... */
    /* e.g: ChPriorityBit1 = HPL_Bit1(OC32_INT_PRI1, OC32_HIC_CH_SERR) | HPL_Bit1(OC32_INT_PRI0, OC32_HIC_CH_UART1) | .... */
    WRITE_SR(HIPL0, Init->ChPriorityBit0);
    WRITE_SR(HIPL1, Init->ChPriorityBit1);

    return HAL_OK;
}

/**
 * @brief  This function is called to setup global HIC(hardware interrupt controller)
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
 * @brief  Returns the device revision identifier.
 * @retval Revision ID (from fused __FUSE_PID[1])
 * @note   This function should not be used in an interrupt service routine.
 */
uint32_t HAL_GetRevID(void)
{
    request_PID();
    return requestbuf[1];
}

/**
 * @brief  Returns the device identifier.
 * @retval Device ID (from fused __FUSE_PID[0])
 * @note   This function should not be used in an interrupt service routine.
 */
uint32_t HAL_GetDevID(void)
{
    request_PID();
    return requestbuf[0];
}

/**
 * @brief  Returns the first word of the 128-bit unique device ID.
 * @retval Unique Device ID word 0 (from fused __FUSE_TSN[0])
 * @note   This function should not be used in an interrupt service routine.
 */
uint32_t HAL_GetUIDw0(void)
{
    request_TSN();
    return requestbuf[0];
}

/**
 * @brief  Returns the second word of the 128-bit unique device ID.
 * @retval Unique Device ID word 1 (from fused __FUSE_TSN[1])
 * @note   This function should not be used in an interrupt service routine.
 */
uint32_t HAL_GetUIDw1(void)
{
    request_TSN();
    return requestbuf[1];
}

/**
 * @brief  Returns the third word of the 128-bit unique device ID.
 * @retval Unique Device ID word 2 (from fused __FUSE_TSN[2])
 * @note   This function should not be used in an interrupt service routine.
 */
uint32_t HAL_GetUIDw2(void)
{
    request_TSN();
    return requestbuf[2];
}

/**
 * @brief  Returns the fourth word of the 128-bit unique device ID.
 * @retval Unique Device ID word 3 (from fused __FUSE_TSN[3])
 * @note   This function should not be used in an interrupt service routine.
 */
uint32_t HAL_GetUIDw3(void)
{
    request_TSN();
    return requestbuf[3];
}

/**
 * @brief  Returns the first word of reg trim value 0
 * @retval Register Trim Value word 0 (from fused __FUSE_REGTRIM0[0])
 * @note   This function should not be used in an interrupt service routine.
 */
uint32_t HAL_GetRegTrim0w0(void)
{
    request_REGTRIM0();
    return requestbuf[0];
}

/**
 * @brief  Returns the first word of reg trim value 1
 * @retval Register Trim Value word 1 (from fused __FUSE_REGTRIM0[1])
 * @note   This function should not be used in an interrupt service routine.
 */
uint32_t HAL_GetRegTrim0w1(void)
{
    request_REGTRIM0();
    return requestbuf[1];
}

/**
 * @brief  Returns the first word of reg trim value 2
 * @retval Register Trim Value word 0 (from fused __FUSE_REGTRIM0[2])
 * @note   This function should not be used in an interrupt service routine.
 */
uint32_t HAL_GetRegTrim0w2(void)
{
    request_REGTRIM0();
    return requestbuf[2];
}

/**
 * @brief  Returns the first word of reg trim value 3
 * @retval Register Trim Value word 0 (from fused __FUSE_REGTRIM0[3])
 * @note   This function should not be used in an interrupt service routine.
 */
uint32_t HAL_GetRegTrim0w3(void)
{
    request_REGTRIM0();
    return requestbuf[3];
}

/**
 * @brief  Handles POR interrupt request.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ HAL_StatusTypeDef HAL_POR_UserIRQ(void)
{
    return HAL_OK;
}

/**
 * @brief  Handles Debug Reset interrupt request.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ HAL_StatusTypeDef HAL_DBGR_UserIRQ(void)
{
    return HAL_OK;
}

/**
 * @brief  Handles POR interrupt request.
 */
HAL_StatusTypeDef HAL_HRF_IRQHandler(void)
{
    /* clear flag */
    if (HRF->PORF)
    {
        HRF->PORFCLR = 1U;
        return HAL_POR_UserIRQ();
    }

    if (HRF->DBGRF)
    {
        HRF->DBGRFCLR = 1U;
        return HAL_DBGR_UserIRQ();
    }

    /* lvd reset flag or lvd event */
#ifdef HAL_LVD_ENABLE
    if (HRF->LVDRF)
        return HAL_LVD_IRQHandler();
#endif /* HAL_LVD_ENABLE */

#ifdef HAL_WDT_ENABLE
    if (HRF->WDT0RF)
        return HAL_WDT_IRQHandler();
    /* WDT1 is tick timer, reset disable */
#endif /* HAL_WDT_ENABLE */

#ifdef HAL_VTS_ENABLE
    if (HRF->VTSAF)
        return HAL_VTS_IRQHandler();
#endif /* HAL_VTS_ENABLE */

    return HAL_ERROR;

}