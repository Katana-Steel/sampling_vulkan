#include "vk_xcb.hpp"

xcbWindow create_default_window(uint16_t height, uint16_t width)
{
  return {height, width};
}

xcbWindow::xcbWindow(uint16_t height, uint16_t width)
{
  connect = xcb_connect(nullptr, nullptr);
  window = xcb_generate_id(connect);
  xcb_create_window(connect,
                    32,
                    window,
                    0,
                    25, 25,
                    width, height,
                    5,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    0,
                    0, nullptr);
  xcb_map_window(connect, window);
}
