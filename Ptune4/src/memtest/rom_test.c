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

#if defined CP400
# define SELECT_DISPLAY_ROW 36
#else
# define SELECT_DISPLAY_ROW 14
#endif

i32 roR[] =
    { roR_0, roR_1, roR_2, roR_3, roR_4,
      roR_5, roR_6, roR_8, roR_10, roR_12,
      roR_14, roR_18 };

static void print_RAM_read_select(u32 *ROM_read_area)
{
    row_clear(SELECT_DISPLAY_ROW);
    row_print(SELECT_DISPLAY_ROW, 1, "ROM select: 0x%08X", ROM_read_area);
}

static void rom_read_test(mem_test_settings test_settings)
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
    for (int FLF = ROM_SEARCH_FLF_START; FLF < 2048; FLF += 2)
    {
        BSC.CS0WCR.WR = ROM_WAIT(s.CS0WCR);
        if (read_address(FLF, ROM_read_area))
            break;
        FLF_max = FLF;
        print_RAM_read_select(ROM_read_area);
        row_print(1, 1, "%d KHz", clock_freq()->Bphi_f / 1000);
        dupdate();
        row_clear(1);
    }
    u32 *pointer = ROM_BASE;
    for (int i = 0; i < 512; i++)
    {
        BSC.CS0WCR.WR = ROM_WAIT(s.CS0WCR);
        if (read_address(FLF_max, ROM_read_area))
        {
            FLF_max -= 2;
            ROM_read_area = pointer;
        }
        print_RAM_read_select(ROM_read_area);
        row_print_color(SELECT_DISPLAY_ROW, 25, C_RED, C_WHITE, "0x%08X", pointer);
        dupdate();
        row_clear(14);
        pointer += 0x10000/4;
    }

    print_RAM_read_select(ROM_read_area);
    for (int i = WAIT_0; i <= WAIT_12; i++)
    {
        if (i == WAIT_10 && !test_settings.roR_10_check)
        {
            roR[WAIT_10] = (roR[WAIT_8] * 2 - roR[WAIT_6]) / 100 * 99;
            continue;
        }
        else if (i == WAIT_12 && !test_settings.roR_12_check)
        {
            roR[WAIT_12] = (roR[WAIT_10] * 2 - roR[WAIT_8]) / 100 * 99;
            continue;
        }
        static const u8 IFC = DIV_4, SFC = DIV_4, BFC = DIV_4, PFC = DIV_32;
        s.FRQCR = ((PLL(6) + i * 2) << 24) + (IFC << 20) + (SFC << 12) + (BFC << 8) + PFC;
        s.CS0WCR = 0x000005C0;
        cpg_set_overclock_setting(&s);
        u32 Bphi_f;
        for (int FLF = roR_default[i] / (PLL(6) + i * 2 + 1) / 4096; FLF < 2048; FLF += 2)
        {
            BSC.CS0WCR.WR = i;
            if (read_address(FLF, ROM_read_area))
                break;
            Bphi_f = clock_freq()->Bphi_f;
            row_clear(2 + i);
            row_print(2 + i, 1, "roR_%d", WR_equivalent(i));
            row_print(2 + i, 11, "%d KHz", Bphi_f / 1000);
            dupdate();
        }
        roR[i] = Bphi_f;
    }

    /* Rough guess */
    roR[WAIT_14] = (roR[WAIT_12] * 2 - roR[WAIT_10]) / 100 * 99;
    roR[WAIT_18] = (roR[WAIT_14] * 2 - roR[WAIT_10]) / 100 * 95;
}

void rom_test(mem_test_settings test_settings)
{
    dclear(C_WHITE);
    row_title("ROM Test");

    struct cpg_overclock_setting s0;
    cpg_get_overclock_setting(&s0);
    rom_read_test(test_settings);
    cpg_set_overclock_setting(&s0);
}