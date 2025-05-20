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

#if defined CP400
# define TRC_DISPLAY_ROW 14
# define TRC_DISPLAY_X 1
# define SELECT_DISPLAY_ROW 36
#else
# define TRC_DISPLAY_ROW 2
# define TRC_DISPLAY_X 25
# define SELECT_DISPLAY_ROW 14
#endif

static void print_SDRAM_speed(u32 Bphi_f, u8 TRC)
{
    row_clear(TRC_DISPLAY_ROW + TRC);
    row_print(TRC_DISPLAY_ROW + TRC, TRC_DISPLAY_X, "TRC_%d", TRC_equivalent(TRC));
    row_print(TRC_DISPLAY_ROW + TRC, TRC_DISPLAY_X + 10, "%d KHz", Bphi_f / 1000);
}

static void ram_write_test(bool TRC_3_check)
{
    u32 temp[WRITE_N];
    u32 *write_area = NON_CACHE(temp);

    row_clear(SELECT_DISPLAY_ROW);
    row_print(SELECT_DISPLAY_ROW, 1, "RAM select: 0x%08X", write_area);
    struct cpg_overclock_setting s;
    cpg_get_overclock_setting(&s);
    static const u8 IFC = DIV_4, SFC = DIV_4, BFC = DIV_4, PFC = DIV_32;
    s.FRQCR = (PLL(24) << 24) + (IFC << 20) + (SFC << 12) + (BFC << 8) + PFC;
    cpg_set_overclock_setting(&s);
    
    int FLF_max = 600;
    u32 Bphi_f;
    for (int TRC = !TRC_3_check; TRC <= 3; TRC++)
    {
        for (int FLF = FLF_max; FLF < 2048; FLF += 2)
        {
            BSC.CS3WCR.TRC = TRC;
            if (write_address(FLF, write_area))
            {
                FLF_max = FLF;
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
                    row_print(TRC_DISPLAY_ROW - 1, TRC_DISPLAY_X, "Trial %d", trial);
                    row_print_color(TRC_DISPLAY_ROW - 1, TRC_DISPLAY_X + 10, C_RED, C_WHITE, "%d KHz", Bphi_f / 1000);
                    dupdate();
                    row_clear(TRC_DISPLAY_ROW - 1);
                }
                raW_TRC[TRC] = Bphi_f;
                FLF = 2048;
            }
            print_SDRAM_speed(clock_freq()->Bphi_f, TRC);
            dupdate();
        }
    }
    for (int i = 2; i >= 0; i--)
        if (raW_TRC[i] > raW_TRC[i + 1])
            raW_TRC[i] = raW_TRC[i + 1];
    BUS_CLK_MAX = raW_TRC[3] / 100 * (100 - RAM_MARGIN);
}

void sdram_test(bool TRC_3_check)
{
    dclear(C_WHITE);
    row_title("SDRAM Test");

    struct cpg_overclock_setting s0;
    cpg_get_overclock_setting(&s0);
    ram_write_test(TRC_3_check);
    cpg_set_overclock_setting(&s0);
}
#endif