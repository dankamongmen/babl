/* babl - dynamically extendable universal pixel conversion library.
 * Copyright (C) 2005-2008, Øyvind Kolås.
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

/* NOTE! the babl API is polymorphic, it is probably easier to learn
 * it's usage by studying examples than this header file. The public
 * functions are attempted explained anyways.
 */

#ifndef _BABL_H
#define _BABL_H

#include <stdlib.h>

#ifdef _BABL_INTERNAL_H
#error babl.h included after babl-internal.h
#endif

typedef struct _BablList BablList;

#include "babl-macros.h"
#include "babl-main.h"
#include "babl-class.h"

/**
 * babl_name:
 *
 * Return a string decsribing a BablInstance, might work better than
 * babl->instance.name when a good human readable name is desired.
 *
 * Returns: a name describing the instance.
 */
const char * babl_name       (const Babl *babl);

void         babl_introspect (Babl       *babl); /* introspect a given BablObject     */

#include "babl-version.h"
#include "babl-type.h"
#include "babl-sampling.h"
#include "babl-component.h"
#include "babl-model.h"
#include "babl-format.h"
#include "babl-image.h"
#include "babl-conversion.h"
#include "babl-fish.h"
#include "babl-extension.h"

/* This union can be used for convenient access to any field without the need
 * to case if the variable already is of the type Babl *
 */
typedef union _Babl
{
  BablClassType     class_type;
  BablInstance      instance;
  BablType          type;
  BablSampling      sampling;
  BablComponent     component;
  BablModel         model;
  BablFormat        format;
  BablConversion    conversion;
  BablImage         image;
  BablFish          fish;
  BablFishReference fish_reference;
  BablFishSimple    fish_simple;
  BablFishPath      fish_path;
  BablExtension     extension;
} _Babl;

#endif
