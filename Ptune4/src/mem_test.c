#include <gint/clock.h>
#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/display.h>

#include "settings.h"
#include "validate.h"
#include "mem_test.h"
#include "util.h"

#define WRITE_N 2000

static int ram_ad(int FLF, volatile u32 *RAM, int block_size)
{
    u32 *ad;
    ad = RAM;
    cpu_atomic_start();
    CPG.FLLFRQ.FLF = FLF;
    cpg_compute_freq();
    BSC.CS0WCR.WR = best_rom_wait(clock_freq()->Bphi_f * (100ull - RAM_MARGIN) / 100) + 1;
    if (*RAM == *RAM)
        ad = mem_write(RAM, &CPG.FLLFRQ.lword, FLF_x810, block_size);
    cpu_atomic_end();
    return (int)ad;
}

static int loop_write_test()
{
    u32 temp[WRITE_N+1];
    u32 *write_area = (u32 *)(((u32)&temp & 0x0FFFFFFF) | 0xA0000000);
    
    row_print(13, 1, "RAM select: 0x%08x", write_area);
    row_print(14, 1, "Write Test");

    u32 Bphi_f = 0;
    const u8 PLL = PLL_x16, IFC = DIV_4, SFC = DIV_4, BFC = DIV_4, PFC = DIV_16;
    
    CPG.FRQCR.lword = (PLL << 24) + (IFC << 20) + (SFC << 12) + (BFC << 8) + PFC;
    struct cpg_overclock_setting s;
    cpg_get_overclock_setting(&s);
    cpg_set_overclock_setting(&s);
    
    BSC.CS0WCR.WR = best_rom_wait(clock_freq()->Bphi_f) + 2;

    for (int FLF = 900; FLF < 2000; FLF += 2)
    {
        if (ram_ad(FLF, write_area, WRITE_N))
            return Bphi_f * (100ull - RAM_MARGIN) / 100;

        Bphi_f = clock_freq()->Bphi_f;
        row_clear(4);
        row_print(4, 1, "Write:");
        row_print_color(4, 8, C_BLUE, C_WHITE, "%d KHz", Bphi_f / 1000);
        dupdate();
    }
}

void sdram_test()
{
    dclear(C_WHITE);
    row_title("SDRAM Test");

    struct cpg_overclock_setting s0;
    cpg_get_overclock_setting(&s0);
    BUS_CLK_MAX = loop_write_test();
    cpg_set_overclock_setting(&s0);

    row_print(10, 1, "Max bus frequency set: %d KHz", BUS_CLK_MAX / 1000);
    row_print(11, 1, "Press any key to exit...");

    dupdate();
    getkey();
}