#include <gint/display.h>
#include <gint/keyboard.h>
#include <util.h>

#include "settings.h"
#include "config.h"
#include "validate.h"
#include "mem_test.h"

#ifdef ENABLE_HELP
static void help_info()
{
    #ifdef CG100
    info_box(4, 7, "HELP");
    row_print(5, 2, "[ON]: Reset to default");
    row_print(6, 2, "[+][-]: +/- option value (1000 KHz)");
    row_print(7, 2, "[LEFT][RIGHT]: -/+ option value (100 KHz)");
    row_print(8, 2, "[UP][DOWN]: Select option");
    row_print(9, 2, "[PGUP]: About this add-in");
    row_print(10, 2, "[BACK]: Close help / Return to express menu");
    #else
    info_box(3, 9, "HELP");
    row_print(4, 2, "[F1]: Reset to default");
    row_print(5, 2, "[F2][+]: Increase option value (1000 KHz)");
    row_print(6, 2, "[F3][-]: Decrease option value (1000 KHz)");
    row_print(7, 2, "[LEFT][RIGHT]: -/+ option value (100 KHz)");
    row_print(8, 2, "[UP][DOWN]: Select option");
    row_print(10, 2, "[F6]: About this add-in");
    row_print(11, 2, "[EXIT]: Close help / Return to express menu");
    #endif
    dupdate();
    while (getkey().key != KEY_EXIT);
}
#endif

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
    info_box(5, 4, "About");
    row_print(6, 2, VERSION);
    row_print(7, 2, "Copyright (C) 2025 Sentaro21, CalcLoverHK");
    row_print(8, 2, "This software is licensed under MIT/Expat.");
    dupdate();
    getkey();
}

void settings_menu()
{
    key_event_t key;
    i8 select = 0;
    static const char *option[] = {"PLL", "CPU", "SHW", "Bus", "I/O"};
    static const i32 settings_def[] =
        {ROM_MARGIN_DEF, RAM_MARGIN_DEF, PLL_CLK_MAX_DEF, IFC_CLK_MAX_DEF, SFC_CLK_MAX_DEF, BFC_CLK_MAX_DEF, PFC_CLK_MAX_DEF};
    static const i32 settings_max[] =
        {ROM_MARGIN_MAX, RAM_MARGIN_MAX, PLL_MAX, CPU_MAX, SHW_MAX, BUS_MAX, IO_MAX};

    static const i8 select_max = SELECT_PFC + 1;

    #ifdef ENABLE_HELP
    set_help_function(help_info);
    #endif

    while (true)
    {
        dclear(C_WHITE);
        row_title("Settings");
        row_print(1, 1, "ROM Margin");
        row_print(1, 25, "%d%%", ROM_MARGIN);
        row_print(2, 1, "RAM Margin");
        row_print(2, 25, "%d%%", RAM_MARGIN);
        for (int i = 0; i < 5; i++)
        {
            row_print(i + 3, 1, "%s Clock Max", option[i]);
            row_print(i + 3, 25, "%d", settings[i + 2] / 1000);
            row_print(i + 3, 33, "KHz");
        }

        row_highlight(select + 1);

        fkey_action(1, "Reset");
        #ifndef CG100
        fkey_action(2, "+");
        fkey_action(3, "-");
        #endif
        fkey_button(6, "About");

        dupdate();
        key = getkey();

        i32 modify = 0;
        bool scale = false;
        switch (key.key)
        {
            case KEY_UP:
                select = (select + select_max - 1) % select_max;
                break;
            case KEY_DOWN:
                select = (select + 1) % select_max;
                break;

            #ifdef CG100
            case KEY_ON:
            #else
            case KEY_F1:
            #endif
                settings[select] = settings_def[select];
                break;

            #ifndef CG100
            case KEY_F2:
            #endif
            case KEY_PLUS:
                scale = true;
                __attribute__((fallthrough));
            case KEY_RIGHT:
                modify++;
                break;
            #ifndef CG100
            case KEY_F3:
            #endif
            case KEY_MINUS:
                scale = true;
                __attribute__((fallthrough));
            case KEY_LEFT:
                modify--;
                break;

            #ifdef CG100
            case KEY_PAGEUP:
            #else
            case KEY_F6:
            #endif
                about();
                break;

            case KEY_EXIT:
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