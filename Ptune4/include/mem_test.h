#define FLF_x810 0x432a
#define PLL_x6 0x05
#define PLL_x24 0x17
#define WRITE_N 2000
#define READ_N 65536/4
#define ROM_BASE (u32 *)0xA0000000

enum TEST_MODE {READ, WRITE};

void sdram_test();
void rom_test();
u32 *rom_read_address(int FLF, int ROM_wait, volatile u32 *ROM);
u32 *sdram_write_address(int FLF, volatile u32 *RAM);
u32 *sram_address(int FLF, int RAM_wait, volatile u32 *RAM, bool mode);
u32 *mem_read(volatile u32 *address, volatile u32 *FLLFRQ_lword, int FLF, int n);
u32 *mem_write(volatile u32 *address, volatile u32 *FLLFRQ_lword, int FLF, int n);
