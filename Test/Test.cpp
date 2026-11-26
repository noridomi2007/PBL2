// test/test_main.cpp
// Chương trình CONSOLE để thử toàn bộ chức năng (không mở cửa sổ raylib).
// Biên dịch:  mingw32-make test      Chạy:  .\test.exe   (đứng ở thư mục gốc PBL2)
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <filesystem>
#include "../src/Shop/Shop.h"
#include "../src/Dialog/Dialog.h"
#include "../src/Customer/Quanlykhachhang.h"
#include "../src/Customer/Hangthanhvien.h"
#include "../src/Staff/Quanlynhanvien.h"
#include "../src/Tienich/Tienich.h"
#include "../src/Cautrucdulieu/List.h"
#include "../src/Cautrucdulieu/Slist.h"
#include "../src/Cautrucdulieu/Hash.h"
#include "../src/Cautrucdulieu/Heap.h"

using namespace std;
namespace fs = std::filesystem;

static const char *F_ITEM = "Data/item.txt";
static const char *F_DT   = "Data/doanhthu.txt";
static const char *F_KH   = "Data/khachhang.txt";
static const char *F_NV   = "Data/nhanvien.txt";

// Phiên đăng nhập hiện tại
struct Phien {
    int loai = 0;      // 0 chưa đăng nhập, 1 khách, 2 nhân viên, 3 admin
    string ma;         // mã KH hoặc mã NV
    string ten;
};

// =========================================================================
// TIỆN ÍCH NHẬP / XUẤT
// =========================================================================
static string cat(const string &s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

static string doc_dong(const string &hoi) {
    cout << hoi;
    string s;
    if (!getline(cin, s)) { cout << "\n(het du lieu nhap, thoat)\n"; exit(0); }
    return cat(s);
}

static long long doc_so(const string &hoi, long long mac_dinh = -1) {
    string s = doc_dong(hoi);
    if (s.empty()) return mac_dinh;
    try { size_t pos = 0; long long v = stoll(s, &pos); if (pos == s.size()) return v; } catch (...) {}
    cout << "  (gia tri khong hop le)\n";
    return mac_dinh;
}

static string tien(long long v) { return dinh_dang_tien(v) + " VNĐ"; }

static bool doc_ngay(const string &s, int &d, int &m, int &y) {
    if (s.empty()) { ngay_hom_nay(d, m, y); return true; }
    return sscanf(s.c_str(), "%d/%d/%d", &d, &m, &y) == 3;
}

static void in_san_pham(const product *p) {
    cout << p->getid() << " | " << p->getname() << " | " << p->getbrand() << " | "
         << p->getsubtype() << " / " << p->getkieu() << " | " << tien(p->getprice())
         << " | tồn: " << p->getsl() << " | ảnh: " << dem_anh(p->getid()) << "\n";
    for (int i = 0; i < p->so_variant(); i++) {
        const Variant &v = p->get_variant(i);
        cout << "      - " << v.color << " / " << v.size << " : " << v.sl << "\n";
    }
}

static const char *ten_kq_tt(KetQuaThanhToan k) {
    switch (k) {
        case TT_OK: return "TT_OK (thành công)";
        case TT_GIO_TRONG: return "TT_GIO_TRONG (giỏ hàng trống)";
        case TT_THIEU_KHACH: return "TT_THIEU_KHACH (chưa có khách / tài khoản)";
        case TT_NGAY_SAI: return "TT_NGAY_SAI (ngày không có thật hoặc sau hôm nay)";
        case TT_SO_LUONG_SAI: return "TT_SO_LUONG_SAI (số lượng <= 0)";
        case TT_KHONG_CO_SP: return "TT_KHONG_CO_SP (không có mã này)";
        case TT_KHONG_CO_BIEN_THE: return "TT_KHONG_CO_BIEN_THE (không có ô màu/size này)";
        case TT_THIEU_HANG: return "TT_THIEU_HANG (không đủ hàng)";
        case TT_LOI_GHI_FILE: return "TT_LOI_GHI_FILE (không ghi được file, đã hoàn kho)";
    }
    return "?";
}

static const char *ten_kq_them(KetQuaThem k) {
    switch (k) {
        case THEM_OK: return "THEM_OK (thành công)";
        case THEM_THIEU_THONG_TIN: return "THEM_THIEU_THONG_TIN (thiếu tên/hãng/giá/biến thể)";
        case THEM_SAI_KIEU: return "THEM_SAI_KIEU (kiểu không thuộc nhóm quần áo)";
        case THEM_KY_TU_CAM: return "THEM_KY_TU_CAM (có ký tự | hoặc xuống dòng)";
        case THEM_LOI_ANH: return "THEM_LOI_ANH (không đọc/lưu được ảnh, hoặc chưa chọn ảnh)";
        case THEM_TRUNG_MA: return "THEM_TRUNG_MA (trùng mã hoặc hết mã)";
        case THEM_LOI_GHI_FILE: return "THEM_LOI_GHI_FILE (không ghi được file, đã hoàn tác)";
        case THEM_QUA_DAI: return "THEM_QUA_DAI (một ô nhập dài hơn độ rộng cột cho phép)";
    }
    return "?";
}

static const char *ten_kq_dk(KetQuaDangKy k) {
    switch (k) {
        case DK_OK: return "DK_OK (đăng ký thành công)";
        case DK_THIEU_THONG_TIN: return "DK_THIEU_THONG_TIN (thiếu tên hoặc mật khẩu)";
        case DK_KY_TU_CAM: return "DK_KY_TU_CAM (có ký tự | hoặc xuống dòng)";
        case DK_SDT_SAI: return "DK_SDT_SAI (SĐT phải đủ 10 số, bắt đầu bằng 0)";
        case DK_MAT_KHAU_NGAN: return "DK_MAT_KHAU_NGAN (mật khẩu quá ngắn)";
        case DK_SDT_TRUNG: return "DK_SDT_TRUNG (SĐT đã có tài khoản)";
        case DK_LOI_GHI_FILE: return "DK_LOI_GHI_FILE (không ghi được file)";
        case DK_QUA_DAI: return "DK_QUA_DAI (tên / email / địa chỉ quá dài)";
    }
    return "?";
}

static const char *ten_kq_nv(KetQuaTaoNV k) {
    switch (k) {
        case NV_OK: return "NV_OK (đã tạo nhân viên)";
        case NV_THIEU_THONG_TIN: return "NV_THIEU_THONG_TIN";
        case NV_KY_TU_CAM: return "NV_KY_TU_CAM";
        case NV_SDT_SAI: return "NV_SDT_SAI";
        case NV_SDT_TRUNG: return "NV_SDT_TRUNG";
        case NV_VAI_TRO_SAI: return "NV_VAI_TRO_SAI (chỉ admin hoặc nhanvien)";
        case NV_MAT_KHAU_NGAN: return "NV_MAT_KHAU_NGAN";
        case NV_LOI_GHI_FILE: return "NV_LOI_GHI_FILE";
        case NV_QUA_DAI: return "NV_QUA_DAI (tên / email / địa chỉ quá dài)";
    }
    return "?";
}

// =========================================================================
// SẢN PHẨM
// =========================================================================
static void xem_ds(Shop &shop) {
    cout << "\n--- Có " << shop.size() << " sản phẩm ---\n";
    for (int i = 0; i < shop.size(); i++) in_san_pham(shop.take(i));
}

static void them_sp(Shop &shop) {
    cout << "\nChọn hãng:\n";
    for (int i = 0; i < SO_HANG_SP; i++)
        cout << "  " << i + 1 << ". " << DS_HANG_SP[i].ten << " (" << DS_HANG_SP[i].ma << ")\n";
    cout << "  0. Hãng khác (tự nhập)\n";
    long long c = doc_so("Chọn: ");
    string hang;
    if (c >= 1 && c <= SO_HANG_SP) hang = DS_HANG_SP[c - 1].ten;
    else if (c == 0) hang = doc_dong("Tên hãng: ");
    else { cout << "Huỷ.\n"; return; }

    cout << "\nChọn kiểu (chỉ nhóm quần áo / giày dép):\n";
    int chi_so[20], so = 0;
    for (int i = 0; i < SO_KIEU_SP; i++) {
        if (la_ma_kieu_quan_ao(DS_KIEU_SP[i].ma)) {
            chi_so[so++] = i;
            cout << "  " << so << ". " << DS_KIEU_SP[i].ten << " (mã " << DS_KIEU_SP[i].ma << ")\n";
        }
    }
    c = doc_so("Chọn: ");
    if (c < 1 || c > so) { cout << "Huỷ.\n"; return; }
    string kieu = DS_KIEU_SP[chi_so[c - 1]].ten;

    string ten = doc_dong("Tên sản phẩm: ");
    string kieu_ct = doc_dong("Kiểu chi tiết (vd: Áo thun oversize): ");
    long long gia = doc_so("Giá (VNĐ): ", -1);
    string chat_lieu = doc_dong("Chất liệu: ");

    VECTOR<Variant> bien_the;
    cout << "Nhập các biến thể. MỖI THÔNG TIN MỘT DÒNG. Để trống MÀU để kết thúc.\n";
    while (true) {
        string mau = doc_dong("  Màu: ");
        if (mau.empty()) break;
        string size = doc_dong("  Size: ");
        long long sl = doc_so("  Số lượng: ", 0);
        bien_the.push_back(Variant(mau, size, (int)sl));
    }

    VECTOR<string> anh;
    cout << "Chọn ảnh (tối đa " << MAX_ANH_MOI_SP << "). Phải có ít nhất 1 ảnh hợp lệ thì mới lưu được sản phẩm.\n";
    while (anh.size() < MAX_ANH_MOI_SP) {
        string s = doc_dong("  Enter = mở hộp thoại chọn ảnh | gõ đường dẫn | 'x' = xong: ");
        if (s == "x" || s == "X") break;
        vector<string> chon;
        if (s.empty()) chon = mo_hop_thoai_chon_anh(MAX_ANH_MOI_SP - anh.size());
        else chon.push_back(s);
        for (const string &p : chon) {
            Image im = doc_anh_tu_file(p);
            if (im.data == nullptr) { cout << "    Bỏ qua (không đọc được): " << p << "\n"; continue; }
            UnloadImage(im);
            anh.push_back(p);
            cout << "    + " << p << "\n";
        }
    }

    string ma;
    KetQuaThem kq = shop.them_quan_ao(hang, kieu, ten, kieu_ct, gia, chat_lieu, bien_the, anh, &ma);
    cout << "\nKết quả: " << ten_kq_them(kq) << "\n";
    if (kq == THEM_OK) {
        cout << "Mã mới: " << ma << "  -> ảnh lưu tại " << duong_dan_anh(ma, 1) << " ...\n";
        in_san_pham(shop.take(shop.find(ma)));
    }
}

static void xoa_sp(Shop &shop) {
    string ma = doc_dong("\nMã sản phẩm cần xóa: ");
    int vt = shop.find(ma);
    if (vt == -1) { cout << "Không có mã này.\n"; return; }
    in_san_pham(shop.take(vt));
    string id = shop.take(vt)->getid();
    if (doc_dong("Xóa thật? (y/n): ") != "y") { cout << "Huỷ.\n"; return; }
    bool ok = shop.xoa_san_pham(id);
    cout << "xóa + lưu file: " << (ok ? "OK" : "LOI") << " | số ảnh sau khi xóa: " << dem_anh(id) << " (mong đợi 0)\n";
}

static void tim_kiem(Shop &shop) {
    string tk = doc_dong("\nTừ khóa (Enter = bỏ qua): ");
    VECTOR<string> hangs = shop.lay_ds_hang();
    cout << "Các hãng đang có:\n";
    for (int i = 0; i < hangs.size(); i++) cout << "  " << i + 1 << ". " << hangs[i] << "\n";
    long long c = doc_so("Lọc theo hãng (số, Enter = tất cả): ", 0);
    string hang = (c >= 1 && c <= hangs.size()) ? hangs[c - 1] : "";
    long long gmin = doc_so("Giá tối thiểu (Enter = 0): ", 0);
    long long gmax = doc_so("Giá tối đa (Enter = không giới hạn): ", 0);
    long long sx = doc_so("Sắp xếp (0 mã, 1 giá tăng, 2 giá giảm, 3 tên A-Z): ", 0);
    VECTOR<KetQuaTimKiem> kq = shop.tim_kiem_va_loc(tk, hang, "", "", gmin, gmax, (int)sx);
    cout << "\n--- " << kq.size() << " kết quả ---\n";
    for (int i = 0; i < kq.size(); i++) { cout << (int)kq[i].ti_le_khop << "%  "; in_san_pham(kq[i].sp); }
}

// =========================================================================
// TÀI KHOẢN
// =========================================================================
static void dang_ky_khach(QuanLyKhachHang &qk) {
    cout << "\n--- Đăng ký tài khoản khách (đăng nhập bằng số điện thoại) ---\n";
    string ten = doc_dong("Họ tên: ");
    string sdt = doc_dong("Số điện thoại (10 số, bắt đầu bằng 0): ");
    string mk = doc_dong("Mật khẩu (>= " + to_string(MAT_KHAU_TOI_THIEU) + " ký tự): ");
    string email = doc_dong("Email (Enter = bỏ qua): ");
    string dc = doc_dong("Địa chỉ (Enter = bỏ qua): ");
    string ma;
    KetQuaDangKy kq = qk.dang_ky(ten, sdt, mk, email, dc, &ma);
    cout << "Kết quả: " << ten_kq_dk(kq) << "\n";
    if (kq == DK_OK) cout << "Mã khách của bạn: " << ma << "\n";
}

static void dang_nhap(Phien &p, QuanLyKhachHang &qk, QuanLyNhanVien &qn) {
    cout << "\n  1. Khách hàng (đăng nhập bằng SĐT)   2. Nhân viên / Admin (đăng nhập bằng mã NV)\n";
    long long c = doc_so("Chọn: ");
    if (c == 1) {
        string sdt = doc_dong("SĐT: "), mk = doc_dong("Mật khẩu: ");
        int i = qk.dang_nhap(sdt, mk);
        if (i == -1) { cout << "Sai SĐT / mật khẩu, hoặc tài khoản bị khóa.\n"; return; }
        p.loai = 1; p.ma = qk.lay(i).getMaKH(); p.ten = qk.lay(i).getTenKH();
    } else if (c == 2) {
        string ma = doc_dong("Mã nhân viên (vd NV0001): "), mk = doc_dong("Mật khẩu: ");
        int i = qn.dang_nhap(ma, mk);
        if (i == -1) { cout << "Sai mã / mật khẩu, hoặc nhân viên đã nghỉ.\n"; return; }
        p.loai = qn.lay(i).laAdmin() ? 3 : 2; p.ma = qn.lay(i).getMaNV(); p.ten = qn.lay(i).getTenNV();
    } else return;
    cout << "Đăng nhập thành công: " << p.ten << " (" << p.ma << ") - "
         << (p.loai == 1 ? "Khách" : p.loai == 2 ? "Nhân viên (chỉ thanh toán)" : "Admin") << "\n";
}

static bool can_dang_nhap(const Phien &p) {
    if (p.loai == 0) { cout << "Bạn cần đăng nhập trước (menu 3).\n"; return false; }
    return true;
}
static bool can_admin(const Phien &p) {
    if (p.loai != 3) { cout << "Chức năng này chỉ dành cho ADMIN.\n"; return false; }
    return true;
}

static void doi_mat_khau(const Phien &p, QuanLyKhachHang &qk, QuanLyNhanVien &qn) {
    if (!can_dang_nhap(p)) return;
    string cu = doc_dong("Mật khẩu cũ: "), moi = doc_dong("Mật khẩu mới: ");
    bool ok = (p.loai == 1) ? qk.doi_mat_khau(p.ma, cu, moi) : qn.doi_mat_khau(p.ma, cu, moi);
    cout << (ok ? "Đã đổi mật khẩu.\n" : "Không đổi được (sai mật khẩu cũ hoặc mật khẩu mới quá ngắn).\n");
}

static void quan_ly_nv(QuanLyNhanVien &qn) {
    cout << "\n  1. Danh sách  2. Tạo nhân viên  3. Cấp lại mật khẩu  4. Cho nghỉ / đi làm lại  5. Đổi vai trò\n";
    long long c = doc_so("Chọn: ");
    if (c == 1) {
        for (int i = 0; i < qn.size(); i++) cout << "  " << qn.lay(i).xuatThongTin() << "\n";
    } else if (c == 2) {
        string ten = doc_dong("Họ tên: "), sdt = doc_dong("SĐT: ");
        string vt = doc_dong("Vai trò (admin / nhanvien): ");
        string ma;
        KetQuaTaoNV kq = qn.tao_nhan_vien(ten, sdt, vt, "", "", "", &ma);
        cout << "Kết quả: " << ten_kq_nv(kq) << "\n";
        if (kq == NV_OK) cout << "Mã NV: " << ma << " | mật khẩu mặc định = số điện thoại\n";
    } else if (c == 3) {
        string ma = doc_dong("Mã NV: ");
        cout << (qn.dat_lai_mat_khau(ma) ? "Đã cấp lại: mật khẩu = số điện thoại của nhân viên.\n" : "Không tìm thấy mã này.\n");
    } else if (c == 4) {
        string ma = doc_dong("Mã NV: ");
        bool lam = doc_dong("Đi làm lại (y) hay cho nghỉ (n)? ") == "y";
        cout << (qn.dat_trang_thai(ma, lam) ? "Đã cập nhật.\n" : "Không thực hiện được (không có mã này, hoặc đây là admin cuối cùng).\n");
    } else if (c == 5) {
        string ma = doc_dong("Mã NV: "), vt = doc_dong("Vai trò mới (admin / nhanvien): ");
        cout << (qn.doi_vai_tro(ma, vt) ? "Đã đổi vai trò.\n" : "Không thực hiện được.\n");
    }
}

static void quan_ly_khach(Shop &shop, QuanLyKhachHang &qk) {
    cout << "\n  1. Danh sách khách (kèm tổng chi tiêu, hạng)  2. Cấp lại mật khẩu  3. Khóa / mở tài khoản\n";
    long long c = doc_so("Chọn: ");
    if (c == 1) {
        for (int i = 0; i < qk.size(); i++) {
            long long chi = shop.lay_doanh_thu().tong_chi_tieu_khach(qk.lay(i).getMaKH());
            cout << "  " << qk.lay(i).xuatThongTin() << " | Chi tiêu: " << tien(chi)
                 << " | Hạng: " << lay_hang(chi).ten << " (giảm " << lay_hang(chi).phan_tram << "%)\n";
        }
    } else if (c == 2) {
        string ma = doc_dong("Mã KH: ");
        cout << (qk.dat_lai_mat_khau(ma) ? "Đã cấp lại: mật khẩu = số điện thoại của khách.\n" : "Không tìm thấy mã này.\n");
    } else if (c == 3) {
        string ma = doc_dong("Mã KH: ");
        bool mo = doc_dong("Mở khóa (y) hay khóa (n)? ") == "y";
        cout << (qk.dat_trang_thai(ma, mo) ? "Đã cập nhật.\n" : "Không tìm thấy mã này.\n");
    }
}

// =========================================================================
// MUA HÀNG
// =========================================================================
static void mua_hang(Shop &shop, QuanLyKhachHang &qk, const Phien &p) {
    if (!can_dang_nhap(p)) return;
    string ma_kh, ma_nv;
    if (p.loai == 1) {
        ma_kh = p.ma;
    } else {
        string sdt = doc_dong("SĐT khách hàng (khách phải có tài khoản): ");
        int i = qk.tim_theo_sdt(sdt);
        if (i == -1 || !qk.lay(i).getDangHoatDong()) { cout << "Khách chưa có tài khoản (hoặc đang bị khóa). Hãy đăng ký trước (menu 2).\n"; return; }
        ma_kh = qk.lay(i).getMaKH();
        ma_nv = p.ma;
    }
    long long da_chi = shop.lay_doanh_thu().tong_chi_tieu_khach(ma_kh);
    const HangTV &hang = lay_hang(da_chi);
    cout << "Khách " << ma_kh << " | đã chi: " << tien(da_chi) << " | hạng: " << hang.ten
         << " (giảm " << hang.phan_tram << "%)\n";
    long long thieu = con_thieu_de_len_hang(da_chi);
    if (thieu > 0) cout << "Còn " << tien(thieu) << " nữa để lên hạng kế tiếp.\n";

    GioHang gio;
    cout << "Thêm món vào giỏ. Để trống MÃ để kết thúc.\n";
    while (true) {
        string ma = doc_dong("  Mã sản phẩm: ");
        if (ma.empty()) break;
        int vt = shop.find(ma);
        if (vt == -1) { cout << "  Không có mã này.\n"; continue; }
        in_san_pham(shop.take(vt));
        string mau = doc_dong("  Màu: "), size = doc_dong("  Size: ");
        long long sl = doc_so("  Số lượng: ", 0);
        if (!gio.them(ma, mau, size, (int)sl)) cout << "  Số lượng phải > 0.\n";
    }
    if (gio.empty()) { cout << "Giỏ hàng trống.\n"; return; }

    int d, m, y;
    if (p.loai == 1) {
        ngay_hom_nay(d, m, y);      // khách tự mua: luôn là hôm nay, không cho chọn ngày
    } else if (!doc_ngay(doc_dong("Ngày bán dd/mm/yyyy (Enter = hôm nay): "), d, m, y)) {
        cout << "Sai định dạng ngày.\n"; return;
    }

    HoaDon hd; int dong_loi = -1;
    KetQuaThanhToan kq = shop.thanh_toan(gio, ma_kh, ma_nv, d, m, y, hd, &dong_loi);
    cout << "\nKết quả: " << ten_kq_tt(kq);
    if (dong_loi >= 0) cout << "  (lỗi ở dòng số " << dong_loi + 1 << " của giỏ)";
    cout << "\n";
    if (kq == TT_OK) cout << in_hoa_don(hd);
}

static void xem_hoa_don(Shop &shop, QuanLyKhachHang &qk, const Phien &p) {
    if (!can_dang_nhap(p)) return;
    string ma_kh = p.ma;
    if (p.loai != 1) {
        int i = qk.tim_theo_sdt(doc_dong("SĐT khách: "));
        if (i == -1) { cout << "Không có khách này.\n"; return; }
        ma_kh = qk.lay(i).getMaKH();
    }
    VECTOR<HoaDon> ds = ds_hoa_don_cua_khach(shop.lay_doanh_thu(), ma_kh);
    cout << "\n--- " << ds.size() << " hóa đơn của " << ma_kh << " (mới nhất trước) ---\n";
    for (int i = 0; i < ds.size(); i++) cout << in_hoa_don(ds[i]) << "\n";
}

// =========================================================================
// THỐNG KÊ (admin)
// =========================================================================
static bool chon_kieu_ky(KieuKy &k) {
    cout << "  1. Tuần  2. Tháng  3. Quý  4. Năm\n";
    long long c = doc_so("Chọn loại kỳ: ");
    if (c == 1) k = KY_TUAN; else if (c == 2) k = KY_THANG;
    else if (c == 3) k = KY_QUY; else if (c == 4) k = KY_NAM;
    else return false;
    return true;
}

static bool nhap_khoa_ky(KieuKy k, long long &khoa) {
    string s = doc_dong("Enter = kỳ hiện tại, hoặc gõ 'c' để tự chọn kỳ: ");
    if (s.empty()) { khoa = khoa_ky_hien_tai(k); return true; }
    if (k == KY_NAM) {
        long long nam = doc_so("Năm: ", -1);
        if (nam < 2000) return false;
        khoa = nam; return true;
    }
    if (k == KY_QUY) {
        long long q = doc_so("Quý (1-4): ", -1), nam = doc_so("Năm: ", -1);
        if (q < 1 || q > 4 || nam < 2000) return false;
        khoa = nam * 10 + q; return true;
    }
    if (k == KY_THANG) {
        long long t = doc_so("Tháng (1-12): ", -1), nam = doc_so("Năm: ", -1);
        if (t < 1 || t > 12 || nam < 2000) return false;
        khoa = nam * 100 + t; return true;
    }
    int d, m, y; long long tg;
    if (!doc_ngay(doc_dong("Một ngày bất kỳ trong tuần (dd/mm/yyyy): "), d, m, y)) return false;
    if (!tao_thoi_gian(d, m, y, tg)) { cout << "  Ngày không hợp lệ.\n"; return false; }
    khoa = lay_khoa_ky(tg, KY_TUAN);
    return true;
}

static void thong_ke(Shop &shop) {
    const DoanhThu &dt = shop.lay_doanh_thu();
    cout << "\nSổ bán hàng hiện có " << dt.so_giao_dich() << " dòng hàng (doanh thu tính SAU giảm giá).\n";
    cout << "  1. Tổng doanh thu 1 kỳ\n  2. Top kỳ doanh thu cao nhất\n  3. Top sản phẩm trong 1 kỳ\n"
            "  4. Top nhân viên trong 1 kỳ\n  5. Doanh số của 1 nhân viên (mọi thời gian)\n";
    long long c = doc_so("Chọn: ");
    KieuKy k; long long khoa;
    if (c == 5) {
        string ma = doc_dong("Mã NV: ");
        cout << ma << ": " << tien(dt.doanh_so_nhan_vien(ma)) << "\n";
        return;
    }
    if (c < 1 || c > 4 || !chon_kieu_ky(k)) { cout << "Huỷ.\n"; return; }

    if (c == 1) {
        if (!nhap_khoa_ky(k, khoa)) { cout << "Kỳ không hợp lệ.\n"; return; }
        cout << ten_ky(khoa, k) << ": " << tien(dt.tong_doanh_thu(k, khoa)) << "\n";
    } else if (c == 2) {
        long long n = doc_so("Lấy top mấy (Enter = 5): ", 5);
        VECTOR<DoanhThuKy> t = dt.top_ky(k, (int)n);
        for (int i = 0; i < t.size(); i++)
            cout << "  #" << i + 1 << "  " << ten_ky(t[i].khoa, k) << "  " << tien(t[i].tong) << "\n";
        if (t.empty()) cout << "  (chưa có dữ liệu)\n";
    } else if (c == 3) {
        if (!nhap_khoa_ky(k, khoa)) { cout << "Kỳ không hợp lệ.\n"; return; }
        long long n = doc_so("Lấy top mấy (Enter = 5): ", 5);
        VECTOR<DoanhThuSP> t = dt.top_san_pham(k, khoa, (int)n);
        cout << "Top sản phẩm trong " << ten_ky(khoa, k) << ":\n";
        for (int i = 0; i < t.size(); i++)
            cout << "  #" << i + 1 << "  " << t[i].idsp << " | " << t[i].ten << " | "
                 << t[i].so_luong << " cái | " << tien(t[i].tong) << "\n";
        if (t.empty()) cout << "  (chưa có dữ liệu)\n";
    } else {
        if (!nhap_khoa_ky(k, khoa)) { cout << "Kỳ không hợp lệ.\n"; return; }
        long long n = doc_so("Lấy top mấy (Enter = 5): ", 5);
        VECTOR<DoanhThuNV> t = dt.top_nhan_vien(k, khoa, (int)n);
        cout << "Top nhân viên trong " << ten_ky(khoa, k) << ":\n";
        for (int i = 0; i < t.size(); i++)
            cout << "  #" << i + 1 << "  " << t[i].ma_nv << "  " << tien(t[i].tong) << "\n";
        if (t.empty()) cout << "  (chưa có dữ liệu)\n";
    }
}

// =========================================================================
// KIỂM TRA TỰ ĐỘNG
// =========================================================================
static int so_dat = 0, so_loi = 0;
static void kiem(bool ok, const string &mo_ta) {
    cout << (ok ? "  [OK]   " : "  [LOI]  ") << mo_ta << "\n";
    if (ok) so_dat++; else so_loi++;
}

static void chep_de(const string &nguon, const string &dich) {
    error_code ec;
    fs::copy_file(nguon, dich, fs::copy_options::overwrite_existing, ec);
}

static bool file_chua(const char *file, const string &chuoi) {
    ifstream f(file);
    string line;
    while (getline(f, line)) if (line.find(chuoi) != string::npos) return true;
    return false;
}

// File dạng bảng: dòng đầu là tiêu đề chứa `tieu_de`, mọi dòng bắt đầu bằng '|' và dài bằng nhau (tính theo ký tự)
static bool bang_thang_hang(const char *file, const string &tieu_de) {
    ifstream f(file);
    string dong;
    int dai = -1, so_dong = 0;
    while (getline(f, dong)) {
        if (!dong.empty() && dong.back() == '\r') dong.pop_back();
        if (dong.empty()) continue;
        if (so_dong == 0 && dong.find(tieu_de) == string::npos) return false;
        if (dong[0] != '|') return false;
        int d = do_dai_utf8(dong);
        if (dai == -1) dai = d; else if (d != dai) return false;
        so_dong++;
    }
    return so_dong >= 2;
}

static void tao_anh_mau(const char *ten, Color c) {
    Image im = GenImageColor(64, 64, c);
    ExportImage(im, ten);
    UnloadImage(im);
}

static void kich_ban(Shop &shop, const string &ma_du_kien) {
    VECTOR<string> hai_anh; hai_anh.push_back("_test_anh_1.png"); hai_anh.push_back("_test_anh_2.png");
    VECTOR<string> khong_anh;
    VECTOR<string> anh_ma; anh_ma.push_back("_khong_ton_tai.png");
    VECTOR<Variant> bt;
    bt.push_back(Variant("Đen", "M", 90000));
    bt.push_back(Variant("Trắng", "L", 50));
    VECTOR<Variant> bt_rong;
    const long long GIA = 199000;

    int so_sp_ban_dau = shop.size();

    cout << "\n[A] Thêm sản phẩm + ảnh\n";
    kiem(shop.them_quan_ao("Coolmate", "Áo", "Ao test", "Áo thun", GIA, "Cotton", bt_rong, hai_anh) == THEM_THIEU_THONG_TIN, "không có biến thể -> THIEU_THONG_TIN");
    kiem(shop.them_quan_ao("Coolmate", "Mũ", "Mu test", "Mũ lưỡi trai", 99000, "Cotton", bt, hai_anh) == THEM_SAI_KIEU, "kiểu Mũ (phụ kiện) -> SAI_KIEU");
    kiem(shop.them_quan_ao("Coolmate", "Áo", "Ao|loi", "Áo thun", GIA, "Cotton", bt, hai_anh) == THEM_KY_TU_CAM, "tên có ký tự | -> KY_TU_CAM");
    kiem(shop.them_quan_ao("Coolmate", "Áo", "Ao test", "Áo thun", GIA, "Cotton", bt, khong_anh) == THEM_LOI_ANH, "không chọn ảnh -> LOI_ANH");
    kiem(shop.them_quan_ao("Coolmate", "Áo", "Ao test", "Áo thun", GIA, "Cotton", bt, anh_ma) == THEM_LOI_ANH, "ảnh không tồn tại -> LOI_ANH");
    kiem(shop.size() == so_sp_ban_dau && dem_anh(ma_du_kien) == 0, "các ca lỗi không để lại sản phẩm hay ảnh rác");

    string ma;
    kiem(shop.them_quan_ao("Coolmate", "Áo", "Ao test", "Áo thun", GIA, "Cotton", bt, hai_anh, &ma) == THEM_OK, "thêm hợp lệ -> THEM_OK");
    kiem(ma == ma_du_kien && !ma.empty(), "mã sinh ra đúng bằng mã dự kiến: " + ma);
    kiem(dem_anh(ma) == 2 && fs::exists(duong_dan_anh(ma, 1)) && !fs::exists(duong_dan_anh(ma, 3)), "có đúng 2 ảnh " + duong_dan_anh(ma, 1) + ", _2");
    kiem(file_chua(F_ITEM, ma), "item.txt đã chứa mã mới");

    cout << "\n[B] Đọc lại từ file + tìm kiếm\n";
    Shop s2;
    s2.doc_file(F_ITEM);
    int vt = s2.find(ma);
    kiem(vt != -1, "đọc lại file: tìm thấy sản phẩm");
    if (vt == -1) { cout << "  Dừng vì không thấy sản phẩm.\n"; return; }
    kiem(s2.take(vt)->so_variant() == 2 && s2.take(vt)->getsl("den", "m") == 90000 && s2.take(vt)->getsl("TRANG", "l") == 50,
         "đủ 2 biến thể, tồn kho đúng (tra màu/size không dấu, không phân biệt hoa thường)");
    {
        VECTOR<KetQuaTimKiem> r = s2.tim_kiem("ao test");
        bool thay = false;
        for (int i = 0; i < r.size(); i++) if (r[i].sp->getid() == ma && r[i].ti_le_khop == 100.0) thay = true;
        kiem(thay, "tìm 'ao test' ra sản phẩm với 100%");
        kiem(s2.tim_kiem("zzzkhongco").size() == 0, "từ khóa không có -> 0 kết quả");
    }

    // ------------------------------------------------------------------
    cout << "\n[C] Tài khoản khách\n";
    QuanLyKhachHang qk(F_KH);
    qk.doc_file();
    string ma_kh1, ma_kh2;
    kiem(qk.dang_ky("", "0912345678", "matkhau123", "", "") == DK_THIEU_THONG_TIN, "thiếu tên -> THIEU_THONG_TIN");
    kiem(qk.dang_ky("An|B", "0912345678", "matkhau123", "", "") == DK_KY_TU_CAM, "tên có ký tự | -> KY_TU_CAM");
    kiem(qk.dang_ky("An", "12345", "matkhau123", "", "") == DK_SDT_SAI, "SĐT 5 số -> SDT_SAI");
    kiem(qk.dang_ky("An", "1912345678", "matkhau123", "", "") == DK_SDT_SAI, "SĐT không bắt đầu bằng 0 -> SDT_SAI");
    kiem(qk.dang_ky("An", "0912345678", "123", "", "") == DK_MAT_KHAU_NGAN, "mật khẩu ngắn -> MAT_KHAU_NGAN");
    kiem(qk.dang_ky(string(RONG_KH_TEN + 1, 'a'), "0912345678", "matkhau123", "", "") == DK_QUA_DAI, "tên khách quá dài -> QUA_DAI");
    kiem(qk.size() == 0, "các ca lỗi không tạo tài khoản");
    kiem(qk.dang_ky("Nguyen An", "0912345678", "matkhau123", "an@x.com", "Ha Noi", &ma_kh1) == DK_OK && ma_kh1 == "KH0001", "đăng ký đầu tiên -> KH0001");
    kiem(qk.dang_ky("Khac", "0912345678", "matkhau999", "", "") == DK_SDT_TRUNG, "trùng SĐT -> SDT_TRUNG");
    kiem(qk.dang_ky("Le Binh", "0987654321", "binh-pass", "", "", &ma_kh2) == DK_OK && ma_kh2 == "KH0002", "khách thứ hai -> KH0002");
    kiem(qk.dang_nhap("0912345678", "matkhau123") == 0, "đăng nhập đúng");
    kiem(qk.dang_nhap("0912345678", "sai-mat-khau") == -1, "sai mật khẩu -> không vào được");
    kiem(qk.dang_nhap("0999999999", "matkhau123") == -1, "SĐT chưa đăng ký -> không vào được");
    kiem(file_chua(F_KH, "0912345678") && !file_chua(F_KH, "matkhau123") && !file_chua(F_KH, "binh-pass"), "file khachhang.txt không chứa mật khẩu nguyên văn (đã mã hóa)");
    kiem(bang_thang_hang(F_KH, "maKH"), "khachhang.txt dạng bảng: có tiêu đề, mọi dòng thẳng hàng");
    kiem(!qk.doi_mat_khau(ma_kh1, "sai-cu", "matkhau-moi"), "đổi mật khẩu với mật khẩu cũ sai -> từ chối");
    kiem(!qk.doi_mat_khau(ma_kh1, "matkhau123", "ab"), "mật khẩu mới quá ngắn -> từ chối");
    kiem(qk.doi_mat_khau(ma_kh1, "matkhau123", "matkhau-moi") && qk.dang_nhap("0912345678", "matkhau-moi") != -1
         && qk.dang_nhap("0912345678", "matkhau123") == -1, "đổi mật khẩu đúng: mật khẩu mới vào được, cũ hết hiệu lực");
    kiem(qk.dat_lai_mat_khau(ma_kh1) && qk.dang_nhap("0912345678", "0912345678") != -1, "admin cấp lại: mật khẩu về = số điện thoại");
    kiem(qk.dat_trang_thai(ma_kh2, false) && qk.dang_nhap("0987654321", "binh-pass") == -1, "khóa tài khoản -> không đăng nhập được");
    kiem(qk.dat_trang_thai(ma_kh2, true) && qk.dang_nhap("0987654321", "binh-pass") != -1, "mở khóa -> đăng nhập lại được");
    {
        ofstream o(F_KH, ios::app);
        o << "abc\nKH9|x|123|a|b|c|1|1\nKH8|Ok|0933333333|a|b|xxxx|khong-phai-so|1\n";   // 3 dòng hỏng
    }
    QuanLyKhachHang qk2(F_KH);
    qk2.doc_file();
    kiem(qk2.size() == 2, "đọc file có 3 dòng hỏng: bỏ qua dòng hỏng, không sập, còn đúng 2 khách");
    kiem(qk2.dang_nhap("0912345678", "0912345678") != -1, "mở lại app: tài khoản và mật khẩu vẫn còn");

    // ------------------------------------------------------------------
    cout << "\n[D] Tài khoản nhân viên / admin\n";
    QuanLyNhanVien qn(F_NV);
    qn.doc_file();
    kiem(qn.khoi_tao_admin_neu_trong() && qn.size() == 1, "lần đầu chưa có ai: tự tạo admin NV0001");
    kiem(!qn.khoi_tao_admin_neu_trong(), "gọi lần nữa: không tạo thêm");
    kiem(qn.dang_nhap("NV0001", "admin123") == 0 && qn.lay(0).laAdmin(), "admin đăng nhập bằng mật khẩu mặc định");
    kiem(qn.dang_nhap("nv0001", "admin123") == 0, "mã NV gõ chữ thường vẫn nhận");
    string ma_nv2;
    kiem(qn.tao_nhan_vien("Tran Binh", "0911111111", "quanly") == NV_VAI_TRO_SAI, "vai trò lạ -> VAI_TRO_SAI (chỉ admin / nhanvien)");
    kiem(qn.tao_nhan_vien("Tran Binh", "091", "nhanvien") == NV_SDT_SAI, "SĐT sai -> SDT_SAI");
    kiem(qn.tao_nhan_vien("Tran Binh", "0911111111", "nhanvien", "", "", "", &ma_nv2) == NV_OK && ma_nv2 == "NV0002", "tạo nhân viên -> NV0002");
    kiem(qn.tao_nhan_vien(string(RONG_NV_TEN + 1, 'a'), "0922222223", "nhanvien") == NV_QUA_DAI, "tên nhân viên quá dài -> QUA_DAI");
    kiem(bang_thang_hang(F_NV, "maNV"), "nhanvien.txt dạng bảng: có tiêu đề, mọi dòng thẳng hàng");
    kiem(qn.tao_nhan_vien("Khac", "0911111111", "nhanvien") == NV_SDT_TRUNG, "trùng SĐT -> SDT_TRUNG");
    kiem(qn.dang_nhap("NV0002", "0911111111") == 1 && !qn.lay(1).laAdmin(), "nhân viên: mật khẩu mặc định = SĐT, KHÔNG phải admin");
    kiem(!file_chua(F_NV, "admin123"), "file nhanvien.txt không chứa mật khẩu nguyên văn");
    kiem(qn.doi_mat_khau("NV0002", "0911111111", "mat-khau-moi") && qn.dang_nhap("NV0002", "mat-khau-moi") == 1, "nhân viên tự đổi mật khẩu");
    kiem(qn.dat_lai_mat_khau("NV0002") && qn.dang_nhap("NV0002", "0911111111") == 1, "quên mật khẩu: admin cấp lại -> về SĐT");
    kiem(!qn.dat_trang_thai("NV0001", false), "KHÔNG cho vô hiệu hóa admin cuối cùng");
    kiem(!qn.doi_vai_tro("NV0001", "nhanvien"), "KHÔNG cho hạ quyền admin cuối cùng");
    kiem(qn.dat_trang_thai("NV0002", false) && qn.dang_nhap("NV0002", "0911111111") == -1, "cho nghỉ -> không đăng nhập được");
    kiem(qn.dat_trang_thai("NV0002", true) && qn.dang_nhap("NV0002", "0911111111") == 1, "đi làm lại -> đăng nhập được");
    {
        ofstream o(F_NV, ios::app);
        o << "NV0009|Cu|0922222222|a@x|HN|oldpass|nhanvien|1767225600|1\n";   // mật khẩu viết thô của file cũ
    }
    QuanLyNhanVien qn2(F_NV);
    qn2.doc_file();
    kiem(qn2.size() == 3 && qn2.dang_nhap("NV0009", "oldpass") != -1, "mật khẩu thô của file cũ vẫn đăng nhập được (rồi đổi để mã hóa)");
    kiem(qn2.doi_mat_khau("NV0009", "oldpass", "pass-moi-1") && !file_chua(F_NV, "oldpass"), "đổi xong thì mật khẩu thô biến mất khỏi file");
    const string NV_BAN = "NV0002", NV_ADMIN = "NV0001";

    // ------------------------------------------------------------------
    cout << "\n[E] Giỏ hàng + thanh toán\n";
    const DoanhThu &dt = s2.lay_doanh_thu();
    GioHang g;
    kiem(!g.them(ma, "Đen", "M", 0) && g.empty(), "thêm với số lượng 0 -> từ chối");
    kiem(g.them(ma, "Đen", "M", 1) && g.them(ma, "Đen", "M", 1) && g.so_dong() == 1 && g.lay(0).sl == 2, "thêm cùng món 2 lần -> cộng dồn thành 1 dòng");
    g.lam_rong();

    HoaDon hd; int dl = -2;
    kiem(s2.thanh_toan(g, ma_kh1, NV_BAN, 15, 1, 2025, hd, &dl) == TT_GIO_TRONG, "giỏ trống -> GIO_TRONG");
    g.them(ma, "Đen", "M", 2); g.them(ma, "Trắng", "L", 1);
    kiem(s2.thanh_toan(g, "", NV_BAN, 15, 1, 2025, hd, &dl) == TT_THIEU_KHACH, "chưa có mã khách -> THIEU_KHACH (khách phải có tài khoản)");
    kiem(s2.thanh_toan(g, ma_kh1, NV_BAN, 31, 2, 2025, hd, &dl) == TT_NGAY_SAI, "ngày 31/2 -> NGAY_SAI");
    kiem(s2.thanh_toan(g, ma_kh1, NV_BAN, 1, 1, 2099, hd, &dl) == TT_NGAY_SAI, "ngày tương lai -> NGAY_SAI");

    GioHang g2; g2.them(ma, "Đen", "M", 1); g2.them("SPXX9999A", "Đen", "M", 1);
    kiem(s2.thanh_toan(g2, ma_kh1, NV_BAN, 15, 1, 2025, hd, &dl) == TT_KHONG_CO_SP && dl == 1, "mã không tồn tại ở dòng 2 -> KHONG_CO_SP, báo đúng dòng");
    GioHang g3; g3.them(ma, "Đen", "M", 1); g3.them(ma, "Xanh", "M", 1);
    kiem(s2.thanh_toan(g3, ma_kh1, NV_BAN, 15, 1, 2025, hd, &dl) == TT_KHONG_CO_BIEN_THE && dl == 1, "màu không tồn tại ở dòng 2 -> KHONG_CO_BIEN_THE");
    GioHang g4; g4.them(ma, "Đen", "M", 1); g4.them(ma, "Trắng", "L", 9999);
    kiem(s2.thanh_toan(g4, ma_kh1, NV_BAN, 15, 1, 2025, hd, &dl) == TT_THIEU_HANG && dl == 1, "dòng 2 thiếu hàng -> THIEU_HANG");
    GioHang g5; g5.them(ma, "Trắng", "L", 30); g5.them(ma, "trang", "l", 30);
    kiem(s2.thanh_toan(g5, ma_kh1, NV_BAN, 15, 1, 2025, hd, &dl) == TT_THIEU_HANG, "2 dòng cùng 1 ô gõ khác nhau (30 + 30 > 50) -> vẫn bắt được thiếu hàng");
    kiem(s2.take(vt)->getsl("Đen", "M") == 90000 && s2.take(vt)->getsl("Trắng", "L") == 50 && dt.so_giao_dich() == 0,
         "sau 8 ca lỗi: kho và sổ KHÔNG đổi (được tất cả hoặc không gì: dòng 1 hợp lệ cũng không bị trừ)");

    // --- Đơn 1: khách 1 mới, hạng thấp nhất, NV0002 bán ngày 15/1/2025
    const HangTV &h1 = lay_hang(0);
    long long giam1 = tinh_giam_gia_dong(h1, GIA, 2) + tinh_giam_gia_dong(h1, GIA, 1);
    kiem(s2.thanh_toan(g, ma_kh1, NV_BAN, 15, 1, 2025, hd, &dl) == TT_OK, "đơn 1 thành công");
    HoaDon hd1 = hd;
    kiem(hd1.ma_hd == "HD000001" && hd1.dong.size() == 2, "mã hóa đơn HD000001, 2 dòng");
    kiem(hd1.tam_tinh() == 3 * GIA && hd1.giam_gia() == giam1 && hd1.thanh_toan() == 3 * GIA - giam1, "tạm tính / giảm giá / thanh toán đúng theo bảng hạng");
    kiem(s2.take(vt)->getsl("Đen", "M") == 89998 && s2.take(vt)->getsl("Trắng", "L") == 49, "kho trừ đúng: Đen-M 89998, Trắng-L 49");
    kiem(dt.tong_doanh_thu(KY_THANG, 202501) == hd1.thanh_toan(), "doanh thu tháng 1/2025 = số tiền thực thu (SAU giảm giá)");
    kiem(dt.tong_chi_tieu_khach(ma_kh1) == hd1.thanh_toan(), "tổng chi tiêu của khách tính từ sổ");
    kiem(dt.doanh_so_nhan_vien(NV_BAN) == hd1.thanh_toan(), "doanh số nhân viên tính từ sổ");

    // --- Đơn 2: mua đủ nhiều để lên hạng
    long long da_chi = dt.tong_chi_tieu_khach(ma_kh1);
    long long muc = (SO_HANG_TV > 1) ? BANG_HANG[1].nguong : 0;
    long long can = muc - da_chi;
    if (can < GIA) can = GIA;
    int q = (int)(can / GIA + 1);
    const HangTV &h_truoc = lay_hang(da_chi);
    GioHang gb; gb.them(ma, "Đen", "M", q);
    kiem(s2.thanh_toan(gb, ma_kh1, NV_BAN, 20, 2, 2025, hd, &dl) == TT_OK, "đơn 2 (mua nhiều để lên hạng) thành công");
    HoaDon hd2 = hd;
    kiem(hd2.giam_gia() == tinh_giam_gia_dong(h_truoc, GIA, q), "đơn 2 giảm theo hạng TRƯỚC đơn (hạng cũ), không phải hạng mới");
    long long tong_sau = dt.tong_chi_tieu_khach(ma_kh1);
    kiem(chi_so_hang(tong_sau) >= (SO_HANG_TV > 1 ? 1 : 0) && tong_sau == hd1.thanh_toan() + hd2.thanh_toan(), "sau đơn 2: tổng chi tiêu đúng, hạng đã lên");

    // --- Đơn 3: admin bán, khách đã lên hạng -> áp mức giảm của hạng mới
    const HangTV &h3 = lay_hang(tong_sau);
    GioHang gc; gc.them(ma, "Trắng", "L", 1);
    kiem(s2.thanh_toan(gc, ma_kh1, NV_ADMIN, 5, 3, 2025, hd, &dl) == TT_OK, "đơn 3 (admin bán) thành công");
    HoaDon hd3 = hd;
    kiem(hd3.giam_gia() == tinh_giam_gia_dong(h3, GIA, 1), "đơn 3 giảm theo hạng mới");

    // --- Đơn 4: khách 2 tự mua (không có nhân viên)
    GioHang gd; gd.them(ma, "Đen", "M", 1);
    kiem(s2.thanh_toan(gd, ma_kh2, "", 10, 3, 2025, hd, &dl) == TT_OK && hd.ma_nv.empty() && hd.ma_hd == "HD000004", "đơn 4: khách tự mua, không có nhân viên");
    HoaDon hd4 = hd;

    cout << "\n[F] Thống kê / hóa đơn (tất cả theo tiền SAU giảm giá)\n";
    long long tong_nam = hd1.thanh_toan() + hd2.thanh_toan() + hd3.thanh_toan() + hd4.thanh_toan();
    kiem(dt.tong_doanh_thu(KY_NAM, 2025) == tong_nam, "doanh thu năm 2025 = tổng 4 hóa đơn sau giảm giá");
    kiem(dt.tong_doanh_thu(KY_QUY, 20251) == tong_nam, "doanh thu quý 1/2025 đúng");
    kiem(dt.tong_doanh_thu(KY_TUAN, 20250113) == hd1.thanh_toan(), "tuần từ T2 13/1/2025 = đơn 1");
    VECTOR<DoanhThuSP> tsp = dt.top_san_pham(KY_NAM, 2025, 5);
    kiem(tsp.size() == 1 && tsp[0].idsp == ma && tsp[0].tong == tong_nam && tsp[0].so_luong == 3 + q + 1 + 1, "top sản phẩm: 1 sản phẩm, đúng doanh thu và số lượng");
    long long nv2 = hd1.thanh_toan() + hd2.thanh_toan(), nv1 = hd3.thanh_toan();
    VECTOR<DoanhThuNV> tnv = dt.top_nhan_vien(KY_NAM, 2025, 5);
    kiem(tnv.size() == 2, "top nhân viên: đơn khách tự mua không tính cho ai (chỉ 2 nhân viên)");
    if (tnv.size() == 2) {
        string dau = (nv2 > nv1) ? NV_BAN : NV_ADMIN;
        kiem(tnv[0].ma_nv == dau && tnv[0].tong == (nv2 > nv1 ? nv2 : nv1), "top 1 nhân viên đúng (heap)");
    }
    kiem(dt.doanh_so_nhan_vien(NV_BAN, KY_THANG, 202501) == hd1.thanh_toan(), "doanh số NV0002 riêng tháng 1/2025 = đơn 1");
    kiem(dt.doanh_so_nhan_vien(NV_ADMIN) == hd3.thanh_toan(), "doanh số admin = đơn 3");
    HoaDon doc = lay_hoa_don(dt, hd2.ma_hd);
    kiem(doc.ma_hd == hd2.ma_hd && doc.dong.size() == 1 && doc.thanh_toan() == hd2.thanh_toan() && doc.ma_nv == NV_BAN, "tra lại hóa đơn cũ từ sổ: khớp từng số");
    kiem(in_hoa_don(doc).find(hd2.ma_hd) != string::npos, "in hóa đơn có mã hóa đơn");
    VECTOR<HoaDon> ds_hd = ds_hoa_don_cua_khach(dt, ma_kh1);
    kiem(ds_hd.size() == 3 && ds_hd[0].ma_hd == hd3.ma_hd && ds_hd[2].ma_hd == hd1.ma_hd, "hóa đơn của khách 1: 3 đơn, mới nhất trước");
    kiem(lay_hoa_don(dt, "HD999999").ma_hd.empty(), "mã hóa đơn không có -> rỗng");
    kiem(dt.so_hoa_don() == 4 && dt.dong_cua_hoa_don(hd2.ma_hd) != nullptr && dt.dong_cua_hoa_don(hd2.ma_hd)->size() == 1
         && dt.dong_cua_hoa_don("HD999999") == nullptr, "BẢNG BĂM hóa đơn: 4 hóa đơn, tra theo mã có / không có đúng");
    kiem(dt.hoa_don_cua_khach(ma_kh1) != nullptr && dt.hoa_don_cua_khach(ma_kh1)->size() == 3
         && dt.hoa_don_cua_khach(ma_kh2) != nullptr && dt.hoa_don_cua_khach(ma_kh2)->size() == 1
         && dt.hoa_don_cua_khach("KH9999") == nullptr, "BẢNG BĂM khách -> hóa đơn: khách 1 có 3, khách 2 có 1, khách lạ không có");
    kiem(bang_thang_hang(F_DT, "thoi_gian"), "doanhthu.txt dạng bảng: có tiêu đề, mọi dòng thẳng hàng");

    cout << "\n[G] Lỗi ghi file: phải hoàn kho, không để dở dang\n";
    int sl_truoc = s2.take(vt)->getsl("Đen", "M"), gd_truoc = dt.so_giao_dich();
    GioHang gl; gl.them(ma, "Đen", "M", 1);
    error_code ec;
    fs::create_directory("Data/item.txt.tmp", ec);    // làm cho việc ghi item.txt bị thất bại
    kiem(s2.thanh_toan(gl, ma_kh1, NV_BAN, 5, 3, 2025, hd, &dl) == TT_LOI_GHI_FILE, "không ghi được item.txt -> LOI_GHI_FILE");
    fs::remove("Data/item.txt.tmp", ec);
    kiem(s2.take(vt)->getsl("Đen", "M") == sl_truoc && dt.so_giao_dich() == gd_truoc, "đã hoàn kho, sổ không thêm dòng nào");

    fs::rename(F_DT, string(F_DT) + ".mv", ec);
    fs::create_directory(F_DT, ec);                       // làm cho việc ghi sổ doanh thu bị thất bại
    kiem(s2.thanh_toan(gl, ma_kh1, NV_BAN, 5, 3, 2025, hd, &dl) == TT_LOI_GHI_FILE, "không ghi được sổ doanh thu -> LOI_GHI_FILE");
    kiem(s2.take(vt)->getsl("Đen", "M") == sl_truoc && dt.so_giao_dich() == gd_truoc, "đã hoàn kho trong RAM, sổ không thêm dòng nào");
    kiem(dt.so_hoa_don() == 4 && dt.dong_cua_hoa_don("HD000005") == nullptr && dt.ma_hd_tiep_theo() == "HD000005",
         "đơn lỗi không để lại gì trong bảng băm; mã hóa đơn kế tiếp vẫn là HD000005");
    {
        Shop s5;
        s5.doc_file(F_ITEM);
        int v5 = s5.find(ma);
        kiem(v5 != -1 && s5.take(v5)->getsl("Đen", "M") == sl_truoc, "item.txt trên đĩa cũng đã hoàn lại đúng số lượng (không lệch RAM)");
    }
    fs::remove(F_DT, ec);
    fs::rename(string(F_DT) + ".mv", F_DT, ec);

    cout << "\n[H] Mở lại app: sổ doanh thu và kho còn nguyên, đọc được cả dòng cũ\n";
    {
        Shop s6;
        s6.doc_file(F_ITEM);
        int v6 = s6.find(ma);
        kiem(s6.lay_doanh_thu().so_giao_dich() == 5 && v6 != -1, "mở lại: sổ có 5 dòng hàng (4 hóa đơn)");
        kiem(s6.lay_doanh_thu().tong_doanh_thu(KY_NAM, 2025) == tong_nam, "doanh thu năm 2025 sau khi mở lại vẫn đúng");
        kiem(s6.lay_doanh_thu().tong_chi_tieu_khach(ma_kh1) == tong_sau + hd3.thanh_toan(), "tổng chi tiêu khách 1 sau khi mở lại vẫn đúng");
        kiem(s6.lay_doanh_thu().ma_hd_tiep_theo() == "HD000005", "mã hóa đơn kế tiếp = HD000005");
        HoaDon lai = lay_hoa_don(s6.lay_doanh_thu(), hd2.ma_hd);
        kiem(s6.lay_doanh_thu().so_hoa_don() == 4 && lai.thanh_toan() == hd2.thanh_toan() && lai.dong.size() == 1,
             "mở lại: bảng băm hóa đơn được dựng lại từ sổ, tra mã vẫn khớp");
        kiem(ds_hoa_don_cua_khach(s6.lay_doanh_thu(), ma_kh1).size() == 3, "mở lại: hóa đơn của khách 1 vẫn đủ 3");
        kiem(s6.take(v6)->getsl("Đen", "M") == sl_truoc, "kho còn nguyên");
        {
            ofstream o(F_DT, ios::app);
            o << "1735689600|SPOLD0001A|Hang cu|Đen|M|1|100000\n";     // dòng kiểu cũ, chỉ 7 cột
        }
        Shop s7;
        s7.doc_file(F_ITEM);
        const DoanhThu &d7 = s7.lay_doanh_thu();
        kiem(d7.so_giao_dich() == 6, "dòng cũ 7 cột vẫn đọc được");
        if (d7.so_giao_dich() == 6) {
            const GiaoDich &cu = d7.lay(5);
            kiem(cu.ma_hd.empty() && cu.ma_kh.empty() && cu.giam_gia == 0 && cu.thanh_tien() == 100000, "dòng cũ: không giảm giá, thành tiền = đơn giá x số lượng");
            kiem(d7.so_hoa_don() == 4, "dòng cũ không có mã hóa đơn nên KHÔNG vào bảng băm (vẫn 4 hóa đơn)");
        }
    }

    cout << "\n[I] Xóa sản phẩm: sổ doanh thu cũ phải còn\n";
    {
        Shop s8;
        s8.doc_file(F_ITEM);
        kiem(s8.xoa_san_pham(ma), "xoa_san_pham trả true (đã tự ghi file)");
        kiem(dem_anh(ma) == 0 && !fs::exists(duong_dan_anh(ma, 1)), "toàn bộ ảnh đã bị xóa theo");
        kiem(s8.find(ma) == -1, "không còn trong danh sách");
        Shop s9;
        s9.doc_file(F_ITEM);
        kiem(s9.find(ma) == -1 && !file_chua(F_ITEM, ma), "mở lại file: sản phẩm đã mất hẳn");
        kiem(s9.lay_doanh_thu().so_giao_dich() == 6, "doanh thu cũ vẫn giữ nguyên (đúng, vì đã bán rồi)");
    }
}


static void kich_ban_cau_truc() {
    cout << "\n[J] Cấu trúc dữ liệu tự cài: LIST (danh sách liên kết đôi), HASHTABLE (bảng băm), HEAP\n";
    LIST<int> l;
    for (int i = 1; i <= 5; i++) l.push_back(i);
    l.push_front(0);
    kiem(l.size() == 6 && l[0] == 0 && l[5] == 5 && l[3] == 3, "LIST: push_back / push_front / truy cập theo chỉ số");
    l.remove(0); l.remove(4); l.remove(1);                     // còn 1 3 4
    kiem(l.size() == 3 && l[0] == 1 && l[1] == 3 && l[2] == 4, "LIST: xóa đầu / cuối / giữa");
    int tong = 0; for (int x : l) tong += x;
    kiem(tong == 8, "LIST: duyệt bằng range-for");
    LIST<int> c(l); c[0] = 100;
    kiem(l[0] == 1 && c[0] == 100, "LIST: sao chép sâu (sửa bản sao không ảnh hưởng bản gốc)");
    l.pop_back(); l.pop_front(); l.pop_back(); l.pop_back();
    kiem(l.empty(), "LIST: pop hết, pop khi rỗng vẫn an toàn");
    LIST<int> m; for (int i = 0; i < 2000; i++) m.push_back(i);
    for (int i = 1999; i >= 0; i -= 2) m.remove(i);
    long long s = 0; for (int i = 0; i < m.size(); i++) s += m[i];
    kiem(m.size() == 1000 && s == 999000, "LIST: 2000 phần tử, xóa một nửa, duyệt theo chỉ số");

    DSLK_DON<string> dl1;
    dl1.push_front("HD000001"); dl1.push_front("HD000002"); dl1.push_front("HD000003");
    string thu_tu; for (const string &x : dl1) thu_tu += x.substr(7);
    kiem(dl1.size() == 3 && dl1.front() == "HD000003" && thu_tu == "321", "DSLK_DON: push_front -> phần tử thêm sau cùng đứng đầu, duyệt từ đầu");
    DSLK_DON<string> dl2(dl1); dl2.push_front("HD000004");
    kiem(dl1.size() == 3 && dl2.size() == 4 && dl2.front() == "HD000004", "DSLK_DON: sao chép sâu (sửa bản sao không ảnh hưởng bản gốc)");
    dl1.pop_front(); dl1.pop_front(); dl1.pop_front();
    kiem(dl1.empty() && !dl1.pop_front(), "DSLK_DON: pop hết, pop khi rỗng vẫn an toàn");

    HASHTABLE<int> h(8);
    h.put("HD000001", 1); h.put("HD000002", 2); h.put("HD000001", 11);
    kiem(h.size() == 2 && *h.find("HD000001") == 11 && h.find("HD000009") == nullptr, "HASHTABLE: thêm, ghi đè, tra có / không có");
    kiem(h.erase("HD000001") && !h.erase("HD000001") && h.size() == 1, "HASHTABLE: xóa");
    HASHTABLE<int> big(8);
    for (int i = 0; i < 20000; i++) big.put("HD" + to_string(i), i);
    bool ok = big.size() == 20000;
    for (int i = 0; i < 20000; i += 7) { const int *p = big.find("HD" + to_string(i)); if (!p || *p != i) ok = false; }
    kiem(ok, "HASHTABLE: 20000 khóa, tự mở rộng và băm lại nhiều lần mà không mất khóa nào");
    kiem(big.chuoi_dai_nhat() <= 12, "HASHTABLE: hàm băm phân bố đều (chuỗi nối dài nhất " + to_string(big.chuoi_dai_nhat()) + ")");

    HEAP<int> hp([](const int &a, const int &b) { return a > b; });
    int dl[] = {5, 1, 9, 3, 7, 2, 8, 9};
    for (int x : dl) hp.push(x);
    bool dung = true; int truoc = 1 << 30;
    while (!hp.empty()) { int x = hp.pop(); if (x > truoc) dung = false; truoc = x; }
    kiem(dung, "HEAP: pop ra dãy giảm dần");

    // Nhiều khách -> kiểm tra QuanLyKhachHang chạy trên danh sách liên kết
    const char *tam = "Data/kh_bulk.txt";
    error_code ec;
    fs::remove(tam, ec);
    {
        QuanLyKhachHang q(tam);
        q.doc_file();
        bool het = true;
        char sdt[16];
        for (int i = 1; i <= 300; i++) {
            snprintf(sdt, sizeof(sdt), "09%08d", i);
            if (q.dang_ky("Khach " + to_string(i), sdt, "matkhau" + to_string(i), "", "") != DK_OK) het = false;
        }
        kiem(het && q.size() == 300, "QuanLyKhachHang (LIST): đăng ký 300 khách liên tiếp");
        kiem(q.dang_nhap("0900000300", "matkhau300") == 299 && q.dang_nhap("0900000001", "matkhau1") == 0 && q.dang_nhap("0900000150", "matkhau151") == -1,
             "đăng nhập khách ở đầu / cuối / sai mật khẩu");
        kiem(q.tim_theo_ma("KH0300") == 299 && q.lay(299).getTenKH() == "Khach 300", "tìm theo mã và truy cập theo chỉ số");
    }
    {
        QuanLyKhachHang q2(tam);
        q2.doc_file();
        kiem(q2.size() == 300 && q2.dang_nhap("0900000200", "matkhau200") == 199, "mở lại từ file: đủ 300 khách, đăng nhập đúng");
    }
    fs::remove(tam, ec);
}

static void tu_kiem_tra(Shop &shop, QuanLyKhachHang &qk, QuanLyNhanVien &qn) {
    cout << "\n=== KIỂM TRA TỰ ĐỘNG ===\n";
    cout << "(Tự sao lưu item / doanhthu / khachhang / nhanvien, chạy xong tự khôi phục. Dữ liệu thật không bị đổi.)\n";
    so_dat = so_loi = 0;
    error_code ec;
    fs::create_directories("Data", ec);

    const char *files[4] = {F_ITEM, F_DT, F_KH, F_NV};
    bool co[4];
    for (int i = 0; i < 4; i++) {
        co[i] = fs::exists(files[i]);
        if (co[i]) chep_de(files[i], string(files[i]) + ".pretest");
    }
    fs::remove(F_DT, ec); fs::remove(F_KH, ec); fs::remove(F_NV, ec);   // bắt đầu từ sổ / tài khoản trống để số liệu chính xác

    tao_anh_mau("_test_anh_1.png", RED);
    tao_anh_mau("_test_anh_2.png", BLUE);

    shop.doc_file(F_ITEM);
    string ma_du_kien = shop.sinh_ma_sp("Coolmate", "Áo");
    kich_ban(shop, ma_du_kien);
    kich_ban_cau_truc();

    // ---- Khôi phục dữ liệu thật ----
    xoa_tat_ca_anh(ma_du_kien);
    for (int i = 0; i < 4; i++) {
        string tam = string(files[i]) + ".pretest";
        if (co[i]) chep_de(tam, files[i]); else fs::remove(files[i], ec);
        fs::remove(tam, ec);
    }
    fs::remove("_test_anh_1.png", ec);
    fs::remove("_test_anh_2.png", ec);
    shop.doc_file(F_ITEM);
    qk.doc_file();
    qn.doc_file();

    cout << "\n=== KẾT QUẢ: " << so_dat << " đạt, " << so_loi << " lỗi ===\n";
    cout << (so_loi == 0 ? "TẤT CẢ ĐỀU ĐÚNG.\n" : "CÓ LỖI, xem các dòng [LOI] ở trên.\n");
}

// =========================================================================
// MAIN
// =========================================================================
int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");   // để console hiển thị tiếng Việt
#endif
    error_code ec;
    fs::create_directories("Data", ec);
    // Sao lưu 1 lần duy nhất (không đè nếu đã có bản sao lưu)
    for (const char *f : {F_ITEM, F_DT, F_KH, F_NV})
        fs::copy_file(f, string(f) + ".backup", fs::copy_options::skip_existing, ec);

    Shop shop;
    QuanLyKhachHang qk;
    QuanLyNhanVien qn;
    if (!shop.doc_file(F_ITEM)) cout << "(Chưa đọc được " << F_ITEM << ", bắt đầu với kho trống)\n";
    qk.doc_file();
    qn.doc_file();
    if (qn.khoi_tao_admin_neu_trong())
        cout << "(Lần đầu chạy: đã tạo admin NV0001, mật khẩu admin123. Hãy đổi mật khẩu bằng menu 13.)\n";
    cout << "Đã nạp " << shop.size() << " sản phẩm, " << qk.size() << " khách, " << qn.size() << " nhân viên, "
         << shop.lay_doanh_thu().so_giao_dich() << " dòng hàng trong sổ.\n";

    Phien phien;
    while (true) {
        cout << "\n===== MENU THỬ NGHIỆM =====   ["
             << (phien.loai == 0 ? "chưa đăng nhập" : phien.ten + " - " + (phien.loai == 1 ? "Khách" : phien.loai == 2 ? "Nhân viên" : "Admin")) << "]\n"
             << " 1. Xem danh sách sản phẩm\n"
             << " 2. Đăng ký tài khoản khách\n"
             << " 3. Đăng nhập (khách / nhân viên / admin)\n"
             << " 4. Đăng xuất\n"
             << " 5. Mua hàng (giỏ hàng + thanh toán)\n"
             << " 6. Xem hóa đơn\n"
             << " 7. Tìm kiếm / lọc sản phẩm\n"
             << "13. Đổi mật khẩu của tôi\n"
             << " --- chỉ ADMIN ---\n"
             << " 8. Thêm quần áo mới (chọn nhiều ảnh)\n"
             << " 9. Xóa sản phẩm (xóa luôn ảnh)\n"
             << "10. Thống kê doanh thu (kỳ, top kỳ, top sản phẩm, top nhân viên)\n"
             << "11. Quản lý nhân viên (tạo, cấp lại mật khẩu, cho nghỉ)\n"
             << "12. Quản lý khách (cấp lại mật khẩu, khóa tài khoản, hạng)\n"
             << "90. CHẠY KIỂM TRA TỰ ĐỘNG\n"
             << " 0. Thoát\n";
        long long c = doc_so("Chọn: ");
        if (c == 0) break;
        else if (c == 1) xem_ds(shop);
        else if (c == 2) dang_ky_khach(qk);
        else if (c == 3) dang_nhap(phien, qk, qn);
        else if (c == 4) { phien = Phien(); cout << "Đã đăng xuất.\n"; }
        else if (c == 5) mua_hang(shop, qk, phien);
        else if (c == 6) xem_hoa_don(shop, qk, phien);
        else if (c == 7) tim_kiem(shop);
        else if (c == 13) doi_mat_khau(phien, qk, qn);
        else if (c == 8) { if (can_admin(phien)) them_sp(shop); }
        else if (c == 9) { if (can_admin(phien)) xoa_sp(shop); }
        else if (c == 10) { if (can_admin(phien)) thong_ke(shop); }
        else if (c == 11) { if (can_admin(phien)) quan_ly_nv(qn); }
        else if (c == 12) { if (can_admin(phien)) quan_ly_khach(shop, qk); }
        else if (c == 90) { tu_kiem_tra(shop, qk, qn); phien = Phien(); }
    }
    return 0;
}
