#include <gint/display.h>
#include <gint/clock.h>
#include "bsc.h"
#include "validate.h"
#include "util.h"

static const char *csn_name[] = {"0", "2", "3", "4", "5A", "5B", "6A", "6B"};

static void print_csnbcr(select_option select)
{
    static const char *bcr_reg_name[] = {"IWW", "IWRWD", "IWRWS", "IWRRD", "IWRRS", 0};
    static const u8 bcr_wait[] = {0, 1, 2, 4, 6, 8, 10, 12};
    for (int i = 0; i < 8; i++)
    {
        const sh7305_bsc_CSnBCR_t *bcr_addr = &BSC.CS0BCR + i;
        const u8 column = (i % 4) * 12;
        const u8 row = (i >= SELECT_CS5ABCR) * 7;
        row_print(0 + row, 2 + column, "CS%sBCR", csn_name[i]);
        row_print(1 + row, 2 + column, "%08x", bcr_addr->lword);
        print_options(2 + row, 1 + column, bcr_reg_name, i == select.CSn ? select.REG : -1);
        for (int j = 0; j < 5; j++)
        {
            const u8 mask = 28 - j * 3;
            row_print(2 + row + j, 9 + column, "%d", bcr_wait[(bcr_addr->lword >> mask) & 0b111]);
        }
    }
}

static void print_csnwcr(select_option select)
{
    static const char *csnwcr_reg_name[] = {"WW", "SW", "HW", "WR", 0};
    static const u8 wr_wait[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 18, 24};

    for (int i = 0; i < 8; i++)
    {
        const sh7305_bsc_CSnWCR_06A6B_t *wcr_addr = &BSC.CS0WCR + i;
        const u8 column = (i % 4) * 12;
        const u8 row = (i >= SELECT_CS5AWCR) * 7;
        const u8 highlight = i == select.CSn ? select.REG : -1;
        row_print(0 + row, 2 + column, "CS%sWCR", csn_name[i]);
        row_print(1 + row, 2 + column, "%08x", wcr_addr->lword);
        if (i == SELECT_CS3WCR)
        {
            static const char *cs3wcr_reg_name[] = {"TRP", "TRCD", "A3CL", "TRWL", "TRC", 0};
            static const int trc_wait[4] = {3, 4, 6, 9};
            print_options(2, 25, cs3wcr_reg_name, highlight);
            for (int i = 0; i < 3; i++)
                row_print(2 + i, 32, "%d", wr_wait[((wcr_addr->lword >> (13 - i * 3)) & 0b11) + 1]);
            row_print(5, 32, "%d", wr_wait[(wcr_addr->lword >> 3) & 0b11]);
            row_print(6, 32, "%d", trc_wait[wcr_addr->lword & 0b11]);
            continue;
        }
        print_options(2 + row, 1 + column, csnwcr_reg_name, highlight);
        if (wcr_addr->WW)
            row_print(2 + row, 7 + column, "%d", wcr_addr->WW - 1);
        else
            row_print(2 + row, 7 + column, "=WR");
        row_print(3 + row, 7 + column, "%d.5", wcr_addr->SW);
        row_print(4 + row, 7 + column, "%d.5", wcr_addr->HW);
        row_print(5 + row, 7 + column, "%d", wr_wait[wcr_addr->WR]);
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
        if (select.CSn == SELECT_CS3WCR)
        {
            const u8 mask = 13 - select.REG * 3 - (select.REG >= SELECT_TRWL);
            const i8 check = ((wcr_addr->lword >> mask) & 0b11) + modify;
            const bool A3CL = select.REG == SELECT_A3CL;
            if (check < 0 + A3CL || check > 3 - A3CL)
                return;
            wcr_addr->lword = (wcr_addr->lword & ~(0b11 << mask)) | (check << mask);
            if (select.REG == SELECT_A3CL)
            {
                if (check == 1)
                    *SH7305_SDMR3_CL2 = 0;
                else
                    *SH7305_SDMR3_CL3 = 0;
            }
            return;
        }
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
        if (select.MODE == SELECT_WCR && select.CSn != SELECT_CS3WCR && select.REG == SELECT_TRC)
            select.REG--;
        if (select.byte == 0b01000000)
            select.REG++;
    }
}