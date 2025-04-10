#define FLF_x810 0x432a
#define PLL_x6 0x05
#define PLL_x24 0x17
#define WRITE_N 2000
#define READ_N 65536/4
#define ROM_BASE (u32 *)0xA0000000

void sdram_test();
void rom_test();
uint32_t *mem_read(volatile u32 *mem_area, volatile u32 *FLLFRQ_lword, int FLF, int n);
uint32_t *mem_write(volatile u32 *mem_area, volatile u32 *FLLFRQ_lword, int FLF, int n);