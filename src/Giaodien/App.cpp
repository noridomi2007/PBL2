// VÒNG LẶP CHÍNH, ĐĂNG NHẬP / ĐĂNG KÝ, KHUNG HỘP THOẠI
#include "App.h"
#include <cmath>
using namespace ui;

App::App() {
    shop.doc_file();
    qk.doc_file();
    qn.doc_file();
    qn.khoi_tao_admin_neu_trong();
    bst.doc_file();

    f_sdt = Field(FK_PHONE, 10, "Số điện thoại");
    f_manv = Field(FK_TEXT, 8, "Mã nhân viên (vd: NV0001)");
    f_mk = Field(FK_PASSWORD, 25, "Mật khẩu");
    f_dk_ten = Field(FK_TEXT, RONG_KH_TEN, "Họ và tên");
    f_dk_sdt = Field(FK_PHONE, 10, "Số điện thoại (10 số)");
    f_dk_mail = Field(FK_EMAIL, RONG_KH_EMAIL, "Email (không bắt buộc)");
    f_dk_dc = Field(FK_TEXT, RONG_KH_DIACHI, "Địa chỉ (không bắt buộc)");
    f_dk_mk = Field(FK_PASSWORD, 25, "Mật khẩu (từ 6 ký tự)");
    f_dk_mk2 = Field(FK_PASSWORD, 25, "Nhập lại mật khẩu");

    f_tim = Field(FK_TEXT, 60, "Tìm áo, quần, giày, thương hiệu...");
    f_gia_min = Field(FK_MONEY, 9, "Từ");
    f_gia_max = Field(FK_MONEY, 9, "Đến");
    f_mk_cu = Field(FK_PASSWORD, 25, "Mật khẩu hiện tại");
    f_mk_moi = Field(FK_PASSWORD, 25, "Mật khẩu mới");
    f_mk_moi2 = Field(FK_PASSWORD, 25, "Nhập lại mật khẩu mới");

    f_pos_tim = Field(FK_TEXT, 40, "Tìm sản phẩm theo tên / mã...");
    f_pos_sdt = Field(FK_PHONE, 10, "SĐT khách hàng");
    f_pos_ngay = Field(FK_DATE, 8, "dd/mm/yyyy");
    f_pos_ngay.set_today();
    f_kho_tim = Field(FK_TEXT, 40, "Tìm theo tên / mã sản phẩm...");
    f_nk_mau = Field(FK_TEXT, RONG_MAU, "Màu");
    f_nk_size = Field(FK_TEXT, RONG_SIZE, "Size");
    f_nk_sl = Field(FK_DIGITS, 5, "Số lượng");
    f_sp_gia = Field(FK_MONEY, 11, "Giá bán");
    f_sp_bst = Field(FK_TEXT, RONG_BST_TEN, "Tên bộ sưu tập (để trống = không thuộc BST nào)");
    f_ap_hang_khac = Field(FK_TEXT, RONG_HANG, "Tên hãng");
    f_ap_ten = Field(FK_TEXT, RONG_TEN, "Tên sản phẩm");
    f_ap_kieu_ct = Field(FK_TEXT, RONG_KIEU, "Kiểu chi tiết (vd: Áo thun)");
    f_ap_gia = Field(FK_MONEY, 11, "Giá bán");
    f_ap_chat_lieu = Field(FK_TEXT, RONG_CHAT_LIEU, "Chất liệu");
    f_ap_bst = Field(FK_TEXT, RONG_BST_TEN, "Bộ sưu tập (không bắt buộc)");
    for (int i = 0; i < 6; i++) {
        f_ap_mau[i] = Field(FK_TEXT, RONG_MAU, "Màu");
        f_ap_size[i] = Field(FK_TEXT, RONG_SIZE, "Size");
        f_ap_sl[i] = Field(FK_DIGITS, 5, "SL");
    }
    f_kh_tim = Field(FK_TEXT, 30, "Tìm theo tên / số điện thoại...");
    f_nk_ten = Field(FK_TEXT, RONG_KH_TEN, "Họ và tên");
    f_nk_sdt = Field(FK_PHONE, 10, "Số điện thoại");
    f_nk_mail = Field(FK_EMAIL, RONG_KH_EMAIL, "Email (không bắt buộc)");
    f_nk_dc = Field(FK_TEXT, RONG_KH_DIACHI, "Địa chỉ (không bắt buộc)");
    f_nk_mk = Field(FK_PASSWORD, 25, "Mật khẩu (trống = dùng số điện thoại)");
    f_nv_ten = Field(FK_TEXT, RONG_NV_TEN, "Họ và tên nhân viên");
    f_nv_sdt = Field(FK_PHONE, 10, "Số điện thoại");
    f_dt_ngay = Field(FK_DATE, 8, "dd/mm/yyyy");
    f_dt_ngay.set_today();
    dt_khoa = khoa_ky_hien_tai(dt_ky);
    tc.sap_xep = SX_LIEN_QUAN;
    tinh_loc();
}

void App::cuon_cap_nhat(Rectangle vung) {
    float toi_da = cao_nd - vung.height;
    if (toi_da < 0) toi_da = 0;
    if (hover(vung)) cuon_dich -= C.wheel * 80.0f;
    if (cuon_dich < 0) cuon_dich = 0;
    if (cuon_dich > toi_da) cuon_dich = toi_da;
    cuon += (cuon_dich - cuon) * fminf(1.0f, C.dt * 16.0f);
    if (fabsf(cuon - cuon_dich) < 0.3f) cuon = cuon_dich;
}

void App::doi_man(Screen s) {
    screen = s;
    cuon = cuon_dich = 0;
    cao_nd = 0;
    unfocus();
    if (s == S_REVENUE) dt_anim0 = GetTime();
}

void App::dang_xuat() {
    role = R_GUEST; ma.clear(); ten.clear();
    gio.lam_rong(); gio_pos.lam_rong();
    pos_kh = -1; menu_mo = false; modal = M_NONE;
    f_mk.clear(); f_sdt.clear(); f_manv.clear();
    doi_man(S_HOME);
}

// ============================ VÒNG LẶP ============================
void App::run() {
    while (!WindowShouldClose()) {
        BeginDrawing();
        begin_frame();
        ClearBackground(BG);
        Modal modal_truoc = modal;
        C.blocked = (modal != M_NONE);
        switch (screen) {
            case S_LOGIN: ve_dang_nhap(); break;
            case S_REGISTER: ve_dang_ky(); break;
            case S_HOME: case S_DETAIL: case S_CART: case S_ACCOUNT: ve_cua_hang(); break;
            default: ve_khu_lam_viec(); break;
        }
        C.blocked = false;
        if (modal_truoc == M_NONE && modal != M_NONE) C.pressed = false;   // cú bấm mở hộp thoại không được tính là bấm ra ngoài để đóng
        if (modal != M_NONE) ve_hop_thoai();
        draw_toasts();
        end_frame();
        EndDrawing();
    }
}

// ============================ KHUNG HỘP THOẠI ============================
// Vẽ nền mờ + thẻ giữa màn hình. Trả về true nếu người dùng bấm dấu X / ngoài khung (đóng).
bool App::khung_hop_thoai(Rectangle &r, float w, float h, const std::string &tieu_de) {
    DrawRectangle(-4, -4, C.sw + 8, C.sh + 8, Color{ 27, 45, 66, 120 });
    if (h > C.sh - 40) h = (float)C.sh - 40;
    if (w > C.sw - 40) w = (float)C.sw - 40;
    r = Rectangle{ (C.sw - w) / 2, (C.sh - h) / 2, w, h };
    shadow(r, 20, 1.5f);
    rrect(r, 20, SURFACE);
    text(F.s22, tieu_de, r.x + 28, r.y + 22, INK);
    Rectangle x = { r.x + w - 52, r.y + 16, 36, 36 };
    bool dong = icon_button(x, IC_CLOSE, MUTED, SKY50, 18);
    hline(r.x + 28, r.y + 66, w - 56);
    if (C.pressed && !CheckCollisionPointRec(C.mouse, r)) dong = true;
    return dong;
}

void App::ve_hop_thoai() {
    switch (modal) {
        case M_INVOICE: ve_hd_modal(); break;
        case M_NEED_LOGIN: ve_can_dang_nhap(); break;
        case M_PICK_VARIANT: ve_chon_bien_the(); break;
        case M_ADD_PRODUCT: ve_them_sp_modal(); break;
        case M_STOCK_IN: ve_nhap_kho_modal(); break;
        case M_EDIT_PRODUCT: ve_sua_sp_modal(); break;
        case M_DELETE_PRODUCT: ve_xoa_sp_modal(); break;
        case M_NEW_CUSTOMER: ve_khach_moi_modal(); break;
        case M_NEW_STAFF: ve_nv_moi_modal(); break;
        case M_CUSTOMER_INFO: ve_khach_info_modal(); break;
        default: modal = M_NONE; break;
    }
}

void App::ve_can_dang_nhap() {
    Rectangle r;
    if (khung_hop_thoai(r, 440, 250, "Cần đăng nhập")) { modal = M_NONE; return; }
    text_wrap(F.r15, "Bạn cần đăng nhập tài khoản khách hàng để thêm sản phẩm vào giỏ và đặt hàng. Chưa có tài khoản? Đăng ký chỉ mất một phút.",
              r.x + 28, r.y + 86, r.width - 56, TEXT, 4, 22);
    if (button(Rectangle{ r.x + 28, r.y + 180, 180, 46 }, "Đăng nhập", BTN_PRIMARY)) { modal = M_NONE; quay_lai = screen; tab_dn = 0; doi_man(S_LOGIN); }
    if (button(Rectangle{ r.x + 224, r.y + 180, 188, 46 }, "Tạo tài khoản", BTN_OUTLINE)) { modal = M_NONE; quay_lai = screen; doi_man(S_REGISTER); }
}

// ============================ ĐĂNG NHẬP ============================
static const char *loi_dang_ky(KetQuaDangKy k) {
    switch (k) {
        case DK_OK: return "";
        case DK_THIEU_THONG_TIN: return "Vui lòng nhập họ tên và mật khẩu.";
        case DK_KY_TU_CAM: return "Không dùng ký tự | trong thông tin.";
        case DK_SDT_SAI: return "Số điện thoại phải đủ 10 số và bắt đầu bằng 0.";
        case DK_MAT_KHAU_NGAN: return "Mật khẩu quá ngắn (tối thiểu 6 ký tự).";
        case DK_SDT_TRUNG: return "Số điện thoại này đã có tài khoản.";
        case DK_LOI_GHI_FILE: return "Không ghi được file, thử lại sau.";
        case DK_QUA_DAI: return "Tên / email / địa chỉ quá dài.";
    }
    return "";
}

void App::dang_nhap_khach() {
    int i = qk.dang_nhap(f_sdt.utf8(), f_mk.utf8());
    if (i == -1) { thong_bao_dn = "Sai số điện thoại / mật khẩu, hoặc tài khoản đang bị khóa."; return; }
    role = R_CUSTOMER; ma = qk.lay(i).getMaKH(); ten = qk.lay(i).getTenKH();
    f_mk.clear(); thong_bao_dn.clear();
    toast("Xin chào " + ten + "!", TOAST_OK);
    doi_man(quay_lai == S_LOGIN || quay_lai == S_REGISTER ? S_HOME : quay_lai);
}

void App::dang_nhap_nv() {
    int i = qn.dang_nhap(f_manv.utf8(), f_mk.utf8());
    if (i == -1) { thong_bao_dn = "Sai mã / mật khẩu, hoặc nhân viên đã nghỉ việc."; return; }
    role = qn.lay(i).laAdmin() ? R_ADMIN : R_STAFF;
    ma = qn.lay(i).getMaNV(); ten = qn.lay(i).getTenNV();
    f_mk.clear(); thong_bao_dn.clear();
    toast("Đăng nhập thành công: " + ten, TOAST_OK);
    doi_man(role == R_ADMIN ? S_REVENUE : S_POS);
}

void App::ve_dang_nhap() {
    float W = (float)C.sw, H = (float)C.sh;
    bool tren = (tab_dn == 1);
    // nửa trái: thương hiệu
    float lw = W * 0.46f;
    if (lw < 360) lw = 360;
    Color c1 = tren ? Color{ 205, 236, 226, 255 } : SKY200, c2 = tren ? Color{ 236, 250, 245, 255 } : SKY50;
    DrawRectangleGradientEx(Rectangle{ 0, 0, lw, H }, c1, c2, c2, c1);
    push_clip(Rectangle{ 0, 0, lw, H });
    DrawCircleV(Vector2{ lw * 0.2f, H * 0.82f }, 190, Fade(WHITE, 0.35f));
    DrawCircleV(Vector2{ lw * 0.86f, H * 0.18f }, 120, Fade(WHITE, 0.4f));
    pop_clip();
    text(F.s40, TEN_SHOP, 56, 56, INK, 4);
    text(F.r17, KHAU_HIEU, 58, 108, TEXT);
    const char *dong1 = tren ? "Cổng nội bộ" : "Chào mừng quay lại";
    text(F.s40, dong1, 56, H * 0.42f, INK);
    text_wrap(F.r17, tren ? "Dành cho nhân viên và quản trị viên của cửa hàng. Khách hàng vui lòng đăng nhập ở tab Khách hàng."
                          : "Đăng nhập để theo dõi đơn hàng, nhận ưu đãi theo hạng thành viên và thanh toán nhanh hơn.",
              58, H * 0.42f + 64, lw - 120, TEXT, 4, 26);
    if (!tren && button(Rectangle{ 56, H - 92, 200, 44 }, "Xem cửa hàng", BTN_OUTLINE)) { doi_man(S_HOME); return; }

    // nửa phải: form
    float fw = 400, fx = lw + (W - lw - fw) / 2, fy = H / 2 - 250;
    if (fy < 30) fy = 30;
    Rectangle tabs = { fx, fy, fw, 46 };
    rrect(tabs, 23, SKY100);
    Rectangle t1 = { tabs.x + 4, tabs.y + 4, fw / 2 - 4, 38 }, t2 = { tabs.x + fw / 2, tabs.y + 4, fw / 2 - 4, 38 };
    if (tab_dn == 0) rrect(t1, 19, SURFACE); else rrect(t2, 19, SURFACE);
    if (hover(t1)) C.hand = true;
    if (hover(t2)) C.hand = true;
    text_c(F.m15, "Khách hàng", t1, tab_dn == 0 ? SKY600 : MUTED);
    text_c(F.m15, "Nhân viên / Admin", t2, tab_dn == 1 ? Color{ 40, 150, 110, 255 } : MUTED);
    if (C.pressed && hover(t1) && tab_dn != 0) { tab_dn = 0; thong_bao_dn.clear(); f_mk.clear(); unfocus(); }
    if (C.pressed && hover(t2) && tab_dn != 1) { tab_dn = 1; thong_bao_dn.clear(); f_mk.clear(); unfocus(); }

    float y = fy + 84;
    text(F.s30, tab_dn == 0 ? "Đăng nhập" : "Đăng nhập nội bộ", fx, y, INK);
    y += 56;
    bool vao = false;
    text(F.m13, tab_dn == 0 ? "SỐ ĐIỆN THOẠI" : "MÃ NHÂN VIÊN", fx, y, MUTED, 1.2f);
    if (field(tab_dn == 0 ? f_sdt : f_manv, Rectangle{ fx, y + 22, fw, 48 })) vao = true;
    y += 86;
    text(F.m13, "MẬT KHẨU", fx, y, MUTED, 1.2f);
    if (field(f_mk, Rectangle{ fx, y + 22, fw, 48 })) vao = true;
    y += 90;
    if (!thong_bao_dn.empty()) {
        Rectangle e = { fx, y - 10, fw, 40 };
        rrect(e, 10, DANGBG);
        text_l(F.r13, fit(F.r13, thong_bao_dn, fw - 24), e, DANGER, 12);
        y += 40;
    }
    Rectangle nut = { fx, y, fw, 50 };
    if (button(nut, "ĐĂNG NHẬP", tab_dn == 0 ? BTN_PRIMARY : BTN_DARK, F.m15, true, 12, 1.5f) || vao) {
        if (tab_dn == 0) dang_nhap_khach(); else dang_nhap_nv();
    }
    y += 70;
    if (tab_dn == 0) {
        text(F.r15, "Chưa có tài khoản?", fx + 74, y, MUTED);
        Rectangle lk = { fx + 74 + tw(F.r15, "Chưa có tài khoản? "), y - 4, 90, 26 };
        if (hover(lk)) { C.hand = true; if (C.pressed) { thong_bao_dn.clear(); doi_man(S_REGISTER); } }
        text(F.m15, "Đăng ký", lk.x, y, hover(lk) ? SKY600 : SKY500);
    } else {
        text_c(F.r13, "Quên mật khẩu? Liên hệ quản trị viên để được cấp lại.", Rectangle{ fx, y - 4, fw, 20 }, MUTED);
    }
}

void App::ve_dang_ky() {
    float W = (float)C.sw, H = (float)C.sh;
    float lw = W * 0.38f;
    if (lw < 320) lw = 320;
    DrawRectangleGradientEx(Rectangle{ 0, 0, lw, H }, SKY200, SKY50, SKY50, SKY200);
    DrawCircleV(Vector2{ lw * 0.3f, H * 0.78f }, 170, Fade(WHITE, 0.35f));
    text(F.s40, TEN_SHOP, 56, 56, INK, 4);
    text(F.s30, "Tạo tài khoản", 56, H * 0.4f, INK);
    text_wrap(F.r17, "Tích lũy chi tiêu để lên hạng Bạc, Vàng, Kim cương và nhận mức giảm giá cao hơn cho mỗi đơn hàng.", 58, H * 0.4f + 50, lw - 110, TEXT, 5, 26);

    float fw = 440, fx = lw + (W - lw - fw) / 2, y = fmaxf(24.0f, H / 2 - 330);
    bool gui = false;
    text(F.s30, "Thông tin của bạn", fx, y, INK);
    y += 54;
    struct Dong { const char *nhan; Field *f; };
    Dong ds[] = { { "HỌ VÀ TÊN", &f_dk_ten }, { "SỐ ĐIỆN THOẠI (DÙNG ĐỂ ĐĂNG NHẬP)", &f_dk_sdt }, { "EMAIL", &f_dk_mail },
                  { "ĐỊA CHỈ", &f_dk_dc }, { "MẬT KHẨU", &f_dk_mk }, { "NHẬP LẠI MẬT KHẨU", &f_dk_mk2 } };
    for (Dong &d : ds) {
        text(F.m13, d.nhan, fx, y, MUTED, 1.0f);
        if (field(*d.f, Rectangle{ fx, y + 20, fw, 44 })) gui = true;
        y += 76;
    }
    if (!thong_bao_dn.empty()) {
        Rectangle e = { fx, y - 6, fw, 36 };
        rrect(e, 10, DANGBG);
        text_l(F.r13, fit(F.r13, thong_bao_dn, fw - 24), e, DANGER, 12);
        y += 42;
    }
    if (button(Rectangle{ fx, y, fw, 50 }, "TẠO TÀI KHOẢN", BTN_PRIMARY, F.m15, true, 12, 1.5f) || gui) {
        if (f_dk_mk.utf8() != f_dk_mk2.utf8()) thong_bao_dn = "Hai ô mật khẩu chưa giống nhau.";
        else {
            std::string m;
            KetQuaDangKy r = qk.dang_ky(f_dk_ten.utf8(), f_dk_sdt.utf8(), f_dk_mk.utf8(), f_dk_mail.utf8(), f_dk_dc.utf8(), &m);
            if (r != DK_OK) thong_bao_dn = loi_dang_ky(r);
            else {
                toast("Đăng ký thành công! Mã khách của bạn: " + m, TOAST_OK);
                f_sdt.set(f_dk_sdt.utf8()); f_mk.clear();
                f_dk_ten.clear(); f_dk_sdt.clear(); f_dk_mail.clear(); f_dk_dc.clear(); f_dk_mk.clear(); f_dk_mk2.clear();
                thong_bao_dn.clear(); tab_dn = 0; doi_man(S_LOGIN);
            }
        }
    }
    y += 64;
    Rectangle lk = { fx, y, fw, 28 };
    if (hover(lk)) { C.hand = true; if (C.pressed) { thong_bao_dn.clear(); doi_man(S_LOGIN); } }
    text_c(F.m15, "Đã có tài khoản? Đăng nhập", lk, hover(lk) ? SKY600 : SKY500);
}
