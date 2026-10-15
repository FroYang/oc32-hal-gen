/* PLEASE DO NOT CHANGE THE FUNCTION NAME */

#include "oc32_hal.h"

/* =============================================================================
 * Interrupt Preemption Support
 *
 * Per the manual (section 8.3), by default the HIC does not generate new
 * interrupt requests while one is being serviced.  Setting the corresponding
 * HIPRM bit allows higher-priority interrupts to preempt the current one.
 *
 * The EIA register holds the return address (it is the shadow of PC; R1 is the
 * alias of EIA, so writing R1 writes EIA).  A preempting interrupt overwrites
 * EIA, so it must be saved before HIPRM is set and restored after HIPRM is
 * cleared, right before RETE.
 *
 * General-purpose register saving (R2~R7 by hardware stack, R9~R23 by
 * crti-hw.S) is handled outside of these handlers.
 * ===========================================================================*/

/**
 * @brief  Handles HRF(Hardware Reset Flags), LVD and PLL interrupt request.
 * @note   These interrupts mean that some kind of error occurs, thus it
 *         should not be preempted.
 */
int __int0()
{

    /* power-on reset flag */
    if (READ_SR(HRF))
        HAL_HRF_IRQHandler();

#ifdef HAL_LVD_ENABLE
    if (LVDCON->LVDIE && LVDCON->LVDEF)
        HAL_LVD_IRQHandler();
#endif /* HAL_LVD_ENABLE */

    /* pll frequency over */
    if (PLLCON->PFO)
        HAL_PLL_IRQHandler();

    return 0;
}

__sram__ __weak__ int __int1()
{
#ifdef HAL_CMP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V1 = 1U;

    /* handler function */

    HIPRM->V1 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CMP_ENABLE */

    return 0;
}

__sram__ __weak__ int __int2()
{
#ifdef HAL_CMP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V2 = 1U;

    /* handler function */

    HIPRM->V2 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CMP_ENABLE */

    return 0;
}

__sram__ __weak__ int __int3()
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V3 = 1U;

    /* handler function */

    HIPRM->V3 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return 0;
}

__sram__ __weak__ int __int4()
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V4 = 1U;

    /* handler function */

    HIPRM->V4 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return 0;
}

__sram__ __weak__ int __int5()
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V5 = 1U;

    /* handler function */

    HIPRM->V5 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return 0;
}

__sram__ __weak__ int __int6()
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V6 = 1U;

    /* handler function */

    HIPRM->V6 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return 0;
}

__sram__ __weak__ int __int7()
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V7 = 1U;

    /* handler function */

    HIPRM->V7 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return 0;
}

__sram__ __weak__ int __int8()
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V8 = 1U;

    /* handler function */

    HIPRM->V8 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return 0;
}

__sram__ __weak__ int __int9()
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V9 = 1U;

    /* handler function */

    HIPRM->V9 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return 0;
}

__sram__ __weak__ int __int10()
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V10 = 1U;

    /* handler function */

    HIPRM->V10 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return 0;
}

__sram__ __weak__ int __int11()
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V11 = 1U;

    /* handler function */

    HIPRM->V11 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return 0;
}

__sram__ __weak__ int __int12()
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V12 = 1U;

    /* handler function */

    HIPRM->V12 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return 0;
}

__sram__ __weak__ int __int13()
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V13 = 1U;

    /* handler function */

    HIPRM->V13 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return 0;
}

__sram__ __weak__ int __int14()
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V14 = 1U;

    /* handler function */

    HIPRM->V14 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return 0;
}

__sram__ __weak__ int __int15()
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V15 = 1U;

    /* handler function */

    HIPRM->V15 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return 0;
}

__sram__ __weak__ int __int16()
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V16 = 1U;

    /* handler function */

    HIPRM->V16 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return 0;
}

/**
 * @brief  Handles WDT interrupt request.
 */
__sram__ __weak__ int __int17()
{
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V17 = 1U;

    /* handler function */

#ifdef HAL_WDT_ENABLE
    if (WDT0CON->WDTTO)
    {
        HAL_WDT_IRQHandler(WDT_0);
    }
#endif /* HAL_WDT_ENABLE */

    /* reload the counter and inc tick */
    if (WDT1CON->WDTTO)
    {
        HAL_WDT_Clear(WDT_1);
        HAL_TICK_Inc();
    }

    HIPRM->V17 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
    return 0;
}

__sram__ __weak__ int __int18()
{
#ifdef HAL_ADC_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V18 = 1U;

    /* handler function */

    HIPRM->V18 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_ADC_ENABLE */

    return 0;
}

__sram__ __weak__ int __int19()
{
#ifdef HAL_ADC_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V19 = 1U;

    /* handler function */

    HIPRM->V19 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_ADC_ENABLE */

    return 0;
}

__sram__ __weak__ int __int20()
{
#ifdef HAL_UART_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V20 = 1U;

    /* handler function */
    HAL_UART_IRQHandler(UART_0);

    HIPRM->V20 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_UART_ENABLE */

    return 0;
}

__sram__ __weak__ int __int21()
{
#ifdef HAL_UART_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V21 = 1U;

    /* handler function */
    HAL_UART_IRQHandler(UART_1);

    HIPRM->V21 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_UART_ENABLE */

    return 0;
}

__sram__ __weak__ int __int22()
{
#ifdef HAL_SPI_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V22 = 1U;

    /* handler function */

    HIPRM->V22 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_SPI_ENABLE */

    return 0;
}

__sram__ __weak__ int __int23()
{
#ifdef HAL_SPI_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V23 = 1U;

    /* handler function */

    HIPRM->V23 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_SPI_ENABLE */

    return 0;
}

__sram__ __weak__ int __int24()
{
#ifdef HAL_IIC_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V24 = 1U;

    /* handler function */

    HIPRM->V24 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_IIC_ENABLE */

    return 0;
}

__sram__ __weak__ int __int25()
{
#ifdef HAL_MPWM_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V25 = 1U;

    /* handler function */

    HIPRM->V25 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_MPWM_ENABLE */

    return 0;
}

__sram__ __weak__ int __int26()
{
#ifdef HAL_MPWM_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V26 = 1U;

    /* handler function */

    HIPRM->V26 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_MPWM_ENABLE */

    return 0;
}

__sram__ __weak__ int __int27()
{
#ifdef HAL_MPWM_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V27 = 1U;

    /* handler function */

    HIPRM->V27 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_MPWM_ENABLE */

    return 0;
}

__sram__ __weak__ int __int28()
{
#ifdef HAL_DAC_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V28 = 1U;

    /* handler function */

    HIPRM->V28 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_DAC_ENABLE */

    return 0;
}

/**
 * @brief  Handles Flash(always off in HAL) and CRC interrupt request.
 */
__sram__ __weak__ int __int29()
{
#ifdef HAL_CRC_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V29 = 1U;

    /* handler function */
    HAL_CRC_IRQHandler();

    HIPRM->V29 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CRC_ENABLE */

    return 0;
}

__sram__ __weak__ int __int30()
{
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V30 = 1U;

    /* handler function */

    HIPRM->V30 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
    return 0;
}

__sram__ __weak__ int __int31()
{
#ifdef HAL_PORTB_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V31 = 1U;

    /* handler function */

    HIPRM->V31 = 0U;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_PORTB_ENABLE */

    return 0;
}
