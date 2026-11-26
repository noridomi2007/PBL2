// CỬA HÀNG DÀNH CHO KHÁCH: đầu trang, trang chủ + bộ lọc, chi tiết, giỏ hàng, tài khoản
#include "App.h"
#include <cmath>
using namespace ui;

static const float TOP_H = 148;       // chiều cao phần đầu trang cố định (thông báo 30 + đầu trang 74 + menu loại 44)

long long App::chi_tieu_khach() {
    if (role != R_CUSTOMER) return 0;
    int n = shop.lay_doanh_thu().so_giao_dich();
    if (n != chi_n) { chi_n = n; chi_v = shop.lay_doanh_thu().tong_chi_tieu_khach(ma); }
    return chi_v;
}

void App::tinh_loc() { kq = loc_cua_hang(shop, bst, tc); can_loc = false; }

void App::xoa_loc() {
    KieuSapXep sx = tc.sap_xep;
    tc = TieuChiLoc();
    tc.sap_xep = sx;
    f_tim.clear(); f_gia_min.clear(); f_gia_max.clear();
    can_loc = true;
}

void App::mo_chi_tiet(const std::string &id) {
    ma_ct = id; ct_anh = 0; ct_sl = 1; ct_mau.clear(); ct_size.clear();
    int vt = shop.find(id);
    if (vt != -1) {
        product *p = shop.take(vt);
        for (int i = 0; i < p->so_variant(); i++) if (p->get_variant(i).sl > 0) { ct_mau = p->get_variant(i).color; ct_size = p->get_variant(i).size; break; }
    }
    doi_man(S_DETAIL);
}

// ============================ KHUNG CỬA HÀNG ============================
void App::ve_cua_hang() {
    if (can_loc) tinh_loc();
    Rectangle view = { 0, TOP_H, (float)C.sw, C.sh - TOP_H };
    bool khoa = C.blocked;
    if (menu_mo) C.blocked = true;
    cuon_cap_nhat(view);
    push_clip(view);
    switch (screen) {
        case S_HOME: ve_trang_chu(view); break;
        case S_DETAIL: ve_chi_tiet(view); break;
        case S_CART: ve_gio_hang(view); break;
        case S_ACCOUNT: ve_tai_khoan(view); break;
        default: break;
    }
    pop_clip();
    scroll_bar(view, cao_nd, cuon);
    ve_dau_trang();
    C.blocked = khoa;
}

// ============================ ĐẦU TRANG ============================
void App::ve_dau_trang() {
    float W = (float)C.sw;
    float cw = fminf(W - 64, 1280), x0 = (W - cw) / 2;
    // thanh thông báo
    DrawRectangle(0, 0, (int)W, 30, SKY100);
    int toi_da = BANG_HANG[SO_HANG_TV - 1].phan_tram;
    text_c(F.r13, "Ưu đãi theo hạng thành viên: giảm đến " + std::to_string(toi_da) + "% cho mỗi đơn hàng", Rectangle{ 0, 0, W, 30 }, SKY600);
    {
        Rectangle lk = { x0 + cw - 170, 0, 170, 30 };
        bool h = hover(lk);
        if (h) { C.hand = true; if (C.pressed) { tab_dn = 1; quay_lai = S_HOME; doi_man(S_LOGIN); } }
        text_r(F.r13, "Dành cho nhân viên", lk, h ? SKY600 : MUTED, 0);
    }
    // đầu trang
    DrawRectangle(0, 30, (int)W, 118, SURFACE);
    hline(0, 147, W);
    Rectangle logo = { x0, 36, 220, 62 };
    if (hover(logo)) { C.hand = true; if (C.pressed) { xoa_loc(); doi_man(S_HOME); } }
    text(F.s30, TEN_SHOP, x0, 50, INK, 3.5f);
    // ô tìm kiếm kiểu Shopee
    float sw = fminf(560.0f, W * 0.40f), sx = (W - sw) / 2;
    Rectangle sr = { sx, 46, sw, 46 };
    bool enter = field(f_tim, Rectangle{ sr.x, sr.y, sr.width - 58, sr.height });
    Rectangle nut = { sr.x + sr.width - 52, sr.y + 4, 48, 38 };
    bool bam = button(nut, "", BTN_PRIMARY, F.m15, true, 19);
    icon(IC_SEARCH, nut.x + 24, nut.y + 19, 20, WHITE, 2.0f);
    if (enter || bam) {
        tc.tu_khoa = f_tim.utf8(); can_loc = true; cuon = cuon_dich = 0;
        if (screen != S_HOME) doi_man(S_HOME);
    }
    // bên phải: tài khoản + giỏ hàng
    float rx = x0 + cw;
    int dem = 0;
    for (int i = 0; i < gio.so_dong(); i++) dem += gio.lay(i).sl;
    Rectangle gr = { rx - 52, 46, 46, 46 };
    bool hg = hover(gr);
    if (hg) { C.hand = true; rrect(gr, 23, SKY50); if (C.pressed) { if (role == R_CUSTOMER) doi_man(S_CART); else { modal = M_NEED_LOGIN; } } }
    icon(IC_CART, gr.x + 23, gr.y + 23, 26, hg ? SKY600 : INK, 2.0f);
    if (dem > 0) {
        DrawCircleV(Vector2{ gr.x + 36, gr.y + 8 }, 10, SKY500);
        text_c(F.m13, std::to_string(dem > 99 ? 99 : dem), Rectangle{ gr.x + 26, gr.y - 2, 20, 20 }, WHITE);
    }
    std::string nhan = role == R_CUSTOMER ? ten : "Đăng nhập";
    nhan = fit(F.m15, nhan, 130);
    float uw = tw(F.m15, nhan) + 62;
    Rectangle ur = { gr.x - 12 - uw, 46, uw, 46 };
    bool hu = hover(ur);
    bool vua_mo = false;   // đánh dấu menu vừa được mở ở khung hình này
    if (hu) { C.hand = true; rrect(ur, 23, SKY50); }
    if (hu && C.pressed) {
        if (role == R_CUSTOMER) { menu_mo = !menu_mo; vua_mo = menu_mo; }
        else { quay_lai = screen; tab_dn = 0; doi_man(S_LOGIN); }
    }
    if (role == R_CUSTOMER) {
        DrawCircleV(Vector2{ ur.x + 26, ur.y + 23 }, 16, SKY200);
        std::string ch = ten.empty() ? "?" : to_utf8(std::u32string(1, to_u32(ten)[0]));
        text_c(F.b15, hoa(ch), Rectangle{ ur.x + 10, ur.y + 7, 32, 32 }, SKY600);
    } else icon(IC_USER, ur.x + 26, ur.y + 23, 26, hu ? SKY600 : INK, 2.0f);
    text_l(F.m15, nhan, Rectangle{ ur.x + 48, ur.y, uw - 48, ur.height }, INK);

    // hàng menu loại (kiểu LV: chữ hoa, giãn chữ)
    float ny = 104;
    hline(0, ny, W);
    VECTOR<std::string> loai = shop.lay_ds_subtype("");
    float x = x0;
    auto muc = [&](const std::string &nhan_mn, bool chon, bool &bam_mn) {
        std::string t = hoa(nhan_mn);
        float w = tw(F.m15, t, 1.3f);
        Rectangle r = { x - 6, ny, w + 12, 44 };
        bool h = hover(r);
        if (h) C.hand = true;
        text_l(F.m15, t, Rectangle{ x, ny + 1, w, 44 }, (chon || h) ? SKY600 : TEXT, 0, 1.3f);
        if (chon) DrawRectangle((int)x, (int)(ny + 40), (int)w, 3, SKY500);
        bam_mn = h && C.pressed;
        x += w + 38;
    };
    bool b;
    muc("Tất cả", tc.subtype.empty() && screen == S_HOME, b);
    if (b) { tc.subtype.clear(); tc.kieu.clear(); can_loc = true; doi_man(S_HOME); }
    for (int i = 0; i < loai.size(); i++) {
        muc(loai[i], tc.subtype == loai[i] && screen == S_HOME, b);
        if (b) { tc.subtype = loai[i]; tc.kieu.clear(); can_loc = true; doi_man(S_HOME); }
    }

    // menu tài khoản (mở khi bấm vào tên)
    if (menu_mo) {
        bool khoa = C.blocked;
        C.blocked = false;
        Rectangle m = { ur.x + ur.width - 230, 96, 230, 3 * 46 + 16 };
        panel(m, 14);
        const char *muc_t[3] = { "Tài khoản của tôi", "Đơn mua", "Đăng xuất" };
        Icon ics[3] = { IC_USER, IC_BAG, IC_LOGOUT };
        for (int i = 0; i < 3; i++) {
            Rectangle r = { m.x + 8, m.y + 8 + i * 46, m.width - 16, 42 };
            bool h = hover(r);
            if (h) { C.hand = true; rrect(r, 10, SKY50); }
            icon(ics[i], r.x + 24, r.y + 21, 18, h ? SKY600 : TEXT, 1.7f);
            text_l(F.r15, muc_t[i], Rectangle{ r.x + 46, r.y, r.width - 46, r.height }, h ? SKY600 : TEXT);
            if (h && C.pressed) {
                menu_mo = false;
                if (i == 0) { tk_tab = 0; doi_man(S_ACCOUNT); }
                else if (i == 1) { tk_tab = 1; doi_man(S_ACCOUNT); }
                else dang_xuat();
                C.pressed = false;
            }
        }
        if (C.pressed && !vua_mo && !CheckCollisionPointRec(C.mouse, m)) menu_mo = false;
        C.blocked = khoa;
    }
}

// ============================ TRANG CHỦ ============================
static std::string nhan_gia(long long a, long long b) {
    if (a == 0 && b > 0) return "Dưới " + tien_gon(b);
    if (a > 0 && b == 0) return "Trên " + tien_gon(a);
    return tien_gon(a) + " - " + tien_gon(b);
}

void App::ve_trang_chu(Rectangle view) {
    float W = (float)C.sw;
    float cw = fminf(W - 64, 1280), x0 = (W - cw) / 2;
    float y = view.y - cuon + 28;
    float y_dau = y;
    bool dang_loc = tc.co_loc() || !tc.tu_khoa.empty();

    if (!dang_loc) {
        Rectangle hr = { x0, y, cw, 270 };
        rrect(hr, 26, SKY100);
        push_clip(hr);
        DrawCircleV(Vector2{ hr.x + hr.width * 0.78f, hr.y + 60 }, 250, Fade(SKY200, 0.7f));
        DrawCircleV(Vector2{ hr.x + 90, hr.y + hr.height + 40 }, 160, Fade(WHITE, 0.45f));
        DrawCircleV(Vector2{ hr.x + hr.width * 0.5f, hr.y + hr.height }, 110, Fade(SKY300, 0.35f));
        pop_clip();
        text(F.m13, "BỘ SƯU TẬP MỚI", hr.x + 56, hr.y + 52, SKY600, 3.0f);
        text(F.s40, "Phong cách nhẹ nhàng,", hr.x + 56, hr.y + 84, INK);
        text(F.s40, "cho mỗi ngày", hr.x + 56, hr.y + 132, INK);
        if (button(Rectangle{ hr.x + 56, hr.y + 200, 190, 46 }, "KHÁM PHÁ NGAY", BTN_DARK, F.m15, true, 23, 1.4f)) cuon_dich = hero_y;
        // ảnh minh họa: các sản phẩm đầu tiên có ảnh
        float ix = hr.x + hr.width - 70;
        int dem = 0;
        for (int i = kq.size() - 1; i >= 0 && dem < 3; i--) {
            const std::string &id = kq[i].sp->getid();
            if (anh_dem(id) <= 0) continue;
            const Texture2D *t = anh_lay(id, 1, 480);
            float w = 160, h = 210;
            Rectangle r = { ix - w - dem * 118.0f, hr.y + 30 + (dem == 1 ? 14 : 0) + dem * 6.0f, w, h };
            shadow(r, 16, 1.3f);
            rrect(r, 16, SURFACE);
            if (t) ve_anh(*t, Rectangle{ r.x + 6, r.y + 6, r.width - 12, r.height - 12 }, 12, SURFACE);
            dem++;
        }
        y += 270 + 34;
        VECTOR<std::string> ds_bst = bst.ds_ten();
        if (ds_bst.size() > 0) {
            text(F.s22, "Bộ sưu tập", x0, y, INK);
            y += 40;
            float x = x0;
            for (int i = 0; i < ds_bst.size(); i++) {
                std::string t = ds_bst[i] + "  (" + std::to_string(bst.dem(ds_bst[i])) + ")";
                float w = tw(F.m15, t) + 40;
                if (chip(Rectangle{ x, y, w, 38 }, t, false, F.m15)) { tc.bo_suu_tap = ds_bst[i]; can_loc = true; cuon_dich = hero_y; }
                x += w + 10;
            }
            y += 38 + 34;
        }
    }
    hero_y = y - (view.y - cuon) - 8;   // vị trí đầu phần sản phẩm (để cuộn tới)

    float sbw = 270, gap = 30;
    float kq_y = y;
    float bl_h = ve_bo_loc(x0, y, sbw);
    float gy = ve_luoi(x0 + sbw + gap, kq_y, cw - sbw - gap);
    y = fmaxf(gy, y + bl_h);
    y = ve_chan_trang(y + 50, x0, cw);
    cao_nd = y - (view.y - cuon) - 0;
    (void)y_dau;
}

// ----- chân trang: tên shop, hỗ trợ, bảng hạng thành viên (lấy từ BANG_HANG) -----
float App::ve_chan_trang(float y, float x, float w) {
    float W = (float)C.sw;
    float cao = 230;
    DrawRectangle(0, (int)y, (int)W, (int)cao, SKY100);
    float cx = x + 8;
    text(F.s30, TEN_SHOP, cx, y + 36, INK, 3.0f);
    text_wrap(F.r15, KHAU_HIEU, cx, y + 84, 280, TEXT, 3, 22);
    float c2 = x + w * 0.42f;
    text(F.m13, "HẠNG THÀNH VIÊN", c2, y + 38, SKY600, 2.0f);
    for (int i = 0; i < SO_HANG_TV && i < 5; i++) {
        std::string dong = std::string(BANG_HANG[i].ten) + ":  giảm " + std::to_string(BANG_HANG[i].phan_tram) + "%";
        if (BANG_HANG[i].nguong > 0) dong += "  (từ " + tien_gon(BANG_HANG[i].nguong) + " chi tiêu)";
        text(F.r15, dong, c2, y + 66 + i * 26.0f, TEXT);
    }
    float c3 = x + w * 0.76f;
    text(F.m13, "HỖ TRỢ", c3, y + 38, SKY600, 2.0f);
    text(F.r15, "Mua hàng trực tuyến hoặc tại quầy", c3, y + 66, TEXT);
    text(F.r15, "Tích lũy chi tiêu để lên hạng", c3, y + 92, TEXT);
    text(F.r15, "Ưu đãi áp dụng tự động khi thanh toán", c3, y + 118, TEXT);
    hline(x, y + cao - 44, w, SKY200);
    text(F.r13, std::string("© ") + TEN_SHOP, x + 8, y + cao - 30, MUTED);
    return y + cao;
}

// ============================ BỘ LỌC (thanh bên) ============================
float App::ve_bo_loc(float x, float y, float w) {
    Rectangle pr = { x, y, w, bl_cao };
    panel(pr, 18);
    float x1 = x + 12, w1 = w - 24;
    float cy = y + 22;
    icon(IC_FILTER, x + 28, cy + 9, 18, SKY600, 1.8f);
    text(F.b15, "BỘ LỌC", x + 46, cy, INK, 1.5f);
    if (tc.co_loc()) {
        Rectangle r = { x + w - 104, cy - 6, 90, 28 };
        bool h = hover(r);
        if (h) C.hand = true;
        text_r(F.r13, "Xóa tất cả", r, h ? DANGER : MUTED, 4);
        if (h && C.pressed) { xoa_loc(); }
    }
    cy += 34;

    auto muc = [&](const char *t) {
        cy += 6; hline(x + 18, cy, w - 36); cy += 18;
        text(F.m13, t, x + 20, cy, MUTED, 1.6f);
        cy += 26;
    };
    const float RH = 34;

    // ---- HÃNG ----
    muc("HÃNG");
    VECTOR<std::string> hangs = shop.lay_ds_hang();
    if (radio_row(Rectangle{ x1, cy, w1, RH }, "Tất cả hãng", tc.hang.empty(), F.r15)) { tc.hang.clear(); can_loc = true; }
    cy += RH;
    int hien = xem_them_hang ? hangs.size() : (hangs.size() < 6 ? hangs.size() : 6);
    for (int i = 0; i < hien; i++) {
        if (radio_row(Rectangle{ x1, cy, w1, RH }, hangs[i], tc.hang == hangs[i], F.r15)) {
            tc.hang = hangs[i];
            VECTOR<std::string> st = shop.lay_ds_subtype(tc.hang);
            bool co = false;
            for (int k = 0; k < st.size(); k++) if (st[k] == tc.subtype) co = true;
            if (!co) { tc.subtype.clear(); tc.kieu.clear(); }
            can_loc = true;
        }
        cy += RH;
    }
    if (hangs.size() > 6) {
        Rectangle r = { x1, cy, w1, 30 };
        bool h = hover(r);
        if (h) C.hand = true;
        text_l(F.m13, xem_them_hang ? "Thu gọn" : "Xem thêm " + std::to_string(hangs.size() - 6) + " hãng", r, SKY600, 34);
        icon(xem_them_hang ? IC_UP : IC_DOWN, r.x + 20, r.y + 15, 12, SKY600, 1.6f);
        if (h && C.pressed) xem_them_hang = !xem_them_hang;
        cy += 30;
    }

    // ---- DANH MỤC -> LOẠI CỤ THỂ (phân cấp) ----
    muc("DANH MỤC");
    VECTOR<std::string> loai = shop.lay_ds_subtype(tc.hang);
    if (radio_row(Rectangle{ x1, cy, w1, RH }, "Tất cả danh mục", tc.subtype.empty(), F.r15)) { tc.subtype.clear(); tc.kieu.clear(); can_loc = true; }
    cy += RH;
    for (int i = 0; i < loai.size(); i++) {
        bool chon = tc.subtype == loai[i];
        if (radio_row(Rectangle{ x1, cy, w1, RH }, loai[i], chon, chon ? F.m15 : F.r15)) { tc.subtype = loai[i]; tc.kieu.clear(); can_loc = true; }
        cy += RH;
        if (chon) {   // bung các loại cụ thể của danh mục này (Áo thun, Áo polo, ...)
            VECTOR<std::string> kieu = shop.lay_ds_kieu_con(tc.subtype, tc.hang);
            if (kieu.size() > 0) {
                float ix = x1 + 20, iw = w1 - 20;
                rrect(Rectangle{ ix - 4, cy, iw + 4, kieu.size() * 32.0f + 8 }, 10, SKY50);
                cy += 4;
                for (int k = 0; k < kieu.size(); k++) {
                    if (check_row(Rectangle{ ix, cy, iw, 32 }, kieu[k], tc.kieu == kieu[k], F.r15)) { tc.kieu = (tc.kieu == kieu[k]) ? "" : kieu[k]; can_loc = true; }
                    cy += 32;
                }
                cy += 8;
            }
        }
    }

    // ---- GIÁ ----
    muc("KHOẢNG GIÁ (₫)");
    float fw = (w1 - 16 - 14) / 2;
    bool ap = false;
    if (field(f_gia_min, Rectangle{ x1 + 8, cy, fw, 40 }, F.r15)) ap = true;
    text_c(F.r15, "-", Rectangle{ x1 + 8 + fw, cy, 14, 40 }, MUTED);
    if (field(f_gia_max, Rectangle{ x1 + 8 + fw + 14, cy, fw, 40 }, F.r15)) ap = true;
    cy += 50;
    if (button(Rectangle{ x1 + 8, cy, w1 - 16, 38 }, "Áp dụng", BTN_SOFT, F.m15, true, 19) || ap) {
        tc.gia_min = (long long)f_gia_min.number(); tc.gia_max = (long long)f_gia_max.number();
        if (tc.gia_min > 0 && tc.gia_max > 0 && tc.gia_min > tc.gia_max) {
            std::swap(tc.gia_min, tc.gia_max);
            f_gia_min.set(std::to_string(tc.gia_min)); f_gia_max.set(std::to_string(tc.gia_max));
        }
        can_loc = true;
    }
    cy += 48;
    struct P { const char *t; long long a, b; };
    P pre[] = { { "Dưới 200k", 0, 200000 }, { "200k - 500k", 200000, 500000 }, { "500k - 1tr", 500000, 1000000 }, { "Trên 1tr", 1000000, 0 } };
    for (int i = 0; i < 4; i++) {
        float cwp = (w1 - 16 - 10) / 2;
        Rectangle r = { x1 + 8 + (i % 2) * (cwp + 10), cy + (i / 2) * 42.0f, cwp, 34 };
        bool chon = tc.gia_min == pre[i].a && tc.gia_max == pre[i].b;
        if (chip(r, pre[i].t, chon, F.m13)) {
            if (chon) { tc.gia_min = tc.gia_max = 0; f_gia_min.clear(); f_gia_max.clear(); }
            else {
                tc.gia_min = pre[i].a; tc.gia_max = pre[i].b;
                if (pre[i].a) f_gia_min.set(std::to_string(pre[i].a)); else f_gia_min.clear();
                if (pre[i].b) f_gia_max.set(std::to_string(pre[i].b)); else f_gia_max.clear();
            }
            can_loc = true;
        }
    }
    cy += 84;

    // ---- BỘ SƯU TẬP ----
    VECTOR<std::string> ds_bst = bst.ds_ten();
    if (ds_bst.size() > 0) {
        muc("BỘ SƯU TẬP");
        if (radio_row(Rectangle{ x1, cy, w1, RH }, "Tất cả bộ sưu tập", tc.bo_suu_tap.empty(), F.r15)) { tc.bo_suu_tap.clear(); can_loc = true; }
        cy += RH;
        for (int i = 0; i < ds_bst.size(); i++) {
            if (radio_row(Rectangle{ x1, cy, w1, RH }, ds_bst[i], tc.bo_suu_tap == ds_bst[i], F.r15, std::to_string(bst.dem(ds_bst[i])))) { tc.bo_suu_tap = ds_bst[i]; can_loc = true; }
            cy += RH;
        }
    }

    // ---- LƯỢT ĐÃ BÁN ----
    muc("LƯỢT ĐÃ BÁN");
    int moc[] = { 0, 1, 10, 50, 100 };
    float bx = x1 + 8, by = cy;
    for (int i = 0; i < 5; i++) {
        std::string t = moc[i] == 0 ? "Tất cả" : "Từ " + std::to_string(moc[i]);
        float cwid = tw(F.m13, t) + 28;
        if (bx + cwid > x1 + w1 - 4) { bx = x1 + 8; by += 40; }
        if (chip(Rectangle{ bx, by, cwid, 32 }, t, tc.ban_min == moc[i], F.m13)) { tc.ban_min = moc[i]; can_loc = true; }
        bx += cwid + 8;
    }
    cy = by + 42;

    // ---- TÌNH TRẠNG ----
    muc("TÌNH TRẠNG");
    if (check_row(Rectangle{ x1, cy, w1, RH }, "Chỉ hiện sản phẩm còn hàng", tc.chi_con_hang, F.r15)) { tc.chi_con_hang = !tc.chi_con_hang; can_loc = true; }
    cy += RH + 16;
    bl_cao = cy - y;
    return bl_cao;
}

// ============================ LƯỚI SẢN PHẨM ============================
float App::ve_luoi(float x, float y, float w) {
    // thanh sắp xếp kiểu Shopee
    Rectangle bar = { x, y, w, 54 };
    rrect(bar, 14, SKY100);
    text_l(F.r15, "Sắp xếp theo", Rectangle{ bar.x + 18, bar.y, 120, bar.height }, TEXT);
    struct Sx { const char *t; KieuSapXep k; };
    Sx ds[] = { { "Liên quan", SX_LIEN_QUAN }, { "Bán chạy", SX_BAN_CHAY }, { "Giá: thấp đến cao", SX_GIA_TANG }, { "Giá: cao đến thấp", SX_GIA_GIAM }, { "Tên A-Z", SX_TEN_AZ } };
    float cx = bar.x + 134;
    for (Sx &s : ds) {
        float cwid = tw(F.m15, s.t) + 34;
        if (chip(Rectangle{ cx, bar.y + 8, cwid, 38 }, s.t, tc.sap_xep == s.k, F.m15)) { tc.sap_xep = s.k; can_loc = true; }
        cx += cwid + 8;
    }
    std::string dem = std::to_string(kq.size()) + " sản phẩm";
    if (cx + tw(F.r15, dem) + 20 < bar.x + bar.width) text_r(F.r15, dem, bar, MUTED, 18);
    y += 54 + 14;

    // chip các bộ lọc đang bật (bấm để bỏ)
    struct Ch { std::string t; int loai; };
    std::vector<Ch> chs;
    if (!tc.tu_khoa.empty()) chs.push_back({ "Từ khóa: " + tc.tu_khoa, 0 });
    if (!tc.hang.empty()) chs.push_back({ "Hãng: " + tc.hang, 1 });
    if (!tc.subtype.empty()) chs.push_back({ "Danh mục: " + tc.subtype, 2 });
    if (!tc.kieu.empty()) chs.push_back({ "Loại: " + tc.kieu, 3 });
    if (tc.gia_min > 0 || tc.gia_max > 0) chs.push_back({ "Giá: " + nhan_gia(tc.gia_min, tc.gia_max), 4 });
    if (!tc.bo_suu_tap.empty()) chs.push_back({ "BST: " + tc.bo_suu_tap, 5 });
    if (tc.ban_min > 0) chs.push_back({ "Đã bán từ " + std::to_string(tc.ban_min), 6 });
    if (tc.chi_con_hang) chs.push_back({ "Còn hàng", 7 });
    if (!chs.empty()) {
        float px = x;
        for (Ch &c : chs) {
            std::string t = fit(F.m13, c.t, 260);
            float cwid = tw(F.m13, t) + 46;
            if (px + cwid > x + w) { px = x; y += 40; }
            Rectangle r = { px, y, cwid, 32 };
            bool h = hover(r);
            if (h) C.hand = true;
            rrect(r, 16, h ? SKY200 : SKY100);
            text_l(F.m13, t, r, SKY600, 14);
            icon(IC_CLOSE, r.x + cwid - 16, r.y + 16, 11, SKY600, 1.6f);
            if (h && C.pressed) {
                switch (c.loai) {
                    case 0: tc.tu_khoa.clear(); f_tim.clear(); break;
                    case 1: tc.hang.clear(); break;
                    case 2: tc.subtype.clear(); tc.kieu.clear(); break;
                    case 3: tc.kieu.clear(); break;
                    case 4: tc.gia_min = tc.gia_max = 0; f_gia_min.clear(); f_gia_max.clear(); break;
                    case 5: tc.bo_suu_tap.clear(); break;
                    case 6: tc.ban_min = 0; break;
                    case 7: tc.chi_con_hang = false; break;
                }
                can_loc = true;
            }
            px += cwid + 8;
        }
        y += 32 + 16;
    }

    if (kq.size() == 0) {
        Rectangle r = { x, y, w, 300 };
        panel(r, 18);
        DrawCircleV(Vector2{ r.x + w / 2, r.y + 90 }, 44, SKY100);
        icon(IC_SEARCH, r.x + w / 2, r.y + 90, 44, SKY400, 3.0f);
        text_c(F.s22, "Không tìm thấy sản phẩm phù hợp", Rectangle{ r.x, r.y + 150, w, 30 }, INK);
        text_c(F.r15, "Thử bỏ bớt bộ lọc hoặc đổi từ khóa tìm kiếm.", Rectangle{ r.x, r.y + 186, w, 24 }, MUTED);
        if (button(Rectangle{ r.x + w / 2 - 90, r.y + 226, 180, 44 }, "Xóa bộ lọc", BTN_PRIMARY, F.m15, true, 22)) xoa_loc();
        return y + 300;
    }

    float gap = 22;
    int cols = (int)((w + gap) / (216 + gap));
    if (cols < 2) cols = 2;
    float cwid = (w - gap * (cols - 1)) / cols;
    float cao_anh = (cwid - 16) * 1.2f;
    float cao_the = cao_anh + 16 + 112;
    for (int i = 0; i < kq.size(); i++) {
        int c = i % cols, r = i / cols;
        Rectangle rc = { x + c * (cwid + gap), y + r * (cao_the + gap), cwid, cao_the };
        if (rc.y + rc.height < TOP_H || rc.y > C.sh) continue;      // chỉ vẽ phần đang nhìn thấy
        ve_the_sp(rc, kq[i]);
    }
    int hang_so = (kq.size() + cols - 1) / cols;
    return y + hang_so * (cao_the + gap);
}

void App::ve_the_sp(Rectangle r, const KetQuaLoc &k) {
    product *p = k.sp;
    bool h = hover(r);
    if (h) { C.hand = true; shadow(r, 18, 1.2f); }
    rrect(r, 18, SURFACE);
    rrect_line(r, 18, h ? 1.6f : 1.0f, h ? SKY300 : LINE);
    float ca = (r.width - 16) * 1.2f;
    Rectangle ar = { r.x + 8, r.y + 8, r.width - 16, ca };
    const Texture2D *t = anh_dem(p->getid()) > 0 ? anh_lay(p->getid(), 1, 520) : nullptr;
    if (t) ve_anh(*t, ar, 14, SURFACE, h ? 1.06f : 1.0f);
    else ve_anh_trong(ar, 14, p->getbrand(), p->getname());
    if (k.ton <= 0) {
        rrect(ar, 14, Fade(WHITE, 0.62f));
        tag(Rectangle{ ar.x + ar.width / 2 - 48, ar.y + ar.height / 2 - 15, 96, 30 }, "Hết hàng", MUTED, SKY100, F.m13);
    } else if (k.ton <= 5) {
        tag_auto(ar.x + 10, ar.y + 10, 24, "Sắp hết", WARNC, WARNBG, F.m13);
    }
    float y = ar.y + ar.height + 14;
    text(F.m13, hoa(fit(F.m13, p->getbrand(), r.width - 80, 1.0f)), r.x + 14, y, MUTED, 1.0f);
    // chấm màu
    VECTOR<std::string> mau = p->ds_mau();
    for (int i = 0; i < mau.size() && i < 4; i++) {
        bool biet; Color c = mau_theo_ten(mau[i], &biet);
        float cx = r.x + r.width - 18 - i * 15.0f;
        DrawCircleV(Vector2{ cx, y + 8 }, 6, c);
        DrawRing(Vector2{ cx, y + 8 }, 5.5f, 6.5f, 0, 360, 16, Color{ 0, 0, 0, 40 });
    }
    text_wrap(F.r15, p->getname(), r.x + 14, y + 22, r.width - 28, INK, 2, 20);
    float py = r.y + r.height - 34;
    text(F.b17, tien(p->getprice()), r.x + 14, py, SKY600);
    if (k.da_ban > 0) text_r(F.r13, "Đã bán " + so_gon(k.da_ban), Rectangle{ r.x, py, r.width, 22 }, MUTED, 14);
    if (h && C.pressed) mo_chi_tiet(p->getid());
}

// ============================ CHI TIẾT SẢN PHẨM ============================
void App::them_vao_gio(bool mua_ngay) {
    if (role != R_CUSTOMER) { modal = M_NEED_LOGIN; return; }
    int vt = shop.find(ma_ct);
    if (vt == -1) return;
    product *p = shop.take(vt);
    if (ct_mau.empty() || ct_size.empty()) { toast("Vui lòng chọn màu và kích cỡ.", TOAST_ERR); return; }
    int ton = p->getsl(ct_mau, ct_size);
    int co = 0;
    for (int i = 0; i < gio.so_dong(); i++)
        if (gio.lay(i).idsp == ma_ct && gio.lay(i).color == ct_mau && gio.lay(i).size == ct_size) co += gio.lay(i).sl;
    if (ct_sl + co > ton) { toast("Chỉ còn " + std::to_string(ton) + " sản phẩm cho lựa chọn này (trong giỏ đã có " + std::to_string(co) + ").", TOAST_ERR); return; }
    gio.them(ma_ct, ct_mau, ct_size, ct_sl);
    if (mua_ngay) doi_man(S_CART);
    else toast("Đã thêm vào giỏ hàng", TOAST_OK);
}

void App::ve_chi_tiet(Rectangle view) {
    float W = (float)C.sw;
    float cw = fminf(W - 64, 1180), x0 = (W - cw) / 2;
    float y = view.y - cuon + 22;
    int vt = shop.find(ma_ct);
    if (vt == -1) { doi_man(S_HOME); return; }
    product *p = shop.take(vt);

    // đường dẫn
    float bx = x0;
    auto mon = [&](const std::string &t, bool bam_duoc, bool &bam) {
        float w = tw(F.r15, t);
        Rectangle r = { bx, y, w, 22 };
        bool h = bam_duoc && hover(r);
        if (h) C.hand = true;
        text(F.r15, t, bx, y, h ? SKY600 : MUTED);
        bam = h && C.pressed;
        bx += w;
    };
    bool b;
    mon("Trang chủ", true, b); if (b) { xoa_loc(); doi_man(S_HOME); return; }
    text(F.r15, "  /  ", bx, y, MUTED); bx += tw(F.r15, "  /  ");
    mon(p->getsubtype(), true, b); if (b) { xoa_loc(); tc.subtype = p->getsubtype(); can_loc = true; doi_man(S_HOME); return; }
    text(F.r15, "  /  ", bx, y, MUTED); bx += tw(F.r15, "  /  ");
    text(F.r15, fit(F.r15, p->getname(), cw - (bx - x0) - 10), bx, y, INK);
    y += 44;

    // ----- cột trái: ảnh -----
    float lw = 540;
    int n_anh = anh_dem(p->getid());
    if (ct_anh >= n_anh) ct_anh = 0;
    Rectangle ar = { x0, y, lw, lw * 1.2f };
    panel(ar, 22);
    Rectangle ai = { ar.x + 10, ar.y + 10, ar.width - 20, ar.height - 20 };
    const Texture2D *t = n_anh > 0 ? anh_lay(p->getid(), ct_anh + 1, 900) : nullptr;
    if (t) ve_anh(*t, ai, 16, SURFACE);
    else ve_anh_trong(ai, 16, p->getbrand(), p->getname());
    if (n_anh > 1) {
        Rectangle l = { ai.x + 12, ai.y + ai.height / 2 - 22, 44, 44 }, r = { ai.x + ai.width - 56, ai.y + ai.height / 2 - 22, 44, 44 };
        rrect(l, 22, Fade(WHITE, 0.85f)); rrect(r, 22, Fade(WHITE, 0.85f));
        if (icon_button(l, IC_LEFT, INK, SKY100, 20)) ct_anh = (ct_anh + n_anh - 1) % n_anh;
        if (icon_button(r, IC_RIGHT, INK, SKY100, 20)) ct_anh = (ct_anh + 1) % n_anh;
        float ty = ar.y + ar.height + 16;
        for (int i = 0; i < n_anh && i < 6; i++) {
            Rectangle tr = { x0 + i * 92.0f, ty, 80, 96 };
            bool h = hover(tr);
            if (h) C.hand = true;
            const Texture2D *tt = anh_lay(p->getid(), i + 1, 240);
            rrect(tr, 14, SURFACE);
            if (tt) ve_anh(*tt, Rectangle{ tr.x + 3, tr.y + 3, tr.width - 6, tr.height - 6 }, 11, SURFACE);
            rrect_line(tr, 14, i == ct_anh ? 2.4f : 1.0f, i == ct_anh ? SKY500 : LINE);
            if (h && C.pressed) ct_anh = i;
        }
    }
    float cao_trai = ar.height + (n_anh > 1 ? 16 + 96 : 0);

    // ----- cột phải: thông tin -----
    float rx = x0 + lw + 56, rw = cw - lw - 56;
    float ry = y;
    text(F.m13, hoa(p->getbrand()), rx, ry, SKY600, 2.4f);
    ry += 26;
    int dong = text_wrap(F.s30, p->getname(), rx, ry, rw, INK, 4, 38);
    ry += dong * 38.0f + 10;
    text(F.b35, tien(p->getprice()), rx, ry, INK);
    ry += 48;
    int ban = shop.lay_doanh_thu().da_ban(p->getid());
    float tx = rx;
    if (ban > 0) tx = tag_auto(tx, ry, 26, "Đã bán " + so_gon(ban), SKY600, SKY100, F.m13);
    std::string b_t = bst.cua(p->getid());
    if (!b_t.empty()) tx = tag_auto(tx, ry, 26, "BST " + b_t, Color{ 120, 110, 200, 255 }, LAVBG, F.m13);
    if (ban > 0 || !b_t.empty()) ry += 40;
    if (role == R_CUSTOMER) {   // giá theo hạng
        const HangTV &h = lay_hang(chi_tieu_khach());
        long long giam = tinh_giam_gia_dong(h, p->getprice(), 1);
        if (giam > 0) {
            Rectangle gr = { rx, ry, rw, 44 };
            rrect(gr, 12, OKBG);
            text_l(F.m15, std::string("Hạng ") + h.ten + " (giảm " + std::to_string(h.phan_tram) + "%): chỉ còn " + tien(p->getprice() - giam), gr, Color{ 40, 130, 96, 255 }, 16);
            ry += 58;
        }
    }
    hline(rx, ry, rw);
    ry += 18;
    auto thong_tin = [&](const char *nhan, const std::string &gt) {
        text(F.r15, nhan, rx, ry, MUTED);
        text(F.m15, fit(F.m15, gt, rw - 130), rx + 130, ry, INK);
        ry += 28;
    };
    thong_tin("Mã sản phẩm", p->getid());
    thong_tin("Loại", p->getsubtype() + (p->getkieu() == p->getsubtype() ? "" : "  /  " + p->getkieu()));
    thong_tin("Chất liệu", p->xuat_thong_tin().empty() ? "-" : (dynamic_cast<clothing *>(p) ? dynamic_cast<clothing *>(p)->getmaterial() : (dynamic_cast<accessory *>(p) ? dynamic_cast<accessory *>(p)->getmaterial() : "-")));
    if (accessory *pk = dynamic_cast<accessory *>(p)) {
    if (!pk->getdimension().empty() && pk->getdimension() != "-") thong_tin("Kích thước", pk->getdimension());
    }
    ry += 6;
    hline(rx, ry, rw);
    ry += 18;

    // màu
    text(F.m13, "MÀU SẮC", rx, ry, MUTED, 1.6f);
    ry += 24;
    VECTOR<std::string> mau = p->ds_mau();
    float cx = rx;
    for (int i = 0; i < mau.size(); i++) {
        bool het = true;
        for (int k = 0; k < p->so_variant(); k++) if (p->get_variant(k).color == mau[i] && p->get_variant(k).sl > 0) het = false;
        float w = tw(F.m15, mau[i]) + 56;
        if (cx + w > rx + rw) { cx = rx; ry += 48; }
        Rectangle r = { cx, ry, w, 40 };
        bool chon = ct_mau == mau[i];
        bool h = hover(r);
        if (h) C.hand = true;
        rrect(r, 20, chon ? SKY50 : SURFACE);
        rrect_line(r, 20, chon ? 2.0f : 1.2f, chon ? SKY500 : (h ? SKY300 : LINE));
        bool biet; Color mc = mau_theo_ten(mau[i], &biet);
        DrawCircleV(Vector2{ r.x + 22, r.y + 20 }, 9, mc);
        DrawRing(Vector2{ r.x + 22, r.y + 20 }, 8.5f, 9.5f, 0, 360, 16, Color{ 0, 0, 0, 45 });
        text_l(F.m15, mau[i], Rectangle{ r.x + 40, r.y, w - 40, r.height }, het ? Color{ 170, 185, 200, 255 } : INK);
        if (h && C.pressed) {
            ct_mau = mau[i];
            VECTOR<std::string> sz = p->ds_size(ct_mau);
            bool co = false;
            for (int k = 0; k < sz.size(); k++) if (sz[k] == ct_size && p->getsl(ct_mau, sz[k]) > 0) co = true;
            if (!co) { ct_size.clear(); for (int k = 0; k < sz.size(); k++) if (p->getsl(ct_mau, sz[k]) > 0) { ct_size = sz[k]; break; } }
            ct_sl = 1;
        }
        cx += w + 10;
    }
    ry += 58;

    // size
    text(F.m13, "KÍCH CỠ", rx, ry, MUTED, 1.6f);
    ry += 24;
    VECTOR<std::string> sz = p->ds_size(ct_mau);
    cx = rx;
    for (int i = 0; i < sz.size(); i++) {
        int ton_o = p->getsl(ct_mau, sz[i]);
        float w = fmaxf(54.0f, tw(F.m15, sz[i]) + 32);
        if (cx + w > rx + rw) { cx = rx; ry += 48; }
        Rectangle r = { cx, ry, w, 40 };
        bool chon = ct_size == sz[i];
        bool h = ton_o > 0 && hover(r);
        if (h) C.hand = true;
        rrect(r, 12, chon ? SKY500 : (ton_o > 0 ? SURFACE : SKY50));
        if (!chon) rrect_line(r, 12, 1.2f, h ? SKY300 : LINE);
        text_c(F.m15, sz[i], r, chon ? WHITE : (ton_o > 0 ? INK : Color{ 175, 190, 205, 255 }));
        if (ton_o <= 0) DrawLineEx(Vector2{ r.x + 8, r.y + r.height - 8 }, Vector2{ r.x + r.width - 8, r.y + 8 }, 1.2f, Color{ 190, 205, 220, 255 });
        if (h && C.pressed) { ct_size = sz[i]; ct_sl = 1; }
        cx += w + 10;
    }
    ry += 58;

    int ton = (!ct_mau.empty() && !ct_size.empty()) ? p->getsl(ct_mau, ct_size) : 0;
    text(F.r15, ct_size.empty() ? "Chọn màu và kích cỡ" : (ton > 0 ? "Còn " + std::to_string(ton) + " sản phẩm" : "Hết hàng"), rx, ry, ton > 0 && ton <= 5 ? WARNC : MUTED);
    ry += 32;
    stepper(Rectangle{ rx, ry, 140, 48 }, ct_sl, 1, ton > 0 ? (ton > 99 ? 99 : ton) : 1);
    bool dung = ton > 0;
    if (button(Rectangle{ rx + 160, ry, (rw - 160 - 12) / 2, 48 }, "THÊM VÀO GIỎ", BTN_OUTLINE, F.m15, dung, 24, 1.2f)) them_vao_gio(false);
    if (button(Rectangle{ rx + 160 + (rw - 160 - 12) / 2 + 12, ry, (rw - 160 - 12) / 2, 48 }, "MUA NGAY", BTN_PRIMARY, F.m15, dung, 24, 1.2f)) them_vao_gio(true);
    ry += 48 + 20;

    float ykt = y + fmaxf(cao_trai, ry - y) + 50;

    // sản phẩm cùng danh mục
    if (cung_ma_cu != p->getid() || cung_n_cu != shop.size()) {
        cung_ids.clear();
        TieuChiLoc c2; c2.subtype = p->getsubtype();
        VECTOR<KetQuaLoc> ds_cung = loc_cua_hang(shop, bst, c2);
        for (int i = 0; i < ds_cung.size() && cung_ids.size() < 4; i++)
            if (ds_cung[i].sp != p) cung_ids.push_back(ds_cung[i].sp->getid());
        cung_ma_cu = p->getid(); cung_n_cu = shop.size();
    }
    VECTOR<product *> cung;
    for (int i = 0; i < cung_ids.size(); i++) { int v = shop.find(cung_ids[i]); if (v >= 0) cung.push_back(shop.take(v)); }
    if (cung.size() > 0) {
        text(F.s22, "Có thể bạn cũng thích", x0, ykt, INK);
        ykt += 44;
        float gap = 22, cwid = (cw - gap * 3) / 4;
        float cao_the = (cwid - 16) * 1.2f + 16 + 112;
        for (int i = 0; i < cung.size(); i++) {
            KetQuaLoc k; k.sp = cung[i]; k.da_ban = shop.lay_doanh_thu().da_ban(cung[i]->getid()); k.ton = cung[i]->getsl();
            Rectangle rc = { x0 + i * (cwid + gap), ykt, cwid, cao_the };
            if (rc.y + rc.height >= TOP_H && rc.y <= C.sh) ve_the_sp(rc, k);
        }
        ykt += cao_the;
    }
    float yy = ve_chan_trang(ykt + 50, x0, cw);
    cao_nd = yy - (view.y - cuon);
}

// ============================ GIỎ HÀNG + ĐẶT HÀNG ============================
void App::dat_hang() {
    int d, m, y;
    ngay_hom_nay(d, m, y);
    int dong_loi = -1;
    HoaDon hd;
    KetQuaThanhToan r = shop.thanh_toan(gio, ma, "", d, m, y, hd, &dong_loi);
    if (r == TT_OK) {
        hd_cuoi = hd;
        gio.lam_rong();
        can_loc = true;
        modal = M_INVOICE;
        return;
    }
    std::string d_s = dong_loi >= 0 ? " (sản phẩm thứ " + std::to_string(dong_loi + 1) + " trong giỏ)" : "";
    switch (r) {
        case TT_THIEU_HANG: toast("Không đủ hàng" + d_s + ". Vui lòng giảm số lượng.", TOAST_ERR); break;
        case TT_KHONG_CO_SP: case TT_KHONG_CO_BIEN_THE: toast("Sản phẩm" + d_s + " không còn bán.", TOAST_ERR); break;
        case TT_LOI_GHI_FILE: toast("Không ghi được dữ liệu, đơn chưa được tạo. Thử lại sau.", TOAST_ERR); break;
        default: toast("Không đặt được hàng, vui lòng thử lại.", TOAST_ERR); break;
    }
}

void App::ve_gio_hang(Rectangle view) {
    float W = (float)C.sw;
    float cw = fminf(W - 64, 1180), x0 = (W - cw) / 2;
    float y = view.y - cuon + 22;
    text(F.s30, "Giỏ hàng của bạn", x0, y, INK);
    y += 58;
    if (gio.empty()) {
        Rectangle r = { x0, y, cw, 320 };
        panel(r, 20);
        DrawCircleV(Vector2{ r.x + cw / 2, r.y + 100 }, 52, SKY100);
        icon(IC_CART, r.x + cw / 2, r.y + 100, 52, SKY400, 3.0f);
        text_c(F.s22, "Giỏ hàng đang trống", Rectangle{ r.x, r.y + 172, cw, 30 }, INK);
        if (button(Rectangle{ r.x + cw / 2 - 110, r.y + 226, 220, 48 }, "TIẾP TỤC MUA SẮM", BTN_PRIMARY, F.m15, true, 24, 1.2f)) doi_man(S_HOME);
        cao_nd = ve_chan_trang(r.y + r.height + 60, x0, cw) - (view.y - cuon);
        return;
    }
    float lw = cw - 380 - 28;
    float cao_dong = 150;
    Rectangle lr = { x0, y, lw, gio.so_dong() * cao_dong + 8 };
    panel(lr, 20);
    long long tam = 0, giam = 0;
    const HangTV &h = lay_hang(chi_tieu_khach());
    int xoa_i = -1;
    for (int i = 0; i < gio.so_dong(); i++) {
        DongGio d = gio.lay(i);
        int vt = shop.find(d.idsp);
        float ry = lr.y + 4 + i * cao_dong;
        if (i > 0) hline(lr.x + 24, ry, lr.width - 48);
        if (vt == -1) { xoa_i = i; continue; }
        product *p = shop.take(vt);
        Rectangle ir = { lr.x + 24, ry + 18, 92, 112 };
        const Texture2D *t = anh_dem(d.idsp) > 0 ? anh_lay(d.idsp, 1, 320) : nullptr;
        if (t) ve_anh(*t, ir, 12, SURFACE); else ve_anh_trong(ir, 12, p->getbrand(), "");
        float tx = ir.x + ir.width + 20;
        float tw_ = lr.width - (tx - lr.x) - 190;
        Rectangle nm = { tx, ry + 18, tw_, 44 };
        if (hover(nm)) { C.hand = true; if (C.pressed) { mo_chi_tiet(d.idsp); return; } }
        text(F.m13, hoa(p->getbrand()), tx, ry + 18, MUTED, 1.0f);
        text_wrap(F.m15, p->getname(), tx, ry + 38, tw_, INK, 2, 20);
        text(F.r13, "Màu: " + d.color + "   Kích cỡ: " + d.size, tx, ry + 84, MUTED);
        int ton = p->getsl(d.color, d.size);
        int sl = d.sl;
        if (stepper(Rectangle{ tx, ry + 104, 124, 34 }, sl, 1, ton > 0 ? ton : 1)) gio.doi_sl(i, sl);
        if (d.sl > ton) text(F.r13, ton > 0 ? "Chỉ còn " + std::to_string(ton) : "Đã hết hàng", tx + 134, ry + 112, DANGER);
        long long dong_tam = p->getprice() * d.sl, dong_giam = tinh_giam_gia_dong(h, p->getprice(), d.sl);
        tam += dong_tam; giam += dong_giam;
        Rectangle gr = { lr.x + lr.width - 170, ry + 18, 146, 24 };
        text_r(F.b17, tien(dong_tam - dong_giam), gr, INK, 0);
        if (dong_giam > 0) text_r(F.r13, tien(dong_tam), Rectangle{ gr.x, gr.y + 26, gr.width, 20 }, MUTED, 0);
        else text_r(F.r13, tien(p->getprice()) + " / cái", Rectangle{ gr.x, gr.y + 26, gr.width, 20 }, MUTED, 0);
        if (icon_button(Rectangle{ lr.x + lr.width - 56, ry + 96, 36, 36 }, IC_TRASH, MUTED, DANGBG, 20)) xoa_i = i;
    }
    if (xoa_i >= 0) gio.xoa(xoa_i);

    // tóm tắt đơn hàng
    Rectangle sr = { x0 + lw + 28, y, 380, 360 };
    panel(sr, 20);
    float sy = sr.y + 24;
    text(F.s22, "Tóm tắt đơn hàng", sr.x + 26, sy, INK);
    sy += 52;
    auto dong_tt = [&](const std::string &a, const std::string &b, Color cb) {
        text(F.r15, a, sr.x + 26, sy, TEXT);
        text_r(F.m15, b, Rectangle{ sr.x, sy - 2, sr.width, 22 }, cb, 26);
        sy += 32;
    };
    dong_tt("Tạm tính", tien(tam), INK);
    dong_tt(std::string("Giảm giá hạng ") + h.ten + " (" + std::to_string(h.phan_tram) + "%)", giam > 0 ? "-" + tien(giam) : "0₫", giam > 0 ? OKC : MUTED);
    hline(sr.x + 26, sy + 2, sr.width - 52);
    sy += 16;
    text(F.m17, "Thanh toán", sr.x + 26, sy + 4, INK);
    text_r(F.b21, tien(tam - giam), Rectangle{ sr.x, sy, sr.width, 28 }, SKY600, 26);
    sy += 50;
    long long thieu = con_thieu_de_len_hang(chi_tieu_khach());
    if (thieu > 0) {
        int hi = chi_so_hang(chi_tieu_khach());
        Rectangle b = { sr.x + 26, sy, sr.width - 52, 44 };
        rrect(b, 12, SKY50);
        text_wrap(F.r13, "Mua thêm " + tien(thieu) + " để lên hạng " + BANG_HANG[hi + 1].ten + ".", b.x + 12, b.y + 5, b.width - 24, SKY600, 2, 17);
        sy += 56;
    }
    if (button(Rectangle{ sr.x + 26, sy, sr.width - 52, 52 }, "ĐẶT HÀNG", BTN_PRIMARY, F.m17, true, 26, 2.0f)) dat_hang();
    if (button(Rectangle{ sr.x + 26, sy + 62, sr.width - 52, 42 }, "Tiếp tục mua sắm", BTN_GHOST, F.m15, true, 21)) doi_man(S_HOME);
    float ydone = fmaxf(lr.y + lr.height, sr.y + 360) + 60;
    cao_nd = ve_chan_trang(ydone, x0, cw) - (view.y - cuon);
}

// ============================ TÀI KHOẢN CỦA KHÁCH ============================
void App::ve_tai_khoan(Rectangle view) {
    float W = (float)C.sw;
    float cw = fminf(W - 64, 1100), x0 = (W - cw) / 2;
    float y = view.y - cuon + 22;
    if (role != R_CUSTOMER) { doi_man(S_HOME); return; }
    int ik = qk.tim_theo_ma(ma);
    if (ik == -1) { dang_xuat(); return; }
    const KhachHang &kh = qk.lay(ik);
    long long chi = chi_tieu_khach();
    const HangTV &h = lay_hang(chi);

    text(F.s30, "Tài khoản của tôi", x0, y, INK);
    y += 60;
    // thanh bên
    Rectangle sb = { x0, y, 250, 3 * 54 + 20 };
    panel(sb, 18);
    const char *tabs[3] = { "Hồ sơ & hạng thành viên", "Đơn mua", "Đổi mật khẩu" };
    Icon ics[3] = { IC_USER, IC_BAG, IC_LOCK };
    for (int i = 0; i < 3; i++) {
        Rectangle r = { sb.x + 10, sb.y + 10 + i * 54.0f, sb.width - 20, 48 };
        bool hv = hover(r);
        if (hv) C.hand = true;
        if (tk_tab == i) rrect(r, 12, SKY100); else if (hv) rrect(r, 12, SKY50);
        icon(ics[i], r.x + 26, r.y + 24, 20, tk_tab == i ? SKY600 : TEXT, 1.7f);
        text_l(F.m15, tabs[i], Rectangle{ r.x + 48, r.y, r.width - 48, r.height }, tk_tab == i ? SKY600 : TEXT);
        if (hv && C.pressed) tk_tab = i;
    }
    float cx = x0 + 250 + 28, cwid = cw - 250 - 28;
    float cy = y, cao = 400;

    if (tk_tab == 0) {
        Rectangle pr = { cx, cy, cwid, 210 };
        panel(pr, 20);
        DrawCircleV(Vector2{ pr.x + 64, pr.y + 66 }, 40, SKY200);
        std::string ch = kh.getTenKH().empty() ? "?" : to_utf8(std::u32string(1, to_u32(kh.getTenKH())[0]));
        text_c(F.b27, hoa(ch), Rectangle{ pr.x + 24, pr.y + 26, 80, 80 }, SKY600);
        text(F.s22, kh.getTenKH(), pr.x + 124, pr.y + 36, INK);
        text(F.r15, "Mã khách: " + kh.getMaKH(), pr.x + 124, pr.y + 68, MUTED);
        float yy = pr.y + 114;
        text(F.r15, "Điện thoại", pr.x + 28, yy, MUTED); text(F.m15, kh.getSoDienThoai(), pr.x + 130, yy, INK);
        text(F.r15, "Email", pr.x + 28, yy + 28, MUTED); text(F.m15, kh.getEmail().empty() ? "-" : fit(F.m15, kh.getEmail(), 260), pr.x + 130, yy + 28, INK);
        text(F.r15, "Địa chỉ", pr.x + 28, yy + 56, MUTED); text(F.m15, kh.getDiaChi().empty() ? "-" : fit(F.m15, kh.getDiaChi(), cwid - 170), pr.x + 130, yy + 56, INK);
        cy += 210 + 22;
        // thẻ hạng
        Rectangle hr = { cx, cy, cwid, 210 };
        rrect(hr, 20, SKY500);
        push_clip(hr);
        DrawCircleV(Vector2{ hr.x + hr.width - 40, hr.y + 20 }, 150, Fade(WHITE, 0.12f));
        DrawCircleV(Vector2{ hr.x + hr.width - 160, hr.y + hr.height + 30 }, 110, Fade(WHITE, 0.1f));
        pop_clip();
        text(F.m13, "HẠNG THÀNH VIÊN", hr.x + 28, hr.y + 26, Fade(WHITE, 0.8f), 2.0f);
        text(F.s40, h.ten, hr.x + 28, hr.y + 50, WHITE);
        text(F.r15, "Giảm " + std::to_string(h.phan_tram) + "% cho mỗi đơn hàng", hr.x + 28, hr.y + 104, WHITE);
        text(F.r15, "Tổng chi tiêu: " + tien(chi), hr.x + 28, hr.y + 130, WHITE);
        int hi = chi_so_hang(chi);
        if (hi + 1 < SO_HANG_TV) {
            long long tu = BANG_HANG[hi].nguong, den = BANG_HANG[hi + 1].nguong;
            float tl = den > tu ? (float)(chi - tu) / (float)(den - tu) : 1.0f;
            Rectangle bar = { hr.x + 28, hr.y + 164, hr.width - 56, 10 };
            rrect(bar, 5, Fade(WHITE, 0.3f));
            rrect(Rectangle{ bar.x, bar.y, bar.width * fminf(1.0f, tl), bar.height }, 5, WHITE);
            text(F.r13, "Còn " + tien(den - chi) + " để lên hạng " + BANG_HANG[hi + 1].ten, bar.x, bar.y + 18, Fade(WHITE, 0.9f));
        } else text(F.r15, "Bạn đang ở hạng cao nhất!", hr.x + 28, hr.y + 164, WHITE);
        cao = 210 + 22 + 210;
    } else if (tk_tab == 1) {
        VECTOR<HoaDon> ds = ds_hoa_don_cua_khach(shop.lay_doanh_thu(), ma);
        if (ds.size() == 0) {
            Rectangle pr = { cx, cy, cwid, 240 };
            panel(pr, 20);
            icon(IC_BAG, pr.x + cwid / 2, pr.y + 80, 56, SKY300, 2.6f);
            text_c(F.s22, "Bạn chưa có đơn hàng nào", Rectangle{ pr.x, pr.y + 130, cwid, 30 }, INK);
            if (button(Rectangle{ pr.x + cwid / 2 - 100, pr.y + 176, 200, 44 }, "Mua sắm ngay", BTN_PRIMARY, F.m15, true, 22)) doi_man(S_HOME);
            cao = 240;
        } else {
            float yy = cy;
            for (int i = 0; i < ds.size(); i++) {
                const HoaDon &hd = ds[i];
                bool mo = hd_mo == hd.ma_hd;
                float cao_the = mo ? 96 + hd.dong.size() * 34.0f + 24 : 84;
                Rectangle r = { cx, yy, cwid, cao_the };
                panel(r, 18);
                Rectangle dau = { r.x, r.y, r.width, 84 };
                bool hv = hover(dau);
                if (hv) { C.hand = true; if (C.pressed) hd_mo = mo ? "" : hd.ma_hd; }
                text(F.b17, hd.ma_hd, r.x + 24, r.y + 18, INK);
                text(F.r13, dinh_dang_ngay(hd.thoi_gian) + "   -   " + std::to_string(hd.dong.size()) + " dòng hàng", r.x + 24, r.y + 46, MUTED);
                text_r(F.b17, tien(hd.thanh_toan()), Rectangle{ r.x, r.y + 16, r.width - 56, 26 }, SKY600, 0);
                if (hd.giam_gia() > 0) text_r(F.r13, "đã giảm " + tien(hd.giam_gia()), Rectangle{ r.x, r.y + 46, r.width - 56, 20 }, OKC, 0);
                icon(mo ? IC_UP : IC_DOWN, r.x + r.width - 32, r.y + 42, 16, MUTED, 1.8f);
                if (mo) {
                    hline(r.x + 24, r.y + 84, r.width - 48);
                    for (int k = 0; k < hd.dong.size(); k++) {
                        const GiaoDich &g = hd.dong[k];
                        float ly = r.y + 96 + k * 34.0f;
                        text(F.r15, fit(F.r15, g.ten, r.width - 330), r.x + 24, ly, TEXT);
                        text(F.r13, g.color + " / " + g.size + "  x" + std::to_string(g.sl), r.x + r.width - 300, ly + 2, MUTED);
                        text_r(F.m15, tien(g.thanh_tien()), Rectangle{ r.x, ly, r.width, 22 }, INK, 24);
                    }
                }
                yy += cao_the + 14;
            }
            cao = yy - cy;
        }
    } else {
        Rectangle pr = { cx, cy, cwid, 410 };
        panel(pr, 20);
        text(F.s22, "Đổi mật khẩu", pr.x + 28, pr.y + 24, INK);
        float fy = pr.y + 76;
        bool gui = false;
        text(F.m13, "MẬT KHẨU HIỆN TẠI", pr.x + 28, fy, MUTED, 1.2f); if (field(f_mk_cu, Rectangle{ pr.x + 28, fy + 20, 420, 46 })) gui = true; fy += 82;
        text(F.m13, "MẬT KHẨU MỚI", pr.x + 28, fy, MUTED, 1.2f); if (field(f_mk_moi, Rectangle{ pr.x + 28, fy + 20, 420, 46 })) gui = true; fy += 82;
        text(F.m13, "NHẬP LẠI MẬT KHẨU MỚI", pr.x + 28, fy, MUTED, 1.2f); if (field(f_mk_moi2, Rectangle{ pr.x + 28, fy + 20, 420, 46 })) gui = true; fy += 86;
        if (button(Rectangle{ pr.x + 28, fy, 200, 48 }, "Cập nhật", BTN_PRIMARY, F.m15, true, 24) || gui) {
            if (f_mk_moi.utf8() != f_mk_moi2.utf8()) toast("Hai ô mật khẩu mới chưa giống nhau.", TOAST_ERR);
            else if (qk.doi_mat_khau(ma, f_mk_cu.utf8(), f_mk_moi.utf8())) {
                toast("Đã đổi mật khẩu.", TOAST_OK);
                f_mk_cu.clear(); f_mk_moi.clear(); f_mk_moi2.clear();
            } else toast("Mật khẩu hiện tại sai hoặc mật khẩu mới quá ngắn (tối thiểu 6 ký tự).", TOAST_ERR);
        }
        cao = 410;
    }
    float yy = fmaxf(y + cao, y + sb.height) + 60;
    cao_nd = ve_chan_trang(yy, x0, cw) - (view.y - cuon);
}
