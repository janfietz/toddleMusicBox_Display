/**
 * @file    effect_font_5x5.c
 * @brief
 *
 * @addtogroup effects
 * @{
 */

#include "effect_font_5x5.h"
#include <stdlib.h>
#include "fadeout.h"
#include "display.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/
struct EffectFontConfig
{
    uint8_t a[5];
    uint8_t b[5];
    uint8_t c[5];
    uint8_t d[5];
    uint8_t e[5];
    uint8_t f[5];
    uint8_t g[5];
    uint8_t h[5];
    uint8_t i[5];
    uint8_t j[5];
    uint8_t k[5];
    uint8_t l[5];
    uint8_t m[5];
    uint8_t n[5];
    uint8_t o[5];
    uint8_t p[5];
    uint8_t q[5];
    uint8_t r[5];
    uint8_t s[5];
    uint8_t t[5];
    uint8_t u[5];
    uint8_t v[5];
    uint8_t w[5];
    uint8_t x[5];
    uint8_t y[5];
    uint8_t z[5];

    uint8_t ze[5];
    uint8_t on[5];
    uint8_t tw[5];
    uint8_t th[5];
    uint8_t fo[5];
    uint8_t fi[5];
    uint8_t si[5];
    uint8_t se[5];
    uint8_t ei[5];
    uint8_t ni[5];
};

static struct EffectFontConfig effectFont =
{
    .a = {0b01111100,
          0b01000100,
          0b01000100,
          0b01111100,
          0b01000100},
    .b = {0b01111100,
          0b01000100,
          0b01111000,
          0b01000100,
          0b01111100},
    .c = {0x7c,0x40,0x40,0x40,0x7c},
    .d = {0x78,0x44,0x44,0x44,0x78},
    .e = {0x7c,0x40,0x78,0x40,0x7c},
    .f = {0x7c,0x40,0x70,0x40,0x40},
    .g = {0x7c,0x40,0x4c,0x44,0x7c},
    .h = {0x44,0x44,0x7c,0x44,0x44},
    .i = {0x7c,0x10,0x10,0x10,0x7c},
    .j = {0x0c,0x04,0x04,0x44,0x7c},
    .k = {0x44,0x48,0x70,0x48,0x44},
    .l = {0x40,0x40,0x40,0x40,0x7c},
    .m = {0x44,0x6c,0x54,0x44,0x44},
    .n = {0x44,0x64,0x54,0x4c,0x44},
    .o = {0x38,0x44,0x44,0x44,0x38},
    .p = {0x78,0x44,0x78,0x40,0x40},
    .q = {0x7c,0x44,0x44,0x7c,0x10},
    .r = {0x78,0x44,0x78,0x44,0x44},
    .s = {0x7c,0x40,0x7c,0x04,0x7c},
    .t = {0x7c,0x10,0x10,0x10,0x10},
    .u = {0x44,0x44,0x44,0x44,0x7c},
    .v = {0x44,0x44,0x28,0x28,0x10},
    .w = {0x44,0x44,0x54,0x54,0x28},
    .x = {0x44,0x28,0x10,0x28,0x44},
    .y = {0x44,0x44,0x28,0x10,0x10},
    .z = {0x7c,0x08,0x10,0x20,0x7c},
    .ze = {0x7c,0x4c,0x54,0x64,0x7c},
    .on = {0x10,0x30,0x10,0x10,0x38},
    .tw = {0x78,0x04,0x38,0x40,0x7c},
    .th = {0x7c,0x04,0x38,0x04,0x7c},
    .fo = {0x40,0x40,0x50,0x7c,0x10},
    .fi = {0x7c,0x40,0x78,0x04,0x78},
    .si = {0x7c,0x40,0x7c,0x44,0x7c},
    .se = {0x7c,0x04,0x08,0x10,0x10},
    .ei = {0x7c,0x44,0x7c,0x44,0x7c},
    .ni = {0x7c,0x44,0x7c,0x04,0x7c},
};
/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/
static void DrawCharConf(uint8_t* charConf, int16_t x, int16_t y, systime_t time, const struct Color* color, struct effect_t* next)
{
    uint8_t column;
    for (column = 0; column < 5; column++)
    {
        uint8_t line;
        uint8_t lineCfg = charConf[column] >> 2;
        for (line = 0; line < 5; line++)
        {
            if ((lineCfg & (0x10 >> line)) > 0)
            {
                EffectReset(next, x + line, y + column, time);
                EffectUpdate(next, x + line, y + column, time, color);
            }
        }
    }
}

static uint8_t* GetCharConf(const char c)
{
    if (c == 'a')
        return effectFont.a;
    if (c == 'b')
        return effectFont.b;
    if (c == 'c')
        return effectFont.c;
    if (c == 'd')
        return effectFont.d;
    if (c == 'e')
        return effectFont.e;
    if (c == 'f')
        return effectFont.f;
    if (c == 'g')
        return effectFont.g;
    if (c == 'h')
        return effectFont.h;
    if (c == 'i')
        return effectFont.i;
    if (c == 'j')
        return effectFont.j;
    if (c == 'k')
        return effectFont.k;
    if (c == 'l')
        return effectFont.l;
    if (c == 'm')
        return effectFont.m;
    if (c == 'n')
        return effectFont.n;
    if (c == 'o')
        return effectFont.o;
    if (c == 'p')
        return effectFont.p;
    if (c == 'q')
        return effectFont.q;
    if (c == 'r')
        return effectFont.r;
    if (c == 's')
        return effectFont.s;
    if (c == 't')
        return effectFont.t;
    if (c == 'u')
        return effectFont.u;
    if (c == 'v')
        return effectFont.v;
    if (c == 'w')
        return effectFont.w;
    if (c == 'x')
        return effectFont.x;
    if (c == 'y')
        return effectFont.y;
    if (c == 'z')
        return effectFont.z;
    if (c == '0')
        return effectFont.ze;
    if (c == '1')
        return effectFont.on;
    if (c == '2')
        return effectFont.tw;
    if (c == '3')
        return effectFont.th;
    if (c == '4')
        return effectFont.fo;
    if (c == '5')
        return effectFont.fi;
    if (c == '6')
        return effectFont.si;
    if (c == '7')
        return effectFont.se;
    if (c == '8')
        return effectFont.ei;
    if (c == '9')
        return effectFont.ni;

    return NULL;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief
 *
 */
msg_t EffectFont5x5Update(int16_t x, int16_t y, systime_t time, void* effectcfg, void* effectdata, const struct Color* color, struct effect_t* next)
{
    (void) color;
    (void) effectdata;
    struct EffectFont5x5Cfg* cfg = (struct EffectFont5x5Cfg*) effectcfg;

    uint8_t* charConf = GetCharConf(cfg->text[0]);
    if (charConf != NULL)
    {
        DrawCharConf(charConf, 3, 0, time, color, next);
    }

    return 0;
}

void EffectFont5x5Reset(int16_t x, int16_t y, systime_t time, void* effectcfg, void* effectdata, struct effect_t* next)
{
    (void)effectcfg;
    (void)effectdata;

    EffectReset(next, x, y, time);
}
/** @} */
