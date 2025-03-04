#define FLF_x810 0x432a
#define PLL_x16 0xf

void sdram_test();
uint32_t *mem_read(volatile u32 *mem_area, volatile u32 *FLLFRQ_lword, int FLF, int n);
uint32_t *mem_write(volatile u32 *mem_area, volatile u32 *FLLFRQ_lword, int FLF, int n);