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
#include "config.h"

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
    bool benchmark = false;
    static const char *option[] = {"FLL:", "PLL:", "IFC:", "SFC:", "BFC:", "PFC:", 0};
    static const u8 rom_wait[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 18, 24};

#ifdef ENABLE_FP
    __printf_enable_fp();
#endif

    prof_init();
    cpg_set_overclock_permanent(true);
    init_roR(roR);

    do
    {
        static struct cpg_overclock_setting s;
        cpg_get_overclock_setting(&s);
        cpg_set_overclock_setting(&s);

        u8 current_preset = clock_get_speed();
        dclear(C_WHITE);
        print_preset(current_preset);
        fkey_menu(6, "Bench");

        row_title(VERSION " [SETTINGS] [VARS]: BSC menu");
        row_print(1, 29, "FLLFRQ:");
        row_print(2, 29, "FRQCR:");
        for (int i = 0; i < 8; i++)
        {
            static const char *csn_name[] = {"0", "2", "3", "5a"};
            static const char reg_name[] = {'B', 'W'};
            row_print(i + 3, 29, "CS%s%cCR:", csn_name[i % 4], reg_name[i >= 4]);
        }
        for (int i = 0; i < 10; i++)
            row_print(i + 1, 38, "0x%08x", *(&(s.FLLFRQ) + i));

        const clock_frequency_t f = *clock_freq();
        row_print(1, 7, "x%d", f.FLL);
        row_print(2, 7, "x%d", f.PLL);
        row_print(3, 7, "1/%d", f.Iphi_div);
        row_print(4, 7, "1/%d", f.Sphi_div);
        row_print(5, 7, "1/%d", f.Bphi_div);
        row_print(6, 7, "1/%d", f.Pphi_div);

        static const int trc_wait[4] = {3, 4, 6, 9};
        print_options(1, 1, option, select);
        row_print_color(3, 11, C_WHITE, f.Iphi_f > IFC_RED_ZONE ? C_RED : C_BLUE, "CPU", rom_wait[BSC.CS0WCR.WR]);
        row_print_color(4, 11, C_WHITE, C_BLACK, "roR %d", rom_wait[BSC.CS0WCR.WR]);
        row_print_color(5, 11, C_WHITE, C_BLACK, "CL %d", BSC.CS3WCR.A3CL + 1);
        row_print_color(6, 11, C_WHITE, C_BLACK, "TRC %d", trc_wait[BSC.CS3WCR.TRC]);

#if defined CG20
        row_print(8, 2, "[x]/[/]: +/- roR");
        row_print(9, 2, "[+]/[-]: +/- raR");
        row_print(10, 2, "[SHIFT][+]/[-]: +/- raW");
#elif defined CG50 || defined CG100
        row_print(8, 2, "[x][/]: +/- roR");
        row_print(9, 2, "[+][-]: +/- CL");
        row_print(10, 2, "[SHIFT][+][-]: +/- TRC");
#endif

        u32 freq[6] = {f.FLL * 32768, f.FLL * f.PLL * 32768, f.Iphi_f, f.Sphi_f, f.Bphi_f, f.Pphi_f};
        static const u32 red_zone[6] = {FLL_RED_ZONE, PLL_RED_ZONE, IFC_RED_ZONE, SFC_RED_ZONE, BFC_RED_ZONE, PFC_RED_ZONE};
#ifdef ENABLE_FP
        for (int i = 0; i < 6; i++)
        {
            row_print_color(i + 1, 17, freq[i] > red_zone[i] ? C_RED : C_BLACK, C_WHITE, "%3.2f", freq[i] / 1e6);
            row_print(i + 1, 24, "MHz");
        }
#else
        for (int i = 0; i < 6; i++)
        {
            row_print_color(i + 1, 17, freq[i] > red_zone[i] ? C_RED : C_BLACK, C_WHITE, "%d", freq[i] / 1000);
            row_print(i + 1, 24, "KHz");
        }
#endif

        if (benchmark)
            run_benchmark();
        else
        {
            static const char *description[] = {"FLL", "PLL", "CPU core", "SuperHyway", "Memory bus", "I/O bus"};
            static const char *type[] = {"multiplier", "frequency ratio"};
            row_print(12, 2, "%s %s", description[select], type[select >= SELECT_IFC]);
            if (select == SELECT_FLL)
                row_print(12, 34, "(Up to x1023)");
            else
                row_print(12, 34, "(Up to %d MHz)", settings[select + 1] / 1000000);
            row_highlight(12);
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
            benchmark = !benchmark;
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
        case KEY_PLUS:
            if (key.shift && BSC.CS3WCR.TRC < 3)
            {
                BSC.CS3WCR.TRC++;
                break;
            }
            modify_A3CL(CL3);
            break;
        case KEY_MINUS:
            if (key.shift && BSC.CS3WCR.TRC > 0)
            {
                BSC.CS3WCR.TRC--;
                break;
            }
            modify_A3CL(CL2);
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
                if (f.FLL == 225)
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
                if (f.PLL == 64)
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
