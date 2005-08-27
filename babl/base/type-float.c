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
#include <assert.h>

#include "babl.h"
#include "babl-ids.h"

static void
convert_double_float (void *src,
                      void *dst,
                      int   src_pitch,
                      int   dst_pitch,
                      int   n)
{
  while (n--)
    {
      (*(float *) dst) = (*(double *) src);
      dst += dst_pitch;
      src += src_pitch;
    }
}

static void
convert_float_double (void *src,
                      void *dst,
                      int   src_pitch,
                      int   dst_pitch,
                      int   n)
{
  while (n--)
    {
      (*(double *) dst) = (*(float *) src);
      dst += dst_pitch;
      src += src_pitch;
    }
}

void
babl_base_type_float (void)
{
  babl_type_new (
    "float",
    "id",   BABL_FLOAT,
    "bits", 32,
    NULL);

  babl_conversion_new (
    babl_type_id (BABL_FLOAT),
    babl_type_id (BABL_DOUBLE),
    "linear", convert_float_double,
    NULL
  );

  babl_conversion_new (
    babl_type_id (BABL_DOUBLE),
    babl_type_id (BABL_FLOAT),
    "linear", convert_double_float,
    NULL
  );
}
