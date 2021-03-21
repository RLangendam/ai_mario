#include <Windows.h>
#include <strsafe.h>
#include <tlhelp32.h>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <unordered_map>

class keyboard_control {
 public:
  void press(std::list<WORD> keys) {
    using namespace std::chrono_literals;
    std::list<WORD> keys_to_press;
    for (auto key : keys) {
      auto found{presses.find(key)};
      if (found == presses.end()) {
        keys_to_press.emplace_back(key);
        presses.emplace(key, std::chrono::steady_clock::now() + 100ms);
      } else {
        found->second = std::chrono::steady_clock::now() + 100ms;
      }
    }
    keys_down(keys_to_press);
  }

  void update() {
    auto const now{std::chrono::steady_clock::now()};
    std::list<WORD> keys_to_release;
    for (auto& [key, time_point] : presses) {
      if (now > time_point) {
        keys_to_release.emplace_back(key);
      }
    }
    keys_up(keys_to_release);
    for (auto key : keys_to_release) {
      presses.erase(key);
    }
  }

 private:
  void keys_down(std::list<WORD> keys) {
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

    SendInput(ips.size(), ips.data(), sizeof(INPUT));
  }

  void keys_up(std::list<WORD> keys) {
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

    SendInput(ips.size(), ips.data(), sizeof(INPUT));
  }

  std::unordered_map<WORD, std::chrono::time_point<std::chrono::steady_clock>>
      presses;
};

void print_error(LPCTSTR lpszFunction) {
  LPVOID lpMsgBuf;
  LPVOID lpDisplayBuf;
  DWORD dw = GetLastError();

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpMsgBuf, 0, NULL);

  lpDisplayBuf = (LPVOID)LocalAlloc(
      LMEM_ZEROINIT,
      (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) *
          sizeof(TCHAR));
  StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                  TEXT("%s failed with error %d: %s"), lpszFunction, dw,
                  lpMsgBuf);
  MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

  LocalFree(lpMsgBuf);
  LocalFree(lpDisplayBuf);
}

uintptr_t GetSingleModuleBaseAddress(DWORD dwProcID) {
  uintptr_t ModuleBaseAddress = 0;
  HANDLE hSnapshot = CreateToolhelp32Snapshot(
      TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
  if (hSnapshot == INVALID_HANDLE_VALUE) {
    print_error(TEXT("CreateToolhelp32Snapshot"));
  } else {
    MODULEENTRY32 ModuleEntry32;
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(hSnapshot, &ModuleEntry32)) {
      ModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
    }
    CloseHandle(hSnapshot);
  }
  return ModuleBaseAddress;
}

enum class offsets : uintptr_t {
  score_ten_thousands = 0x9821,
  score_thousands = 0x9822,
  score_hundreds = 0x9823,
  score_tens = 0x9824,
  score_ones = 0x9825,
  coins_tens = 0x9829,
  coins_ones = 0x982A,
  timer_hundreds = 0x9831,
  timer_tens = 0x9832,
  timer_ones = 0x9833,
  game_over = 0xC0A4,
  lives = 0xDA15
};

class game {
 public:
  explicit game(char const* name) : name{name} {}

  void run() {
    std::cout << "Score: " << get_score() << " Coins: " << get_coins()
              << " Timer: " << get_timer() << " Game Over: " << std::boolalpha
              << is_game_over() << " Lives: " << get_lives() << "\n";
  }

  std::uint16_t get_score() const {
    std::uint16_t result{0};
    result += 10000 * read_decimal_position(offsets::score_ten_thousands);
    result += 1000 * read_decimal_position(offsets::score_thousands);
    result += 100 * read_decimal_position(offsets::score_hundreds);
    result += 10 * read_decimal_position(offsets::score_tens);
    result += read_decimal_position(offsets::score_ones);
    return result;
  }

  std::uint16_t get_coins() const {
    std::uint16_t result{0};
    result += 10 * read_decimal_position(offsets::coins_tens);
    result += read_decimal_position(offsets::coins_ones);
    return result;
  }

  std::uint16_t get_timer() const {
    std::uint16_t result{0};
    result += 100 * read_decimal_position(offsets::timer_hundreds);
    result += 10 * read_decimal_position(offsets::timer_tens);
    result += read_decimal_position(offsets::timer_ones);
    return result;
  }

  bool is_game_over() const {
    return 0x39 == read<BYTE>(start_of_rom +
                              static_cast<uintptr_t>(offsets::game_over));
  }

  std::uint16_t get_lives() const {
    return read<BYTE>(start_of_rom + static_cast<uintptr_t>(offsets::lives));
  }

  void write_score(std::uint16_t score) {
    write<BYTE>(start_of_rom + static_cast<uintptr_t>(offsets::score_ones),
                static_cast<BYTE>(score % 10));
    write<BYTE>(start_of_rom + static_cast<uintptr_t>(offsets::score_tens),
                static_cast<BYTE>((score / 10) % 10));
    write<BYTE>(start_of_rom + static_cast<uintptr_t>(offsets::score_hundreds),
                static_cast<BYTE>((score / 100) % 10));
    write<BYTE>(start_of_rom + static_cast<uintptr_t>(offsets::score_thousands),
                static_cast<BYTE>((score / 1000) % 10));
    write<BYTE>(
        start_of_rom + static_cast<uintptr_t>(offsets::score_ten_thousands),
        static_cast<BYTE>(score / 10000));
  }

 private:
  std::uint16_t read_decimal_position(offsets offset) const {
    BYTE result = read<BYTE>(start_of_rom + static_cast<uintptr_t>(offset));
    if (result == 44) {
      result = 0;
    }
    return result;
  }

  static DWORD get_pid(HWND hWnd) {
    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid;
  }

  template <typename T>
  T read(uintptr_t where) const {
    T result;

    SIZE_T bytes_read;
    ReadProcessMemory(pHandle, (LPVOID)where, &result, sizeof(result),
                      &bytes_read);
    if (bytes_read != sizeof(result)) {
      print_error(TEXT("ReadProcessMemory"));
    }
    return result;
  }

  template <typename T>
  void write(uintptr_t where, T what) {
    SIZE_T bytes_written;
    WriteProcessMemory(pHandle, (LPVOID)where, &what, sizeof(what),
                       &bytes_written);
    if (bytes_written != sizeof(what)) {
      print_error(TEXT("WriteProcessMemory"));
    }
  }

  char const* name;
  HWND hWnd{FindWindowA(0, name)};
  DWORD pid{get_pid(hWnd)};
  HANDLE pHandle{OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid)};
  uintptr_t start_of_rom{
      read<uintptr_t>(GetSingleModuleBaseAddress(pid) + 0x22B204)};
};

int main() {
  using namespace std::chrono_literals;

  game g{"VisualBoyAdvance"};
  g.run();
  std::this_thread::sleep_for(5s);

  keyboard_control keyboard;
  keyboard.press({'Z'});
  while (true) {
    keyboard.update();
    std::this_thread::sleep_for(1ms);
  }
  return 0;
}