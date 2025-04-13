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

    while (true)
    {
        dclear(C_WHITE);
        row_title("Memory data");
        for (int i = 0; i < 10; i++)
        {
            static const u8 mem_wait[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12};
            row_print(i + 2, 1, "roR_%d: %d KHz", mem_wait[i], roR[i] / 100 * (100 - ROM_MARGIN) / 1000);
        }
        for (int i = 0; i < 4; i++)
        {
            static const u8 trc_wait[] = {3, 4, 6, 9};
            row_print(i + 2, 25, "TRC_%d: %d KHz", trc_wait[i], raW_TRC[i] / 100 * (100 - RAM_MARGIN) / 1000);
        }

        fkey_action(1, "Reset");
        fkey_button(5, "RAM");
        fkey_button(6, "ROM");

        dupdate();
        key = getkey();
        switch (key.key)
        {
            case KEY_EXIT:
                return;

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
                rom_test();
                break;
        }
    }
}