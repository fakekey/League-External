#include "Utils.h"
#include "Offsets.h"
#include <algorithm>

DWORD64 Mem::ReadDWORD(HANDLE hProcess, DWORD64 addr)
{
    DWORD64 ptr = NULL;
    SIZE_T bytesRead = 0;

    ReadProcessMemory(hProcess, (DWORD64*)addr, &ptr, 8, &bytesRead);

    return ptr;
}

void Mem::Read(HANDLE hProcess, DWORD64 addr, void* structure, int size)
{
    SIZE_T bytesRead = 0;

    ReadProcessMemory(hProcess, (DWORD64*)addr, structure, size, &bytesRead);
}

DWORD64 Mem::ReadDWORDFromBuffer(void* buff, int position)
{
    DWORD64 result;
    memcpy(&result, (char*)buff + position, 8);
    return result;
}

__int64 Mem::GetAIMangerAddress(DWORD64 base, HANDLE hProcess)
{
    uint8_t v3b;
    unsigned char* v1;
    unsigned __int64 v2;
    __int64 v5, v7;
    __int64 return_val;

    v1 = (unsigned __int8*)(base + Offsets::AIManagerAddr);
    v2 = 0;
    ReadProcessMemory(hProcess, (LPCVOID)(v1 + 16), &v3b, sizeof(uint8_t), NULL);
    ReadProcessMemory(hProcess, (LPCVOID)(v1 + (8 * v3b + 24)), &v7, sizeof(__int64), NULL);
    ReadProcessMemory(hProcess, (LPCVOID)(v1 + (8 * v2 + 8)), &v5, sizeof(__int64), NULL);

    v7 ^= ~v5;
    ReadProcessMemory(hProcess, (LPCVOID)(v7 + 16), &return_val, sizeof(__int64), NULL);
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