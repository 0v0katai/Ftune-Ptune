#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/clock.h>
#include <gint/cpu.h>
#include "validate.h"
#include "mem_test.h"
#include "config.h"

static void update(int FLF)
{
    CPG.FLLFRQ.FLF = FLF;
    cpg_compute_freq();
}

static void slowdown()
{
    BSC.CS0WCR.WR = WAIT_18;
    BSC.CS2WCR.lword = 0x000005C0;
    BSC.CS3WCR.TRC = 3;
}

u32 *read_address(int FLF, volatile u32 *address)
{
    u32 *ad = (u32 *)address;
    update(FLF);
    if (*address == *address)
        ad = mem_read(address, &CPG.FLLFRQ.lword, FLF_x500, READ_N);
    slowdown();
    return ad;
}

u32 *write_address(int FLF, volatile u32 *address)
{
    u32 *ad = (u32 *)address;
    update(FLF);
    if (*address == *address)
        ad = mem_write(address, &CPG.FLLFRQ.lword, FLF_x500, WRITE_N);
    slowdown();
    return ad;
}
