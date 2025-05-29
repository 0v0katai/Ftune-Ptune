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
    #if defined CP400
    info_box(15, 7, "HELP");
    row_print(16, 2, "[=]: Reset to default");
    row_print(17, 2, "[x]: Toggle margin view");
    row_print(18, 2, "[y]: ROM read test");
    row_print(19, 2, "[z]: SDRAM write test");
    row_print(21, 2, "[Clear]: Close help / < Express menu");
    #elif defined CG100
    info_box(4, 7, "HELP");
    row_print(5, 2, "[ON]: Reset to default");
    row_print(6, 2, "[|<-]: Toggle margin view");
    row_print(7, 2, "[->|]: ROM read test");
    row_print(8, 2, "[PGUP]: SDRAM write test");
    row_print(10, 2, "[BACK]: Close help / < Express menu");
    #elif defined CG50
    info_box(4, 7, "HELP");
    row_print(5, 2, "[F1]: Reset to default");
    row_print(6, 2, "[F3]: Toggle margin view");
    row_print(7, 2, "[F5]: ROM read test");
    row_print(8, 2, "[F6]: SDRAM write test");
    row_print(10, 2, "[EXIT]: Close help / < Express menu");
    #else
    info_box(3, 9, "HELP");
    row_print(4, 2, "[F1]: Reset to default");
    row_print(5, 2, "[F2]: Toggle SRAM read/write view");
    row_print(6, 2, "[F3]: Toggle margin view");
    row_print(7, 2, "[F5]: ROM read test");
    row_print(8, 2, "[F6]: SRAM read/write test");
    row_print(11, 2, "[EXIT]: Close help / < Express menu");
    #endif
    while (xtune_getkey().key != KEY_EXIT);
}
#endif

#if defined CP400
# define RAM_DISPLAY_ROW 14
# define OFFSET_X 1
# define TEST_DISPLAY_ROW 20
#else
# define RAM_DISPLAY_ROW 2
# define OFFSET_X 25
# define TEST_DISPLAY_ROW 10
#endif

void mem_data_menu()
{
    key_event_t key;
    bool margin = false;
    mem_test_settings test_settings = {.byte = 0b111};

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
        #if defined CP400
        row_print(13, 1, "RAM Margin: %d%%", RAM_MARGIN);
        #else
        row_print(1, 25, "RAM Margin: %d%%", RAM_MARGIN);
        #endif
        for (int i = 0; i < 10; i++)
        {
            row_print(i + 2, 1, "roR_%d", WR_equivalent(i));
            row_print_color(i + 2, 11, margin ? C_BLUE : C_BLACK, C_WHITE, "%d KHz", margin
                ? roR[i] / 100 * (100 - ROM_MARGIN) / 1000
                : roR[i] / 1000);
        }
        #if defined CP400 || defined CG50 || defined CG100
        for (int i = 0; i < 4; i++)
        {
            row_print(i + RAM_DISPLAY_ROW, OFFSET_X, "TRC_%d", TRC_equivalent(i));
            row_print_color(i + RAM_DISPLAY_ROW, OFFSET_X + 10,
                margin ? C_BLUE : C_BLACK, C_WHITE, "%d KHz", margin
                ? raW_TRC[i] / 100 * (100 - RAM_MARGIN) / 1000
                : raW_TRC[i] / 1000);
        }
        #else
        if (mode == READ)
            for (int i = WAIT_0; i <= WAIT_8; i++)
            {
                row_print(i + 2, 25, "raR_%d", WR_equivalent(i));
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

        static const char *on_off[] = {"Off", "On"};
        #if defined CP400 || defined CG50 || defined CG100
        for (int i = 0; i < 3; i++)
        {
            static const char *test_settings_name[3] = {"TRC_3", "roR_10", "roR_12"};
            row_print(TEST_DISPLAY_ROW - 1 + i, OFFSET_X, "%s Check [%d]", test_settings_name[i], i);
            row_print_color(TEST_DISPLAY_ROW - 1 + i, OFFSET_X + 18, C_WHITE, C_BLACK, on_off[(test_settings.byte >> (2 - i)) & 0b1]);
        }
        #else
        for (int i = 0; i < 2; i++)
        {
            row_print(TEST_DISPLAY_ROW, OFFSET_X, "roR_%d Check [%d]", i ? 12 : 10, i + 1);
            row_print_color(TEST_DISPLAY_ROW - 1 + i, OFFSET_X + 18, C_WHITE, C_BLACK, on_off[(test_settings.byte >> (1 - i)) & 0b1]);
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

        key = xtune_getkey();
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

            case KEY_0:
                test_settings.TRC_3_check = !test_settings.TRC_3_check;
                break;
            case KEY_1:
                test_settings.roR_10_check = !test_settings.roR_10_check;
                if (!test_settings.roR_10_check && test_settings.roR_12_check)
                    test_settings.roR_12_check = false;
                break;
            case KEY_2:
                test_settings.roR_12_check = !test_settings.roR_12_check;
                if (!test_settings.roR_10_check && test_settings.roR_12_check)
                    test_settings.roR_10_check = true;
                break;

            case KEY_MEMDATA_MARGIN:
                margin = !margin;
                break;

            case KEY_MEMDATA_ROMTEST:
                rom_test(test_settings);
                break;
            
            case KEY_MEMDATA_RAMTEST:
                #if defined CP400
                warning_box(15, 8);
                row_print_color(16, 2, C_RED, C_WHITE,
                    "SDRAM test may cause system errors!");
                row_print_color(17, 2, C_RED, C_WHITE,
                    "It is highly recommended to press the");
                row_print_color(18, 2, C_RED, C_WHITE,
                    "RESTART button after this test is");
                row_print_color(19, 2, C_RED, C_WHITE,
                    "finished.");
                row_print(21, 2,
                    "Are you sure you want to continue?");
                if (yes_no(22))
                    sdram_test(test_settings.TRC_3_check);
                #elif defined CG50 || defined CG100
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
                    sdram_test(test_settings.TRC_3_check);
                #else
                sram_test();
                #endif
                break;
            
            case KEY_EXIT:
                return;
        }
    }
}