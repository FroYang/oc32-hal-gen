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
    __asm__ __volatile__("OR R1, R0, %0" : : "r"(val) : "r1");
}

/**
 * @brief  Enable interrupt preemption for the given vector.
 *         Saves EIA and sets the corresponding HIPRM bit.
 * @param  irq  Interrupt vector number (0~31). HIPRM bit N maps to vector N.
 */
void HAL_IntEnablePreemption(uint32_t irq)
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
void HAL_IntDisablePreemption(uint32_t irq)
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
 *   HAL_IntEnablePreemption(N);   // save EIA, set HIPRM bit N
 *   ... user interrupt code ...
 *   HAL_IntDisablePreemption(N);  // clear HIPRM bit N, restore EIA
 *   return 0;
 * 
 * Comment out the calling if you dont want the preemption feature
 * ===========================================================================*/


/* Reset Flags, PLL, LVD Interrupt */
__sram__  __weak__ int __int0()
{
    HAL_IntEnablePreemption(0);

    HAL_IntDisablePreemption(0);
    return 0;
}


__sram__  __weak__ int __int1()
{
    HAL_IntEnablePreemption(1);

    HAL_IntDisablePreemption(1);
    return 0;
}

__sram__  __weak__ int __int2()
{
    HAL_IntEnablePreemption(2);

    HAL_IntDisablePreemption(2);
    return 0;
}

__sram__  __weak__ int __int3()
{
    HAL_IntEnablePreemption(3);

    HAL_IntDisablePreemption(3);
    return 0;
}

__sram__  __weak__ int __int4()
{
    HAL_IntEnablePreemption(4);

    HAL_IntDisablePreemption(4);
    return 0;
}

__sram__  __weak__ int __int5()
{
    HAL_IntEnablePreemption(5);

    HAL_IntDisablePreemption(5);
    return 0;
}

__sram__  __weak__ int __int6()
{
    HAL_IntEnablePreemption(6);

    HAL_IntDisablePreemption(6);
    return 0;
}

__sram__  __weak__ int __int7()
{
    HAL_IntEnablePreemption(7);

    HAL_IntDisablePreemption(7);
    return 0;
}

__sram__  __weak__ int __int8()
{
    HAL_IntEnablePreemption(8);

    HAL_IntDisablePreemption(8);
    return 0;
}


__sram__  __weak__ int __int9()
{
    HAL_IntEnablePreemption(9);

    HAL_IntDisablePreemption(9);
    return 0;
}

__sram__  __weak__ int __int10()
{
    HAL_IntEnablePreemption(10);

    HAL_IntDisablePreemption(10);
    return 0;
}

__sram__  __weak__ int __int11()
{
    HAL_IntEnablePreemption(11);

    HAL_IntDisablePreemption(11);
    return 0;
}

__sram__  __weak__ int __int12()
{
    HAL_IntEnablePreemption(12);

    HAL_IntDisablePreemption(12);
    return 0;
}

__sram__  __weak__ int __int13()
{
    HAL_IntEnablePreemption(13);

    HAL_IntDisablePreemption(13);
    return 0;
}

__sram__  __weak__ int __int14()
{
    HAL_IntEnablePreemption(14);

    HAL_IntDisablePreemption(14);
    return 0;
}

__sram__  __weak__ int __int15()
{
    HAL_IntEnablePreemption(15);

    HAL_IntDisablePreemption(15);
    return 0;
}

__sram__  __weak__ int __int16()
{
    HAL_IntEnablePreemption(16);

    HAL_IntDisablePreemption(16);
    return 0;
}

__sram__  __weak__ int __int17()
{
    HAL_IntEnablePreemption(17);

    HAL_IntDisablePreemption(17);
    return 0;
}


__sram__  __weak__ int __int18()
{
    HAL_IntEnablePreemption(18);

    HAL_IntDisablePreemption(18);
    return 0;
}

__sram__  __weak__ int __int19()
{
    HAL_IntEnablePreemption(19);

    HAL_IntDisablePreemption(19);
    return 0;
}

__sram__  __weak__ int __int20()
{
    HAL_IntEnablePreemption(20);

    HAL_IntDisablePreemption(20);
    return 0;
}

__sram__  __weak__ int __int21()
{
    HAL_IntEnablePreemption(21);

    HAL_IntDisablePreemption(21);
    return 0;
}

__sram__  __weak__ int __int22()
{
    HAL_IntEnablePreemption(22);

    HAL_IntDisablePreemption(22);
    return 0;
}

__sram__  __weak__ int __int23()
{
    HAL_IntEnablePreemption(23);

    HAL_IntDisablePreemption(23);
    return 0;
}

__sram__  __weak__ int __int24()
{
    HAL_IntEnablePreemption(24);

    HAL_IntDisablePreemption(24);
    return 0;
}

__sram__  __weak__ int __int25()
{
    HAL_IntEnablePreemption(25);

    HAL_IntDisablePreemption(25);
    return 0;
}

__sram__  __weak__ int __int26()
{
    HAL_IntEnablePreemption(26);

    HAL_IntDisablePreemption(26);
    return 0;
}

__sram__  __weak__ int __int27()
{
    HAL_IntEnablePreemption(27);

    HAL_IntDisablePreemption(27);
    return 0;
}

__sram__  __weak__ int __int28()
{
    HAL_IntEnablePreemption(28);

    HAL_IntDisablePreemption(28);
    return 0;
}

__sram__  __weak__ int __int29()
{
    HAL_IntEnablePreemption(29);

    HAL_IntDisablePreemption(29);
    return 0;
}

__sram__  __weak__ int __int30()
{
    HAL_IntEnablePreemption(30);

    HAL_IntDisablePreemption(30);
    return 0;
}

__sram__  __weak__ int __int31()
{
    HAL_IntEnablePreemption(31);

    HAL_IntDisablePreemption(31);
    return 0;
}
