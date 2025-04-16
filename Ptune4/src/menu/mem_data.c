#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>

#include "util.h"
#include "validate.h"
#include "mem_test.h"
#include "settings.h"
#include "config.h"
#include "menu.h"

void mem_data_menu()
{
    key_event_t key;
    bool margin = true;

    static const i32 roR_default[] =
    {
        roR_0, roR_1, roR_2, roR_3, roR_4,
        roR_5, roR_6, roR_8, roR_10, roR_12,
        roR_14, roR_18
    };
    static const u32 raW_TRC_default[] = {raW_TRC_3, raW_TRC_4, raW_TRC_6, raW_TRC_9};

    while (true)
    {
        dclear(C_WHITE);
        row_title("Memory data");
        row_print(1, 1, "ROM Margin: %d%%", ROM_MARGIN);
        row_print(1, 25, "RAM Margin: %d%%", RAM_MARGIN);
        for (int i = 0; i < 10; i++)
        {
            static const u8 mem_wait[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12};
            row_print(i + 2, 1, "roR_%d", mem_wait[i]);
            row_print_color(i + 2, 11, margin ? C_BLUE : C_BLACK, C_WHITE, "%d KHz", margin
                ? roR[i] / 100 * (100 - ROM_MARGIN) / 1000
                : roR[i] / 1000);
        }
        for (int i = 0; i < 4; i++)
        {
            static const u8 trc_wait[] = {3, 4, 6, 9};
            row_print(i + 2, 25, "TRC_%d", trc_wait[i]);
            row_print_color(i + 2, 35, margin ? C_BLUE : C_BLACK, C_WHITE, "%d KHz", margin
                ? raW_TRC[i] / 100 * (100 - RAM_MARGIN) / 1000
                : raW_TRC[i] / 1000);
        }

        fkey_action(1, "Reset");
        if (margin)
            fkey_button(2, "Margin");
        else
            fkey_action(2, "Margin");
        fkey_menu(5, "ROM");
        fkey_menu(6, "RAM");

        dupdate();
        key = getkey();
        switch (key.key)
        {
            case KEY_EXIT:
                return;
            
            case KEY_F1:
            case KEY_ON:
                for (int i = 0; i < 12; i++)
                    roR[i] = roR_default[i];
                for (int i = 0; i < 4; i++)
                    raW_TRC[i] = raW_TRC_default[i];
                break;
            
            case KEY_F2:
            case KEY_HOME:
                margin = !margin;
                break;

            case KEY_F5:
            case KEY_NEXTTAB:
                rom_test();
                break;
            case KEY_F6:
            case KEY_PAGEUP:
                #if defined CG50 || defined CG100
                warning_box(5, 6);
                row_print_color(6, 2, C_RED, C_WHITE,
                    "SDRAM test may cause system errors!");
                row_print_color(7, 2, C_RED, C_WHITE,
                    "It is highly recommended to press the RESTART");
                row_print_color(8, 2, C_RED, C_WHITE,
                    "button after this test is finished.");
                row_print(9, 2,
                    "Are you sure you want to continue?");
                if (yes_no(10))
                    sdram_test();
                #else
                // sram_test();
                #endif
                break;
        }
    }
}