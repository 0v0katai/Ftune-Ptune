#include <gint/display.h>
#include <gint/timer.h>
#include <libprof.h>

#include "dhrystone.h"
#include "whetstone.h"
#include "util.h"
#include "config.h"
#include "menu.h"

#ifdef ENABLE_AZUR
#include <azur/gint/render.h>
#endif

u32 tick_count(volatile bool *flag);
u32 mem_bench(u32 *mem_area, volatile bool *flag);
bool bench_flag;

static int disable_bench_flag()
{
    bench_flag = false;
    return TIMER_STOP;
}

void run_benchmark()
{
    bench_flag = true;
    timer_start(timer_configure(TIMER_TMU, 100000, GINT_CALL(disable_bench_flag)));
    row_print(11, 2, "CPU: %d", tick_count(&bench_flag) / 100);
    
    for (int i = 0; i < 3; i++)
    {
        static const char *mem[] = {"ROM:", "RAM:", "I/O:"};
#if defined CG50 || defined CG100
        static const u32 address[] = {0xa0150000, 0xac150000, 0xa4150000};
#else
        static const u32 address[] = {0xa0150000, 0xa8150000, 0xa4150000};
#endif
        bench_flag = true;
        timer_start(timer_configure(TIMER_TMU, 50000, GINT_CALL(disable_bench_flag)));
        row_print(11, 14 + i * 12, "%s %d", mem[i], mem_bench((u32 *)address[i], &bench_flag));
    }
    row_highlight(11);
#ifdef ENABLE_AZUR
    const u32 time_azrp_update = prof_exec(azrp_update());
#endif
    const u32 time_dupdate = prof_exec(dupdate());
    row_print(12, 2, "dupd: "
    #ifdef ENABLE_FP
        "%.4g ms/%.4g FPS",
        time_dupdate / 1000.0f,
        1000000.0f / time_dupdate
    #else
        "%d us/%d FPS",
        time_dupdate,
        1000000 / time_dupdate
    #endif
    );
#ifdef ENABLE_AZUR
    row_print(12, 26, "azrp: "
    #ifdef ENABLE_FP
        "%.4g ms/%.4g FPS",
        time_azrp_update / 1000.0f,
        1000000.0f / time_azrp_update
    #else
        "%d us/%d FPS",
        time_azrp_update,
        1000000 / time_azrp_update
    #endif
    );
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
