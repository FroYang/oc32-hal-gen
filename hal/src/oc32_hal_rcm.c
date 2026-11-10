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
uint32_t SystemClock = OC32_IHOSC_FREQ;

/* Private function prototypes -----------------------------------------------*/

/* Wait for the clock switch to complete (CSF set).
 *
 * Uses a local busy-loop counter, NOT the system tick (uwTick): this function
 * is invoked from HAL_RCM_Init, which runs before HAL_TICK_Init has enabled
 * the WDT tick interrupt, so uwTick is still 0 and a tick-based timeout would
 * never fire (deadlock on CSF==0 when the PLL/osc switch fails).
 *
 * RCM_CSWAIT_LOOP_MAX (4 800 000) is a ~200 ms software safety net at the
 * IHOSC 24 MHz clock the system still runs on when this wait is invoked from
 * HAL_RCM_Init (before any oscillator source switch):
 *   @ 24 MHz  -> 4 800 000 loops ~= 200 ms
 * The loop reads the volatile CSF each iteration, so the constant is a
 * conservative upper bound on loop count, not an exact cycle count.
 */
static HAL_StatusTypeDef RCM_ClockSwitchWait(void)
{
    uint32_t loops = 0U;
    while (CLKCON->CSF == HAL_FLAG_INVALID)
    {
        if (++loops > RCM_CSWAIT_LOOP_MAX)
        {
            return HAL_TIMEOUT;
        }
    }
    return HAL_OK;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Resets the RCM clock configuration to the default reset state.
 * @retval HAL_OK: on success
 * @note   Writes default values to CLKCON, PLLCON, BPCON, IOSCCON, VTSCON,
 *         and LVDCON, resets SystemClock to IHOSC, and updates the flash
 *         clock divider.
 */
HAL_StatusTypeDef HAL_RCM_DeInit(void)
{
    /* default value */
    WRITE_SR(CLKCON, CLKCON_DEFAULT);
    WRITE_SR(PLLCON, PLLCON_DEFAULT);
    WRITE_SR(BPCON, HAL_GetRegTrim0w0());
    WRITE_SR(IOSCCON, HAL_GetRegTrim0w1());
    WRITE_SR(VTSCON, HAL_GetRegTrim0w2());
    WRITE_SR(LVDCON, HAL_GetRegTrim0w3());

    SystemClock = OC32_IHOSC_FREQ;

    /* update flash clock setting base on new system clock */
    HAL_FLASH_ClkUpdate();

    return HAL_OK;
}

/**
 * @brief  Initializes the RCM clock according to the configured parameters.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if clock switching times out or no valid PLL config is found
 * @note   Selects the reference oscillator (XHOSC/XLOSC/ILOSC/CDR as configured),
 *         configures the PLL if enabled, and updates the flash clock divider.
 *         On error, calls HAL_RCM_DeInit to restore the default clock state.
 */
HAL_StatusTypeDef HAL_RCM_Init(void)
{
    /* flash clock config: slow down the clock for safe */
    FLCCON->FLCC = FLASH_CLK_DIV5;
    FLUCON->FLUC = FLASH_CLK_DIV5;
    FLCCON->FLTSHSL = FLASH_CORE_TSHSL_MAX;

    /* config wait timer(biggest number) for clock switching */
    CLKCON->CSWT = CSWT_MAX;

    /* default is IHOSC */
    uint32_t reffreq = OC32_IHOSC_FREQ;
    SystemClock = OC32_IHOSC_FREQ;

#ifdef HAL_RCM_CDR_ENABLE
    CLKCON->UXCDRE = HAL_ENABLE;
    reffreq = OC32_CDR_FREQ;
    /* CDR requires the system to run on IHOSC. Skip the oscillator switch
       blocks (they would select a different source) and jump straight to
       the PLL stage. reffreq stays based on the default IHOSC. */
    goto pll_cfg;
#endif

/* Real ILOSC hardware oscillator — reserved for future chips.
   Only compiles when the part actually supports ILOSC as a real source. */
#if defined(OC32_CLK_SRC_ILOSC) && defined(OC32_CLK_ILOSC_REAL)
    CLKCON->ILOSCE = HAL_ENABLE;
    reffreq = OC32_ILOSC_FREQ;
#endif // OC32_CLK_SRC_ILOSC && OC32_CLK_ILOSC_REAL

/* reserved for future chips. */
#ifdef OC32_CLK_SRC_XLOSC
    CLKCON->XLOSCE = HAL_ENABLE;
    reffreq = OC32_XLOSC_FREQ;
#endif // OC32_CLK_SRC_XLOSC

#ifdef OC32_CLK_SRC_XHOSC
    CLKCON->XHOSCE = HAL_ENABLE;
    reffreq = OC32_XHOSC_FREQ;
#endif // OC32_CLK_SRC_XHOSC

#if defined(OC32_CLK_SRC_XHOSC) || defined(OC32_CLK_SRC_XLOSC) || defined(OC32_CLK_ILOSC_REAL)
    if (RCM_ClockSwitchWait() != HAL_OK)
        goto rcm_cfg_err;
#endif // OC32_CLK_SRC_XHOSC || OC32_CLK_SRC_XLOSC || OC32_CLK_ILOSC_REAL

/* Fake ILOSC — low-frequency mode of IHOSC (no separate oscillator).
   Slows the internal IHOSC clock; no source switch, so no CSF wait. */
#if defined(OC32_CLK_SRC_ILOSC) && defined(OC32_CLK_ILOSC_FAKE)
    CLKCON->IOSCH = 0U;
    reffreq = OC32_ILOSC_FREQ;
#endif // OC32_CLK_SRC_ILOSC && OC32_CLK_ILOSC_FAKE

/* Convergence point for the PLL stage. Reached either normally (after the
   oscillator-switch blocks above) or via `goto pll_cfg` from the CDR path
   (which skips those blocks because CDR must run on IHOSC). SystemClock is
   updated here from whichever reference the earlier blocks selected. */
/* system may use pll clock */
pll_cfg:
    SystemClock = reffreq;

#ifdef OC32_CLK_PLL_ENABLE
    /* Pll div factor */
    static const uint32_t plldf[] = {1U, 2U, 4U, 8U, 16U, 32U, 63U, 128U};
    uint32_t pllfreq = 0U; /* actual chosen PLL output frequency, set by the loop */

    /* Find optimal plldf and pllmf (closest to OutFreq) */
    uint32_t best_plldf_idx = 0U;
    uint32_t best_pllmf = 1U;
    uint32_t best_error = UINT32_MAX;
    for (uint32_t i = 0U; i < sizeof(plldf) / sizeof(plldf[0]); i++)
    {
        uint32_t div_freq = reffreq / plldf[i];
        if (div_freq == 0U)
            continue;
        uint32_t pllmf = (OC32_SCLK_FREQ + div_freq - 1U) / div_freq; /* ceil */
        if (pllmf > 255U)
            continue;
        uint32_t cand_freq = div_freq * pllmf;
        uint32_t error = (cand_freq > OC32_SCLK_FREQ) ? (cand_freq - OC32_SCLK_FREQ) : (OC32_SCLK_FREQ - cand_freq);
        if (error < best_error)
        {
            best_error = error;
            best_plldf_idx = i;
            best_pllmf = pllmf;
            pllfreq = cand_freq; /* keep outer pllfreq in sync with the winner */
        }
    }
    if (best_error == UINT32_MAX)
        goto rcm_cfg_err; /* no valid pll config found */

    /* PLL output monitor */
#ifdef OC32_CLK_PLL_OM_ENABLE
    /* PLL freq over value */
    uint32_t pfoval = (2048U * reffreq) / OC32_SCLK_FREQ;
    if (pfoval > 0U && pfoval <= PFCNT_MAX)
    {
        PLLCON->PFCNT = pfoval - pfoval / 16U; /* 15/16 expected count */
        PLLCON->PFOM = HAL_ENABLE;
    }
    else
    {
        goto rcm_cfg_err; /* invalid pfoval */
    }
#endif // OC32_CLK_PLL_OM_ENABLE

    /* PLLM = 0U is a safe default (low jitter mode). Users who need a
       different PLLM setting can override this by hand in their own copy
       of the RCM driver. */
    CLKCON->PLLM = 0U;
    PLLCON->PFD = best_plldf_idx;
    PLLCON->PFM = best_pllmf;
    CLKCON->PLLE = HAL_ENABLE;
    if (RCM_ClockSwitchWait() != HAL_OK)
        goto rcm_cfg_err;

    /* check pll freq over or not */
#ifdef OC32_CLK_PLL_OM_ENABLE
    if (PLLCON->PFO)
        goto rcm_cfg_err;
#endif // OC32_CLK_PLL_OM_ENABLE

    /* update system clock */
    SystemClock = pllfreq;
#endif // OC32_CLK_PLL_ENABLE

    /* update flash clock setting base on new system clock */
    HAL_FLASH_ClkUpdate();
    return HAL_OK;

rcm_cfg_err:
    HAL_RCM_DeInit();
    return HAL_ERROR;
}

#ifdef HAL_USB_ENABLE
/**
 * @brief  Enables the IOSC trim value controlled by USB.
 * @param  Trim: the trim enable value (HAL_ENABLE / HAL_DISABLE)
 * @retval HAL_OK: always
 */
HAL_StatusTypeDef HAL_RCM_UsbTrim(HAL_EnableTypeDef Trim)
{
    CLKCON->IOSCUTE = Trim;
    return HAL_OK;
}

/**
 * @brief  Enables or disables the IOSC trim value lock.
 * @param  Lock: the lock enable value (HAL_ENABLE / HAL_DISABLE)
 * @retval HAL_OK: always
 */
HAL_StatusTypeDef HAL_RCM_UsbTrimLock(HAL_EnableTypeDef Lock)
{

    CLKCON->IOSCUTL = Lock;
    return HAL_OK;
}
#endif

/**
 * @brief  Gets the system clock frequency.
 * @retval The current SCLK frequency in Hz (read from the SystemClock variable).
 */
uint32_t HAL_RCM_GetSysClockFreq(void)
{
    return SystemClock;
}

/**
 * @brief  Handles PLL frequency-over interrupt request at user side.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
HAL_StatusTypeDef HAL_PLL_UserIRQ(void)
{
    return HAL_OK;
}

/**
 * @brief  Handles PLL frequency-over interrupt request.
 * @retval Return value of HAL_PLL_UserIRQ
 * @note   Invoked from the global interrupt handler. Falls back to the default
 *         clock (HAL_RCM_DeInit), clears the PFO flag, then calls
 *         HAL_PLL_UserIRQ.
 */
HAL_StatusTypeDef HAL_PLL_IRQHandler(void)
{
    /* switchback to default clock */
    HAL_RCM_DeInit();

    /* clear flag */
    PLLCON->PFOCLR = HAL_ENABLE;
    return HAL_PLL_UserIRQ();
}
