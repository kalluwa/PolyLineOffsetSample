/*  canvas.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of IncludeCanvas.
 */

#ifndef include_canvas_h
#define include_canvas_h

#include "canvas/buffer.h"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <span>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
#include <windows.h>

#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif

#ifdef near
#  undef near
#endif

#ifdef far
#  undef far
#endif

namespace canvas {
  static constexpr size_t default_frame_width  = 960;
  static constexpr size_t default_frame_height = 720;
  static constexpr auto   default_title        = L"Canvas";

  using fn_render = std::function<void()>;

  /*  Setup the render function.
   */
  void on_render(fn_render fn);

  /*  Set the window title.
   */
  void set_title(std::wstring_view title);

  /*  Set the window client frame size.
   */
  void set_size(size_t frame_width, size_t frame_height);

  /*  Run the main loop.
   */
  auto run() -> int;

  /*  Reset the render function.
   */
  void done();

  /*  Exit the main loop.
   */
  void quit();

  /*  Get the render frame buffer.
   */
  auto get_frame() -> buffer &;

  /*  Draw the canvas.
   */
  void finalize();
}

#include "canvas/data.impl.h"

#include "canvas/window.impl.h"

#include "canvas/render.impl.h"

#endif
