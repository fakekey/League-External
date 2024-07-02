#include "Utils.h"
#include "Offsets.h"
#include <algorithm>

Chokevy* Chokevy::__instancePtr = nullptr;

Chokevy* Chokevy::GetInstance()
{
    if (__instancePtr == nullptr) {
        __instancePtr = new Chokevy();
    }

    return __instancePtr;
}

void Chokevy::CloseInstance()
{
    delete __instancePtr;
}

DWORD64 Chokevy::GetModuleBase()
{
    DWORD64 result = GetPEProcess();
    if (!result) {
        printf("Failed to get PE Process\n");
        return 0;
    }

    return Read<DWORD64>(result + 0x520);
}

Chokevy::Chokevy()
{
    hDevice = INVALID_HANDLE_VALUE;
    pid = 0;
}

void Chokevy::Destructor()
{
    if (hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(hDevice);
    }

    CloseInstance();
    printf("Press any key to exit...\n");
}

DWORD64 Chokevy::GetPEProcess()
{
    DWORD64 res;
    DWORD x;
    if (DeviceIoControl(hDevice, IOCTL_CE_GETPEPROCESS, &pid, 4, &res, 8, &x, nullptr)) {
        return res;
    }

    return 0;
}

bool Chokevy::AdjustPrivilege(std::string_view privilegeName)
{
    HANDLE hToken = NULL;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false;
    }

    LUID luid {};

    if (!LookupPrivilegeValueA(nullptr, privilegeName.data(), &luid)) {
        return false;
    }

    TOKEN_PRIVILEGES state {};

    state.PrivilegeCount = 1;
    state.Privileges[0].Luid = luid;
    state.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, FALSE, &state, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr)) {
        return false;
    }

    CloseHandle(hToken);

    return true;
}

bool Chokevy::GetProcessId()
{
    HWND wnd = FindWindowW(NULL, L"League of Legends (TM) Client");
    if (wnd == NULL) {
        return false;
    }

    GetWindowThreadProcessId(wnd, &pid);
    if (!pid || pid == 4) {
        return false;
    }

    return true;
}

bool Chokevy::IsWindowActive()
{
    HWND wnd = GetForegroundWindow();
    if (wnd == NULL) {
        return false;
    }

    DWORD h;
    GetWindowThreadProcessId(wnd, &h);
    return pid == h;
}

bool Chokevy::InitializeDriver()
{
    hDevice = CreateFileW(L"\\\\.\\chokevy", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("Can't open driver\n");
        return false;
    }

    return true;
}

bool Chokevy::ReadProcessMemory64_Internal(DWORD64 lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead)
{
    __Request req;
    DWORD x;
    DWORD64 mempointer;
    DWORD64 bufpointer;
    DWORD toread;

    *lpNumberOfBytesRead = 0;
    mempointer = lpBaseAddress;
    bufpointer = (DWORD64)lpBuffer;

    while (true) {
        if ((mempointer & 0xfff) > 0) {
            toread = 4096 - (mempointer & 0xfff);
            if (toread > (nSize - *lpNumberOfBytesRead)) {
                toread = nSize - *lpNumberOfBytesRead;
            }
        } else {
            if (nSize - *lpNumberOfBytesRead >= 4096) {
                toread = 4096;
            } else {
                toread = nSize - *lpNumberOfBytesRead;
            }
        }

        req.processid = pid;
        req.bytestoread = toread;
        req.startaddress = mempointer;

        if (!DeviceIoControl(hDevice, IOCTL_CE_READMEMORY, &req, sizeof(req), (LPVOID)bufpointer, toread, &x, nullptr)) {
            return false;
        }

        mempointer += toread;
        bufpointer += toread;
        *lpNumberOfBytesRead = *lpNumberOfBytesRead + toread;

        if (*lpNumberOfBytesRead == nSize) {
            return true;
        }
    }
}

bool Chokevy::ReadProcessMemory64(DWORD64 lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead)
{
    return ReadProcessMemory64_Internal(lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
}

bool Chokevy::ReadProcessMemory(LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead)
{
    return ReadProcessMemory64((DWORD64)lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
}

template <typename T>
T Chokevy::Read(DWORD64 addr)
{
    T res = NULL;
    SIZE_T x = 0;
    if (ReadProcessMemory((LPCVOID)addr, &res, sizeof(T), &x)) {
        return res;
    }

    return NULL;
}

DWORD64 Mem::ReadDWORD(DWORD64 addr)
{
    DWORD64 res = NULL;
    SIZE_T x = 0;
    if (Chokevy::GetInstance()->ReadProcessMemory((LPCVOID)addr, &res, 8, &x)) {
        return res;
    }

    return NULL;
}

void Mem::Read(DWORD64 addr, void* structure, int size)
{
    SIZE_T x = 0;
    Chokevy::GetInstance()->ReadProcessMemory((LPCVOID)addr, structure, size, &x);
}

DWORD64 Mem::ReadDWORDFromBuffer(void* buff, int position)
{
    DWORD64 result;
    memcpy(&result, (char*)buff + position, 8);
    return result;
}

__int64 Mem::GetAIManagerAddress(DWORD64 base, DWORD64 custom)
{
    uint8_t v3b;
    unsigned char* v1;
    unsigned __int64 v2;
    __int64 v5, v7;
    __int64 return_val;
    SIZE_T x = 0;

    v1 = (unsigned __int8*)(base + (Offsets::AIManagerAddr + custom));
    v2 = 0;
    Chokevy::GetInstance()->ReadProcessMemory((LPCVOID)(v1 + 16), &v3b, sizeof(uint8_t), &x);
    Chokevy::GetInstance()->ReadProcessMemory((LPCVOID)(v1 + (8 * v3b + 24)), &v7, sizeof(__int64), &x);
    Chokevy::GetInstance()->ReadProcessMemory((LPCVOID)(v1 + (8 * v2 + 8)), &v5, sizeof(__int64), &x);

    v7 ^= ~v5;
    Chokevy::GetInstance()->ReadProcessMemory((LPCVOID)(v7 + 16), &return_val, sizeof(__int64), &x);
    return return_val;
}

bool Character::ContainsOnlyASCII(const char* buff, int maxSize)
{
    for (int i = 0; i < maxSize; ++i) {
        if (buff[i] == 0)
            return true;
        if ((unsigned char)buff[i] > 127)
            return false;
    }
    return true;
}

std::string Character::ToLower(std::string str)
{
    std::string strLower;
    strLower.resize(str.size());

    std::transform(str.begin(),
        str.end(),
        strLower.begin(),
        ::tolower);

    return strLower;
}

std::string Character::RandomString(const int len)
{
    std::string tmp_s;
    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "!@#$%^&*"
                                   "abcdefghijklmnopqrstuvwxyz";

    srand((unsigned int)time(0));
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];

    return tmp_s;
}

std::string Character::Format(const char* c, const char* args...)
{
    char buff[200];
    sprintf_s(buff, c, args);

    return std::string(buff);
}

BOOL LeagueProcess::IsProcessRunning(DWORD64 pid)
{
    HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
    DWORD64 ret = WaitForSingleObject(process, 0);
    CloseHandle(process);
    return ret == WAIT_TIMEOUT;
}