#include "config.h"

extern i32 roR[];
extern u32 raW_TRC[];
extern i32 settings[];

#define ROM_MARGIN settings[0]
#define RAM_MARGIN settings[1]
#define PLL_CLK_MAX settings[2]
#define CPU_CLK_MAX settings[3]
#define SHW_CLK_MAX settings[4]
#define BUS_CLK_MAX settings[5]
#define IO_CLK_MAX settings[6]
