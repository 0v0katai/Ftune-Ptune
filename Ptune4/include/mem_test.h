#define FLL_x810 0x432A

enum test_mode
{
    READ,
    WRITE
};

void sdram_test();
uint32_t *mem_read(volatile u32 *mem_area, volatile u32 *FLLFRQ_lword, int FLL, int n);
uint32_t *mem_write(volatile u32 *mem_area, volatile u32 *FLLFRQ_lword, int FLL, int n);