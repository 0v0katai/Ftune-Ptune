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

#include <gint/usb.h>
#include <gint/usb-ff-bulk.h>

#include "util.h"
#include "validate.h"
#include "mem_test.h"
#include "dhrystone.h"
#include "settings.h"
#include "bsc.h"
#include "config.h"
#include "bench.h"

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

#if defined CG50 || defined CG100
static void fxcg50_100_expt_f5_preset()
{
    static struct cpg_overclock_setting const settings_fxcg50_100_expt_f5 =
        { .FLLFRQ   = 0x00004000 + 900,
          .FRQCR    = 0x1F001103,
          .CS0BCR   = 0x46D80400,
          .CS2BCR   = 0x36DA3400,
          .CS3BCR   = 0x04904400,
          .CS5aBCR  = 0x17DF0400,
          .CS0WCR   = 0x000004C0,
          .CS2WCR   = 0x000003C0,
          .CS3WCR   = 0x000024D2,
          .CS5aWCR  = 0x000103C0 };
    cpg_set_overclock_setting(&settings_fxcg50_100_expt_f5);
}
#endif

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
    static const u8 mem_wait[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 18, 24};

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
            static const char *csn_name[] = {"0", "2", "3", "5A"};
            static const char reg_name[] = {'B', 'W'};
            row_print(i + 3, 29, "CS%s%cCR:", csn_name[i % 4], reg_name[i >= 4]);
        }
        for (int i = 0; i < 10; i++)
            row_print(i + 1, 38, "0x%08X", *(&(s.FLLFRQ) + i));

        const clock_frequency_t f = *clock_freq();
        row_print(1, 7, "x%d", f.FLL);
        row_print(2, 7, "x%d", f.PLL);
        row_print(3, 7, "1/%d", f.Iphi_div);
        row_print(4, 7, "1/%d", f.Sphi_div);
        row_print(5, 7, "1/%d", f.Bphi_div);
        row_print(6, 7, "1/%d", f.Pphi_div);

        print_options(1, 1, option, select);
        row_print_color(3, 11, C_WHITE, f.Iphi_f > IFC_RED_ZONE ? C_RED : C_BLUE, "CPU");
        row_print_color(4, 11, C_WHITE, C_BLACK, "roR %d", mem_wait[BSC.CS0WCR.WR]);
#if defined CG20
        row_print_color(5, 11, C_WHITE, C_BLACK, "raR %d", mem_wait[BSC.CS2WCR.WR]);
        if (BSC.CS2WCR.WW)
            row_print_color(6, 11, C_WHITE, C_BLACK, "raW %d", BSC.CS2WCR.WW - 1);
        else
            row_print_color(6, 11, C_WHITE, C_BLACK, "raW =R");
#elif defined CG50 || defined CG100
        static const int trc_wait[4] = {3, 4, 6, 9};
        row_print_color(5, 11, C_WHITE, C_BLACK, "CL %d", BSC.CS3WCR.A3CL + 1);
        row_print_color(6, 11, C_WHITE, C_BLACK, "TRC %d", trc_wait[BSC.CS3WCR.TRC]);
#endif

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
#if defined CG20
        static const u32 red_zone[6] = {FLL_RED_ZONE, PLL_RED_ZONE, IFC_RED_ZONE, SFC_RED_ZONE, BFC_RED_ZONE, PFC_RED_ZONE};
#elif defined CG50 || defined CG100
        static const u32 red_zone[6] = {FLL_RED_ZONE, PLL_RED_ZONE, IFC_RED_ZONE, SFC_RED_ZONE, 0, PFC_RED_ZONE};
        static const u32 BFC_red_zone[4] =
            {BFC_TRC3_RED_ZONE, BFC_TRC4_RED_ZONE, BFC_TRC6_RED_ZONE, BFC_TRC9_RED_ZONE};
#endif
        for (int i = 0; i < 6; i++)
        {

    #ifdef ENABLE_FP
            row_print(i + 1, 24, "MHz");
            row_print_color(i + 1, 17, freq[i] >
            #if defined CG50 || defined CG100
                (i == 4 ? BFC_red_zone[BSC.CS3WCR.TRC] : red_zone[i])
            #else
                red_zone[i]
            #endif
                ? C_RED : C_BLACK, C_WHITE, "%3.2f", freq[i] / 1e6);
    #else
            row_print(i + 1, 24, "KHz");
            row_print_color(i + 1, 17, freq[i] >
            #if defined CG50 || defined CG100
                (i == 4 ? BFC_red_zone[BSC.CS3WCR.TRC] : red_zone[i])
            #else
                red_zone[i]
            #endif
                ? C_RED : C_BLACK, C_WHITE, "%d", freq[i] / 1000);
    #endif
        }

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

        static const u8 divs_ratio[3][3] =
        {
            {2, 0, 0},
            {8, 4, 0},
            {16, 8, 8}
        };
        u8 divs[4] = {f.Iphi_div, f.Sphi_div, f.Bphi_div, f.Pphi_div};
        bool update = false;
        switch (key.key)
        {
        case KEY_F1:
        case KEY_F2:
        case KEY_F3:
        case KEY_F4:
        case KEY_F5:
#if defined CG50 || defined CG100
            if (key.shift)
            {
                fxcg50_100_expt_f5_preset();
                break;
            }
#endif
            clock_set_speed(key.key - KEY_F1 + 1);
            break;
        case KEY_PREVTAB:
            clock_set_speed(abs(current_preset - 1));
            break;
        case KEY_NEXTTAB:
#if defined CG50 || defined CG100
            if (key.shift)
            {
                fxcg50_100_expt_f5_preset();
                break;
            }
#endif
            clock_set_speed(current_preset % 5 + 1);
            break;

        case KEY_F6:
        case KEY_PAGEUP:
            benchmark = !benchmark;
            break;

#if defined ENABLE_USB
        case KEY_OPTN:
            if (!usb_is_open())
            {
                usb_interface_t const *interfaces[] = {&usb_ff_bulk, NULL};
                usb_open(interfaces, GINT_CALL_NULL);
                usb_open_wait();
            }
            else
                usb_close();
            break;
        case KEY_EXP:
        case KEY_7:
            if (key.shift && usb_is_open())
                usb_fxlink_screenshot(true);
            break;
#endif

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
#if defined CG20
            if (key.shift && BSC.CS2WCR.WW < WAIT_6 + 1)
            {
                BSC.CS2WCR.WW++;
                break;
            }
            BSC.CS2WCR.WR++;
#elif defined CG50 || defined CG100
            if (key.shift && BSC.CS3WCR.TRC < 3)
            {
                BSC.CS3WCR.TRC++;
                break;
            }
            modify_A3CL(CL3);
#endif
            break;
        case KEY_MINUS:
#if defined CG20
            if (key.shift && BSC.CS2WCR.WW > 0)
            {
                BSC.CS2WCR.WW--;
                break;
            }
            BSC.CS2WCR.WR--;
#elif defined CG50 || defined CG100
            if (key.shift && BSC.CS3WCR.TRC > best_TRC(f.Bphi_f))
            {
                BSC.CS3WCR.TRC--;
                break;
            }
            modify_A3CL(CL2);
#endif
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
            update = true;
            if (select == SELECT_FLL)
            {
                if (f.FLL == 225)
                    break;
                CPG.FLLFRQ.FLF -= 2;
                divs[SELECT_PFC - 2] >>= auto_up_PFC();
            }
            else if (select == SELECT_PLL)
            {
                if (f.PLL == 1)
                    break;
                CPG.FRQCR.STC--;
                divs[SELECT_PFC - 2] >>= auto_up_PFC();
            }
            else
            {
                const u8 check = select - 2;
                if (divs[check] == 64)
                    break;
                for (int i = check + 1; i <= 3; i++)
                    if (divs[check] == divs[i])
                        divs[i] <<= 1;
                divs[check] <<= 1;
                for (int i = check - 1; i >= 0; i--)
                    if (divs[check] / divs[i] > divs_ratio[check - 1][i])
                        divs[i] <<= 1; 
            }
            break;
        case KEY_RIGHT:
            update = true;
            if (select == SELECT_FLL)
            {
                if (f.FLL == 1023)
                    break;
                CPG.FLLFRQ.FLF += 2;
                if (exceed_limit())
                {
                    CPG.FLLFRQ.FLF -= 2;
                    break;
                }
                divs[SELECT_PFC - 2] <<= auto_down_PFC();
            }
            else if (select == SELECT_PLL)
            {
                if (f.PLL == 64)
                    break;
                CPG.FRQCR.STC++;
                if (exceed_limit())
                {
                    CPG.FRQCR.STC--;
                    break;
                }
                divs[SELECT_PFC - 2] <<= auto_down_PFC();
            }
            else
            {
                const u8 check = select - 2;
                if (divs[check] == 2)
                    break;
                const i32 fs[4] = {f.Iphi_f, f.Sphi_f, f.Bphi_f, f.Pphi_f};
#if defined CG50 || defined CG100
                const i32 limit[4] = {CPU_CLK_MAX, SHW_CLK_MAX, BUS_CLK_MAX(BSC.CS3WCR.TRC), IO_CLK_MAX};
#else
                const i32 limit[4] = {CPU_CLK_MAX, SHW_CLK_MAX, BUS_CLK_MAX, IO_CLK_MAX};
#endif
                if ((fs[check] << 1) > limit[check] && check != SELECT_BFC - 2)
                    break;
                for (int i = check - 1; i >= 0; i--)
                    if (divs[check] == divs[i])
                        divs[i] >>= 1;
                divs[check] >>= 1;
                for (int i = check + 1; i <= 3; i++)
                    if (divs[i] / divs[check] > divs_ratio[i - 1][check])
                        divs[i] >>= 1;
            }
            break;
        }
        if (update)
        {
            CPG.FRQCR.lword &= 0xFF000000;
            for (int i = 0; i < 4; i++)
            {
                static const u8 field[4] = {20, 12, 8, 0};
                while ((divs[i] >>= 1) != 1)
                    CPG.FRQCR.lword += 1 << field[i];
            }
            cpg_compute_freq();
            const u32 Bphi_f = clock_freq()->Bphi_f;
            BSC.CS0WCR.WR = best_rom_wait(Bphi_f);
#if defined CG50 || defined CG100
            BSC.CS3WCR.TRC = best_TRC(Bphi_f);
#endif
        }
    } while (key.key != KEY_EXIT);
    return 1;
}
