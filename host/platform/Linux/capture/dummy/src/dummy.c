/**
 * Looking Glass
 * Copyright © 2017-2026 The Looking Glass Authors
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

#include "interface/capture.h"
#include "common/debug.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DUMMY_WIDTH  1920
#define DUMMY_HEIGHT 1080
#define DUMMY_BPP    4
#define DUMMY_PITCH  (DUMMY_WIDTH * DUMMY_BPP)
#define DUMMY_FPS    240

struct dummy
{
  bool     stop;
  unsigned formatVer;
  uint8_t  * frameData;
};

static struct dummy * this = NULL;

static const char * dummy_getName(void)
{
  return "Dummy";
}

static bool dummy_create(
  CaptureGetPointerBuffer  getPointerBufferFn,
  CapturePostPointerBuffer postPointerBufferFn,
  unsigned                 frameBuffers)
{
  DEBUG_ASSERT(!this);
  this = calloc(1, sizeof(*this));
  if (!this)
  {
    DEBUG_ERROR("Failed to allocate dummy state");
    return false;
  }

  this->frameData = aligned_alloc(32, DUMMY_HEIGHT * DUMMY_PITCH);
  if (!this->frameData)
  {
    DEBUG_ERROR("Failed to allocate frame buffer");
    free(this);
    this = NULL;
    return false;
  }

  return true;
}

static bool dummy_init(void * ivshmemBase, unsigned * alignSize)
{
  DEBUG_ASSERT(this);
  this->stop      = false;
  this->formatVer = 0;

  *alignSize = sysconf(_SC_PAGESIZE);

  DEBUG_INFO("Dummy capture initialized (%dx%d @ %d fps)",
    DUMMY_WIDTH, DUMMY_HEIGHT, DUMMY_FPS);
  return true;
}

static void dummy_stop(void)
{
  this->stop = true;
}

static bool dummy_deinit(void)
{
  return true;
}

static void dummy_free(void)
{
  DEBUG_ASSERT(this);
  free(this->frameData);
  free(this);
  this = NULL;
}

static void dummy_renderFrame(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  const unsigned offset = (unsigned)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);

  uint8_t * pixel = this->frameData;
  for (int y = 0; y < DUMMY_HEIGHT; ++y)
  {
    for (int x = 0; x < DUMMY_WIDTH; ++x)
    {
      pixel[0] = 0;                  // B
      pixel[1] = (y + offset) % 256; // G
      pixel[2] = (x + offset) % 256; // R
      pixel[3] = 255;                // A
      pixel += DUMMY_BPP;
    }
  }
}

static CaptureResult dummy_capture(
  unsigned      frameBufferIndex,
  FrameBuffer * frame)
{
  if (this->stop)
    return CAPTURE_RESULT_REINIT;

  dummy_renderFrame();
  static struct timespec lastFrame = {0};
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  const long frameTimeNs = 1000000000L / DUMMY_FPS;
  long elapsed = (now.tv_sec - lastFrame.tv_sec) * 1000000000L +
                 (now.tv_nsec - lastFrame.tv_nsec);

  if (elapsed < frameTimeNs)
  {
    struct timespec sleep = {
      .tv_sec  = 0,
      .tv_nsec = frameTimeNs - elapsed
    };
    nanosleep(&sleep, NULL);
  }

  clock_gettime(CLOCK_MONOTONIC, &lastFrame);
  return CAPTURE_RESULT_OK;
}

static CaptureResult dummy_waitFrame(
  unsigned       frameBufferIndex,
  CaptureFrame * frame,
  const size_t   maxFrameSize)
{
  if (this->stop)
    return CAPTURE_RESULT_REINIT;

  frame->formatVer    = this->formatVer;
  frame->format       = CAPTURE_FMT_BGRA;
  frame->hdr          = false;
  frame->hdrPQ        = false;
  frame->screenWidth  = DUMMY_WIDTH;
  frame->screenHeight = DUMMY_HEIGHT;
  frame->dataWidth    = DUMMY_WIDTH;
  frame->dataHeight   = DUMMY_HEIGHT;
  frame->frameWidth   = DUMMY_WIDTH;
  frame->frameHeight  = DUMMY_HEIGHT;
  frame->truncated    = false;
  frame->pitch        = DUMMY_PITCH;
  frame->stride       = DUMMY_WIDTH;
  frame->rotation     = CAPTURE_ROT_0;

  frame->damageRectsCount = 0;

  return CAPTURE_RESULT_OK;
}

static CaptureResult dummy_getFrame(
  unsigned       frameBufferIndex,
  FrameBuffer  * frame,
  const size_t   maxFrameSize)
{
  if (this->stop)
    return CAPTURE_RESULT_REINIT;

  framebuffer_write(frame, this->frameData, DUMMY_HEIGHT * DUMMY_PITCH);
  return CAPTURE_RESULT_OK;
}

struct CaptureInterface Capture_dummy =
{
  .shortName    = "dummy",
  .asyncCapture = false,
  .getName      = dummy_getName,
  .create       = dummy_create,
  .init         = dummy_init,
  .stop         = dummy_stop,
  .deinit       = dummy_deinit,
  .free         = dummy_free,
  .capture      = dummy_capture,
  .waitFrame    = dummy_waitFrame,
  .getFrame     = dummy_getFrame
};
