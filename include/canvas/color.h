/*  canvas/color.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of IncludeCanvas.
 */

#ifndef include_canvas_color_h
#define include_canvas_color_h

#include <algorithm>
#include <cstdint>
#include <tuple>

namespace canvas {
  constexpr auto red_of(uint32_t color) -> int;
  constexpr auto green_of(uint32_t color) -> int;
  constexpr auto blue_of(uint32_t color) -> int;
  constexpr auto alpha_of(uint32_t color) -> int;

  constexpr auto rgb(uint8_t red, uint8_t green, uint8_t blue)
      -> uint32_t;
  constexpr auto rgba(uint8_t red, uint8_t green, uint8_t blue,
                      uint8_t alpha) -> uint32_t;

  constexpr auto rgb(int red, int green, int blue) -> uint32_t;
  constexpr auto rgba(int red, int green, int blue, int alpha)
      -> uint32_t;

  constexpr auto unpack_color(uint32_t color)
      -> std::tuple<int, int, int, int>;
  constexpr auto blend_color(uint32_t begin, uint32_t end,
                             uint8_t factor) -> uint32_t;
}

#include "color.impl.h"

#endif
