#include "staff.h"

// ======================== CONSTRUCTORS ========================
NhanVien::NhanVien() : doanhSo(0), dangLamViec(true) {}

NhanVien::NhanVien(string tenNV, string maNV, string soDienThoai,
                   string email, string diaChi, string matKhau,
                   string vaiTro, string ngayVaoLam, int doanhSo, bool dangLamViec)
    : tenNV(tenNV), maNV(maNV), soDienThoai(soDienThoai),
      email(email), diaChi(diaChi), matKhau(matKhau),
      vaiTro(vaiTro), ngayVaoLam(ngayVaoLam), doanhSo(doanhSo),
      dangLamViec(dangLamViec) {}

// ======================== GETTERS ========================
string NhanVien::getTenNV() const { return tenNV; }
string NhanVien::getMaNV() const { return maNV; }
string NhanVien::getSoDienThoai() const { return soDienThoai; }
string NhanVien::getEmail() const { return email; }
string NhanVien::getDiaChi() const { return diaChi; }
string NhanVien::getVaiTro() const { return vaiTro; }
string NhanVien::getNgayVaoLam() const { return ngayVaoLam; }
int NhanVien::getDoanhSo() const { return doanhSo; }
bool NhanVien::getDangLamViec() const { return dangLamViec; }

// ======================== SETTERS ========================
void NhanVien::setTenNV(string ten) { tenNV = ten; }
void NhanVien::setMaNV(string ma) { maNV = ma; }
void NhanVien::setSoDienThoai(string sdt) { soDienThoai = sdt; }
void NhanVien::setEmail(string mail) { email = mail; }
void NhanVien::setDiaChi(string dc) { diaChi = dc; }
void NhanVien::setMatKhau(string mk) { matKhau = mk; }
void NhanVien::setVaiTro(string vt) { vaiTro = vt; }
void NhanVien::setNgayVaoLam(string ngay) { ngayVaoLam = ngay; }
void NhanVien::setDoanhSo(int ds) { doanhSo = ds; }
void NhanVien::setDangLamViec(bool trangThai) { dangLamViec = trangThai; }

// ======================== NGHIEP VU ========================

// Xac thuc dang nhap: tra ve true neu ma NV va mat khau dung
bool NhanVien::dangNhap(string maDN, string matKhauDN) const {
    return (maNV == maDN && matKhau == matKhauDN && dangLamViec);
}

// Doi mat khau: can nhap dung mat khau cu
bool NhanVien::doiMatKhau(string matKhauCu, string matKhauMoi) {
    if (matKhau == matKhauCu && !matKhauMoi.empty()) {
        matKhau = matKhauMoi;
        return true;
    }
    return false;
}

// Cong doanh so ban hang
void NhanVien::congDoanhSo(int soTien) {
    if (soTien > 0) {
        doanhSo += soTien;
    }
}

// Kiem tra co phai admin khong
bool NhanVien::laAdmin() const {
    return (vaiTro == "admin");
}

// Chuyen thong tin thanh chuoi de hien thi
string NhanVien::xuatThongTin() const {
    stringstream ss;
    ss << "Ma NV: " << maNV
       << " | Ten: " << tenNV
       << " | SDT: " << soDienThoai
       << " | Email: " << email
       << " | Dia chi: " << diaChi
       << " | Vai tro: " << vaiTro
       << " | Ngay vao lam: " << ngayVaoLam
       << " | Doanh so: " << doanhSo
       << " | Trang thai: " << (dangLamViec ? "Dang lam" : "Da nghi");
    return ss.str();
}

// ======================== FILE I/O ========================
// Format: moi field cach nhau boi dau '|'
void NhanVien::ghiFile(ofstream &fout) const {
    fout << tenNV << "|"
         << maNV << "|"
         << soDienThoai << "|"
         << email << "|"
         << diaChi << "|"
         << matKhau << "|"
         << vaiTro << "|"
         << ngayVaoLam << "|"
         << doanhSo << "|"
         << dangLamViec << endl;
}

void NhanVien::docFile(ifstream &fin) {
    string line;
    if (getline(fin, line)) {
        stringstream ss(line);
        string temp;
        getline(ss, tenNV, '|');
        getline(ss, maNV, '|');
        getline(ss, soDienThoai, '|');
        getline(ss, email, '|');
        getline(ss, diaChi, '|');
        getline(ss, matKhau, '|');
        getline(ss, vaiTro, '|');
        getline(ss, ngayVaoLam, '|');
        getline(ss, temp, '|');
        doanhSo = stoi(temp);
        getline(ss, temp, '|');
        dangLamViec = (temp == "1");
    }
}

// ======================== OPERATORS ========================
bool NhanVien::operator==(const NhanVien &other) const {
    return maNV == other.maNV;
}

ostream& operator<<(ostream &os, const NhanVien &nv) {
    os << nv.xuatThongTin();
    return os;
}
