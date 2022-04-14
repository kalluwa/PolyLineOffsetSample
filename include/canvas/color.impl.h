/*  canvas/color.impl.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of IncludeCanvas.
 */

#ifndef include_canvas_color_impl_h
#define include_canvas_color_impl_h

namespace canvas {
  constexpr auto red_of(uint32_t color) -> int {
    return (color >> 16) & 0xff;
  }

  constexpr auto green_of(uint32_t color) -> int {
    return (color >> 8) & 0xff;
  }

  constexpr auto blue_of(uint32_t color) -> int {
    return color & 0xff;
  }

  constexpr auto alpha_of(uint32_t color) -> int {
    return (color >> 24) & 0xff;
  }

  constexpr auto rgb(uint8_t red, uint8_t green, uint8_t blue)
      -> uint32_t {
    return blue | (green << 8) | (red << 16) | 0xff000000;
  }

  constexpr auto rgba(uint8_t red, uint8_t green, uint8_t blue,
                      uint8_t alpha) -> uint32_t {
    return blue | (green << 8) | (red << 16) | (alpha << 24);
  }

  constexpr auto rgb(int red, int green, int blue) -> uint32_t {
    return rgb(static_cast<uint8_t>(std::max(0, std::min(255, red))),
               static_cast<uint8_t>(std::max(0, std::min(255, green))),
               static_cast<uint8_t>(std::max(0, std::min(255, blue))));
  }

  constexpr auto rgba(int red, int green, int blue, int alpha)
      -> uint32_t {
    return rgba(
        static_cast<uint8_t>(std::max(0, std::min(255, red))),
        static_cast<uint8_t>(std::max(0, std::min(255, green))),
        static_cast<uint8_t>(std::max(0, std::min(255, blue))),
        static_cast<uint8_t>(std::max(0, std::min(255, alpha))));
  }

  constexpr auto unpack_color(uint32_t color)
      -> std::tuple<int, int, int, int> {
    return { red_of(color), green_of(color), blue_of(color),
             alpha_of(color) };
  }

  constexpr auto blend_color(uint32_t begin, uint32_t end,
                             uint8_t factor) -> uint32_t {
    auto f0 = 0xff - factor;

    auto r = std::max(
        0, std::min((red_of(begin) * f0 + red_of(end) * factor) / 0xff,
                    0xff));
    auto g = std::max(
        0, std::min(
               (green_of(begin) * f0 + green_of(end) * factor) / 0xff,
               0xff));
    auto b = std::max(
        0, std::min((blue_of(begin) * f0 + blue_of(end) * factor) / 0xff,
                    0xff));

    return rgb(r, g, b);
  }
}

namespace canvas::color {
  constexpr auto transparent = rgba(0, 0, 0, 0);

  constexpr auto white      = rgb(255, 255, 255);
  constexpr auto black      = rgb(0, 0, 0);
  constexpr auto gray       = rgb(127, 127, 127);
  constexpr auto red        = rgb(255, 0, 0);
  constexpr auto green      = rgb(0, 255, 0);
  constexpr auto blue       = rgb(0, 0, 255);
  constexpr auto yellow     = rgb(255, 255, 0);
  constexpr auto cyan       = rgb(0, 255, 255);
  constexpr auto purple     = rgb(255, 0, 255);
  constexpr auto orange     = rgb(255, 127, 0);
  constexpr auto pink       = rgb(255, 0, 127);
  constexpr auto dark_red   = rgb(127, 0, 0);
  constexpr auto dark_green = rgb(0, 127, 0);
  constexpr auto dark_blue  = rgb(0, 0, 127);
}

#endif
