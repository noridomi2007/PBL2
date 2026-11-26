// file_dialog.cpp  (KHÔNG include raylib ở file này)
#include "Dialog.h"
#include <windows.h>
#include <commdlg.h>

static std::string wide_sang_utf8(const std::wstring &w) {
    if (w.empty()) return "";
    int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string out(len - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, &out[0], len, nullptr, nullptr);
    return out;
}

std::vector<std::string> mo_hop_thoai_chon_anh(int toi_da) {
    std::vector<std::string> kq;
    if (toi_da <= 0) return kq;

    static wchar_t buf[65536];   // chọn nhiều file thì chuỗi kết quả rất dài
    buf[0] = L'\0';

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = GetActiveWindow();
    ofn.lpstrFilter = L"Anh (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0";
    ofn.lpstrFile   = buf;
    ofn.nMaxFile    = sizeof(buf) / sizeof(buf[0]);
    ofn.lpstrTitle  = L"Chon anh san pham (giu Ctrl de chon nhieu anh)";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR |
                OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (!GetOpenFileNameW(&ofn)) return kq;

    // Kết quả: chọn 1 file -> "đường dẫn đầy đủ\0\0"
    //          chọn nhiều  -> "thư mục\0file1\0file2\0\0"
    std::wstring dau = buf;
    const wchar_t *p = buf + dau.size() + 1;

    if (*p == L'\0') {
        kq.push_back(wide_sang_utf8(dau));
    } else {
        if (dau.back() != L'\\') dau += L'\\';
        while (*p && (int)kq.size() < toi_da) {
            kq.push_back(wide_sang_utf8(dau + p));
            p += wcslen(p) + 1;
        }
    }
    if ((int)kq.size() > toi_da) kq.resize(toi_da);
    return kq;
}