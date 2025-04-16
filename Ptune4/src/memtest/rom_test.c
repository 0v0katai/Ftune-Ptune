#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/clock.h>
#include <gint/display.h>

#include "settings.h"
#include "validate.h"
#include "mem_test.h"
#include "util.h"
#include "config.h"

#define ROM_WAIT(CS0WCR) (((CS0WCR) >> 7) & 0b1111)

i32 roR[] =
    { roR_0, roR_1, roR_2, roR_3, roR_4,
      roR_5, roR_6, roR_8, roR_10, roR_12,
      roR_14, roR_18 };

static void print_ROM_select(u32 *ROM_read_area)
{
    row_clear(1);
    row_print(1, 1, "ROM select: 0x%08X", ROM_read_area);
}

static void rom_read_test()
{
    u32 *ROM_read_area = ROM_BASE;
    static const u32 roR_default[] =
    { roR_0, roR_1, roR_2, roR_3, roR_4,
      roR_5, roR_6, roR_8, roR_10, roR_12};
    
    /* Slowest ROM read area search */
    struct cpg_overclock_setting s;
    clock_set_speed(CLOCK_SPEED_DEFAULT);
    cpg_get_overclock_setting(&s);
    int FLF_max;
    for (int FLF = roR_default[ROM_WAIT(s.CS0WCR)] / 262144 * clock_freq()->Bphi_div; FLF < 2048; FLF += 2)
    {
        if (read_address(FLF, ROM_WAIT(s.CS0WCR), ROM_read_area))
            break;
        FLF_max = FLF;
        print_ROM_select(ROM_read_area);
        row_print(2, 1, "%d KHz", clock_freq()->Bphi_f / 1000);
        dupdate();
        row_clear(2);
    }
    u32 *pointer = ROM_BASE;
    for (int i = 0; i < 512; i++)
    {
        if (read_address(FLF_max, ROM_WAIT(s.CS0WCR), ROM_read_area))
        {
            FLF_max -= 2;
            ROM_read_area = pointer;
            print_ROM_select(ROM_read_area);
        }
        row_print_color(1, 25, C_RED, C_WHITE, "0x%08X", pointer);
        dupdate();
        pointer += 0x10000/4;
    }

    print_ROM_select(ROM_read_area);
    for (int i = WAIT_0; i <= WAIT_12; i++)
    {
        static const u8 IFC = DIV_4, SFC = DIV_4, BFC = DIV_4, PFC = DIV_32;
        s.FRQCR = ((PLL_x6 + i * 2) << 24) + (IFC << 20) + (SFC << 12) + (BFC << 8) + PFC;
        cpg_set_overclock_setting(&s);
        for (int FLF = roR_default[i] / (PLL_x6 + i * 2 + 1) / 4096; FLF < 2048; FLF += 2)
        {
            if (read_address(FLF, i, ROM_read_area))
                break;
            static const u8 mem_wait[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12};
            const u32 Bphi_f = clock_freq()->Bphi_f;
            row_clear(2 + i);
            row_print(2 + i, 1, "roR_%d: %d KHz", mem_wait[i], Bphi_f / 1000);
            roR[i] = Bphi_f;
            dupdate();
        }
    }

    /* Rough guess */
    roR[WAIT_14] = (roR[WAIT_12] * 2 - roR[WAIT_10]) / 100 * 99;
    roR[WAIT_18] = (roR[WAIT_14] * 2 - roR[WAIT_10]) / 100 * 95;
}

void rom_test()
{
    dclear(C_WHITE);
    row_title("ROM Test");

    struct cpg_overclock_setting s0;
    cpg_get_overclock_setting(&s0);
    rom_read_test();
    cpg_set_overclock_setting(&s0);

    row_print(13, 1, "ROM margin: %d%%", ROM_MARGIN);
    row_print(14, 1, "Press any key to continue...");

    dupdate();
    getkey();
}