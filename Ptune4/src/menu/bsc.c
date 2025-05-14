#include <stdio.h>
#include <gint/display.h>
#include <gint/clock.h>
#include "bsc.h"
#include "validate.h"
#include "util.h"
#include "config.h"

#ifdef ENABLE_HELP
static void help_info()
{
    #if defined CP400
    info_box(15, 7, "HELP");
    row_print(16, 2, "[-][+]: -/+ option value");
    row_print(17, 2, "[x]: Toggle BCR/WCR mode");
    row_print(18, 2, "[UP][DOWN]: Select option");
    row_print(19, 2, "[LEFT][RIGHT]: Select CSn area");
    row_print(21, 2, "[Clear]: Close help / < Express menu");
    #elif defined CG100
    info_box(3, 9, "HELP");
    row_print(4, 2, "[+]: Increase option value");
    row_print(5, 2, "[-]: Decrease option value");
    row_print(6, 2, "[|<-][->|]: Toggle BCR/WCR mode");
    row_print(7, 2, "[UP][DOWN]: Select option");
    row_print(8, 2, "[LEFT][RIGHT]: Select CSn area");
    row_print(9, 2, "[PGUP][PGDW]: Quick jump to previous/next row");
    row_print(11, 2, "[BACK]: Close help / < Express menu");
    #else
    info_box(3, 9, "HELP");
    row_print(4, 2, "[F1][+]: Increase option value");
    row_print(5, 2, "[F2][-]: Decrease option value");
    row_print(6, 2, "[F6]: Toggle BCR/WCR mode");
    row_print(7, 2, "[UP][DOWN]: Select option");
    row_print(8, 2, "[LEFT][RIGHT]: Select CSn area");
    row_print(11, 2, "[EXIT]: Close help / < Express menu");
    #endif
    dupdate();
    while (getkey().key != KEY_EXIT);
}
#endif

BSC_option const CS0WCR_WR_ptr = { .CSn = SELECT_CS0, .MODE = SELECT_WCR, .REG = SELECT_WR };
BSC_option const CS0WCR_WW_ptr = { .CSn = SELECT_CS0, .MODE = SELECT_WCR, .REG = SELECT_WW };
BSC_option const CS2WCR_WR_ptr = { .CSn = SELECT_CS2, .MODE = SELECT_WCR, .REG = SELECT_WR };
BSC_option const CS2WCR_WW_ptr = { .CSn = SELECT_CS2, .MODE = SELECT_WCR, .REG = SELECT_WW };
BSC_option const CS3WCR_CL_ptr = { .CSn = SELECT_CS3, .MODE = SELECT_WCR, .REG = SELECT_A3CL };
BSC_option const CS3WCR_TRC_ptr = { .CSn = SELECT_CS3, .MODE = SELECT_WCR, .REG = SELECT_TRC };

const char *csn_name[] = {"0", "2", "3", "4", "5A", "5B", "6A", "6B"};

struct cpg_overclock_setting s_default;

static void get_default_preset()
{
    static struct cpg_overclock_setting s_current;
    cpg_get_overclock_setting(&s_current);
    clock_set_speed(CLOCK_SPEED_DEFAULT);
    cpg_get_overclock_setting(&s_default);
    cpg_set_overclock_setting(&s_current);
}

static void print_csnxcr(int row, int x, u8 check, BSC_option select)
{
    static const char *bcr_reg_name[] = {"IWW", "IWRWD", "IWRWS", "IWRRD", "IWRRS", 0};
    static const char *csnwcr_reg_name[] = {"WW", "SW", "HW", "WR", 0};
    static const char *cs3wcr_reg_name[] = {"TRP", "TRCD", "A3CL", "TRWL", "TRC", 0};
    static const int mode_name[] = {'B', 'W'};

    BSC_option read;
    read.byte = check;
    const u32 lword = (&BSC.CS0BCR + read.MODE * 8 + read.CSn)->lword;
    row_print(row, x, "CS%s%cCR", csn_name[read.CSn], mode_name[read.MODE]);
    row_print(row + 1, x, "%08X", lword);
    print_options(row + 2, x - 1, read.MODE
        ? (read.CSn == SELECT_CS3 ? cs3wcr_reg_name : csnwcr_reg_name)
        : bcr_reg_name, (read.MODE == select.MODE && read.CSn == select.CSn) ? select.REG : -1);

    const u8 REG_count = 5 - (read.MODE == SELECT_WCR && read.CSn != SELECT_CS3);
    u8 mask, field, value;
    char str[4];

    for (int REG = 0; REG < REG_count; REG++)
    {
        if (read.MODE == SELECT_BCR)
        {
            mask = 28 - REG * 3;
            field = 0b111;
        }
        else if (read.CSn == SELECT_CS3)
        {
            mask = 13 - REG * 3 - (REG >= SELECT_TRWL);
            field = 0b11;
        }
        else
        {
            static const u8 csnwcr_mask[4] = {16, 11, 0, 7};
            static const u8 csnwcr_field[4] = {0b111, 0b11, 0b11, 0b1111};
            mask = csnwcr_mask[REG];
            field = csnwcr_field[REG];
        }
        value = (lword >> mask) & field;
        const u32 s_select = *(&s_default.CS0BCR + read.MODE * 4 + read.CSn - (read.CSn == SELECT_CS5A));
        const i8 diff = value - ((s_select >> mask) & field);
        if (read.MODE == SELECT_BCR)
        {
            static const u8 bcr_wait[] = {0, 1, 2, 4, 6, 8, 10, 12};
            sprintf(str, "%d", bcr_wait[value]);
        }
        else if (read.CSn == SELECT_CS3)
            sprintf(str, "%d", REG == SELECT_TRC ? TRC_equivalent(value) : value + (REG != SELECT_TRWL));
        else
        {
            if (REG == SELECT_WW)
            {
                if (value)
                    sprintf(str, "%d", value - 1);
                else
                    sprintf(str, "=WR");
            }
            else if (REG == SELECT_WR)
            {
                static const u8 wr_wait[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 18, 24};
                sprintf(str, "%d", wr_wait[value]);
            }
            else
                sprintf(str, "%d.5", value);
        }
        row_print_color(row + 2 + REG, x + 6 - read.MODE,
            read.CSn <= SELECT_CS5A && read.CSn != SELECT_CS4 && diff
            ? diff > 0 ? C_BLUE : C_RED
            : C_BLACK, C_WHITE, str);
    }
}

static void modify_A3CL(u8 value)
{
    if (value != CL2 && value != CL3)
        return;
    BSC.CS3WCR.A3CL = value;
    for (int i = 0; i < 10; i++)
        __asm__ volatile("nop");
    if (value == CL2)
        *SH7305_SDMR3_CL2 = 0;
    else
        *SH7305_SDMR3_CL3 = 0;
}

void bsc_modify(BSC_option select, i8 modify)
{
    if (select.MODE == SELECT_BCR)
    {
        sh7305_bsc_CSnBCR_t *bcr_addr = &BSC.CS0BCR + select.CSn;
        const u8 mask = 28 - select.REG * 3;
        const i8 check = ((bcr_addr->lword >> mask) & 0b111) + modify;
        if (check < 0 || check > 7)
            return;
        /* `bcr_addr->lword += modify << mask` works, but this approach is overflow-proof. */
        bcr_addr->lword = (bcr_addr->lword & ~(0b111 << mask)) | (check << mask);
    }
    else
    {
        sh7305_bsc_CSnWCR_06A6B_t *wcr_addr = &BSC.CS0WCR + select.CSn;
        u8 min = 0;
        const i32 Bphi_f = clock_freq()->Bphi_f;
        if (select.CSn == SELECT_CS3)
        {
            const u8 mask = 13 - select.REG * 3 - (select.REG >= SELECT_TRWL);
            const i8 check = ((wcr_addr->lword >> mask) & 0b11) + modify;
            if (select.REG == SELECT_A3CL)
            {
                modify_A3CL(check);
                return;
            }
            #if defined CG50 || defined CG100
            if (select.REG == SELECT_TRC)
                min = best_TRC(Bphi_f);
            #endif
            if (check >= min && check <= 3)
                wcr_addr->lword = (wcr_addr->lword & ~(0b11 << mask)) | (check << mask);
            return;
        }
        static const u8 max[4] = {7, 3, 3, WAIT_24};
        static const u8 mask[4] = {16, 11, 0, 7};
        static const u8 field[4] = {0b111, 0b11, 0b11, 0b1111};
        i8 check = ((wcr_addr->lword >> mask[select.REG]) & field[select.REG]) + modify;
        if (select.byte == CS0WCR_WR_ptr.byte)
            min = best_rom_wait(Bphi_f);
        #if !defined CG50 && !defined CG100 && !defined CP400
        else if (select.byte == CS2WCR_WW_ptr.byte)
        {
            u8 best_wait = best_ram_write(Bphi_f);
            if (((wcr_addr->lword >> mask[select.REG]) & field[select.REG]) == 0)
                check = best_wait;
            else if (check < best_wait || check > WAIT_6 + 1)
                check = 0;
        }
        else if (select.byte == CS2WCR_WR_ptr.byte)
            min = best_ram_read(Bphi_f);
        #endif
        if (check >= min && check <= max[select.REG])
            wcr_addr->lword = (wcr_addr->lword & ~(field[select.REG] << mask[select.REG])) | (check << mask[select.REG]);
    }
}

void bsc_menu()
{
    key_event_t key;
    BSC_option select;
    select.byte = 0;
    #ifdef ENABLE_HELP
    set_help_function(help_info);
    #endif
    get_default_preset();

    while (true)
    {
        dclear(C_WHITE);

        #if defined CP400
        row_title("Bus State Controller");
        row_print(2, 11, "Bus Control Register");
        row_print(19, 11, "Wait Control Register");
        for (int i = 0; i < 8; i++)
        {
            const u8 shift = (i >= SELECT_CS5A) * 7;
            print_csnxcr(4 + shift, 2 + (i % 4) * 10, (SELECT_BCR << 6) | (i << 3), select);
            print_csnxcr(21 + shift, 2 + (i % 4) * 10, (SELECT_WCR << 6) | (i << 3), select);
        }
        for (int i = 0; i < 6; i++)
        {
            static const u8 highlight_row[6] = {2, 4, 11, 19, 21, 28};
            row_highlight(highlight_row[i]);
        }
        #else
        for (int i = 0; i < 8; i++)
            print_csnxcr((i >= SELECT_CSd5A) * 7, 2 + (i % 4) * 12, (select.MODE << 6) | (i << 3), select);
        row_highlight(0);
        row_highlight(7);
        #endif

        #ifndef CP400
        #if !defined CG100
        fkey_action(1, "+");
        fkey_action(2, "-");
        fkey_menu(6, "BCR/WCR");
        #else
        if (select.MODE == SELECT_BCR)
        {
            tab_menu(1, 3, "BCR");
            tab_action(4, 6, "WCR");
        }
        else
        {
            tab_action(1, 3, "BCR");
            tab_menu(4, 6, "WCR");
        }
        #endif
        #endif

        dupdate();
        key = getkey();

        switch (key.key)
        {
            #if !defined CG100 && !defined CP400
            case KEY_F1:
            #endif
            case KEY_PLUS:
                bsc_modify(select, 1);
                break;
            
            #if !defined CG100 && !defined CP400
            case KEY_F2:
            #endif
            case KEY_MINUS:
                bsc_modify(select, -1);
                break;

            #ifdef CG100
            case KEY_PREVTAB:
            #endif
            case KEY_BSC_BCRWCR:
                select.MODE = !select.MODE;
                break;

            case KEY_LEFT:
                if (select.CSn)
                    select.CSn--;
                break;
            case KEY_RIGHT:
                if (select.CSn < SELECT_CS6B)
                    select.CSn++;
                break;
            case KEY_UP:
                if (select.REG)
                    select.REG--;
                break;
            case KEY_DOWN:
                if (select.REG < SELECT_IWRRS)
                    select.REG++;
                break;
            
            #ifdef CG100
            case KEY_PAGEUP:
                if (select.CSn >= SELECT_CS5A)
                    select.CSn -= 4;
                break;
            case KEY_PAGEDOWN:
                if (select.CSn <= SELECT_CS4)
                    select.CSn += 4;
                break;
            #endif

            case KEY_EXIT:
                return;
        }
        if (select.MODE == SELECT_WCR && select.CSn != SELECT_CS3 && select.REG == SELECT_TRC)
            select.REG--;
        if (select.byte == CS0WCR_WW_ptr.byte)
            select.REG++;
    }
}