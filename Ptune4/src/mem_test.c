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

static bool bench_flag;

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

static int loop_write_test()
{
    u32 temp[WRITE_N+1];
    u32 *write_area = (u32 *)(((u32)&temp & 0x0FFFFFFF) | 0xA0000000);
    
    row_print(13, 1, "RAM select: 0x%08x", write_area);
    row_print(14, 1, "Write Test");

    static const u8 PLL = PLL_x16, IFC = DIV_4, SFC = DIV_4, BFC = DIV_4, PFC = DIV_16;
    CPG.FRQCR.lword = (PLL << 24) + (IFC << 20) + (SFC << 12) + (BFC << 8) + PFC;
    struct cpg_overclock_setting s;
    cpg_get_overclock_setting(&s);
    cpg_set_overclock_setting(&s);
    
    u32 Bphi_f = clock_freq()->Bphi_f;
    BSC.CS0WCR.WR = best_rom_wait(Bphi_f) + 2;

    for (int FLF = 1200; FLF < 2000; FLF += 2)
    {
        if (ram_ad(FLF, write_area, WRITE_N))
            break;

        Bphi_f = clock_freq()->Bphi_f;
        row_clear(4);
        row_print(4, 1, "Write:");
        row_print_color(4, 8, C_BLUE, C_WHITE, "%d KHz", Bphi_f / 1000);
        dupdate();
    }

    return Bphi_f * (100ull - RAM_MARGIN) / 100;
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

void run_benchmark()
{
    bench_flag = true;
    timer_start(timer_configure(TIMER_TMU, 100000, GINT_CALL(disable_bench_flag)));
    row_print(11, 2, "CPU: %d", tick_count(&bench_flag) / 100);
    
    for (int i = 0; i < 3; i++)
    {
        const char *mem[] = {"ROM:", "RAM:", "I/O:"};
        const u32 address[] = {0xa0010000, 0xa8010000, 0xa4150000};
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