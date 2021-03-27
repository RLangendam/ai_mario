#include "game_facade_implementation.hpp"

#include <Windows.h>
#include <strsafe.h>
#include <tlhelp32.h>

#include "print_error.hpp"

namespace detail {
class game_facade_impl {
 public:
  explicit game_facade_impl(char const* name) : name{name} {}

  static void startup(char* application_name, char* rom_name) {
     
    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program
    auto status = CreateProcess((LPCTSTR)application_name,  // the path
                                (LPSTR)rom_name,          // Command line
                                NULL,   // Process handle not inheritable
                                NULL,   // Thread handle not inheritable
                                FALSE,  // Set handle inheritance to FALSE
                                0,      // No creation flags
                                NULL,   // Use parent's environment block
                                NULL,   // Use parent's starting directory
                                &si,    // Pointer to STARTUPINFO structure
                                &pi  // Pointer to PROCESS_INFORMATION structure
                                     // (removed extra parentheses)
    );

    if (status == FALSE) {
      print_error(L"ReadProcessMemory");
    }

    // todo: move to destructor
    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  }

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
      print_error(L"CreateToolhelp32Snapshot");
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
      print_error(L"ReadProcessMemory");
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
    print_error(L"ReadProcessMemory");
  }
}

unsigned char game_facade_implementation::read_byte(offset where) const {
  return impl->read<BYTE>(impl->start_of_rom + static_cast<uintptr_t>(where));
}