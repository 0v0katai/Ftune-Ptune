#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/clock.h>
#include <gint/cpu.h>
#include "validate.h"
#include "mem_test.h"
#include "config.h"

static void update(int FLF, u8 ROM_wait)
{
    CPG.FLLFRQ.FLF = FLF;
    cpg_compute_freq();
    BSC.CS0WCR.WR = ROM_wait;
}

u32 *read_address(int FLF, u8 ROM_wait, volatile u32 *address)
{
    u32 *ad = (u32 *)address;
    update(FLF, ROM_wait);
    if (*address == *address)
        ad = mem_read(address, &CPG.FLLFRQ.lword, FLF_x810, READ_N);
    BSC.CS0WCR.WR = ROM_wait + 1;
    return ad;
}

u32 *write_address(int FLF, volatile u32 *address)
{
    u32 *ad = (u32 *)address;
    update(FLF, WAIT_18);
    if (*address == *address)
        ad = mem_write(address, &CPG.FLLFRQ.lword, FLF_x810, WRITE_N);
    return ad;
}
