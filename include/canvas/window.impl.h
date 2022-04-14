/*  canvas/window.impl.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of IncludeCanvas.
 */

#ifndef include_canvas_window_impl_h
#define include_canvas_window_impl_h

namespace canvas {
  static constexpr uint32_t _window_style = WS_CAPTION | WS_SYSMENU |
                                            WS_MINIMIZEBOX |
                                            WS_VISIBLE;

  inline auto _width() -> int {
    return static_cast<int>(get_frame().get_width());
  }

  inline auto _height() -> int {
    return static_cast<int>(get_frame().get_height());
  }

  inline void _paint_to(HDC hDC) {
    StretchDIBits(hDC, 0, 0, _width(), _height(), 0, 0, _width(),
                  _height(), get_frame().get_pixels(), &_ref().info,
                  DIB_RGB_COLORS, SRCCOPY);
  }

  inline void _on_paint(HWND hWnd) {
    PAINTSTRUCT paint;
    auto        hDC = BeginPaint(hWnd, &paint);

    if (hDC) {
      _paint_to(hDC);
      EndPaint(hWnd, &paint);
    } else {
      std::cerr << "BeginPaint failed." << std::endl;
    }
  }

  inline auto CALLBACK _window_proc(HWND hWnd, UINT message,
                                    WPARAM wParam, LPARAM lParam)
      -> LRESULT {
    LRESULT result = 0;

    switch (message) {
      case WM_PAINT: _on_paint(hWnd); break;

      case WM_CLOSE: PostQuitMessage(0); break;

      default: result = DefWindowProcW(hWnd, message, wParam, lParam);
    }

    return result;
  }

  inline auto _adjust_rect() -> std::tuple<int, int, int, int> {
    RECT r = { .left   = 0,
               .top    = 0,
               .right  = static_cast<LONG>(_ref().frame_width),
               .bottom = static_cast<LONG>(_ref().frame_height) };

    AdjustWindowRectEx(&r, _window_style, false, 0);

    int width  = r.right - r.left;
    int height = r.bottom - r.top;

    int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    return { x, y, width, height };
  }

  inline auto _register_class() -> bool {
    bool result = false;

    _ref().hInstance = GetModuleHandle(nullptr);

    if (!_ref().hInstance) {
      std::cerr << "GetModuleHandle failed." << std::endl;
    } else {
      WNDCLASSEXW wcex = { 0 };

      wcex.cbSize        = sizeof wcex;
      wcex.hInstance     = _ref().hInstance;
      wcex.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
      wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
      wcex.lpfnWndProc   = _window_proc;
      wcex.lpszClassName = _ref().class_name.c_str();

      if (!RegisterClassExW(&wcex)) {
        std::cerr << "RegisterClassExW failed." << std::endl;
      } else {
        result = true;
      }
    }

    return result;
  }

  inline void _init_buffer() {
    get_frame().set_size(_ref().frame_width, _ref().frame_height, 0);

    _ref().info.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    _ref().info.bmiHeader.biWidth       = _width();
    _ref().info.bmiHeader.biHeight      = -_height();
    _ref().info.bmiHeader.biPlanes      = 1;
    _ref().info.bmiHeader.biBitCount    = 32;
    _ref().info.bmiHeader.biCompression = BI_RGB;
    _ref().info.bmiHeader.biSizeImage   = static_cast<DWORD>(
        _ref().frame_width * _ref().frame_height * sizeof(uint32_t));
  }

  inline auto _create_window() -> bool {
    bool result = false;

    if (_ref().hWnd != nullptr) {
      std::cerr << "Window is already created." << std::endl;
    } else if (_register_class()) {
      auto [x, y, width, height] = _adjust_rect();

      _ref().hWnd = CreateWindowExW(
          0, _ref().class_name.c_str(), _ref().title.c_str(),
          _window_style, x, y, width, height, nullptr, nullptr,
          _ref().hInstance, nullptr);

      if (_ref().hWnd) {
        _init_buffer();

        result = true;
      } else {
        std::cerr << "CreateWindowExW failed." << std::endl;
      }
    }

    return result;
  }

  inline void on_render(fn_render fn) {
    _ref().on_render = fn;
  }

  inline void set_title(std::wstring_view title) {
    _ref().title = title;
  }

  inline void set_size(size_t frame_width, size_t frame_height) {
    _ref().frame_width  = frame_width;
    _ref().frame_height = frame_height;
  }

  inline auto run() -> int {
    int status = 0;

    if (_create_window()) {
      MSG msg = { 0 };

      while (!_ref().is_done) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE)) {
          if (GetMessageW(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
          } else {
            _ref().is_done = true;
          }

          if (msg.message == WM_QUIT) {
            status         = static_cast<int>(msg.wParam);
            _ref().is_done = true;
          }
        }

        if (auto fn = _ref().on_render; fn) {
          fn();
        }

        std::this_thread::yield();
      }
    }

    return status;
  }

  inline void done() {
    _ref().on_render = nullptr;
  }

  inline void quit() {
    _ref().is_done = true;
  }
}

#endif
