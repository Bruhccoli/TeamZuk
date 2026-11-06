
#include <windows.h>
#include <shellapi.h>
#include <string>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Advapi32.lib")

// Steam 설치 경로 검색
std::wstring FindSteam()
{
    const wchar_t* keys[] = {
        L"Software\\Valve\\Steam",
        L"Software\\WOW6432Node\\Valve\\Steam"
    };

    HKEY roots[] = {
        HKEY_CURRENT_USER,
        HKEY_LOCAL_MACHINE
    };

    wchar_t buffer[32768];

    for (HKEY root : roots)
    {
        for (const auto& key : keys)
        {
            DWORD size = sizeof(buffer);

            // SteamExe 값 검색
            if (RegGetValueW(
                root, key, L"SteamExe",
                RRF_RT_REG_SZ, nullptr,
                buffer, &size) == ERROR_SUCCESS)
            {
                return buffer;
            }

            size = sizeof(buffer);

            // InstallPath 값 검색
            if (RegGetValueW(
                root, key, L"InstallPath",
                RRF_RT_REG_SZ, nullptr,
                buffer, &size) == ERROR_SUCCESS)
            {
                std::wstring path = buffer;
                path += L"\\steam.exe";

                if (GetFileAttributesW(path.c_str())
                    != INVALID_FILE_ATTRIBUTES)
                {
                    return path;
                }
            }
        }
    }

    return L"";
}

// Windows GUI 프로그램 진입점
int WINAPI WinMain(
    HINSTANCE,
    HINSTANCE,
    LPSTR,
    int)
{
    std::wstring steam = FindSteam();

    // Steam을 찾지 못한 경우
    if (steam.empty())
    {
        MessageBoxW(
            nullptr,
            L"Steam 설치 경로를 찾을 수 없습니다.",
            L"TF2 Quick Connect",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    // TF2 실행 및 서버 접속
    HINSTANCE result = ShellExecuteW(
        nullptr,
        L"open",
        steam.c_str(),
        L"-applaunch 440 +connect gaez.uk:37015",
        nullptr,
        SW_SHOWNORMAL
    );

    // 실행 실패 확인
    if ((INT_PTR)result <= 32)
    {
        MessageBoxW(
            nullptr,
            L"Steam 실행에 실패했습니다.",
            L"TF2 Quick Connect",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    return 0;
}
