#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/clock.h>
#include <gint/display.h>

#include "settings.h"
#include "validate.h"
#include "mem_test.h"
#include "util.h"
#include "config.h"

#if defined CG50 || defined CG100 || defined CP400
u32 raW_TRC[4] = {raW_TRC_3, raW_TRC_4, raW_TRC_6, raW_TRC_9};

static void print_SDRAM_speed(u32 Bphi_f, u8 TRC)
{
    static const int trc_wait[4] = {3, 4, 6, 9};
    row_clear(2 + TRC);
    row_print(2 + TRC, 25, "TRC_%d", trc_wait[TRC]);
    row_print(2 + TRC, 35, "%d KHz", Bphi_f / 1000);
}

static void ram_write_test()
{
    u32 temp[WRITE_N];
    u32 *write_area = NON_CACHE(temp);

    row_print(14, 1, "RAM select: 0x%08X", write_area);
    struct cpg_overclock_setting s;
    cpg_get_overclock_setting(&s);
    static const u8 IFC = DIV_4, SFC = DIV_4, BFC = DIV_4, PFC = DIV_32;
    s.FRQCR = (PLL(24) << 24) + (IFC << 20) + (SFC << 12) + (BFC << 8) + PFC;
    cpg_set_overclock_setting(&s);
    
    int FLF_max = 300;
    for (int TRC = 0; TRC <= 3; TRC++)
    {
        for (int FLF = FLF_max; FLF < 2048; FLF += 2)
        {
            BSC.CS3WCR.TRC = TRC;
            if (write_address(FLF, write_area))
            {
                FLF_max = FLF;
                u32 Bphi_f;
                for (int trial = 1; trial <= 100; trial++)
                {
                    BSC.CS3WCR.TRC = TRC;
                    if (write_address(FLF_max, write_area))
                    {
                        trial = 0;
                        FLF_max -= 2;
                        continue;
                    }
                    Bphi_f = clock_freq()->Bphi_f;
                    row_print(1, 25, "Trial %d", trial);
                    row_print_color(1, 35, C_RED, C_WHITE, "%d KHz", Bphi_f / 1000);
                    dupdate();
                    row_clear(1);
                }
                if (raW_TRC[TRC - 1] > Bphi_f)
                {
                    raW_TRC[TRC - 1] = Bphi_f;
                    print_SDRAM_speed(Bphi_f, TRC - 1);
                }
                raW_TRC[TRC] = Bphi_f;
                FLF = 2048;
            }
            print_SDRAM_speed(clock_freq()->Bphi_f, TRC);
            dupdate();
        }
    }
    BUS_CLK_MAX = raW_TRC[3] / 100 * (100 - RAM_MARGIN);
}

void sdram_test()
{
    dclear(C_WHITE);
    row_title("SDRAM Test");

    struct cpg_overclock_setting s0;
    cpg_get_overclock_setting(&s0);
    ram_write_test();
    cpg_set_overclock_setting(&s0);
}
#endif