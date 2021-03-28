#include "game_facade_implementation.hpp"

#include <Windows.h>
#include <strsafe.h>
#include <tlhelp32.h>

#include <thread>

#include "print_error.hpp"

namespace detail {
class process_information {
 public:
  explicit process_information(PROCESS_INFORMATION process_information)
      : info{process_information} {}

  ~process_information() {
    CloseHandle(info.hProcess);
    CloseHandle(info.hThread);
  }

  using map_type = std::unordered_map<DWORD, std::pair<HWND, DWORD>>;

  static BOOL CALLBACK each_window(_In_ HWND hwnd, _In_ LPARAM lParam) {
    map_type* map{reinterpret_cast<map_type*>(lParam)};
    DWORD process;
    auto const thread_id{GetWindowThreadProcessId(hwnd, &process)};
    map->emplace(process, std::make_pair(hwnd, thread_id));
    return TRUE;
  }

  static std::shared_ptr<window_handle> get_window_handle(
      PROCESS_INFORMATION const& info) {
    using namespace std::chrono_literals;
    std::shared_ptr<window_handle> result;
    while (!result) {
      std::this_thread::sleep_for(100ms);
      map_type association;
      EnumWindows(&process_information::each_window,
                  reinterpret_cast<LPARAM>(&association));

      auto const found{association.find(info.dwProcessId)};
      if (found != association.end()) {
        result = std::make_shared<window_handle>(found->second.first);
      }
    }
    return result;
  }

  PROCESS_INFORMATION info;
  std::shared_ptr<window_handle> handle{get_window_handle(info)};
};
class game_facade_impl {
 public:
  explicit game_facade_impl(char const* application_name, char const* rom_name)
      : application_name{application_name}, rom_name{rom_name} {}

  ~game_facade_impl() {
    auto const result{
        TerminateProcess(my_process_information.info.hProcess, 0)};
    if (!result) {
      print_error(L"TerminateProcess");
    }
  }

  static process_information startup(char const* application_name,
                                     char const* rom_name) {
    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program
    auto status = CreateProcess((LPCTSTR)application_name,  // the path
                                (LPSTR)rom_name,            // Command line
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

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5s);

    return process_information{pi};
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

  char const* application_name;
  char const* rom_name;
  process_information my_process_information{
      startup(application_name, rom_name)};
  HANDLE pHandle{my_process_information.info.hProcess};
  uintptr_t start_of_rom{read<uintptr_t>(
      get_single_module_base_address(my_process_information.info.dwProcessId) +
      0x1a92f8)};
};
}  // namespace detail

game_facade_implementation::game_facade_implementation(
    char const* application_name, char const* rom_name)
    : impl{std::make_unique<detail::game_facade_impl>(application_name,
                                                      rom_name)} {}

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

std::shared_ptr<window_handle> game_facade_implementation::get_window_handle() {
  return impl->my_process_information.handle;
}
