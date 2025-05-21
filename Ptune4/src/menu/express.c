#include <math.h>
#include <stdio.h>

#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/gint.h>
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
#include "menu.h"

enum select_option
{
    SELECT_FLL,
    SELECT_PLL,
    SELECT_IFC,
    SELECT_SFC,
    SELECT_BFC,
    SELECT_PFC
};

#ifdef ENABLE_HELP
static void help_info()
{
    #if defined CP400
    row_print(26, 2, "[KBD]: Reset to default");
    row_print(27, 2, "[DEL]: Toggle benchmark");
    row_print(28, 2, "[UP][DOWN]: Select option");
    row_print(29, 2, "[LEFT][RIGHT]: -/+ option value");
    row_print(30, 2, "[/][x]: -/+ roR");
    row_print(31, 2, "[-][+]: -/+ CL");
    row_print(32, 2, "[SHIFT][-][+]: -/+ TRC");
    row_print(33, 2, "[=]: Settings");
    row_print(34, 2, "[x]: BSC settings");
    row_print(35, 2, "[y]: Memory data & tests");
    row_print(36, 2, "[Clear]: Close help / Quit CPtune4");
    #elif defined CG100
    info_box(1, 13, "HELP");
    row_print(2, 2, "[ON]: Reset to default");
    row_print(3, 2, "[|<-][->|]: Select preset, [OK]: Confirm");
    row_print(4, 2, "[PGUP]: Toggle benchmark");
    row_print(5, 2, "[UP][DOWN]: Select option");
    row_print(6, 2, "[LEFT][RIGHT]: -/+ option value");
    row_print(7, 2, "[x][/]: +/- roR");
    row_print(8, 2, "[+][-]: +/- CL");
    row_print(9, 2, "[SHIFT][+][-]: +/- TRC");
    row_print(10, 2, "[SETTINGS]: Settings");
    row_print(11, 2, "[VARIABLE]: BSC settings");
    row_print(12, 2, "[TOOLS]: Memory data & tests");
    row_print(13, 2, "[BACK]: Close help / Quit Ptune4");
    #else
    info_box(1, 13, "HELP");
    row_print(2, 2, "[F1]-[F5]: Apply preset");
    row_print(3, 2, "[F6]: Toggle benchmark");
    row_print(4, 2, "[UP][DOWN]: Select option");
    row_print(5, 2, "[LEFT][RIGHT]: -/+ option value");
    row_print(6, 2, "[x][/]: +/- roR");
    # ifdef CG50
    row_print(7, 2, "[+][-]: +/- CL");
    row_print(8, 2, "[SHIFT][+][-]: +/- TRC");
    # else
    row_print(7, 2, "[+]/[-]: +/- raR");
    row_print(8, 2, "[SHIFT][+]/[-]: +/- raW");
    # endif
    row_print(9, 2, "[OPTN]: Memory data & tests");
    row_print(10, 2, "[VARS]: BSC settings");
    row_print(11, 2, "[SHIFT][MENU]: Settings");
    row_print(13, 2, "[EXIT]: Close help / Quit Ptune4");
    #endif
    dupdate();
    while (getkey().key != KEY_EXIT);
}
#endif

static void print_preset(int current)
{
    #if defined CG100
    fkey_action(1, "Default");
    if (current == CLOCK_SPEED_UNKNOWN)
        tab_menu(2, 5, "Current preset: Custom");
    else
        tab_menu(2, 5, "Current preset: F%d", current);
    #else
    char string[3];
    for (int i = 1; i <= 5; i++)
    {
        sprintf(string, "F%d", i);
        if (i == current)
            fkey_button(i, string);
        else
            fkey_action(i, string);
    }
    #endif
}

#if defined CG50 || defined CG100 || defined CP400
static void shift_f5_preset()
{
    /* dupdate: 5633 μs/177 FPS, INT: 211264 Dhrystone/s */
    static struct cpg_overclock_setting const settings_fxcg50_100_shift_f5 =
        { .FLLFRQ   = 0x00004384,
          .FRQCR    = 0x1F001103,   // PLL: x32, IFC: 1/2, SFC: 1/4, BFC: 1/4, PFC: 1/16
          .CS0BCR   = 0x46DA0400,   // IWW: 6
          .CS2BCR   = 0x36DA3400,
          .CS3BCR   = 0x24924400,   // IWW: 2, IWRWD: 2, IWRWS: 2, IWRRD: 2, IWRRS: 2
          .CS5aBCR  = 0x17DF0400,
          .CS0WCR   = 0x000004C0,   // WR: 12
          .CS2WCR   = 0x000003C0,
          .CS3WCR   = 0x00004953,   // TRP: 3, TRCD: 3, A3CL: 3, TRC: 9
          .CS5aWCR  = 0x000203C1
        };
    cpg_set_overclock_setting(&settings_fxcg50_100_shift_f5);
}

static void alpha_f5_preset()
{
    /* dupdate: 3896 μs/256 FPS, INT: 257122 Dhrystone/s */
    static struct cpg_overclock_setting const settings_fxcg50_100_alpha_f5 =
        { .FLLFRQ   = 0x00004384,
          .FRQCR    = 0x1F001103,   // PLL: x32, IFC: 1/2, SFC: 1/4, BFC: 1/4, PFC: 1/16
          .CS0BCR   = 0x46D80400,   // IWW: 6, IWRRS: 0
          .CS2BCR   = 0x36DA3400,
          .CS3BCR   = 0x24924400,   // IWW: 2, IWRWD: 2, IWRWS: 2, IWRRD: 2, IWRRS: 2
          .CS5aBCR  = 0x17DF0400,
          .CS0WCR   = 0x000004C0,   // WR: 12
          .CS2WCR   = 0x000003C0,
          .CS3WCR   = 0x000024D2,   // TRC: 6
          .CS5aWCR  = 0x000103C0    // WW: 0, HW: 0.5
        };
    cpg_set_overclock_setting(&settings_fxcg50_100_alpha_f5);
}
#endif

#ifdef CG100
static void cg100_getkey(key_event_t key)
{
    if (key.key == KEY_ON)
        clock_set_speed(CLOCK_SPEED_DEFAULT);
    if (key.key == KEY_NEXTTAB && key.shift)
        shift_f5_preset();
    else if (key.key == KEY_NEXTTAB && key.alpha)
        alpha_f5_preset();
    else if (key.key == KEY_PREVTAB || key.key == KEY_NEXTTAB)
    {
        u8 select_preset = CLOCK_SPEED_DEFAULT - 1;
        while (true)
        {
            tab_clear(2, 5);
            tab_action(2, 5, "%-10sSet preset: F%d%10s", "|<-", select_preset + 1, "->|");
            dupdate();
            switch (getkey().key)
            {
                case KEY_PREVTAB:
                    select_preset = (select_preset - 1 + CLOCK_SPEED_F5) % CLOCK_SPEED_F5;
                    break;
                case KEY_NEXTTAB:
                    select_preset = (select_preset + 1) % CLOCK_SPEED_F5;
                    break;
                case KEY_OK:
                case KEY_EXE:
                    clock_set_speed(select_preset + 1);
                    __attribute__((fallthrough));
                case KEY_BACK:
                    return;
            }  
        }
    }
}
#endif

#ifdef CP400
static void cp400_getkey(key_event_t key)
{
    if (key.key == KEY_KBD)
        clock_set_speed(CLOCK_SPEED_DEFAULT);
    if (key.key == KEY_Z && key.shift)
        shift_f5_preset();
    else if (key.key == KEY_POWER && key.shift)
        alpha_f5_preset();
}
#endif

static void print_express_cpg_bsc(struct cpg_overclock_setting s)
{
    #ifdef CP400
    row_print(13, 2, "FLLFRQ: 0x%08X", s.FLLFRQ);
    row_print(13, 21, "FRQCR: 0x%08X", s.FRQCR);
    for (int i = 0; i < 4; i++)
    {
        static const char *csn_name[] = {"0", "2", "3", "5A"};
        row_print(i + 14, 2, "CS%sBCR: 0x%08X", csn_name[i], *(&(s.CS0BCR) + i));
        row_print(i + 14, 21, "CS%sWCR: 0x%08X", csn_name[i], *(&(s.CS0WCR) + i));
    }
    #else
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
    #endif
}

#if defined CP400
# define KEY_DISPLAY_ROW 8
# define REG_DISPLAY_X 7
# define WAIT_DISPLAY_X 13
# define SPEED_DISPLAY_X 21
#else
# define KEY_DISPLAY_ROW 9
# define REG_DISPLAY_X 6
# define WAIT_DISPLAY_X 11
# define SPEED_DISPLAY_X 17
#endif

void express_menu()
{
    key_event_t key;
    u8 select = SELECT_FLL;
    bool benchmark = false;
    bool update = false;
    bool spread_spectrum = false;
    static const char *option[] = {"FLL:", "PLL:", "IFC:", "SFC:", "BFC:", "PFC:", 0};

    do
    {
        static struct cpg_overclock_setting s;
        cpg_get_overclock_setting(&s);
        cpg_set_overclock_setting(&s);
        CPG.SSCGCR.SSEN = spread_spectrum;
        if (update && AUTO_REDUCE_WAIT)
            BSC.CS0WCR.WR = best_rom_wait(clock_freq()->Bphi_f);

        u8 current_preset = clock_get_speed();
        dclear(C_WHITE);
        #ifdef ENABLE_HELP
        set_help_function(help_info);
        #endif
        #if !defined CP400
        print_preset(current_preset);
        fkey_menu(6, "Bench");
        #endif

        row_title(VERSION);
        print_express_cpg_bsc(s);

        const clock_frequency_t f = *clock_freq();
        row_print(1, REG_DISPLAY_X, "x%d", f.FLL);
        row_print(2, REG_DISPLAY_X, "x%d", f.PLL);
        row_print(3, REG_DISPLAY_X, "1/%d", f.Iphi_div);
        row_print(4, REG_DISPLAY_X, "1/%d", f.Sphi_div);
        row_print(5, REG_DISPLAY_X, "1/%d", f.Bphi_div);
        row_print(6, REG_DISPLAY_X, "1/%d", f.Pphi_div);

        print_options(1, 1, option, select);
        row_print_color(1, WAIT_DISPLAY_X, C_WHITE, C_BLACK, CPG.FLLFRQ.SELXM ? "XM 1/2" : "XM 1");
        row_print_color(2, WAIT_DISPLAY_X, spread_spectrum ? C_GREEN : C_WHITE, C_BLACK, spread_spectrum ? "SS On" : "SS Off");
        row_print_color(3, WAIT_DISPLAY_X, C_WHITE, f.Iphi_f > IFC_RED_ZONE ? C_RED : C_BLUE, "CPU");
        row_print_color(4, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "roR %d", WR_equivalent(BSC.CS0WCR.WR));
        #if defined CG20
        row_print_color(5, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "raR %d", WR_equivalent(BSC.CS2WCR.WR));
        if (BSC.CS2WCR.WW)
            row_print_color(6, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "raW %d", BSC.CS2WCR.WW - 1);
        else
            row_print_color(6, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "raW =R");
        #elif defined CG50 || defined CG100 || defined CP400
        row_print_color(5, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "CL %d", BSC.CS3WCR.A3CL + 1);
        row_print_color(6, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "TRC %d", TRC_equivalent(BSC.CS3WCR.TRC));
        #endif

        #ifdef ENABLE_USB
        if (usb_is_open())
            row_print(KEY_DISPLAY_ROW, 2, "Capture");
        else
            row_print(KEY_DISPLAY_ROW, 2, "Open USB");

        # ifdef CG100
        row_print(KEY_DISPLAY_ROW, 12, "[SHIFT][x10^]");
        # else
        row_print(KEY_DISPLAY_ROW, 12, "[SHIFT][7]");
        # endif
        #endif

        #ifdef ENABLE_HELP
        row_print(KEY_DISPLAY_ROW + 1, 2, "Help");
        # ifdef CG100
        row_print(KEY_DISPLAY_ROW + 1, 12, "[CATALOG]");
        # else
        row_print(KEY_DISPLAY_ROW + 1, 12, "[SHIFT][4]");
        # endif
        #endif

        u32 freq[6] =
            {f.FLL * 32768, f.FLL * f.PLL * 32768, f.Iphi_f, f.Sphi_f, f.Bphi_f, f.Pphi_f};
        static const u32 red_zone[6] =
            {FLL_RED_ZONE, PLL_RED_ZONE, IFC_RED_ZONE, SFC_RED_ZONE, BFC_RED_ZONE, PFC_RED_ZONE};
        for (int i = 0; i < 6; i++)
        {
            row_print_color(i + 1, SPEED_DISPLAY_X, freq[i] > red_zone[i]
                ? C_RED : C_BLACK, C_WHITE, "%d", freq[i] / 1000);
            row_print(i + 1, SPEED_DISPLAY_X + 7, "KHz");
        }

        #if !defined CP400
        if (!benchmark)
        #endif
        {
            static const char *description[] = {"FLL", "PLL", "CPU", "SuperHyway", "Bus", "I/O"};
            static const char *type[] = {"multiplier", "clock divider"};
            row_print(KEY_DISPLAY_ROW + 3, 2, "%s %s", description[select], type[select >= SELECT_IFC]);
            #if defined CP400
            if (select == SELECT_FLL)
                row_print(11, 27, "(Max x1023)");
            else
                row_print(11, 27, "(Max %d MHz)", settings[select + 1] / 1000000);
            row_highlight(11);
            #else
            if (select == SELECT_FLL)
                row_print(12, 35, "(Max x1023)");
            else
                row_print(12, 35, "(Max %d MHz)", settings[select + 1] / 1000000);
            row_highlight(12);
            #endif
        }

        if (benchmark)
            run_benchmark();

        dupdate();
        key = getkey();

        static const u8 divs_ratio[3][3] =
        {
            {2, 0, 0},
            {8, 4, 0},
            {16, 8, 8}
        };
        u8 divs[4] = {f.Iphi_div, f.Sphi_div, f.Bphi_div, f.Pphi_div};
        update = false;
        #ifdef CG100
        cg100_getkey(key);
        #elif defined CP400
        cp400_getkey(key);
        #endif
        switch (key.key)
        {
            #if !defined CG100 && !defined CP400
            case KEY_F1:
            case KEY_F2:
            case KEY_F3:
            case KEY_F4:
            case KEY_F5:
            # ifdef CG50
            if (key.shift)
                shift_f5_preset();
            else if (key.alpha)
                alpha_f5_preset();
            else
            # endif
            clock_set_speed(key.key - KEY_F1 + 1);
            break;
            #endif

            case KEY_EXPRESS_BENCHMARK:
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

            case KEY_EXPRESS_SELXM:
                if (f.FLL >= 1024)
                    break;
                CPG.FLLFRQ.lword = CPG.FLLFRQ.SELXM ? f.FLL : 0x4000 + (f.FLL << 1);
                break;

            case KEY_EXPRESS_SS:
                if (f.PLL > 32)
                    break;
                spread_spectrum = !spread_spectrum;
                break;
            case KEY_MUL:
            case KEY_DIV:
                bsc_modify(CS0WCR_WR_ptr, key.key == KEY_MUL ? 1 : -1);
                break;
            case KEY_PLUS:
            case KEY_MINUS:
                #if defined CG50 || defined CG100 || defined CP400
                bsc_modify(key.shift ? CS3WCR_TRC_ptr : CS3WCR_CL_ptr, key.key == KEY_PLUS ? 1 : -1);
                #else
                bsc_modify(key.shift ? CS2WCR_WW_ptr : CS2WCR_WR_ptr, key.key == KEY_PLUS ? 1 : -1);
                #endif
                break;

            case KEY_EXPRESS_SETTINGS:
            #if !defined CG100 && !defined CP400
                if (!key.shift)
                    break;
            #endif
                settings_menu();
                break;
            case KEY_EXPRESS_BSC:
                bsc_menu();
                break;
            case KEY_EXPRESS_MEMDATA:
                mem_data_menu();
                break;

            case KEY_LEFT:
                update = true;
                if (select == SELECT_FLL)
                {
                    if (f.FLL == 225)
                        break;
                    CPG.FLLFRQ.FLF -= 1 + CPG.FLLFRQ.SELXM;
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
                    if (f.FLL == (2047 >> CPG.FLLFRQ.SELXM))
                        break;
                    CPG.FLLFRQ.FLF += 1 + CPG.FLLFRQ.SELXM;
                    if (exceed_limit())
                    {
                        CPG.FLLFRQ.FLF -= 1 + CPG.FLLFRQ.SELXM;
                        break;
                    }
                    divs[SELECT_PFC - 2] <<= auto_down_PFC();
                }
                else if (select == SELECT_PLL)
                {
                    if (f.PLL == (64 >> spread_spectrum))
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
                    const i32 limit[4] = {CPU_CLK_MAX, SHW_CLK_MAX, BUS_CLK_MAX, IO_CLK_MAX};
                    if (!UNLOCKED_MODE && (fs[check] << 1) > limit[check])
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
            const u8 new_CS0WCR_WR = best_rom_wait(Bphi_f);
            if (new_CS0WCR_WR > BSC.CS0WCR.WR)
                BSC.CS0WCR.WR = new_CS0WCR_WR;
            #if defined CG50 || defined CG100 || defined CP400
            const u8 new_CS3WCR_TRC = best_TRC(Bphi_f);
            if (new_CS3WCR_TRC > BSC.CS3WCR.TRC || AUTO_REDUCE_WAIT)
                BSC.CS3WCR.TRC = new_CS3WCR_TRC;
            #else
            const u8 new_CS2WCR_WR = best_ram_read(Bphi_f);
            const u8 new_CS2WCR_WW = best_ram_write(Bphi_f);
            if (new_CS2WCR_WR > BSC.CS2WCR.WR || AUTO_REDUCE_WAIT)
                BSC.CS2WCR.WR = new_CS2WCR_WR;
            if (new_CS2WCR_WW > BSC.CS2WCR.WW || AUTO_REDUCE_WAIT)
                BSC.CS2WCR.WW = new_CS2WCR_WW;
            #endif
        }
    } while (key.key != KEY_EXIT);
    return;
}
