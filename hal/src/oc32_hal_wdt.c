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
 * @param  Init: pointer to a HAL_WDTInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_WDT_Init(HAL_WDTInitTypeDef *Init)
{
  if (Init == NULL || Init->WDTCON == NULL)
    return HAL_ERROR;

  uint32_t wdtclkperiod;
  if (CLKCON->IOSCH)
  {
    /* 6MHz clock */
    wdtclkperiod = 1000000000U / (OC32_IHOSC_FREQ / 6);
    if ((Init->Period < WDT_H_PERIOD_MIN) || (Init->Period > WDT_H_PERIOD_MAX))
      return HAL_ERROR;
  }
  else
  {
    wdtclkperiod = 1000000000U / (OC32_ILOSC_FREQ / 6);
    /* 1K clock */
    if ((Init->Period < WDT_L_PERIOD_MIN) || (Init->Period > WDT_L_PERIOD_MAX))
      return HAL_ERROR;
  }

  /* caculates period in ns */
  uint32_t wdtovperiod = Init->Period * 1000U;

  /* caculates cycles */
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

      Init->WDTCON->WDT1E = 0;
      Init->WDTCON->WDTCD = i;
      Init->WDTCON->WDTRLV = counter - 1U;

      break;
    }
  }

  if (Init->Mode == WDT_MODE_RST)
  {

    Init->WDTCON->WDTRE = 1U;
  }
  else
  {
    if ((Init->Mode == WDT_MODE_INT) || (Init->Mode == WDT_MODE_INT_AND_WAKE))
    {

      Init->WDTCON->WDTIE = 1U;
      HIE->WDTHIE = 1u;
      HIPL0->WDTHIPL0 = (Init->IntPri & 1u);        /* Bit 0 of priority */
      HIPL1->WDTHIPL1 = ((Init->IntPri >> 1) & 1u); /* Bit 1 of priority */
    }

    if ((Init->Mode == WDT_MODE_WAKE) || (Init->Mode == WDT_MODE_INT_AND_WAKE))
    {
      Init->WDTCON->WDTWE = 1U;
    }
  }

  /* enable the timer */
  Init->WDTCON->WDTE = 1U;

  return HAL_OK;
}

/**
 * @brief  Refreshes the WDT.
 * @param  clear: pointer to a HAL_WDTInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_WDT_Clear(HAL_WDTInitTypeDef *Clear)
{
  if (Init == NULL || Init->WDTCON == NULL)
    return HAL_ERROR;

  Clear->WDTCON->WDTCLR = WDT_CLR_KEY;
  while (Clear->WDTCON->WDTO)
    ;
  return HAL_OK;
}

/**
 * @brief  DeInitializes the WDT peripheral.
 * @param  Init: pointer to a HAL_WDTInitTypeDef structure.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_WDT_DeInit(HAL_WDTInitTypeDef *Init)
{
  if (Init == NULL || Init->WDTCON == NULL)
    return HAL_ERROR;

  Init->WDTCON->WDTRE = 0;
  Init->WDTCON->WDTWE = 0;
  Init->WDTCON->WDTIE = 0;
  HIE->WDTHIE = 0;
  HIPL0->WDTHIPL0 = 0;
  HIPL1->WDTHIPL1 = 0;
  HAL_WDT_Clear(Init);
  Init->WDTCON->WDTE = 0;
  return HAL_OK;
}

/**
 * @brief  Handles LVD interrupt request.
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ void HAL_WDT_IRQHandler(void)
{
  HAL_WDTInitTypeDef clear = {
      .WDTCON = WDT0CON};
  HAL_WDT_Clear(clear);
}