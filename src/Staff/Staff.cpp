#include "Staff.h"
#include "../Tienich/Tienich.h"
#include <sstream>

NhanVien::NhanVien() : vaiTro(VAI_TRO_NHAN_VIEN), ngayVaoLam(0), dangLamViec(true) {}

NhanVien::NhanVien(const string &maNV, const string &tenNV, const string &sdt, const string &email,
                   const string &diaChi, const string &matKhauDaMaHoa, const string &vaiTro,
                   long long ngayVaoLam, bool dangLamViec)
    : maNV(maNV), tenNV(tenNV), soDienThoai(sdt), email(email), diaChi(diaChi),
      matKhau(matKhauDaMaHoa), vaiTro(vaiTro), ngayVaoLam(ngayVaoLam), dangLamViec(dangLamViec) {}

bool NhanVien::setTenNV(const string &ten) {
    if (ten.empty() || co_ky_tu_cam(ten) || do_dai_utf8(ten) > RONG_NV_TEN) return false;
    tenNV = ten;
    return true;
}
bool NhanVien::setSoDienThoai(const string &sdt) {
    if (!sdt_hop_le(sdt)) return false;
    soDienThoai = sdt;
    return true;
}
bool NhanVien::setEmail(const string &mail) {
    if (co_ky_tu_cam(mail) || do_dai_utf8(mail) > RONG_NV_EMAIL) return false;
    email = mail;
    return true;
}
bool NhanVien::setDiaChi(const string &dc) {
    if (co_ky_tu_cam(dc) || do_dai_utf8(dc) > RONG_NV_DIACHI) return false;
    diaChi = dc;
    return true;
}
bool NhanVien::setVaiTro(const string &vt) {
    if (vt != VAI_TRO_ADMIN && vt != VAI_TRO_NHAN_VIEN) return false;
    vaiTro = vt;
    return true;
}

bool NhanVien::dangNhap(const string &maDN, const string &matKhauNhap) const {
    return dangLamViec && maNV == maDN && kiem_tra_mat_khau(matKhauNhap, matKhau);
}

bool NhanVien::doiMatKhau(const string &cu, const string &moi) {
    if (!kiem_tra_mat_khau(cu, matKhau)) return false;
    if ((int)moi.size() < MAT_KHAU_TOI_THIEU) return false;
    matKhau = ma_hoa_mat_khau(moi);
    return true;
}

void NhanVien::datLaiMatKhau(const string &moi) { matKhau = ma_hoa_mat_khau(moi); }

string NhanVien::xuatThongTin() const {
    stringstream ss;
    ss << "Mã NV: " << maNV << " | Tên: " << tenNV << " | SĐT: " << soDienThoai
       << " | Vai trò: " << (laAdmin() ? "Quản trị" : "Nhân viên")
       << " | Vào làm: " << dinh_dang_ngay(ngayVaoLam)
       << " | " << (dangLamViec ? "Đang làm" : "Đã nghỉ");
    return ss.str();
}

static const int RONG_COT_NV[9] = { RONG_NV_MA, RONG_NV_TEN, RONG_NV_SDT, RONG_NV_EMAIL, RONG_NV_DIACHI,
                                    RONG_NV_MATKHAU, RONG_NV_VAITRO, RONG_NV_NGAY, RONG_NV_TT };

string NhanVien::tieuDe() {
    const string cot[9] = { "maNV", "tenNV", "sdt", "email", "diachi", "matkhau", "vaitro", "ngayVL", "tt" };
    return dong_bang(cot, RONG_COT_NV, 9);
}

string NhanVien::ghiDong() const {
    const string cot[9] = { maNV, tenNV, soDienThoai, email, diaChi, matKhau, vaiTro,
                            to_string(ngayVaoLam), dangLamViec ? "1" : "0" };
    return dong_bang(cot, RONG_COT_NV, 9);
}

bool NhanVien::docDong(const string &dong, NhanVien &nv) {
    string tok[9];
    if (tach_dong_bang(dong, tok, 9) < 9) return false;
    if (tok[0].empty() || !sdt_hop_le(tok[2]) || tok[5].empty()) return false;     // dòng tiêu đề cũng rơi vào đây
    if (tok[6] != VAI_TRO_ADMIN && tok[6] != VAI_TRO_NHAN_VIEN) return false;
    long long ngay = 0;
    try { size_t pos = 0; ngay = stoll(tok[7], &pos); if (pos != tok[7].size()) return false; } catch (...) { return false; }
    nv = NhanVien(tok[0], tok[1], tok[2], tok[3], tok[4], tok[5], tok[6], ngay, tok[8] == "1");
    return true;
}
