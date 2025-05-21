/* UI utility from gintctl  */

#include <gint/display.h>
#include <gint/gint.h>
#include <gint/bfile.h>
#include <gint/config.h>

#include <util.h>
#include <stdio.h>
#include "config.h"

/* Short-shorthand for calling out vsprintf() */
#define shortprint(str, format) {	\
	va_list _args;			\
	va_start(_args, format);	\
	vsprintf(str, format, _args);	\
	va_end(_args);			\
}

//---
//	Row manipulation functions
//---

#if GINT_RENDER_MONO
#define ROW_X      1
#define ROW_W      6
#define ROW_Y      0
#define ROW_YPAD   0
#define ROW_H      8
#endif

#if GINT_RENDER_RGB
#define ROW_X      6
#define ROW_W      8
#define ROW_Y      20
#define ROW_YPAD   2
#define ROW_H      14
#endif

/* row_title(): Render the main title */
void row_title(char const *format, ...)
{
	char str[80];
	shortprint(str, format);

	#if GINT_RENDER_MONO
	dtext(1, 0, C_BLACK, str);
	#elif GINT_RENDER_RGB
	dtext(ROW_X, 3, C_BLACK, str);
	drect(0, 0, DWIDTH-1, 15, C_INVERT);
	#endif
}

/* row_print(): Formatted printing in a predefined row */
void row_print(int row, int x, char const *format, ...)
{
	char str[80];
	shortprint(str, format);

	dtext(ROW_X + ROW_W * (x - 1), ROW_Y + ROW_H * (row - 1) + ROW_YPAD,
		C_BLACK, str);
}

/* row_print_color(): Formatted printing... with custom colors! */
void row_print_color(int row, int x, int fg, int bg, char const *format, ...)
{
	char str[80];
	shortprint(str, format);

	dtext_opt(ROW_X + ROW_W * (x-1), ROW_Y + ROW_H * (row-1) + ROW_YPAD,
		fg, bg, DTEXT_LEFT, DTEXT_TOP, str);
}

/* row_highlight(): Invert a row's pixels to highlight it */
void row_highlight(int row)
{
	int y1 = ROW_Y + ROW_H * (row - 1);
	int y2 = y1 + ROW_H;

	#if GINT_RENDER_MONO
	drect(0, y1, 125, y2 - 1, C_INVERT);
	#elif GINT_RENDER_RGB
	drect(0, y1, DWIDTH - 1, y2 - 1, C_INVERT);
	#endif
}

/* row_right(): Print at the last column of a row */
void row_right(int row, char const *character)
{
	#if GINT_RENDER_MONO
	row_print(row, 21, character);
	#elif GINT_RENDER_RGB
	dtext(370, ROW_Y + ROW_H * (row - 1) + ROW_YPAD, C_BLACK, character);
	#endif
}

/* scrollbar(): Show a scrollbar */
void scrollbar(int offset, int length, int top, int bottom)
{
	int area_x      = _(127, 391);
	int area_width  = _(1, 2);
	int area_top    = ROW_Y + ROW_H * (top - 1);
	int area_height = ROW_H * (bottom - top);

	int bar_top = (offset * area_height) / length;
	int bar_height = ((bottom - top) * area_height) / length;

	drect(area_x, area_top + bar_top, area_x + area_width - 1,
		area_top + bar_top + bar_height, C_BLACK);
}

int row_x(int x)
{
	return ROW_X + ROW_W * (x - 1);
}
int row_y(int y)
{
	return ROW_Y + ROW_H * (y - 1) + ROW_YPAD;
}

//---
//	General rendering
//---

/* scrollbar_px(): Pixel-based scrollbar */
void scrollbar_px(int view_top, int view_bottom, int range_min, int range_max,
	int range_pos, int range_view)
{
	int view_x      = _(127, 391);
	int view_width  = _(1, 2);
	int view_height = view_bottom - view_top;

	/* Bring virtual range to 0..range_max */
	range_max -= range_min;
	range_pos -= range_min;

	int bar_pos    = (range_pos  * view_height + range_max/2) / range_max;
	int bar_height = (range_view * view_height + range_max/2) / range_max;

	drect(view_x, view_top + bar_pos, view_x + view_width - 1,
		view_top + bar_pos + bar_height, C_BLACK);
}

//---
//	Other drawing utilities
//---

#if GINT_RENDER_RGB

/* fkey_action(): A black-on-white F-key */
void fkey_action(int position, char const *text)
{
	int width;
	dsize(text, NULL, &width, NULL);

	int x = 4 + 65 * (position - 1);
	int y = 207;
	int w = 63;

	dline(x + 1, y, x + w - 2, y, C_BLACK);
	dline(x + 1, y + 14, x + w - 2, y + 14, C_BLACK);
	drect(x, y + 1, x + 1, y + 13, C_BLACK);
	drect(x + w - 2, y + 1, x + w - 1, y + 13, C_BLACK);

	dtext(x + ((w - width) >> 1), y + 3, C_BLACK, text);
}

void tab_action(int start, int end, char const *format, ...)
{
	char str[80];
	shortprint(str, format);
	
	int x_start = 4 + 65 * (start - 1);
	int x_end = 4 + 65 * (end - 1);
	int y = 207;
	int w = 63;

	int width;
	dsize(str, NULL, &width, NULL);
	
	dline(x_start + 1, y, x_end + w - 2, y, C_BLACK);
	dline(x_start + 1, y + 14, x_end + w - 2, y + 14, C_BLACK);
	drect(x_start, y + 1, x_start + 1, y + 13, C_BLACK);
	drect(x_end + w - 2, y + 1, x_end + w - 1, y + 13, C_BLACK);

	int diff = end - start;
	if (diff % 2)
		dtext(4 + 65 * (start - 1) + 32 * diff + (diff >> 1) + ((w - width) >> 1), y + 3, C_BLACK, str);
	else
		dtext(4 + 65 * (((start + end) >> 1) - 1) + ((w - width) >> 1), y + 3, C_BLACK, str);
}

/* fkey_button(): A rectangular F-key */
void fkey_button(int position, char const *text)
{
	int width;
	dsize(text, NULL, &width, NULL);

	int x = 4 + 65 * (position - 1);
	int y = 207;
	int w = 63;

	dline(x + 1, y, x + w - 2, y, C_BLACK);
	dline(x + 1, y + 14, x + w - 2, y + 14, C_BLACK);
	drect(x, y + 1, x + w - 1, y + 13, C_BLACK);

	dtext(x + ((w - width) >> 1), y + 3, C_WHITE, text);
}

void tab_menu(int start, int end, char const *format, ...)
{
	char str[80];
	shortprint(str, format);
	
	int x_start = 4 + 65 * (start - 1);
	int x_end = 4 + 65 * (end - 1);
	int y = 207;
	int w = 63;

	int width;
	dsize(str, NULL, &width, NULL);
	
	dline(x_start + 1, y, x_end + w - 2, y, C_BLACK);
	dline(x_start + 1, y + 14, x_end + w - 2, y + 14, C_BLACK);
	drect(x_start, y + 1, x_end + w - 1, y + 13, C_BLACK);

	int diff = end - start;
	if (diff % 2)
		dtext(4 + 65 * (start - 1) + 32 * diff + (diff >> 1) + ((w - width) >> 1), y + 3, C_WHITE, str);
	else
		dtext(4 + 65 * (((start + end) >> 1) - 1) + ((w - width) >> 1), y + 3, C_WHITE, str);
}

/* fkey_menu(): A rectangular F-key with the bottom right corner removed */
void fkey_menu(int position, char const *text)
{
	int x = 4 + 65 * (position - 1);
	int y = 207;
	int w = 63;

	fkey_button(position, text);
	dline(x + w - 1, y + 10, x + w - 5, y + 14, C_WHITE);
	dline(x + w - 1, y + 11, x + w - 4, y + 14, C_WHITE);
	dline(x + w - 1, y + 12, x + w - 3, y + 14, C_WHITE);
	dline(x + w - 1, y + 13, x + w - 2, y + 14, C_WHITE);
}

void tab_clear(int start, int end)
{
	int x_start = 4 + 65 * (start - 1);
	int x_end = 4 + 65 * (end - 1);
	int y = 207;
	int w = 63;

	drect(x_start, y + 1, x_end + w - 1, y + 13, C_WHITE);
}

static void msg_box_frame(int row, int size, int color)
{
	drect_border(3, ROW_Y + ROW_H * (row - 1) - 1,
		DWIDTH - 4, ROW_Y + ROW_H * (row + size - 1) - 1,
		C_WHITE, 2, color);
	drect(3, ROW_Y + ROW_H * (row - 1) - 1,
		DWIDTH - 4, ROW_Y + ROW_H * row - 1,
		color);
}

void info_box(int row, int size, const char *title)
{
	msg_box_frame(row, size, C_BLACK);
	row_print_color(row, 2, C_WHITE, C_BLACK, title);
}

void warning_box(int row, int size)
{
	msg_box_frame(row, size, C_RED);
	#if defined CP400
	row_print_color(row, 15, C_WHITE, C_RED, "WARNING");
	#else
	row_print_color(row, 21, C_WHITE, C_RED, "WARNING");
	#endif
}

bool yes_no(int row)
{
	#if defined CP400
	row_print(row, 7, "%s %21s", "[KBD]: Yes", "[DEL]: No");
	dupdate();
	while (true)
	{
		switch (getkey().key)
		{
			case KEY_KBD:
			case KEY_EXE:
				return true;
			case KEY_DEL:
				return false;
		}
	}
	#elif defined CG100
	row_print(row, 11, "%s %25s", "[OK]: Yes", "[BACK]: No");
	dupdate();
	while (true)
	{
		switch (getkey().key)
		{
			case KEY_OK:
			case KEY_EXE:
				return true;
			case KEY_BACK:
				return false;
		}
	}
	#else
	row_print(row, 11, "%s %25s", "[F1]: Yes", "[F6]: No");
	dupdate();
	while (true)
	{
		switch (getkey().key)
		{
			case KEY_F1:
			case KEY_EXE:
				return true;
			case KEY_F6:
			case KEY_EXIT:
				return false;
		}
	}
	#endif
}

void print_options(int row, int x, const char *option[], u8 select)
{
	for (int i = 0; option[i] != NULL; i++)
    {
		if (i == select)
        {
            row_print(row + i, x, ">");
            row_print_color(row + i, x + 1, C_WHITE, C_BLACK, option[i]);
        }
        else
            row_print(row + i, x + 1, option[i]);
    }
}

void row_clear(int row)
{
	int y1 = ROW_Y + ROW_H * (row - 1);
	int y2 = y1 + ROW_H;

	#if GINT_RENDER_MONO
	drect(0, y1, 125, y2 - 1, C_WHITE);
	#elif GINT_RENDER_RGB
	drect(0, y1, DWIDTH - 1, y2 - 1, C_WHITE);
	#endif
}

#endif /* FXCG50 */

static help_function_t help_function = NULL;

void set_help_function(help_function_t function)
{
	help_function = function;
}

void call_help_function()
{
	extern bool help_status;
	if (help_status)
		return;
	help_status = true;
	help_function();
	help_status = false;
}