#include <stdio.h>
#include <gint/display.h>
#include <gint/clock.h>
#include "bsc.h"
#include "validate.h"
#include "util.h"
#include "config.h"

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

static void print_csnbcr(select_option select)
{
    static const char *bcr_reg_name[] = {"IWW", "IWRWD", "IWRWS", "IWRRD", "IWRRS", 0};
    static const u8 bcr_wait[] = {0, 1, 2, 4, 6, 8, 10, 12};
    for (int i = 0; i < 8; i++)
    {
        const u32 bcr_lword = (&BSC.CS0BCR + i)->lword;
        const u8 column = (i % 4) * 12;
        const u8 row = (i >= SELECT_CS5ABCR) * 7;
        row_print(0 + row, 2 + column, "CS%sBCR", csn_name[i]);
        row_print(1 + row, 2 + column, "%08X", bcr_lword);
        print_options(2 + row, 1 + column, bcr_reg_name, i == select.CSn ? select.REG : -1);
        for (int j = 0; j < 5; j++)
        {
            const u8 mask = 28 - j * 3;
            const u8 value = (bcr_lword >> mask) & 0b111;
            const u32 s_select = *(&s_default.CS0BCR + i - (i == SELECT_CS5ABCR));
            const i8 diff = value - ((s_select >> mask) & 0b111);
            row_print_color(2 + row + j, 9 + column,
                i <= SELECT_CS5ABCR && i != SELECT_CS4BCR && diff
                ? diff > 0 ? C_BLUE : C_RED
                : C_BLACK, C_WHITE, "%d", bcr_wait[value]);
        }
    }
}

static void print_csnwcr(select_option select)
{
    static const char *csnwcr_reg_name[] = {"WW", "SW", "HW", "WR", 0};
    static const u8 wr_wait[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 18, 24};

    for (int i = 0; i < 8; i++)
    {
        const u32 wcr_lword = (&BSC.CS0WCR + i)->lword;
        const u8 column = (i % 4) * 12;
        const u8 row = (i >= SELECT_CS5AWCR) * 7;
        const u8 highlight = i == select.CSn ? select.REG : -1;
        const u32 s_select = *(&s_default.CS0WCR + i - (i == SELECT_CS5ABCR));
        row_print(0 + row, 2 + column, "CS%sWCR", csn_name[i]);
        row_print(1 + row, 2 + column, "%08X", wcr_lword);
#if defined CG50 || defined CG100
        if (i == SELECT_CS3WCR)
        {
            static const char *cs3wcr_reg_name[] = {"TRP", "TRCD", "A3CL", "TRWL", "TRC", 0};
            print_options(2, 25, cs3wcr_reg_name, highlight);
            for (int j = 0; j < 5; j++)
            {
                static const u8 trc_wait[4] = {3, 4, 6, 9};
                const u8 mask = 13 - j * 3 - (j >= SELECT_TRWL);
                const u8 value = (wcr_lword >> mask) & 0b11;
                const i8 diff = value - ((s_select >> mask) & 0b11);
                row_print_color(2 + j, 32,
                    diff > 0 ? C_BLUE : diff < 0 ? C_RED : C_BLACK, C_WHITE, "%d",
                    j == SELECT_TRC ? trc_wait[value] : value + (j != SELECT_TRWL)
                );
            }
            continue;
        }
#endif
        print_options(2 + row, 1 + column, csnwcr_reg_name, highlight);
        for (int j = 0; j < 4; j++)
        {
            static const u8 mask[4] = {16, 11, 0, 7};
            static const u8 field[4] = {0b111, 0b11, 0b11, 0b1111};
            const u8 value = (wcr_lword >> mask[j]) & field[j];
            const i8 diff = value - ((s_select >> mask[j]) & field[j]);
            char str[4];
            if (j == SELECT_WW)
            {
                if (value)
                    sprintf(str, "%d", value - 1);
                else
                    sprintf(str, "=WR");
            }
            else if (j == SELECT_WR)
                sprintf(str, "%d", wr_wait[value]);
            else
                sprintf(str, "%d.5", value);
            row_print_color(2 + row + j, 7 + column,
                i <= SELECT_CS5AWCR && i != SELECT_CS4WCR && diff
                ? diff > 0 ? C_BLUE : C_RED
                : C_BLACK, C_WHITE, str);
        }
    }
}

static void bsc_modify(select_option select, i8 modify)
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
#if defined CG50 || defined CG100
        if (select.CSn == SELECT_CS3WCR)
        {
            const u8 mask = 13 - select.REG * 3 - (select.REG >= SELECT_TRWL);
            const i8 check = ((wcr_addr->lword >> mask) & 0b11) + modify;
            if (select.REG == SELECT_A3CL)
            {
                modify_A3CL(check);
                return;
            }
            if (check < 0 || check > 3)
                return;
            wcr_addr->lword = (wcr_addr->lword & ~(0b11 << mask)) | (check << mask);
            return;
        }
#endif
        static const u8 max[4] = {7, 3, 3, WAIT_24};
        static const u8 mask[4] = {16, 11, 0, 7};
        static const u8 field[4] = {0b111, 0b11, 0b11, 0b1111};
        const i8 check = ((wcr_addr->lword >> mask[select.REG]) & field[select.REG]) + modify;
        const u8 min = select.CSn == SELECT_CS0WCR && select.REG == SELECT_WR ? best_rom_wait(clock_freq()->Bphi_f) : 0;
        if (check >= min && check <= max[select.REG])
            wcr_addr->lword = (wcr_addr->lword & ~(field[select.REG] << mask[select.REG])) | (check << mask[select.REG]);
    }
}

void bsc_menu()
{
    key_event_t key;
    select_option select;
    select.byte = 0;
    get_default_preset();

    while (true)
    {
        dclear(C_WHITE);

        if (select.MODE == SELECT_BCR)
            print_csnbcr(select);
        else
            print_csnwcr(select);

        row_highlight(0);
        row_highlight(7);

        fkey_action(1, "+");
        fkey_action(2, "-");
        fkey_menu(5, "BCR/WCR");

        dupdate();
        key = getkey();

        switch (key.key)
        {
        case KEY_F1:
        case KEY_PLUS:
            bsc_modify(select, 1);
            break;
        case KEY_F2:
        case KEY_MINUS:
            bsc_modify(select, -1);
            break;
        case KEY_F5:
        case KEY_NEXTTAB:
        case KEY_VARS:
            select.MODE = !select.MODE;
            break;
        case KEY_LEFT:
            if (select.CSn)
                select.CSn--;
            break;
        case KEY_RIGHT:
            if (select.CSn < SELECT_CS6BBCR)
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
        case KEY_PAGEUP:
            if (select.CSn >= SELECT_CS5ABCR)
                select.CSn -= 4;
            break;
        case KEY_PAGEDOWN:
            if (select.CSn <= SELECT_CS4BCR)
                select.CSn += 4;
            break;
        case KEY_EXIT:
            return;
        }
#if defined CG20
        if (select.MODE == SELECT_WCR && select.REG == SELECT_IWRRS)
#elif defined CG50 || defined CG100
        if (select.MODE == SELECT_WCR && select.CSn != SELECT_CS3WCR && select.REG == SELECT_TRC)
#endif
            select.REG--;
        if (select.byte == 0b01000000)
            select.REG++;
    }
}