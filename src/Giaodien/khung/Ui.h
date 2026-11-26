#pragma once
// KHUNG GIAO DIỆN RAYLIB (immediate mode): màu, font, vẽ cơ bản, ô nhập liệu, ảnh sản phẩm, thông báo.
// Mọi widget được gọi MỖI KHUNG HÌNH và trả về kết quả ngay (button -> true nếu vừa được bấm).
#include "raylib.h"
#include <string>
#include <vector>

namespace ui {

// ============================ BẢNG MÀU: XANH SKY PASTEL ============================
inline constexpr Color BG      = {243, 249, 255, 255};   // nền trang
inline constexpr Color SURFACE = {255, 255, 255, 255};   // thẻ / ô trắng
inline constexpr Color SKY50   = {236, 246, 254, 255};
inline constexpr Color SKY100  = {219, 238, 253, 255};
inline constexpr Color SKY200  = {190, 225, 249, 255};
inline constexpr Color SKY300  = {152, 207, 243, 255};
inline constexpr Color SKY400  = {112, 187, 238, 255};   // sky pastel chính
inline constexpr Color SKY500  = {66, 158, 228, 255};    // nút chính
inline constexpr Color SKY600  = {40, 129, 201, 255};    // hover / nhấn
inline constexpr Color INK     = {27, 45, 66, 255};      // chữ tiêu đề
inline constexpr Color TEXT    = {58, 78, 99, 255};      // chữ thường
inline constexpr Color MUTED   = {121, 142, 162, 255};   // chữ phụ
inline constexpr Color LINE    = {221, 234, 245, 255};   // đường kẻ / viền
inline constexpr Color OKC     = {86, 184, 146, 255};
inline constexpr Color OKBG    = {222, 246, 237, 255};
inline constexpr Color WARNC   = {232, 154, 66, 255};
inline constexpr Color WARNBG  = {255, 239, 217, 255};
inline constexpr Color DANGER  = {226, 94, 104, 255};
inline constexpr Color DANGBG  = {254, 226, 229, 255};
inline constexpr Color LAV     = {164, 156, 232, 255};   // admin
inline constexpr Color LAVBG   = {234, 231, 252, 255};
inline constexpr Color MINT    = {86, 184, 146, 255};    // nhân viên
inline constexpr Color MINTBG  = {222, 246, 237, 255};

// ============================ FONT ============================
// Chữ được nạp ở độ phân giải gấp FONT_SS lần rồi vẽ thu nhỏ lại, nhờ vậy khi cửa sổ phóng to (giao diện
// được nhân tỉ lệ C.scale) chữ vẫn sắc nét. Mọi số đo trong code (kích thước, tọa độ) đều là tọa độ "logic".
constexpr int FONT_SS = 2;
inline float fsz(const Font &f) { return f.baseSize / (float)FONT_SS; }   // cỡ chữ logic
struct Fonts {
    Font r13, r15, r17;                    // Roboto Regular
    Font m13, m15, m17;                    // Medium
    Font b15, b17, b21, b27, b35;          // Bold
    Font s22, s30, s40;                    // Times Bold (tiêu đề kiểu LV)
};
extern Fonts F;
void load_fonts();      // gọi SAU InitWindow
void unload_fonts();

float tw(const Font &f, const std::string &s, float spacing = 0);                 // đo độ rộng chữ
void text(const Font &f, const std::string &s, float x, float y, Color c, float spacing = 0);
void text_l(const Font &f, const std::string &s, Rectangle r, Color c, float padX = 0, float spacing = 0); // trái, giữa dọc
void text_c(const Font &f, const std::string &s, Rectangle r, Color c, float spacing = 0);                 // giữa cả hai chiều
void text_r(const Font &f, const std::string &s, Rectangle r, Color c, float padX = 0, float spacing = 0); // phải, giữa dọc
std::string fit(const Font &f, const std::string &s, float maxw, float spacing = 0);   // cắt bớt, thêm "…"
// Xuống dòng theo từ, tối đa max_dong dòng (dòng cuối bị cắt "…"). Trả về số dòng đã vẽ.
int text_wrap(const Font &f, const std::string &s, float x, float y, float maxw, Color c, int max_dong, float cao_dong, float spacing = 0);
int dem_dong(const Font &f, const std::string &s, float maxw, int max_dong, float spacing = 0);
std::string hoa(const std::string &s);   // IN HOA có dấu tiếng Việt (dùng cho menu / nhãn)

// UTF-8 <-> UTF-32
std::u32string to_u32(const std::string &s);
std::string to_utf8(const std::u32string &s);
bool glyph_ok(char32_t c);               // font có vẽ được ký tự này không

// ============================ KHUNG HÌNH & ĐẦU VÀO ============================
struct Ctx {
    Vector2 mouse{0, 0};
    float wheel = 0;
    bool pressed = false, released = false, down = false;
    bool blocked = false;       // true: bỏ qua mọi tương tác (đang có hộp thoại phía trên)
    bool hand = false, ibeam = false;
    float dt = 0.016f;
    double time = 0;
    int sw = 0, sh = 0;         // kích thước cửa sổ TÍNH THEO TỌA ĐỘ LOGIC (đã chia cho scale)
    float scale = 1.0f;         // hệ số phóng: cửa sổ càng to giao diện càng to theo
    // phục vụ ô nhập liệu
    int bs_lap = 0;             // số lần xóa lặp do GIỮ phím Backspace trong khung hình này
    bool focus_next = false, focus_first = false;
};
extern Ctx C;

void init_input();              // gọi SAU InitWindow: móc GLFW để nhận phím / chữ ĐÚNG THỨ TỰ (chống dính chữ Unikey)
void begin_frame();
void end_frame();

bool hover(Rectangle r);
void push_clip(Rectangle r);
void pop_clip();

// ============================ VẼ CƠ BẢN ============================
void rrect(Rectangle r, float radius, Color c);
void rrect_line(Rectangle r, float radius, float thick, Color c);
void shadow(Rectangle r, float radius, float do_mo = 1.0f);          // bóng mềm dưới thẻ
void panel(Rectangle r, float radius = 14, bool vien = true);        // thẻ trắng có viền + bóng nhẹ
void hline(float x, float y, float w, Color c = LINE);
void vline(float x, float y, float h, Color c = LINE);
void tag(Rectangle r, const std::string &s, Color fg, Color bg, const Font &f);  // nhãn nhỏ bo tròn
float tag_auto(float x, float y, float h, const std::string &s, Color fg, Color bg, const Font &f); // tự đo độ rộng, trả về x kế tiếp

// icon vẽ bằng hình học (không phụ thuộc font)
enum Icon { IC_SEARCH, IC_CART, IC_USER, IC_CLOSE, IC_CHECK, IC_LEFT, IC_RIGHT, IC_DOWN, IC_UP, IC_TRASH,
            IC_PLUS, IC_MINUS, IC_EYE, IC_EYE_OFF, IC_FILTER, IC_BAG, IC_BOX, IC_CHART, IC_USERS, IC_TAG, IC_POS, IC_LOGOUT, IC_STORE, IC_LOCK };
void icon(Icon ic, float cx, float cy, float size, Color c, float thick = 2.0f);

// ============================ WIDGET ============================
enum BtnStyle { BTN_PRIMARY, BTN_OUTLINE, BTN_SOFT, BTN_GHOST, BTN_DANGER, BTN_DARK };
bool button(Rectangle r, const std::string &label, BtnStyle st = BTN_PRIMARY, const Font &f = F.m15,
            bool enabled = true, float radius = 10, float spacing = 0);
bool icon_button(Rectangle r, Icon ic, Color c = TEXT, Color nen_hover = SKY50, float size = 18);
bool chip(Rectangle r, const std::string &label, bool chon, const Font &f = F.m15);   // viên thuốc chọn / bỏ chọn
bool radio_row(Rectangle r, const std::string &label, bool chon, const Font &f = F.r15, const std::string &phu = "");
bool check_row(Rectangle r, const std::string &label, bool chon, const Font &f = F.r15);
bool stepper(Rectangle r, int &v, int lo, int hi);   // [-] số [+]; true nếu đổi giá trị
void scroll_bar(Rectangle vung, float cao_noi_dung, float cuon);

// ============================ Ô NHẬP LIỆU ============================
enum FieldKind { FK_TEXT, FK_PASSWORD, FK_PHONE, FK_DIGITS, FK_MONEY, FK_DATE, FK_EMAIL };

struct Field {
    FieldKind kind = FK_TEXT;
    std::u32string s;            // nội dung THẬT (ngày / tiền / sđt chỉ chứa chữ số)
    int max_len = 64;            // số ký tự (không phải byte) tối đa
    std::string hint;            // chữ mờ khi rỗng
    int caret = 0;               // vị trí con trỏ (tính theo ký tự)
    float scroll_x = 0;
    bool reveal = false;         // ô mật khẩu: đang hiện chữ

    Field() {}
    Field(FieldKind k, int max, const std::string &goi_y = "") : kind(k), max_len(max), hint(goi_y) {}

    std::string utf8() const { return to_utf8(s); }
    void set(const std::string &v);
    void clear() { s.clear(); caret = 0; scroll_x = 0; }
    bool empty() const { return s.empty(); }
    long long number() const;                         // FK_MONEY / FK_DIGITS / FK_PHONE -> số (0 nếu rỗng)
    bool date(int &d, int &m, int &y) const;          // FK_DATE: đủ 8 chữ số -> dd, mm, yyyy
    void set_today();                                 // FK_DATE: điền ngày hôm nay
    std::string display() const;                      // chuỗi hiển thị (có "/" của ngày, dấu chấm tiền)
    bool end_only() const { return kind == FK_DATE || kind == FK_MONEY; }   // con trỏ luôn ở cuối
};

// Vẽ + xử lý nhập. Trả về true khi vừa nhấn Enter trong ô này.
bool field(Field &f, Rectangle r, const Font &font = F.r17);
bool is_focused(const Field &f);
void focus(Field *f);
void unfocus();
bool any_field_focused();

// ============================ ẢNH SẢN PHẨM ============================
int  anh_dem(const std::string &idsp);                                          // số ảnh hiện có (có nhớ đệm)
const Texture2D *anh_lay(const std::string &idsp, int stt, int rong_toi_da);    // nullptr nếu chưa nạp xong
void anh_xoa_dem(const std::string &idsp);                                      // sau khi thêm / xóa sản phẩm
void anh_giai_phong();
// Vẽ ảnh phủ kín khung (cắt giữa), bo góc bằng cách che 4 góc bằng màu nền `nen`
void ve_anh(const Texture2D &t, Rectangle dst, float bo_goc, Color nen, float zoom = 1.0f);
void ve_anh_trong(Rectangle dst, float bo_goc, const std::string &hang, const std::string &ten);   // khung mẫu khi chưa có ảnh

// ============================ THÔNG BÁO ============================
enum ToastKind { TOAST_INFO, TOAST_OK, TOAST_ERR };
void toast(const std::string &msg, ToastKind k = TOAST_INFO);
void draw_toasts();

// ============================ TIỆN ÍCH ============================
std::string tien(long long v);               // 1234567 -> "1.234.567₫"
std::string tien_gon(long long v);           // 12.300.000 -> "12,3tr"
std::string so_gon(long long v);             // 1250 -> "1,2k" (cho "Đã bán")
Color mau_theo_ten(const std::string &ten_mau, bool *biet = nullptr);   // "Đen" -> màu thật để vẽ chấm màu
Color lerp_c(Color a, Color b, float t);
float ease_out(float t);

}  // namespace ui
