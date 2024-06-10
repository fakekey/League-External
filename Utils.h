#pragma once
#include "imgui.h"
#include "windows.h"
#include <iostream>
#include <string>

constexpr DWORD FILE_RW_ACCESS = FILE_READ_ACCESS | FILE_WRITE_ACCESS;
constexpr DWORD IOCTL_UNKNOWN_BASE = FILE_DEVICE_UNKNOWN;
constexpr DWORD IOCTL_CE_READMEMORY = (IOCTL_UNKNOWN_BASE << 16) | (0x0800 << 2) | (METHOD_BUFFERED) | (FILE_RW_ACCESS << 14);
constexpr DWORD IOCTL_CE_GETPEPROCESS = (IOCTL_UNKNOWN_BASE << 16) | (0x0805 << 2) | (METHOD_BUFFERED) | (FILE_RW_ACCESS << 14);

class Chokevy {
public:
    Chokevy();
    void Destructor();
    bool InitializeDriver();
    bool GetProcessId();
    DWORD64 GetModuleBase();
    bool ReadProcessMemory(LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);
    bool IsWindowActive();

    template <typename T>
    T Read(DWORD64 addr);

private:
    struct __Request {
        UINT64 processid;
        UINT64 startaddress;
        WORD bytestoread;
    };

private:
    HANDLE hDevice;
    DWORD pid;
    DWORD64 GetPEProcess();
    bool ReadProcessMemory64(DWORD64 lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);
    bool ReadProcessMemory64_Internal(DWORD64 lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);

private:
    static Chokevy* __instancePtr;

public:
    static bool AdjustPrivilege(std::string_view privilegeName);
    static Chokevy* GetInstance();
    static void CloseInstance();
};

class WinApiException : public std::runtime_error {

public:
    WinApiException(const char* message)
        : std::runtime_error(message)
    {

        errorCode = GetLastError();
        this->message = message;
    }

    std::string GetErrorMessage()
    {
        std::string msg = std::string(message);

        if (errorCode > 0) {
            char winapiError[255];
            FormatMessageA(
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                winapiError,
                (sizeof(winapiError) / sizeof(wchar_t)),
                NULL);

            msg.append("(");
            msg.append(winapiError);
            msg.append(")");
        }

        return msg;
    }

private:
    const char* message = nullptr;
    int errorCode = 0;
};

/// Utilities used for realing process memory
namespace Mem {

/// Reads a DWORD64 at the specified memory location
DWORD64 ReadDWORD(DWORD64 addr);

/// Reads an arbitrary struct at the specified memory location
void Read(DWORD64 addr, void* structure, int size);

/// Reads a DWORD64 at the specified location in a given buffer
DWORD64 ReadDWORDFromBuffer(void* buff, int position);

/// Reads a DWORD64 of AIMangerAddress
__int64 GetAIMangerAddress(DWORD64 base);
};

namespace Character {
bool ContainsOnlyASCII(const char* buff, int maxSize);
std::string ToLower(std::string str);
std::string RandomString(const int len);
std::string Format(const char* c, const char* args...);
}

namespace LeagueProcess {
BOOL IsProcessRunning(DWORD64 pid);
};

/// Some basic colors
namespace Colors {
static const ImVec4 BLACK = ImVec4(0.06f, 0.06f, 0.06f, 1.f);
static const ImVec4 WHITE = ImVec4(1.f, 1.f, 1.f, 1.f);
static const ImVec4 RED = ImVec4(1.f, 0.f, 0.f, 1.f);
static const ImVec4 DARK_RED = ImVec4(0.6f, 0.f, 0.f, 1.f);
static const ImVec4 GREEN = ImVec4(0.f, 1.f, 0.f, 1.f);
static const ImVec4 DARK_GREEN = ImVec4(0.f, 0.6f, 0.f, 1.f);
static const ImVec4 YELLOW = ImVec4(1.f, 1.f, 0.f, 1.f);
static const ImVec4 DARK_YELLOW = ImVec4(0.6f, 0.6f, 0.f, 1.f);
static const ImVec4 CYAN = ImVec4(0.f, 1.f, 1.f, 1.f);
static const ImVec4 PURPLE = ImVec4(1.f, 0.f, 1.f, 1.f);
static const ImVec4 GRAY = ImVec4(0.5f, 0.5f, 0.5f, 1.f);
static const ImVec4 ORANGE = ImVec4(1.f, 0.54f, 0.f, 1.f);
static const ImVec4 BLUE = ImVec4(0.f, 0.f, 1.f, 1.f);
static const ImVec4 BROWN = ImVec4(0.54f, 0.27f, 0.06f, 1.f);
}