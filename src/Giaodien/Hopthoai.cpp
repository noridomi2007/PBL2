// CÁC HỘP THOẠI
#include "App.h"
#include "../Dialog/Dialog.h"
#include <cmath>
using namespace ui;

// ============================ HÓA ĐƠN ============================
void App::ve_hd_modal() {
    const HoaDon &hd = hd_cuoi;
    int n = hd.dong.size(), hien = n < 7 ? n : 7;
    Rectangle r;
    float h = 66 + 60 + 112 + 40 + hien * 52 + (n > hien ? 28 : 0) + 140 + 90;
    if (khung_hop_thoai(r, 780, h, "Hóa đơn " + hd.ma_hd)) { modal = M_NONE; return; }
    float y = r.y + 82;
    Rectangle bn = { r.x + 28, y, r.width - 56, 44 };
    rrect(bn, 12, OKBG);
    icon(IC_CHECK, bn.x + 24, bn.y + 22, 18, Color{ 40, 130, 96, 255 }, 2.2f);
    text_l(F.m15, "Thanh toán thành công", bn, Color{ 40, 130, 96, 255 }, 46);
    y += 60;
    std::string ten_kh = "";
    int ik = qk.tim_theo_ma(hd.ma_kh);
    if (ik != -1) ten_kh = "  -  " + qk.lay(ik).getTenKH();
    text(F.r15, "Ngày", r.x + 28, y, MUTED); text(F.m15, dinh_dang_ngay(hd.thoi_gian), r.x + 140, y, INK);
    text(F.r15, "Khách hàng", r.x + 28, y + 28, MUTED); text(F.m15, fit(F.m15, hd.ma_kh + ten_kh, r.width - 190), r.x + 140, y + 28, INK);
    text(F.r15, "Nhân viên", r.x + 28, y + 56, MUTED); text(F.m15, hd.ma_nv.empty() ? "Khách tự mua trực tuyến" : hd.ma_nv, r.x + 140, y + 56, INK);
    y += 96;
    rrect(Rectangle{ r.x + 28, y, r.width - 56, 32 }, 10, SKY100);
    float cw = r.width - 56;
    text_l(F.m13, "SẢN PHẨM", Rectangle{ r.x + 40, y, 200, 32 }, SKY600, 0, 1.2f);
    text_r(F.m13, "SL", Rectangle{ r.x + 28 + cw * 0.58f, y, 40, 32 }, SKY600, 0, 1.2f);
    text_r(F.m13, "ĐƠN GIÁ", Rectangle{ r.x + 28 + cw * 0.76f, y, 80, 32 }, SKY600, 0, 1.2f);
    text_r(F.m13, "THÀNH TIỀN", Rectangle{ r.x + 28, y, cw, 32 }, SKY600, 14, 1.2f);
    y += 40;
    for (int i = 0; i < hien; i++) {
        const GiaoDich &g = hd.dong[i];
        text(F.m15, fit(F.m15, g.ten, cw * 0.54f), r.x + 40, y + 4, INK);
        text(F.r13, g.idsp + "  -  " + g.color + " / " + g.size, r.x + 40, y + 26, MUTED);
        text_r(F.r15, std::to_string(g.sl), Rectangle{ r.x + 28 + cw * 0.58f, y, 40, 40 }, TEXT, 0);
        text_r(F.r15, tien(g.don_gia), Rectangle{ r.x + 28 + cw * 0.76f, y, 80, 40 }, TEXT, 0);
        text_r(F.m15, tien(g.thanh_tien()), Rectangle{ r.x + 28, y, cw, 40 }, INK, 14);
        y += 52;
    }
    if (n > hien) { text(F.r13, "... và " + std::to_string(n - hien) + " dòng hàng khác", r.x + 40, y, MUTED); y += 28; }
    hline(r.x + 28, y, cw);
    y += 14;
    text(F.r15, "Tạm tính", r.x + 40, y, TEXT); text_r(F.m15, tien(hd.tam_tinh()), Rectangle{ r.x + 28, y - 2, cw, 22 }, INK, 14); y += 28;
    text(F.r15, "Giảm giá theo hạng", r.x + 40, y, TEXT); text_r(F.m15, hd.giam_gia() > 0 ? "-" + tien(hd.giam_gia()) : "0₫", Rectangle{ r.x + 28, y - 2, cw, 22 }, hd.giam_gia() > 0 ? OKC : MUTED, 14); y += 34;
    text(F.m17, "Tổng thanh toán", r.x + 40, y + 2, INK); text_r(F.b21, tien(hd.thanh_toan()), Rectangle{ r.x + 28, y - 2, cw, 28 }, SKY600, 14);
    float by = r.y + r.height - 70;
    if (role == R_CUSTOMER) {
        if (button(Rectangle{ r.x + 28, by, 200, 46 }, "Xem đơn mua", BTN_PRIMARY, F.m15, true, 23)) { modal = M_NONE; tk_tab = 1; hd_mo = hd.ma_hd; doi_man(S_ACCOUNT); }
        if (button(Rectangle{ r.x + 244, by, 200, 46 }, "Tiếp tục mua sắm", BTN_OUTLINE, F.m15, true, 23)) { modal = M_NONE; xoa_loc(); doi_man(S_HOME); }
    } else {
        if (button(Rectangle{ r.x + r.width - 168, by, 140, 46 }, "Đóng", BTN_PRIMARY, F.m15, true, 23)) modal = M_NONE;
    }
}

// ============================ CHỌN MÀU / SIZE (bán tại quầy) ============================
void App::ve_chon_bien_the() {
    int vt = shop.find(pk_ma);
    if (vt == -1) { modal = M_NONE; return; }
    product *p = shop.take(vt);
    Rectangle r;
    if (khung_hop_thoai(r, 560, 470, "Chọn màu và kích cỡ")) { modal = M_NONE; return; }
    text(F.m15, fit(F.m15, p->getname(), r.width - 56), r.x + 28, r.y + 82, INK);
    text(F.b17, tien(p->getprice()), r.x + 28, r.y + 108, SKY600);
    float y = r.y + 148;
    text(F.m13, "MÀU SẮC", r.x + 28, y, MUTED, 1.6f); y += 24;
    VECTOR<std::string> mau = p->ds_mau();
    float cx = r.x + 28;
    for (int i = 0; i < mau.size(); i++) {
        float w = tw(F.m15, mau[i]) + 36;
        if (cx + w > r.x + r.width - 28) { cx = r.x + 28; y += 46; }
        if (chip(Rectangle{ cx, y, w, 38 }, mau[i], pk_mau == mau[i], F.m15)) {
            pk_mau = mau[i]; pk_size.clear();
            VECTOR<std::string> sz = p->ds_size(pk_mau);
            for (int k = 0; k < sz.size(); k++) if (p->getsl(pk_mau, sz[k]) > 0) { pk_size = sz[k]; break; }
            pk_sl = 1;
        }
        cx += w + 8;
    }
    y += 62;
    text(F.m13, "KÍCH CỠ  (số trong ngoặc là tồn kho)", r.x + 28, y, MUTED, 1.2f); y += 24;
    VECTOR<std::string> sz = p->ds_size(pk_mau);
    cx = r.x + 28;
    for (int i = 0; i < sz.size(); i++) {
        int ton = p->getsl(pk_mau, sz[i]);
        std::string t = sz[i] + " (" + std::to_string(ton) + ")";
        float w = tw(F.m15, t) + 32;
        if (cx + w > r.x + r.width - 28) { cx = r.x + 28; y += 46; }
        bool chon = pk_size == sz[i];
        if (ton > 0) { if (chip(Rectangle{ cx, y, w, 38 }, t, chon, F.m15)) { pk_size = sz[i]; pk_sl = 1; } }
        else { rrect(Rectangle{ cx, y, w, 38 }, 19, SKY50); text_c(F.m15, t, Rectangle{ cx, y, w, 38 }, Color{ 175, 190, 205, 255 }); }
        cx += w + 8;
    }
    y += 62;
    int ton = (!pk_mau.empty() && !pk_size.empty()) ? p->getsl(pk_mau, pk_size) : 0;
    stepper(Rectangle{ r.x + 28, y, 140, 46 }, pk_sl, 1, ton > 0 ? ton : 1);
    if (button(Rectangle{ r.x + 188, y, r.width - 216, 46 }, "THÊM VÀO ĐƠN", BTN_PRIMARY, F.m15, ton > 0, 23, 1.2f)) {
        int co = 0;
        for (int i = 0; i < gio_pos.so_dong(); i++) if (gio_pos.lay(i).idsp == pk_ma && gio_pos.lay(i).color == pk_mau && gio_pos.lay(i).size == pk_size) co += gio_pos.lay(i).sl;
        if (co + pk_sl > ton) toast("Chỉ còn " + std::to_string(ton) + " (trong đơn đã có " + std::to_string(co) + ").", TOAST_ERR);
        else { gio_pos.them(pk_ma, pk_mau, pk_size, pk_sl); modal = M_NONE; }
    }
}

// ============================ THÊM SẢN PHẨM ============================
void App::mo_them_sp() {
    ap_hang = 0; ap_kieu = 0; ap_rows = 1; ap_anh.clear(); md_cuon = 0;
    f_ap_hang_khac.clear(); f_ap_ten.clear(); f_ap_kieu_ct.clear(); f_ap_gia.clear(); f_ap_chat_lieu.clear(); f_ap_bst.clear();
    for (int i = 0; i < 6; i++) { f_ap_mau[i].clear(); f_ap_size[i].clear(); f_ap_sl[i].clear(); }
    modal = M_ADD_PRODUCT;
}

static const char *loi_them(KetQuaThem k) {
    switch (k) {
        case THEM_OK: return "";
        case THEM_THIEU_THONG_TIN: return "Thiếu thông tin: cần hãng, tên, giá và ít nhất 1 biến thể.";
        case THEM_SAI_KIEU: return "Kiểu này không thuộc nhóm quần áo / giày dép.";
        case THEM_KY_TU_CAM: return "Có ký tự | hoặc xuống dòng trong thông tin.";
        case THEM_LOI_ANH: return "Chưa chọn ảnh, hoặc không đọc / lưu được ảnh.";
        case THEM_TRUNG_MA: return "Trùng mã hoặc đã hết mã cho hãng + kiểu này.";
        case THEM_LOI_GHI_FILE: return "Không ghi được file, đã hoàn tác.";
        case THEM_QUA_DAI: return "Có ô nhập dài hơn mức cho phép.";
    }
    return "";
}

void App::gui_them_sp() {
    std::string hang = ap_hang < SO_HANG_SP ? DS_HANG_SP[ap_hang].ten : f_ap_hang_khac.utf8();
    int dem = 0, chi_so[16];
    for (int i = 0; i < SO_KIEU_SP; i++) if (la_ma_kieu_quan_ao(DS_KIEU_SP[i].ma)) chi_so[dem++] = i;
    std::string kieu = DS_KIEU_SP[chi_so[ap_kieu < dem ? ap_kieu : 0]].ten;
    VECTOR<Variant> bt;
    for (int i = 0; i < ap_rows; i++) {
        if (f_ap_mau[i].empty() && f_ap_size[i].empty() && f_ap_sl[i].empty()) continue;
        bt.push_back(Variant(cat_khoang_trang(f_ap_mau[i].utf8()), cat_khoang_trang(f_ap_size[i].utf8()), (int)f_ap_sl[i].number()));
    }
    VECTOR<std::string> anh;
    for (const std::string &a : ap_anh) anh.push_back(a);
    std::string id;
    KetQuaThem kq_ = shop.them_quan_ao(cat_khoang_trang(hang), kieu, cat_khoang_trang(f_ap_ten.utf8()), cat_khoang_trang(f_ap_kieu_ct.utf8()),
                                      (long long)f_ap_gia.number(), cat_khoang_trang(f_ap_chat_lieu.utf8()), bt, anh, &id);
    if (kq_ != THEM_OK) { toast(loi_them(kq_), TOAST_ERR); return; }
    std::string b = f_ap_bst.utf8();
    if (!cat_khoang_trang(b).empty() && bst.dat(id, b) != BST_OK) toast("Đã thêm sản phẩm nhưng chưa gán được bộ sưu tập.", TOAST_ERR);
    anh_xoa_dem(id);
    pos_dirty = true; can_loc = true;
    toast("Đã thêm sản phẩm " + id, TOAST_OK);
    modal = M_NONE;
}

void App::ve_them_sp_modal() {
    Rectangle r;
    if (khung_hop_thoai(r, 780, 860, "Thêm sản phẩm mới")) { modal = M_NONE; return; }
    Rectangle nd = { r.x, r.y + 70, r.width, r.height - 70 - 84 };
    float toi_da = md_cao - nd.height;
    if (toi_da < 0) toi_da = 0;
    if (hover(nd)) md_cuon -= C.wheel * 60.0f;
    if (md_cuon < 0) md_cuon = 0;
    if (md_cuon > toi_da) md_cuon = toi_da;
    float x = r.x + 28, w = r.width - 56, y = nd.y + 18 - md_cuon;
    push_clip(nd);
    text(F.m13, "HÃNG", x, y, MUTED, 1.6f); y += 24;
    float cx = x;
    for (int i = 0; i <= SO_HANG_SP; i++) {
        std::string t = i < SO_HANG_SP ? DS_HANG_SP[i].ten : "Hãng khác";
        float cwid = tw(F.m13, t) + 28;
        if (cx + cwid > x + w) { cx = x; y += 40; }
        if (chip(Rectangle{ cx, y, cwid, 32 }, t, ap_hang == i, F.m13)) ap_hang = i;
        cx += cwid + 8;
    }
    y += 42;
    if (ap_hang >= SO_HANG_SP) { field(f_ap_hang_khac, Rectangle{ x, y, 300, 44 }, F.r15); y += 54; }
    text(F.m13, "KIỂU (CHỈ NHÓM QUẦN ÁO / GIÀY DÉP)", x, y, MUTED, 1.4f); y += 24;
    cx = x;
    int idx = 0;
    for (int i = 0; i < SO_KIEU_SP; i++) {
        if (!la_ma_kieu_quan_ao(DS_KIEU_SP[i].ma)) continue;
        float cwid = tw(F.m13, DS_KIEU_SP[i].ten) + 28;
        if (chip(Rectangle{ cx, y, cwid, 32 }, DS_KIEU_SP[i].ten, ap_kieu == idx, F.m13)) ap_kieu = idx;
        cx += cwid + 8; idx++;
    }
    y += 50;
    text(F.m13, "TÊN SẢN PHẨM", x, y, MUTED, 1.4f); field(f_ap_ten, Rectangle{ x, y + 20, w, 44 }, F.r15); y += 76;
    float hw = (w - 14) / 2;
    text(F.m13, "KIỂU CHI TIẾT", x, y, MUTED, 1.4f); text(F.m13, "GIÁ BÁN (₫)", x + hw + 14, y, MUTED, 1.4f);
    field(f_ap_kieu_ct, Rectangle{ x, y + 20, hw, 44 }, F.r15); field(f_ap_gia, Rectangle{ x + hw + 14, y + 20, hw, 44 }, F.r15); y += 76;
    text(F.m13, "CHẤT LIỆU", x, y, MUTED, 1.4f); text(F.m13, "BỘ SƯU TẬP", x + hw + 14, y, MUTED, 1.4f);
    field(f_ap_chat_lieu, Rectangle{ x, y + 20, hw, 44 }, F.r15); field(f_ap_bst, Rectangle{ x + hw + 14, y + 20, hw, 44 }, F.r15); y += 76;
    VECTOR<std::string> ds_bst = bst.ds_ten();
    if (ds_bst.size() > 0) {
        text(F.r13, "Có sẵn:", x, y + 6, MUTED);
        float bx = x + 56;
        for (int i = 0; i < ds_bst.size(); i++) {
            float cwid = tw(F.m13, ds_bst[i]) + 24;
            if (bx + cwid > x + w) break;
            if (chip(Rectangle{ bx, y, cwid, 28 }, ds_bst[i], false, F.m13)) f_ap_bst.set(ds_bst[i]);
            bx += cwid + 6;
        }
        y += 40;
    }
    text(F.m13, "BIẾN THỂ (MÀU / SIZE / SỐ LƯỢNG)", x, y, MUTED, 1.4f); y += 24;
    for (int i = 0; i < ap_rows; i++) {
        field(f_ap_mau[i], Rectangle{ x, y, w * 0.4f, 42 }, F.r15);
        field(f_ap_size[i], Rectangle{ x + w * 0.4f + 10, y, w * 0.28f, 42 }, F.r15);
        field(f_ap_sl[i], Rectangle{ x + w * 0.68f + 20, y, w * 0.32f - 20, 42 }, F.r15);
        y += 50;
    }
    if (ap_rows < 6 && button(Rectangle{ x, y, 150, 36 }, "+ Thêm dòng", BTN_SOFT, F.m13, true, 18)) ap_rows++;
    y += 52;
    text(F.m13, "ẢNH SẢN PHẨM (TỐI ĐA " + std::to_string(MAX_ANH_MOI_SP) + ", ÍT NHẤT 1)", x, y, MUTED, 1.4f); y += 24;
    if (button(Rectangle{ x, y, 170, 42 }, "Chọn ảnh...", BTN_OUTLINE, F.m15, true, 21)) {
        std::vector<std::string> chon = mo_hop_thoai_chon_anh(MAX_ANH_MOI_SP);
        if (!chon.empty()) ap_anh = chon;
    }
    text_l(F.r15, ap_anh.empty() ? "Chưa chọn ảnh nào" : "Đã chọn " + std::to_string(ap_anh.size()) + " ảnh", Rectangle{ x + 186, y, 300, 42 }, ap_anh.empty() ? MUTED : OKC);
    y += 60;
    md_cao = y - (nd.y + 18 - md_cuon) + 20;
    pop_clip();
    scroll_bar(nd, md_cao, md_cuon);
    hline(r.x + 28, r.y + r.height - 84, r.width - 56);
    if (button(Rectangle{ r.x + r.width - 208, r.y + r.height - 68, 180, 50 }, "LƯU SẢN PHẨM", BTN_PRIMARY, F.m15, true, 25, 1.2f)) gui_them_sp();
    if (button(Rectangle{ r.x + r.width - 316, r.y + r.height - 68, 96, 50 }, "Hủy", BTN_GHOST, F.m15, true, 25)) modal = M_NONE;
}

// ============================ NHẬP KHO / KIỂM KÊ ============================
void App::ve_nhap_kho_modal() {
    int vt = shop.find(md_ma);
    if (vt == -1) { modal = M_NONE; return; }
    product *p = shop.take(vt);
    Rectangle r;
    float cao = 490 + ((p->so_variant() + 2) / 3) * 38.0f;
    if (khung_hop_thoai(r, 620, cao, "Nhập kho / kiểm kê")) { modal = M_NONE; return; }
    float x = r.x + 28, w = r.width - 56, y = r.y + 82;
    text(F.m15, fit(F.m15, p->getname(), w), x, y, INK);
    text(F.r13, p->getid() + "  -  tổng tồn " + std::to_string(p->getsl()), x, y + 24, MUTED);
    y += 58;
    if (chip(Rectangle{ x, y, 150, 36 }, "Nhập thêm hàng", nk_che_do == 0, F.m15)) nk_che_do = 0;
    if (chip(Rectangle{ x + 160, y, 220, 36 }, "Kiểm kê (đặt số lượng)", nk_che_do == 1, F.m15)) nk_che_do = 1;
    y += 52;
    text(F.m13, "CHỌN NHANH Ô ĐÃ CÓ", x, y, MUTED, 1.4f); y += 24;
    float cx = x;
    for (int i = 0; i < p->so_variant(); i++) {
        const Variant &v = p->get_variant(i);
        std::string t = v.color + " / " + v.size + ": " + std::to_string(v.sl);
        float cwid = tw(F.m13, t) + 24;
        if (cx + cwid > x + w) { cx = x; y += 38; }
        if (chip(Rectangle{ cx, y, cwid, 30 }, t, false, F.m13)) { f_nk_mau.set(v.color); f_nk_size.set(v.size); }
        cx += cwid + 6;
    }
    y += 46;
    float cw3 = (w - 20) / 3;
    text(F.m13, "MÀU", x, y, MUTED, 1.4f); text(F.m13, "SIZE", x + cw3 + 10, y, MUTED, 1.4f); text(F.m13, nk_che_do == 0 ? "SỐ LƯỢNG NHẬP THÊM" : "SỐ LƯỢNG MỚI", x + 2 * (cw3 + 10), y, MUTED, 1.0f);
    bool gui = false;
    if (field(f_nk_mau, Rectangle{ x, y + 20, cw3, 44 }, F.r15)) gui = true;
    if (field(f_nk_size, Rectangle{ x + cw3 + 10, y + 20, cw3, 44 }, F.r15)) gui = true;
    if (field(f_nk_sl, Rectangle{ x + 2 * (cw3 + 10), y + 20, cw3, 44 }, F.r15)) gui = true;
    y += 80;
    text(F.r13, nk_che_do == 0 ? "Màu / size chưa có sẽ được tạo thành biến thể mới." : "Kiểm kê chỉ sửa được ô đã có (có thể đặt về 0).", x, y, MUTED);
    if (button(Rectangle{ x, r.y + r.height - 68, w, 50 }, nk_che_do == 0 ? "NHẬP KHO" : "CẬP NHẬT SỐ LƯỢNG", BTN_PRIMARY, F.m15, true, 25, 1.2f) || gui) {
        KetQuaKho k;
        if (f_nk_sl.empty()) { toast("Hãy nhập số lượng.", TOAST_ERR); return; }
        if (nk_che_do == 0) k = shop.nhap_kho(md_ma, f_nk_mau.utf8(), f_nk_size.utf8(), (int)f_nk_sl.number());
        else k = shop.kiem_ke(md_ma, f_nk_mau.utf8(), f_nk_size.utf8(), (int)f_nk_sl.number());
        switch (k) {
            case KHO_OK: toast(nk_che_do == 0 ? "Đã nhập kho." : "Đã cập nhật số lượng.", TOAST_OK); f_nk_sl.clear(); pos_dirty = true; can_loc = true; break;
            case KHO_KHONG_CO_BIEN_THE: toast("Không có ô màu / size này để kiểm kê.", TOAST_ERR); break;
            case KHO_SAI_SO_LUONG: toast("Số lượng không hợp lệ (nhập > 0; tối đa 99.999 mỗi ô).", TOAST_ERR); break;
            case KHO_THIEU_THONG_TIN: toast("Cần nhập cả màu và size.", TOAST_ERR); break;
            case KHO_KY_TU_CAM: toast("Không dùng ký tự | trong màu / size.", TOAST_ERR); break;
            case KHO_QUA_DAI: toast("Màu hoặc size quá dài.", TOAST_ERR); break;
            default: toast("Không ghi được file, đã hoàn tác.", TOAST_ERR); break;
        }
    }
}

// ============================ SỬA GIÁ / BỘ SƯU TẬP ============================
void App::ve_sua_sp_modal() {
    int vt = shop.find(md_ma);
    if (vt == -1) { modal = M_NONE; return; }
    product *p = shop.take(vt);
    Rectangle r;
    if (khung_hop_thoai(r, 600, 420, "Sửa giá / bộ sưu tập")) { modal = M_NONE; return; }
    float x = r.x + 28, w = r.width - 56, y = r.y + 82;
    text(F.m15, fit(F.m15, p->getname(), w), x, y, INK);
    text(F.r13, p->getid(), x, y + 24, MUTED);
    y += 58;
    text(F.m13, "GIÁ BÁN (₫)", x, y, MUTED, 1.4f);
    field(f_sp_gia, Rectangle{ x, y + 20, w, 44 }, F.r15);
    y += 80;
    text(F.m13, "BỘ SƯU TẬP", x, y, MUTED, 1.4f);
    field(f_sp_bst, Rectangle{ x, y + 20, w, 44 }, F.r15);
    y += 70;
    VECTOR<std::string> ds_bst = bst.ds_ten();
    float cx = x;
    for (int i = 0; i < ds_bst.size(); i++) {
        float cwid = tw(F.m13, ds_bst[i]) + 24;
        if (cx + cwid > x + w) { cx = x; y += 34; }
        if (chip(Rectangle{ cx, y, cwid, 28 }, ds_bst[i], false, F.m13)) f_sp_bst.set(ds_bst[i]);
        cx += cwid + 6;
    }
    if (button(Rectangle{ x, r.y + r.height - 68, w, 50 }, "LƯU THAY ĐỔI", BTN_PRIMARY, F.m15, true, 25, 1.2f)) {
        KetQuaKho k = shop.dat_gia(md_ma, (long long)f_sp_gia.number());
        KetQuaBST b = bst.dat(md_ma, f_sp_bst.utf8());
        if (k == KHO_OK && b == BST_OK) { toast("Đã lưu thay đổi.", TOAST_OK); pos_dirty = true; can_loc = true; modal = M_NONE; }
        else if (b == BST_QUA_DAI) toast("Tên bộ sưu tập quá dài (tối đa 28 ký tự).", TOAST_ERR);
        else if (b == BST_KY_TU_CAM) toast("Không dùng ký tự | trong tên bộ sưu tập.", TOAST_ERR);
        else toast("Không lưu được, vui lòng kiểm tra lại giá.", TOAST_ERR);
    }
}

void App::ve_xoa_sp_modal() {
    int vt = shop.find(md_ma);
    if (vt == -1) { modal = M_NONE; return; }
    product *p = shop.take(vt);
    Rectangle r;
    if (khung_hop_thoai(r, 520, 290, "Xóa sản phẩm?")) { modal = M_NONE; return; }
    text_wrap(F.r15, "Xóa \"" + p->getname() + "\" (" + p->getid() + ")? Ảnh sản phẩm cũng bị xóa. Doanh thu đã ghi vẫn được giữ nguyên.", r.x + 28, r.y + 86, r.width - 56, TEXT, 4, 22);
    if (button(Rectangle{ r.x + 28, r.y + 210, 200, 48 }, "Xóa sản phẩm", BTN_DANGER, F.m15, true, 24)) {
        std::string id = md_ma;
        if (shop.xoa_san_pham(id)) {
            bst.dat(id, "");
            anh_xoa_dem(id);
            kho_mo.clear(); pos_dirty = true; can_loc = true;
            toast("Đã xóa sản phẩm " + id, TOAST_OK);
        } else toast("Không xóa được (lỗi ghi file).", TOAST_ERR);
        modal = M_NONE;
    }
    if (button(Rectangle{ r.x + 244, r.y + 210, 120, 48 }, "Hủy", BTN_OUTLINE, F.m15, true, 24)) modal = M_NONE;
}

// ============================ ĐĂNG KÝ KHÁCH Ở QUẦY ============================
void App::ve_khach_moi_modal() {
    Rectangle r;
    if (khung_hop_thoai(r, 560, 590, "Đăng ký khách hàng mới")) { modal = M_NONE; return; }
    float x = r.x + 28, w = r.width - 56, y = r.y + 84;
    bool gui = false;
    struct D { const char *nh; Field *f; };
    D ds[] = { { "HỌ VÀ TÊN", &f_nk_ten }, { "SỐ ĐIỆN THOẠI", &f_nk_sdt }, { "EMAIL", &f_nk_mail }, { "ĐỊA CHỈ", &f_nk_dc }, { "MẬT KHẨU (TRỐNG = DÙNG SỐ ĐIỆN THOẠI)", &f_nk_mk } };
    for (D &d : ds) { text(F.m13, d.nh, x, y, MUTED, 1.2f); if (field(*d.f, Rectangle{ x, y + 20, w, 42 }, F.r15)) gui = true; y += 72; }
    if (button(Rectangle{ x, r.y + r.height - 68, w, 50 }, "TẠO TÀI KHOẢN", BTN_PRIMARY, F.m15, true, 25, 1.2f) || gui) {
        std::string mk = f_nk_mk.utf8().empty() ? f_nk_sdt.utf8() : f_nk_mk.utf8();
        std::string m;
        KetQuaDangKy k = qk.dang_ky(f_nk_ten.utf8(), f_nk_sdt.utf8(), mk, f_nk_mail.utf8(), f_nk_dc.utf8(), &m);
        if (k == DK_OK) {
            toast("Đã tạo khách " + m, TOAST_OK);
            if (screen == S_POS) { pos_kh = qk.tim_theo_ma(m); }
            f_nk_ten.clear(); f_nk_sdt.clear(); f_nk_mail.clear(); f_nk_dc.clear(); f_nk_mk.clear();
            modal = M_NONE;
        } else {
            const char *t = k == DK_THIEU_THONG_TIN ? "Cần nhập họ tên." : k == DK_SDT_SAI ? "Số điện thoại phải đủ 10 số, bắt đầu bằng 0." :
                            k == DK_SDT_TRUNG ? "Số điện thoại này đã có tài khoản." : k == DK_MAT_KHAU_NGAN ? "Mật khẩu tối thiểu 6 ký tự." :
                            k == DK_QUA_DAI ? "Tên / email / địa chỉ quá dài." : "Không tạo được tài khoản.";
            toast(t, TOAST_ERR);
        }
    }
}

// ============================ THÊM NHÂN VIÊN ============================
void App::ve_nv_moi_modal() {
    Rectangle r;
    if (khung_hop_thoai(r, 540, 430, "Thêm nhân viên")) { modal = M_NONE; return; }
    float x = r.x + 28, w = r.width - 56, y = r.y + 84;
    bool gui = false;
    text(F.m13, "HỌ VÀ TÊN", x, y, MUTED, 1.2f); if (field(f_nv_ten, Rectangle{ x, y + 20, w, 44 }, F.r15)) gui = true; y += 76;
    text(F.m13, "SỐ ĐIỆN THOẠI (LÀ MẬT KHẨU MẶC ĐỊNH)", x, y, MUTED, 1.2f); if (field(f_nv_sdt, Rectangle{ x, y + 20, w, 44 }, F.r15)) gui = true; y += 76;
    text(F.m13, "VAI TRÒ", x, y, MUTED, 1.2f);
    if (chip(Rectangle{ x, y + 20, 130, 36 }, "Nhân viên", nv_vai_tro == 0, F.m15)) nv_vai_tro = 0;
    if (chip(Rectangle{ x + 140, y + 20, 100, 36 }, "Admin", nv_vai_tro == 1, F.m15)) nv_vai_tro = 1;
    if (button(Rectangle{ x, r.y + r.height - 68, w, 50 }, "TẠO NHÂN VIÊN", BTN_PRIMARY, F.m15, true, 25, 1.2f) || gui) {
        std::string m;
        KetQuaTaoNV k = qn.tao_nhan_vien(f_nv_ten.utf8(), f_nv_sdt.utf8(), nv_vai_tro ? VAI_TRO_ADMIN : VAI_TRO_NHAN_VIEN, "", "", "", &m);
        if (k == NV_OK) { toast("Đã tạo " + m + ". Mật khẩu mặc định = số điện thoại.", TOAST_OK); modal = M_NONE; }
        else toast(k == NV_SDT_SAI ? "Số điện thoại phải đủ 10 số, bắt đầu bằng 0." : k == NV_SDT_TRUNG ? "Số điện thoại này đã được dùng." :
                   k == NV_THIEU_THONG_TIN ? "Cần nhập họ tên." : "Không tạo được nhân viên.", TOAST_ERR);
    }
}

// ============================ CHI TIẾT KHÁCH (hóa đơn đã mua) ============================
void App::ve_khach_info_modal() {
    int ik = qk.tim_theo_ma(kh_xem);
    if (ik == -1) { modal = M_NONE; return; }
    const KhachHang &k = qk.lay(ik);
    VECTOR<HoaDon> ds = ds_hoa_don_cua_khach(shop.lay_doanh_thu(), kh_xem);
    int hien = ds.size() < 8 ? ds.size() : 8;
    Rectangle r;
    if (khung_hop_thoai(r, 700, 330 + hien * 58.0f, k.getTenKH())) { modal = M_NONE; return; }
    float x = r.x + 28, w = r.width - 56, y = r.y + 82;
    long long chi = shop.lay_doanh_thu().tong_chi_tieu_khach(kh_xem);
    const HangTV &h = lay_hang(chi);
    text(F.r15, "Mã", x, y, MUTED); text(F.m15, k.getMaKH(), x + 110, y, INK);
    text(F.r15, "Điện thoại", x, y + 26, MUTED); text(F.m15, k.getSoDienThoai(), x + 110, y + 26, INK);
    text(F.r15, "Email", x, y + 52, MUTED); text(F.m15, k.getEmail().empty() ? "-" : fit(F.m15, k.getEmail(), w - 120), x + 110, y + 52, INK);
    text(F.r15, "Địa chỉ", x, y + 78, MUTED); text(F.m15, k.getDiaChi().empty() ? "-" : fit(F.m15, k.getDiaChi(), w - 120), x + 110, y + 78, INK);
    text(F.r15, "Đăng ký", x, y + 104, MUTED); text(F.m15, dinh_dang_ngay(k.getNgayDangKy()), x + 110, y + 104, INK);
    tag_auto(x + w - 220, y, 28, std::string("Hạng ") + h.ten + " - " + std::to_string(h.phan_tram) + "%", SKY600, SKY100, F.m13);
    text_r(F.b17, "Chi tiêu: " + tien(chi), Rectangle{ x, y + 40, w, 26 }, INK, 0);
    y += 140;
    text(F.m13, "ĐƠN HÀNG (MỚI NHẤT TRƯỚC) - BẤM ĐỂ XEM", x, y, MUTED, 1.2f);
    y += 26;
    if (ds.size() == 0) text(F.r15, "Khách này chưa mua gì.", x, y + 8, MUTED);
    for (int i = 0; i < hien; i++) {
        Rectangle row = { x, y, w, 50 };
        bool hv = hover(row);
        rrect(row, 12, hv ? SKY100 : SKY50);
        if (hv) { C.hand = true; if (C.pressed) { hd_cuoi = ds[i]; modal = M_INVOICE; return; } }
        text_l(F.m15, ds[i].ma_hd, Rectangle{ row.x + 16, row.y, 110, 50 }, INK);
        text_l(F.r15, dinh_dang_ngay(ds[i].thoi_gian), Rectangle{ row.x + 140, row.y, 120, 50 }, TEXT);
        text_r(F.m15, tien(ds[i].thanh_toan()), row, SKY600, 16);
        y += 58;
    }
}
