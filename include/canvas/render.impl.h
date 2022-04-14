/*  canvas/render.impl.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of IncludeCanvas.
 */

#ifndef include_canvas_render_impl_h
#define include_canvas_render_impl_h

namespace canvas {
  inline void finalize() {
    if (!_ref().hWnd) {
      std::cerr << "No window." << std::endl;
    } else {
      auto hDC = GetDC(_ref().hWnd);

      if (!hDC) {
        std::cerr << "GetDC failed." << std::endl;
      } else {
        _paint_to(hDC);

        ReleaseDC(_ref().hWnd, hDC);
      }
    }
  }
}

#endif
