#include <fxlibc/printf.h>
#include <gint/gint.h>
#include <gint/clock.h>
#include <gint/gdb.h>
#include <libprof.h>

#include "config.h"
#include "menu.h"

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

    express_menu();
    return 1;
}
