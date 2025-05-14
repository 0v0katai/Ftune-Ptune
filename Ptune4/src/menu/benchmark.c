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
    u32 benchmark_data[8] = {0};
    
    timer_start(timer_configure(TIMER_TMU, 100000, GINT_CALL(disable_bench_flag)));
    benchmark_data[0] = tick_count(&bench_flag) / 100;
    for (int i = 0; i < 3; i++)
    {
        #if defined CG50 || defined CG100 || defined CP400
        static const u32 address[] = {0xa0150000, 0xac150000, 0xa4150000};
        #else
        static const u32 address[] = {0xa0150000, 0xa8150000, 0xa4150000};
        #endif
        bench_flag = true;
        timer_start(timer_configure(TIMER_TMU, 50000, GINT_CALL(disable_bench_flag)));
        benchmark_data[i + 1] = mem_bench((u32 *)address[i], &bench_flag);
    }

    #ifdef ENABLE_AZUR
    benchmark_data[5] = prof_exec(azrp_update());
    #endif
    benchmark_data[4] = prof_exec(dupdate());

    #ifdef ENABLE_DHRY
    benchmark_data[6] = prof_exec(dhrystone(DHRY_LOOP));
    #endif

    #ifdef ENABLE_WHET
    benchmark_data[7] = prof_exec(whetstone());
    #endif
    
    #if defined CP400
    for (int i = 0; i < 4; i++)
    {
        static const char *score_name[] = {"CPU", "ROM", "RAM", "I/O"};
        row_print(19 + i, 2, "%s %d", score_name[i], benchmark_data[i]);
    }
    row_print(23, 2, "dupdate: %d us/%d FPS", benchmark_data[4], 1000000 / benchmark_data[4]);
    
    # ifdef ENABLE_DHRY
    row_print(24, 2, "INT: %llu Dhrystone/s", DHRY_LOOP * 1000000ull / benchmark_data[6]);
    # endif

    # ifdef ENABLE_WHET
    row_print(25, 2, "DBL: %d KWIPS", 100 * ITERATIONS * 1000000 / benchmark_data[7]);
    row_highlight(25);
    # endif

    for (int i = 0; i < 6; i++)
        row_highlight(19 + i);
    #else
    for (int i = 0; i < 4; i++)
    {
        static const char *score_name[] = {"CPU", "ROM", "RAM", "I/O"};
        row_print(11, 2 + i * 12, "%s %d", score_name[i], benchmark_data[i]);
    }

    row_print(12, 2, "dupd: %d us/%d FPS", benchmark_data[4], 1000000 / benchmark_data[4]);
    # ifdef ENABLE_AZUR
    row_print(12, 26, "azrp: %d us/%d FPS", benchmark_data[5], 1000000 / benchmark_data[5]);
    # endif

    # ifdef ENABLE_DHRY
    row_print(13, 2, "INT: %llu Dhrystone/s", DHRY_LOOP * 1000000ull / benchmark_data[6]);
    # endif

    # ifdef ENABLE_WHET
    row_print(13, 26, "DBL: %d KWIPS", 100 * ITERATIONS * 1000000 / benchmark_data[7]);
    # endif

    for (int i = 0; i < 3; i++)
        row_highlight(11 + i);
    #endif
}
