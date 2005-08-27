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

#include <math.h>
#include "babl.h"
#include "babl-ids.h"
#include "util.h"

static void models        (void);
static void components    (void);
static void conversions   (void);
static void formats       (void);

void
babl_base_model_rgb (void)
{
  components    ();
  models        ();
  conversions   ();
  formats       ();
}

static void
components (void)
{

  babl_component_new (
    "R'",
   "id",    BABL_RED_GAMMA_2_2,
    "luma",
    "chroma",
    NULL);

  babl_component_new (
    "G'",
   "id",    BABL_GREEN_GAMMA_2_2,
   "luma", 
   "chroma",
   NULL);
  
  babl_component_new (
    "B'",
   "id",    BABL_BLUE_GAMMA_2_2,
   "luma",
   "chroma",
   NULL);


  babl_component_new (
    "Ra",
    "id",    BABL_RED_MUL_ALPHA,
    "luma",
    "chroma",
    "alpha",
    NULL);
  babl_component_new (
    "Ga",
    "id",    BABL_GREEN_MUL_ALPHA,
    "luma",
    "chroma",
    "alpha",
    NULL);
  babl_component_new (
    "Ba",
    "id",    BABL_BLUE_MUL_ALPHA,
    "luma",
    "chroma",
    "alpha",
    NULL);

}

static void
models (void)
{

  babl_model_new (
    "id", BABL_RGB,
    babl_component_id (BABL_RED),
    babl_component_id (BABL_GREEN),
    babl_component_id (BABL_BLUE),
    NULL);

    babl_model_new (
    "id", BABL_RGBA_PREMULTIPLIED,
    babl_component_id (BABL_RED_MUL_ALPHA),
    babl_component_id (BABL_GREEN_MUL_ALPHA),
    babl_component_id (BABL_BLUE_MUL_ALPHA),
    babl_component_id (BABL_ALPHA),
    NULL);

  babl_model_new (
    "id", BABL_RGB_GAMMA_2_2,
    babl_component_id (BABL_RED_GAMMA_2_2),
    babl_component_id (BABL_GREEN_GAMMA_2_2),
    babl_component_id (BABL_BLUE_GAMMA_2_2),
    NULL);

  babl_model_new (
    "id", BABL_RGBA_GAMMA_2_2,
    babl_component_id (BABL_RED_GAMMA_2_2),
    babl_component_id (BABL_GREEN_GAMMA_2_2),
    babl_component_id (BABL_BLUE_GAMMA_2_2),
    babl_component_id (BABL_ALPHA),
    NULL);
}

static void
copy_strip_1 (int    src_bands,
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
      int i;

      for (i=0;i<dst_bands;i++)
        {
          double foo;
          if (i<src_bands)
            foo = *(double *) src[i];
          else
            foo = 1.0;
          *(double*)dst[i] = foo;
        }

      BABL_PLANAR_STEP
    }
}

static void
g3_gamma_2_2 (int    src_bands,
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
      int band;
      for (band=0;band<3;band++)
        *(double*)dst[band] = linear_to_gamma_2_2 (*(double*) src[band]);
      for (;band<dst_bands;band++)
        *(double*)dst[band] = *(double*) src[band];

      BABL_PLANAR_STEP
    }
}


static void
g3_inv_gamma_2_2 (int    src_bands,
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
      int band;
      for (band=0;band<3;band++)
        {
          *(double*)dst[band] = gamma_2_2_to_linear (*(double*) src[band]);
        }
      for (;band<dst_bands;band++)
        {
          if (band<src_bands)
            *(double*)dst[band] = *(double*) src[band];
          else
            *(double*)dst[band] = 1.0;
        }
      BABL_PLANAR_STEP
    }
}

static void
non_premultiplied_to_premultiplied (int    src_bands,
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
      double alpha;
      int band;

      alpha = *(double *)src[src_bands-1];
      for (band=0; band<src_bands-1;band++)
        {
          *(double*)dst[band] = *(double*) src[band] * alpha;
        }
      *(double*)dst[dst_bands-1] = alpha;

      BABL_PLANAR_STEP
    }
}

static void
premultiplied_to_non_premultiplied (int    src_bands,
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
      double alpha;
      int band;

      alpha = *(double *)src[src_bands-1];
      for (band=0; band<src_bands-1;band++)
        {
          if (alpha>0.001)
            {
              *(double*)dst[band] = *(double*) src[band] / alpha;
            }
          else
            {
              *(double*)dst[band] = 0.001;
            }
        }
      *(double*)dst[dst_bands-1] = alpha;

      BABL_PLANAR_STEP
    }
}

static void
conversions (void)
{
  babl_conversion_new (
    babl_model_id (BABL_RGBA),
    babl_model_id (BABL_RGBA),
    "planar",      copy_strip_1,
    NULL
  );

  babl_conversion_new (
    babl_model_id (BABL_RGBA),
    babl_model_id (BABL_RGB_GAMMA_2_2),
    "planar",      g3_gamma_2_2,
    NULL
  );

  babl_conversion_new (
    babl_model_id (BABL_RGBA),
    babl_model_id (BABL_RGBA_GAMMA_2_2),
    "planar",      g3_gamma_2_2,
    NULL
  );

  babl_conversion_new (
    babl_model_id (BABL_RGB_GAMMA_2_2),
    babl_model_id (BABL_RGBA),
    "planar",      g3_inv_gamma_2_2,
    NULL
  );

  babl_conversion_new (
    babl_model_id (BABL_RGBA_GAMMA_2_2),
    babl_model_id (BABL_RGBA),
    "planar",      g3_inv_gamma_2_2,
    NULL
  );

  babl_conversion_new (
    babl_model_id (BABL_RGB),
    babl_model_id (BABL_RGBA),
    "planar",      copy_strip_1,
    NULL
  );

  babl_conversion_new (
    babl_model_id (BABL_RGBA),
    babl_model_id (BABL_RGB),
    "planar",      copy_strip_1,
    NULL
  );

  babl_conversion_new (
    babl_model_id (BABL_RGBA),
    babl_model_id (BABL_RGBA_PREMULTIPLIED),
    "planar",      non_premultiplied_to_premultiplied,
    NULL
  );

  babl_conversion_new (
    babl_model_id (BABL_RGBA_PREMULTIPLIED),
    babl_model_id (BABL_RGBA),
    "planar",      premultiplied_to_non_premultiplied,
    NULL
  );
}

static void
formats (void)
{
    babl_format_new (
    "srgb",
    "id", BABL_SRGB,
    babl_model_id     (BABL_RGB_GAMMA_2_2),
    babl_type_id      (BABL_U8),
    babl_component_id (BABL_RED_GAMMA_2_2),
    babl_component_id (BABL_GREEN_GAMMA_2_2),
    babl_component_id (BABL_BLUE_GAMMA_2_2),
    NULL);

  babl_format_new (
    "srgba",
    "id", BABL_SRGBA,
    babl_model_id     (BABL_RGBA_GAMMA_2_2),
    babl_type_id      (BABL_U8),
    babl_component_id (BABL_RED_GAMMA_2_2),
    babl_component_id (BABL_GREEN_GAMMA_2_2),
    babl_component_id (BABL_BLUE_GAMMA_2_2),
    babl_component_id (BABL_ALPHA),
    NULL);

  babl_format_new (
    "rgba-float",
    "id",              BABL_RGBA_FLOAT,
    babl_model_id     (BABL_RGBA),
    babl_type_id      (BABL_FLOAT),
    babl_component_id (BABL_RED), 
    babl_component_id (BABL_GREEN), 
    babl_component_id (BABL_BLUE),
    babl_component_id (BABL_ALPHA),
    NULL);

  babl_format_new (
    "rgba-double",
    "id",              BABL_RGBA_DOUBLE,
    babl_model_id     (BABL_RGBA),
    babl_type_id      (BABL_DOUBLE),
    babl_component_id (BABL_RED), 
    babl_component_id (BABL_GREEN), 
    babl_component_id (BABL_BLUE),
    babl_component_id (BABL_ALPHA),
    NULL);

  babl_format_new (
    "rgb-float",
    "id", BABL_RGB_FLOAT,
    babl_model_id     (BABL_RGB),
    babl_type_id      (BABL_FLOAT),
    babl_component_id (BABL_RED), 
    babl_component_id (BABL_GREEN), 
    babl_component_id (BABL_BLUE),
    NULL);
}
