#define FLF_x810 0x432a
#define PLL_x6 0x05
#define PLL_x24 0x17
#define WRITE_N 2000
#define READ_N 65536/4
#define ROM_BASE (u32 *)0xA0000000

enum TEST_MODE {READ, WRITE};

void sdram_test();
void rom_test();
u32 *test_address(volatile u32 *address, bool mode, u32 block_size);
uint32_t *mem_read(volatile u32 *address, volatile u32 *FLLFRQ_lword, int FLF, int n);
uint32_t *mem_write(volatile u32 *address, volatile u32 *FLLFRQ_lword, int FLF, int n);
