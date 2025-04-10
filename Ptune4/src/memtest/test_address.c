#include <gint/mpu/cpg.h>
#include <gint/cpu.h>
#include "validate.h"
#include "mem_test.h"

u32 *test_address(volatile u32 *address, bool mode, u32 block_size)
{
    u32 *ad = (u32 *)address;
    cpu_atomic_start();
    if (*address == *address)
        ad = mode == READ
        ? mem_read(address, &CPG.FLLFRQ.lword, FLF_x810, block_size)
        : mem_write(address, &CPG.FLLFRQ.lword, FLF_x810, block_size);
    cpu_atomic_end();
    return ad;
}
