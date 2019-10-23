#include <xcb/xcb.h>

struct xcbWindow {
  xcb_connection_t *connect;
  xcb_window_t window;
  xcbWindow(uint16_t height, uint16_t width);
};

xcbWindow create_default_window(uint16_t h, uint16_t w);
