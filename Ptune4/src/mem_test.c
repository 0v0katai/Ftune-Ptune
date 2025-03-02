#include <gint/clock.h>
#include <gint/timer.h>
#include <gint/gint.h>
#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/display.h>

#include "settings.h"
#include "validate.h"
#include "mem_test.h"
#include "util.h"

#define WRITE_N 2000

static int ram_ad(int FLL, volatile u32 *RAM, bool mode, int block_size)
{
    u32 *ad;
    ad = RAM;
    cpu_atomic_start();
    CPG.FLLFRQ.FLF = FLL;
    cpg_compute_freq();
    BSC.CS0WCR.WR = best_rom_wait(clock_freq()->Bphi_f * (100 - RAM_MARGIN) / 100) + 1;
    if (*RAM == *RAM)
        ad = mode ? mem_write(RAM, &CPG.FLLFRQ.lword, FLL_x810, block_size)
                  : mem_read(RAM, &CPG.FLLFRQ.lword, FLL_x810, block_size);
    cpu_atomic_end();
    return (int)ad;
}

static int loop_test(volatile u32 *select, bool mode)
{
    for (int PLL = 20; PLL < 32; PLL++)
    {
        int IFC = DIV_2, SFC = DIV_4, BFC = DIV_4, PFC = DIV_8;
        if (PLL > 26)
        {
            IFC = DIV_4;
            SFC = DIV_4;
            BFC = DIV_4;
            PFC = DIV_16;
        }
        CPG.FRQCR.lword = (PLL << 24) + (IFC << 20) + (SFC << 12) + (BFC << 8) + PFC;
        struct cpg_overclock_setting s;
        cpg_get_overclock_setting(&s);
        cpg_set_overclock_setting(&s);
        BSC.CS0WCR.WR = best_rom_wait(clock_freq()->Bphi_f) + 2;

        for (int FLL = 900; FLL < 1100; FLL += 2)
        {
            int ad = mode ? ram_ad(FLL, select, WRITE, WRITE_N)
                          : ram_ad(FLL, select, READ, 16384);
            if (ad)
                return FLL;
            row_clear(3 + mode);
            row_print(3, 1, "Read:");
            row_print(4, 1, "Write:");
            row_print_color(3 + mode, 8, C_BLUE, C_WHITE, "%d KHz", clock_freq()->Bphi_f / 1000);
            dupdate();
        }
    }
}

// static void search_area(volatile u32 **select, int FLLP)
// {
//     row_clear(14);
//     row_print(14, 1, "RAM Search");

//     u32 min_freq = 9999999;
//     volatile u32 *min_select = *select; 

//     for (int f = 0; f < 32; f++)
//     {
//         row_clear(14);
//         row_print(14, 1, "RAM select: 0x%08x", *select);
//         for (int FLL = FLLP; FLL > FLLP-50; FLL--)
//         {
//             if (ram_ad(FLL, *select, READ, 16384) == 0)
//             {
//                 u32 freq = clock_freq()->Bphi_f * (100 - RAM_MARGIN) / 100;
//                 if (freq < min_freq)
//                 {
//                     min_freq = freq;
//                     *select = min_select;
//                     row_clear(3);
//                     row_print(3, 1, "Read:");
//                     row_print_color(3, 8, C_BLUE, C_WHITE, "%d KHz", freq);
//                     dupdate();
//                 }
//                 break;
//             }
//         }
//         min_select += 0x2500;
//     }
// }

void sdram_test()
{
    dclear(C_WHITE);
    row_title("SDRAM Test");

    struct cpg_overclock_setting s0;
    cpg_get_overclock_setting(&s0);
    u16 *vram, *unused;
    dgetvram(&vram, &unused);
    volatile u32 *select = (void *)vram;

    row_print(13, 1, "RAM select: 0x%08x", select);
    row_print(14, 1, "Read Test");
    
    // int FLLP =
    loop_test(select, READ);
    settings[3] = clock_freq()->Bphi_f * (u64)(100 - RAM_MARGIN) / 100;
    // search_area(&select, FLLP);

    // row_clear(13);
    // row_clear(14);
    // row_print(13, 1, "RAM select: 0x%08x", select);
    // row_print(14, 1, "2nd Read Test");
    // loop_test(select, READ);
    // cpg_set_overclock_setting(&s0);

    u32 temp[WRITE_N+1];
    u32 *write = ((u32)&temp & 0x0FFFFFFF) | 0xA0000000;
    row_clear(13);
    row_clear(14);
    row_print(13, 1, "RAM select: 0x%08x", write);
    row_print(14, 1, "Write Test");
    loop_test(write, WRITE);
    
    cpg_set_overclock_setting(&s0);

    row_print(10, 1, "Max bus frequency set: %d KHz", settings[3] / 1000);
    row_print(11, 1, "Press any key to exit...");

    dupdate();
    getkey();
}