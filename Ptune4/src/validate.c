#include <gint/clock.h>
#include <gint/timer.h>
#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>

#include "settings.h"
#include "validate.h"

#define CS2WCR_DEFAULT WAIT_2
#define CS0WCR_DEFAULT WAIT_3

extern i32 roR[];

void init_roR(i32 *roR)
{
    for (int i = WAIT_0; i < WAIT_24; i++)
        roR[i] = roR[i] * (100ull - ROM_MARGIN) / 100;
}

bool exceed_limit()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    return (freq->FLL * freq->PLL > PLL_CLK_MAX) ||
           (freq->Iphi_f > CPU_CLK_MAX) || (freq->Sphi_f > SHW_CLK_MAX) ||
           (freq->Bphi_f > BUS_CLK_MAX) || (freq->Pphi_f > IO_CLK_MAX && freq->Pphi_div == 64);
}

unsigned int best_rom_wait(i32 Bphi_f)
{
    int i;
    for (i = WAIT_18; i >= WAIT_0; i--)
        if (Bphi_f >= roR[i])
            break;
    return i + 1;
}

static sh7305_bsc_CSnBCR_t *CS0BCR = &BSC.CS0BCR;

void up_roR_IWW()
{
    cpg_compute_freq();
    const i32 check[3] = {roR[WAIT_2], roR[WAIT_6], roR[WAIT_12]};
    const clock_frequency_t *freq = clock_freq();
    for (int i = 0; i < 3; i++)
        if (freq->Bphi_f >= check[i] && CS0BCR->IWW == i)
            CS0BCR->IWW++;
    if (best_rom_wait(freq->Bphi_f) > BSC.CS0WCR.WR)
        BSC.CS0WCR.WR = best_rom_wait(freq->Bphi_f);
}

void down_roR_IWW()
{
    cpg_compute_freq();
    const i32 check[3] = {roR[WAIT_2], roR[WAIT_6], roR[WAIT_12]};
    const clock_frequency_t *freq = clock_freq();
    BSC.CS0WCR.WR = best_rom_wait(freq->Bphi_f);
    for (int i = 0; i < 3; i++)
        if (freq->Bphi_f < check[i] && CS0BCR->IWW == i)
            CS0BCR->IWW = i;
}

void up_BFC()
{
    cpg_compute_freq();
    const i32 check[3] = {roR[WAIT_2], roR[WAIT_6], roR[WAIT_12]};
    const clock_frequency_t *freq = clock_freq();
    if (freq->Bphi_f << 1 >= BUS_CLK_MAX)
        return;
    CPG.FRQCR.BFC--;
    for (int i = 0; i < 3; i++)
        if (freq->Bphi_f >= check[i] && CS0BCR->IWW == i)
            CS0BCR->IWW++;
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