#include "customer.h"

// ======================== CONSTRUCTORS ========================
KhachHang::KhachHang() : diemTichLuy(0), tongChiTieu(0), hangThanhVien("Bac"), dangHoatDong(true) {}

KhachHang::KhachHang(string tenKH, string maKH, string soDienThoai,
                     string email, string diaChi, string ngayDangKy,
                     double diemTichLuy, double tongChiTieu, string hangThanhVien, bool dangHoatDong)
    : tenKH(tenKH), maKH(maKH), soDienThoai(soDienThoai),
      email(email), diaChi(diaChi), ngayDangKy(ngayDangKy),
      diemTichLuy(diemTichLuy), tongChiTieu(tongChiTieu), hangThanhVien(hangThanhVien),
      dangHoatDong(dangHoatDong) {}

// ======================== GETTERS ========================
string KhachHang::getTenKH() const { return tenKH; }
string KhachHang::getMaKH() const { return maKH; }
string KhachHang::getSoDienThoai() const { return soDienThoai; }
string KhachHang::getEmail() const { return email; }
string KhachHang::getDiaChi() const { return diaChi; }
string KhachHang::getNgayDangKy() const { return ngayDangKy; }
double KhachHang::getDiemTichLuy() const { return diemTichLuy; }
double KhachHang::getTongChiTieu() const { return tongChiTieu; }
string KhachHang::getHangThanhVien() const { return hangThanhVien; }
bool KhachHang::getDangHoatDong() const { return dangHoatDong; }

// ======================== SETTERS ========================
void KhachHang::setTenKH(string ten) { tenKH = ten; }
void KhachHang::setMaKH(string ma) { maKH = ma; }
void KhachHang::setSoDienThoai(string sdt) { soDienThoai = sdt; }
void KhachHang::setEmail(string mail) { email = mail; }
void KhachHang::setDiaChi(string dc) { diaChi = dc; }
void KhachHang::setNgayDangKy(string ngay) { ngayDangKy = ngay; }
void KhachHang::setDiemTichLuy(double diem) { diemTichLuy = diem; }
void KhachHang::setTongChiTieu(double tong) { tongChiTieu = tong; }
void KhachHang::setHangThanhVien(string hang) { hangThanhVien = hang; }
void KhachHang::setDangHoatDong(bool trangThai) { dangHoatDong = trangThai; }

// ======================== NGHIEP VU ========================

// Cong diem thuong sau moi lan mua hang
void KhachHang::congDiem(double soDiem) {
    if (soDiem > 0) {
        diemTichLuy += soDiem;
    }
}

// Dung diem de giam gia, tra ve true neu du diem
bool KhachHang::suDungDiem(double soDiemDung) {
    if (soDiemDung > 0 && soDiemDung <= diemTichLuy) {
        diemTichLuy -= soDiemDung;
        return true;
    }
    return false;
}

// Cong tong chi tieu va tu dong cap nhat hang thanh vien
void KhachHang::congChiTieu(double soTien) {
    if (soTien > 0) {
        tongChiTieu += soTien;
        capNhatHangTV();
    }
}

// Cap nhat hang thanh vien theo tong chi tieu
// Bac: < 2.000.000 | Vang: < 5.000.000 | BachKim: >= 5.000.000
void KhachHang::capNhatHangTV() {
    if (tongChiTieu >= 5000000) {
        hangThanhVien = "BachKim";
    } else if (tongChiTieu >= 2000000) {
        hangThanhVien = "Vang";
    } else {
        hangThanhVien = "Bac";
    }
}

// Tra ve ty le giam gia (%) theo hang thanh vien
// Bac: 3% | Vang: 5% | BachKim: 10%
double KhachHang::getTyLeGiamGia() const {
    if (hangThanhVien == "BachKim") return 10.0;
    if (hangThanhVien == "Vang") return 5.0;
    return 3.0; // Bac
}

// Tinh so tien duoc giam gia tu 1 hoa don
double KhachHang::tinhGiamGia(double tongHoaDon) const {
    return tongHoaDon * getTyLeGiamGia() / 100.0;
}

// Chuyen thong tin thanh chuoi de hien thi
string KhachHang::xuatThongTin() const {
    stringstream ss;
    ss << "Ma KH: " << maKH
       << " | Ten: " << tenKH
       << " | SDT: " << soDienThoai
       << " | Email: " << email
       << " | Dia chi: " << diaChi
       << " | Ngay DK: " << ngayDangKy
       << " | Diem: " << diemTichLuy
       << " | Tong chi: " << tongChiTieu
       << " | Hang: " << hangThanhVien
       << " | Giam gia: " << getTyLeGiamGia() << "%"
       << " | Trang thai: " << (dangHoatDong ? "Hoat dong" : "Ngung");
    return ss.str();
}

// ======================== FILE I/O ========================
// Format: moi field cach nhau boi dau '|'
void KhachHang::ghiFile(ofstream &fout) const {
    fout << tenKH << "|"
         << maKH << "|"
         << soDienThoai << "|"
         << email << "|"
         << diaChi << "|"
         << ngayDangKy << "|"
         << diemTichLuy << "|"
         << tongChiTieu << "|"
         << hangThanhVien << "|"
         << dangHoatDong << endl;
}

void KhachHang::docFile(ifstream &fin) {
    string line;
    if (getline(fin, line)) {
        stringstream ss(line);
        string temp;
        getline(ss, tenKH, '|');
        getline(ss, maKH, '|');
        getline(ss, soDienThoai, '|');
        getline(ss, email, '|');
        getline(ss, diaChi, '|');
        getline(ss, ngayDangKy, '|');
        getline(ss, temp, '|');
        diemTichLuy = stod(temp);
        getline(ss, temp, '|');
        tongChiTieu = stod(temp);
        getline(ss, hangThanhVien, '|');
        getline(ss, temp, '|');
        dangHoatDong = (temp == "1");
    }
}

// ======================== OPERATORS ========================
bool KhachHang::operator==(const KhachHang &other) const {
    return maKH == other.maKH;
}

ostream& operator<<(ostream &os, const KhachHang &kh) {
    os << kh.xuatThongTin();
    return os;
}
