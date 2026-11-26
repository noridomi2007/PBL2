// ĐẦU VÀO: nhận phím / chữ ĐÚNG THỨ TỰ + ô nhập liệu (Unikey, giữ Backspace, ngày tự chèn "/")
//
// VÌ SAO TRƯỚC ĐÂY BỊ "DÍNH CHỮ" KHI GÕ UNIKEY?
//   Unikey không gửi sẵn chữ có dấu. Gõ "as" nó gửi:  Backspace (xóa chữ a)  rồi  chữ "á".
//   raylib lại tách thành HAI hàng đợi riêng (GetKeyPressed / GetCharPressed) nên mất thứ tự giữa phím và chữ.
//   Nếu đọc "tất cả Backspace trước, tất cả chữ sau" thì khi 2 thao tác rơi vào cùng 1 khung hình
//   (gõ nhanh, hoặc khung hình bị khựng) Backspace sẽ xóa NHẦM chữ -> chữ bị dính / mất / thừa.
//   Cách sửa: móc thẳng callback phím + chữ của GLFW (đã có sẵn trong libraylib.a), ghi vào MỘT hàng đợi
//   duy nhất theo đúng thứ tự đến, rồi xử lý lần lượt từng sự kiện.
#include "Ui.h"
#include "../../Tienich/Tienich.h"
#include <ctime>
#include <cmath>
#include <cstring>
struct GLFWwindow;
extern "C" {
typedef void (*GLFWcharfun)(GLFWwindow *, unsigned int);
typedef void (*GLFWkeyfun)(GLFWwindow *, int, int, int, int);
GLFWwindow *glfwGetCurrentContext(void);
GLFWcharfun glfwSetCharCallback(GLFWwindow *, GLFWcharfun);
GLFWkeyfun glfwSetKeyCallback(GLFWwindow *, GLFWkeyfun);
void rlScalef(float x, float y, float z);      // của rlgl (đã có trong libraylib.a)
}

namespace ui {

Ctx C;

namespace {
struct Ev { int loai; unsigned cp; int key; int action; int mods; };   // loai: 0 = chữ, 1 = phím
std::vector<Ev> evq;
size_t evhead = 0;
bool hooked = false;
GLFWcharfun prev_char = nullptr;
GLFWkeyfun prev_key = nullptr;
Field *g_focus = nullptr;
double g_blink = 0;
double bs_down_tu = -1, bs_ke_tiep = 0;

void gioi_han_hang_doi() {
    if (evq.size() > 4096) { evq.erase(evq.begin(), evq.begin() + 2048); evhead = evhead > 2048 ? evhead - 2048 : 0; }
}
void on_char(GLFWwindow *w, unsigned int cp) {
    if (prev_char) prev_char(w, cp);          // vẫn cho raylib nhận như cũ
    evq.push_back(Ev{ 0, cp, 0, 0, 0 });
    gioi_han_hang_doi();
}
void on_key(GLFWwindow *w, int key, int sc, int action, int mods) {
    if (prev_key) prev_key(w, key, sc, action, mods);
    if (action != 0) evq.push_back(Ev{ 1, 0, key, action, mods });   // 1 = nhấn, 2 = giữ lặp (bỏ qua nhả)
    gioi_han_hang_doi();
}
}  // namespace

void init_input() {
    GLFWwindow *w = glfwGetCurrentContext();
    if (!w) { hooked = false; return; }
    prev_char = glfwSetCharCallback(w, on_char);
    prev_key = glfwSetKeyCallback(w, on_key);
    hooked = true;
    SetExitKey(KEY_NULL);                      // ESC không được tắt cả chương trình
}

void begin_frame() {
    // PHÓNG TO THEO CỬA SỔ: thiết kế gốc là 1360x840; cửa sổ to hơn bao nhiêu lần thì toàn bộ giao diện to lên bấy nhiêu lần.
    // Mọi thứ vẽ ra đều nhân tỉ lệ này (rlScalef), chuột thì chia lại, nên code bố cục vẫn chỉ làm việc với tọa độ logic.
    int pw = GetScreenWidth(), ph = GetScreenHeight();
    float s = fminf(pw / 1360.0f, ph / 840.0f);
    if (s < 0.6f) s = 0.6f;
    if (s > 4.0f) s = 4.0f;
    C.scale = s;
    C.sw = (int)ceilf(pw / s);
    C.sh = (int)ceilf(ph / s);
    rlScalef(s, s, 1.0f);
    Vector2 mp = GetMousePosition();
    C.mouse = Vector2{ mp.x / s, mp.y / s };
    C.wheel = GetMouseWheelMove();
    C.pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    C.released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    C.down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    C.dt = GetFrameTime();
    C.time = GetTime();
    C.blocked = false;
    C.hand = C.ibeam = false;

    if (!hooked) {   // dự phòng nếu không móc được GLFW: đọc 2 hàng đợi của raylib (có thể sai thứ tự khi gõ cực nhanh)
        int k = GetKeyPressed();
        while (k > 0) { evq.push_back(Ev{ 1, 0, k, 1, (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) ? 2 : 0 }); k = GetKeyPressed(); }
        int ch = GetCharPressed();
        while (ch > 0) { evq.push_back(Ev{ 0, (unsigned)ch, 0, 0, 0 }); ch = GetCharPressed(); }
    }

    // GIỮ Backspace -> xóa liên tục: chờ 0,42s rồi cứ 0,032s xóa 1 ký tự (tự đo thời gian, không phụ thuộc phím lặp của Windows)
    C.bs_lap = 0;
    if (IsKeyDown(KEY_BACKSPACE)) {
        if (bs_down_tu < 0) { bs_down_tu = C.time; bs_ke_tiep = C.time + 0.42; }
        else if (C.time >= bs_ke_tiep) {
            int n = (int)((C.time - bs_ke_tiep) / 0.032) + 1;
            C.bs_lap = n;
            bs_ke_tiep += n * 0.032;
        }
    } else bs_down_tu = -1;
}

void end_frame() {
    evq.clear();
    evhead = 0;
    if (C.focus_next) { C.focus_first = true; C.focus_next = false; }   // Tab ở ô cuối -> quay lại ô đầu khung sau
    else C.focus_first = false;
    SetMouseCursor(C.hand ? MOUSE_CURSOR_POINTING_HAND : (C.ibeam ? MOUSE_CURSOR_IBEAM : MOUSE_CURSOR_DEFAULT));
}

// ============================================================================================
// Ô NHẬP LIỆU
// ============================================================================================
void Field::set(const std::string &v) {
    s.clear();
    std::u32string u = to_u32(v);
    bool so = (kind == FK_DATE || kind == FK_PHONE || kind == FK_DIGITS || kind == FK_MONEY);
    for (char32_t c : u) {
        if ((int)s.size() >= max_len) break;
        if (so) { if (c >= U'0' && c <= U'9') s.push_back(c); }
        else if (c >= 32 && c != '|' && glyph_ok(c)) s.push_back(c);
    }
    caret = (int)s.size();
    scroll_x = 0;
}

long long Field::number() const {
    long long v = 0;
    for (char32_t c : s) { if (c < U'0' || c > U'9') continue; if (v > 90000000000000000LL) break; v = v * 10 + (c - U'0'); }
    return v;
}

bool Field::date(int &d, int &m, int &y) const {
    if (kind != FK_DATE || s.size() != 8) return false;
    auto dg = [&](int i) { return (int)(s[i] - U'0'); };
    d = dg(0) * 10 + dg(1);
    m = dg(2) * 10 + dg(3);
    y = dg(4) * 1000 + dg(5) * 100 + dg(6) * 10 + dg(7);
    return true;
}

void Field::set_today() {
    int d, m, y;
    ngay_hom_nay(d, m, y);
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d%02d%04d", d, m, y);
    set(buf);
}

std::string Field::display() const {
    if (kind == FK_DATE) {
        std::u32string o;
        size_t n = s.size();
        o += s.substr(0, n < 2 ? n : 2);
        if (n >= 2) o += U'/';
        if (n > 2) o += s.substr(2, n < 4 ? n - 2 : 2);
        if (n >= 4) o += U'/';
        if (n > 4) o += s.substr(4);
        return to_utf8(o);
    }
    if (kind == FK_MONEY) {
        std::string r;
        int n = (int)s.size();
        for (int i = 0; i < n; i++) { r += (char)s[i]; if ((n - 1 - i) % 3 == 0 && i != n - 1) r += '.'; }
        return r;
    }
    return to_utf8(s);
}

namespace {

bool ngay_hop_le_tung_phan(const std::u32string &d) {
    auto dg = [&](size_t i) { return (int)(d[i] - U'0'); };
    if (d.size() >= 2) { int ng = dg(0) * 10 + dg(1); if (ng < 1 || ng > 31) return false; }
    if (d.size() >= 4) { int th = dg(2) * 10 + dg(3); if (th < 1 || th > 12) return false; }
    return true;
}

// Ngày: chỉ lưu CHỮ SỐ; dấu "/" chỉ là hiển thị nên Backspace xóa đúng 1 chữ số, không bao giờ "kẹt" ở dấu "/".
// Gõ "5" ở ô ngày -> tự thành "05/"; gõ "1" rồi "/" -> "01/"; chặn ngày > 31, tháng > 12.
void chen_ngay(Field &f, char32_t c) {
    std::u32string &d = f.s;
    if (c >= U'0' && c <= U'9') {
        if (d.size() >= 8) return;
        std::u32string t = d;
        if (d.size() == 0 && c > U'3') { t += U'0'; t += c; }
        else if (d.size() == 2 && c > U'1') { t += U'0'; t += c; }
        else t += c;
        if (t.size() > 8 || !ngay_hop_le_tung_phan(t)) return;
        d = t;
    } else if (c == U'/' || c == U'-' || c == U'.' || c == U' ') {
        std::u32string t = d;
        if (d.size() == 1) t = U"0" + d;
        else if (d.size() == 3) t = d.substr(0, 2) + U"0" + d.substr(2);
        else return;
        if (ngay_hop_le_tung_phan(t)) d = t;
    }
    f.caret = (int)d.size();
}

void chen_ky_tu(Field &f, char32_t c) {
    if (f.kind == FK_DATE) { chen_ngay(f, c); return; }
    if (f.kind == FK_PHONE || f.kind == FK_DIGITS || f.kind == FK_MONEY) {
        if (c < U'0' || c > U'9') return;
        if ((int)f.s.size() >= f.max_len) return;
        if (f.kind != FK_PHONE && f.s.empty() && c == U'0') return;   // không cho số 0 đứng đầu (tiền, số lượng)
        if (f.end_only()) f.caret = (int)f.s.size();
        f.s.insert(f.s.begin() + f.caret, c);
        f.caret++;
        return;
    }
    if (c < 32 || c == 127 || c == U'|' || !glyph_ok(c)) return;   // "|" làm hỏng file txt; ký tự ngoài font sẽ hiện "?"
    if (f.kind == FK_EMAIL && c == U' ') return;
    if ((int)f.s.size() >= f.max_len) return;
    f.s.insert(f.s.begin() + f.caret, c);
    f.caret++;
}

void xoa_truoc(Field &f) {
    if (f.end_only()) f.caret = (int)f.s.size();
    if (f.caret <= 0) return;
    f.s.erase(f.s.begin() + f.caret - 1);
    f.caret--;
}

void xoa_tu(Field &f) {
    if (f.kind == FK_DATE || f.kind == FK_MONEY) { xoa_truoc(f); return; }
    while (f.caret > 0 && f.s[f.caret - 1] == U' ') xoa_truoc(f);
    while (f.caret > 0 && f.s[f.caret - 1] != U' ') xoa_truoc(f);
}

float rong_tien_to(const Font &font, const Field &f, int so_ky_tu, const std::string &disp, bool che) {
    if (che) return so_ky_tu * 11.0f;
    if (f.end_only()) return tw(font, disp);
    std::u32string u = f.s.substr(0, so_ky_tu);
    return tw(font, to_utf8(u));
}

}  // namespace

bool is_focused(const Field &f) { return g_focus == &f; }
void focus(Field *f) { g_focus = f; g_blink = C.time; }
void unfocus() { g_focus = nullptr; }
bool any_field_focused() { return g_focus != nullptr; }

bool field(Field &f, Rectangle r, const Font &font) {
    bool enter = false;
    bool che = (f.kind == FK_PASSWORD && !f.reveal);
    bool h = hover(r);
    if (h) C.ibeam = true;

    // Tab: nhảy sang ô kế tiếp (ô được vẽ sau)
    if (C.focus_next && g_focus == nullptr) { g_focus = &f; f.caret = (int)f.s.size(); g_blink = C.time; C.focus_next = false; }
    if (C.focus_first) { g_focus = &f; f.caret = (int)f.s.size(); g_blink = C.time; C.focus_first = false; }

    // nút con mắt của ô mật khẩu
    Rectangle mat = { r.x + r.width - 38, r.y + (r.height - 30) / 2, 30, 30 };
    bool bam_mat = false;
    if (f.kind == FK_PASSWORD) {
        if (hover(mat)) { C.hand = true; C.ibeam = false; if (C.pressed) { f.reveal = !f.reveal; bam_mat = true; } }
    }

    float pad_l = 14, pad_r = (f.kind == FK_PASSWORD) ? 44 : 14;
    Rectangle in = { r.x + pad_l, r.y, r.width - pad_l - pad_r, r.height };
    std::string disp = che ? std::string() : f.display();

    if (C.pressed && !bam_mat) {
        if (h) {
            g_focus = &f; g_blink = C.time;
            if (f.end_only()) f.caret = (int)f.s.size();
            else {   // đặt con trỏ tại vị trí bấm chuột
                float lx = C.mouse.x - (in.x - f.scroll_x);
                int best = 0; float bd = 1e9f;
                for (int i = 0; i <= (int)f.s.size(); i++) {
                    float d = fabsf(rong_tien_to(font, f, i, disp, che) - lx);
                    if (d < bd) { bd = d; best = i; }
                }
                f.caret = best;
            }
        } else if (g_focus == &f && !C.blocked) {
            g_focus = nullptr;
        }
    }

    bool focused = (g_focus == &f);
    if (focused) {
        // Xử lý TỪNG sự kiện theo đúng thứ tự đến (chống dính chữ Unikey)
        while (evhead < evq.size()) {
            Ev e = evq[evhead++];
            g_blink = C.time;
            if (e.loai == 0) { chen_ky_tu(f, (char32_t)e.cp); continue; }
            bool ctrl = (e.mods & 2) != 0;
            switch (e.key) {
                case KEY_BACKSPACE:
                    if (e.action == 2) break;                    // phím lặp của Windows: bỏ qua, đã có bộ đếm giữ phím riêng
                    if (ctrl) xoa_tu(f); else xoa_truoc(f);
                    break;
                case KEY_DELETE:
                    if (!f.end_only() && f.caret < (int)f.s.size()) f.s.erase(f.s.begin() + f.caret);
                    break;
                case KEY_LEFT:  if (!f.end_only() && f.caret > 0) f.caret--; break;
                case KEY_RIGHT: if (!f.end_only() && f.caret < (int)f.s.size()) f.caret++; break;
                case KEY_HOME:  if (!f.end_only()) f.caret = 0; break;
                case KEY_END:   f.caret = (int)f.s.size(); break;
                case KEY_ENTER: case KEY_KP_ENTER: enter = true; break;
                case KEY_ESCAPE: g_focus = nullptr; break;
                case KEY_V:
                    if (ctrl) {
                        const char *clip = GetClipboardText();
                        if (clip) {
                            std::u32string u = to_u32(clip);
                            for (char32_t c : u) { if (c == U'\n' || c == U'\r') break; chen_ky_tu(f, c); }
                        }
                    }
                    break;
                case KEY_TAB: g_focus = nullptr; C.focus_next = true; break;
                default: break;
            }
            if (g_focus != &f) break;    // đã chuyển focus: phần sự kiện còn lại để ô mới xử lý
        }
        if (g_focus == &f) for (int i = 0; i < C.bs_lap; i++) xoa_truoc(f);
        focused = (g_focus == &f);
    }

    // ----- vẽ -----
    rrect(r, 10, SURFACE);
    rrect_line(r, 10, focused ? 2.0f : 1.2f, focused ? SKY400 : (h ? SKY300 : LINE));

    int n = (int)f.s.size();
    float fh = fsz(font);
    float ty = r.y + (r.height - fh) / 2.0f - 1.0f;
    int cur = f.end_only() ? n : f.caret;
    float caret_px = rong_tien_to(font, f, cur, disp, che);
    float tong = che ? n * 11.0f : (disp.empty() ? 0 : tw(font, disp));
    if (f.end_only()) caret_px = tong;
    if (caret_px - f.scroll_x > in.width - 3) f.scroll_x = caret_px - in.width + 3;
    if (caret_px - f.scroll_x < 0) f.scroll_x = caret_px;
    if (f.scroll_x < 0) f.scroll_x = 0;
    if (tong <= in.width) f.scroll_x = 0;

    push_clip(Rectangle{ in.x, r.y + 1, in.width, r.height - 2 });
    if (n == 0 && !f.hint.empty()) text(font, f.hint, in.x, ty, Color{160, 178, 196, 255});
    if (che) { for (int i = 0; i < n; i++) DrawCircleV(Vector2{ in.x - f.scroll_x + i * 11.0f + 5.0f, r.y + r.height / 2 }, 3.3f, TEXT); }
    else if (!disp.empty()) text(font, disp, in.x - f.scroll_x, ty, INK);
    if (focused && fmod(C.time - g_blink, 1.1) < 0.65)
        DrawRectangle((int)(in.x - f.scroll_x + caret_px), (int)(ty + 1), 2, (int)fh + 1, SKY600);
    pop_clip();

    if (f.kind == FK_PASSWORD) icon(f.reveal ? IC_EYE_OFF : IC_EYE, mat.x + 15, mat.y + 15, 18, MUTED, 1.7f);
    return enter;
}

}  // namespace ui
