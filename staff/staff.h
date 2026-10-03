#ifndef STAFF_H
#define STAFF_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

class NhanVien {
protected:
    string tenNV;
    string maNV;
    string soDienThoai;
    string email;
    string diaChi;
    string matKhau;
    string vaiTro;         // "admin", "cashier", "warehouse"
    string ngayVaoLam;     // "dd/mm/yyyy"
    int doanhSo;
    bool dangLamViec;

public:
    // Constructors
    NhanVien();
    NhanVien(string tenNV, string maNV, string soDienThoai,
             string email, string diaChi, string matKhau,
             string vaiTro, string ngayVaoLam, int doanhSo, bool dangLamViec);

    // Getters
    string getTenNV() const;
    string getMaNV() const;
    string getSoDienThoai() const;
    string getEmail() const;
    string getDiaChi() const;
    string getVaiTro() const;
    string getNgayVaoLam() const;
    int getDoanhSo() const;
    bool getDangLamViec() const;

    // Setters
    void setTenNV(string ten);
    void setMaNV(string ma);
    void setSoDienThoai(string sdt);
    void setEmail(string mail);
    void setDiaChi(string dc);
    void setMatKhau(string mk);
    void setVaiTro(string vt);
    void setNgayVaoLam(string ngay);
    void setDoanhSo(int ds);
    void setDangLamViec(bool trangThai);

    // Nghiep vu
    bool dangNhap(string maDN, string matKhauDN) const;
    bool doiMatKhau(string matKhauCu, string matKhauMoi);
    void congDoanhSo(int soTien);
    bool laAdmin() const;
    string xuatThongTin() const;

    // File I/O
    void ghiFile(ofstream &fout) const;
    void docFile(ifstream &fin);

    // Operators
    bool operator==(const NhanVien &other) const;
    friend ostream& operator<<(ostream &os, const NhanVien &nv);
};

#endif