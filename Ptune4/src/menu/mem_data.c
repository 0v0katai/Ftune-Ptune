#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>

#include "util.h"
#include "validate.h"
#include "mem_test.h"
#include "settings.h"
#include "config.h"
#include "menu.h"

#ifdef ENABLE_HELP
static void help_info()
{
    #ifdef CG100
    info_box(4, 7, "HELP");
    row_print(5, 2, "[ON]: Reset to default");
    row_print(6, 2, "[|<-]: Toggle margin view");
    row_print(7, 2, "[->|]: ROM read test");
    row_print(8, 2, "[PGUP]: SDRAM write test");
    row_print(10, 2, "[BACK]: Close help / Return to express menu");
    #elif defined CG50
    info_box(4, 7, "HELP");
    row_print(5, 2, "[F1]: Reset to default");
    row_print(6, 2, "[F3]: Toggle margin view");
    row_print(7, 2, "[F5]: ROM read test");
    row_print(8, 2, "[F6]: SDRAM write test");
    row_print(10, 2, "[EXIT]: Close help / Return to express menu");
    #else
    info_box(3, 9, "HELP");
    row_print(4, 2, "[F1]: Reset to default");
    row_print(5, 2, "[F2]: Toggle SRAM read/write view");
    row_print(6, 2, "[F3]: Toggle margin view");
    row_print(7, 2, "[F5]: ROM read test");
    row_print(8, 2, "[F6]: SRAM read/write test");
    row_print(11, 2, "[EXIT]: Close help / Return to express menu");
    #endif
    dupdate();
    while (getkey().key != KEY_EXIT);
}
#endif

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
    #if !defined CG50 && !defined CG100 && !defined CP400
    bool mode = READ;
    static const u32 raR_default[] = {raR_0, raR_1, raR_2, raR_3, raR_4, raR_5, raR_6, raR_8};
    static const u32 raW_default[] = {raW_0, raW_1, raW_2, raW_3, raW_4, raW_5, raW_6};
    #else
    static const u32 raW_TRC_default[] = {raW_TRC_3, raW_TRC_4, raW_TRC_6, raW_TRC_9};
    #endif

    #ifdef ENABLE_HELP
    set_help_function(help_info);
    #endif

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
        #if defined CG50 || defined CG100 || defined CP400
        for (int i = 0; i < 4; i++)
        {
            static const u8 trc_wait[] = {3, 4, 6, 9};
            row_print(i + 2, 25, "TRC_%d", trc_wait[i]);
            row_print_color(i + 2, 35, margin ? C_BLUE : C_BLACK, C_WHITE, "%d KHz", margin
                ? raW_TRC[i] / 100 * (100 - RAM_MARGIN) / 1000
                : raW_TRC[i] / 1000);
        }
        #else
        if (mode == READ)
            for (int i = WAIT_0; i <= WAIT_8; i++)
            {
                row_print(i + 2, 25, "raR_%d", i == WAIT_8 ? 8 : i);
                row_print_color(i + 2, 35, margin ? C_BLUE : C_BLACK, C_WHITE, "%d KHz", margin
                    ? raR[i] / 100 * (100 - RAM_MARGIN) / 1000
                    : raR[i] / 1000);
            }
        else
            for (int i = WAIT_0; i <= WAIT_6; i++)
            {
                row_print(i + 2, 25, "raW_%d", i);
                row_print_color(i + 2, 35, margin ? C_BLUE : C_BLACK, C_WHITE, "%d KHz", margin
                    ? raW[i] / 100 * (100 - RAM_MARGIN) / 1000
                    : raW[i] / 1000);
            }
        #endif

        #ifndef CP400
        fkey_action(1, "Reset");
        #if !defined CG50 && !defined CG100
        if (mode)
            fkey_button(2, "Write");
        else
            fkey_button(2, "Read");
        #endif
        if (margin)
            fkey_button(3, "Margin");
        else
            fkey_action(3, "Margin");
        fkey_menu(5, "ROM");
        fkey_menu(6, "RAM");
        #endif

        dupdate();
        key = getkey();
        switch (key.key)
        { 
            case KEY_MEMDATA_RESET:
                for (int i = WAIT_0; i <= WAIT_18; i++)
                    roR[i] = roR_default[i];
                #if !defined CG50 && !defined CG100 && !defined CP400
                for (int i = WAIT_0; i <= WAIT_8; i++)
                    raR[i] = raR_default[i];
                for (int i = WAIT_0; i <= WAIT_6; i++)
                    raW[i] = raW_default[i];
                #else
                for (int i = 0; i < 4; i++)
                    raW_TRC[i] = raW_TRC_default[i];
                #endif
                break;

            #if !defined CG50 && !defined CG100 && !defined CP400
            case KEY_F2:
                mode = !mode;
                break;
            #endif

            case KEY_MEMDATA_MARGIN:
                margin = !margin;
                break;

            case KEY_MEMDATA_ROMTEST:
                rom_test();
                break;
            
            case KEY_MEMDATA_RAMTEST:
                #if defined CG50 || defined CG100 || defined CP400
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
                sram_test();
                #endif
                break;
            
            case KEY_EXIT:
                return;
        }
    }
}