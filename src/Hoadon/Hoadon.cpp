#include "Hoadon.h"
#include "../Tienich/Tienich.h"
#include <sstream>

// ---------------- GIỎ HÀNG ----------------
bool GioHang::them(const string &idsp, const string &color, const string &size, int sl) {
    if (sl <= 0 || idsp.empty()) return false;
    for (int i = 0; i < ds.size(); i++) {
        if (ds[i].idsp == idsp && ds[i].color == color && ds[i].size == size) {
            ds[i].sl += sl;
            return true;
        }
    }
    DongGio d;
    d.idsp = idsp; d.color = color; d.size = size; d.sl = sl;
    ds.push_back(d);
    return true;
}

bool GioHang::doi_sl(int i, int sl) {
    if (i < 0 || i >= ds.size()) return false;
    if (sl <= 0) return xoa(i);
    ds[i].sl = sl;
    return true;
}

bool GioHang::xoa(int i) {
    if (i < 0 || i >= ds.size()) return false;
    ds.remove(i);
    return true;
}

// ---------------- HÓA ĐƠN ----------------
long long HoaDon::tam_tinh() const {
    long long t = 0;
    for (int i = 0; i < dong.size(); i++) t += dong[i].tam_tinh();
    return t;
}
long long HoaDon::giam_gia() const {
    long long t = 0;
    for (int i = 0; i < dong.size(); i++) t += dong[i].giam_gia;
    return t;
}
long long HoaDon::thanh_toan() const { return tam_tinh() - giam_gia(); }

// Tra hóa đơn bằng BẢNG BĂM theo mã: không còn quét cả sổ doanh thu
HoaDon lay_hoa_don(const DoanhThu &sodt, const string &ma_hd) {
    HoaDon hd;
    const VECTOR<int> *cac_dong = sodt.dong_cua_hoa_don(ma_hd);
    if (!cac_dong) return hd;
    for (int i = 0; i < cac_dong->size(); i++) {
        const GiaoDich &g = sodt.lay((*cac_dong)[i]);
        if (hd.dong.empty()) { hd.ma_hd = g.ma_hd; hd.ma_kh = g.ma_kh; hd.ma_nv = g.ma_nv; hd.thoi_gian = g.thoi_gian; }
        hd.dong.push_back(g);
    }
    return hd;
}

// Bảng băm thứ hai (mã khách -> danh sách liên kết đơn các MÃ hóa đơn): có mã rồi thì lay_hoa_don tra bằng bảng băm ~O(1)
VECTOR<HoaDon> ds_hoa_don_cua_khach(const DoanhThu &sodt, const string &ma_kh) {
    VECTOR<HoaDon> kq;
    const DSLK_DON<string> *cac_ma = sodt.hoa_don_cua_khach(ma_kh);
    if (!cac_ma) return kq;
    for (const string &ma : *cac_ma) kq.push_back(lay_hoa_don(sodt, ma));   // duyệt danh sách từ đầu: hóa đơn ghi sau cùng đứng trước
    // Xếp lại theo NGÀY BÁN, mới nhất trước (cần vì có thể nhập bù hóa đơn ngày cũ). Danh sách đã gần đúng thứ tự nên chạy rất nhanh.
    for (int i = 1; i < kq.size(); i++) {
        HoaDon x = kq[i];
        int j = i - 1;
        while (j >= 0 && kq[j].thoi_gian < x.thoi_gian) { kq[j + 1] = kq[j]; j--; }
        kq[j + 1] = x;
    }
    return kq;
}

// Độ rộng các cột khi in hóa đơn (tổng 76 ký tự)
static const int RONG_IN_MA = 10, RONG_IN_MAUSIZE = 18, RONG_IN_SL = 4, RONG_IN_GIA = 14, RONG_IN_GIAM = 14, RONG_IN_TIEN = 15;
static const int RONG_IN_TONG = RONG_IN_MA + RONG_IN_MAUSIZE + RONG_IN_SL + RONG_IN_GIA + RONG_IN_GIAM + RONG_IN_TIEN + 5;

string in_hoa_don(const HoaDon &hd) {
    stringstream ss;
    string ke(RONG_IN_TONG, '-');
    ss << "================ HÓA ĐƠN " << hd.ma_hd << " ================\n"
       << "Ngày: " << dinh_dang_ngay(hd.thoi_gian) << "\n"
       << "Khách: " << hd.ma_kh << "   Nhân viên: " << (hd.ma_nv.empty() ? "(khách tự mua)" : hd.ma_nv) << "\n"
       << ke << "\n"
       << can_trai("Mã SP", RONG_IN_MA) << " " << can_trai("Màu / Size", RONG_IN_MAUSIZE) << " "
       << can_phai("SL", RONG_IN_SL) << " " << can_phai("Đơn giá", RONG_IN_GIA) << " "
       << can_phai("Giảm", RONG_IN_GIAM) << " " << can_phai("Thành tiền", RONG_IN_TIEN) << "\n"
       << ke << "\n";
    for (int i = 0; i < hd.dong.size(); i++) {
        const GiaoDich &g = hd.dong[i];
        ss << g.ten << "\n"                                           // tên dài nên cho riêng 1 dòng
           << can_trai(g.idsp, RONG_IN_MA) << " " << can_trai(g.color + " / " + g.size, RONG_IN_MAUSIZE) << " "
           << can_phai(to_string(g.sl), RONG_IN_SL) << " " << can_phai(dinh_dang_tien(g.don_gia), RONG_IN_GIA) << " "
           << can_phai(dinh_dang_tien(g.giam_gia), RONG_IN_GIAM) << " " << can_phai(dinh_dang_tien(g.thanh_tien()), RONG_IN_TIEN) << "\n";
    }
    int rong_nhan = RONG_IN_TONG - RONG_IN_TIEN - 1;
    ss << ke << "\n"
       << can_phai("Tạm tính (VNĐ):", rong_nhan) << " " << can_phai(dinh_dang_tien(hd.tam_tinh()), RONG_IN_TIEN) << "\n"
       << can_phai("Giảm giá (VNĐ):", rong_nhan) << " " << can_phai(dinh_dang_tien(hd.giam_gia()), RONG_IN_TIEN) << "\n"
       << can_phai("THANH TOÁN (VNĐ):", rong_nhan) << " " << can_phai(dinh_dang_tien(hd.thanh_toan()), RONG_IN_TIEN) << "\n";
    return ss.str();
}
