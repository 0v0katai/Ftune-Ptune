#include <gint/clock.h>
#include <gint/timer.h>
#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>

#include "settings.h"
#include "validate.h"
#include "bsc.h"
#include "config.h"

#define CS2WCR_DEFAULT WAIT_2
#define CS0WCR_DEFAULT WAIT_3

extern i32 roR[];

void init_roR()
{
    for (int i = WAIT_0; i < WAIT_24; i++)
        roR[i] = roR[i] * (100ull - ROM_MARGIN) / 100;
}

bool exceed_limit()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    return (freq->FLL * freq->PLL * 32768 > PLL_CLK_MAX) ||
           (freq->Iphi_f > CPU_CLK_MAX) || (freq->Sphi_f > SHW_CLK_MAX) ||
#if defined CG50 || defined CG100
           (freq->Bphi_f > BUS_CLK_MAX(BSC.CS3WCR.TRC)) ||
#else
            (freq->Bphi_f > BUS_CLK_MAX) ||
#endif
           (freq->Pphi_f > IO_CLK_MAX && freq->Pphi_div == 64);
}

unsigned int best_rom_wait(i32 Bphi_f)
{
    int i;
    for (i = WAIT_18; i >= WAIT_0; i--)
        if (Bphi_f >= roR[i])
            break;
    return i + 1;
}

bool auto_up_PFC()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    if (freq->Pphi_f << 1 >= IO_CLK_MAX || freq->Pphi_div == freq->Bphi_div)
        return false;
    return true;
}

bool auto_down_PFC()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    if (freq->Pphi_f < IO_CLK_MAX || freq->Pphi_div == 64)
        return false;
    return true;
}

void modify_A3CL(u8 value)
{
    if (value != CL2 && value != CL3)
        return;
    BSC.CS3WCR.A3CL = value;
    if (value == 1)
        *SH7305_SDMR3_CL2 = 0;
    else
        *SH7305_SDMR3_CL3 = 0;
}