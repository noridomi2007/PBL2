#ifndef STAFF_H
#define STAFF_H
#include <string>
using namespace std;

// Độ rộng cột trong nhanvien.txt, cũng là số ký tự tối đa của trường đó khi nhập.
const int RONG_NV_MA      = 8;     // NV0001
const int RONG_NV_TEN     = 28;
const int RONG_NV_SDT     = 10;
const int RONG_NV_EMAIL   = 30;
const int RONG_NV_DIACHI  = 80;
const int RONG_NV_MATKHAU = 25;    // muoi$bam (8 + 1 + 16), không sửa
const int RONG_NV_VAITRO  = 8;     // "nhanvien" là dài nhất
const int RONG_NV_NGAY    = 10;
const int RONG_NV_TT      = 2;

// Nhân viên có 2 vai trò:
//   "admin"    : thêm / xóa / sửa hàng, xem doanh thu, quản lý nhân viên, cấp lại mật khẩu
//   "nhanvien" : chỉ thanh toán (bán hàng tại quầy)
// Doanh số của nhân viên KHÔNG lưu ở đây: tính từ sổ doanh thu (DoanhThu::doanh_so_nhan_vien).
const char *const VAI_TRO_ADMIN = "admin";
const char *const VAI_TRO_NHAN_VIEN = "nhanvien";

class NhanVien {
protected:
    string maNV;         // NV0001...  (dùng để đăng nhập)
    string tenNV;
    string soDienThoai;
    string email;
    string diaChi;
    string matKhau;      // đã mã hóa (xem tienich.h)
    string vaiTro;       // VAI_TRO_ADMIN | VAI_TRO_NHAN_VIEN
    long long ngayVaoLam;
    bool dangLamViec;

public:
    NhanVien();
    NhanVien(const string &maNV, const string &tenNV, const string &sdt, const string &email,
             const string &diaChi, const string &matKhauDaMaHoa, const string &vaiTro,
             long long ngayVaoLam, bool dangLamViec);

    const string &getMaNV() const { return maNV; }
    const string &getTenNV() const { return tenNV; }
    const string &getSoDienThoai() const { return soDienThoai; }
    const string &getEmail() const { return email; }
    const string &getDiaChi() const { return diaChi; }
    const string &getVaiTro() const { return vaiTro; }
    long long getNgayVaoLam() const { return ngayVaoLam; }
    bool getDangLamViec() const { return dangLamViec; }

    bool setTenNV(const string &ten);
    bool setSoDienThoai(const string &sdt);
    bool setEmail(const string &mail);
    bool setDiaChi(const string &dc);
    bool setVaiTro(const string &vt);          // chỉ nhận "admin" hoặc "nhanvien"
    void setDangLamViec(bool tt) { dangLamViec = tt; }

    bool laAdmin() const { return vaiTro == VAI_TRO_ADMIN; }

    bool dangNhap(const string &maDN, const string &matKhauNhap) const;   // đúng mã + mật khẩu + đang làm việc
    bool doiMatKhau(const string &cu, const string &moi);
    void datLaiMatKhau(const string &moi);                                // admin cấp lại

    string xuatThongTin() const;

    // File dạng bảng thẳng hàng:  | maNV | tenNV | sdt | email | diaChi | matKhau | vaiTro | ngayVL | tt |
    static string tieuDe();
    string ghiDong() const;
    static bool docDong(const string &dong, NhanVien &nv);

    bool operator==(const NhanVien &o) const { return maNV == o.maNV; }
};

#endif
