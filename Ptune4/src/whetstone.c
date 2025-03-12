#include <gint/defs/types.h>
#include <math.h>

#include "whetstone.h"
#include "config.h"

#ifdef ENABLE_WHET
static double xx1, xx2, xx3, xx4, x, y, z, t, t1, t2;
static double e1[4];
static u32 i, j, k, l, n1, n2, n3, n4, n6, n7, n8, n9, n10, n11;

static void _MODULE_1()
{
    xx1 = 1.0;
    xx2 = xx3 = xx4 = -1.0;

    for (i = 1; i <= n1; i += 1)
    {
        xx1 = (xx1 + xx2 + xx3 - xx4) * t;
        xx2 = (xx1 + xx2 - xx3 - xx4) * t;
        xx3 = (xx1 - xx2 + xx3 + xx4) * t;
        xx4 = (-xx1 + xx2 + xx3 + xx4) * t;
    }
}

static void _MODULE_2()
{
    e1[0] = 1.0;
    e1[1] = e1[2] = e1[3] = -1.0;

    for (i = 1; i <= n2; i += 1)
    {
        e1[0] = (e1[0] + e1[1] + e1[2] - e1[3]) * t;
        e1[1] = (e1[0] + e1[1] - e1[2] - e1[3]) * t;
        e1[2] = (e1[0] - e1[1] + e1[2] + e1[3]) * t;
        e1[3] = (-e1[0] + e1[1] + e1[2] + e1[3]) * t;
    }
}

static void pa_sub(e) double e[4];
{
    register int j;
    j = 0;
lab:
    e[0] = (e[0] + e[1] + e[2] - e[3]) * t;
    e[1] = (e[0] + e[1] - e[2] + e[3]) * t;
    e[2] = (e[0] - e[1] + e[2] + e[3]) * t;
    e[3] = (-e[0] + e[1] + e[2] + e[3]) / t2;
    j += 1;
    if (j < 6)
        goto lab;
}

static void _MODULE_3()
{
    for (i = 1; i <= n3; i += 1)
        pa_sub(e1);
}

static void _MODULE_4()
{
    j = 1;
    for (i = 1; i <= n4; i += 1)
    {
        if (j == 1)
            j = 2;
        else
            j = 3;
        if (j > 2)
            j = 0;
        else
            j = 1;
        if (j < 1)
            j = 1;
        else
            j = 0;
    }
}

/* module 5 skipped */

static void _MODULE_6()
{
    j = 1;
    k = 2;
    l = 3;

    for (i = 1; i <= n6; i += 1)
    {
        j = j * (k - j) * (l - k);
        k = l * k - (l - j) * k;
        l = (l - k) * (k + j);

        e1[l - 2] = j + k + l;
        e1[k - 2] = j * k * l;
    }
}

static void _MODULE_7()
{
    x = y = 1.0;
    for (i = 1; i <= n7; i += 1)
    {
        x = t * atan(t2 * sin(x) * cos(x) / (cos(x + y) + cos(x - y) - 1.0));
        y = t * atan(t2 * sin(y) * cos(y) / (cos(x + y) + cos(x - y) - 1.0));
    }
}

static void p3(x, y, z) double x, y, *z;
{
    x = t * (x + y);
    y = t * (x + y);
    *z = (x + y) / t2;
}

static void _MODULE_8()
{
    x = y = z = 1.0;
    for (i = 1; i <= n8; i += 1)
        p3(x, y, &z);
}

static void p0()
{
    e1[j] = e1[k];
    e1[k] = e1[l];
    e1[l] = e1[j];
}

static void _MODULE_9()
{
    j = 1;
    k = 2;
    l = 3;
    e1[0] = 1.0;
    e1[1] = 2.0;
    e1[2] = 3.0;
    for (i = 1; i <= n9; i += 1)
        p0();
}

static void _MODULE_10()
{
    j = 2;
    k = 3;
    for (i = 1; i <= n10; i += 1)
    {
        j = j + k;
        k = j + k;
        j = k - j;
        k = k - j - j;
    }
}

static void _MODULE_11()
{
    x = 0.75;
    for (i = 1; i <= n11; i += 1)
        x = sqrt(exp(log(x) / t1));
}

void whetstone()
{
    t = 0.499975;
    t1 = 0.50025;
    t2 = 2.0;

    n1 = 0 * ITERATIONS;
    n2 = 12 * ITERATIONS;
    n3 = 14 * ITERATIONS;
    n4 = 345 * ITERATIONS;
    n6 = 210 * ITERATIONS;
    n7 = 32 * ITERATIONS;
    n8 = 899 * ITERATIONS;
    n9 = 616 * ITERATIONS;
    n10 = 0 * ITERATIONS;
    n11 = 93 * ITERATIONS;

    _MODULE_1();
    _MODULE_2();
    _MODULE_3();
    _MODULE_4();
    _MODULE_6();
    _MODULE_7();
    _MODULE_8();
    _MODULE_9();
    _MODULE_10();
    _MODULE_11(); 
}
#endif
