#include <gint/clock.h>
#include <gint/timer.h>
#include <libprof.h>
#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/display.h>

#include "settings.h"
#include "validate.h"
#include "mem_test.h"
#include "dhrystone.h"
#include "whetstone.h"
#include "util.h"
#include "config.h"

#define WRITE_N 2000

bool bench_flag;
u32 Bphi_max[4];

int disable_bench_flag()
{
    bench_flag = false;
    return TIMER_STOP;
}

static u32 *ram_ad(int FLF, volatile u32 *RAM, int block_size)
{
    u32 *ad = (u32 *)RAM;
    cpu_atomic_start();
    CPG.FLLFRQ.FLF = FLF;
    cpg_compute_freq();
    BSC.CS0WCR.WR = best_rom_wait(clock_freq()->Bphi_f * (100ull - RAM_MARGIN) / 100) + 1;
    if (*RAM == *RAM)
        ad = mem_write(RAM, &CPG.FLLFRQ.lword, FLF_x810, block_size);
    cpu_atomic_end();
    return ad;
}

static void print_Bphi_max(u32 Bphi_f, u8 TRC)
{
    static const int trc_wait[4] = {3, 4, 6, 9};
    row_clear(4 + TRC);
    row_print(4 + TRC, 1, "Write (TRC=%d):", trc_wait[TRC]);
    row_print_color(4 + TRC, 16, C_BLUE, C_WHITE, "%d KHz", Bphi_f / 1000);
}

static void ram_write_test()
{
    u32 temp[WRITE_N+1];
    u32 *write_area = (u32 *)(((u32)&temp & 0x0FFFFFFF) | 0xA0000000);

    row_print(1, 1, "RAM select: 0x%08x", write_area);
    struct cpg_overclock_setting s;
    cpg_get_overclock_setting(&s);
    static const u8 PLL = PLL_x24, IFC = DIV_4, SFC = DIV_4, BFC = DIV_4, PFC = DIV_32;
    s.FRQCR = (PLL << 24) + (IFC << 20) + (SFC << 12) + (BFC << 8) + PFC;
    cpg_set_overclock_setting(&s);
    
    BSC.CS0WCR.WR = 0b1011;
    int FLF_max = 600;
    for (int TRC = 0; TRC <= 3; TRC++)
    {
        bool attained = false;
        BSC.CS3WCR.TRC = TRC;
        for (int FLF = FLF_max; FLF < 2048; FLF += 2)
        {
            if (attained)
                break;
            if (ram_ad(FLF, write_area, WRITE_N))
            {
                FLF_max = FLF;
                u32 Bphi_f;
                for (int trial = 1; trial <= 100; trial++)
                {
                    if (ram_ad(FLF_max, write_area, WRITE_N))
                    {
                        trial = 0;
                        FLF_max -= 2;
                        continue;
                    }
                    Bphi_f = clock_freq()->Bphi_f;
                    row_print(2, 1, "Trial (%d/100)", trial);
                    row_print_color(2, 16, C_RED, C_WHITE, "%d KHz", Bphi_f / 1000);
                    dupdate();
                    row_clear(2);
                }
                attained = true;
                if (Bphi_max[TRC - 1] > Bphi_f)
                {
                    Bphi_max[TRC - 1] = Bphi_f;
                    print_Bphi_max(Bphi_f, TRC - 1);
                }
                Bphi_max[TRC] = Bphi_f;
            }
            print_Bphi_max(clock_freq()->Bphi_f, TRC);
            dupdate();
        }
    }
    for (int i = 0; i < 4; i++)
        Bphi_max[i] = Bphi_max[i] * (100ull - RAM_MARGIN) / 100;
}

void sdram_test()
{
    dclear(C_WHITE);
    row_title("SDRAM Test");

    struct cpg_overclock_setting s0;
    cpg_get_overclock_setting(&s0);
    ram_write_test();
    cpg_set_overclock_setting(&s0);

    BUS_CLK_MAX = Bphi_max[s0.CS3WCR & 0b11];
    row_highlight(4 + (s0.CS3WCR & 0b11));
    row_print(13, 1, "Bus Clock Max set: %d KHz", BUS_CLK_MAX / 1000);
    row_print(14, 1, "Press any key to exit...");

    dupdate();
    getkey();
}

void run_benchmark()
{
    bench_flag = true;
    timer_start(timer_configure(TIMER_TMU, 100000, GINT_CALL(disable_bench_flag)));
    row_print(11, 2, "CPU: %d", tick_count(&bench_flag) / 100);
    
    for (int i = 0; i < 3; i++)
    {
        static const char *mem[] = {"ROM:", "RAM:", "I/O:"};
        static const u32 address[] = {0xa0010000, 0xa8010000, 0xa4150000};
        bench_flag = true;
        timer_start(timer_configure(TIMER_TMU, 50000, GINT_CALL(disable_bench_flag)));
        row_print(11, 14 + i * 12, "%s %d", mem[i], mem_bench((u32 *)address[i], &bench_flag));
    }
    row_highlight(11);
    
    const u32 time_dupdate = prof_exec(dupdate());
#ifdef ENABLE_FP
    row_print(12, 2, "dupdate(): %d us (%3.2f FPS)", time_dupdate, 1000000.0f / time_dupdate);
#else
    row_print(12, 2, "dupdate(): %d us (%d FPS)", time_dupdate, 1000000 / time_dupdate);
#endif
    row_highlight(12);

#ifdef ENABLE_DHRY
    const u32 time_dhrystone = prof_exec(dhrystone(DHRY_LOOP));
    row_print(13, 2, "INT: %llu Dhrystone/s", DHRY_LOOP * 1000000ull / time_dhrystone);
#endif

#ifdef ENABLE_WHET
    const u32 time_whetstone = prof_exec(whetstone());
    row_print(13, 26, "DBL: %d KWIPS", 100 * ITERATIONS * 1000000 / time_whetstone);
#endif
    
    row_highlight(13);
}