#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

class KhachHang {
protected:
    string tenKH;
    string maKH;
    string soDienThoai;
    string email;
    string diaChi;
    string ngayDangKy;        // "dd/mm/yyyy"
    double diemTichLuy;
    double tongChiTieu;
    string hangThanhVien;     // "Bac", "Vang", "BachKim"
    bool dangHoatDong;

public:
    // Constructors
    KhachHang();
    KhachHang(string tenKH, string maKH, string soDienThoai,
              string email, string diaChi, string ngayDangKy,
              double diemTichLuy, double tongChiTieu, string hangThanhVien, bool dangHoatDong);

    // Getters
    string getTenKH() const;
    string getMaKH() const;
    string getSoDienThoai() const;
    string getEmail() const;
    string getDiaChi() const;
    string getNgayDangKy() const;
    double getDiemTichLuy() const;
    double getTongChiTieu() const;
    string getHangThanhVien() const;
    bool getDangHoatDong() const;

    // Setters
    void setTenKH(string ten);
    void setMaKH(string ma);
    void setSoDienThoai(string sdt);
    void setEmail(string mail);
    void setDiaChi(string dc);
    void setNgayDangKy(string ngay);
    void setDiemTichLuy(double diem);
    void setTongChiTieu(double tong);
    void setHangThanhVien(string hang);
    void setDangHoatDong(bool trangThai);

    // Nghiep vu
    void congDiem(double soDiem);
    bool suDungDiem(double soDiemDung);
    void congChiTieu(double soTien);
    void capNhatHangTV();
    double getTyLeGiamGia() const;
    double tinhGiamGia(double tongHoaDon) const;
    string xuatThongTin() const;

    // File I/O
    void ghiFile(ofstream &fout) const;
    void docFile(ifstream &fin);

    // Operators
    bool operator==(const KhachHang &other) const;
    friend ostream& operator<<(ostream &os, const KhachHang &kh);
};

#endif