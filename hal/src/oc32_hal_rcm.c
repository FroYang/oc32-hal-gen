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

    /* if PLL, switch from PLL to IHOSC */
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
    CLKCON = WRITE_SR(0x00001F01U);
    PLLCON = WRITE_SR(0x00000028U);
    BPCON = WRITE_SR(HAL_GetREGTRIM0w0());
    IOSCCON = WRITE_SR(HAL_GetREGTRIM0w1());
    VTSCON = WRITE_SR(HAL_GetREGTRIM0w2());
    LVDCON = WRITE_SR(HAL_GetREGTRIM0w3());

    SystemClock = OC32_IHOSC_FREQ * 1000000U;

    /* update flash clock setting base on new system clock */
    HAL_FLASH_ClkUpdate();
    return HAL_OK;
}

/**
 * @brief  Initializes the RCM clock according to the specified parameters.
 * @param  Init: pointer to an HAL_RCMClkInitTypeDef structure that contains
 *         the configuration information for the RCM clock.
 * @retval HAL status
 */
__sram__ HAL_StatusTypeDef HAL_RCM_Init(HAL_RCMClkInitTypeDef *Init)
{
    /* Check Null pointer */
    if (Init == NULL)
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

    /* XHOSC config */
    if ((Init->OscType == RCM_OSCTYPE_XHOSC) && !CLKCON->XHOSCE)
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
    }
    /* IOSC config */
    else if ((Init->OscType == RCM_OSCTYPE_IOSC) && CLKCON->XHOSCE)
    {
        CLKCON->IOSCH = RCM_IOSCSPEED_HIGH & Init->IoscHigh;
        CLKCON->XHOSCE = 0;
        /* Wait till clock switch is ready */
        tickstart = HAL_SYS_GetTick();
        while (CLKCON->CSF == 0)
        {
            if ((HAL_SYS_GetTick() - tickstart) > RCM_CLOCKSWITCH_TIMEOUT)
            {
                return HAL_TIMEOUT;
            }
        }
    }
    else
    {
        return HAL_ERROR;
    }

    /* PLL config */
    if ((Init->PllState == RCM_PLL_ON) && !CLKCON->PLLE)
    {
#ifdef HAL_RCM_CDR_ENABLE
        CLKCON->UXCDRE = Init->PllRef & RCM_PLLREF_CDR;
#endif

#ifdef HAL_USB_ENABLE
        CLKCON->IOSCUTE = Init->CdrTrim & RCM_USBTRIM_ON;
#endif

        if ((Init->PllFD > 7u) || (Init->PllFM > 255u))
        {
            return HAL_ERROR;
        }

        /* Calculate PLL output frequency and validate against max system clock */
        uint32_t pll_input_freq = (CLKCON->XHOSCE)
            ? (OC32_XHOSC_FREQ * 1000000U)
            : (OC32_IHOSC_FREQ * 1000000U);
        uint32_t pll_out_freq   = (pll_input_freq >> Init->PllFD) * Init->PllFM;
        if (pll_out_freq > (OC32_SCLK_FREQ * 1000000U))
        {
            return HAL_ERROR;
        }
        CLKCON->PLLM = Init->PllMode & RCM_PLLMODE_FASTLOCK;
        CLKCON->PFD = Init->PllFD;
        CLKCON->PFM = Init->PllFM;
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
    }
    else if ((Init->PllState == RCM_PLL_OFF) && CLKCON->PLLE)
    {
        CLKCON->PLLE = 0;
        /* Wait till clock switch is ready */
        tickstart = HAL_SYS_GetTick();
        while (CLKCON->CSF == 0)
        {
            if ((HAL_SYS_GetTick() - tickstart) > RCM_CLOCKSWITCH_TIMEOUT)
            {
                return HAL_TIMEOUT;
            }
        }
    }

    /* update system clock */
    SystemClock = CLKCON->PLLE ? pll_out_freq : 
                                 ((CLKCON->XHOSCE ? OC32_XHOSC_FREQ : 
                                                    OC32_IHOSC_FREQ) * 1000000U);

    /* update flash clock setting base on new system clock */
    HAL_FLASH_ClkUpdate();
    return HAL_OK;
}


#ifdef HAL_USB_ENABLE
/**
 * @brief  Enable IOSC trim value lock
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_RCM_TrimLockOn(void)
{
  if (CLKCON->IOSCUTL == 0)
  {
    CLKCON->IOSCUTL = 1u;
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }
}

/**
 * @brief  Disable IOSC trim value lock
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_RCM_TrimLockOff(void)
{
  if (CLKCON->IOSCUTL == 1)
  {
    CLKCON->IOSCUTL = 0;
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }
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