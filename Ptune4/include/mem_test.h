#define FLF_x810 0x432a
#define PLL_x24 0x17

void run_benchmark();
int disable_bench_flag();
void sdram_test();
uint32_t *mem_read(volatile u32 *mem_area, volatile u32 *FLLFRQ_lword, int FLF, int n);
uint32_t *mem_write(volatile u32 *mem_area, volatile u32 *FLLFRQ_lword, int FLF, int n);
uint32_t mem_bench(u32 *mem_area, volatile bool *flag);
uint32_t tick_count(volatile bool *flag);