extern i32 settings[];

#define PLL_CLK_MAX settings[0]
#define CPU_CLK_MAX settings[1]
#define SHW_CLK_MAX settings[2]
#define BUS_CLK_MAX settings[3]
#define IO_CLK_MAX settings[4]

#define PLL_CLK_MAX_DEF 800 * 1000000
#define IFC_CLK_MAX_DEF 275 * 1000000
#define SFC_CLK_MAX_DEF 150 * 1000000
#define BFC_CLK_MAX_DEF 100 * 1000000
#define PFC_CLK_MAX_DEF  50 * 1000000

#define PLL_MAX 999 * 1000000
#define CPU_MAX 350 * 1000000
#define SHW_MAX 200 * 1000000
#define BUS_MAX 200 * 1000000
#define IO_MAX 60 * 1000000

void settings_menu();