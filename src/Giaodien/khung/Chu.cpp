// FONT + CHỮ + UTF-8
#include "Ui.h"
#include "../../Tienich/Tiengviet.h"   // load_font_utf8 (nạp đủ chữ Việt)
#include <cmath>

namespace ui {

Fonts F;

void load_fonts() {
    const char *R = "Assets/Fonts/Roboto/Roboto-Regular.ttf";
    const char *M = "Assets/Fonts/Roboto/Roboto-Medium.ttf";
    const char *B = "Assets/Fonts/Roboto/Roboto-Bold.ttf";
    const char *S = "Assets/Fonts/Times/TIMESBD.TTF";
    auto L = [](const char *p, int co) { return load_font_utf8(p, co * FONT_SS); };
    F.r13 = L(R, 13); F.r15 = L(R, 15); F.r17 = L(R, 17);
    F.m13 = L(M, 13); F.m15 = L(M, 15); F.m17 = L(M, 17);
    F.b15 = L(B, 15); F.b17 = L(B, 17); F.b21 = L(B, 21);
    F.b27 = L(B, 27); F.b35 = L(B, 35);
    F.s22 = L(S, 22); F.s30 = L(S, 30); F.s40 = L(S, 40);
}

void unload_fonts() {
    Font *all[] = { &F.r13, &F.r15, &F.r17, &F.m13, &F.m15, &F.m17, &F.b15, &F.b17, &F.b21, &F.b27, &F.b35, &F.s22, &F.s30, &F.s40 };
    for (Font *f : all) UnloadFont(*f);
}

// ---------------- UTF-8 <-> UTF-32 ----------------
std::u32string to_u32(const std::string &s) {
    std::u32string r;
    size_t i = 0;
    while (i < s.size()) {
        unsigned char c = (unsigned char)s[i];
        char32_t cp; int n;
        if (c < 0x80) { cp = c; n = 1; }
        else if ((c >> 5) == 6) { cp = c & 0x1F; n = 2; }
        else if ((c >> 4) == 14) { cp = c & 0x0F; n = 3; }
        else if ((c >> 3) == 30) { cp = c & 0x07; n = 4; }
        else { i++; continue; }
        if (i + n > s.size()) break;
        for (int k = 1; k < n; k++) cp = (cp << 6) | ((unsigned char)s[i + k] & 0x3F);
        r.push_back(cp);
        i += n;
    }
    return r;
}

std::string to_utf8(const std::u32string &s) {
    std::string r;
    for (char32_t c : s) {
        if (c < 0x80) r += (char)c;
        else if (c < 0x800) { r += (char)(0xC0 | (c >> 6)); r += (char)(0x80 | (c & 0x3F)); }
        else if (c < 0x10000) { r += (char)(0xE0 | (c >> 12)); r += (char)(0x80 | ((c >> 6) & 0x3F)); r += (char)(0x80 | (c & 0x3F)); }
        else { r += (char)(0xF0 | (c >> 18)); r += (char)(0x80 | ((c >> 12) & 0x3F)); r += (char)(0x80 | ((c >> 6) & 0x3F)); r += (char)(0x80 | (c & 0x3F)); }
    }
    return r;
}

// Đúng các khoảng mà load_font_utf8() đã nạp: chữ không nằm trong đây sẽ hiện thành "?" nên không cho nhập
bool glyph_ok(char32_t c) {
    return (c >= 32 && c <= 126) || (c >= 0xC0 && c <= 0xFF) || (c >= 0x102 && c <= 0x103) || (c >= 0x110 && c <= 0x111) ||
           (c >= 0x128 && c <= 0x129) || (c >= 0x168 && c <= 0x169) || (c >= 0x1A0 && c <= 0x1A1) ||
           (c >= 0x1AF && c <= 0x1B0) || (c >= 0x1EA0 && c <= 0x1EF9) || c == 0x20AB;
}

std::string hoa(const std::string &s) {
    std::u32string u = to_u32(s);
    for (char32_t &c : u) {
        if (c >= 'a' && c <= 'z') c -= 32;
        else if (c >= 0xE0 && c <= 0xFE && c != 0xF7) c -= 0x20;     // à á â ã è é ê ì í ò ó ô õ ù ú ý...
        else if (c == 0x103 || c == 0x111 || c == 0x129 || c == 0x169 || c == 0x1A1 || c == 0x1B0) c -= 1;   // ă đ ĩ ũ ơ ư
        else if (c >= 0x1EA0 && c <= 0x1EF9 && (c & 1)) c -= 1;                                              // ạ ả ấ ... ỹ
    }
    return to_utf8(u);
}

// ---------------- ĐO / VẼ CHỮ ----------------
float tw(const Font &f, const std::string &s, float spacing) {
    if (s.empty()) return 0;
    return MeasureTextEx(f, s.c_str(), fsz(f), spacing).x;
}

void text(const Font &f, const std::string &s, float x, float y, Color c, float spacing) {
    if (s.empty()) return;
    DrawTextEx(f, s.c_str(), Vector2{ floorf(x), floorf(y) }, fsz(f), spacing, c);
}

static float giua_doc(const Font &f, Rectangle r) { return r.y + (r.height - fsz(f)) / 2.0f - 1.0f; }

void text_l(const Font &f, const std::string &s, Rectangle r, Color c, float padX, float spacing) {
    text(f, s, r.x + padX, giua_doc(f, r), c, spacing);
}
void text_c(const Font &f, const std::string &s, Rectangle r, Color c, float spacing) {
    float w = tw(f, s, spacing);
    text(f, s, r.x + (r.width - w) / 2.0f, giua_doc(f, r), c, spacing);
}
void text_r(const Font &f, const std::string &s, Rectangle r, Color c, float padX, float spacing) {
    float w = tw(f, s, spacing);
    text(f, s, r.x + r.width - w - padX, giua_doc(f, r), c, spacing);
}

std::string fit(const Font &f, const std::string &s, float maxw, float spacing) {
    if (tw(f, s, spacing) <= maxw) return s;
    std::u32string u = to_u32(s);
    while (!u.empty()) {
        u.pop_back();
        std::string t = to_utf8(u) + "...";
        if (tw(f, t, spacing) <= maxw) return t;
    }
    return "";
}

// Tách thành các dòng theo từ. Quá max_dong dòng thì gom phần dư vào dòng cuối rồi cắt "...".
static std::vector<std::string> tach_dong(const Font &f, const std::string &s, float maxw, int max_dong, float spacing) {
    std::vector<std::string> tu, dong;
    {
        std::string cur;
        for (char ch : s) {
            if (ch == ' ') { if (!cur.empty()) tu.push_back(cur); cur.clear(); }
            else cur += ch;
        }
        if (!cur.empty()) tu.push_back(cur);
    }
    std::string line;
    for (const std::string &w : tu) {
        std::string thu = line.empty() ? w : line + " " + w;
        if (line.empty() || tw(f, thu, spacing) <= maxw) line = thu;
        else { dong.push_back(line); line = w; }
    }
    if (!line.empty()) dong.push_back(line);
    if (max_dong > 0 && (int)dong.size() > max_dong) {
        std::string cuoi = dong[max_dong - 1];
        for (size_t k = max_dong; k < dong.size(); k++) cuoi += " " + dong[k];
        dong.resize(max_dong);
        dong[max_dong - 1] = fit(f, cuoi, maxw, spacing);
    }
    for (std::string &d : dong) if (tw(f, d, spacing) > maxw) d = fit(f, d, maxw, spacing);
    return dong;
}

int text_wrap(const Font &f, const std::string &s, float x, float y, float maxw, Color c, int max_dong, float cao_dong, float spacing) {
    std::vector<std::string> d = tach_dong(f, s, maxw, max_dong, spacing);
    for (size_t i = 0; i < d.size(); i++) text(f, d[i], x, y + i * cao_dong, c, spacing);
    return (int)d.size();
}

int dem_dong(const Font &f, const std::string &s, float maxw, int max_dong, float spacing) {
    return (int)tach_dong(f, s, maxw, max_dong, spacing).size();
}

}  // namespace ui
