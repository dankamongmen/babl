/* babl - dynamically extendable universal pixel conversion library.
 * Copyright (C) 2005, Øyvind Kolås.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include "babl.h"
#include "util.h"

static void types         (void);
static void components    (void);
static void models        (void);
static void conversions   (void);
static void formats       (void);

int
init (void)
{
  types         ();
  components    ();
  models        ();
  formats       ();
  conversions   ();
  return 0;
}

static void
components (void)
{
  babl_component_new ( "CIE L", NULL);
  babl_component_new ( "CIE a", "chroma", NULL);
  babl_component_new ( "CIE b", "chroma", NULL);
}

static void
models (void)
{
  babl_model_new (
    "name", "CIE Lab",
    babl_component ("CIE L"),
    babl_component ("CIE a"),
    babl_component ("CIE b"),
    NULL);

  babl_model_new (
    "name", "CIE Lab alpha",
    babl_component ("CIE L"),
    babl_component ("CIE a"),
    babl_component ("CIE b"),
    babl_component ("A"),
    NULL);
}

/***********    cpercep.h *********   */ 

/*
Copyright (C) 1997-2002 Adam D. Moss (the "Author").  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is fur-
nished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FIT-
NESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CON-
NECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the Author of the
Software shall not be used in advertising or otherwise to promote the sale,
use or other dealings in this Software without prior written authorization
from the Author.
*/

/*
  cpercep.c: The CPercep Functions v0.9: 2002-02-10
  Adam D. Moss: adam@gimp.org <http://www.foxbox.org/adam/code/cpercep/>

  TODO: document functions, rename erroneously-named arguments
*/

#ifndef __CPERCEP_H__
#define __CPERCEP_H__


void  cpercep_init         (void);

void  cpercep_rgb_to_space (double  inr,
                            double  ing,
                            double  inb,
                            double *outr,
                            double *outg,
                            double *outb);

void  cpercep_space_to_rgb (double  inr,
                            double  ing,
                            double  inb,
                            double *outr,
                            double *outg,
                            double *outb);


#if 0
/* This is in the header so that it can potentially be inlined. */
static const double
cpercep_distance_space (const double L1, const double a1, const double b1,
			const double L2, const double a2, const double b2)
{
  const double Ld = L1 - L2;
  const double ad = a1 - a2;
  const double bd = b1 - b2;

  return (Ld*Ld + ad*ad + bd*bd);
}
#endif


#endif /* __CPERCEP_H__ */

/***********   /cpercep.h *********   */ 



static void
rgb_to_lab (int    src_bands,
            void **src,
            int   *src_pitch,
            int    dst_bands,
            void **dst,
            int   *dst_pitch,
            int    n)
{
  BABL_PLANAR_SANITY

  while (n--)
    {
      double red   = *(double*)src[0];
      double green = *(double*)src[1];
      double blue  = *(double*)src[2];

      double L, a, b;

      cpercep_rgb_to_space (red, green, blue, &L, &a, &b);

      *(double*)dst[0] = L;
      *(double*)dst[1] = a;
      *(double*)dst[2] = b;

      if (dst_bands > 3)               /* alpha passthorugh */
        *(double*)dst[3] = (src_bands>3)?*(double*)src[3]:1.0;

      BABL_PLANAR_STEP
    }
}

static void
lab_to_rgb (int    src_bands,
            void **src,
            int   *src_pitch,
            int    dst_bands,
            void **dst,
            int   *dst_pitch,
            int    n)
{
  BABL_PLANAR_SANITY

  while (n--)
    {
      double L = *(double*)src[0];
      double a = *(double*)src[1];
      double b = *(double*)src[2];

      double red, green, blue;

      cpercep_space_to_rgb (L, a, b, &red, &green, &blue);

      *(double*)dst[0] = red;
      *(double*)dst[1] = green;
      *(double*)dst[2] = blue;

      if (dst_bands > 3)               /* alpha passthorugh */
        *(double*)dst[3] = (src_bands>3)?*(double*)src[3]:1.0;

      BABL_PLANAR_STEP
    }
}

static void
conversions (void)
{

  babl_conversion_new (
    babl_model ("RGBA"),
    babl_model ("CIE Lab"),
    "planar",      rgb_to_lab,
    NULL
  );
  babl_conversion_new (
    babl_model ("CIE Lab"),
    babl_model ("RGBA"),
    "planar",      lab_to_rgb,
    NULL
  );
  babl_conversion_new (
    babl_model ("RGB"),
    babl_model ("CIE Lab"),
    "planar",      rgb_to_lab,
    NULL
  );
  babl_conversion_new (
    babl_model ("CIE Lab"),
    babl_model ("RGB"),
    "planar",      lab_to_rgb,
    NULL
  );
  babl_conversion_new (
    babl_model ("RGBA"),
    babl_model ("CIE Lab alpha"),
    "planar",      rgb_to_lab,
    NULL
  );
  babl_conversion_new (
    babl_model ("CIE Lab alpha"),
    babl_model ("RGBA"),
    "planar",      lab_to_rgb,
    NULL
  );

  cpercep_init  ();
}

static void
formats (void)
{
  babl_format_new (
    "name",        "CIE Lab float",
    babl_model     ("CIE Lab"),

    babl_type      ("float"),
    babl_component ("CIE L"), 
    babl_component ("CIE a"), 
    babl_component ("CIE b"),
    NULL);

  babl_format_new (
    "name",        "CIE Lab u8",
    babl_model     ("CIE Lab"),

    babl_type      ("CIE u8 L"),
    babl_component ("CIE L"),
    babl_type      ("CIE u8 ab"),
    babl_component ("CIE a"), 
    babl_type      ("CIE u8 ab"),
    babl_component ("CIE b"),
    NULL);

  babl_format_new (
    "name",        "cie-lab-u16",
    babl_model     ("CIE Lab"),

    babl_type      ("CIE u16 L"),
    babl_component ("CIE L"),
    babl_type      ("CIE u16 ab"),
    babl_component ("CIE a"), 
    babl_type      ("CIE u16 ab"),
    babl_component ("CIE b"),
    NULL);
}


static inline void
convert_double_u8_scaled (double        min_val,
                          double        max_val,
                          unsigned char min,
                          unsigned char max,
                          void         *src,
                          void         *dst,
                          int           src_pitch,
                          int           dst_pitch,
                          int           n)
{
  while (n--)
    {
      double         dval = *(double *) src;
      unsigned char u8val;

      if (dval < min_val)
        u8val = min;
      else if (dval > max_val)
        u8val = max;
      else
        u8val = (dval-min_val) / (max_val-min_val) * (max-min) + min;

      *(unsigned char *) dst = u8val;
      src += src_pitch;
      dst += dst_pitch;
    }
}

static inline void
convert_u8_double_scaled (double        min_val,
                          double        max_val,
                          unsigned char min,
                          unsigned char max,
                          void         *src,
                          void         *dst,
                          int           src_pitch,
                          int           dst_pitch,
                          int           n)
{
  while (n--)
    {
      int    u8val = *(unsigned char *) src;
      double dval;

      if (u8val < min)
        dval = min_val;
      else if (u8val > max)
        dval = max_val;
      else
        dval  = (u8val-min) / (double)(max-min) * (max_val-min_val) + min_val;

      (*(double *) dst) = dval;

      dst += dst_pitch;
      src += src_pitch;
    }
}

#define MAKE_CONVERSIONS(name, min_val, max_val, min, max)      \
static void                                                     \
convert_##name##_double (void *src,                             \
                         void *dst,                             \
                         int   src_pitch,                       \
                         int   dst_pitch,                       \
                         int   n)                               \
{                                                               \
  convert_u8_double_scaled (min_val, max_val, min, max,         \
                            src, dst, src_pitch, dst_pitch, n); \
}                                                               \
static void                                                     \
convert_double_##name (void *src,                               \
                       void *dst,                               \
                       int   src_pitch,                         \
                       int   dst_pitch,                         \
                       int   n)                                 \
{                                                               \
  convert_double_u8_scaled (min_val, max_val, min, max,         \
                            src, dst, src_pitch, dst_pitch, n); \
}

/* source ICC.1:2004-10 */

MAKE_CONVERSIONS (u8_l,  0.0, 100.0,    0x00, 0xff);
MAKE_CONVERSIONS (u8_ab, -128.0, 127.0, 0x00, 0xff);

#undef MAKE_CONVERSIONS

static void
types_u8 (void)
{
  babl_type_new (
    "CIE u8 L",
    "integer",
    "unsigned",
    "bits",         8,
    "min_val",    0.0,
    "max_val",  100.0,
    NULL
  );

  babl_type_new (
    "CIE u8 ab",
    "integer",
    "unsigned",
    "bits",         8,
    "min_val",  -50.0,
    "max_val",   50.0,
    NULL
  );

  babl_conversion_new (
    babl_type ("CIE u8 L"),
    babl_type ("double"),
    "linear",      convert_u8_l_double,
    NULL
  );
  babl_conversion_new (
    babl_type ("double"),
    babl_type ("CIE u8 L"),
    "linear",      convert_double_u8_l,
    NULL
  );

  babl_conversion_new (
    babl_type ("CIE u8 ab"),
    babl_type ("double"),
    "linear",      convert_u8_ab_double,
    NULL
  );
  babl_conversion_new (
    babl_type ("double"),
    babl_type ("CIE u8 ab"),
    "linear",      convert_double_u8_ab,
    NULL
  );
}

static inline void
convert_double_u16_scaled (double         min_val,
                           double         max_val,
                           unsigned short min,
                           unsigned short max,
                           void          *src,
                           void          *dst,
                           int            src_pitch,
                           int            dst_pitch,
                           int            n)
{
  while (n--)
    {
      double         dval = *(double *) src;
      unsigned short u16val;

      if (dval < min_val)
        u16val = min;
      else if (dval > max_val)
        u16val = max;
      else
        u16val = (dval-min_val) / (max_val-min_val) * (max-min) + min;

      *(unsigned short *) dst = u16val;
      dst += dst_pitch;
      src += src_pitch;
    }
}

static inline void
convert_u16_double_scaled (double         min_val,
                           double         max_val,
                           unsigned short min,
                           unsigned short max,
                           void          *src,
                           void          *dst,
                           int            src_pitch,
                           int            dst_pitch,
                           int            n)
{
  while (n--)
    {
      int    u16val = *(unsigned short*) src;
      double dval;

      if (u16val < min)
        dval = min_val;
      else if (u16val > max)
        dval = max_val;
      else
        dval  = (u16val-min) / (double)(max-min) * (max_val-min_val) + min_val;

      (*(double *) dst) = dval;
      dst += dst_pitch;
      src += src_pitch;
    }
}

#define MAKE_CONVERSIONS(name, min_val, max_val, min, max)      \
static void                                                     \
convert_##name##_double (void *src,                             \
                         void *dst,                             \
                         int   src_pitch,                       \
                         int   dst_pitch,                       \
                         int   n)                               \
{                                                               \
  convert_u16_double_scaled (min_val, max_val, min, max,        \
                             src, dst, src_pitch, dst_pitch, n);\
}                                                               \
static void                                                     \
convert_double_##name (void *src,                               \
                       void *dst,                               \
                       int   src_pitch,                         \
                       int   dst_pitch,                         \
                       int   n)                                 \
{                                                               \
  convert_double_u16_scaled (min_val, max_val, min, max,        \
                             src, dst, src_pitch, dst_pitch, n);\
}

MAKE_CONVERSIONS (u16_l,    0.0,  100.0, 0x00, 0xffff);
MAKE_CONVERSIONS (u16_ab, -128.0, 127.0, 0x00, 0xffff);

#undef MAKE_CONVERSIONS

static void
types_u16 (void)
{

  babl_type_new (
    "CIE u16 L",
    "integer",
    "unsigned",
    "bits",        16,
    "min_val",    0.0,
    "max_val",  100.0,
    NULL
  );

  babl_type_new (
    "CIE u16 ab",
    "id",       "CIE u8 ab",
    "integer",
    "unsigned",
    "bits",        16,
    "min_val",  -50.0,
    "max_val",   50.0,
    NULL
  );


  babl_conversion_new (
    babl_type ("CIE u16 L"),
    babl_type ("double"),
    "linear",      convert_u16_l_double,
    NULL
  );
  babl_conversion_new (
    babl_type ("double"),
    babl_type ("CIE u16 L"),
    "linear",      convert_double_u16_l,
    NULL
  );

  babl_conversion_new (
    babl_type ("CIE u16 ab"),
    babl_type ("double"),
    "linear",      convert_u16_ab_double,
    NULL
  );
  babl_conversion_new (
    babl_type ("double"),
    babl_type ("CIE u16 ab"),
    "linear",      convert_double_u16_ab,
    NULL
  );
}

static void
types (void)
{
  types_u8 ();
  types_u16 ();
}




/***********   cpercep.c *********   */ 


/*
Copyright (C) 1999-2002 Adam D. Moss (the "Author").  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is fur-
nished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FIT-
NESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CON-
NECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the Author of the
Software shall not be used in advertising or otherwise to promote the sale,
use or other dealings in this Software without prior written authorization
from the Author.
*/

/*
  cpercep.c: The CPercep Functions v0.9: 2002-02-10
  Adam D. Moss: adam@gimp.org <http://www.foxbox.org/adam/code/cpercep/>

  This code module concerns itself with conversion from a hard-coded
  RGB colour space (sRGB by default) to CIE L*a*b* and back again with
  (primarily) precision and (secondarily) speed, oriented largely
  towards the purposes of quantifying the PERCEPTUAL difference between
  two arbitrary RGB colours with a minimum of fuss.

  Motivation One: The author is disheartened at the amount of graphics
  processing software around which uses weighted or non-weighted
  Euclidean distance between co-ordinates within a (poorly-defined) RGB
  space as the basis of what should really be an estimate of perceptual
  difference to the human eye.  Certainly it's fast to do it that way,
  but please think carefully about whether your particular application
  should be tolerating sloppy results for the sake of real-time response.

  Motivation Two: Lack of tested, re-usable and free code available
  for this purpose.  The difficulty in finding something similar to
  CPercep with a free license motivated this project; I hope that this
  code also serves to illustrate how to perform the
  R'G'B'->XYZ->L*a*b*->XYZ->R'G'B' transformation correctly since I
  was distressed to note how many of the equations and code snippets
  on the net were omitting the reverse transform and/or were using
  incorrectly-derived or just plain wrong constants.

  TODO: document functions, rename erroneously-named arguments
*/

/* defines added to make it compile outside gimp */

#ifndef gboolean
#define gboolean int
#endif
#ifndef FALSE
#define FALSE    0
#endif
#ifndef TRUE
#define TRUE     1
#endif


/* #include "config.h" */
#include <math.h>

#ifndef __GLIBC__
/* cbrt() is a GNU extension */
#define cbrt(x) (pow(x, 1.0/3.0))
#endif

/* #include "cpercep.h" */


/* defines:

   SANITY: emits warnings when passed non-sane colours (and usually
   corrects them) -- useful when debugging.

   APPROX: speeds up the conversion from RGB to the colourspace by
   assuming that the RGB values passed in are integral and definitely
   in the range 0->255

   SRGB: assumes that the RGB values being passed in (and out) are
   destined for an sRGB-alike display device (a typical modern monitor)
   -- if you change this then you'll probably want to change ASSUMED_GAMMA,
   the phosphor colours and the white point definition.
*/

/* #define SANITY */
/* #define APPROX */
/* #define SRGB   */


#ifdef SRGB
#define ASSUMED_GAMMA (2.2F)
#else
/*#define ASSUMED_GAMMA (2.591F)*/
#define ASSUMED_GAMMA (1.0F)
#endif

#define REV_GAMMA ((1.0F / ASSUMED_GAMMA))


/* define characteristics of the source RGB space (and the space
   within which we try to behave linearly). */

/* Phosphor colours: */

/* sRGB/HDTV phosphor colours */
static const double pxr = 0.64F;
static const double pyr = 0.33F;
static const double pxg = 0.30F;
static const double pyg = 0.60F;
static const double pxb = 0.15F;
static const double pyb = 0.06F;

/* White point: */

/* D65 (6500K) (recommended but not a common display default) */
static const double lxn = 0.312713F;
static const double lyn = 0.329016F;

/* D50 (5000K) */
/*static const double lxn = 0.3457F; */
/*static const double lyn = 0.3585F; */

/* D55 (5500K) */
/*static const double lxn = 0.3324F; */
/*static const double lyn = 0.3474F; */

/* D93 (9300K) (a common monitor default, but poor colour reproduction) */
/* static const double lxn = 0.2848F; */
/* static const double lyn = 0.2932F; */

/* illum E (normalized) */
/*static const double lxn = 1.0/3.0F; */
/*static const double lyn = 1.0/3.0F; */

/* illum C (average sunlight) */
/*static const double lxn = 0.3101F; */
/*static const double lyn = 0.3162F; */

/* illum B (direct sunlight) */
/*static const double lxn = 0.3484F; */
/*static const double lyn = 0.3516F; */

/* illum A (tungsten lamp) */
/*static const double lxn = 0.4476F; */
/*static const double lyn = 0.4074F; */


static const double LRAMP = 7.99959199F;


static double xnn, znn;

static double powtable[256];


#ifndef CLAMP
#define CLAMP(x,l,u) ((x)<(l)?(l):((x)>(u)?(u):(x)))
#endif


static void
init_powtable(const double gamma)
{
  int i;

#ifndef SRGB
  /* pure gamma function */
  for (i=0; i<256; i++)
    {
      powtable[i] = pow((i)/255.0F, gamma);
    }
#else
  /* sRGB gamma curve */
  for (i=0; i<11 /* 0.03928 * 255 */; i++)
    {
      powtable[i] = (i) / (255.0F * 12.92F);
    }
  for (; i<256; i++)
    {
      powtable[i] = pow( (((i) / 255.0F) + 0.055F) / 1.055F, 2.4F);
    }
#endif
}


typedef double CMatrix[3][3];
typedef double CVector[3];

static CMatrix Mrgb_to_xyz, Mxyz_to_rgb;

static int
Minvert (CMatrix src, CMatrix dest)
{
  double det;

  dest[0][0] = src[1][1] * src[2][2] - src[1][2] * src[2][1];
  dest[0][1] = src[0][2] * src[2][1] - src[0][1] * src[2][2];
  dest[0][2] = src[0][1] * src[1][2] - src[0][2] * src[1][1];
  dest[1][0] = src[1][2] * src[2][0] - src[1][0] * src[2][2];
  dest[1][1] = src[0][0] * src[2][2] - src[0][2] * src[2][0];
  dest[1][2] = src[0][2] * src[1][0] - src[0][0] * src[1][2];
  dest[2][0] = src[1][0] * src[2][1] - src[1][1] * src[2][0];
  dest[2][1] = src[0][1] * src[2][0] - src[0][0] * src[2][1];
  dest[2][2] = src[0][0] * src[1][1] - src[0][1] * src[1][0];

  det =
    src[0][0] * dest[0][0] +
    src[0][1] * dest[1][0] +
    src[0][2] * dest[2][0];

  if (det <= 0.0F)
    {
#ifdef SANITY
      g_printerr ("\n\007 XXXX det: %f\n", det);
#endif
      return 0;
    }

  dest[0][0] /= det;
  dest[0][1] /= det;
  dest[0][2] /= det;
  dest[1][0] /= det;
  dest[1][1] /= det;
  dest[1][2] /= det;
  dest[2][0] /= det;
  dest[2][1] /= det;
  dest[2][2] /= det;

  return 1;
}


static void
rgbxyzrgb_init(void)
{
  init_powtable (ASSUMED_GAMMA);

  xnn = lxn / lyn;
  /* ynn taken as 1.0 */
  znn = (1.0F - (lxn + lyn)) / lyn;

  {
    CMatrix MRC, MRCi;
    double C1,C2,C3;

    MRC[0][0] = pxr;
    MRC[0][1] = pxg;
    MRC[0][2] = pxb;
    MRC[1][0] = pyr;
    MRC[1][1] = pyg;
    MRC[1][2] = pyb;
    MRC[2][0] = 1.0F - (pxr + pyr);
    MRC[2][1] = 1.0F - (pxg + pyg);
    MRC[2][2] = 1.0F - (pxb + pyb);

    Minvert (MRC, MRCi);

    C1 = MRCi[0][0]*xnn + MRCi[0][1] + MRCi[0][2]*znn;
    C2 = MRCi[1][0]*xnn + MRCi[1][1] + MRCi[1][2]*znn;
    C3 = MRCi[2][0]*xnn + MRCi[2][1] + MRCi[2][2]*znn;

    Mrgb_to_xyz[0][0] = MRC[0][0] * C1;
    Mrgb_to_xyz[0][1] = MRC[0][1] * C2;
    Mrgb_to_xyz[0][2] = MRC[0][2] * C3;
    Mrgb_to_xyz[1][0] = MRC[1][0] * C1;
    Mrgb_to_xyz[1][1] = MRC[1][1] * C2;
    Mrgb_to_xyz[1][2] = MRC[1][2] * C3;
    Mrgb_to_xyz[2][0] = MRC[2][0] * C1;
    Mrgb_to_xyz[2][1] = MRC[2][1] * C2;
    Mrgb_to_xyz[2][2] = MRC[2][2] * C3;

    Minvert (Mrgb_to_xyz, Mxyz_to_rgb);
  }
}


static void
xyz_to_rgb (double *inx_outr,
	    double *iny_outg,
	    double *inz_outb)
{
  const double x = *inx_outr;
  const double y = *iny_outg;
  const double z = *inz_outb;

  *inx_outr = Mxyz_to_rgb[0][0]*x + Mxyz_to_rgb[0][1]*y + Mxyz_to_rgb[0][2]*z;
  *iny_outg = Mxyz_to_rgb[1][0]*x + Mxyz_to_rgb[1][1]*y + Mxyz_to_rgb[1][2]*z;
  *inz_outb = Mxyz_to_rgb[2][0]*x + Mxyz_to_rgb[2][1]*y + Mxyz_to_rgb[2][2]*z;
}


static void
rgb_to_xyz (double *inr_outx,
	    double *ing_outy,
	    double *inb_outz)
{
  const double r = *inr_outx;
  const double g = *ing_outy;
  const double b = *inb_outz;

  *inr_outx = Mrgb_to_xyz[0][0]*r + Mrgb_to_xyz[0][1]*g + Mrgb_to_xyz[0][2]*b;
  *ing_outy = Mrgb_to_xyz[1][0]*r + Mrgb_to_xyz[1][1]*g + Mrgb_to_xyz[1][2]*b;
  *inb_outz = Mrgb_to_xyz[2][0]*r + Mrgb_to_xyz[2][1]*g + Mrgb_to_xyz[2][2]*b;
}


static inline double
ffunc(const double t)
{
  if (t > 0.008856F)
    {
      return (cbrt(t));
    }
  else
    {
      return (7.787F * t + 16.0F/116.0F);
    }
}


static inline double
ffunc_inv(const double t)
{
  if (t > 0.206893F)
    {
      return (t * t * t);
    }
  else
    {
      return ((t - 16.0F/116.0F) / 7.787F);
    }
}


static void
xyz_to_lab (double *inx,
	    double *iny,
	    double *inz)
{
  double L,a,b;
  double ffuncY;
  const double X = *inx;
  const double Y = *iny;
  const double Z = *inz;

  if (Y > 0.0F)
    {
      if (Y > 0.008856F)
	{
	  L = (116.0F * cbrt(Y)) - 16.0F;
	}
      else
	{
	  L = (Y * 903.3F);
	}

#ifdef SANITY
      if (L < 0.0F)
	{
	  g_printerr (" <eek1>%f \007",(float)L);
	}

      if (L > 100.0F)
	{
	  g_printerr (" <eek2>%f \007",(float)L);
	}
#endif
    }
  else
    {
      L = 0.0;
    }

  ffuncY = ffunc(Y);
  a = 500.0F * (ffunc(X/xnn) - ffuncY);
  b = 200.0F * (ffuncY - ffunc(Z/znn));

  *inx = L;
  *iny = a;
  *inz = b;
}


static void
lab_to_xyz (double *inl,
	    double *ina,
	    double *inb)
{
  double X,Y,Z;
  double P;
  const double L = *inl;
  const double a = *ina;
  const double b = *inb;

  if (L > LRAMP)
    {
      P = Y = (L + 16.0F) / 116.0F;
      Y = Y * Y * Y;
    }
  else
    {
      Y = L / 903.3F;
      P = 7.787F * Y + 16.0F/116.0F;
    }

  X = (P + a / 500.0F);
  X = xnn * ffunc_inv(X);
  Z = (P - b / 200.0F);
  Z = znn * ffunc_inv(Z);

#ifdef SANITY
  if (X<-0.00000F)
    {
      if (X<-0.0001F)
	g_printerr ("{badX %f {%f,%f,%f}}",X,L,a,b);
      X = 0.0F;
    }
  if (Y<-0.00000F)
    {
      if (Y<-0.0001F)
	g_printerr ("{badY %f}",Y);
      Y = 0.0F;
    }
  if (Z<-0.00000F)
    {
      if (Z<-0.1F)
	g_printerr ("{badZ %f}",Z);
      Z = 0.0F;
    }
#endif

  *inl = X;
  *ina = Y;
  *inb = Z;
}



/* call this before using the CPercep function */
void
cpercep_init (void)
{
  static gboolean initialized = FALSE;

  if (! initialized)
    {
      rgbxyzrgb_init();
      initialized = TRUE;
    }
}

void
cpercep_rgb_to_space (double  inr,
                      double  ing,
                      double  inb,
		      double *outr,
                      double *outg,
                      double *outb)
{
#ifdef APPROX
#ifdef SANITY
  /* ADM extra sanity */
  if ((inr) > 255.0F ||
      (ing) > 255.0F ||
      (inb) > 255.0F ||
      (inr) < -0.0F ||
      (ing) < -0.0F ||
      (inb) < -0.0F
      )
    abort();
#endif /* SANITY */
  inr = powtable[(int)inr];
  ing = powtable[(int)ing];
  inb = powtable[(int)inb];
#else
#ifdef SRGB
  /* sRGB gamma curve */
  if (inr <= (0.03928F * 255.0F))
      inr = inr / (255.0F * 12.92F);
  else
      inr = pow( (inr + (0.055F * 255.0F)) / (1.055F * 255.0F), 2.4F);

  if (ing <= (0.03928F * 255.0F))
      ing = ing / (255.0F * 12.92F);
  else
      ing = pow( (ing + (0.055F * 255.0F)) / (1.055F * 255.0F), 2.4F);

  if (inb <= (0.03928F * 255.0F))
      inb = inb / (255.0F * 12.92F);
  else
      inb = pow( (inb + (0.055F * 255.0F)) / (1.055F * 255.0F), 2.4F);
#else
  /* pure gamma function */

  /*  babl uses normalized RGB
  inr = pow((inr)/255.0F, ASSUMED_GAMMA);
  ing = pow((ing)/255.0F, ASSUMED_GAMMA);
  inb = pow((inb)/255.0F, ASSUMED_GAMMA);
  */
#endif /* SRGB */
#endif /* APPROX */

#ifdef SANITY
  /* ADM extra sanity */
  if ((inr) > 1.0F ||
      (ing) > 1.0F ||
      (inb) > 1.0F ||
      (inr) < 0.0F ||
      (ing) < 0.0F ||
      (inb) < 0.0F
      )
    {
      g_printerr ("%%");
      /* abort(); */
    }
#endif /* SANITY */

  rgb_to_xyz(&inr, &ing, &inb);

#ifdef SANITY
  if (inr < 0.0F || ing < 0.0F || inb < 0.0F)
    {
      g_printerr (" [BAD2 XYZ: %f,%f,%f]\007 ",
	      inr,ing,inb);
    }
#endif /* SANITY */

  xyz_to_lab(&inr, &ing, &inb);

  *outr = inr;
  *outg = ing;
  *outb = inb;
}


void
cpercep_space_to_rgb (double  inr,
                      double  ing,
                      double  inb,
		      double *outr,
                      double *outg,
                      double *outb)
{
  lab_to_xyz(&inr, &ing, &inb);

#ifdef SANITY
  if (inr<-0.0F || ing<-0.0F || inb<-0.0F)
    {
      g_printerr (" [BAD1 XYZ: %f,%f,%f]\007 ",
	      inr,ing,inb);
    }
#endif

  xyz_to_rgb(&inr, &ing, &inb);

  /* yes, essential.  :( */
  inr = CLAMP(inr,0.0F,1.0F);
  ing = CLAMP(ing,0.0F,1.0F);
  inb = CLAMP(inb,0.0F,1.0F);

#ifdef SRGB
  if (inr <= 0.0030402477F)
    inr = inr * (12.92F * 255.0F);
  else
    inr = pow(inr, 1.0F/2.4F) * (1.055F * 255.0F) - (0.055F * 255.0F);

  if (ing <= 0.0030402477F)
    ing = ing * (12.92F * 255.0F);
  else
    ing = pow(ing, 1.0F/2.4F) * (1.055F * 255.0F) - (0.055F * 255.0F);

  if (inb <= 0.0030402477F)
    inb = inb * (12.92F * 255.0F);
  else
    inb = pow(inb, 1.0F/2.4F) * (1.055F * 255.0F) - (0.055F * 255.0F);
#else
  /* babl uses normalized RGB values
  inr = 255.0F * pow(inr, REV_GAMMA);
  ing = 255.0F * pow(ing, REV_GAMMA);
  inb = 255.0F * pow(inb, REV_GAMMA);
  */
#endif

  *outr = inr;
  *outg = ing;
  *outb = inb;
}


#if 0
/* EXPERIMENTAL SECTION */

const double
xscaler(const double start, const double end,
	const double me, const double him)
{
  return start + ((end-start) * him) / (me + him);
}


void
mix_colours (const double L1, const double a1, const double b1,
	     const double L2, const double a2, const double b2,
	     double *rtnL, double *rtna, double *rtnb,
	     double mass1, double mass2)
{
  double w1, w2;

#if 0
  *rtnL = xscaler (L1, L2, mass1, mass2);
  *rtna = xscaler (a1, a2, mass1, mass2);
  *rtnb = xscaler (b1, b2, mass1, mass2);
#else

#if 1
  w1 = mass1 * L1;
  w2 = mass2 * L2;
#else
  w1 = mass1 * (L1*L1*L1);
  w2 = mass2 * (L2*L2*L2);
#endif

  *rtnL = xscaler (L1, L2, mass1, mass2);

  if (w1 <= 0.0 &&
      w2 <= 0.0)
    {
      *rtna =
	*rtnb = 0.0;
#ifdef SANITY
      /* g_printerr ("\007OUCH. "); */
#endif
    }
  else
    {
      *rtna = xscaler(a1, a2, w1, w2);
      *rtnb = xscaler(b1, b2, w1, w2);
    }
#endif
}
#endif /* EXPERIMENTAL SECTION */

/***********  /cpercep.c *********   */ 