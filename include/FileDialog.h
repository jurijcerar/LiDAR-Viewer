#pragma once
// Native file-open dialog.
// Windows (MSYS2/MinGW): uses Win32 GetOpenFileName — no extra deps.
// Linux: uses zenity or kdialog if available, otherwise returns empty.

#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#endif

namespace FileDialog {

inline bool isAvailable() {
#ifdef _WIN32
    return true;
#else
    FILE* f = popen("which zenity 2>/dev/null || which kdialog 2>/dev/null", "r");
    if (!f) return false;
    char buf[8];
    bool found = (fgets(buf, sizeof(buf), f) != nullptr);
    pclose(f);
    return found;
#endif
}

inline std::string openFile(const char* title = "Open LAS file",
                             const char* /*filter*/ = "*.las")
{
#ifdef _WIN32
    char szFile[512] = {};
    OPENFILENAMEA ofn    = {};
    ofn.lStructSize      = sizeof(ofn);
    ofn.hwndOwner        = nullptr;
    ofn.lpstrFilter      = "LAS Files\0*.las;*.LAS\0All Files\0*.*\0\0";
    ofn.lpstrFile        = szFile;
    ofn.nMaxFile         = sizeof(szFile);
    ofn.lpstrTitle       = title;
    ofn.Flags            = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&ofn))
        return std::string(szFile);
    return {};
#else
    std::string cmd;
    FILE* check = popen("which zenity 2>/dev/null", "r");
    char buf[8] = {};
    bool hasZenity = check && fgets(buf, sizeof(buf), check);
    if (check) pclose(check);

    if (hasZenity) {
        cmd = std::string("zenity --file-selection --title='") + title +
              "' --file-filter=*.las 2>/dev/null";
    } else {
        FILE* check2 = popen("which kdialog 2>/dev/null", "r");
        char buf2[8] = {};
        bool hasKd = check2 && fgets(buf2, sizeof(buf2), check2);
        if (check2) pclose(check2);
        if (!hasKd) return {};
        cmd = std::string("kdialog --getopenfilename . '*.las' --title '") + title + "' 2>/dev/null";
    }

    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return {};
    char rbuf[4096];
    while (fgets(rbuf, sizeof(rbuf), pipe))
        result += rbuf;
    pclose(pipe);
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
        result.pop_back();
    return result;
#endif
}

} // namespace FileDialog
