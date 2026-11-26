#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <string>
using namespace std;

// Độ rộng cột trong khachhang.txt, cũng là số ký tự tối đa của trường đó khi nhập.
const int RONG_KH_MA      = 8;     // KH0001
const int RONG_KH_TEN     = 28;
const int RONG_KH_SDT     = 10;
const int RONG_KH_EMAIL   = 30;
const int RONG_KH_DIACHI  = 80;
const int RONG_KH_MATKHAU = 25;    // muoi$bam (8 + 1 + 16), không sửa
const int RONG_KH_NGAY    = 10;    // timestamp 10 chữ số
const int RONG_KH_TT      = 2;     // 1 = hoạt động, 0 = khóa

// Khách hàng. KHÔNG lưu tiền / hạng / điểm: tổng chi tiêu và hạng được TÍNH TỪ SỔ DOANH THU
// (DoanhThu::tong_chi_tieu_khach + lay_hang) nên không bao giờ lệch với doanh thu thật.
// Tên đăng nhập = số điện thoại.
class KhachHang {
protected:
    string maKH;          // KH0001...
    string tenKH;
    string soDienThoai;   // 10 chữ số, duy nhất, dùng để đăng nhập
    string email;
    string diaChi;
    string matKhau;       // đã mã hóa (xem tienich.h), không bao giờ là mật khẩu thô
    long long ngayDangKy; // timestamp lúc đăng ký (tự động)
    bool dangHoatDong;

public:
    KhachHang();
    KhachHang(const string &maKH, const string &tenKH, const string &sdt, const string &email,
              const string &diaChi, const string &matKhauDaMaHoa, long long ngayDangKy, bool dangHoatDong);

    const string &getMaKH() const { return maKH; }
    const string &getTenKH() const { return tenKH; }
    const string &getSoDienThoai() const { return soDienThoai; }
    const string &getEmail() const { return email; }
    const string &getDiaChi() const { return diaChi; }
    long long getNgayDangKy() const { return ngayDangKy; }
    bool getDangHoatDong() const { return dangHoatDong; }

    // Setter có kiểm tra: false nếu dữ liệu không hợp lệ (rỗng khi bắt buộc, có '|' hoặc xuống dòng...)
    bool setTenKH(const string &ten);
    bool setSoDienThoai(const string &sdt);
    bool setEmail(const string &mail);
    bool setDiaChi(const string &dc);
    void setDangHoatDong(bool tt) { dangHoatDong = tt; }

    // Mật khẩu
    bool dangNhap(const string &sdt, const string &matKhauNhap) const;   // đúng SĐT + mật khẩu + tài khoản đang hoạt động
    bool doiMatKhau(const string &cu, const string &moi);                // phải đúng mật khẩu cũ
    void datLaiMatKhau(const string &moi);                               // admin cấp lại (không cần mật khẩu cũ)

    string xuatThongTin() const;

    // File dạng bảng thẳng hàng, mỗi khách 1 dòng:  | maKH | tenKH | sdt | email | diaChi | matKhau | ngayDK | tt |
    static string tieuDe();     // dòng tiêu đề của bảng
    string ghiDong() const;
    static bool docDong(const string &dong, KhachHang &kh);   // false nếu dòng hỏng hoặc là tiêu đề; đọc được cả dạng cũ a|b|c

    bool operator==(const KhachHang &o) const { return maKH == o.maKH; }
};

#endif
