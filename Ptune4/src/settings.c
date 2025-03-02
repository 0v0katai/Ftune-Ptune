#include <gint/display.h>
#include <gint/keyboard.h>
#include <util.h>

#include "settings.h"
#include "validate.h"

i32 settings[] =
    {
        PLL_CLK_MAX_DEF,
        IFC_CLK_MAX_DEF,
        SFC_CLK_MAX_DEF,
        BFC_CLK_MAX_DEF,
        PFC_CLK_MAX_DEF};

enum select_option
{
    SELECT_PLL,
    SELECT_IFC,
    SELECT_SFC,
    SELECT_BFC,
    SELECT_PFC
};

static void about()
{
    row_print(9, 1, "Ptune4 v0.01");
    row_print(10, 1, "Copyright (C) 2025 Sentaro21, CalcLoverHK");
    row_print(11, 1, "This software is licensed under MIT/X11.");
}

void settings_menu()
{
    key_event_t key;
    i8 select = 0;
    const char *option[] = {"PLL", "CPU", "SHW", "Bus", "I/O"};
    const i32 settings_def[] = {PLL_CLK_MAX_DEF, IFC_CLK_MAX_DEF, SFC_CLK_MAX_DEF, BFC_CLK_MAX_DEF, PFC_CLK_MAX_DEF};
    const i32 settings_max[] = {PLL_MAX, CPU_MAX, SHW_MAX, BUS_MAX, IO_MAX};
    const i8 select_max = SELECT_PFC + 1;

    while (true)
    {
        dclear(C_WHITE);
        row_title("Settings");
        for (int i = 0; i < 5; i++)
        {
            row_print(i + 1, 1, "%s Clock Max:", option[i]);
            row_print(i + 1, 25, "%6d", settings[i] / 1000);
            row_print(i + 1, 33, "KHz");
        }

        row_highlight(select + 1);

        about();

        fkey_action(1, "+");
        fkey_action(2, "-");
        fkey_action(5, "Init");
        fkey_button(6, "RAM");

        dupdate();
        key = getkey();

        switch (key.key)
        {
        case KEY_UP:
            if (select)
                select--;
            else
                select = select_max - 1;
            break;
        case KEY_DOWN:
            select = (select + 1) % select_max;
            break;

        case KEY_MINUS:
            settings[select] -= 1000 * 1000;
            break;
        case KEY_PLUS:
            settings[select] += 1000 * 1000;
            break;
        case KEY_LEFT:
            settings[select] -= 100 * 1000;
            break;
        case KEY_RIGHT:
            settings[select] += 100 * 1000;
            break;

        case KEY_F5:
        case KEY_NEXTTAB:
            settings[select] = settings_def[select];
            break;

        case KEY_F6:
        case KEY_PAGEUP:
            sdram_test();
            break;

        case KEY_EXIT:
        case KEY_SETTINGS:
            return;
        }
        if (settings[select] > settings_max[select])
            settings[select] = settings_max[select];
        if (settings[select] < 0)
            settings[select] = 0;
    };
}