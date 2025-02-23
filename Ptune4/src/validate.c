#include <gint/clock.h>
#include <gint/timer.h>
#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>

#include "settings.h"
#include "validate.h"

#define ROM_MARGIN 5
#define RAM_MARGIN 5

#define CS2WCR_DEFAULT WAIT_2
#define CS0WCR_DEFAULT WAIT_3

extern u32 roR[];

void init_roR(u32 *roR)
{
    for (int i = WAIT_0; i < WAIT_24; i++)
        roR[i] = roR[i] * (100 - ROM_MARGIN) / 100;
}

bool exceed_limit()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    return (freq->FLL * freq->PLL > PLL_CLK_MAX) ||
           (freq->Iphi_f > CPU_CLK_MAX) || (freq->Sphi_f > SHW_CLK_MAX) ||
           (freq->Bphi_f > BUS_CLK_MAX) || (freq->Pphi_f > IO_CLK_MAX);
}

unsigned int best_rom_wait(u32 Bphi_f)
{
    const u32 Bphi_10k = Bphi_f / 10000;

    int i;
    for (i = WAIT_18; i >= WAIT_0; i--)
        if (Bphi_10k >= roR[i])
            break;
    return i + 1;
}

void up_roR_IWW()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    u32 IWW_0 = BSC.CS0BCR.IWW;
    const u32 Bphi_10k = freq->Bphi_f / 10000;
    if (Bphi_10k >= roR[WAIT_2] && IWW_0 == 0)
        BSC.CS0BCR.IWW = 1;
    if (Bphi_10k >= roR[WAIT_6] && IWW_0 == 1)
        BSC.CS0BCR.IWW = 2;
    if (Bphi_10k >= roR[WAIT_12] && IWW_0 == 2)
        BSC.CS0BCR.IWW = 3;
    if (best_rom_wait(freq->Bphi_f) > BSC.CS0WCR.WR)
        BSC.CS0WCR.WR = best_rom_wait(freq->Bphi_f);
}

void down_roR_IWW()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    BSC.CS0WCR.WR = best_rom_wait(freq->Bphi_f);
    u32 IWW_0 = BSC.CS0BCR.IWW;
    const u32 Bphi_10k = freq->Bphi_f / 10000;
    if (Bphi_10k < roR[WAIT_2] && IWW_0 > 0)
        BSC.CS0BCR.IWW = 0;
    if (Bphi_10k < roR[WAIT_6] && IWW_0 > 1)
        BSC.CS0BCR.IWW = 1;
    if (Bphi_10k < roR[WAIT_12] && IWW_0 > 2)
        BSC.CS0BCR.IWW = 2;
}

void up_BFC()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    if (freq->Bphi_f << 1 >= BUS_CLK_MAX)
        return;
    CPG.FRQCR.BFC--;
    u32 IWW_0 = BSC.CS0BCR.IWW;
    const u32 Bphi_10k = freq->Bphi_f / 10000;
    if (Bphi_10k >= roR[WAIT_2] && IWW_0 == 0)
        BSC.CS0BCR.IWW = 1;
    if (Bphi_10k >= roR[WAIT_6] && IWW_0 == 1)
        BSC.CS0BCR.IWW = 2;
    if (Bphi_10k >= roR[WAIT_12] && IWW_0 == 2)
        BSC.CS0BCR.IWW = 3;
    BSC.CS0WCR.WR = best_rom_wait(freq->Bphi_f << 1);
}

void up_PFC()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    if (freq->Pphi_f << 1 >= BUS_CLK_MAX)
        return;
    CPG.FRQCR.P1FC--;
}

void auto_up_PFC()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    if (freq->Pphi_f << 1 >= IO_CLK_MAX)
        return;
    if (freq->Pphi_div == freq->Bphi_div)
        return;
    CPG.FRQCR.P1FC--;
}

void auto_down_PFC()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    if (freq->Pphi_f < IO_CLK_MAX)
        return;
    if (freq->Pphi_div == 64)
        return;
    CPG.FRQCR.P1FC++;
}