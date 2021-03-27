#include "keyboard_facade_implementation.hpp"

#include <Windows.h>

#include <algorithm>
#include <iterator>
#include <vector>

void keyboard_facade_implementation::keys_down(
    std::list<unsigned short> keys) {
  std::vector<INPUT> ips;
  ips.reserve(keys.size());
  std::transform(keys.begin(), keys.end(), std::back_inserter(ips),
                 [](auto key) {
                   INPUT ip;
                   ip.type = INPUT_KEYBOARD;
                   ip.ki.wScan = 0;  // hardware scan code for key
                   ip.ki.time = 0;
                   ip.ki.dwExtraInfo = 0;
                   ip.ki.wVk = key;    // virtual-key code for the "a" key
                   ip.ki.dwFlags = 0;  // 0 for key press
                   return ip;
                 });

  SendInput(static_cast<UINT>(ips.size()), ips.data(),
            static_cast<int>(sizeof(INPUT)));
}

void keyboard_facade_implementation::keys_up(
    std::list<unsigned short> keys) {
  std::vector<INPUT> ips;
  ips.reserve(keys.size());
  std::transform(keys.begin(), keys.end(), std::back_inserter(ips),
                 [](auto key) {
                   INPUT ip;
                   ip.type = INPUT_KEYBOARD;
                   ip.ki.wScan = 0;  // hardware scan code for key
                   ip.ki.time = 0;
                   ip.ki.dwExtraInfo = 0;
                   ip.ki.wVk = key;  // virtual-key code for the "a" key
                   ip.ki.dwFlags = KEYEVENTF_KEYUP;  // 0 for key press
                   return ip;
                 });

  SendInput(static_cast<UINT>(ips.size()), ips.data(),
            static_cast<int>(sizeof(INPUT)));
}
