#include "quanlykhachhang.h"
#include "../librarytucode/tienich.h"
#include <fstream>
#include <cstdio>
#include <iostream>
#include <filesystem>

bool QuanLyKhachHang::doc_file() {
    ds.clear();
    ifstream fin(file);
    if (!fin.is_open()) return true;
    string line;
    bool dong_dau = true, co_tieu_de = true;
    while (getline(fin, line)) {
        if (cat_khoang_trang(line).empty()) continue;
        if (dong_dau) { string o[1]; tach_dong_bang(line, o, 1); co_tieu_de = (o[0] == "maKH"); dong_dau = false; }
        KhachHang k;
        if (!KhachHang::docDong(line, k)) continue;
        if (tim_theo_ma(k.getMaKH()) != -1 || tim_theo_sdt(k.getSoDienThoai()) != -1) continue;   // bỏ dòng trùng
        ds.push_back(k);
    }
    fin.close();
    if (!co_tieu_de) ghi_file();     // file cũ chưa có tiêu đề -> tự chuyển sang bảng thẳng hàng một lần
    return true;
}

bool QuanLyKhachHang::ghi_file() const {
    string tam = file + ".tmp";
    {
        ofstream fout(tam);
        if (!fout.is_open()) return false;
        fout << KhachHang::tieuDe() << '\n';
        for (const KhachHang &k : ds) fout << k.ghiDong() << '\n';       // duyệt danh sách liên kết từ đầu đến cuối
        fout.close();
        if (fout.fail()) return false;
    }
    error_code ec;
    filesystem::rename(tam, file, ec);
    return !ec;
}

int QuanLyKhachHang::tim_theo_ma(const string &ma) const {
    int i = 0;
    for (const KhachHang &k : ds) { if (k.getMaKH() == ma) return i; i++; }
    return -1;
}

int QuanLyKhachHang::tim_theo_sdt(const string &sdt) const {
    int i = 0;
    for (const KhachHang &k : ds) { if (k.getSoDienThoai() == sdt) return i; i++; }
    return -1;
}

KetQuaDangKy QuanLyKhachHang::dang_ky(const string &ten, const string &sdt, const string &mat_khau,
                                      const string &email, const string &dia_chi, string *ma_moi) {
    if (ten.empty() || mat_khau.empty()) return DK_THIEU_THONG_TIN;
    if (co_ky_tu_cam(ten) || co_ky_tu_cam(email) || co_ky_tu_cam(dia_chi)) return DK_KY_TU_CAM;
    if (do_dai_utf8(ten) > RONG_KH_TEN || do_dai_utf8(email) > RONG_KH_EMAIL || do_dai_utf8(dia_chi) > RONG_KH_DIACHI)
        return DK_QUA_DAI;
    if (!sdt_hop_le(sdt)) return DK_SDT_SAI;
    if ((int)mat_khau.size() < MAT_KHAU_TOI_THIEU) return DK_MAT_KHAU_NGAN;
    if (tim_theo_sdt(sdt) != -1) return DK_SDT_TRUNG;

    // Mã mới = KH + (số lớn nhất hiện có + 1)
    int max_so = 0;
    for (const KhachHang &k : ds) {
        const string &m = k.getMaKH();
        if (m.size() > 2 && m.compare(0, 2, "KH") == 0) {
            try { max_so = max(max_so, stoi(m.substr(2))); } catch (...) {}
        }
    }
    char buf[16];
    snprintf(buf, sizeof(buf), "KH%04d", max_so + 1);
    string ma = buf;

    ds.push_back(KhachHang(ma, ten, sdt, email, dia_chi, ma_hoa_mat_khau(mat_khau),
                           static_cast<long long>(time(nullptr)), true));
    if (!ghi_file()) {
        ds.pop_back();                 // ghi file lỗi -> bỏ khách vừa thêm ở cuối danh sách
        return DK_LOI_GHI_FILE;
    }
    if (ma_moi) *ma_moi = ma;
    return DK_OK;
}

int QuanLyKhachHang::dang_nhap(const string &sdt, const string &mat_khau) const {
    int i = tim_theo_sdt(sdt);
    if (i == -1 || !ds[i].dangNhap(sdt, mat_khau)) return -1;
    return i;
}

bool QuanLyKhachHang::doi_mat_khau(const string &ma, const string &cu, const string &moi) {
    int i = tim_theo_ma(ma);
    if (i == -1) return false;
    KhachHang truoc = ds[i];
    if (!ds[i].doiMatKhau(cu, moi)) return false;
    if (!ghi_file()) { ds[i] = truoc; return false; }
    return true;
}

bool QuanLyKhachHang::dat_lai_mat_khau(const string &ma, const string &moi) {
    int i = tim_theo_ma(ma);
    if (i == -1) return false;
    string mk = moi.empty() ? ds[i].getSoDienThoai() : moi;
    if ((int)mk.size() < MAT_KHAU_TOI_THIEU) return false;
    KhachHang truoc = ds[i];
    ds[i].datLaiMatKhau(mk);
    if (!ghi_file()) { ds[i] = truoc; return false; }
    return true;
}

bool QuanLyKhachHang::dat_trang_thai(const string &ma, bool hoat_dong) {
    int i = tim_theo_ma(ma);
    if (i == -1) return false;
    bool truoc = ds[i].getDangHoatDong();
    ds[i].setDangHoatDong(hoat_dong);
    if (!ghi_file()) { ds[i].setDangHoatDong(truoc); return false; }
    return true;
}
