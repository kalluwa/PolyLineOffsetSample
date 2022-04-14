/*  canvas/buffer.h
 *
 *      2D graphics data buffer.
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of IncludeCanvas.
 */

#ifndef include_canvas_buffer_h
#define include_canvas_buffer_h

#include "color.h"
#include <cmath>
#include <cstdint>
#include <span>
#include <vector>

namespace canvas {
  class buffer final {
  public:
    buffer() = default;

    buffer(buffer &)  = default;
    buffer(buffer &&) = default;
    auto operator=(buffer &) -> buffer & = default;
    auto operator=(buffer &&) -> buffer & = default;

    void set_size(size_t width, size_t height,
                  uint32_t default_color = 0);

    void set_pixel(int x, int y, uint32_t color);
    void put_pixel(int x, int y, uint32_t color);
    auto get_pixel(int x, int y) const -> uint32_t;

    void set_pixel_unsafe(int x, int y, uint32_t color);
    void put_pixel_unsafe(int x, int y, uint32_t color);
    auto get_pixel_unsafe(int x, int y) const -> uint32_t;

    void draw_horisontal(int x0, int x1, int y, uint32_t color);
    void draw_vertical(int x, int y0, int y1, uint32_t color);

    void draw_rectangle(int x0, int y0, int x1, int y1, uint32_t color);
    void fill_rectangle(int x0, int y0, int x1, int y1, uint32_t color);

    void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
    void draw_line_fast(int x0, int y0, int x1, int y1, uint32_t color);

    void put_pixel(double x, double y, uint32_t color);
    void draw_line(double x0, double y0, double x1, double y1,
                   uint32_t color);

    void draw_towards(double x0, double y0, double x1, double y1,
                      uint32_t color);

    void copy_sprite(int x0, int y0, size_t width,
                     std::span<const uint32_t> pixels);
    void copy_sprite_masked(int x0, int y0, size_t width,
                            std::span<const uint32_t> pixels);
    void draw_sprite(int x0, int y0, size_t width,
                     std::span<const uint32_t> pixels);

    void stretch(const buffer &buf);

    auto get_width() const -> size_t;
    auto get_height() const -> size_t;
    auto get_pixels() const -> const uint32_t *;

  private:
    auto _check2(int x, int y) const -> bool;
    auto _index(int x, int y) const -> int;
    auto _get_pixel(int index) const -> uint32_t;
    void _set_pixel(int index, uint32_t color);
    void _put_pixel(int index, uint32_t color);
    void _put_line(int x0, int x1, int y, uint32_t color);
    auto _clamp_relative(int x0, int y0, int x1, int y1) const
        -> std::tuple<int, int, int, int>;

    size_t                m_width  = 0;
    size_t                m_height = 0;
    std::vector<uint32_t> m_pixels;
  };
}

#include "buffer.impl.h"

#endif
