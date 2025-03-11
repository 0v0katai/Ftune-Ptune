//---
//	gintctl:util - Utility functions
//---

#ifndef GINTCTL_UTIL
#define GINTCTL_UTIL

#include <stdarg.h>
#include <stdint.h>
#include <gint/keyboard.h>
#include <gint/usb-ff-bulk.h>
#include <gint/config.h>

//---
//	Platform disambiguation functions
//---

#if GINT_HW_FX || GINT_FX9860G_G3A
# define _(fx,cg) fx
#else
# define _(fx,cg) cg
#endif

//---
// Application-specific getkey() with extra hooks
//---

/* Set to break out of gintctl_getkey_opt() */
extern int volatile gintctl_interrupt;

key_event_t gintctl_getkey_opt(int options);
key_event_t gintctl_getkey(void);

//---
//	Row manipulation functions
//---

/* row_title(): Render the main title */
void row_title(char const *format, ...);

/* row_print(): Formatted printing in a predefined row */
void row_print(int row, int x, char const *format, ...);

/* row_print_color(): Formatted printing... with custom colors! */
void row_print_color(int row, int x, int fg, int bg, char const *format, ...);

/* row_highlight(): Invert a row's pixels to highlight it */
void row_highlight(int row);

/* row_right(): Print at the last column of a row */
void row_right(int row, char const *character);

/* scrollbar(): Show a scrollbar */
void scrollbar(int offset, int length, int top, int bottom);

/* row_count(): Number of rows available to row_print() */
int row_count(void);

/* row_x() row_y(): Coordinates for row_print() */
int row_x(int x);
int row_y(int y);

//---
//	General rendering
//---

/* scrollbar_px(): Pixel-based scrollbar
   @view_top     First pixel covered by scrollbar area
   @view_bottom  Pixel below last pixel covered by scrollbar area
   @range_min    Minimum value in the virtual range we're scrolling through
   @range_max    Maximum value in the virtual range
   @range_pos    Position within the virtual range
   @range_view   How much of the range is visible on-screen */
void scrollbar_px(int view_top, int view_bottom, int range_min, int range_max,
	int range_pos, int range_view);

#define print_prefix(x, y, prefix, fmt, ...) { \
	dtext_opt(x-_(3,5), y, C_BLACK,C_NONE,DTEXT_RIGHT,DTEXT_TOP, prefix); \
	dprint(x+_(3,5), y, C_BLACK, fmt __VA_OPT__(,) __VA_ARGS__); \
}

//---
//	F-key rendering
//---

#if GINT_RENDER_RGB

/* fkey_action(): A black-on-white F-key */
void fkey_action(int position, char const *text);

/* fkey_button(): A rectangular F-key */
void fkey_button(int position, char const *text);

/* fkey_menu(): A rectangular F-key with the bottom right corner removed */
void fkey_menu(int position, char const *text);

void msg_box(int size, int center_position, char const *text);

void print_options(int row, int x, const char *option[], u8 select);

void row_clear(int row);

#endif /* FXCG50 */

//---
// USB commands
//---

void gintctl_handle_usb_command(usb_fxlink_header_t const *header);

#endif /* GINTCTL_UTIL */
