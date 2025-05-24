#include <gint/display.h>
#include <gint/timer.h>
#include <libprof.h>

#include "dhrystone.h"
#include "whetstone.h"
#include "util.h"
#include "config.h"
#include "menu.h"

#ifdef ENABLE_AZUR
# include <azur/gint/render.h>
#endif

#if defined CG50 || defined CG100 || defined CP400
# define RAM_ADDRESS 0xAC150000
#else
# define RAM_ADDRESS 0xA8150000
#endif

#if defined CP400
# define SCORE_X(i) 2
# define SCORE_ROW(i) (19 + (i))
# define UPDATE_ROW 23
# define DHRYSTONE_ROW 24
# define WHETSTONE_X 2
# if defined ENABLE_WHETSTONE
#  define WHETSTONE_ROW 25
# else
#  define WHETSTONE_ROW 24
# endif
#else
# define SCORE_X(i) ((i) * 12 + 2)
# define SCORE_ROW(i) 11
# define UPDATE_ROW 12
# define DHRYSTONE_ROW 13
# define WHETSTONE_X 26
# define WHETSTONE_ROW 13
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
        static const u32 address[] = {0xA0150000, RAM_ADDRESS, 0xA4150000};
        bench_flag = true;
        timer_start(timer_configure(TIMER_TMU, 50000, GINT_CALL(disable_bench_flag)));
        benchmark_data[i + 1] = mem_bench((u32 *)address[i], &bench_flag);
    }

    #ifdef ENABLE_AZUR
    benchmark_data[5] = prof_exec(azrp_update());
    #endif
    int ETMU_timer = timer_configure(TIMER_ETMU, 0xFFFFFFFF, GINT_CALL_NULL);
    timer_start(ETMU_timer);
    int ETMU_start = SH7305_ETMU[ETMU_timer - 3].TCNT;
    benchmark_data[4] = prof_exec(dupdate());
    u32 ETMU_time = 32768 / (ETMU_start - SH7305_ETMU[ETMU_timer - 3].TCNT);
    timer_stop(ETMU_timer);

    #ifdef ENABLE_DHRY
    benchmark_data[6] = prof_exec(dhrystone(DHRY_LOOP));
    #endif

    #ifdef ENABLE_WHET
    benchmark_data[7] = prof_exec(whetstone());
    #endif
    
    for (int i = 0; i < 4; i++)
    {
        static const char *score_name[] = {"CPU", "ROM", "RAM", "I/O"};
        row_print(SCORE_ROW(i), SCORE_X(i), "%s %d", score_name[i], benchmark_data[i]);
    }
    row_print_color(UPDATE_ROW, 2, ETMU_time < (1000000 / benchmark_data[4]) ? C_RGB(0,31,31) : C_RGB(31,0,31), C_WHITE,
        "dupd: %d us/%d (%d) FPS", benchmark_data[4], 1000000 / benchmark_data[4], ETMU_time);
    
    #ifdef ENABLE_AZUR
    row_print(UPDATE_ROW, 26, "azrp: %d us/%d FPS", benchmark_data[5], 1000000 / benchmark_data[5]);
    #endif

    #ifdef ENABLE_DHRY
    row_print(DHRYSTONE_ROW, 2, "INT: %llu Dhrystone/s", DHRY_LOOP * 1000000ull / benchmark_data[6]);
    #endif

    #ifdef ENABLE_WHET
    row_print(WHETSTONE_ROW, WHETSTONE_X, "DBL: %d KWIPS", 100 * ITERATIONS * 1000000 / benchmark_data[7]);
    #endif

    for (int i = SCORE_ROW(0); i != WHETSTONE_ROW + 1; i++)
        row_highlight(i);
}
