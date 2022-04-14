/*  canvas/buffer.impl.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of IncludeCanvas.
 */

#ifndef include_canvas_buffer_impl_h
#define include_canvas_buffer_impl_h

namespace canvas {
  constexpr auto _check(int x0, size_t size) -> bool {
    return x0 >= 0 && static_cast<size_t>(x0) < size;
  }

  constexpr void _clamp(int &x, size_t size) {
    if (x < 0) {
      x = 0;
    } else if (static_cast<size_t>(x) >= size) {
      x = static_cast<int>(size) - 1;
    }
  }

  inline void _clamp(int &x0, int &x1, size_t size) {
    if (x1 < x0)
      std::swap(x0, x1);
    _clamp(x0, size);
    _clamp(x1, size);
  }

  inline auto buffer::_check2(int x, int y) const -> bool {
    return _check(x, static_cast<int>(m_width)) &&
           _check(y, static_cast<int>(m_height));
  }

  inline auto buffer::_index(int x, int y) const -> int {
    return y * static_cast<int>(m_width) + x;
  }

  inline auto buffer::_get_pixel(int index) const -> uint32_t {
    return m_pixels[index];
  }

  inline void buffer::_set_pixel(int index, uint32_t color) {
    m_pixels[index] = color;
  }

  inline void buffer::_put_pixel(int index, uint32_t color) {
    int alpha = alpha_of(color);

    if (alpha == 0xff) {
      _set_pixel(index, color);
    } else {
      _set_pixel(index, blend_color(_get_pixel(index), color, alpha));
    }
  }

  inline void buffer::_put_line(int x0, int x1, int y, uint32_t color) {
    int i0 = y * static_cast<int>(m_width) + x0;
    int i1 = i0 + (x1 - x0);

    for (int i = i0; i <= i1; i++) { _put_pixel(i, color); }
  }

  inline auto buffer::_clamp_relative(int x0, int y0, int x1, int y1) const
      -> std::tuple<int, int, int, int> {
    int i0 = x0, i1 = x1;
    int j0 = y0, j1 = y1;

    _clamp(i0, m_width);
    _clamp(j0, m_height);
    _clamp(i1, m_width);
    _clamp(j1, m_height);

    i0 -= x0;
    j0 -= y0;
    i1 -= x0;
    j1 -= y0;

    return { i0, j0, i1, j1 };
  }

  inline void buffer::set_size(size_t width, size_t height,
                               uint32_t default_color) {
    m_width  = width;
    m_height = height;
    m_pixels.resize(width * height, default_color);
  }

  inline void buffer::set_pixel(int x, int y, uint32_t color) {
    if (_check2(x, y)) {
      set_pixel_unsafe(x, y, color);
    }
  }

  inline void buffer::put_pixel(int x, int y, uint32_t color) {
    if (_check2(x, y)) {
      put_pixel_unsafe(x, y, color);
    }
  }

  inline auto buffer::get_pixel(int x, int y) const -> uint32_t {
    uint32_t result = 0;

    if (_check2(x, y)) {
      result = get_pixel_unsafe(x, y);
    }

    return result;
  }

  inline void buffer::set_pixel_unsafe(int x, int y, uint32_t color) {
    _set_pixel(_index(x, y), color);
  }

  inline void buffer::put_pixel_unsafe(int x, int y, uint32_t color) {
    _put_pixel(_index(x, y), color);
  }

  inline auto buffer::get_pixel_unsafe(int x, int y) const -> uint32_t {
    return _get_pixel(_index(x, y));
  }

  inline void buffer::draw_horisontal(int x0, int x1, int y,
                                      uint32_t color) {
    _clamp(x0, x1, m_width);
    _put_line(x0, x1, y, color);
  }

  inline void buffer::draw_vertical(int x, int y0, int y1,
                                    uint32_t color) {
    _clamp(y0, y1, m_height);

    for (int y = y0; y <= y1; y++) { put_pixel_unsafe(x, y, color); }
  }

  inline void buffer::draw_rectangle(int x0, int y0, int x1, int y1,
                                     uint32_t color) {
    if (_check(x0, m_width)) {
      draw_vertical(x0, y0, y1, color);
    }

    if (_check(x1, m_width)) {
      draw_vertical(x1, y0, y1, color);
    }

    if (_check(y0, m_height)) {
      draw_horisontal(x0, x1, y0, color);
    }

    if (_check(y1, m_height)) {
      draw_horisontal(x0, x1, y1, color);
    }
  }

  inline void buffer::fill_rectangle(int x0, int y0, int x1, int y1,
                                     uint32_t color) {
    _clamp(x0, x1, m_width);
    _clamp(y0, y1, m_height);

    for (int y = y0; y <= y1; y++) { _put_line(x0, x1, y, color); }
  }

  inline void buffer::draw_line(int x0, int y0, int x1, int y1,
                                uint32_t color) {
    const auto [r, g, b, a] = unpack_color(color);

    const auto dx = std::abs(x1 - x0);
    const auto dy = std::abs(y1 - y0);

    if (dx > dy) {
      if (x1 < x0) {
        std::swap(x0, x1);
        std::swap(y0, y1);
      }

      const auto sdy = static_cast<double>(y1) - y0;

      for (int x = 0; x <= dx; x++) {
        const auto fy  = (static_cast<double>(x) * sdy) / dx;
        const auto fy0 = std::floor(fy);

        const auto k0 = fy - fy0;
        const auto k1 = 1. - k0;

        const auto y = static_cast<int>(fy0);

        put_pixel(x0 + x, y0 + y + 1,
                  rgba(r, g, b, static_cast<int>(a * k0 + .5)));
        put_pixel(x0 + x, y0 + y,
                  rgba(r, g, b, static_cast<int>(a * k1 + .5)));
      }
    } else if (dy > 0) {
      if (y1 < y0) {
        std::swap(x0, x1);
        std::swap(y0, y1);
      }

      const auto sdx = static_cast<double>(x1) - x0;

      for (int y = 0; y <= dy; y++) {
        const auto fx  = (static_cast<double>(y) * sdx) / dy;
        const auto fx0 = std::floor(fx);

        const auto k0 = fx - fx0;
        const auto k1 = 1. - k0;

        const auto x = static_cast<int>(fx0);

        put_pixel(x0 + x + 1, y0 + y,
                  rgba(r, g, b, static_cast<int>(a * k0 + .5)));
        put_pixel(x0 + x, y0 + y,
                  rgba(r, g, b, static_cast<int>(a * k1 + .5)));
      }
    } else {
      put_pixel(x0, y0, color);
    }
  }

  inline void buffer::draw_line_fast(int x0, int y0, int x1, int y1,
                                     uint32_t color) {
    const int dx = std::abs(x1 - x0);
    const int dy = std::abs(y1 - y0);

    if (dx > dy) {
      if (x1 < x0) {
        std::swap(x0, x1);
        std::swap(y0, y1);
      }

      int y = y0;
      int d = dx / 2;

      if (y0 < y1) {
        for (int x = x0; x <= x1; x++) {
          put_pixel(x, y, color);

          d += dy;

          if (d >= dx) {
            y++;
            d -= dx;
          }
        }
      } else {
        for (int x = x0; x <= x1; x++) {
          put_pixel(x, y, color);

          d += dy;

          if (d >= dx) {
            y--;
            d -= dx;
          }
        }
      }
    } else {
      if (y1 < y0) {
        std::swap(x0, x1);
        std::swap(y0, y1);
      }

      int x = x0;
      int d = dy / 2;

      if (x0 < x1) {
        for (int y = y0; y <= y1; y++) {
          put_pixel(x, y, color);

          d += dx;

          if (d >= dy) {
            x++;
            d -= dy;
          }
        }
      } else {
        for (int y = y0; y <= y1; y++) {
          put_pixel(x, y, color);

          d += dx;

          if (d >= dy) {
            x--;
            d -= dy;
          }
        }
      }
    }
  }

  inline void buffer::put_pixel(double x, double y, uint32_t color) {
    const auto x0 = std::floor(x);
    const auto y0 = std::floor(y);

    const auto ix0 = static_cast<int>(x0);
    const auto iy0 = static_cast<int>(y0);

    const auto kx = x - x0;
    const auto ky = y - y0;

    const auto k00 = (1. - kx) * (1. - ky);
    const auto k01 = (1. - kx) * ky;
    const auto k10 = kx * (1. - ky);
    const auto k11 = kx * ky;

    const auto [r, g, b, a] = unpack_color(color);

    put_pixel(ix0, iy0, rgba(r, g, b, static_cast<int>(k00 * a + .5)));
    put_pixel(
        ix0, iy0 + 1, rgba(r, g, b, static_cast<int>(k01 * a + .5)));
    put_pixel(
        ix0 + 1, iy0, rgba(r, g, b, static_cast<int>(k10 * a + .5)));
    put_pixel(ix0 + 1, iy0 + 1,
              rgba(r, g, b, static_cast<int>(k11 * a + .5)));
  }

  inline void buffer::draw_line(double x0, double y0, double x1,
                                double y1, uint32_t color) {
    const auto [r, g, b, a] = unpack_color(color);

    const auto dx = std::abs(x1 - x0);
    const auto dy = std::abs(y1 - y0);

    if (dx > dy) {
      if (x1 < x0) {
        std::swap(x0, x1);
        std::swap(y0, y1);
      }

      const auto sdy = static_cast<double>(y1) - y0;

      const auto ix0 = static_cast<int>(std::floor(x0));
      const auto ix1 = static_cast<int>(std::floor(x1 + .5));
      const auto iy0 = static_cast<int>(std::floor(y0));
      const auto iy1 = static_cast<int>(std::floor(y1 + .5));
      const auto idx = ix1 - ix0;

      const auto ox0 = x0 - ix0;
      const auto ox1 = ix1 - x1;
      const auto oy0 = y0 - iy0;
      const auto oy1 = iy1 - y1;

      for (int x = 1; x < idx; x++) {
        const auto fy = oy0 +
                        ((static_cast<double>(x) - ox0) * sdy) / dx;
        const auto fy0 = std::floor(fy);

        const auto k0 = fy - fy0;
        const auto k1 = 1. - k0;

        const auto y = static_cast<int>(fy0);

        put_pixel(ix0 + x, iy0 + y + 1,
                  rgba(r, g, b, static_cast<int>(a * k0 + .5)));
        put_pixel(ix0 + x, iy0 + y,
                  rgba(r, g, b, static_cast<int>(a * k1 + .5)));
      }

      {
        const auto fy  = oy0 + (-ox0 * sdy) / dx;
        const auto fy0 = std::floor(fy);

        const auto k  = 1. - ox0;
        const auto k0 = (fy - fy0) * k;
        const auto k1 = (1. + fy0 - fy) * k;

        const auto y = static_cast<int>(fy0);

        put_pixel(ix0, iy0 + y + 1,
                  rgba(r, g, b, static_cast<int>(a * k0 + .5)));
        put_pixel(ix0, iy0 + y,
                  rgba(r, g, b, static_cast<int>(a * k1 + .5)));
      }

      {
        const auto fy = oy0 +
                        ((static_cast<double>(idx) - ox0) * sdy) / dx;
        const auto fy0 = std::floor(fy);

        const auto k  = 1. - ox1;
        const auto k0 = (fy - fy0) * k;
        const auto k1 = (1. + fy0 - fy) * k;

        const auto y = static_cast<int>(fy0);

        put_pixel(ix1, iy0 + y + 1,
                  rgba(r, g, b, static_cast<int>(a * k0 + .5)));
        put_pixel(ix1, iy0 + y,
                  rgba(r, g, b, static_cast<int>(a * k1 + .5)));
      }
    } else if (dy > 0) {
      if (y1 < y0) {
        std::swap(x0, x1);
        std::swap(y0, y1);
      }

      const auto sdx = static_cast<double>(x1) - x0;

      const auto ix0 = static_cast<int>(std::floor(x0));
      const auto ix1 = static_cast<int>(std::floor(x1 + .5));
      const auto iy0 = static_cast<int>(std::floor(y0));
      const auto iy1 = static_cast<int>(std::floor(y1 + .5));
      const auto idy = iy1 - iy0;

      const auto ox0 = x0 - ix0;
      const auto ox1 = ix1 - x1;
      const auto oy0 = y0 - iy0;
      const auto oy1 = iy1 - y1;

      for (int y = 1; y < idy; y++) {
        const auto fx = ox0 +
                        ((static_cast<double>(y) - oy0) * sdx) / dy;
        const auto fx0 = std::floor(fx);

        const auto k0 = fx - fx0;
        const auto k1 = 1. - k0;

        const auto x = static_cast<int>(fx0);

        put_pixel(ix0 + x + 1, iy0 + y,
                  rgba(r, g, b, static_cast<int>(a * k0 + .5)));
        put_pixel(ix0 + x, iy0 + y,
                  rgba(r, g, b, static_cast<int>(a * k1 + .5)));
      }

      {
        const auto fx  = ox0 + (-oy0 * sdx) / dy;
        const auto fx0 = std::floor(fx);

        const auto k  = 1. - oy0;
        const auto k0 = (fx - fx0) * k;
        const auto k1 = (1. + fx0 - fx) * k;

        const auto x = static_cast<int>(fx0);

        put_pixel(ix0 + x + 1, iy0,
                  rgba(r, g, b, static_cast<int>(a * k0 + .5)));
        put_pixel(ix0 + x, iy0,
                  rgba(r, g, b, static_cast<int>(a * k1 + .5)));
      }

      {
        const auto fx = ox0 +
                        ((static_cast<double>(idy) - oy0) * sdx) / dy;
        const auto fx0 = std::floor(fx);

        const auto k  = 1. - oy1;
        const auto k0 = (fx - fx0) * k;
        const auto k1 = (1. + fx0 - fx) * k;

        const auto x = static_cast<int>(fx0);

        put_pixel(ix0 + x + 1, iy1,
                  rgba(r, g, b, static_cast<int>(a * k0 + .5)));
        put_pixel(ix0 + x, iy1,
                  rgba(r, g, b, static_cast<int>(a * k1 + .5)));
      }
    } else {
      put_pixel(x0, y0, color);
    }
  }

  inline void buffer::draw_towards(double x0, double y0, double x1,
                                   double y1, uint32_t color) {
    double vx = x0 - x1;
    double vy = y0 - y1;

    const double v = std::sqrt(vx * vx + vy * vy);

    if (v >= 1.) {
      vx /= v;
      vy /= v;

      draw_line(x0, y0, x1 + vx, y1 + vy, color);
    }
  }

  inline void buffer::copy_sprite(int x0, int y0, size_t width,
                                  std::span<const uint32_t> pixels) {
    if (width > 0) {
      auto [i0, j0, i1, j1] = _clamp_relative(
          x0, y0, x0 + static_cast<int>(width) - 1,
          y0 + static_cast<int>(pixels.size() / width) - 1);

      auto n0 = static_cast<size_t>(j0 * width);
      auto n1 = static_cast<size_t>(j1 * width);
      auto i  = _index(x0 + i0, y0 + j0);

      for (size_t n = n0; n <= n1;
           n += width, i += static_cast<int>(m_width)) {
        std::copy(pixels.begin() + n + i0,
                  pixels.begin() + n + i1 + 1, m_pixels.begin() + i);
      }
    }
  }

  inline void buffer::copy_sprite_masked(
      int x0, int y0, size_t width, std::span<const uint32_t> pixels) {
    if (width > 0) {
      auto [i0, j0, i1, j1] = _clamp_relative(
          x0, y0, x0 + static_cast<int>(width) - 1,
          y0 + static_cast<int>(pixels.size() / width) - 1);

      auto n0 = static_cast<size_t>(j0 * width);
      auto n1 = static_cast<size_t>(j1 * width);
      auto i  = _index(x0 + i0, y0 + j0);

      for (size_t n = n0; n <= n1;
           n += width, i += static_cast<int>(m_width)) {
        for (int k = 0; k <= i1 - i0;) {
          if (alpha_of(m_pixels[static_cast<size_t>(k) + i]) == 0) {
            k++;
          } else {
            int k1 = k + 1;

            for (; k <= i1 - i0; k++) {
              if (alpha_of(m_pixels[static_cast<size_t>(i1) + i] == 0))
                break;
            }

            std::copy(pixels.begin() + n + i0 + k,
                      pixels.begin() + n + i0 + k1,
                      m_pixels.begin() + i + k);

            k = k1;
          }
        }
      }
    }
  }

  inline void buffer::draw_sprite(int x0, int y0, size_t width,
                                  std::span<const uint32_t> pixels) {
    if (width > 0) {
      auto [i0, j0, i1, j1] = _clamp_relative(
          x0, y0, x0 + static_cast<int>(width) - 1,
          y0 + static_cast<int>(pixels.size() / width) - 1);

      auto n0 = static_cast<size_t>(j0 * width);
      auto n1 = static_cast<size_t>(j1 * width);
      auto i  = _index(x0 + i0, y0 + j0);

      for (size_t n = n0; n <= n1;
           n += width, i += static_cast<int>(m_width)) {
        for (int k = 0; k < i1 - i0; k++) {
          auto c = _get_pixel(i + k);

          m_pixels[n + i0 + k] = blend_color(
              pixels[n + i0 + k], c, alpha_of(c));
        }
      }
    }
  }

  inline void buffer::stretch(const buffer &buf) {
    for (size_t j = 0; j < buf.m_height; j++) {
      for (size_t i = 0; i < buf.m_width; i++) {
        size_t x0 = (i * m_width) / buf.m_width;
        size_t y0 = (j * m_height) / buf.m_height;
        size_t x1 = ((i + 1) * m_width) / buf.m_width;
        size_t y1 = ((j + 1) * m_height) / buf.m_height;

        for (size_t y = y0; y < y1; y++) {
          for (size_t x = x0; x < x1; x++) {
            const auto i_dst = _index(
                static_cast<int>(x), static_cast<int>(y));
            const auto i_src = buf._index(
                static_cast<int>(i), static_cast<int>(j));

            m_pixels[i_dst] = buf.m_pixels[i_src];
          }
        }
      }
    }
  }

  inline auto buffer::get_width() const -> size_t {
    return m_width;
  }

  inline auto buffer::get_height() const -> size_t {
    return m_height;
  }

  inline auto buffer::get_pixels() const -> const uint32_t * {
    return m_pixels.data();
  }
}

#endif
