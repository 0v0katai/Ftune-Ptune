#include "config.h"

#define FLF_x500 0x41F4
#define PLL(a) ((a) - 1)
#define WRITE_N 2000
#define READ_N 65536/4
#define ROM_BASE (u32 *)0xA0000000
#define SRAM_BASE (u32 *)0xA8000000
#define NON_CACHE(address) ((u32 *)(((u32)&(address) & 0x0FFFFFFF) | 0xA0000000))

#if defined CP400
enum roR_default_table
{
    roR_0  =  13 * 1000000, roR_1  =  23 * 1000000, roR_2  =  33 * 1000000,
    roR_3  =  43 * 1000000, roR_4  =  53 * 1000000, roR_5  =  62 * 1000000,
    roR_6  =  72 * 1000000, roR_8  =  92 * 1000000, roR_10 = 110 * 1000000,
    roR_12 = 127 * 1000000, roR_14 = 142 * 1000000, roR_18 = 165 * 1000000
};
# define ROM_SEARCH_FLF_START 1350
#elif defined CG20 || defined CG50 || defined CG100
enum roR_default_table
{
    roR_0  =  14 * 1000000, roR_1  =  24 * 1000000, roR_2  =  35 * 1000000,
    roR_3  =  45 * 1000000, roR_4  =  56 * 1000000, roR_5  =  66 * 1000000,
    roR_6  =  77 * 1000000, roR_8  =  97 * 1000000, roR_10 = 115 * 1000000,
    roR_12 = 132 * 1000000, roR_14 = 158 * 1000000, roR_18 = 191 * 1000000
};
# define ROM_SEARCH_FLF_START 1350
enum raR_default_table
{
    raR_0  =  15 * 1000000, raR_1  =  30 * 1000000, raR_2  =  52 * 1000000,
    raR_3  =  70 * 1000000, raR_4  =  88 * 1000000, raR_5  = 106 * 1000000,
    raR_6  = 124 * 1000000, raR_8  = 150 * 1000000
};
# define RAM_SEARCH_FLF_START 1600
enum raW_default_table
{
    raW_0  =  28 * 1000000, raW_1  =  57 * 1000000, raW_2  =  85 * 1000000,
    raW_3  = 109 * 1000000, raW_4  = 137 * 1000000, raW_5  = 142 * 1000000,
    raW_6  = 147 * 1000000
};
#elif defined MONO_SH4A
enum roR_default_table
{
    roR_0  =  19 * 1000000, roR_1  =  30 * 1000000,  roR_2  =  46 * 1000000,
    roR_3  =  60 * 1000000, roR_4  =  72 * 1000000,  roR_5  =  84 * 1000000,
    roR_6  = 100 * 1000000, roR_8  = 120 * 1000000,  roR_10 = 150 * 1000000,
    roR_12 = 200 * 1000000, roR_14 = 200 * 1000000,  roR_18 = 200 * 1000000
};
#endif
enum raW_TRC_default_table
{
    raW_TRC_3 =  50 * 1000000, raW_TRC_4 = 80 * 1000000,
    raW_TRC_6 = 120 * 1000000, raW_TRC_9 = 130 * 1000000
};

enum TEST_MODE {READ, WRITE};

typedef byte_union(mem_test_settings,
                   u8 : 5;
                   bool TRC_3_check  : 1;
                   bool roR_10_check : 1;
                   bool roR_12_check : 1;);

void sram_test();
void sdram_test(bool TRC_3_check);
void rom_test(mem_test_settings test_settings);
u32 *read_address(int FLF, volatile u32 *address);
u32 *write_address(int FLF, volatile u32 *address);
u32 *mem_read(volatile u32 *address, volatile u32 *FLLFRQ_lword, int FLF, int n);
u32 *mem_write(volatile u32 *address, volatile u32 *FLLFRQ_lword, int FLF, int n);
