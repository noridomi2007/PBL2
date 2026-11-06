#include "customer.h"
#include "../librarytucode/tienich.h"
#include <sstream>

KhachHang::KhachHang() : ngayDangKy(0), dangHoatDong(true) {}

KhachHang::KhachHang(const string &maKH, const string &tenKH, const string &sdt, const string &email,
                     const string &diaChi, const string &matKhauDaMaHoa, long long ngayDangKy, bool dangHoatDong)
    : maKH(maKH), tenKH(tenKH), soDienThoai(sdt), email(email), diaChi(diaChi),
      matKhau(matKhauDaMaHoa), ngayDangKy(ngayDangKy), dangHoatDong(dangHoatDong) {}

bool KhachHang::setTenKH(const string &ten) {
    if (ten.empty() || co_ky_tu_cam(ten) || do_dai_utf8(ten) > RONG_KH_TEN) return false;
    tenKH = ten;
    return true;
}
bool KhachHang::setSoDienThoai(const string &sdt) {
    if (!sdt_hop_le(sdt)) return false;
    soDienThoai = sdt;
    return true;
}
bool KhachHang::setEmail(const string &mail) {
    if (co_ky_tu_cam(mail) || do_dai_utf8(mail) > RONG_KH_EMAIL) return false;
    email = mail;
    return true;
}
bool KhachHang::setDiaChi(const string &dc) {
    if (co_ky_tu_cam(dc) || do_dai_utf8(dc) > RONG_KH_DIACHI) return false;
    diaChi = dc;
    return true;
}

bool KhachHang::dangNhap(const string &sdt, const string &matKhauNhap) const {
    return dangHoatDong && soDienThoai == sdt && kiem_tra_mat_khau(matKhauNhap, matKhau);
}

bool KhachHang::doiMatKhau(const string &cu, const string &moi) {
    if (!kiem_tra_mat_khau(cu, matKhau)) return false;
    if ((int)moi.size() < MAT_KHAU_TOI_THIEU) return false;
    matKhau = ma_hoa_mat_khau(moi);
    return true;
}

void KhachHang::datLaiMatKhau(const string &moi) { matKhau = ma_hoa_mat_khau(moi); }

string KhachHang::xuatThongTin() const {
    stringstream ss;
    ss << "Mã KH: " << maKH << " | Tên: " << tenKH << " | SĐT: " << soDienThoai
       << " | Email: " << (email.empty() ? "-" : email) << " | Địa chỉ: " << (diaChi.empty() ? "-" : diaChi)
       << " | Ngày ĐK: " << dinh_dang_ngay(ngayDangKy)
       << " | " << (dangHoatDong ? "Hoạt động" : "Đã khóa");
    return ss.str();
}

static const int RONG_COT_KH[8] = { RONG_KH_MA, RONG_KH_TEN, RONG_KH_SDT, RONG_KH_EMAIL,
                                    RONG_KH_DIACHI, RONG_KH_MATKHAU, RONG_KH_NGAY, RONG_KH_TT };

string KhachHang::tieuDe() {
    const string cot[8] = { "maKH", "tenKH", "sdt", "email", "diachi", "matkhau", "ngayDK", "tt" };
    return dong_bang(cot, RONG_COT_KH, 8);
}

string KhachHang::ghiDong() const {
    const string cot[8] = { maKH, tenKH, soDienThoai, email, diaChi, matKhau,
                            to_string(ngayDangKy), dangHoatDong ? "1" : "0" };
    return dong_bang(cot, RONG_COT_KH, 8);
}

bool KhachHang::docDong(const string &dong, KhachHang &kh) {
    string tok[8];
    if (tach_dong_bang(dong, tok, 8) < 8) return false;
    if (tok[0].empty() || !sdt_hop_le(tok[2]) || tok[5].empty()) return false;     // dòng tiêu đề cũng rơi vào đây
    long long ngay = 0;
    try { size_t pos = 0; ngay = stoll(tok[6], &pos); if (pos != tok[6].size()) return false; } catch (...) { return false; }
    kh = KhachHang(tok[0], tok[1], tok[2], tok[3], tok[4], tok[5], ngay, tok[7] == "1");
    return true;
}
