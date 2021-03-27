#include "game_facade_implementation.hpp"

#include <Windows.h>
#include <strsafe.h>
#include <tlhelp32.h>

namespace {
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

}  // namespace

namespace detail {
class game_facade_impl {
 public:
  explicit game_facade_impl(char const* name) : name{name} {}

  static DWORD get_pid(HWND hWnd) {
    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid;
  }
  static uintptr_t get_single_module_base_address(DWORD dwProcID) {
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

  template <typename T>
  T read(uintptr_t where) const {
    T result;

    SIZE_T bytes_read;
    ReadProcessMemory(pHandle, (LPCVOID)where, &result, sizeof(result),
                      &bytes_read);
    if (bytes_read != sizeof(result)) {
      print_error(TEXT("ReadProcessMemory"));
    }
    return result;
  }

  char const* name;
  HWND hWnd{FindWindowA(0, name)};
  DWORD pid{get_pid(hWnd)};
  HANDLE pHandle{OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid)};
  uintptr_t start_of_rom{
      read<uintptr_t>(get_single_module_base_address(pid) + 0x22B204)};
};
}  // namespace detail

game_facade_implementation::game_facade_implementation(char const* name)
    : impl{std::make_unique<detail::game_facade_impl>(name)} {}

game_facade_implementation::~game_facade_implementation() = default;

void game_facade_implementation::read_screen(screen_buffer& buffer) const {
  SIZE_T bytes_read;
  ReadProcessMemory(
      impl->pHandle,
      reinterpret_cast<LPCVOID>(impl->start_of_rom +
                                static_cast<uintptr_t>(offset::screen_begin)),
      buffer.data(), buffer.size(), &bytes_read);
  if (bytes_read != buffer.size()) {
    print_error(TEXT("ReadProcessMemory"));
  }
}

unsigned char game_facade_implementation::read_byte(offset where) const {
  return impl->read<BYTE>(impl->start_of_rom + static_cast<uintptr_t>(where));
}