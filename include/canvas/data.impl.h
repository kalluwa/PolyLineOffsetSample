/*  canvas/data.impl.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of IncludeCanvas.
 */

#ifndef include_canvas_data_impl_h
#define include_canvas_data_impl_h

namespace canvas {
  struct _data {
    HINSTANCE hInstance = nullptr;
    HWND      hWnd      = nullptr;

    fn_render    on_render;
    std::wstring class_name = L"INCLUDE_CANVAS";
    std::wstring title      = default_title;

    size_t frame_width  = default_frame_width;
    size_t frame_height = default_frame_height;
    bool   is_done      = false;

    BITMAPINFO info = { 0 };
    buffer     frame;
  };

  inline auto _ref() -> _data & {
    static _data d;
    return d;
  }

  inline auto get_frame() -> buffer & {
    return _ref().frame;
  }
}

#endif
