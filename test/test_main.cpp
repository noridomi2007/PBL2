// test/test_main.cpp
// Chương trình CONSOLE để thử các chức năng của Shop / DoanhThu. Không mở cửa sổ raylib.
// Biên dịch bằng:  mingw32-make test      (xem Makefile)
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <filesystem>
#include "../Shop/Shop.h"
#include "../dialog/dialog.h"

using namespace std;
namespace fs = std::filesystem;

static const char *F_ITEM = "Filetxt/item.txt";
static const char *F_DT   = "Filetxt/doanhthu.txt";

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

// Trả mac_dinh nếu để trống hoặc nhập sai
static long long doc_so(const string &hoi, long long mac_dinh = -1) {
    string s = doc_dong(hoi);
    if (s.empty()) return mac_dinh;
    try { size_t pos = 0; long long v = stoll(s, &pos); if (pos == s.size()) return v; } catch (...) {}
    cout << "  (gia tri khong hop le)\n";
    return mac_dinh;
}

static string tien(long long v) {
    string s = to_string(v), r;
    int dem = 0;
    for (int i = (int)s.size() - 1; i >= 0; i--) {
        r.insert(r.begin(), s[i]);
        if (++dem % 3 == 0 && i > 0) r.insert(r.begin(), '.');
    }
    return r + " VND";
}

static void hom_nay(int &d, int &m, int &y) {
    time_t t = time(nullptr);
    tm *p = localtime(&t);
    d = p->tm_mday; m = p->tm_mon + 1; y = p->tm_year + 1900;
}

static bool doc_ngay(const string &s, int &d, int &m, int &y) {
    if (s.empty()) { hom_nay(d, m, y); return true; }
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

static const char *ten_kq_ban(KetQuaBan k) {
    switch (k) {
        case BAN_OK: return "BAN_OK (thành công)";
        case BAN_KHONG_CO_SP: return "BAN_KHONG_CO_SP (không có mã này)";
        case BAN_KHONG_CO_BIEN_THE: return "BAN_KHONG_CO_BIEN_THE (không có ô màu/size này)";
        case BAN_THIEU_HANG: return "BAN_THIEU_HANG (không đủ hàng)";
        case BAN_SO_LUONG_SAI: return "BAN_SO_LUONG_SAI (số lượng <= 0)";
        case BAN_LOI_GHI_FILE: return "BAN_LOI_GHI_FILE (không ghi được file, đã hoàn kho)";
        case BAN_NGAY_SAI: return "BAN_NGAY_SAI (ngày không có thật hoặc sau hôm nay)";
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
        case THEM_QUA_DAI: return "THEM_QUA_DAI (có trường quá dài hoặc số quá lớn)";
    }
    return "?";
}

// =========================================================================
// 1. XEM DANH SÁCH
// =========================================================================
static void xem_ds(Shop &shop) {
    cout << "\n--- Có " << shop.size() << " sản phẩm ---\n";
    for (int i = 0; i < shop.size(); i++) in_san_pham(shop.take(i));
}

// =========================================================================
// 2. THÊM QUẦN ÁO
// =========================================================================
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
    cout << "Nhập các biến thể (màu + size + số lượng). Để trống MÀU để kết thúc.\n";
    while (true) {
        string mau = doc_dong("  Màu: ");
        if (mau.empty()) break;
        string size = doc_dong("  Size: ");
        long long sl = doc_so("  Số lượng: ", 0);
        bien_the.push_back(Variant(mau, size, (int)sl));
    }

    VECTOR<string> anh;
    cout << "Chọn ảnh (tối đa " << MAX_ANH_MOI_SP << "):\n";
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

// =========================================================================
// 3. XÓA SẢN PHẨM
// =========================================================================
static void xoa_sp(Shop &shop) {
    string ma = doc_dong("\nMã sản phẩm cần xóa: ");
    int vt = shop.find(ma);
    if (vt == -1) { cout << "Không có mã này.\n"; return; }
    in_san_pham(shop.take(vt));
    string id = shop.take(vt)->getid();
    if (doc_dong("Xóa thật? (y/n): ") != "y") { cout << "Huỷ.\n"; return; }
    cout << "Số ảnh trước khi xóa: " << dem_anh(id) << "\n";
    bool ok = shop.xoa_san_pham(id);
    cout << "xóa + lưu file: " << (ok ? "OK" : "LOI")
         << " | số ảnh sau khi xóa: " << dem_anh(id) << " (mong đợi 0)\n";
}

// =========================================================================
// 4. BÁN HÀNG
// =========================================================================
static void ban_hang(Shop &shop) {
    string ma = doc_dong("\nMã sản phẩm: ");
    int vt = shop.find(ma);
    if (vt != -1) in_san_pham(shop.take(vt));
    string mau = doc_dong("Màu (gõ không dấu/hoa thường đều được): ");
    string size = doc_dong("Size: ");
    long long sl = doc_so("Số lượng: ", 0);
    string n = doc_dong("Ngày bán dd/mm/yyyy (Enter = hôm nay; thử 31/2/2026 và ngày tương lai): ");
    int d, m, y;
    if (!doc_ngay(n, d, m, y)) { cout << "Sai định dạng ngày.\n"; return; }

    KetQuaBan kq = shop.ban(ma, mau, size, (int)sl, d, m, y);
    cout << "Kết quả: " << ten_kq_ban(kq) << "\n";
    if (kq == BAN_OK) in_san_pham(shop.take(shop.find(ma)));
}

// =========================================================================
// 5. TÌM KIẾM / LỌC
// =========================================================================
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
    for (int i = 0; i < kq.size(); i++) {
        cout << (int)kq[i].ti_le_khop << "%  ";
        in_san_pham(kq[i].sp);
    }
}

// =========================================================================
// 6. THỐNG KÊ
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
    // Tuần: nhập 1 ngày bất kỳ nằm trong tuần đó
    int d, m, y; long long tg;
    if (!doc_ngay(doc_dong("Một ngày bất kỳ trong tuần (dd/mm/yyyy): "), d, m, y)) return false;
    if (!tao_thoi_gian(d, m, y, tg)) { cout << "  Ngày không hợp lệ.\n"; return false; }
    khoa = lay_khoa_ky(tg, KY_TUAN);
    return true;
}

static void thong_ke(Shop &shop) {
    const DoanhThu &dt = shop.lay_doanh_thu();
    cout << "\nSổ bán hàng hiện có " << dt.so_giao_dich() << " giao dịch.\n";
    cout << "  1. Tổng doanh thu của 1 kỳ\n  2. Top kỳ doanh thu cao nhất\n  3. Top sản phẩm trong 1 kỳ\n";
    long long c = doc_so("Chọn: ");
    KieuKy k; long long khoa;
    if (c < 1 || c > 3 || !chon_kieu_ky(k)) { cout << "Huỷ.\n"; return; }

    if (c == 1) {
        if (!nhap_khoa_ky(k, khoa)) { cout << "Kỳ không hợp lệ.\n"; return; }
        cout << ten_ky(khoa, k) << ": " << tien(dt.tong_doanh_thu(k, khoa)) << "\n";
    } else if (c == 2) {
        long long n = doc_so("Lấy top mấy (Enter = 5): ", 5);
        VECTOR<DoanhThuKy> t = dt.top_ky(k, (int)n);
        for (int i = 0; i < t.size(); i++)
            cout << "  #" << i + 1 << "  " << ten_ky(t[i].khoa, k) << "  " << tien(t[i].tong) << "\n";
        if (t.empty()) cout << "  (chưa có dữ liệu)\n";
    } else {
        if (!nhap_khoa_ky(k, khoa)) { cout << "Kỳ không hợp lệ.\n"; return; }
        long long n = doc_so("Lấy top mấy (Enter = 5): ", 5);
        VECTOR<DoanhThuSP> t = dt.top_san_pham(k, khoa, (int)n);
        cout << "Top sản phẩm trong " << ten_ky(khoa, k) << ":\n";
        for (int i = 0; i < t.size(); i++)
            cout << "  #" << i + 1 << "  " << t[i].idsp << " | " << t[i].ten << " | "
                 << t[i].so_luong << " cái | " << tien(t[i].tong) << "\n";
        if (t.empty()) cout << "  (chưa có dữ liệu)\n";
    }
}

// =========================================================================
// 9. KIỂM TRA TỰ ĐỘNG
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
    bt.push_back(Variant("Đen", "M", 5));
    bt.push_back(Variant("Trắng", "L", 2));
    VECTOR<Variant> bt_rong;

    int so_sp_ban_dau = shop.size();

    cout << "\n[A] Thêm sản phẩm + ảnh\n";
    kiem(shop.them_quan_ao("Coolmate", "Áo", "Ao test", "Áo thun", 199000, "Cotton", bt_rong, hai_anh) == THEM_THIEU_THONG_TIN, "không có biến thể -> THIEU_THONG_TIN");
    kiem(shop.them_quan_ao("Coolmate", "Mũ", "Mu test", "Mũ lưỡi trai", 99000, "Cotton", bt, hai_anh) == THEM_SAI_KIEU, "kiểu Mũ (phụ kiện) -> SAI_KIEU");
    kiem(shop.them_quan_ao("Coolmate", "Áo", "Ao|loi", "Áo thun", 199000, "Cotton", bt, hai_anh) == THEM_KY_TU_CAM, "tên có ký tự | -> KY_TU_CAM");
    kiem(shop.them_quan_ao("Coolmate", "Áo", "Ao test", "Áo thun", 199000, "Cotton", bt, khong_anh) == THEM_LOI_ANH, "không chọn ảnh -> LOI_ANH");
    kiem(shop.them_quan_ao("Coolmate", "Áo", "Ao test", "Áo thun", 199000, "Cotton", bt, anh_ma) == THEM_LOI_ANH, "ảnh không tồn tại -> LOI_ANH");
    kiem(shop.size() == so_sp_ban_dau && dem_anh(ma_du_kien) == 0, "các ca lỗi không để lại sản phẩm hay ảnh rác");

    string ma;
    KetQuaThem kq = shop.them_quan_ao("Coolmate", "Áo", "Ao test", "Áo thun", 199000, "Cotton", bt, hai_anh, &ma);
    kiem(kq == THEM_OK, "thêm hợp lệ -> THEM_OK");
    kiem(ma == ma_du_kien && !ma.empty(), "mã sinh ra đúng bằng mã dự kiến: " + ma);
    kiem(fs::exists(duong_dan_anh(ma, 1)) && fs::exists(duong_dan_anh(ma, 2)) && !fs::exists(duong_dan_anh(ma, 3)), "có đúng 2 ảnh: " + duong_dan_anh(ma, 1) + ", _2");
    kiem(dem_anh(ma) == 2, "dem_anh = 2");
    kiem(file_chua(F_ITEM, ma), "item.txt đã chứa mã mới");

    cout << "\n[B] Đọc lại từ file + tìm kiếm\n";
    Shop s2;
    s2.doc_file(F_ITEM);
    int vt = s2.find(ma);
    kiem(vt != -1, "đọc lại file: tìm thấy sản phẩm");
    if (vt == -1) { cout << "  Dừng vì không thấy sản phẩm.\n"; return; }
    {
        product *p = s2.take(vt);
        kiem(p->so_variant() == 2, "đủ 2 biến thể sau khi đọc lại");
        kiem(p->getsl("den", "m") == 5 && p->getsl("TRANG", "l") == 2, "tồn kho đúng (tra màu/size không dấu, không phân biệt hoa thường)");
        VECTOR<KetQuaTimKiem> r = s2.tim_kiem("ao test");
        bool thay = false;
        for (int i = 0; i < r.size(); i++) if (r[i].sp->getid() == ma && r[i].ti_le_khop == 100.0) thay = true;
        kiem(thay, "tìm 'ao test' ra sản phẩm với 100%");
        kiem(s2.tim_kiem("zzzkhongco").size() == 0, "từ khóa không có -> 0 kết quả");
        VECTOR<product*> loc = s2.loc_san_pham("Coolmate", "Áo");
        thay = false;
        for (int i = 0; i < loc.size(); i++) if (loc[i]->getid() == ma) thay = true;
        kiem(thay, "lọc hãng Coolmate + loại Áo có sản phẩm");
    }

    cout << "\n[C] Bán hàng (các ca lỗi không được đụng vào kho / sổ)\n";
    kiem(s2.ban(ma, "Đen", "M", 0, 15, 1, 2025) == BAN_SO_LUONG_SAI, "số lượng 0");
    kiem(s2.ban("SPXX9999A", "Đen", "M", 1, 15, 1, 2025) == BAN_KHONG_CO_SP, "mã không tồn tại");
    kiem(s2.ban(ma, "Xanh", "M", 1, 15, 1, 2025) == BAN_KHONG_CO_BIEN_THE, "màu không tồn tại");
    kiem(s2.ban(ma, "Đen", "M", 99, 15, 1, 2025) == BAN_THIEU_HANG, "thiếu hàng");
    kiem(s2.ban(ma, "Đen", "M", 1, 31, 2, 2025) == BAN_NGAY_SAI, "ngày 31/2/2025");
    kiem(s2.ban(ma, "Đen", "M", 1, 29, 2, 2025) == BAN_NGAY_SAI, "ngày 29/2/2025 (không nhuận)");
    kiem(s2.ban(ma, "Đen", "M", 1, 1, 13, 2025) == BAN_NGAY_SAI, "tháng 13");
    kiem(s2.ban(ma, "Đen", "M", 1, 1, 1, 2099) == BAN_NGAY_SAI, "ngày tương lai 1/1/2099");
    kiem(s2.take(vt)->getsl("Đen", "M") == 5 && s2.lay_doanh_thu().so_giao_dich() == 0, "kho vẫn 5, sổ vẫn rỗng sau 8 ca lỗi");

    kiem(s2.ban(ma, "đen", "m", 3, 15, 1, 2025) == BAN_OK, "bán 3 Đen-M ngày 15/1/2025 (gõ thường, không dấu)");
    kiem(s2.ban(ma, "TRANG", "l", 1, 20, 2, 2025) == BAN_OK, "bán 1 Trắng-L ngày 20/2/2025");
    kiem(s2.ban(ma, "Đen", "M", 1, 5, 3, 2025) == BAN_OK, "bán 1 Đen-M ngày 5/3/2025");
    kiem(s2.take(vt)->getsl("Đen", "M") == 1 && s2.take(vt)->getsl("Trắng", "L") == 1, "kho còn Đen-M 1, Trắng-L 1");

    cout << "\n[D] Thống kê doanh thu (giá 199.000)\n";
    const DoanhThu &dt = s2.lay_doanh_thu();
    kiem(dt.so_giao_dich() == 3, "sổ có 3 giao dịch");
    kiem(dt.tong_doanh_thu(KY_THANG, 202501) == 597000, "tháng 1/2025 = 597.000");
    kiem(dt.tong_doanh_thu(KY_THANG, 202502) == 199000, "tháng 2/2025 = 199.000");
    kiem(dt.tong_doanh_thu(KY_THANG, 202503) == 199000, "tháng 3/2025 = 199.000");
    kiem(dt.tong_doanh_thu(KY_QUY, 20251) == 995000, "quý 1/2025 = 995.000");
    kiem(dt.tong_doanh_thu(KY_NAM, 2025) == 995000, "năm 2025 = 995.000");
    kiem(dt.tong_doanh_thu(KY_TUAN, 20250113) == 597000, "tuần từ T2 13/1/2025 = 597.000");
    VECTOR<DoanhThuKy> tk = dt.top_ky(KY_THANG, 3);
    kiem(tk.size() == 3 && tk[0].khoa == 202501 && tk[1].khoa == 202503 && tk[2].khoa == 202502,
         "top 3 tháng: 1/2025, rồi 3/2025, rồi 2/2025 (bằng tiền thì kỳ mới hơn đứng trước)");
    VECTOR<DoanhThuSP> ts = dt.top_san_pham(KY_QUY, 20251, 5);
    kiem(ts.size() == 1 && ts[0].idsp == ma && ts[0].tong == 995000 && ts[0].so_luong == 5, "top sản phẩm quý 1/2025: 5 cái, 995.000");
    kiem(file_chua(F_DT, ma), "doanhthu.txt đã có dòng giao dịch");

    cout << "\n[E] Mở lại app: dữ liệu phải còn nguyên\n";
    {
        Shop s3;
        s3.doc_file(F_ITEM);
        int v3 = s3.find(ma);
        kiem(v3 != -1 && s3.take(v3)->getsl("Đen", "M") == 1, "kho Đen-M vẫn là 1");
        kiem(s3.lay_doanh_thu().so_giao_dich() == 3 && s3.lay_doanh_thu().tong_doanh_thu(KY_NAM, 2025) == 995000, "sổ vẫn 3 giao dịch, năm 2025 vẫn 995.000");

        cout << "\n[F] Xóa sản phẩm\n";
        kiem(s3.xoa_san_pham(ma), "xoa_san_pham trả true (đã tự ghi file)");
        kiem(dem_anh(ma) == 0 && !fs::exists(duong_dan_anh(ma, 1)) && !fs::exists(duong_dan_anh(ma, 2)), "toàn bộ ảnh đã bị xóa theo");
        kiem(s3.find(ma) == -1, "không còn trong danh sách");
        Shop s4;
        s4.doc_file(F_ITEM);
        kiem(s4.find(ma) == -1 && !file_chua(F_ITEM, ma), "mở lại file: sản phẩm đã mất hẳn");
        kiem(s4.lay_doanh_thu().so_giao_dich() == 3, "doanh thu cũ vẫn giữ nguyên (đúng, vì đã bán rồi)");
    }

}

static void tu_kiem_tra(Shop &shop) {
    cout << "\n=== KIỂM TRA TỰ ĐỘNG ===\n";
    cout << "(Tự sao lưu item.txt + doanhthu.txt, chạy xong tự khôi phục. Dữ liệu thật không bị đổi.)\n";
    so_dat = so_loi = 0;
    error_code ec;
    fs::create_directories("Filetxt", ec);

    // ---- Sao lưu và dọn sổ bán hàng để số liệu kiểm tra chính xác ----
    bool co_item = fs::exists(F_ITEM), co_dt = fs::exists(F_DT);
    if (co_item) chep_de(F_ITEM, "Filetxt/item.txt.pretest");
    if (co_dt)   chep_de(F_DT,   "Filetxt/doanhthu.txt.pretest");
    fs::remove(F_DT, ec);

    tao_anh_mau("_test_anh_1.png", RED);
    tao_anh_mau("_test_anh_2.png", BLUE);

    shop.doc_file(F_ITEM);
    string ma_du_kien = shop.sinh_ma_sp("Coolmate", "Áo");
    kich_ban(shop, ma_du_kien);

    // ---- Khôi phục dữ liệu thật ----
    xoa_tat_ca_anh(ma_du_kien);
    if (co_item) chep_de("Filetxt/item.txt.pretest", F_ITEM); else fs::remove(F_ITEM, ec);
    if (co_dt)   chep_de("Filetxt/doanhthu.txt.pretest", F_DT); else fs::remove(F_DT, ec);
    fs::remove("Filetxt/item.txt.pretest", ec);
    fs::remove("Filetxt/doanhthu.txt.pretest", ec);
    fs::remove("_test_anh_1.png", ec);
    fs::remove("_test_anh_2.png", ec);
    shop.doc_file(F_ITEM);

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
    fs::create_directories("Filetxt", ec);
    // Sao lưu 1 lần duy nhất (không đè nếu đã có bản sao lưu)
    fs::copy_file(F_ITEM, "Filetxt/item.txt.backup", fs::copy_options::skip_existing, ec);
    fs::copy_file(F_DT, "Filetxt/doanhthu.txt.backup", fs::copy_options::skip_existing, ec);

    Shop shop;
    if (!shop.doc_file(F_ITEM)) cout << "(Chưa đọc được " << F_ITEM << ", bắt đầu với kho trống)\n";
    cout << "Đã nạp " << shop.size() << " sản phẩm, " << shop.lay_doanh_thu().so_giao_dich() << " giao dịch.\n";

    while (true) {
        cout << "\n===== MENU THỬ NGHIỆM =====\n"
             << " 1. Xem danh sách sản phẩm\n"
             << " 2. Thêm quần áo mới (chọn nhiều ảnh)\n"
             << " 3. Xóa sản phẩm (xóa luôn ảnh)\n"
             << " 4. Bán hàng (nhập ngày)\n"
             << " 5. Tìm kiếm / lọc\n"
             << " 6. Thống kê doanh thu (tuần/tháng/quý/năm)\n"
             << " 7. Đọc lại file\n"
             << " 9. CHẠY KIỂM TRA TỰ ĐỘNG\n"
             << " 0. Thoát\n";
        long long c = doc_so("Chọn: ");
        if (c == 0) break;
        else if (c == 1) xem_ds(shop);
        else if (c == 2) them_sp(shop);
        else if (c == 3) xoa_sp(shop);
        else if (c == 4) ban_hang(shop);
        else if (c == 5) tim_kiem(shop);
        else if (c == 6) thong_ke(shop);
        else if (c == 7) { shop.doc_file(F_ITEM); cout << "Đã nạp " << shop.size() << " sản phẩm.\n"; }
        else if (c == 9) tu_kiem_tra(shop);
    }
    return 0;
}