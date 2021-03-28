#include "window_handle.hpp"

window_handle::window_handle(HWND__* handle) : handle{handle} {}

HWND__* window_handle::get() { return handle; }