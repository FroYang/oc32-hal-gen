/**
 ******************************************************************************
 * @file    oc32_hal_wdt.c
 * @author
 * @brief   OC32 HAL WDT (Watchdog Timer) module driver.
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
 * @brief  Initializes the WDT peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_WDT_InitTypeDef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL, Wdt is not WDT_0/WDT_1,
 *                    Period is out of range, or no valid prescaler found
 * @note   Computes the overflow period in cycles, selects the smallest prescaler
 *         that yields a valid 8-bit reload value, configures the action mode
 *         (reset / interrupt / wake), then enables the watchdog (WDTE=1).
 */
HAL_StatusTypeDef HAL_WDT_Init(HAL_WDT_InitTypeDef *Init)
{
  if (Init == NULL || (Init->Wdt != WDT_0 && Init->Wdt != WDT_1))
    return HAL_ERROR;

  uint32_t wdtclkperiod;

/* if ILOSC is real, use it as the clock always */
#ifndef OC32_CLK_ILOSC_REAL
  if (CLKCON->IOSCH)
  {
    /* 24M / 4 = 6MHz clock */
    wdtclkperiod = 1000000000U / (OC32_IHOSC_FREQ / 4);
    if ((Init->Period < WDT_H_PERIOD_MIN) || (Init->Period > WDT_H_PERIOD_MAX))
      return HAL_ERROR;
  }
  else
#endif
  {
    /* 4K / 4 = 1KHz clock */
    wdtclkperiod = 1000000000U / (OC32_ILOSC_FREQ / 4);
    if ((Init->Period < WDT_L_PERIOD_MIN) || (Init->Period > WDT_L_PERIOD_MAX))
      return HAL_ERROR;
  }

  /* calculates period in ns */
  uint32_t wdtovperiod = Init->Period * 1000U;

  /* calculates cycles */
  uint32_t wdtcycles = wdtovperiod / wdtclkperiod;

  /* WDT reload counter width: 8-bit (WDTRLV = 0x00~0xFF, max count = 256) */
  static const uint32_t prescaler[] = {32u, 256u, 2048u, 16384u, 32768u, 65536u, 131072u, 262144u};

  uint32_t counter = 0;
  uint32_t i;
  /* Select the smallest prescaler that yields a valid 8-bit reload value
     Iterate from fastest (smallest prescaler) to slowest. */
  for (i = 0; i < sizeof(prescaler) / sizeof(prescaler[0]); i++)
  {
    counter = wdtcycles / prescaler[i];
    if (counter <= 256)
    {

      Init->Wdt->WDTCON.WDTE = 0;
      Init->Wdt->WDTCON.WDTCD = i;
      Init->Wdt->WDTCON.WDTRLV = counter - 1U;

      break;
    }
  }

  if (counter > 256)
    return HAL_ERROR;

  if (Init->Mode == WDT_MODE_RST)
  {

    Init->Wdt->WDTCON.WDTRE = HAL_ENABLE;
  }
  else
  {
    if ((Init->Mode == WDT_MODE_INT) || (Init->Mode == WDT_MODE_INT_AND_WAKE))
    {
      Init->Wdt->WDTCON.WDTIE = HAL_ENABLE;
    }

    if ((Init->Mode == WDT_MODE_WAKE) || (Init->Mode == WDT_MODE_INT_AND_WAKE))
    {
      Init->Wdt->WDTCON.WDTWE = HAL_ENABLE;
    }
  }

  /* enable the timer */
  Init->Wdt->WDTCON.WDTE = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Refreshes the WDT.
 * @param  Wdt: pointer to a MODULE_WDT_TypeDef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Wdt is not WDT_0/WDT_1
 * @note   Writes the WDT clear key (WDTCLR) and waits for the timeout flag (WDTTO)
 *         to clear before returning.
 */
__sram__ HAL_StatusTypeDef HAL_WDT_Clear(MODULE_WDT_TypeDef *Wdt)
{
  if (Wdt != WDT_0 && Wdt != WDT_1)
    return HAL_ERROR;

  Wdt->WDTCON.WDTCLR = WDT_CLR_KEY;
  while (Wdt->WDTCON.WDTTO == HAL_FLAG_VALID)
    ;
  return HAL_OK;
}

/**
 * @brief  DeInitializes the WDT peripheral.
 * @param  Wdt: pointer to a MODULE_WDT_TypeDef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Wdt is not WDT_0/WDT_1
 * @note   Disables reset/wake/interrupt actions, refreshes the watchdog,
 *         then disables it (WDTE=0).
 */
HAL_StatusTypeDef HAL_WDT_DeInit(MODULE_WDT_TypeDef *Wdt)
{
  if (Wdt != WDT_0 && Wdt != WDT_1)
    return HAL_ERROR;

  Wdt->WDTCON.WDTRE = HAL_DISABLE;
  Wdt->WDTCON.WDTWE = HAL_DISABLE;
  Wdt->WDTCON.WDTIE = HAL_DISABLE;
  HAL_WDT_Clear(Wdt);
  Wdt->WDTCON.WDTE = HAL_DISABLE;
  return HAL_OK;
}

/**
 * @brief  Handles WDT interrupt request at user side.
 * @param  Wdt: pointer to a MODULE_WDT_TypeDef structure.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ HAL_StatusTypeDef HAL_WDT_UserIRQ(MODULE_WDT_TypeDef *Wdt)
{
  return HAL_OK;
}

/**
 * @brief  Handles WDT interrupt request.
 * @param  Wdt: pointer to a MODULE_WDT_TypeDef structure.
 * @retval Return value of HAL_WDT_UserIRQ
 * @note   Invoked from the global interrupt handler. Refreshes the watchdog,
 *         then calls HAL_WDT_UserIRQ.
 */
HAL_StatusTypeDef HAL_WDT_IRQHandler(MODULE_WDT_TypeDef *Wdt)
{
  HAL_WDT_Clear(Wdt);
  return HAL_WDT_UserIRQ(Wdt);
}