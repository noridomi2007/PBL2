
#pragma once
#include "raylib.h"
#include <cstring>
#include <string>
using namespace std;

#ifndef MAX_INPUT
#define MAX_INPUT 256 // so BYTE toi da cua o nhap (chu co dau chiem 2-3 byte)
#endif

// text la UTF-8, length tinh bang BYTE
struct InputBox {
    Rectangle rect = {0, 0, 0, 0};
    char text[MAX_INPUT] = "";
    int length = 0;
    bool focused = false;
};
// FONT
inline void add_range(int *cp, int &n, int a, int b) {
    for (int c = a; c <= b; c++) {
        cp[n++] = c;
    }
}

// Goi SAU InitWindow(), nho UnloadFont() truoc CloseWindow()
inline Font load_font_utf8(const char *path, int size) {
    int cp[320]; // thuc te dung 262 phan tu
    int n = 0;
    add_range(cp, n, 32, 126);        // ASCII
    add_range(cp, n, 0xC0, 0xFF);     // Latin-1: A E I O U Y co dau
    add_range(cp, n, 0x102, 0x103);   // Ă ă
    add_range(cp, n, 0x110, 0x111);   // Đ đ
    add_range(cp, n, 0x128, 0x129);   // Ĩ ĩ
    add_range(cp, n, 0x168, 0x169);   // Ũ ũ
    add_range(cp, n, 0x1A0, 0x1A1);   // Ơ ơ
    add_range(cp, n, 0x1AF, 0x1B0);   // Ư ư
    add_range(cp, n, 0x1EA0, 0x1EF9); // Ạ ... ỹ
    add_range(cp, n, 0x20AB, 0x20AB); // ₫
    Font f = LoadFontEx(path, size, cp, n);
    SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
    return f;
}
// TIEN ICH UTF-8
// Dem so KY TU (khong phai byte)
inline int len_utf8(const char *s) {
    if (!s) {
        return 0;
    }
    int n = 0;
    for (; *s; s++) {
        if ((*s & 0xC0) != 0x80) {
            n++;
        }
    }
    return n;
}
// Chieu rong hieu chinh theo so byte du cua chu co dau
inline int width_utf8(const char *s, int width) {
    if (!s) {
        return width;
    }
    return width + ((int)strlen(s) - len_utf8(s));
}
inline bool is_ws(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}
// Cat khoang trang dau/cuoi
inline void trim(string &s) {
    size_t b = 0;
    size_t e = s.size();
    while (b < e && is_ws(s[b])) {
        b++;
    }
    while (e > b && is_ws(s[e - 1])) {
        e--;
    }
    s = s.substr(b, e - b);
}

inline void trim(char *s) {
    if (!s) {
        return;
    }
    string t(s);
    trim(t);
    strcpy(s, t.c_str());
}

// ======================================================
// BO DAU + CHU THUONG (dung cho tim kiem)
// ======================================================
// Tra ve chu ASCII goc cho chu tieng Viet, hoac 0 neu khong phai chu co dau
inline int base_letter(int c) {
    int l = (c >= 0xC0 && c <= 0xDD) ? c + 0x20 : c; // Latin-1 hoa -> thuong
    if (l >= 0xE0 && l <= 0xE5) return 'a';
    if (l >= 0xE8 && l <= 0xEB) return 'e';
    if (l >= 0xEC && l <= 0xEF) return 'i';
    if (l >= 0xF2 && l <= 0xF6) return 'o';
    if (l >= 0xF9 && l <= 0xFC) return 'u';
    if (l == 0xFD) return 'y';

    if (c == 0x102 || c == 0x103) return 'a'; // Ă ă
    if (c == 0x110 || c == 0x111) return 'd'; // Đ đ
    if (c == 0x128 || c == 0x129) return 'i'; // Ĩ ĩ
    if (c == 0x168 || c == 0x169) return 'u'; // Ũ ũ
    if (c == 0x1A0 || c == 0x1A1) return 'o'; // Ơ ơ
    if (c == 0x1AF || c == 0x1B0) return 'u'; // Ư ư

    if (c >= 0x1EA0 && c <= 0x1EB7) return 'a';
    if (c >= 0x1EB8 && c <= 0x1EC7) return 'e';
    if (c >= 0x1EC8 && c <= 0x1ECB) return 'i';
    if (c >= 0x1ECC && c <= 0x1EE3) return 'o';
    if (c >= 0x1EE4 && c <= 0x1EF1) return 'u';
    if (c >= 0x1EF2 && c <= 0x1EF9) return 'y';
    return 0;
}

// "Áo Thun Đẹp" -> "ao thun dep"
inline string normalize(const char *s) {
    string out;
    if (!s) {
        return out;
    }
    int i = 0;
    while (s[i]) {
        int bytes = 0;
        int cp = GetCodepoint(s + i, &bytes);
        if (bytes <= 0) {
            break;
        }
        if (cp >= 0x300 && cp <= 0x36F) {
            // dau roi (Unicode to hop) -> bo
        } else if (cp < 128) {
            char ch = (char)cp;
            if (ch >= 'A' && ch <= 'Z') {
                ch = (char)(ch - 'A' + 'a');
            }
            out += ch;
        } else {
            int base = base_letter(cp);
            if (base) {
                out += (char)base;
            } else {
                out.append(s + i, bytes); // ky tu khac: giu nguyen
            }
        }
        i += bytes;
    }
    return out;
}

// Khong phan biet hoa/thuong VA dau: "ao" khop "Áo thun"
inline bool contains(const char *haystack, const char *needle) {
    if (!haystack || !needle) {
        return false;
    }
    string n = normalize(needle);
    if (n.empty()) {
        return true;
    }
    return normalize(haystack).find(n) != string::npos;
}

// ======================================================
// INPUT BOX
// ======================================================
inline void delete_last_char(InputBox &in) {
    if (in.length <= 0) {
        return;
    }
    in.length--;
    while (in.length > 0 && (in.text[in.length] & 0xC0) == 0x80) {
        in.length--;
    }
    in.text[in.length] = '\0';
}

inline void delete_last_word(InputBox &in) {
    // Dau cach la ASCII nen khong bao gio cat giua ky tu UTF-8
    while (in.length > 0 && in.text[in.length - 1] == ' ') {
        in.length--;
    }
    while (in.length > 0 && in.text[in.length - 1] != ' ') {
        in.length--;
    }
    in.text[in.length] = '\0';
}

inline void clear_input(InputBox &in) {
    in.length = 0;
    in.text[0] = '\0';
}

// Noi 1 ky tu UTF-8, khong bao gio cat giua ky tu
inline bool append_utf8(InputBox &in, const char *utf8, int bytes) {
    if (bytes <= 0 || in.length + bytes >= MAX_INPUT) {
        return false;
    }
    memcpy(in.text + in.length, utf8, bytes);
    in.length += bytes;
    in.text[in.length] = '\0';
    return true;
}

inline void paste_clipboard(InputBox &in) {
    const char *clip = GetClipboardText();
    if (!clip) {
        return;
    }
    int i = 0;
    while (clip[i]) {
        int bytes = 0;
        int cp = GetCodepoint(clip + i, &bytes);
        if (bytes <= 0) {
            break;
        }
        if (cp >= 32) { // bo \n, \r, tab
            if (!append_utf8(in, clip + i, bytes)) {
                break; // het cho thi dung
            }
        }
        i += bytes;
    }
}

// Goi moi frame KHI o dang focus
inline void update_input(InputBox &in) {
    bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

    if (ctrl && IsKeyPressed(KEY_V)) {
        paste_clipboard(in);
    }

    if (ctrl && IsKeyPressed(KEY_BACKSPACE)) {
        delete_last_word(in);
    } else {
        // Dem so Backspace trong 1 frame: Unikey/EVKey gui nhieu Backspace lien tiep
        int backspace_count = 0;
        int key = GetKeyPressed();
        while (key > 0) {
            if (key == KEY_BACKSPACE) {
                backspace_count++;
            }
            key = GetKeyPressed();
        }

        for (int i = 0; i < backspace_count; i++) {
            delete_last_char(in);
        }

        // Giu phim de xoa lien tuc
        if (backspace_count == 0 && IsKeyPressedRepeat(KEY_BACKSPACE)) {
            delete_last_char(in);
        }
    }

    // Nhap chu SAU khi xoa
    int ch = GetCharPressed();
    while (ch > 0) {
        if (ch >= 32) {
            int bytes = 0;
            const char *u = CodepointToUTF8(ch, &bytes);
            append_utf8(in, u, bytes);
        }
        ch = GetCharPressed();
    }
}

// Click de focus / bo focus, roi update neu dang focus
inline void handle_input(InputBox &in) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        in.focused = CheckCollisionPointRec(GetMousePosition(), in.rect);
    }
    if (in.focused) {
        update_input(in);
    }
}

/* ======================= VI DU ==========================
#include "raylib.h"
#include "VietText.h"

int main() {
    InitWindow(800, 450, "Shop");
    SetTargetFPS(120);
    Font font = load_font_utf8("font/roboto/Roboto-Regular.ttf", 24);

    InputBox search;
    search.rect = {50, 50, 400, 44};

    while (!WindowShouldClose()) {
        handle_input(search);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleRec(search.rect, WHITE);
        DrawRectangleLinesEx(search.rect, 2, search.focused ? BLUE : GRAY);
        DrawTextEx(font, search.text, {search.rect.x + 8, search.rect.y + 10}, 24, 1, BLACK);

        bool match = contains("Áo thun nam", search.text); // go "ao thun" van ra true
        DrawTextEx(font, match ? "Khop: Ao thun nam" : "Khong khop", {50, 120}, 24, 1, DARKGRAY);
        EndDrawing();
    }
    UnloadFont(font);
    CloseWindow();
    return 0;
}
=========================================================== */