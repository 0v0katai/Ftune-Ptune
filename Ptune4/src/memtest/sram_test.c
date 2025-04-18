#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/clock.h>
#include <gint/display.h>

#include "settings.h"
#include "validate.h"
#include "mem_test.h"
#include "util.h"
#include "config.h"

#if !defined CG50 && !defined CG100
#define RAM_WAIT(CS2WCR) (((CS2WCR) >> 7) & 0b1111)

u32 raR[] = {raR_0, raR_1, raR_2, raR_3, raR_4, raR_5, raR_6, raR_8};
u32 raW[] = {raW_0, raW_1, raW_2, raW_3, raW_4, raW_5, raW_6};

static void print_RAM_read_select(u32 *RAM_read_area)
{
    row_clear(14);
    row_print(14, 1, "RAM read select: 0x%08X", RAM_read_area);
}

static void ram_read_test()
{
    u32 *RAM_read_area = SRAM_BASE;
    static const u32 raR_default[] = {raR_0, raR_1, raR_2, raR_3, raR_4, raR_5, raR_6, raR_8};
    
    /* Slowest RAM read area search */
    struct cpg_overclock_setting s;
    clock_set_speed(CLOCK_SPEED_DEFAULT);
    BSC.CS2WCR.WW = WAIT_6;
    cpg_get_overclock_setting(&s);
    int FLF_max;
    for (int FLF = 900; FLF < 2048; FLF += 2)
    {
        if (read_address(FLF, WAIT_18, RAM_read_area))
            break;
        FLF_max = FLF;
        print_RAM_read_select(RAM_read_area);
        row_print(1, 1, "%d KHz", clock_freq()->Bphi_f / 1000);
        dupdate();
        row_clear(1);
    }
    u32 *pointer = SRAM_BASE;
    for (int i = 0; i < 124; i++)
    {
        if (read_address(FLF_max, WAIT_18, RAM_read_area))
        {
            FLF_max -= 2;
            RAM_read_area = pointer;
        }
        print_RAM_read_select(RAM_read_area);
        row_print_color(14, 30, C_RED, C_WHITE, "0x%08X", pointer);
        dupdate();
        row_clear(14);
        pointer += 0x1000/4;
    }

    print_RAM_read_select(RAM_read_area);
    for (int i = WAIT_0; i <= WAIT_8; i++)
    {
        static const u8 IFC = DIV_4, SFC = DIV_4, BFC = DIV_4, PFC = DIV_32;
        s.FRQCR = ((PLL(8) + i * 3) << 24) + (IFC << 20) + (SFC << 12) + (BFC << 8) + PFC;
        cpg_set_overclock_setting(&s);
        BSC.CS2WCR.WR = i;
        for (int FLF = raR_default[i] / (PLL(8) + i * 3 + 1) / 4096; FLF < 2048; FLF += 2)
        {
            if (read_address(FLF, WAIT_18, RAM_read_area))
                break;
            static const u8 mem_wait[] = {0, 1, 2, 3, 4, 5, 6, 8};
            const u32 Bphi_f = clock_freq()->Bphi_f;
            row_clear(2 + i);
            row_print(2 + i, 25, "raR_%d", mem_wait[i]);
            row_print(2 + i, 35, "%d KHz", Bphi_f / 1000);
            raR[i] = Bphi_f;
            dupdate();
        }
    }
    clock_set_speed(CLOCK_SPEED_DEFAULT);
}

static void ram_write_test()
{
    u32 temp[WRITE_N];
    u32 *write_area = NON_CACHE(temp);
    static const u32 raW_default[] = {raW_0, raW_1, raW_2, raW_3, raW_4, raW_5, raW_6};

    for (int i = 0; i <= WAIT_8; i++)
        row_clear(i + 2);
    row_clear(14);
    row_print(14, 1, "RAM write select: 0x%08X", write_area);
    struct cpg_overclock_setting s;
    BSC.CS2WCR.WR = WAIT_18;
    cpg_get_overclock_setting(&s);
    
    for (int i = WAIT_0; i <= WAIT_6; i++)
    {
        static const u8 IFC = DIV_4, SFC = DIV_4, BFC = DIV_4, PFC = DIV_32;
        s.FRQCR = ((PLL(8) + i * 3) << 24) + (IFC << 20) + (SFC << 12) + (BFC << 8) + PFC;
        cpg_set_overclock_setting(&s);
        BSC.CS2WCR.WW = i + 1;
        for (int FLF = raW_default[i] / (PLL(8) + i * 3 + 1) / 4096; FLF < 2048; FLF += 2)
        {
            if (write_address(FLF, write_area))
                break;
            const u32 Bphi_f = clock_freq()->Bphi_f;
            row_clear(2 + i);
            row_print(2 + i, 25, "raW_%d", i);
            row_print(2 + i, 35, "%d KHz", Bphi_f / 1000);
            raW[i] = Bphi_f;
            dupdate();
        }
    }
}

void sram_test()
{
    dclear(C_WHITE);
    row_title("SRAM Test");

    struct cpg_overclock_setting s0;
    cpg_get_overclock_setting(&s0);
    ram_read_test();
    ram_write_test();
    cpg_set_overclock_setting(&s0);
}
#endif