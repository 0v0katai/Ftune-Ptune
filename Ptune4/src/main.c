#include <fxlibc/printf.h>
#include <gint/gint.h>
#include <gint/clock.h>
#include <gint/gdb.h>
#include <gint/keyboard.h>
#include <gint/usb.h>
#include <gint/usb-ff-bulk.h>
#include <libprof.h>

#include "config.h"
#include "menu.h"
#include "util.h"

static bool global_getkey(key_event_t key)
{
    #ifdef ENABLE_USB
    if (key.shift)
        #ifdef CG100
        if (key.key == KEY_EXP)
        #else
        if (key.key == KEY_7)
        #endif
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
    # ifdef CG100
    if (key.key == KEY_CATALOG)
    # else
    if (key.shift && key.key == KEY_4)
    # endif
        call_help_function();
    #endif
    return false;
}

int main()
{
    #ifdef ENABLE_GDB
    gdb_start_on_exception();
    __asm__("trapa #42");
    #endif

    #ifdef ENABLE_FP
    __printf_enable_fp();
    #endif

    prof_init();
    cpg_set_overclock_permanent(true);
    gint_setrestart(true);
    getkey_set_feature_function(global_getkey);

    express_menu();
    return 1;
}
