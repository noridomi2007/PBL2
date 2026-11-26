#include "Quanlynhanvien.h"
#include "../Tienich/Tienich.h"
#include <fstream>
#include <cstdio>
#include <filesystem>

static string viet_hoa(string s) {
    for (char &c : s) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    return s;
}

bool QuanLyNhanVien::doc_file() {
    ds.clear();
    ifstream fin(file);
    if (!fin.is_open()) return true;
    string line;
    bool dong_dau = true, co_tieu_de = true;
    while (getline(fin, line)) {
        if (cat_khoang_trang(line).empty()) continue;
        if (dong_dau) { string o[1]; tach_dong_bang(line, o, 1); co_tieu_de = (o[0] == "maNV"); dong_dau = false; }
        NhanVien n;
        if (!NhanVien::docDong(line, n)) continue;
        if (tim_theo_ma(n.getMaNV()) != -1 || tim_theo_sdt(n.getSoDienThoai()) != -1) continue;
        ds.push_back(n);
    }
    fin.close();
    if (!co_tieu_de) ghi_file();     // file cũ chưa có tiêu đề -> tự chuyển sang bảng thẳng hàng một lần
    return true;
}

bool QuanLyNhanVien::ghi_file() const {
    string tam = file + ".tmp";
    {
        ofstream fout(tam);
        if (!fout.is_open()) return false;
        fout << NhanVien::tieuDe() << '\n';
        for (int i = 0; i < ds.size(); i++) fout << ds[i].ghiDong() << '\n';
        fout.close();
        if (fout.fail()) return false;
    }
    error_code ec;
    filesystem::rename(tam, file, ec);
    return !ec;
}

int QuanLyNhanVien::tim_theo_ma(const string &ma) const {
    string m = viet_hoa(ma);
    for (int i = 0; i < ds.size(); i++) if (ds[i].getMaNV() == m) return i;
    return -1;
}

int QuanLyNhanVien::tim_theo_sdt(const string &sdt) const {
    for (int i = 0; i < ds.size(); i++) if (ds[i].getSoDienThoai() == sdt) return i;
    return -1;
}

int QuanLyNhanVien::so_admin_dang_lam() const {
    int dem = 0;
    for (int i = 0; i < ds.size(); i++) if (ds[i].laAdmin() && ds[i].getDangLamViec()) dem++;
    return dem;
}

bool QuanLyNhanVien::khoi_tao_admin_neu_trong() {
    if (ds.size() > 0) return false;
    ds.push_back(NhanVien("NV0001", "Quản trị viên", "0000000000", "", "", ma_hoa_mat_khau("admin123"),
                          VAI_TRO_ADMIN, static_cast<long long>(time(nullptr)), true));
    if (!ghi_file()) { ds.remove(0); return false; }
    return true;
}

KetQuaTaoNV QuanLyNhanVien::tao_nhan_vien(const string &ten, const string &sdt, const string &vai_tro,
                                          const string &email, const string &dia_chi,
                                          const string &mat_khau, string *ma_moi) {
    if (ten.empty()) return NV_THIEU_THONG_TIN;
    if (co_ky_tu_cam(ten) || co_ky_tu_cam(email) || co_ky_tu_cam(dia_chi)) return NV_KY_TU_CAM;
    if (do_dai_utf8(ten) > RONG_NV_TEN || do_dai_utf8(email) > RONG_NV_EMAIL || do_dai_utf8(dia_chi) > RONG_NV_DIACHI)
        return NV_QUA_DAI;
    if (!sdt_hop_le(sdt)) return NV_SDT_SAI;
    if (vai_tro != VAI_TRO_ADMIN && vai_tro != VAI_TRO_NHAN_VIEN) return NV_VAI_TRO_SAI;
    string mk = mat_khau.empty() ? sdt : mat_khau;
    if ((int)mk.size() < MAT_KHAU_TOI_THIEU) return NV_MAT_KHAU_NGAN;
    if (tim_theo_sdt(sdt) != -1) return NV_SDT_TRUNG;

    int max_so = 0;
    for (int i = 0; i < ds.size(); i++) {
        const string &m = ds[i].getMaNV();
        if (m.size() > 2 && m.compare(0, 2, "NV") == 0) {
            try { max_so = max(max_so, stoi(m.substr(2))); } catch (...) {}
        }
    }
    char buf[16];
    snprintf(buf, sizeof(buf), "NV%04d", max_so + 1);
    string ma = buf;

    ds.push_back(NhanVien(ma, ten, sdt, email, dia_chi, ma_hoa_mat_khau(mk), vai_tro,
                          static_cast<long long>(time(nullptr)), true));
    if (!ghi_file()) { ds.remove(ds.size() - 1); return NV_LOI_GHI_FILE; }
    if (ma_moi) *ma_moi = ma;
    return NV_OK;
}

int QuanLyNhanVien::dang_nhap(const string &ma, const string &mat_khau) const {
    int i = tim_theo_ma(ma);
    if (i == -1 || !ds[i].dangNhap(viet_hoa(ma), mat_khau)) return -1;
    return i;
}

bool QuanLyNhanVien::doi_mat_khau(const string &ma, const string &cu, const string &moi) {
    int i = tim_theo_ma(ma);
    if (i == -1) return false;
    NhanVien truoc = ds[i];
    if (!ds[i].doiMatKhau(cu, moi)) return false;
    if (!ghi_file()) { ds[i] = truoc; return false; }
    return true;
}

bool QuanLyNhanVien::dat_lai_mat_khau(const string &ma, const string &moi) {
    int i = tim_theo_ma(ma);
    if (i == -1) return false;
    string mk = moi.empty() ? ds[i].getSoDienThoai() : moi;
    if ((int)mk.size() < MAT_KHAU_TOI_THIEU) return false;
    NhanVien truoc = ds[i];
    ds[i].datLaiMatKhau(mk);
    if (!ghi_file()) { ds[i] = truoc; return false; }
    return true;
}

bool QuanLyNhanVien::dat_trang_thai(const string &ma, bool dang_lam) {
    int i = tim_theo_ma(ma);
    if (i == -1) return false;
    if (!dang_lam && ds[i].laAdmin() && ds[i].getDangLamViec() && so_admin_dang_lam() <= 1) return false;
    bool truoc = ds[i].getDangLamViec();
    ds[i].setDangLamViec(dang_lam);
    if (!ghi_file()) { ds[i].setDangLamViec(truoc); return false; }
    return true;
}

bool QuanLyNhanVien::doi_vai_tro(const string &ma, const string &vai_tro) {
    int i = tim_theo_ma(ma);
    if (i == -1) return false;
    if (vai_tro != VAI_TRO_ADMIN && vai_tro != VAI_TRO_NHAN_VIEN) return false;
    if (vai_tro != VAI_TRO_ADMIN && ds[i].laAdmin() && ds[i].getDangLamViec() && so_admin_dang_lam() <= 1) return false;
    NhanVien truoc = ds[i];
    ds[i].setVaiTro(vai_tro);
    if (!ghi_file()) { ds[i] = truoc; return false; }
    return true;
}
