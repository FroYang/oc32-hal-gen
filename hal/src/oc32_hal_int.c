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
 * @note   Intentionally NOT marked __sram__: this vector fires on physical
 *         environment faults (LVD / VTS / PLL over-frequency) where SRAM is
 *         less reliable than Flash. __asm_int0 already switches the clock
 *         back to a stable oscillator, so the handler is meant to be fetched
 *         from Flash for fast, reliable recovery. Other vectors (__int1..31)
 *         are fired by ordinary peripheral faults with Flash fetch intact,
 *         so they live in .stext (SRAM).
 */
void __int0(void)
{

    /* power-on reset flag */
    if (READ_SR(HRF))
        HAL_HRF_IRQHandler();

#ifdef HAL_LVD_ENABLE
    if ((LVDCON->LVDIE == HAL_ENABLE) && (LVDCON->LVDEF == HAL_FLAG_ERROR))
        HAL_LVD_IRQHandler();
#endif /* HAL_LVD_ENABLE */

    /* pll frequency over */
    if ((PLLCON->PFO == HAL_FLAG_ERROR))
        HAL_PLL_IRQHandler();

    return;
}

__sram__ __weak__ void __int1(void)
{
#ifdef HAL_CMP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V1 = HAL_ENABLE;

    /* handler function */

    HIPRM->V1 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CMP_ENABLE */

    return;
}

__sram__ __weak__ void __int2(void)
{
#ifdef HAL_CMP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V2 = HAL_ENABLE;

    /* handler function */

    HIPRM->V2 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CMP_ENABLE */

    return;
}

__sram__ __weak__ void __int3(void)
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V3 = HAL_ENABLE;

    /* handler function */
    HAL_CCP_IRQHandler(CCP_0);

    HIPRM->V3 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return;
}

__sram__ __weak__ void __int4(void)
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V4 = HAL_ENABLE;

    /* handler function */
    HAL_CCP_IRQHandler(CCP_1);

    HIPRM->V4 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return;
}

__sram__ __weak__ void __int5(void)
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V5 = HAL_ENABLE;

    /* handler function */
    HAL_CCP_IRQHandler(CCP_2);

    HIPRM->V5 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return;
}

__sram__ __weak__ void __int6(void)
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V6 = HAL_ENABLE;

    /* handler function */
    HAL_CCP_IRQHandler(CCP_3);

    HIPRM->V6 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return;
}

__sram__ __weak__ void __int7(void)
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V7 = HAL_ENABLE;

    /* handler function */
    HAL_CCP_IRQHandler(CCP_4);

    HIPRM->V7 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return;
}

__sram__ __weak__ void __int8(void)
{
#ifdef HAL_CCP_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V8 = HAL_ENABLE;

    /* handler function */
    HAL_CCP_IRQHandler(CCP_5);

    HIPRM->V8 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CCP_ENABLE */

    return;
}

__sram__ __weak__ void __int9(void)
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V9 = HAL_ENABLE;

    /* handler function */
    HAL_TIMER_IRQHandler(TIMER_0);

    HIPRM->V9 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return;
}

__sram__ __weak__ void __int10(void)
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V10 = HAL_ENABLE;

    /* handler function */
    HAL_TIMER_IRQHandler(TIMER_1);

    HIPRM->V10 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return;
}

__sram__ __weak__ void __int11(void)
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V11 = HAL_ENABLE;

    /* handler function */
    HAL_TIMER_IRQHandler(TIMER_2);

    HIPRM->V11 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return;
}

__sram__ __weak__ void __int12(void)
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V12 = HAL_ENABLE;

    /* handler function */
    HAL_TIMER_IRQHandler(TIMER_3);

    HIPRM->V12 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return;
}

__sram__ __weak__ void __int13(void)
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V13 = HAL_ENABLE;

    /* handler function */
    HAL_TIMER_IRQHandler(TIMER_4);

    HIPRM->V13 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return;
}

__sram__ __weak__ void __int14(void)
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V14 = HAL_ENABLE;

    /* handler function */
    HAL_TIMER_IRQHandler(TIMER_5);

    HIPRM->V14 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return;
}

__sram__ __weak__ void __int15(void)
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V15 = HAL_ENABLE;

    /* handler function */
    HAL_TIMER_IRQHandler(TIMER_6);

    HIPRM->V15 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return;
}

__sram__ __weak__ void __int16(void)
{
#ifdef HAL_TIMER_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V16 = HAL_ENABLE;

    /* handler function */
    HAL_TIMER_IRQHandler(TIMER_7);

    HIPRM->V16 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_TIMER_ENABLE */

    return;
}

/**
 * @brief  Handles WDT interrupt request.
 */
__sram__ __weak__ void __int17(void)
{
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V17 = HAL_ENABLE;

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

    HIPRM->V17 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
    return;
}

__sram__ __weak__ void __int18(void)
{
#ifdef HAL_ADC_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V18 = HAL_ENABLE;

    /* handler function */

    HIPRM->V18 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_ADC_ENABLE */

    return;
}

__sram__ __weak__ void __int19(void)
{
#ifdef HAL_ADC_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V19 = HAL_ENABLE;

    /* handler function */

    HIPRM->V19 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_ADC_ENABLE */

    return;
}

__sram__ __weak__ void __int20(void)
{
#ifdef HAL_UART_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V20 = HAL_ENABLE;

    /* handler function */
    HAL_UART_IRQHandler(UART_0);

    HIPRM->V20 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_UART_ENABLE */

    return;
}

__sram__ __weak__ void __int21(void)
{
#ifdef HAL_UART_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V21 = HAL_ENABLE;

    /* handler function */
    HAL_UART_IRQHandler(UART_1);

    HIPRM->V21 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_UART_ENABLE */

    return;
}

__sram__ __weak__ void __int22(void)
{
#ifdef HAL_SPI_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V22 = HAL_ENABLE;

    /* handler function */
    HAL_SPI_IRQHandler(SPI_0);

    HIPRM->V22 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_SPI_ENABLE */

    return;
}

__sram__ __weak__ void __int23(void)
{
#ifdef HAL_SPI_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V23 = HAL_ENABLE;

    /* handler function */
    HAL_SPI_IRQHandler(SPI_1);

    HIPRM->V23 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_SPI_ENABLE */

    return;
}

__sram__ __weak__ void __int24(void)
{
#ifdef HAL_IIC_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V24 = HAL_ENABLE;

    /* handler function */
    HAL_IIC_IRQHandler();

    HIPRM->V24 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_IIC_ENABLE */

    return;
}

__sram__ __weak__ void __int25(void)
{
#ifdef HAL_MPWM_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V25 = HAL_ENABLE;

    /* handler function */

    HIPRM->V25 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_MPWM_ENABLE */

    return;
}

__sram__ __weak__ void __int26(void)
{
#ifdef HAL_MPWM_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V26 = HAL_ENABLE;

    /* handler function */

    HIPRM->V26 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_MPWM_ENABLE */

    return;
}

__sram__ __weak__ void __int27(void)
{
#ifdef HAL_MPWM_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V27 = HAL_ENABLE;

    /* handler function */

    HIPRM->V27 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_MPWM_ENABLE */

    return;
}

__sram__ __weak__ void __int28(void)
{
#ifdef HAL_DAC_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V28 = HAL_ENABLE;

    /* handler function */

    HIPRM->V28 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_DAC_ENABLE */

    return;
}

/**
 * @brief  Handles Flash(always off in HAL) and CRC interrupt request.
 */
__sram__ __weak__ void __int29(void)
{
#ifdef HAL_CRC_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V29 = HAL_ENABLE;

    /* handler function */
    HAL_CRC_IRQHandler();

    HIPRM->V29 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_CRC_ENABLE */

    return;
}

__sram__ __weak__ void __int30(void)
{
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V30 = HAL_ENABLE;

    /* handler function */

    HIPRM->V30 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
    return;
}

__sram__ __weak__ void __int31(void)
{
#ifdef HAL_PORTB_ENABLE
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    HIPRM->V31 = HAL_ENABLE;

    /* handler function */

    HIPRM->V31 = HAL_DISABLE;
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(eia));
#endif /* HAL_PORTB_ENABLE */

    return;
}
