#pragma once

#include <tmmintrin.h>
#include <smmintrin.h>

static inline size_t compressedTexSize( unsigned int encoded_bpp, size_t width, 
                                        size_t height)
{
  switch (encoded_bpp)
  {
  case BPP_DXT1:
	return width * height / 2;
  case BPP_DXT5:
	return width * height;
  default:
	return 0;
  }
}

#define transcodeI_RGBAtoBGRA transcodeI_BGRAtoRGBA

// In-place transcoding of BGRA to RGBA. Data must be 16-byte aligned.
static inline void transcodeI_BGRAtoRGBA( void * src, size_t width, 
                                          size_t height)
{
  size_t pixels = width * height;
  __m128i* fb = src;
  __m128i mask = _mm_set_epi8(15, 12, 13, 14, 11, 8, 9, 10, 7, 
                              4, 5, 6, 3, 0, 1, 2);
  while (pixels)
  {
	__m128i pix0 = _mm_stream_load_si128(fb);
	__m128i res0 = _mm_shuffle_epi8(pix0, mask);
	_mm_store_si128(fb, res0);
	pixels -= 4;
	fb++;
  }
}