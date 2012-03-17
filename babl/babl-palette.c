/* babl - dynamically extendable universal pixel conversion library.
 * Copyright (C) 2005, Øyvind Kolås.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <stdlib.h>

#include "babl-internal.h"
#include "babl-classes.h"
#include "babl.h"
#include "babl-ids.h"
#include "util.h"
#include "rgb-constants.h"
#include "math.h"
#include "babl-base.h"

typedef struct BablPalette
{
  int         count; /* number of palette entries */
  const Babl *format;/* the pixel format the palette is stored in */
  void       *data;  /* one linear segment of all the pixels representing the palette, in   order */
} BablPalette;


static BablPalette *make_pal (Babl *format, void *data, int count)
{
  BablPalette *pal = NULL;
  int bpp = babl_format_get_bytes_per_pixel (format);
  pal = babl_malloc (sizeof (BablPalette));
  pal->count = count;
  pal->format = format;
  pal->data = babl_malloc (bpp * count);
  memcpy (pal->data, data, bpp * count);
  return pal;
}

static unsigned char defpal_data[4*16] = 
{  
0  ,0  ,0  ,255,
127,0  ,0  ,255,
0  ,127,0  ,255,
127,127,0  ,255,
0  ,0  ,127,255,
127,0  ,127,255,
0  ,127,127,255,
127,127,127,255,
63 ,63 ,63 ,255,
255,0  ,0  ,255,
0  ,255,0  ,255,
255,255,0  ,255,
0  ,0  ,255,255,
255,0  ,255,255,
0  ,255,255,255,
255,255,255,255,
};

static BablPalette *default_palette (void)
{
  static BablPalette pal;
  static int inited = 0;
  if (inited)
    return &pal;
  inited = 1;
  pal.count = 16;
  pal.format = babl_format ("RGBA u8");
  pal.data = defpal_data;
  return &pal;
}

static long
rgba_to_pal (char *src,
             char *dst,
             long  n,
             void *foo,
             void *dst_model_data)
{
  BablPalette *pal = dst_model_data;
  Babl        *fish;
  int bpp;
  
  if (babl_in_fish_path)
    babl_mutex_unlock (babl_format_mutex);
  fish = babl_fish (
      pal->format,
      babl_format_from_id (BABL_RGBA_DOUBLE));
  if (babl_in_fish_path)
    babl_mutex_lock (babl_format_mutex);

  bpp = babl_format_get_bytes_per_pixel (pal->format);
  while (n--)
    {
      int idx;

      int best_idx = 0;
      double best_diff = 100000;
      double *srcf;

      srcf = ((double *) src);

      for (idx = 0; idx<pal->count; idx++)
        {
          double diff;
          double palpx[4];

          /* oh horror, at least cache the format */
          babl_process (fish, ((char*)pal->data) + idx * bpp,
                palpx, 1);

          diff = (palpx[0] - srcf[0]) * (palpx[0] - srcf[0]) +
                 (palpx[1] - srcf[1]) * (palpx[1] - srcf[1]) +
                 (palpx[2] - srcf[2]) * (palpx[2] - srcf[2]) +
                 (palpx[3] - srcf[3]) * (palpx[3] - srcf[3]);
          if (diff < best_diff)
            {
              best_diff = diff;
              best_idx = idx;
            }
        }

      ((double *) dst)[0] = best_idx / 256.0;

      src += sizeof (double) * 4;
      dst += sizeof (double) * 1;
    }
  return n;
}

static long
pal_to_rgba (char *src,
             char *dst,
             long  n,
             void *src_model_data,
             void *foo)
{
  BablPalette *pal = src_model_data;
  Babl        *fish = babl_fish (
      pal->format,
      babl_format_from_id (BABL_RGBA_DOUBLE));
  int bpp = babl_format_get_bytes_per_pixel (pal->format);

  while (n--)
    {
      int    idx;

      idx       = (((double *) src)[0]) * 256.0;

      if (idx < 0) idx = 0;
      if (idx >= pal->count) idx = pal->count-1;

      babl_process (fish, 
            ((char*)pal->data) + idx * bpp,
            dst, 1);

      src += sizeof (double) * 1;
      dst += sizeof (double) * 4;
    }
  return n;
}

Babl *babl_new_palette (const char *name)
{
  Babl *model;
  Babl *format;
  char  cname[64];

  if (!name)
    {
      static int cnt = 0;
      sprintf (cname, "pal-%i", cnt++);
      name = cname;
    }

  babl_component_new (
    "I",
    "id", BABL_PAL,
    "luma",
    "chroma",
    "alpha",
    NULL);
  
  model = babl_model_new (
    "name", name,
    babl_component_from_id (BABL_PAL),
    NULL);
  
  model->model.data = (void*) 0x012;

  babl_conversion_new (
    model,
    babl_model_from_id (BABL_RGBA),
    "linear", pal_to_rgba,
    NULL
  );

  babl_conversion_new (
    babl_model_from_id (BABL_RGBA),
    model,
    "linear", rgba_to_pal,
    NULL
  );

  format = babl_format_new ("name", name, model,
                            babl_type ("u8"),
                            babl_component ("I"), NULL);

  format->format.model_data = default_palette ();
  format->format.model->data = format->format.model_data;

  babl_sanity ();
  return format;
}

void
babl_palette_set_palette (Babl *babl,
                          Babl *format,
                          void *data,
                          int   count)
{
  if (babl->format.model->data != default_palette ())
    {
      BablPalette *pal = babl->format.model->data;
      babl_free (pal->data);
      babl_free (pal);
    }
  babl->format.model_data = make_pal (format, data, count);
  babl->format.model->data = babl->format.model_data;
}

void
babl_palette_reset (Babl *babl)
{
  if (babl->format.model->data != default_palette ())
    {
      BablPalette *pal = babl->format.model->data;
      babl_free (pal->data);
      babl_free (pal);
    }
  babl->format.model_data = default_palette ();
  babl->format.model->data = babl->format.model_data;
}
