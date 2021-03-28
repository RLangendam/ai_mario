#pragma once

struct HWND__;

class window_handle {
 public:
  explicit window_handle(HWND__* handle);

  HWND__* get();

 private:
  HWND__* handle;
};