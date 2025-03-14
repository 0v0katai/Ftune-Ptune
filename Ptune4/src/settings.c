#include <gint/display.h>
#include <gint/keyboard.h>
#include <util.h>

#include "settings.h"
#include "validate.h"
#include "mem_test.h"

i32 settings[] =
    {
        ROM_MARGIN_DEF,
        RAM_MARGIN_DEF,
        PLL_CLK_MAX_DEF,
        IFC_CLK_MAX_DEF,
        SFC_CLK_MAX_DEF,
        BFC_CLK_MAX_DEF,
        PFC_CLK_MAX_DEF};

enum select_option
{
    SELECT_ROM,
    SELECT_RAM,
    SELECT_PLL,
    SELECT_IFC,
    SELECT_SFC,
    SELECT_BFC,
    SELECT_PFC
};

static void about()
{
    row_print(10, 1, "Ptune4 v0.03");
    row_print(11, 1, "Copyright (C) 2025 Sentaro21, CalcLoverHK");
    row_print(12, 1, "This software is licensed under MIT/X11.");
}

void settings_menu()
{
    key_event_t key;
    i8 select = 0;
    const char *option[] = {"PLL", "CPU", "SHW", "Bus", "I/O"};
    const i32 settings_def[] =
        {ROM_MARGIN_DEF, RAM_MARGIN_DEF, PLL_CLK_MAX_DEF, IFC_CLK_MAX_DEF, SFC_CLK_MAX_DEF, BFC_CLK_MAX_DEF, PFC_CLK_MAX_DEF};
    const i32 settings_max[] =
        {ROM_MARGIN_MAX, RAM_MARGIN_MAX, PLL_MAX, CPU_MAX, SHW_MAX, BUS_MAX, IO_MAX};
    const i8 select_max = SELECT_PFC + 1;

    while (true)
    {
        dclear(C_WHITE);
        row_title("Settings");
        row_print(1, 1, "ROM Margin:");
        row_print(1, 25, "%d%%", ROM_MARGIN);
        row_print(2, 1, "RAM Margin:");
        row_print(2, 25, "%d%%", RAM_MARGIN);
        for (int i = 0; i < 5; i++)
        {
            row_print(i + 3, 1, "%s Clock Max:", option[i]);
            row_print(i + 3, 25, "%d", settings[i + 2] / 1000);
            row_print(i + 3, 33, "KHz");
        }

        row_highlight(select + 1);

        about();

        fkey_action(1, "+");
        fkey_action(2, "-");
        fkey_action(5, "Init");
        fkey_button(6, "RAM");

        dupdate();
        key = getkey();

        i32 modify = 0;
        switch (key.key)
        {
        case KEY_UP:
            select = select ? select - 1 : select_max - 1;
            break;
        case KEY_DOWN:
            select = (select + 1) % select_max;
            break;

        case KEY_MINUS:
        case KEY_LEFT:
            modify--;
            break;
        case KEY_RIGHT:
        case KEY_PLUS:
            modify++;
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

        if (select >= SELECT_PLL)
        {
            if (key.key == KEY_MINUS || key.key == KEY_PLUS)
                modify *= 10;
            modify *= 100 * 1000;
        }
        settings[select] += modify;
        if (settings[select] > settings_max[select])
            settings[select] = settings_max[select];
        if (settings[select] < 0)
            settings[select] = 0;
    };
}