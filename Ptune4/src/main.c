#include <math.h>
#include <stdlib.h>
#include <fxlibc/printf.h>

#include <gint/display.h>
#include <gint/keyboard.h>

#include <gint/clock.h>
#include <libprof.h>
#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/gdb.h>

#include "util.h"
#include "validate.h"
#include "mem_test.h"
#include "dhrystone.h"
#include "settings.h"
#include "bsc.h"

// #define ENABLE_FP
// #define ENABLE_GDB

i32 roR[] =
    {
        roR_0, roR_1, roR_2, roR_3, roR_4,
        roR_5, roR_6, roR_8, roR_10, roR_12,
        roR_14, roR_18};

enum select_option
{
    SELECT_FLL,
    SELECT_PLL,
    SELECT_IFC,
    SELECT_SFC,
    SELECT_BFC,
    SELECT_PFC
};

static void print_preset(int current)
{
    char string[3];
    for (int i = 1; i <= 5; i++)
    {
        sprintf(string, "F%d", i);
        if (i == current)
            fkey_button(i, string);
        else
            fkey_action(i, string);
    }
}

int main()
{   
#ifdef ENABLE_GDB
    gdb_start_on_exception();
    __asm__("trapa #42");
#endif
    
    key_event_t key;
    u8 select = SELECT_FLL;
    bool enable_dhrystone = false;
    const char *option[] = {"FLL:", "PLL:", "IFC:", "SFC:", "BFC:", "PFC:", 0};
    const u8 rom_wait[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 18, 24};

#ifdef ENABLE_FP
    __printf_enable_fp();
#endif

    prof_init();
    cpg_set_overclock_permanent(true);
    init_roR(roR);

    do
    {
        struct cpg_overclock_setting s;
        cpg_get_overclock_setting(&s);
        cpg_set_overclock_setting(&s);

        u8 current_preset = clock_get_speed();
        dclear(C_WHITE);
        print_preset(current_preset);
        fkey_menu(6, "Bench");

        row_title("Ptune4 v0.02 (fx-CG100/Graph Math+)");
        row_print(1, 29, "FLLFRQ:");
        row_print(2, 29, "FRQCR:");
        row_print(3, 29, "CS0BCR:");
        row_print(4, 29, "CS2BCR:");
        row_print(5, 29, "CS3BCR:");
        row_print(6, 29, "CS5aBCR:");
        row_print(7, 29, "CS0WCR:");
        row_print(8, 29, "CS2WCR:");
        row_print(9, 29, "CS3WCR:");
        row_print(10, 29, "CS5aWCR:");

        row_print(1, 38, "0x%08x", s.FLLFRQ);
        row_print(2, 38, "0x%08x", s.FRQCR);
        row_print(3, 38, "0x%08x", s.CS0BCR);
        row_print(4, 38, "0x%08x", s.CS2BCR);
        row_print(5, 38, "0x%08x", s.CS3BCR);
        row_print(6, 38, "0x%08x", s.CS5aBCR);
        row_print(7, 38, "0x%08x", s.CS0WCR);
        row_print(8, 38, "0x%08x", s.CS2WCR);
        row_print(9, 38, "0x%08x", s.CS3WCR);
        row_print(10, 38, "0x%08x", s.CS5aWCR);

        print_options(1, 1, option, select);
        row_print(8, 2, "roR %d", rom_wait[BSC.CS0WCR.WR]);
        row_print(8, 12, "CL %d", rom_wait[BSC.CS3WCR.A3CL + 1]);

        const clock_frequency_t f = *clock_freq();
        row_print(1, 7, "%d", f.FLL);
        row_print(2, 7, "x%d", f.PLL);
        row_print(3, 7, "1/%d", f.Iphi_div);
        row_print(4, 7, "1/%d", f.Sphi_div);
        row_print(5, 7, "1/%d", f.Bphi_div);
        row_print(6, 7, "1/%d", f.Pphi_div);

#ifdef ENABLE_FP
        row_print(1, 12, "(%3.2f MHz)", f.FLL * 32768 / 1e6);
        row_print(2, 12, "(%3.2f MHz)", f.FLL * f.PLL * 32768 / 1e6);
        row_print(3, 12, "(%3.2f MHz)", f.Iphi_f / 1e6);
        row_print(4, 12, "(%3.2f MHz)", f.Sphi_f / 1e6);
        row_print(5, 12, "(%3.2f MHz)", f.Bphi_f / 1e6);
        row_print(6, 12, "(%3.2f MHz)", f.Pphi_f / 1e6);
#else
        row_print(1, 12, "(%d KHz)", f.FLL * 32768 / 1000);
        row_print(2, 12, "(%d KHz)", f.FLL * f.PLL * 32768 / 1000);
        row_print(3, 12, "(%d KHz)", f.Iphi_f / 1000);
        row_print(4, 12, "(%d KHz)", f.Sphi_f / 1000);
        row_print(5, 12, "(%d KHz)", f.Bphi_f / 1000);
        row_print(6, 12, "(%d KHz)", f.Pphi_f / 1000);
#endif

        u32 time_dupdate = prof_exec(dupdate());
        row_print(12, 1, "dupdate(): %d us", time_dupdate);
#ifdef ENABLE_FP
        row_print(13, 1, "(%3.2f FPS)", 1000000.0f / time_dupdate);
#else
        row_print(13, 1, "(%d FPS)", 1000000 / time_dupdate);
#endif

        if (enable_dhrystone)
        {
            u32 time_dhrystone = prof_exec(dhrystone(DHRY_LOOP));
            row_print(12, 25, "Dhry10000: %d us", time_dhrystone);
            row_print(13, 25, "(%llu Dhry/s)", DHRY_LOOP * 1000000ull / time_dhrystone);
        }

        dupdate();
        key = getkey();

        switch (key.key)
        {
        case KEY_F1:
        case KEY_F2:
        case KEY_F3:
        case KEY_F4:
        case KEY_F5:
            clock_set_speed(key.key - KEY_F1 + 1);
            break;
        case KEY_PREVTAB:
            clock_set_speed(abs(current_preset - 1));
            break;
        case KEY_NEXTTAB:
            clock_set_speed(current_preset % 5 + 1);
            break;

        case KEY_F6:
        case KEY_PAGEUP:
            enable_dhrystone = !enable_dhrystone;
            break;

        case KEY_UP:
            if (select)
                select--;
            break;
        case KEY_DOWN:
            if (select < SELECT_PFC)
                select++;
            break;

        case KEY_MUL:
            if (BSC.CS0WCR.WR < WAIT_24)
                BSC.CS0WCR.WR++;
            break;
        case KEY_DIV:
            if (BSC.CS0WCR.WR > best_rom_wait(f.Bphi_f))
                BSC.CS0WCR.WR--;
            break;

        case KEY_MENU:
            if (!key.shift)
                break;
            __attribute__((fallthrough));
        case KEY_SETTINGS:
            settings_menu();
            break;
        case KEY_VARS:
            bsc_menu();
            break;

        case KEY_LEFT:
            switch (select)
            {
            case SELECT_FLL:
                if (CPG.FLLFRQ.FLF == 450)
                    break;
                CPG.FLLFRQ.FLF -= 2;
                down_roR_IWW();
                auto_up_PFC();
                break;
            case SELECT_PLL:
                if (f.PLL == 1)
                    break;
                CPG.FRQCR.STC--;
                down_roR_IWW();
                auto_up_PFC();
                break;
            case SELECT_IFC:
                if (f.Iphi_div == 64)
                    break;
                if (f.Iphi_div == f.Pphi_div)
                    CPG.FRQCR.P1FC++;
                if (f.Iphi_div == f.Bphi_div)
                {
                    CPG.FRQCR.BFC++;
                    down_roR_IWW();
                }
                if (f.Iphi_div == f.Sphi_div)
                    CPG.FRQCR.SFC++;
                CPG.FRQCR.IFC++;
                break;
            case SELECT_SFC:
                if (f.Sphi_div == 64)
                    break;
                if (f.Sphi_div == f.Pphi_div)
                    CPG.FRQCR.P1FC++;
                if (f.Sphi_div == f.Bphi_div)
                {
                    CPG.FRQCR.BFC++;
                    down_roR_IWW();
                }
                CPG.FRQCR.SFC++;
                if (f.Sphi_div / f.Iphi_div >= 2)
                    CPG.FRQCR.IFC++;
                break;
            case SELECT_BFC:
                if (f.Bphi_div == 64)
                    break;
                if (f.Bphi_div == f.Pphi_div)
                    CPG.FRQCR.P1FC++;
                CPG.FRQCR.BFC++;
                down_roR_IWW();
                if (f.Bphi_div / f.Sphi_div >= 4)
                    CPG.FRQCR.SFC++;
                if (f.Bphi_div / f.Iphi_div >= 8)
                    CPG.FRQCR.IFC++;
                break;
            case SELECT_PFC:
                if (f.Pphi_div == 64)
                    break;
                CPG.FRQCR.P1FC++;
                if (f.Pphi_div / f.Bphi_div >= 8)
                {
                    CPG.FRQCR.BFC++;
                    down_roR_IWW();
                }
                if (f.Pphi_div / f.Sphi_div >= 8)
                    CPG.FRQCR.SFC++;
                if (f.Pphi_div / f.Iphi_div >= 16)
                    CPG.FRQCR.IFC++;
                break;
            }
            break;
        case KEY_RIGHT:
            switch (select)
            {
            case SELECT_FLL:
                if (f.FLL == 1023)
                    break;
                CPG.FLLFRQ.FLF += 2;
                if (exceed_limit())
                {
                    CPG.FLLFRQ.FLF -= 2;
                    break;
                }
                up_roR_IWW();
                auto_down_PFC();
                break;
            case SELECT_PLL:
                if (f.PLL == 32)
                    break;
                CPG.FRQCR.STC++;
                if (exceed_limit())
                {
                    CPG.FRQCR.STC--;
                    break;
                }
                up_roR_IWW();
                auto_down_PFC();
                break;
            case SELECT_IFC:
                if (f.Iphi_div == 2)
                    break;
                if (f.Iphi_f << 1 > CPU_CLK_MAX)
                    break;
                if (f.Iphi_div > f.Sphi_div)
                    break;
                CPG.FRQCR.IFC--;
                if (f.Iphi_div < f.Sphi_div)
                    CPG.FRQCR.SFC--;
                if (f.Bphi_div / f.Iphi_div >= 8)
                    up_BFC();
                if (f.Pphi_div / f.Iphi_div >= 16)
                    up_PFC();
                break;
            case SELECT_SFC:
                if (f.Sphi_div == 2)
                    break;
                if (f.Sphi_f << 1 > SHW_CLK_MAX)
                    break;
                if (f.Sphi_div < f.Iphi_div)
                    break;
                if (f.Sphi_div == f.Iphi_div)
                    CPG.FRQCR.IFC--;
                CPG.FRQCR.SFC--;
                if (f.Bphi_div / f.Sphi_div >= 4)
                    up_BFC();
                if (f.Pphi_div / f.Sphi_div >= 8)
                    up_PFC();
                break;
            case SELECT_BFC:
                if (f.Bphi_div == 2)
                    break;
                if (f.Bphi_f << 1 > BUS_CLK_MAX)
                    break;
                if (f.Bphi_div == f.Sphi_div)
                {
                    if (f.Bphi_div == f.Iphi_div)
                        CPG.FRQCR.IFC--;
                    CPG.FRQCR.SFC--;
                }
                up_BFC();
                if (f.Pphi_div / f.Bphi_div >= 8)
                    up_PFC();
                break;
            case SELECT_PFC:
                if (f.Pphi_div == 2)
                    break;
                if (f.Pphi_f << 1 > IO_CLK_MAX)
                    break;
                if (f.Pphi_div == f.Bphi_div)
                {
                    if (f.Pphi_div == f.Sphi_div)
                    {
                        if (f.Pphi_div == f.Iphi_div)
                            CPG.FRQCR.IFC--;
                        CPG.FRQCR.SFC--;
                    }
                    up_BFC();
                }
                CPG.FRQCR.P1FC--;
                break;
            }
            break;
        }
    } while (key.key != KEY_EXIT);
    return 1;
}
