// KHU LÀM VIỆC CỦA NHÂN VIÊN / ADMIN: thanh bên, bán tại quầy, khách hàng, kho, nhân viên + các hộp thoại
#include "App.h"
#include "../Dialog/Dialog.h"
#include <cmath>
using namespace ui;

static const float SB_W = 248;
static const float TB_H = 76;

std::vector<NavItem> App::menu_lam_viec() const {
    std::vector<NavItem> m;
    if (role == R_ADMIN) {
        m.push_back({ S_REVENUE, "Doanh thu", IC_CHART });
        m.push_back({ S_POS, "Bán tại quầy", IC_POS });
        m.push_back({ S_PRODUCTS, "Sản phẩm & kho", IC_BOX });
        m.push_back({ S_CUSTOMERS, "Khách hàng", IC_USERS });
        m.push_back({ S_STAFFS, "Nhân viên", IC_TAG });
    } else {
        m.push_back({ S_POS, "Bán tại quầy", IC_POS });
        m.push_back({ S_CUSTOMERS, "Khách hàng", IC_USERS });
        m.push_back({ S_STOCK, "Tồn kho (chỉ xem)", IC_BOX });
        m.push_back({ S_MYSALES, "Doanh số của tôi", IC_CHART });
    }
    return m;
}

std::string App::tieu_de_man() const {
    switch (screen) {
        case S_REVENUE: return "Doanh thu";
        case S_POS: return "Bán tại quầy";
        case S_PRODUCTS: return "Sản phẩm & kho";
        case S_STOCK: return "Tồn kho";
        case S_CUSTOMERS: return "Khách hàng";
        case S_STAFFS: return "Nhân viên";
        case S_MYSALES: return "Doanh số của tôi";
        case S_MATKHAU_NV: return "Đổi mật khẩu";
        default: return "";
    }
}

// ============================ KHUNG ============================
void App::ve_khu_lam_viec() {
    if (role != R_STAFF && role != R_ADMIN) { doi_man(S_LOGIN); return; }
    bool chi_admin = (screen == S_REVENUE || screen == S_PRODUCTS || screen == S_STAFFS);
    if (role == R_STAFF && chi_admin) { doi_man(S_POS); return; }
    if (role == R_ADMIN && (screen == S_STOCK || screen == S_MYSALES)) { doi_man(S_REVENUE); return; }

    float W = (float)C.sw, H = (float)C.sh;
    Rectangle view = { SB_W, TB_H, W - SB_W, H - TB_H };
    cuon_cap_nhat(view);
    push_clip(view);
    switch (screen) {
        case S_POS: ve_pos(view); break;
        case S_CUSTOMERS: ve_khach_hang(view); break;
        case S_STOCK: ve_kho(view, false); break;
        case S_PRODUCTS: ve_kho(view, true); break;
        case S_MYSALES: ve_doanh_so_cua_toi(view); break;
        case S_STAFFS: ve_nhan_vien(view); break;
        case S_REVENUE: ve_doanh_thu(view); break;
        case S_MATKHAU_NV: ve_mat_khau_nv(view); break;
        default: break;
    }
    pop_clip();
    scroll_bar(view, cao_nd, cuon);

    // thanh trên
    DrawRectangle((int)SB_W, 0, (int)(W - SB_W), (int)TB_H, SURFACE);
    hline(SB_W, TB_H - 1, W - SB_W);
    text(F.s30, tieu_de_man(), SB_W + 32, 22, INK);
    bool ad = role == R_ADMIN;
    std::string vt = ad ? "Quản trị viên" : "Nhân viên";
    float bw = tw(F.m13, vt) + 24, nw = tw(F.m15, fit(F.m15, ten, 220));
    float rx = W - 32;
    Rectangle tg = { rx - bw, 26, bw, 26 };
    tag(tg, vt, ad ? Color{ 110, 100, 200, 255 } : Color{ 40, 150, 110, 255 }, ad ? LAVBG : MINTBG, F.m13);
    text_r(F.m15, fit(F.m15, ten, 220), Rectangle{ rx - bw - 16 - nw, 20, nw, 38 }, INK, 0);
    ve_thanh_ben();
}

void App::ve_thanh_ben() {
    float H = (float)C.sh;
    bool ad = role == R_ADMIN;
    Color a1 = ad ? Color{ 226, 222, 250, 255 } : Color{ 205, 236, 226, 255 };
    DrawRectangleGradientV(0, 0, (int)SB_W, (int)H, SKY100, SKY50);
    DrawRectangle((int)SB_W - 1, 0, 1, (int)H, LINE);
    text(F.s22, TEN_SHOP, 28, 26, INK, 2.5f);
    Rectangle bd = { 24, 66, SB_W - 48, 30 };
    rrect(bd, 15, a1);
    text_c(F.m13, ad ? "KHU QUẢN TRỊ" : "KHU NHÂN VIÊN", bd, ad ? Color{ 100, 90, 190, 255 } : Color{ 36, 130, 98, 255 }, 1.6f);
    std::vector<NavItem> m = menu_lam_viec();
    float y = 124;
    for (const NavItem &n : m) {
        Rectangle r = { 14, y, SB_W - 28, 48 };
        bool chon = screen == n.s, h = hover(r);
        if (h) C.hand = true;
        if (chon) { shadow(r, 14, 0.6f); rrect(r, 14, SURFACE); }
        else if (h) rrect(r, 14, Fade(WHITE, 0.6f));
        icon(n.ic, r.x + 28, r.y + 24, 22, chon ? SKY600 : TEXT, 1.8f);
        text_l(chon ? F.m15 : F.r15, n.label, Rectangle{ r.x + 54, r.y, r.width - 54, r.height }, chon ? INK : TEXT);
        if (h && C.pressed && !chon) doi_man(n.s);
        y += 54;
    }
    // chân thanh bên
    float by = H - 120;
    Rectangle r1 = { 14, by, SB_W - 28, 44 }, r2 = { 14, by + 52, SB_W - 28, 44 };
    bool h1 = hover(r1), h2 = hover(r2);
    if (h1) { C.hand = true; rrect(r1, 12, Fade(WHITE, 0.7f)); }
    if (h2) { C.hand = true; rrect(r2, 12, DANGBG); }
    icon(IC_LOCK, r1.x + 28, r1.y + 22, 18, screen == S_MATKHAU_NV ? SKY600 : TEXT, 1.7f);
    text_l(F.r15, "Đổi mật khẩu", Rectangle{ r1.x + 54, r1.y, r1.width - 54, r1.height }, screen == S_MATKHAU_NV ? SKY600 : TEXT);
    icon(IC_LOGOUT, r2.x + 28, r2.y + 22, 18, h2 ? DANGER : TEXT, 1.7f);
    text_l(F.r15, "Đăng xuất", Rectangle{ r2.x + 54, r2.y, r2.width - 54, r2.height }, h2 ? DANGER : TEXT);
    if (h1 && C.pressed) doi_man(S_MATKHAU_NV);
    if (h2 && C.pressed) dang_xuat();
}

void App::ve_mat_khau_nv(Rectangle view) {
    float x = view.x + 32, y = view.y - cuon + 32;
    Rectangle pr = { x, y, 520, 400 };
    panel(pr, 20);
    float fy = pr.y + 32;
    bool gui = false;
    text(F.m13, "MẬT KHẨU HIỆN TẠI", pr.x + 28, fy, MUTED, 1.2f); if (field(f_pw_cu, Rectangle{ pr.x + 28, fy + 20, 464, 46 })) gui = true; fy += 82;
    text(F.m13, "MẬT KHẨU MỚI", pr.x + 28, fy, MUTED, 1.2f); if (field(f_pw_moi, Rectangle{ pr.x + 28, fy + 20, 464, 46 })) gui = true; fy += 82;
    text(F.m13, "NHẬP LẠI MẬT KHẨU MỚI", pr.x + 28, fy, MUTED, 1.2f); if (field(f_pw_moi2, Rectangle{ pr.x + 28, fy + 20, 464, 46 })) gui = true; fy += 88;
    if (button(Rectangle{ pr.x + 28, fy, 200, 48 }, "Cập nhật", BTN_PRIMARY, F.m15, true, 24) || gui) {
        if (f_pw_moi.utf8() != f_pw_moi2.utf8()) toast("Hai ô mật khẩu mới chưa giống nhau.", TOAST_ERR);
        else if (qn.doi_mat_khau(ma, f_pw_cu.utf8(), f_pw_moi.utf8())) { toast("Đã đổi mật khẩu.", TOAST_OK); f_pw_cu.clear(); f_pw_moi.clear(); f_pw_moi2.clear(); }
        else toast("Mật khẩu hiện tại sai hoặc mật khẩu mới quá ngắn (tối thiểu 6 ký tự).", TOAST_ERR);
    }
    cao_nd = 460;
}

// ============================ BÁN TẠI QUẦY ============================
void App::ve_pos(Rectangle view) {
    float x = view.x + 32, y0 = view.y - cuon + 28;
    float pw = view.width - 64;
    float rw = 410, lw = pw - rw - 24;
    if (pw < 760) { rw = 360; lw = pw - rw - 24; }

    // ---------- trái: tìm sản phẩm ----------
    field(f_pos_tim, Rectangle{ x, y0, lw, 48 });
    float y = y0 + 62;
    VECTOR<std::string> loai = shop.lay_ds_subtype("");
    float cx = x;
    if (chip(Rectangle{ cx, y, 70, 34 }, "Tất cả", pos_loai.empty(), F.m13)) pos_loai.clear();
    cx += 78;
    for (int i = 0; i < loai.size(); i++) {
        float w = tw(F.m13, loai[i]) + 30;
        if (cx + w > x + lw) { cx = x; y += 42; }
        if (chip(Rectangle{ cx, y, w, 34 }, loai[i], pos_loai == loai[i], F.m13)) pos_loai = loai[i];
        cx += w + 8;
    }
    y += 52;
    std::string q = f_pos_tim.utf8();
    if (q != pos_q_cu || pos_loai != pos_loai_cu || pos_dirty) {
        TieuChiLoc t; t.tu_khoa = q; t.subtype = pos_loai;
        pos_kq = loc_cua_hang(shop, bst, t);
        pos_q_cu = q; pos_loai_cu = pos_loai; pos_dirty = false;
    }
    for (int i = 0; i < pos_kq.size(); i++) {
        product *p = pos_kq[i].sp;
        Rectangle r = { x, y, lw, 84 };
        if (r.y + r.height >= view.y && r.y <= C.sh) {
            bool h = hover(r);
            panel(r, 16, true);
            if (h) rrect_line(r, 16, 1.6f, SKY300);
            Rectangle ir = { r.x + 10, r.y + 10, 52, 64 };
            const Texture2D *t = anh_dem(p->getid()) > 0 ? anh_lay(p->getid(), 1, 200) : nullptr;
            if (t) ve_anh(*t, ir, 8, SURFACE); else ve_anh_trong(ir, 8, p->getbrand(), "");
            text(F.m15, fit(F.m15, p->getname(), lw - 280), r.x + 76, r.y + 14, INK);
            text(F.r13, p->getid() + "   -   " + p->getbrand(), r.x + 76, r.y + 38, MUTED);
            int ton = p->getsl();
            text(F.r13, ton > 0 ? "Tồn: " + std::to_string(ton) : "Hết hàng", r.x + 76, r.y + 58, ton > 0 ? (ton <= 5 ? WARNC : OKC) : DANGER);
            text_r(F.b17, tien(p->getprice()), Rectangle{ r.x, r.y + 14, r.width - 130, 24 }, SKY600, 0);
            if (button(Rectangle{ r.x + r.width - 112, r.y + 22, 96, 40 }, "Chọn", BTN_SOFT, F.m15, ton > 0, 20)) {
                pk_ma = p->getid(); pk_sl = 1; pk_mau.clear(); pk_size.clear();
                for (int k = 0; k < p->so_variant(); k++) if (p->get_variant(k).sl > 0) { pk_mau = p->get_variant(k).color; pk_size = p->get_variant(k).size; break; }
                modal = M_PICK_VARIANT;
            }
        }
        y += 94;
    }
    if (pos_kq.size() == 0) text_c(F.r15, "Không có sản phẩm phù hợp.", Rectangle{ x, y + 10, lw, 30 }, MUTED);
    float y_trai = y + 30;

    // ---------- phải: đơn hàng ----------
    float rx = x + lw + 24, ry = y0;
    Rectangle kh_r = { rx, ry, rw, pos_kh >= 0 ? 150.0f : 150.0f };
    panel(kh_r, 20);
    text(F.m13, "KHÁCH HÀNG", rx + 22, ry + 20, MUTED, 1.6f);
    long long chi = 0;
    if (pos_kh >= 0 && pos_kh < qk.size()) {
        const KhachHang &k = qk.lay(pos_kh);
        chi = shop.lay_doanh_thu().tong_chi_tieu_khach(k.getMaKH());
        const HangTV &h = lay_hang(chi);
        text(F.b17, fit(F.b17, k.getTenKH(), rw - 150), rx + 22, ry + 46, INK);
        text(F.r13, k.getMaKH() + "  -  " + k.getSoDienThoai(), rx + 22, ry + 74, MUTED);
        tag_auto(rx + 22, ry + 100, 26, std::string("Hạng ") + h.ten + "  -" + std::to_string(h.phan_tram) + "%", SKY600, SKY100, F.m13);
        if (button(Rectangle{ rx + rw - 96, ry + 14, 76, 30 }, "Đổi", BTN_SOFT, F.m13, true, 15)) pos_kh = -1;
    } else {
        bool tim = field(f_pos_sdt, Rectangle{ rx + 22, ry + 46, rw - 130, 46 });
        if (button(Rectangle{ rx + rw - 96, ry + 46, 76, 46 }, "Tìm", BTN_PRIMARY, F.m15, true, 12) || tim) {
            int i = qk.tim_theo_sdt(f_pos_sdt.utf8());
            if (i == -1) toast("Chưa có khách với số này. Bấm \"Đăng ký khách mới\".", TOAST_ERR);
            else if (!qk.lay(i).getDangHoatDong()) toast("Tài khoản của khách này đang bị khóa.", TOAST_ERR);
            else { pos_kh = i; }
        }
        if (button(Rectangle{ rx + 22, ry + 102, rw - 44, 36 }, "+ Đăng ký khách mới", BTN_SOFT, F.m13, true, 18)) { modal = M_NEW_CUSTOMER; }
    }
    ry += 150 + 16;

    // ngày bán
    Rectangle ng = { rx, ry, rw, 92 };
    panel(ng, 20);
    text(F.m13, "NGÀY BÁN", rx + 22, ry + 18, MUTED, 1.6f);
    field(f_pos_ngay, Rectangle{ rx + 22, ry + 38, rw - 150, 44 });
    if (button(Rectangle{ rx + rw - 118, ry + 38, 96, 44 }, "Hôm nay", BTN_SOFT, F.m13, true, 12)) f_pos_ngay.set_today();
    ry += 92 + 16;

    // giỏ
    float cao_gio = 70 + (gio_pos.empty() ? 70.0f : gio_pos.so_dong() * 72.0f) + 200;
    Rectangle gr = { rx, ry, rw, cao_gio };
    panel(gr, 20);
    text(F.m13, "ĐƠN HÀNG", rx + 22, ry + 20, MUTED, 1.6f);
    const HangTV &h = lay_hang(chi);
    long long tam = 0, giam = 0;
    float gy = ry + 48;
    if (gio_pos.empty()) text_c(F.r15, "Chưa có sản phẩm nào.", Rectangle{ rx, gy + 10, rw, 30 }, MUTED);
    int xoa_i = -1;
    for (int i = 0; i < gio_pos.so_dong(); i++) {
        DongGio d = gio_pos.lay(i);
        int vt = shop.find(d.idsp);
        if (vt == -1) { xoa_i = i; continue; }
        product *p = shop.take(vt);
        if (i > 0) hline(rx + 22, gy, rw - 44);
        text(F.m15, fit(F.m15, p->getname(), rw - 150), rx + 22, gy + 8, INK);
        text(F.r13, d.color + " / " + d.size, rx + 22, gy + 30, MUTED);
        int ton = p->getsl(d.color, d.size), sl = d.sl;
        Rectangle sp = { rx + rw - 170, gy + 8, 100, 32 };
        if (stepper(sp, sl, 1, ton > 0 ? ton : 1)) gio_pos.doi_sl(i, sl);
        long long dt = p->getprice() * d.sl, dg = tinh_giam_gia_dong(h, p->getprice(), d.sl);
        tam += dt; giam += dg;
        text_r(F.m15, tien(dt), Rectangle{ rx, gy + 46, rw, 22 }, INK, 22);
        if (icon_button(Rectangle{ rx + rw - 56, gy + 6, 36, 36 }, IC_TRASH, MUTED, DANGBG, 18)) xoa_i = i;
        gy += 72;
    }
    if (xoa_i >= 0) gio_pos.xoa(xoa_i);
    gy = ry + 48 + (gio_pos.empty() ? 70.0f : gio_pos.so_dong() * 72.0f);
    hline(rx + 22, gy, rw - 44);
    gy += 14;
    text(F.r15, "Tạm tính", rx + 22, gy, TEXT); text_r(F.m15, tien(tam), Rectangle{ rx, gy - 2, rw, 22 }, INK, 22); gy += 28;
    text(F.r15, std::string("Giảm hạng ") + h.ten, rx + 22, gy, TEXT); text_r(F.m15, giam > 0 ? "-" + tien(giam) : "0₫", Rectangle{ rx, gy - 2, rw, 22 }, giam > 0 ? OKC : MUTED, 22); gy += 34;
    text(F.m17, "Khách phải trả", rx + 22, gy + 2, INK); text_r(F.b21, tien(tam - giam), Rectangle{ rx, gy - 2, rw, 28 }, SKY600, 22);
    gy += 46;
    if (button(Rectangle{ rx + 22, gy, rw - 44, 52 }, "THANH TOÁN", BTN_PRIMARY, F.m17, !gio_pos.empty(), 26, 2.0f)) {
        int d, m, yy;
        if (pos_kh < 0) toast("Hãy chọn khách hàng (nhập số điện thoại) trước khi thanh toán.", TOAST_ERR);
        else if (!f_pos_ngay.date(d, m, yy)) toast("Nhập đủ ngày bán dạng dd/mm/yyyy.", TOAST_ERR);
        else {
            int dl = -1; HoaDon hd;
            KetQuaThanhToan r = shop.thanh_toan(gio_pos, qk.lay(pos_kh).getMaKH(), ma, d, m, yy, hd, &dl);
            if (r == TT_OK) { hd_cuoi = hd; gio_pos.lam_rong(); pos_dirty = true; modal = M_INVOICE; }
            else if (r == TT_NGAY_SAI) toast("Ngày bán không có thật hoặc sau ngày hôm nay.", TOAST_ERR);
            else if (r == TT_THIEU_HANG) toast("Không đủ hàng ở dòng " + std::to_string(dl + 1) + " của đơn.", TOAST_ERR);
            else toast("Không thanh toán được (mã lỗi " + std::to_string((int)r) + ").", TOAST_ERR);
        }
    }
    float y_phai = ry + cao_gio + 30;
    cao_nd = fmaxf(y_trai, y_phai) - (view.y - cuon);
}

// ============================ KHÁCH HÀNG ============================
void App::ve_khach_hang(Rectangle view) {
    float x = view.x + 32, w = view.width - 64, y = view.y - cuon + 28;
    field(f_kh_tim, Rectangle{ x, y, fminf(440.0f, w - 220), 48 });
    if (button(Rectangle{ x + w - 190, y, 190, 48 }, "+ Đăng ký khách", BTN_PRIMARY, F.m15, true, 24)) modal = M_NEW_CUSTOMER;
    y += 66;
    std::string q = normalize(f_kh_tim.utf8().c_str());
    // tiêu đề bảng
    Rectangle th = { x, y, w, 40 };
    rrect(th, 12, SKY100);
    float c1 = x + 20, c2 = x + 110, c3 = x + w * 0.36f, c4 = x + w * 0.52f, c5 = x + w * 0.68f;
    text_l(F.m13, "MÃ", Rectangle{ c1, th.y, 80, 40 }, SKY600, 0, 1.2f);
    text_l(F.m13, "KHÁCH HÀNG", Rectangle{ c2, th.y, 200, 40 }, SKY600, 0, 1.2f);
    text_l(F.m13, "ĐIỆN THOẠI", Rectangle{ c3, th.y, 120, 40 }, SKY600, 0, 1.2f);
    text_l(F.m13, "HẠNG", Rectangle{ c4, th.y, 100, 40 }, SKY600, 0, 1.2f);
    text_l(F.m13, "CHI TIÊU", Rectangle{ c5, th.y, 120, 40 }, SKY600, 0, 1.2f);
    y += 48;
    int n = 0;
    for (int i = qk.size() - 1; i >= 0; i--) {
        const KhachHang &k = qk.lay(i);
        if (!q.empty() && normalize(k.getTenKH().c_str()).find(q) == std::string::npos && k.getSoDienThoai().find(q) == std::string::npos && normalize(k.getMaKH().c_str()).find(q) == std::string::npos) continue;
        n++;
        Rectangle r = { x, y, w, 64 };
        if (r.y + r.height >= view.y && r.y <= C.sh) {
            long long chi = shop.lay_doanh_thu().tong_chi_tieu_khach(k.getMaKH());
            const HangTV &hg = lay_hang(chi);
            bool hv = hover(Rectangle{ r.x, r.y, r.width - (role == R_ADMIN ? 250.0f : 0.0f), r.height });
            panel(r, 14, true);
            if (hv) { C.hand = true; rrect_line(r, 14, 1.5f, SKY300); if (C.pressed) { kh_xem = k.getMaKH(); modal = M_CUSTOMER_INFO; } }
            text_l(F.m15, k.getMaKH(), Rectangle{ c1, r.y, 80, 64 }, TEXT);
            text_l(F.m15, fit(F.m15, k.getTenKH(), c3 - c2 - 16), Rectangle{ c2, r.y, c3 - c2, 64 }, INK);
            text_l(F.r15, k.getSoDienThoai(), Rectangle{ c3, r.y, 120, 64 }, TEXT);
            tag_auto(c4, r.y + 19, 26, hg.ten, SKY600, SKY100, F.m13);
            text_l(F.m15, tien(chi), Rectangle{ c5, r.y, 140, 64 }, INK);
            if (!k.getDangHoatDong()) tag_auto(c5 + 120, r.y + 19, 26, "Đã khóa", DANGER, DANGBG, F.m13);
            if (role == R_ADMIN) {
                if (button(Rectangle{ r.x + r.width - 240, r.y + 14, 112, 36 }, "Cấp lại MK", BTN_SOFT, F.m13, true, 18)) {
                    if (qk.dat_lai_mat_khau(k.getMaKH())) toast("Đã cấp lại: mật khẩu = số điện thoại của khách.", TOAST_OK);
                }
                if (button(Rectangle{ r.x + r.width - 120, r.y + 14, 104, 36 }, k.getDangHoatDong() ? "Khóa" : "Mở khóa", k.getDangHoatDong() ? BTN_OUTLINE : BTN_PRIMARY, F.m13, true, 18)) {
                    qk.dat_trang_thai(k.getMaKH(), !k.getDangHoatDong());
                }
            }
        }
        y += 72;
    }
    if (n == 0) text_c(F.r15, "Không có khách hàng nào phù hợp.", Rectangle{ x, y + 10, w, 30 }, MUTED);
    cao_nd = y + 40 - (view.y - cuon);
}

// ============================ KHO / SẢN PHẨM ============================
void App::ve_kho(Rectangle view, bool admin) {
    float x = view.x + 32, w = view.width - 64, y = view.y - cuon + 28;
    field(f_kho_tim, Rectangle{ x, y, fminf(440.0f, w - 240), 48 });
    if (admin && button(Rectangle{ x + w - 210, y, 210, 48 }, "+ Thêm sản phẩm", BTN_PRIMARY, F.m15, true, 24)) mo_them_sp();
    y += 66;

    // thống kê nhanh
    int tong_sp = shop.size(), het = 0, sap = 0;
    long long tong_cai = 0;
    for (int i = 0; i < shop.size(); i++) { int t = shop.take(i)->getsl(); tong_cai += t; if (t <= 0) het++; else if (t <= 5) sap++; }
    struct Tk { const char *nhan; std::string gt; Color c; };
    Tk tk[] = { { "SẢN PHẨM", std::to_string(tong_sp), INK }, { "TỔNG TỒN KHO", std::to_string(tong_cai), INK },
                { "SẮP HẾT (<= 5)", std::to_string(sap), sap ? WARNC : INK }, { "HẾT HÀNG", std::to_string(het), het ? DANGER : INK } };
    float kw = (w - 3 * 16) / 4;
    for (int i = 0; i < 4; i++) {
        Rectangle r = { x + i * (kw + 16), y, kw, 84 };
        panel(r, 16);
        text(F.m13, tk[i].nhan, r.x + 20, r.y + 16, MUTED, 1.4f);
        text(F.b27, tk[i].gt, r.x + 20, r.y + 40, tk[i].c);
    }
    y += 84 + 22;

    TieuChiLoc t; t.tu_khoa = f_kho_tim.utf8();
    VECTOR<KetQuaLoc> ds = loc_cua_hang(shop, bst, t);
    for (int i = 0; i < ds.size(); i++) {
        product *p = ds[i].sp;
        bool mo = kho_mo == p->getid();
        int nvar = p->so_variant();
        float cao_mo = mo ? 70.0f + ((nvar + 3) / 4) * 46.0f + (admin ? 62.0f : 0.0f) : 0;
        Rectangle r = { x, y, w, 88 + cao_mo };
        if (r.y + r.height >= view.y && r.y <= C.sh) {
            panel(r, 16);
            Rectangle dau = { r.x, r.y, r.width, 88 };
            bool hv = hover(dau);
            if (hv) { C.hand = true; if (C.pressed) kho_mo = mo ? "" : p->getid(); }
            Rectangle ir = { r.x + 12, r.y + 12, 56, 64 };
            const Texture2D *tx = anh_dem(p->getid()) > 0 ? anh_lay(p->getid(), 1, 200) : nullptr;
            if (tx) ve_anh(*tx, ir, 8, SURFACE); else ve_anh_trong(ir, 8, p->getbrand(), "");
            float c2 = r.x + w * 0.46f, c3 = r.x + w * 0.60f, c4 = r.x + w * 0.72f, c5 = r.x + w * 0.80f;
            text(F.m15, fit(F.m15, p->getname(), c2 - r.x - 100), r.x + 84, r.y + 20, INK);
            text(F.r13, p->getid() + "   -   " + p->getbrand() + "   -   " + p->getsubtype(), r.x + 84, r.y + 46, MUTED);
            text_l(F.m15, tien(p->getprice()), Rectangle{ c2, r.y, 140, 88 }, INK);
            int ton = ds[i].ton;
            tag_auto(c3, r.y + 31, 26, ton <= 0 ? "Hết hàng" : "Tồn " + std::to_string(ton), ton <= 0 ? DANGER : (ton <= 5 ? WARNC : Color{ 40, 130, 96, 255 }), ton <= 0 ? DANGBG : (ton <= 5 ? WARNBG : OKBG), F.m13);
            text_l(F.r15, "Đã bán " + std::to_string(ds[i].da_ban), Rectangle{ c4, r.y, 110, 88 }, TEXT);
            std::string b = bst.cua(p->getid());
            if (!b.empty()) text_l(F.r13, fit(F.r13, b, r.x + r.width - c5 - 50), Rectangle{ c5, r.y, r.x + r.width - c5 - 40, 88 }, Color{ 110, 100, 200, 255 });
            icon(mo ? IC_UP : IC_DOWN, r.x + r.width - 26, r.y + 44, 14, MUTED, 1.8f);
            if (mo) {
                hline(r.x + 20, r.y + 88, r.width - 40);
                text(F.m13, "BIẾN THỂ (MÀU / SIZE : SỐ LƯỢNG)", r.x + 24, r.y + 102, MUTED, 1.4f);
                for (int k = 0; k < nvar; k++) {
                    const Variant &v = p->get_variant(k);
                    Rectangle vr = { r.x + 24 + (k % 4) * ((w - 48) / 4), r.y + 128 + (k / 4) * 46.0f, (w - 48) / 4 - 10, 38 };
                    rrect(vr, 12, v.sl <= 0 ? DANGBG : (v.sl <= 5 ? WARNBG : SKY50));
                    bool biet; Color mc = mau_theo_ten(v.color, &biet);
                    DrawCircleV(Vector2{ vr.x + 18, vr.y + 19 }, 7, mc);
                    DrawRing(Vector2{ vr.x + 18, vr.y + 19 }, 6.5f, 7.5f, 0, 360, 14, Color{ 0, 0, 0, 45 });
                    text_l(F.r13, fit(F.r13, v.color + " / " + v.size, vr.width - 70), Rectangle{ vr.x + 32, vr.y, vr.width - 32, 38 }, TEXT);
                    text_r(F.m15, std::to_string(v.sl), vr, INK, 12);
                }
                if (admin) {
                    float by = r.y + 88 + cao_mo - 56;
                    if (button(Rectangle{ r.x + 24, by, 140, 40 }, "Nhập / kiểm kê", BTN_PRIMARY, F.m13, true, 20)) {
                        md_ma = p->getid(); nk_che_do = 0; f_nk_mau.clear(); f_nk_size.clear(); f_nk_sl.clear(); modal = M_STOCK_IN;
                    }
                    if (button(Rectangle{ r.x + 176, by, 140, 40 }, "Sửa giá / BST", BTN_OUTLINE, F.m13, true, 20)) {
                        md_ma = p->getid(); f_sp_gia.set(std::to_string(p->getprice())); f_sp_bst.set(bst.cua(p->getid())); modal = M_EDIT_PRODUCT;
                    }
                    if (button(Rectangle{ r.x + r.width - 140, by, 116, 40 }, "Xóa", BTN_DANGER, F.m13, true, 20)) { md_ma = p->getid(); modal = M_DELETE_PRODUCT; }
                }
            }
        }
        y += r.height + 12;
    }
    if (ds.size() == 0) text_c(F.r15, "Không có sản phẩm nào phù hợp.", Rectangle{ x, y + 10, w, 30 }, MUTED);
    cao_nd = y + 40 - (view.y - cuon);
}

// ============================ DOANH SỐ CỦA TÔI (nhân viên) ============================
void App::ve_doanh_so_cua_toi(Rectangle view) {
    float x = view.x + 32, w = view.width - 64, y = view.y - cuon + 28;
    const DoanhThu &dt = shop.lay_doanh_thu();
    long long tong = dt.doanh_so_nhan_vien(ma);
    long long thang = dt.doanh_so_nhan_vien(ma, KY_THANG, khoa_ky_hien_tai(KY_THANG));
    long long tuan = dt.doanh_so_nhan_vien(ma, KY_TUAN, khoa_ky_hien_tai(KY_TUAN));
    struct Tk { const char *nhan; long long v; };
    Tk tk[] = { { "TUẦN NÀY", tuan }, { "THÁNG NÀY", thang }, { "TỔNG CỘNG", tong } };
    float kw = (w - 32) / 3;
    for (int i = 0; i < 3; i++) {
        Rectangle r = { x + i * (kw + 16), y, kw, 110 };
        panel(r, 18);
        text(F.m13, tk[i].nhan, r.x + 24, r.y + 22, MUTED, 1.6f);
        text(F.b27, tien(tk[i].v), r.x + 24, r.y + 54, i == 2 ? SKY600 : INK);
    }
    y += 110 + 30;
    text(F.s22, "Hóa đơn gần đây do bạn lập", x, y, INK);
    y += 44;
    int dem = 0;
    VECTOR<std::string> da;
    for (int i = dt.so_giao_dich() - 1; i >= 0 && dem < 15; i--) {
        const GiaoDich &g = dt.lay(i);
        if (g.ma_nv != ma || g.ma_hd.empty()) continue;
        bool co = false;
        for (int k = 0; k < da.size(); k++) if (da[k] == g.ma_hd) co = true;
        if (co) continue;
        da.push_back(g.ma_hd);
        HoaDon hd = lay_hoa_don(dt, g.ma_hd);
        Rectangle r = { x, y, w, 64 };
        bool hv = hover(r);
        panel(r, 14);
        if (hv) { C.hand = true; rrect_line(r, 14, 1.5f, SKY300); if (C.pressed) { hd_cuoi = hd; modal = M_INVOICE; } }
        text_l(F.m15, hd.ma_hd, Rectangle{ r.x + 22, r.y, 120, 64 }, INK);
        text_l(F.r15, dinh_dang_ngay(hd.thoi_gian), Rectangle{ r.x + 150, r.y, 120, 64 }, TEXT);
        text_l(F.r15, "Khách " + hd.ma_kh + "   -   " + std::to_string(hd.dong.size()) + " dòng", Rectangle{ r.x + 290, r.y, 300, 64 }, MUTED);
        text_r(F.b17, tien(hd.thanh_toan()), r, SKY600, 24);
        y += 74; dem++;
    }
    if (dem == 0) text_c(F.r15, "Bạn chưa lập hóa đơn nào.", Rectangle{ x, y + 10, w, 30 }, MUTED);
    cao_nd = y + 40 - (view.y - cuon);
}

// ============================ NHÂN VIÊN (admin) ============================
void App::ve_nhan_vien(Rectangle view) {
    float x = view.x + 32, w = view.width - 64, y = view.y - cuon + 28;
    const DoanhThu &dt = shop.lay_doanh_thu();
    text(F.r15, std::to_string(qn.size()) + " tài khoản nhân viên (không xóa, chỉ cho nghỉ việc vì sổ doanh thu còn lưu mã)", x, y + 12, MUTED);
    if (button(Rectangle{ x + w - 200, y, 200, 48 }, "+ Thêm nhân viên", BTN_PRIMARY, F.m15, true, 24)) { f_nv_ten.clear(); f_nv_sdt.clear(); nv_vai_tro = 0; modal = M_NEW_STAFF; }
    y += 70;
    long long khoa_thang = khoa_ky_hien_tai(KY_THANG);
    for (int i = 0; i < qn.size(); i++) {
        const NhanVien &n = qn.lay(i);
        Rectangle r = { x, y, w, 118 };
        panel(r, 18);
        bool ad = n.laAdmin();
        DrawCircleV(Vector2{ r.x + 44, r.y + 40 }, 24, ad ? LAVBG : MINTBG);
        text_c(F.b17, hoa(to_utf8(std::u32string(1, to_u32(n.getTenNV())[0]))), Rectangle{ r.x + 20, r.y + 16, 48, 48 }, ad ? Color{ 110, 100, 200, 255 } : Color{ 40, 150, 110, 255 });
        text(F.b17, fit(F.b17, n.getTenNV(), w * 0.3f), r.x + 84, r.y + 20, INK);
        text(F.r13, n.getMaNV() + "   -   " + n.getSoDienThoai(), r.x + 84, r.y + 48, MUTED);
        float tx = tag_auto(r.x + 84, r.y + 74, 26, ad ? "Admin" : "Nhân viên", ad ? Color{ 110, 100, 200, 255 } : Color{ 40, 150, 110, 255 }, ad ? LAVBG : MINTBG, F.m13);
        if (!n.getDangLamViec()) tag_auto(tx, r.y + 74, 26, "Đã nghỉ", DANGER, DANGBG, F.m13);
        float cx = r.x + w * 0.42f;
        text(F.m13, "THÁNG NÀY", cx, r.y + 22, MUTED, 1.4f);
        text(F.b17, tien(dt.doanh_so_nhan_vien(n.getMaNV(), KY_THANG, khoa_thang)), cx, r.y + 44, INK);
        text(F.m13, "TỔNG CỘNG", cx, r.y + 74, MUTED, 1.4f);
        text(F.m15, tien(dt.doanh_so_nhan_vien(n.getMaNV())), cx, r.y + 94 - 2, TEXT);
        float bx = r.x + r.width - 150;
        if (button(Rectangle{ bx, r.y + 14, 130, 34 }, "Cấp lại MK", BTN_SOFT, F.m13, true, 17)) {
            if (qn.dat_lai_mat_khau(n.getMaNV())) toast("Đã cấp lại: mật khẩu = số điện thoại của nhân viên.", TOAST_OK);
        }
        if (button(Rectangle{ bx, r.y + 54, 130, 34 }, n.getDangLamViec() ? "Cho nghỉ" : "Đi làm lại", BTN_OUTLINE, F.m13, true, 17)) {
            if (!qn.dat_trang_thai(n.getMaNV(), !n.getDangLamViec())) toast("Không thực hiện được (không thể vô hiệu hóa admin cuối cùng).", TOAST_ERR);
        }
        if (button(Rectangle{ bx - 140, r.y + 54, 130, 34 }, ad ? "Hạ xuống NV" : "Lên admin", BTN_GHOST, F.m13, true, 17)) {
            if (!qn.doi_vai_tro(n.getMaNV(), ad ? VAI_TRO_NHAN_VIEN : VAI_TRO_ADMIN)) toast("Không thực hiện được (không thể hạ admin cuối cùng).", TOAST_ERR);
        }
        y += 130;
    }
    cao_nd = y + 40 - (view.y - cuon);
}
