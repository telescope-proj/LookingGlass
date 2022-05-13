/**
 * Looking Glass
 * Copyright © 2017-2022 The Looking Glass Authors
 * https://looking-glass.io
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "common/KVMFR.h"

const char * FrameTypeStr[FRAME_TYPE_MAX] =
{
  "FRAME_TYPE_INVALID",
  "FRAME_TYPE_BGRA",
  "FRAME_TYPE_RGBA",
  "FRAME_TYPE_RGBA10",
  "FRAME_TYPE_RGBA16F",
  "FRAME_TYPE_RGB",
  "FRAME_TYPE_DXT1",
  "FRAME_TYPE_DXT5",
  "FRAME_TYPE_ETC2",
  "FRAME_TYPE_ETC2_EAC"
};
