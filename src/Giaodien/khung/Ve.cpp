// VẼ CƠ BẢN + WIDGET + ICON + ẢNH + THÔNG BÁO
#include "Ui.h"
#include "../../Item/Item.h"                 // duong_dan_anh, dem_anh
#include "../../Tienich/Tiengviet.h"
#include "../../Tienich/Tienich.h"
#include <cmath>
#include <map>
#include <unordered_map>

namespace ui {

// ============================ KHUNG / CẮT VÙNG ============================
static std::vector<Rectangle> clip_stack;

bool hover(Rectangle r) {
    if (C.blocked) return false;
    if (!CheckCollisionPointRec(C.mouse, r)) return false;
    if (!clip_stack.empty() && !CheckCollisionPointRec(C.mouse, clip_stack.back())) return false;
    return true;
}

static void ap_dung_clip(Rectangle r) {
    if (r.width < 0) r.width = 0;
    if (r.height < 0) r.height = 0;
    // scissor của raylib tính theo PIXEL THẬT (không đi qua rlScalef) nên phải nhân tỉ lệ thủ công
    float k = C.scale;
    int x0 = (int)floorf(r.x * k), y0 = (int)floorf(r.y * k);
    int x1 = (int)ceilf((r.x + r.width) * k), y1 = (int)ceilf((r.y + r.height) * k);
    BeginScissorMode(x0, y0, x1 - x0, y1 - y0);
}

void push_clip(Rectangle r) {
    if (!clip_stack.empty()) {            // giao với vùng cắt hiện tại để vùng con không tràn ra ngoài vùng cha
        Rectangle p = clip_stack.back();
        float x1 = fmaxf(r.x, p.x), y1 = fmaxf(r.y, p.y);
        float x2 = fminf(r.x + r.width, p.x + p.width), y2 = fminf(r.y + r.height, p.y + p.height);
        r = Rectangle{ x1, y1, fmaxf(0, x2 - x1), fmaxf(0, y2 - y1) };
    }
    clip_stack.push_back(r);
    ap_dung_clip(r);
}

void pop_clip() {
    if (clip_stack.empty()) return;
    clip_stack.pop_back();
    if (clip_stack.empty()) EndScissorMode();
    else ap_dung_clip(clip_stack.back());
}

// ============================ HÌNH CƠ BẢN ============================
void rrect(Rectangle r, float radius, Color c) {
    if (r.width <= 0 || r.height <= 0) return;
    float m = fminf(r.width, r.height) / 2.0f;
    float rd = radius >= m ? 1.0f : radius / m;
    DrawRectangleRounded(r, rd, 12, c);
}

void rrect_line(Rectangle r, float radius, float thick, Color c) {
    if (r.width <= 0 || r.height <= 0) return;
    float m = fminf(r.width, r.height) / 2.0f;
    float rd = radius >= m ? 1.0f : radius / m;
    DrawRectangleRoundedLinesEx(r, rd, 12, thick, c);
}

void shadow(Rectangle r, float radius, float do_mo) {
    for (int i = 1; i <= 6; i++) {
        float sp = i * 2.0f;
        unsigned char a = (unsigned char)(13.0f * (1.0f - i / 7.0f) * do_mo);
        rrect(Rectangle{ r.x - sp, r.y - sp + 4, r.width + 2 * sp, r.height + 2 * sp }, radius + sp, Color{ 44, 110, 170, a });
    }
}

void panel(Rectangle r, float radius, bool vien) {
    shadow(r, radius, 0.7f);
    rrect(r, radius, SURFACE);
    if (vien) rrect_line(r, radius, 1.0f, LINE);
}

void hline(float x, float y, float w, Color c) { DrawRectangle((int)x, (int)y, (int)w, 1, c); }
void vline(float x, float y, float h, Color c) { DrawRectangle((int)x, (int)y, 1, (int)h, c); }

void tag(Rectangle r, const std::string &s, Color fg, Color bg, const Font &f) {
    rrect(r, r.height / 2, bg);
    text_c(f, s, r, fg);
}

float tag_auto(float x, float y, float h, const std::string &s, Color fg, Color bg, const Font &f) {
    float w = tw(f, s) + 20;
    tag(Rectangle{ x, y, w, h }, s, fg, bg, f);
    return x + w + 6;
}

// ============================ ICON ============================
static void day(float x1, float y1, float x2, float y2, float t, Color c) { DrawLineEx(Vector2{ x1, y1 }, Vector2{ x2, y2 }, t, c); }
static void vong(float cx, float cy, float rad, float t, Color c) { DrawRing(Vector2{ cx, cy }, rad - t / 2, rad + t / 2, 0, 360, 28, c); }

void icon(Icon ic, float cx, float cy, float size, Color c, float t) {
    float s = size;
    switch (ic) {
        case IC_SEARCH:
            vong(cx - 0.1f * s, cy - 0.1f * s, 0.32f * s, t, c);
            day(cx + 0.14f * s, cy + 0.14f * s, cx + 0.42f * s, cy + 0.42f * s, t, c);
            break;
        case IC_CART:
            day(cx - 0.5f * s, cy - 0.38f * s, cx - 0.3f * s, cy - 0.38f * s, t, c);
            day(cx - 0.3f * s, cy - 0.38f * s, cx - 0.14f * s, cy + 0.16f * s, t, c);
            day(cx - 0.14f * s, cy + 0.16f * s, cx + 0.36f * s, cy + 0.16f * s, t, c);
            day(cx + 0.36f * s, cy + 0.16f * s, cx + 0.46f * s, cy - 0.2f * s, t, c);
            day(cx - 0.24f * s, cy - 0.16f * s, cx + 0.46f * s, cy - 0.2f * s, t, c);
            DrawCircleV(Vector2{ cx - 0.02f * s, cy + 0.4f * s }, 0.075f * s + t * 0.3f, c);
            DrawCircleV(Vector2{ cx + 0.3f * s, cy + 0.4f * s }, 0.075f * s + t * 0.3f, c);
            break;
        case IC_USER:
            vong(cx, cy - 0.18f * s, 0.2f * s, t, c);
            DrawRing(Vector2{ cx, cy + 0.5f * s }, 0.42f * s - t / 2, 0.42f * s + t / 2, 180, 360, 20, c);
            break;
        case IC_USERS:
            vong(cx - 0.18f * s, cy - 0.2f * s, 0.16f * s, t, c);
            DrawRing(Vector2{ cx - 0.18f * s, cy + 0.42f * s }, 0.34f * s - t / 2, 0.34f * s + t / 2, 180, 360, 20, c);
            vong(cx + 0.24f * s, cy - 0.12f * s, 0.13f * s, t, c);
            DrawRing(Vector2{ cx + 0.24f * s, cy + 0.44f * s }, 0.26f * s - t / 2, 0.26f * s + t / 2, 200, 340, 16, c);
            break;
        case IC_CLOSE:
            day(cx - 0.3f * s, cy - 0.3f * s, cx + 0.3f * s, cy + 0.3f * s, t, c);
            day(cx + 0.3f * s, cy - 0.3f * s, cx - 0.3f * s, cy + 0.3f * s, t, c);
            break;
        case IC_CHECK:
            day(cx - 0.32f * s, cy + 0.02f * s, cx - 0.1f * s, cy + 0.26f * s, t, c);
            day(cx - 0.1f * s, cy + 0.26f * s, cx + 0.34f * s, cy - 0.24f * s, t, c);
            break;
        case IC_LEFT:  day(cx + 0.14f * s, cy - 0.3f * s, cx - 0.16f * s, cy, t, c); day(cx - 0.16f * s, cy, cx + 0.14f * s, cy + 0.3f * s, t, c); break;
        case IC_RIGHT: day(cx - 0.14f * s, cy - 0.3f * s, cx + 0.16f * s, cy, t, c); day(cx + 0.16f * s, cy, cx - 0.14f * s, cy + 0.3f * s, t, c); break;
        case IC_DOWN:  day(cx - 0.3f * s, cy - 0.14f * s, cx, cy + 0.16f * s, t, c); day(cx, cy + 0.16f * s, cx + 0.3f * s, cy - 0.14f * s, t, c); break;
        case IC_UP:    day(cx - 0.3f * s, cy + 0.14f * s, cx, cy - 0.16f * s, t, c); day(cx, cy - 0.16f * s, cx + 0.3f * s, cy + 0.14f * s, t, c); break;
        case IC_TRASH:
            day(cx - 0.36f * s, cy - 0.3f * s, cx + 0.36f * s, cy - 0.3f * s, t, c);
            day(cx - 0.12f * s, cy - 0.3f * s, cx - 0.12f * s, cy - 0.44f * s, t, c);
            day(cx + 0.12f * s, cy - 0.3f * s, cx + 0.12f * s, cy - 0.44f * s, t, c);
            day(cx - 0.12f * s, cy - 0.44f * s, cx + 0.12f * s, cy - 0.44f * s, t, c);
            day(cx - 0.28f * s, cy - 0.3f * s, cx - 0.22f * s, cy + 0.42f * s, t, c);
            day(cx + 0.28f * s, cy - 0.3f * s, cx + 0.22f * s, cy + 0.42f * s, t, c);
            day(cx - 0.22f * s, cy + 0.42f * s, cx + 0.22f * s, cy + 0.42f * s, t, c);
            break;
        case IC_PLUS:  day(cx - 0.3f * s, cy, cx + 0.3f * s, cy, t, c); day(cx, cy - 0.3f * s, cx, cy + 0.3f * s, t, c); break;
        case IC_MINUS: day(cx - 0.3f * s, cy, cx + 0.3f * s, cy, t, c); break;
        case IC_EYE:
        case IC_EYE_OFF:
            DrawEllipseLines((int)cx, (int)cy, 0.46f * s, 0.26f * s, c);
            DrawEllipseLines((int)cx, (int)cy, 0.46f * s - 0.5f, 0.26f * s - 0.5f, c);
            DrawCircleV(Vector2{ cx, cy }, 0.12f * s, c);
            if (ic == IC_EYE_OFF) day(cx - 0.4f * s, cy + 0.34f * s, cx + 0.4f * s, cy - 0.34f * s, t, c);
            break;
        case IC_FILTER:
            day(cx - 0.4f * s, cy - 0.34f * s, cx + 0.4f * s, cy - 0.34f * s, t, c);
            day(cx - 0.4f * s, cy - 0.34f * s, cx - 0.06f * s, cy + 0.06f * s, t, c);
            day(cx + 0.4f * s, cy - 0.34f * s, cx + 0.06f * s, cy + 0.06f * s, t, c);
            day(cx - 0.06f * s, cy + 0.06f * s, cx - 0.06f * s, cy + 0.38f * s, t, c);
            day(cx + 0.06f * s, cy + 0.06f * s, cx + 0.06f * s, cy + 0.3f * s, t, c);
            day(cx - 0.06f * s, cy + 0.38f * s, cx + 0.06f * s, cy + 0.3f * s, t, c);
            break;
        case IC_BAG:
            rrect_line(Rectangle{ cx - 0.38f * s, cy - 0.16f * s, 0.76f * s, 0.6f * s }, 4, t, c);
            DrawRing(Vector2{ cx, cy - 0.16f * s }, 0.2f * s - t / 2, 0.2f * s + t / 2, 180, 360, 16, c);
            break;
        case IC_BOX:
            rrect_line(Rectangle{ cx - 0.4f * s, cy - 0.36f * s, 0.8f * s, 0.72f * s }, 3, t, c);
            day(cx - 0.4f * s, cy - 0.1f * s, cx + 0.4f * s, cy - 0.1f * s, t, c);
            day(cx, cy - 0.36f * s, cx, cy - 0.1f * s, t, c);
            break;
        case IC_CHART:
            day(cx - 0.3f * s, cy + 0.38f * s, cx - 0.3f * s, cy + 0.02f * s, t * 1.6f, c);
            day(cx, cy + 0.38f * s, cx, cy - 0.3f * s, t * 1.6f, c);
            day(cx + 0.3f * s, cy + 0.38f * s, cx + 0.3f * s, cy - 0.12f * s, t * 1.6f, c);
            break;
        case IC_TAG:
            day(cx - 0.38f * s, cy - 0.02f * s, cx - 0.02f * s, cy - 0.38f * s, t, c);
            day(cx - 0.02f * s, cy - 0.38f * s, cx + 0.38f * s, cy - 0.38f * s, t, c);
            day(cx + 0.38f * s, cy - 0.38f * s, cx + 0.38f * s, cy + 0.02f * s, t, c);
            day(cx + 0.38f * s, cy + 0.02f * s, cx + 0.02f * s, cy + 0.38f * s, t, c);
            day(cx + 0.02f * s, cy + 0.38f * s, cx - 0.38f * s, cy - 0.02f * s, t, c);
            DrawCircleV(Vector2{ cx + 0.16f * s, cy - 0.16f * s }, 0.06f * s + 0.5f, c);
            break;
        case IC_POS:
            rrect_line(Rectangle{ cx - 0.34f * s, cy - 0.4f * s, 0.68f * s, 0.8f * s }, 4, t, c);
            day(cx - 0.18f * s, cy - 0.16f * s, cx + 0.18f * s, cy - 0.16f * s, t, c);
            day(cx - 0.18f * s, cy + 0.04f * s, cx + 0.18f * s, cy + 0.04f * s, t, c);
            day(cx - 0.18f * s, cy + 0.24f * s, cx + 0.04f * s, cy + 0.24f * s, t, c);
            break;
        case IC_LOGOUT:
            day(cx - 0.12f * s, cy - 0.38f * s, cx - 0.38f * s, cy - 0.38f * s, t, c);
            day(cx - 0.38f * s, cy - 0.38f * s, cx - 0.38f * s, cy + 0.38f * s, t, c);
            day(cx - 0.38f * s, cy + 0.38f * s, cx - 0.12f * s, cy + 0.38f * s, t, c);
            day(cx - 0.08f * s, cy, cx + 0.4f * s, cy, t, c);
            day(cx + 0.18f * s, cy - 0.2f * s, cx + 0.4f * s, cy, t, c);
            day(cx + 0.18f * s, cy + 0.2f * s, cx + 0.4f * s, cy, t, c);
            break;
        case IC_STORE:
            day(cx - 0.42f * s, cy - 0.1f * s, cx - 0.32f * s, cy - 0.38f * s, t, c);
            day(cx - 0.32f * s, cy - 0.38f * s, cx + 0.32f * s, cy - 0.38f * s, t, c);
            day(cx + 0.32f * s, cy - 0.38f * s, cx + 0.42f * s, cy - 0.1f * s, t, c);
            day(cx - 0.42f * s, cy - 0.1f * s, cx + 0.42f * s, cy - 0.1f * s, t, c);
            rrect_line(Rectangle{ cx - 0.34f * s, cy - 0.1f * s, 0.68f * s, 0.5f * s }, 2, t, c);
            break;
        case IC_LOCK:
            rrect_line(Rectangle{ cx - 0.3f * s, cy - 0.04f * s, 0.6f * s, 0.46f * s }, 4, t, c);
            DrawRing(Vector2{ cx, cy - 0.04f * s }, 0.18f * s - t / 2, 0.18f * s + t / 2, 180, 360, 14, c);
            break;
    }
}

// ============================ WIDGET ============================
bool button(Rectangle r, const std::string &label, BtnStyle st, const Font &f, bool enabled, float radius, float spacing) {
    bool h = enabled && hover(r);
    if (h) C.hand = true;
    Color bg = SKY500, fg = WHITE, bd = SKY400;
    bool vien = false;
    switch (st) {
        case BTN_PRIMARY: bg = h ? SKY600 : SKY500; fg = WHITE; break;
        case BTN_OUTLINE: bg = h ? SKY50 : SURFACE; fg = SKY600; bd = SKY400; vien = true; break;
        case BTN_SOFT:    bg = h ? SKY200 : SKY100; fg = SKY600; break;
        case BTN_GHOST:   bg = h ? SKY50 : Color{ 0, 0, 0, 0 }; fg = TEXT; break;
        case BTN_DANGER:  bg = h ? Color{ 212, 76, 88, 255 } : DANGER; fg = WHITE; break;
        case BTN_DARK:    bg = h ? Color{ 46, 72, 104, 255 } : INK; fg = WHITE; break;
    }
    if (!enabled) { bg = Color{ 226, 236, 245, 255 }; fg = Color{ 150, 168, 186, 255 }; vien = false; }
    if (bg.a) rrect(r, radius, bg);
    if (vien) rrect_line(r, radius, 1.5f, bd);
    text_c(f, label, r, fg, spacing);
    return h && C.pressed;
}

bool icon_button(Rectangle r, Icon ic, Color c, Color nen_hover, float size) {
    bool h = hover(r);
    if (h) { C.hand = true; rrect(r, r.height / 2, nen_hover); }
    icon(ic, r.x + r.width / 2, r.y + r.height / 2, size, h ? SKY600 : c, 1.8f);
    return h && C.pressed;
}

bool chip(Rectangle r, const std::string &label, bool chon, const Font &f) {
    bool h = hover(r);
    if (h) C.hand = true;
    if (chon) { rrect(r, r.height / 2, SKY500); text_c(f, label, r, WHITE); }
    else {
        rrect(r, r.height / 2, h ? SKY50 : SURFACE);
        rrect_line(r, r.height / 2, 1.2f, h ? SKY300 : LINE);
        text_c(f, label, r, h ? SKY600 : TEXT);
    }
    return h && C.pressed;
}

bool radio_row(Rectangle r, const std::string &label, bool chon, const Font &f, const std::string &phu) {
    bool h = hover(r);
    if (h) { C.hand = true; rrect(r, 8, SKY50); }
    float cx = r.x + 16, cy = r.y + r.height / 2;
    vong(cx, cy, 8, 1.6f, chon ? SKY500 : Color{ 170, 190, 208, 255 });
    if (chon) DrawCircleV(Vector2{ cx, cy }, 4.2f, SKY500);
    float maxw = r.width - 42 - (phu.empty() ? 0 : tw(F.r13, phu) + 10);
    text_l(f, fit(f, label, maxw), Rectangle{ r.x + 34, r.y, r.width - 34, r.height }, chon ? INK : TEXT);
    if (!phu.empty()) text_r(F.r13, phu, r, MUTED, 10);
    return h && C.pressed;
}

bool check_row(Rectangle r, const std::string &label, bool chon, const Font &f) {
    bool h = hover(r);
    if (h) { C.hand = true; rrect(r, 8, SKY50); }
    Rectangle b = { r.x + 8, r.y + (r.height - 18) / 2, 18, 18 };
    if (chon) { rrect(b, 5, SKY500); icon(IC_CHECK, b.x + 9, b.y + 9, 15, WHITE, 2.0f); }
    else rrect_line(b, 5, 1.6f, Color{ 170, 190, 208, 255 });
    text_l(f, fit(f, label, r.width - 42), Rectangle{ r.x + 34, r.y, r.width - 34, r.height }, chon ? INK : TEXT);
    return h && C.pressed;
}

bool stepper(Rectangle r, int &v, int lo, int hi) {
    int cu = v;
    rrect(r, 10, SURFACE);
    rrect_line(r, 10, 1.2f, LINE);
    float bw = r.height;
    Rectangle tru = { r.x, r.y, bw, r.height }, cong = { r.x + r.width - bw, r.y, bw, r.height };
    bool htru = hover(tru), hcong = hover(cong);
    if (htru) { C.hand = true; rrect(Rectangle{ tru.x + 2, tru.y + 2, tru.width - 4, tru.height - 4 }, 8, SKY50); }
    if (hcong) { C.hand = true; rrect(Rectangle{ cong.x + 2, cong.y + 2, cong.width - 4, cong.height - 4 }, 8, SKY50); }
    icon(IC_MINUS, tru.x + bw / 2, tru.y + r.height / 2, 14, v > lo ? TEXT : Color{ 190, 205, 220, 255 }, 1.8f);
    icon(IC_PLUS, cong.x + bw / 2, cong.y + r.height / 2, 14, v < hi ? TEXT : Color{ 190, 205, 220, 255 }, 1.8f);
    text_c(F.m15, std::to_string(v), Rectangle{ r.x + bw, r.y, r.width - 2 * bw, r.height }, INK);
    if (htru && C.pressed && v > lo) v--;
    if (hcong && C.pressed && v < hi) v++;
    return v != cu;
}

void scroll_bar(Rectangle vung, float cao_nd, float cuon) {
    if (cao_nd <= vung.height + 1) return;
    float ti = vung.height / cao_nd;
    float cao = fmaxf(36, vung.height * ti);
    float toi_da = cao_nd - vung.height;
    float y = vung.y + (vung.height - cao) * (toi_da > 0 ? cuon / toi_da : 0);
    rrect(Rectangle{ vung.x + vung.width - 8, y, 5, cao }, 3, Color{ 150, 190, 224, 150 });
}

// ============================ ẢNH SẢN PHẨM ============================
namespace {
struct TexEntry { Texture2D tex{}; bool ok = false; bool thu = false; };
std::unordered_map<std::string, TexEntry> cache_tex;
std::unordered_map<std::string, int> cache_dem;
int da_nap_khung = 0;
}

int anh_dem(const std::string &idsp) {
    auto it = cache_dem.find(idsp);
    if (it != cache_dem.end()) return it->second;
    int n = dem_anh(idsp);
    cache_dem[idsp] = n;
    return n;
}

const Texture2D *anh_lay(const std::string &idsp, int stt, int rong_toi_da) {
    std::string key = idsp + "_" + std::to_string(stt) + "@" + std::to_string(rong_toi_da);
    TexEntry &e = cache_tex[key];
    if (e.ok) return &e.tex;
    if (e.thu) return nullptr;                       // đã thử mà không có ảnh
    static double khung_cu = -1;
    if (khung_cu != C.time) { khung_cu = C.time; da_nap_khung = 0; }
    if (da_nap_khung >= 2) return nullptr;           // mỗi khung hình chỉ nạp tối đa 2 ảnh để không bị khựng
    da_nap_khung++;
    e.thu = true;
    Image im = LoadImage(duong_dan_anh(idsp, stt).c_str());
    if (im.data == nullptr) return nullptr;
    if (im.width > rong_toi_da) {
        int cao = (int)((long long)im.height * rong_toi_da / im.width);
        ImageResize(&im, rong_toi_da, cao);
    }
    e.tex = LoadTextureFromImage(im);
    UnloadImage(im);
    if (e.tex.id == 0) return nullptr;
    SetTextureFilter(e.tex, TEXTURE_FILTER_BILINEAR);
    e.ok = true;
    return &e.tex;
}

void anh_xoa_dem(const std::string &idsp) {
    cache_dem.erase(idsp);
    for (auto it = cache_tex.begin(); it != cache_tex.end();) {
        if (it->first.compare(0, idsp.size() + 1, idsp + "_") == 0) {
            if (it->second.ok) UnloadTexture(it->second.tex);
            it = cache_tex.erase(it);
        } else ++it;
    }
}

void anh_giai_phong() {
    for (auto &kv : cache_tex) if (kv.second.ok) UnloadTexture(kv.second.tex);
    cache_tex.clear();
    cache_dem.clear();
}

static void che_goc(Rectangle r, float rad, Color nen) {
    if (rad <= 0) return;
    float ro = rad * 1.5f;
    DrawRing(Vector2{ r.x + rad, r.y + rad }, rad, ro, 180, 270, 12, nen);
    DrawRing(Vector2{ r.x + r.width - rad, r.y + rad }, rad, ro, 270, 360, 12, nen);
    DrawRing(Vector2{ r.x + r.width - rad, r.y + r.height - rad }, rad, ro, 0, 90, 12, nen);
    DrawRing(Vector2{ r.x + rad, r.y + r.height - rad }, rad, ro, 90, 180, 12, nen);
}

void ve_anh(const Texture2D &t, Rectangle dst, float bo_goc, Color nen, float zoom) {
    float ta = (float)t.width / t.height, da = dst.width / dst.height;
    float sw, sh;
    if (ta > da) { sh = (float)t.height; sw = sh * da; } else { sw = (float)t.width; sh = sw / da; }
    sw /= zoom; sh /= zoom;
    Rectangle src = { (t.width - sw) / 2, (t.height - sh) / 2, sw, sh };
    push_clip(dst);
    DrawTexturePro(t, src, dst, Vector2{ 0, 0 }, 0, WHITE);
    pop_clip();
    che_goc(dst, bo_goc, nen);
}

void ve_anh_trong(Rectangle dst, float bo_goc, const std::string &hang, const std::string &ten) {
    rrect(dst, bo_goc, SKY100);
    float gy = dst.y + dst.height * 0.35f, ge = dst.y + dst.height - bo_goc;
    DrawRectangleGradientV((int)dst.x, (int)gy, (int)dst.width, (int)(ge - gy), SKY100, SKY200);
    rrect(Rectangle{ dst.x, ge - bo_goc, dst.width, 2 * bo_goc }, bo_goc, SKY200);
    std::string ch = hang.empty() ? "?" : to_utf8(std::u32string(1, to_u32(hang)[0]));
    const Font &f = dst.width > 300 ? F.s40 : F.s30;
    text_c(f, hoa(ch), Rectangle{ dst.x, dst.y + dst.height * 0.3f, dst.width, dst.height * 0.2f }, Color{ 255, 255, 255, 200 });
    if (dst.width >= 120) text_c(F.r13, "Chưa có ảnh", Rectangle{ dst.x, dst.y + dst.height * 0.52f, dst.width, 20 }, Color{ 120, 160, 196, 255 });
    (void)ten;
}

// ============================ THÔNG BÁO ============================
namespace {
struct Toast { std::string msg; ToastKind k; double t0; };
std::vector<Toast> ds_toast;
}

void toast(const std::string &msg, ToastKind k) {
    ds_toast.push_back(Toast{ msg, k, GetTime() });
    if (ds_toast.size() > 4) ds_toast.erase(ds_toast.begin());
}

void draw_toasts() {
    double now = GetTime();
    float y = (float)C.sh - 28;
    for (int i = (int)ds_toast.size() - 1; i >= 0; i--) {
        double tuoi = now - ds_toast[i].t0;
        double song = 3.0;
        if (tuoi > song) { ds_toast.erase(ds_toast.begin() + i); continue; }
        float a = tuoi < 0.2 ? (float)(tuoi / 0.2) : (tuoi > song - 0.5 ? (float)((song - tuoi) / 0.5) : 1.0f);
        const Toast &t = ds_toast[i];
        Color nen = t.k == TOAST_OK ? OKC : (t.k == TOAST_ERR ? DANGER : INK);
        float w = fminf(tw(F.m15, t.msg) + 64, (float)C.sw - 40);
        Rectangle r = { (C.sw - w) / 2, y - 46 + (1.0f - a) * 10, w, 46 };
        y -= 54;
        shadow(r, 14, a);
        rrect(r, 14, Fade(nen, a));
        icon(t.k == TOAST_OK ? IC_CHECK : (t.k == TOAST_ERR ? IC_CLOSE : IC_TAG), r.x + 26, r.y + 23, 16, Fade(WHITE, a), 2.0f);
        text_l(F.m15, fit(F.m15, t.msg, w - 64), Rectangle{ r.x + 46, r.y, w - 46, r.height }, Fade(WHITE, a));
    }
}

// ============================ TIỆN ÍCH ============================
std::string tien(long long v) { return dinh_dang_tien(v) + "₫"; }

static std::string thap_phan(double x) {            // 12.0 -> "12", 12.34 -> "12,3"
    long long nguyen = (long long)x;
    int le = (int)llround((x - nguyen) * 10);
    if (le >= 10) { nguyen++; le = 0; }
    return le == 0 ? std::to_string(nguyen) : std::to_string(nguyen) + "," + std::to_string(le);
}

std::string tien_gon(long long v) {
    long long a = v < 0 ? -v : v;
    std::string s;
    if (a >= 1000000000LL) s = thap_phan(a / 1e9) + " tỷ";
    else if (a >= 1000000LL) s = thap_phan(a / 1e6) + "tr";
    else if (a >= 1000LL) s = thap_phan(a / 1e3) + "k";
    else s = std::to_string(a);
    return (v < 0 ? "-" : "") + s;
}

std::string so_gon(long long v) {
    if (v >= 10000) return std::to_string(v / 1000) + "k";
    if (v >= 1000) return thap_phan(v / 1000.0) + "k";
    return std::to_string(v);
}

Color mau_theo_ten(const std::string &ten, bool *biet) {
    std::string n = normalize(ten.c_str());
    struct M { const char *k; Color c; };
    static const M bang[] = {
        { "xanh den", {28, 44, 78, 255} }, { "xanh navy", {28, 44, 78, 255} }, { "navy", {28, 44, 78, 255} },
        { "xanh dam", {36, 74, 140, 255} }, { "xanh duong", {64, 140, 224, 255} }, { "xanh la", {104, 180, 118, 255} },
        { "xanh reu", {120, 140, 80, 255} }, { "xanh ngoc", {80, 190, 190, 255} }, { "xanh", {96, 160, 228, 255} },
        { "den", {36, 40, 48, 255} }, { "trang", {250, 250, 250, 255} }, { "xam", {158, 166, 176, 255} },
        { "bac", {200, 206, 214, 255} }, { "do", {222, 78, 86, 255} }, { "hong", {244, 168, 190, 255} },
        { "cam", {244, 156, 86, 255} }, { "vang", {244, 208, 90, 255} }, { "nau", {140, 100, 76, 255} },
        { "be", {232, 220, 196, 255} }, { "kem", {240, 230, 206, 255} }, { "tim", {160, 130, 210, 255} },
    };
    for (const M &m : bang) if (n.find(m.k) != std::string::npos) { if (biet) *biet = true; return m.c; }
    if (biet) *biet = false;
    return Color{ 190, 200, 212, 255 };
}

Color lerp_c(Color a, Color b, float t) { return ColorLerp(a, b, t); }
float ease_out(float t) { if (t < 0) t = 0; if (t > 1) t = 1; return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t); }

}  // namespace ui
