#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/clock.h>
#include <gint/display.h>

#include "settings.h"
#include "validate.h"
#include "mem_test.h"
#include "util.h"
#include "config.h"

#if defined CG50 || defined CG100
static void print_SDRAM_speed(u32 Bphi_f, u8 TRC)
{
    static const int trc_wait[4] = {3, 4, 6, 9};
    row_clear(4 + TRC);
    row_print(4 + TRC, 1, "Write (TRC=%d): %d KHz", trc_wait[TRC], Bphi_f / 1000);
}

static void ram_write_test()
{
    u32 Bphi_max[4];
    u32 temp[WRITE_N];
    u32 *write_area = (u32 *)(((u32)&temp & 0x0FFFFFFF) | 0xA0000000);

    row_print(1, 1, "RAM select: 0x%08X", write_area);
    struct cpg_overclock_setting s;
    cpg_get_overclock_setting(&s);
    static const u8 IFC = DIV_4, SFC = DIV_4, BFC = DIV_4, PFC = DIV_32;
    s.FRQCR = (PLL_x24 << 24) + (IFC << 20) + (SFC << 12) + (BFC << 8) + PFC;
    s.CS0WCR = (s.CS0WCR & ~(0b1111 << 7)) | (WAIT_18 << 7);
    cpg_set_overclock_setting(&s);
    
    int FLF_max = 300;
    for (int TRC = 0; TRC <= 3; TRC++)
    {
        BSC.CS3WCR.TRC = TRC;
        for (int FLF = FLF_max; FLF < 2048; FLF += 2)
        {
            if (sdram_write_address(FLF, write_area))
            {
                FLF_max = FLF;
                u32 Bphi_f;
                for (int trial = 1; trial <= 100; trial++)
                {
                    if (sdram_write_address(FLF_max, write_area))
                    {
                        trial = 0;
                        FLF_max -= 2;
                        continue;
                    }
                    Bphi_f = clock_freq()->Bphi_f;
                    row_print(2, 1, "Trial (%d/100)", trial);
                    row_print_color(2, 15, C_RED, C_WHITE, "%d KHz", Bphi_f / 1000);
                    dupdate();
                    row_clear(2);
                }
                if (Bphi_max[TRC - 1] > Bphi_f)
                {
                    Bphi_max[TRC - 1] = Bphi_f;
                    print_SDRAM_speed(Bphi_f, TRC - 1);
                }
                Bphi_max[TRC] = Bphi_f;
                FLF = 2048;
            }
            print_SDRAM_speed(clock_freq()->Bphi_f, TRC);
            dupdate();
        }
    }
    for (int i = 0; i < 4; i++)
    {
        BUS_CLK_MAX(i) = Bphi_max[i] / 100 * (100 - RAM_MARGIN) ;
        row_print(4 + i, 26, ">>");
        row_print_color(4 + i, 29, C_BLUE, C_WHITE, "%d KHz", BUS_CLK_MAX(i) / 1000);
    }
}

void sdram_test()
{
    dclear(C_WHITE);
    row_title("SDRAM Test");

    struct cpg_overclock_setting s0;
    cpg_get_overclock_setting(&s0);
    ram_write_test();
    cpg_set_overclock_setting(&s0);

    row_print(9, 1, "RAM margin: %d%%", RAM_MARGIN);
    row_print(10, 1, "Current preset:");
    if (clock_get_speed())  
        row_print(10, 17, "F%d", clock_get_speed());
    else
        row_print(10, 17, "Custom");
    row_print_color(11, 1, C_RED, C_WHITE, "Warning! SDRAM test may cause system errors!");
    row_print_color(12, 1, C_RED, C_WHITE, "It's strongly advised to RESTART after the test.");
    row_print(14, 1, "Press any key to exit...");

    dupdate();
    getkey();
}
#endif