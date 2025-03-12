#include <gint/display.h>
#include <gint/clock.h>
#include "bsc.h"
#include "validate.h"
#include "util.h"

static void struct_csnbcr(sh7305_bsc_CSnBCR_t reg, u8 val[])
{
    val[0] = reg.IWW;
    val[1] = reg.IWRWD;
    val[2] = reg.IWRWS;
    val[3] = reg.IWRRD;
    val[4] = reg.IWRRS;
}

static void print_csnbcr(select_option select)
{
    const char *bcr_reg_name[] = {"IWW", "IWRWD", "IWRWS", "IWRRD", "IWRRS", 0};
    const u8 bcr_wait[] = {0, 1, 2, 4, 6, 8, 10, 12};
    const sh7305_bsc_CSnBCR_t bcr_addr[] =
        {BSC.CS0BCR, BSC.CS2BCR, BSC.CS3BCR, BSC.CS4BCR, BSC.CS5ABCR, BSC.CS5BBCR, BSC.CS6ABCR, BSC.CS6BBCR};
    u8 csnbcr_val[5];
    for (int i = 0; i < 8; i++)
    {
        struct_csnbcr(bcr_addr[i], csnbcr_val);
        const u8 x_i = (i % 4) * 12;
        const u8 row_i = (i >= SELECT_CS5ABCR) * 7;
        static const char *csn_name[] = {"0", "2", "3", "4", "5A", "5B", "6A", "6B"};
        row_print(0 + row_i, 2 + x_i, "CS%sBCR", csn_name[i]);
        row_print(1 + row_i, 2 + x_i, "%08x", bcr_addr[i].lword);
        print_options(2 + row_i, 1 + x_i, bcr_reg_name, i == select.CSn ? select.REG : -1);
        for (int j = 0; j < 5; j++)
            row_print(2 + row_i + j, 9 + x_i, "%d", bcr_wait[csnbcr_val[j]]);
    }
}

static void print_csnwcr(select_option select)
{
    const char *csnwcr_reg_name[] = {"WW", "SW", "HW", "WR", 0};
    const char *cs3wcr_reg_name[] = {"TRP", "TRCD", "A3CL", "TRWL", "TRC", 0};
    const char *sw_hw_wait[] = {"0.5", "1.5", "2.5", "3.5"};
    const u8 wr_wait[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 18, 24};

    const u8 csnwcr_2d_val[8][4] =
        {
            {BSC.CS0WCR.WW, BSC.CS0WCR.SW, BSC.CS0WCR.HW, BSC.CS0WCR.WR},
            {BSC.CS2WCR.WW, BSC.CS2WCR.SW, BSC.CS2WCR.HW, BSC.CS2WCR.WR},
            {0, 0, 0, 0},
            {BSC.CS4WCR.WW, BSC.CS4WCR.SW, BSC.CS4WCR.HW, BSC.CS4WCR.WR},
            {BSC.CS5AWCR.WW, BSC.CS5AWCR.SW, BSC.CS5AWCR.HW, BSC.CS5AWCR.WR},
            {BSC.CS5BWCR.WW, BSC.CS5BWCR.SW, BSC.CS5BWCR.HW, BSC.CS5BWCR.WR},
            {BSC.CS6AWCR.WW, BSC.CS6AWCR.SW, BSC.CS6AWCR.HW, BSC.CS6AWCR.WR},
            {BSC.CS6BWCR.WW, BSC.CS6BWCR.SW, BSC.CS6BWCR.HW, BSC.CS6BWCR.WR}};
    const u32 csnwcr_lword[8] =
        {BSC.CS0WCR.lword, BSC.CS2WCR.lword, BSC.CS3WCR.lword, BSC.CS4WCR.lword,
         BSC.CS5AWCR.lword, BSC.CS5BWCR.lword, BSC.CS6AWCR.lword, BSC.CS6BWCR.lword};

    for (int i = 0; i < 8; i++)
    {
        const u8 x_i = (i % 4) * 12;
        const u8 row_i = (i >= SELECT_CS5AWCR) * 7;
        const u8 highlight = i == select.CSn ? select.REG : -1;
        static const char *csn_name[] = {"0", "2", "3", "4", "5A", "5B", "6A", "6B"};
        row_print(0 + row_i, 2 + x_i, "CS%sWCR", csn_name[i]);
        row_print(1 + row_i, 2 + x_i, "%08x", csnwcr_lword[i]);

        if (i == SELECT_CS3WCR)
        {
            const int trc_wait[4] = {3, 4, 6, 9};
            print_options(2, 25, cs3wcr_reg_name, highlight);
            row_print(2, 32, "%d", wr_wait[BSC.CS3WCR.TRP + 1]);
            row_print(3, 32, "%d", wr_wait[BSC.CS3WCR.TRCD + 1]);
            row_print(4, 32, "%d", BSC.CS3WCR.A3CL + 1);
            row_print(5, 32, "%d", wr_wait[BSC.CS3WCR.TRWL]);
            row_print(6, 32, "%d", trc_wait[BSC.CS3WCR.TRC]);
            continue;
        }

        print_options(2 + row_i, 1 + x_i, csnwcr_reg_name, highlight);
        if (csnwcr_2d_val[i][0])
            row_print(2 + row_i, 7 + x_i, "%d", csnwcr_2d_val[i][0] - 1);
        else
            row_print(2 + row_i, 7 + x_i, "=WR");
        row_print(3 + row_i, 7 + x_i, "%s", sw_hw_wait[csnwcr_2d_val[i][1]]);
        row_print(4 + row_i, 7 + x_i, "%s", sw_hw_wait[csnwcr_2d_val[i][2]]);
        row_print(5 + row_i, 7 + x_i, "%d", wr_wait[csnwcr_2d_val[i][3]]);
    }
}

static void bsc_modify(select_option select, i8 modify)
{
    if (select.MODE == SELECT_BCR)
    {
        sh7305_bsc_CSnBCR_t *bcr_addr[] =
            {&BSC.CS0BCR, &BSC.CS2BCR, &BSC.CS3BCR, &BSC.CS4BCR,
             &BSC.CS5ABCR, &BSC.CS5BBCR, &BSC.CS6ABCR, &BSC.CS6BBCR};
        u8 csnbcr_val[5];
        struct_csnbcr(*bcr_addr[select.CSn], csnbcr_val);
        const i8 check = csnbcr_val[select.REG] + modify;
        if (check < 0 || check > 11)
            return;
        switch (select.REG)
        {
        case SELECT_IWW:
            bcr_addr[select.CSn]->IWW = check;
            return;
        case SELECT_IWRWD:
            bcr_addr[select.CSn]->IWRWD = check;
            return;
        case SELECT_IWRWS:
            bcr_addr[select.CSn]->IWRWS = check;
            return;
        case SELECT_IWRRD:
            bcr_addr[select.CSn]->IWRRD = check;
            return;
        case SELECT_IWRRS:
            bcr_addr[select.CSn]->IWRRS = check;
            return;
        }
    }
    else
    {
        switch (select.CSn)
        {
        case SELECT_CS0WCR:
            switch (select.REG)
            {
            case SELECT_SW:
            {
                const i8 check = BSC.CS0WCR.SW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS0WCR.SW = check;
                return;
            }
            case SELECT_HW:
            {
                const i8 check = BSC.CS0WCR.HW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS0WCR.HW = check;
                return;
            }
            case SELECT_WR:
            {
                const u8 check = BSC.CS0WCR.WR + modify;
                if (check >= best_rom_wait(clock_freq()->Bphi_f) && check <= WAIT_24)
                    BSC.CS0WCR.WR = check;
                return;
            }
            }
            break;
        case SELECT_CS2WCR:
            switch (select.REG)
            {
            case SELECT_WW:
            {
                const i8 check = BSC.CS2WCR.WW + modify;
                if (check >= 0 && check <= 7)
                    BSC.CS2WCR.WW = check;
                return;
            }
            case SELECT_SW:
            {
                const i8 check = BSC.CS2WCR.SW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS2WCR.SW = check;
                return;
            }
            case SELECT_HW:
            {
                const i8 check = BSC.CS2WCR.HW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS2WCR.HW = check;
                return;
            }
            case SELECT_WR:
            {
                const i8 check = BSC.CS2WCR.WR + modify;
                if (check >= WAIT_0 && check <= WAIT_24)
                    BSC.CS2WCR.WR = check;
                return;
            }
            }
            break;
        case SELECT_CS3WCR:
            switch (select.REG)
            {
            case SELECT_TRP:
            {
                const i8 check = BSC.CS3WCR.TRP + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS3WCR.TRP = check;
                return;
            }
            case SELECT_TRCD:
            {
                const i8 check = BSC.CS3WCR.TRCD + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS3WCR.TRCD = check;
                return;
            }
            case SELECT_A3CL:
            {
                const i8 check = BSC.CS3WCR.A3CL + modify;
                if (check == 1 || check == 2)
                {
                    BSC.CS3WCR.A3CL = check;
                    if (check == 1)
                        *SH7305_SDMR3_CL2 = 0;
                    else
                        *SH7305_SDMR3_CL3 = 0;
                }
                return;
            }
            case SELECT_TRWL:
            {
                const i8 check = BSC.CS3WCR.TRWL + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS3WCR.TRWL = check;
                return;
            }
            case SELECT_TRC:
            {
                const i8 check = BSC.CS3WCR.TRC + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS3WCR.TRC = check;
                return;
            }
            }
            break;
        case SELECT_CS4WCR:
            switch (select.REG)
            {
            case SELECT_WW:
            {
                const i8 check = BSC.CS4WCR.WW + modify;
                if (check >= 0 && check <= 7)
                    BSC.CS4WCR.WW = check;
                return;
            }
            case SELECT_SW:
            {
                const i8 check = BSC.CS4WCR.SW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS4WCR.SW = check;
                return;
            }
            case SELECT_HW:
            {
                const i8 check = BSC.CS4WCR.HW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS4WCR.HW = check;
                return;
            }
            case SELECT_WR:
            {
                const i8 check = BSC.CS4WCR.WR + modify;
                if (check >= WAIT_0 && check <= WAIT_24)
                    BSC.CS4WCR.WR = check;
                return;
            }
            }
            break;
        case SELECT_CS5AWCR:
            switch (select.REG)
            {
            case SELECT_WW:
            {
                const i8 check = BSC.CS5AWCR.WW + modify;
                if (check >= 0 && check <= 7)
                    BSC.CS5AWCR.WW = check;
                return;
            }
            case SELECT_SW:
            {
                const i8 check = BSC.CS5AWCR.SW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS5AWCR.SW = check;
                return;
            }
            case SELECT_HW:
            {
                const i8 check = BSC.CS5AWCR.HW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS5AWCR.HW = check;
                return;
            }
            case SELECT_WR:
            {
                const i8 check = BSC.CS5AWCR.WR + modify;
                if (check >= WAIT_0 && check <= WAIT_24)
                    BSC.CS5AWCR.WR = check;
                return;
            }
            }
            break;
        case SELECT_CS5BWCR:
            switch (select.REG)
            {
            case SELECT_WW:
            {
                const i8 check = BSC.CS5BWCR.WW + modify;
                if (check >= 0 && check <= 7)
                    BSC.CS5BWCR.WW = check;
                return;
            }
            case SELECT_SW:
            {
                const i8 check = BSC.CS5BWCR.SW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS5BWCR.SW = check;
                return;
            }
            case SELECT_HW:
            {
                const i8 check = BSC.CS5BWCR.HW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS5BWCR.HW = check;
                return;
            }
            case SELECT_WR:
            {
                const i8 check = BSC.CS5BWCR.WR + modify;
                if (check >= WAIT_0 && check <= WAIT_24)
                    BSC.CS5BWCR.WR = check;
                return;
            }
            }
            break;
        case SELECT_CS6AWCR:
            switch (select.REG)
            {
            case SELECT_WW:
            {
                const i8 check = BSC.CS6AWCR.WW + modify;
                if (check >= 0 && check <= 7)
                    BSC.CS6AWCR.WW = check;
                return;
            }
            case SELECT_SW:
            {
                const i8 check = BSC.CS6AWCR.SW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS6AWCR.SW = check;
                return;
            }
            case SELECT_HW:
            {
                const i8 check = BSC.CS6AWCR.HW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS6AWCR.HW = check;
                return;
            }
            case SELECT_WR:
            {
                const i8 check = BSC.CS6AWCR.WR + modify;
                if (check >= WAIT_0 && check <= WAIT_24)
                    BSC.CS6AWCR.WR = check;
                return;
            }
            }
            break;
        case SELECT_CS6BWCR:
            switch (select.REG)
            {
            case SELECT_WW:
            {
                const i8 check = BSC.CS6BWCR.WW + modify;
                if (check >= 0 && check <= 7)
                    BSC.CS6BWCR.WW = check;
                return;
            }
            case SELECT_SW:
            {
                const i8 check = BSC.CS6BWCR.SW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS6BWCR.SW = check;
                return;
            }
            case SELECT_HW:
            {
                const i8 check = BSC.CS6BWCR.HW + modify;
                if (check >= 0 && check <= 3)
                    BSC.CS6BWCR.HW = check;
                return;
            }
            case SELECT_WR:
            {
                const i8 check = BSC.CS6BWCR.WR + modify;
                if (check >= WAIT_0 && check <= WAIT_24)
                    BSC.CS6BWCR.WR = check;
                return;
            }
            }
            break;
        }
    }
}

void bsc_menu()
{
    key_event_t key;
    select_option select;
    select.byte = 0;

    while (true)
    {
        const sh7305_bsc_t unused = BSC;
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