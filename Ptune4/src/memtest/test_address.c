#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/clock.h>
#include <gint/cpu.h>
#include "validate.h"
#include "mem_test.h"
#include "config.h"

static u32 *test_address(volatile u32 *address, bool mode)
{
    u32 *ad = (u32 *)address;
    cpu_atomic_start();
    if (*address == *address)
        ad = mode == READ
        ? mem_read(address, &CPG.FLLFRQ.lword, FLF_x810, READ_N)
        : mem_write(address, &CPG.FLLFRQ.lword, FLF_x810, WRITE_N);
    cpu_atomic_end();
    return ad;
}

static void update_freq(int FLF)
{
    CPG.FLLFRQ.FLF = FLF;
    cpg_compute_freq();
}

u32 *rom_read_address(int FLF, int ROM_wait, volatile u32 *ROM)
{
    update_freq(FLF);
    BSC.CS0WCR.WR = ROM_wait;
    return test_address(ROM, READ);
}

#if defined CG50 || defined CG100
u32 *sdram_write_address(int FLF, volatile u32 *RAM)
{
    update_freq(FLF);
    return test_address(RAM, WRITE);
}
#else
u32 *sram_address(int FLF, int RAM_wait, volatile u32 *RAM, bool mode)
{
    update_freq(FLF);
    if (mode == READ)
    {
        BSC.CS2WCR.WR = RAM_wait;
        BSC.CS2WCR.WW = 0;
    }
    else
    {
        BSC.CS2WCR.WW = RAM_wait;
        BSC.CS2WCR.WR = WAIT_18;
    }
    return test_address(RAM, mode);
}
#endif
