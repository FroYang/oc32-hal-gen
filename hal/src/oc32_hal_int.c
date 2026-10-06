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

/* EIA backup, one slot per interrupt vector (0~31).
 * A given vector can only be active once at a time (HIC only allows a higher
 * priority source to preempt), so per-vector storage is safe. */
static volatile uint32_t g_hal_int_eia[32];

/* Read EIA (Exception Instruction Address = shadow PC / return address).
 * EIA is a special register; only accessible via the MOV special-reg form. */
static inline uint32_t hal_int_get_eia(void)
{
    uint32_t eia;
    __asm__ __volatile__("MOV %0, EIA" : "=r"(eia));
    return eia;
}

/* Write EIA through R1 (R1 is the alias of EIA).
 * RETE fetches its return address from EIA. */
static inline void hal_int_set_eia(uint32_t val)
{
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(val));
}

/**
 * @brief  Enable interrupt preemption for the given vector.
 *         Saves EIA and sets the corresponding HIPRM bit.
 * @param  irq  Interrupt vector number (0~31). HIPRM bit N maps to vector N.
 */
void HAL_IntPreemptOn(uint32_t irq)
{
    if (irq < 32U)
    {
        g_hal_int_eia[irq] = hal_int_get_eia();
        WRITE_SR(HIPRM, READ_SR(HIPRM) | (1UL << irq));
    }
}

/**
 * @brief  Disable interrupt preemption for the given vector.
 *         Clears the corresponding HIPRM bit and restores EIA.
 *         This must be the last action before the handler returns, so that
 *         EIA holds the correct return address when RETE executes.
 * @param  irq  Interrupt vector number (0~31).
 */
void HAL_IntPreemptOff(uint32_t irq)
{
    if (irq < 32U)
    {
        WRITE_SR(HIPRM, READ_SR(HIPRM) & ~(1UL << irq));
        hal_int_set_eia(g_hal_int_eia[irq]);
    }
}

/* =============================================================================
 * C form interrupt handlers
 * ".stext" section means that the code locates in SRAM part
 *
 * Each handler follows the preemption pattern:
 *   HAL_IntPreemptOn(N);   // save EIA, set HIPRM bit N
 *   ... user interrupt code ...
 *   HAL_IntPreemptOff(N);  // clear HIPRM bit N, restore EIA
 *   return 0;
 *
 * Comment out the calling if you dont want the preemption feature
 * ===========================================================================*/

/**
 * @brief  Handles HRF(Hardware Reset Flags), LVD and PLL interrupt request.
 * @note   These interrupts mean that some kind of error occurs, thus it
 *         should not be preempted.
 */
__sram__ int __int0()
{

    /* power-on reset flag */
    if (HRF->PORF)
        HAL_POR_IRQHandler();

    /* debug reset flag */
    if (HRF->DBGRF)
        HAL_DBGR_IRQHandler();

    /* lvd reset flag or lvd event */
#ifdef HAL_LVD_ENABLE
    if (HRF->LVDRF || LVDCON->LVDEF)
        HAL_LVD_IRQHandler();
#endif /* HAL_LVD_ENABLE */

    /* pll frequency over */
    if (PLLCON->PFO)
        HAL_PLL_IRQHandler();

    return 0;
}

__sram__ int __int1()
{
    HAL_IntPreemptOn(1);

    HAL_IntPreemptOff(1);
    return 0;
}

__sram__ int __int2()
{
    HAL_IntPreemptOn(2);

    HAL_IntPreemptOff(2);
    return 0;
}

__sram__ int __int3()
{
    HAL_IntPreemptOn(3);

    HAL_IntPreemptOff(3);
    return 0;
}

__sram__ int __int4()
{
    HAL_IntPreemptOn(4);

    HAL_IntPreemptOff(4);
    return 0;
}

__sram__ int __int5()
{
    HAL_IntPreemptOn(5);

    HAL_IntPreemptOff(5);
    return 0;
}

__sram__ int __int6()
{
    HAL_IntPreemptOn(6);

    HAL_IntPreemptOff(6);
    return 0;
}

__sram__ int __int7()
{
    HAL_IntPreemptOn(7);

    HAL_IntPreemptOff(7);
    return 0;
}

__sram__ int __int8()
{
    HAL_IntPreemptOn(8);

    HAL_IntPreemptOff(8);
    return 0;
}

__sram__ int __int9()
{
    HAL_IntPreemptOn(9);

    HAL_IntPreemptOff(9);
    return 0;
}

__sram__ int __int10()
{
    HAL_IntPreemptOn(10);

    HAL_IntPreemptOff(10);
    return 0;
}

__sram__ int __int11()
{
    HAL_IntPreemptOn(11);

    HAL_IntPreemptOff(11);
    return 0;
}

__sram__ int __int12()
{
    HAL_IntPreemptOn(12);

    HAL_IntPreemptOff(12);
    return 0;
}

__sram__ int __int13()
{
    HAL_IntPreemptOn(13);

    HAL_IntPreemptOff(13);
    return 0;
}

__sram__ int __int14()
{
    HAL_IntPreemptOn(14);

    HAL_IntPreemptOff(14);
    return 0;
}

__sram__ int __int15()
{
    HAL_IntPreemptOn(15);

    HAL_IntPreemptOff(15);
    return 0;
}

__sram__ int __int16()
{
    HAL_IntPreemptOn(16);

    HAL_IntPreemptOff(16);
    return 0;
}

/**
 * @brief  Handles WDT interrupt request.
 */
__sram__ int __int17()
{
    HAL_IntPreemptOn(17);

#ifdef HAL_WDT_ENABLE
    if (WDT0CON->WDTE && WDT0CON->WDTIE && WDT0CON->WDTTO)
    {
        HAL_WDT_IRQHandler();
    }
#endif /* HAL_WDT_ENABLE */

    /* reload the counter and inc tick */
    if (WDT1CON->WDTE && WDT1CON->WDTIE && WDT1CON->WDTTO)
    {
        HAL_WDT_Clear(WDT1CON);
        HAL_SYS_IncTick();
    }

    HAL_IntPreemptOff(17);
    return 0;
}

__sram__ int __int18()
{
    HAL_IntPreemptOn(18);

    HAL_IntPreemptOff(18);
    return 0;
}

__sram__ int __int19()
{
    HAL_IntPreemptOn(19);

    HAL_IntPreemptOff(19);
    return 0;
}

__sram__ int __int20()
{
    HAL_IntPreemptOn(20);

    HAL_IntPreemptOff(20);
    return 0;
}

__sram__ int __int21()
{
    HAL_IntPreemptOn(21);

    HAL_IntPreemptOff(21);
    return 0;
}

__sram__ int __int22()
{
    HAL_IntPreemptOn(22);

    HAL_IntPreemptOff(22);
    return 0;
}

__sram__ int __int23()
{
    HAL_IntPreemptOn(23);

    HAL_IntPreemptOff(23);
    return 0;
}

__sram__ int __int24()
{
    HAL_IntPreemptOn(24);

    HAL_IntPreemptOff(24);
    return 0;
}

__sram__ int __int25()
{
    HAL_IntPreemptOn(25);

    HAL_IntPreemptOff(25);
    return 0;
}

__sram__ int __int26()
{
    HAL_IntPreemptOn(26);

    HAL_IntPreemptOff(26);
    return 0;
}

__sram__ int __int27()
{
    HAL_IntPreemptOn(27);

    HAL_IntPreemptOff(27);
    return 0;
}

__sram__ int __int28()
{
    HAL_IntPreemptOn(28);

    HAL_IntPreemptOff(28);
    return 0;
}

/**
 * @brief  Handles Flash(always off in HAL) and CRC interrupt request.
 */
__sram__ int __int29()
{
    HAL_IntPreemptOn(29);

#ifdef HAL_CRC_ENABLE
    if (CRCCON->CRCCF)
        HAL_CRC_IRQHandler();
#endif /* HAL_CRC_ENABLE */

    HAL_IntPreemptOff(29);
    return 0;
}

__sram__ int __int30()
{
    HAL_IntPreemptOn(30);

    HAL_IntPreemptOff(30);
    return 0;
}

__sram__ int __int31()
{
    HAL_IntPreemptOn(31);

    HAL_IntPreemptOff(31);
    return 0;
}
