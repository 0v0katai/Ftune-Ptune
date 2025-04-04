#include <gint/display.h>
#include <gint/keyboard.h>
#include <util.h>

#include "settings.h"
#include "config.h"
#include "validate.h"
#include "mem_test.h"

i32 settings[] =
    {
        ROM_MARGIN_DEF,
        RAM_MARGIN_DEF,
        PLL_CLK_MAX_DEF,
        IFC_CLK_MAX_DEF,
        SFC_CLK_MAX_DEF,
#if defined CG50 || defined CG100
        BFC_CLK_MAX_TRC3_DEF,
        BFC_CLK_MAX_TRC4_DEF,
        BFC_CLK_MAX_TRC6_DEF,
        BFC_CLK_MAX_TRC9_DEF,
#else
        BFC_CLK_MAX_DEF,
        -1,
        -1,
        -1,
#endif
        PFC_CLK_MAX_DEF};

enum select_option
{
    SELECT_ROM,
    SELECT_RAM,
    SELECT_PLL,
    SELECT_IFC,
    SELECT_SFC,
#if defined CG50 || defined CG100
    SELECT_BFC_TRC3,
    SELECT_BFC_TRC4,
    SELECT_BFC_TRC6,
    SELECT_BFC_TRC9,
#else
    SELECT_BFC,
#endif
    SELECT_PFC
};

static void about()
{
    msg_box(5, 5);
    row_print(6, 3, VERSION);
    row_print(7, 3, "Copyright (C) 2025 Sentaro21, CalcLoverHK");
    row_print(8, 3, "This software is licensed under MIT/Expat.");
    dupdate();
    getkey();
}

void settings_menu()
{
    key_event_t key;
    i8 select = 0;
#if defined CG50 || defined CG100
    static const char *option[] = {"PLL", "CPU", "SHW", "Bus", "Bus", "Bus", "Bus", "I/O"};
    static const i32 settings_def[] =
        {ROM_MARGIN_DEF, RAM_MARGIN_DEF, PLL_CLK_MAX_DEF, IFC_CLK_MAX_DEF, SFC_CLK_MAX_DEF,
         BFC_CLK_MAX_TRC3_DEF, BFC_CLK_MAX_TRC4_DEF, BFC_CLK_MAX_TRC6_DEF, BFC_CLK_MAX_TRC9_DEF, PFC_CLK_MAX_DEF};
    static const i32 settings_max[] =
        {ROM_MARGIN_MAX, RAM_MARGIN_MAX, PLL_MAX, CPU_MAX, SHW_MAX, BUS_MAX, BUS_MAX, BUS_MAX, BUS_MAX, IO_MAX};
#else
    static const char *option[] = {"PLL", "CPU", "SHW", "Bus", "I/O"};
    static const i32 settings_def[] =
        {ROM_MARGIN_DEF, RAM_MARGIN_DEF, PLL_CLK_MAX_DEF, IFC_CLK_MAX_DEF, SFC_CLK_MAX_DEF, BFC_CLK_MAX_DEF, PFC_CLK_MAX_DEF};
    static const i32 settings_max[] =
        {ROM_MARGIN_MAX, RAM_MARGIN_MAX, PLL_MAX, CPU_MAX, SHW_MAX, BUS_MAX, IO_MAX};
#endif

    static const i8 select_max = SELECT_PFC + 1;

    while (true)
    {
        dclear(C_WHITE);
        row_title("Settings");
        row_print(1, 1, "ROM Margin");
        row_print(1, 25, "%d%%", ROM_MARGIN);
        row_print(2, 1, "RAM Margin");
        row_print(2, 25, "%d%%", RAM_MARGIN);
#if defined CG50 || defined CG100
        for (int i = 0; i < 4; i++)
        {
            static const int trc_wait[4] = {3, 4, 6, 9};
            row_print(i + 6, 15, "(TRC=%d)", trc_wait[i]);
        }
        for (int i = 0; i < 8; i++)
#else
        for (int i = 0; i < 5; i++)
#endif
        {
            row_print(i + 3, 1, "%s Clock Max", option[i]);
            row_print(i + 3, 25, "%d", settings[i + 2] / 1000);
            row_print(i + 3, 33, "KHz");
        }

        row_highlight(select + 1);

        fkey_action(1, "+");
        fkey_action(2, "-");
        fkey_action(3, "Init");
        fkey_button(5, "Test");
        fkey_button(6, "About");

        dupdate();
        key = getkey();

        i32 modify = 0;
        bool scale = false;
        switch (key.key)
        {
        case KEY_UP:
            select = select ? select - 1 : select_max - 1;
            break;
        case KEY_DOWN:
            select = (select + 1) % select_max;
            break;

        case KEY_F1:
        case KEY_PLUS:
            scale = true;
            __attribute__((fallthrough));
        case KEY_RIGHT:
            modify++;
            break;
        case KEY_F2:
        case KEY_MINUS:
            scale = true;
            __attribute__((fallthrough));
        case KEY_LEFT:
            modify--;
            break;
        case KEY_F3:
        case KEY_PREVTAB:
            settings[select] = settings_def[select];
            break;

        case KEY_F5:
        case KEY_NEXTTAB:
        #if defined CG20
                // sram_test();
        #elif defined CG50 || defined CG100
                sdram_test();
        #endif
                break;

        case KEY_F6:
        case KEY_PAGEUP:
            about();
            break;

        case KEY_EXIT:
        case KEY_SETTINGS:
            return;
        }

        if (select >= SELECT_PLL)
        {
            modify *= 100 * 1000;
            if (scale)
                modify *= 10;
        }
        settings[select] += modify;
        if (settings[select] > settings_max[select])
            settings[select] = settings_max[select];
        if (settings[select] < 0)
            settings[select] = 0;
    };
}