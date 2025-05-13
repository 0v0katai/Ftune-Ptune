#include "config.h"

extern i32 roR[];
#if !defined CG50 && !defined CG100 && !defined CP400
    extern u32 raR[];
    extern u32 raW[];
#else
    extern u32 raW_TRC[];
#endif
extern i32 settings[];

#define ROM_MARGIN settings[0]
#define RAM_MARGIN settings[1]
#define PLL_CLK_MAX settings[2]
#define CPU_CLK_MAX settings[3]
#define SHW_CLK_MAX settings[4]
#define BUS_CLK_MAX settings[5]
#define IO_CLK_MAX settings[6]
