#include <gint/gint.h>
#include <gint/hardware.h>
#include <gint/clock.h>
#include <gint/gdb.h>
#include <gint/keyboard.h>
#include <gint/usb.h>
#include <gint/usb-ff-bulk.h>
#include <libprof.h>
#include <stdlib.h>

#include "config.h"
#include "menu.h"
#include "util.h"

bool help_status = false;

static bool global_getkey(key_event_t key)
{
    #ifdef ENABLE_USB
    if (key.shift && key.key == KEY_ENABLE_USB)
    {
        if (usb_is_open())
            usb_fxlink_screenshot(true);
        else
        {
            usb_interface_t const *interfaces[] = {&usb_ff_bulk, NULL};
            usb_open(interfaces, GINT_CALL_NULL);
            usb_open_wait();
        }
        return true;
    }
    #endif
    #ifdef ENABLE_HELP
    # if !defined CG100
    if (key.shift)
    # endif
    if (key.key == KEY_OPEN_HELP && !help_status)
        call_help_function();
    # if defined CG50 || defined CG100
    if (key.shift && key.key == KEY_ACON)
    {
        info_box(4, 7, "Caution");
        row_print(6, 2, "Poweroff function is disabled in this build");
        row_print(7, 2, "as it targets fx-CG50/100.");
        row_print(8, 2, "Please return to the main menu before");
        row_print(9, 2, "turning off your calculator.");
        xtune_getkey();
    }
    # endif
    #endif
    return false;
}

int main()
{
    #ifdef ENABLE_GDB
    gdb_start_on_exception();
    __asm__("trapa #42");
    #endif

    if (gint[HWCALC] != HARDWARE_TARGET)
        abort();

    prof_init();
    cpg_set_overclock_permanent(true);
    gint_setrestart(true);
    getkey_set_feature_function(global_getkey);

    express_menu();
    return 1;
}
