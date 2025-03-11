extern i32 settings[];

#define ROM_MARGIN settings[0]
#define RAM_MARGIN settings[1]
#define PLL_CLK_MAX settings[2]
#define CPU_CLK_MAX settings[3]
#define SHW_CLK_MAX settings[4]
#define BUS_CLK_MAX settings[5]
#define IO_CLK_MAX settings[6]

#define ROM_MARGIN_DEF 5
#define RAM_MARGIN_DEF 5
#define PLL_CLK_MAX_DEF 800 * 1000000
#define IFC_CLK_MAX_DEF 275 * 1000000
#define SFC_CLK_MAX_DEF 150 * 1000000
#define BFC_CLK_MAX_DEF 100 * 1000000
#define PFC_CLK_MAX_DEF  50 * 1000000

#define ROM_MARGIN_MAX 15
#define RAM_MARGIN_MAX 15
#define PLL_MAX 999 * 1000000
#define CPU_MAX 350 * 1000000
#define SHW_MAX 200 * 1000000
#define BUS_MAX 200 * 1000000
#define IO_MAX 60 * 1000000

void settings_menu();