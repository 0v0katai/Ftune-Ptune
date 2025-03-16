/* Platform toggles */

// #define MONO_SH3    /* All SH3 monochrome calculators */
// #define MONO_SH4A   /* All SH4A monochrome calculators */
// #define CG20        /* fx-CG10, fx-CG20 */
// #define CG50        /* fx-CG50, Graph 90+E */
#define CG100       /* fx-CG100, fx-1AU Graph, Graph Math+ */

/* Feature toggles */

// #define ENABLE_FP
// #define ENABLE_GDB
#define ENABLE_DHRY
// #define ENABLE_WHET

/* Settings defaults */

#define ROM_MARGIN_DEF 5
#define RAM_MARGIN_DEF 5
#define PLL_CLK_MAX_DEF 800 * 1000000
#define IFC_CLK_MAX_DEF 275 * 1000000
#define SFC_CLK_MAX_DEF 150 * 1000000
#define BFC_CLK_MAX_DEF 100 * 1000000
#define PFC_CLK_MAX_DEF  50 * 1000000

#define ROM_MARGIN_MAX 15
#define RAM_MARGIN_MAX 15
#define PLL_MAX 1000 * 1000000
#define CPU_MAX 350 * 1000000
#define SHW_MAX 200 * 1000000
#define BUS_MAX 200 * 1000000
#define IO_MAX 60 * 1000000

#define FLL_RED_ZONE 17 * 1000000
#define PLL_RED_ZONE 750 * 1000000
#define IFC_RED_ZONE 250 * 1000000
#define SFC_RED_ZONE 150 * 1000000
#define BFC_RED_ZONE 100 * 1000000
#define PFC_RED_ZONE 48 * 1000000