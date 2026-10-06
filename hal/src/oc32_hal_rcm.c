/**
 ******************************************************************************
 * @file    oc32_hal_rcm.c
 * @author
 * @brief   OC32 HAL RCM(Reset and Clock Manager) module driver.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "oc32_hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Resets the RCM clock configuration to the default reset state.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_RCM_DeInit(void)
{
    /* config wait timer(biggest number) for clock switching */
    CLKCON->CSWT = 0x7FU;

    /* if PLL, switch from PLL to OSC */
    if (CLKCON->PLLE)
    {
        CLKCON->PLLE = 0;
        /* Wait till clock switch is ready */
        uint32_t tickstart = HAL_SYS_GetTick();
        while (CLKCON->CSF == 0)
        {
            if ((HAL_SYS_GetTick() - tickstart) > RCM_CLOCKSWITCH_TIMEOUT)
            {
                return HAL_TIMEOUT;
            }
        }
    }

    /* if XHOSC, switch from XHOSC to IHOSC */
    if (CLKCON->XHOSCE)
    {
        CLKCON->XHOSCE = 0;
        /* Wait till clock switch is ready */
        uint32_t tickstart = HAL_SYS_GetTick();
        while (CLKCON->CSF == 0)
        {
            if ((HAL_SYS_GetTick() - tickstart) > RCM_CLOCKSWITCH_TIMEOUT)
            {
                return HAL_TIMEOUT;
            }
        }
    }

    /* default value */
    WRITE_SR(CLKCON, 0x00001F01U);
    WRITE_SR(PLLCON, 0x00000028U);
    WRITE_SR(BPCON, HAL_GetREGTRIM0w0());
    WRITE_SR(IOSCCON, HAL_GetREGTRIM0w1());
    WRITE_SR(VTSCON, HAL_GetREGTRIM0w2());
    WRITE_SR(LVDCON, HAL_GetREGTRIM0w3());

    SystemClock = OC32_IHOSC_FREQ;

    /* update flash clock setting base on new system clock */
    HAL_FLASH_ClkUpdate();

    return HAL_OK;
}

/**
 * @brief  Initializes the RCM clock according to the specified parameters.
 * @param  Init: pointer to an HAL_RCMInitTypeDef structure that contains
 *         the configuration information for the RCM clock.
 * @note   DeInit should be used before re-Initialize
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_RCM_Init(HAL_RCMInitTypeDef *Init)
{
    /* Check Null pointer */
    if ((Init == NULL) || (Init->OutFreq > OC32_SCLK_FREQ))
    {
        return HAL_ERROR;
    }

    /* flash clock config: slow down the clock for safe */
    FLCCON->FLCC = FLASH_CLK_DIV5;
    FLUCON->FLUC = FLASH_CLK_DIV5;
    FLCCON->FLTSHSL = FLASH_CORE_TSHSL_MAX;

    /* config wait timer(biggest number) for clock switching */
    CLKCON->CSWT = 0x7FU;
    uint32_t tickstart;
    uint32_t reffreq = 0;

#ifdef HAL_RCM_CDR_ENABLE
    CLKCON->UXCDRE = 1U;
    reffreq = OC32_CDR_FREQ;
    /* skip osc cfg, cdr always need ihosc(default) */
    goto pll_cfg;
#endif

osc_cfg:
    /* XHOSC config */
    if (Init->OscType == RCM_OSC_XHOSC)
    {
        CLKCON->XHOSCE = 1u;
        /* Wait till clock switch is ready */
        tickstart = HAL_SYS_GetTick();
        while (CLKCON->CSF == 0)
        {
            if ((HAL_SYS_GetTick() - tickstart) > RCM_CLOCKSWITCH_TIMEOUT)
            {
                return HAL_TIMEOUT;
            }
        }
        reffreq = OC32_XHOSC_FREQ;
    }
    /* IOSC config */
    else if (Init->OscType == RCM_OSC_ILOSC)
    {
        /* output should be same as low freq clock */
        if (Init->OutFreq != OC32_ILOSC_FREQ)
            return HAL_ERROR;

        CLKCON->IOSCH = 0;
        reffreq = OC32_ILOSC_FREQ;
    }
    else if (Init->OscType == RCM_OSC_IHOSC)
    {
        CLKCON->IOSCH = 1U;
        reffreq = OC32_ILOSC_FREQ;
    }
    else
    {
        return HAL_ERROR;
    }

    /* return if system uses osc clock directly */
    if (reffreq == Init->OutFreq)
    {
        SystemClock = Init->OutFreq;
        goto exit_ok;
    }

    /* system use pll clock */
pll_cfg:
    /* Pll div factor */
    static const uint32_t plldf[] = {1u, 2u, 4u, 8u, 16u, 32u, 63u, 128u};
    uint32_t pllfreq;

    /* Find optimal plldf and pllmf (closest to OutFreq) */
    uint32_t best_plldf_idx = 0;
    uint32_t best_pllmf = 1;
    uint32_t best_error = UINT32_MAX;
    for (uint32_t i = 0; i < sizeof(plldf) / sizeof(plldf[0]); i++)
    {
        uint32_t div_freq = reffreq / plldf[i];
        if (div_freq == 0)
            continue;
        uint32_t pllmf = (Init->OutFreq + div_freq - 1) / div_freq; /* ceil */
        if (pllmf > 255U)
            continue;
        uint32_t pllfreq = div_freq * pllmf;
        uint32_t error = (pllfreq > Init->OutFreq) ? (pllfreq - Init->OutFreq) : (Init->OutFreq - pllfreq);
        if (error < best_error)
        {
            best_error = error;
            best_plldf_idx = i;
            best_pllmf = pllmf;
        }
    }
    if (best_error == UINT32_MAX)
        return HAL_ERROR;

    CLKCON->PLLM = 0U;
    PLLCON->PFD = best_plldf_idx;
    PLLCON->PFM = best_pllmf;
    CLKCON->PLLE = 1u;
    /* Wait till clock switch is ready */
    tickstart = HAL_SYS_GetTick();
    while (CLKCON->CSF == 0)
    {
        if ((HAL_SYS_GetTick() - tickstart) > RCM_CLOCKSWITCH_TIMEOUT)
        {
            return HAL_TIMEOUT;
        }
    }

    /* update system clock */
    SystemClock = pllfreq;

exit_ok:
    /* update flash clock setting base on new system clock */
    HAL_FLASH_ClkUpdate();
    return HAL_OK;
}

#ifdef HAL_USB_ENABLE
/**
 * @brief  Enable IOSC trim by usb
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_RCM_UsbTrim(HAL_RCMOscTrimTypedef Trim)
{
    CLKCON->IOSCUTE = Trim;
    return HAL_OK;
}

/**
 * @brief  Enable IOSC trim value lock
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_RCM_UsbTrimLock(HAL_RCMOscTrimLockTypedef Lock)
{

    CLKCON->IOSCUTL = Lock;
    return HAL_OK;
}
#endif

/**
 * @brief  Gets the system clock frequency.
 * @retval The SYSCLK frequency in Hz.
 */
uint32_t HAL_RCM_GetSysClockFreq(void)
{
    return SystemClock;
}

/**
 * @brief  Handles PLL interrupt request.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ void HAL_PLL_IRQHandler(void)
{
    /* switchback to default clock */
    HAL_RCM_DeInit();

    /* clear flag */
    PLLCON->PFOCLR = 1U;
}
