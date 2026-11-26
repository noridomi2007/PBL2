#ifndef QUANLYKHACHHANG_H
#define QUANLYKHACHHANG_H
#include "Customer.h"
#include "../Cautrucdulieu/List.h"

const char *const FILE_KHACH_HANG = "Data/khachhang.txt";

enum KetQuaDangKy {
    DK_OK, DK_THIEU_THONG_TIN, DK_KY_TU_CAM, DK_SDT_SAI, DK_MAT_KHAU_NGAN, DK_SDT_TRUNG, DK_LOI_GHI_FILE,
    DK_QUA_DAI                  // tên / email / địa chỉ dài hơn độ rộng cột (RONG_KH_...)
};

// Giữ toàn bộ khách trong RAM bằng DANH SÁCH LIÊN KẾT ĐÔI (LIST<KhachHang>), mỗi thay đổi ghi file NGAY (giống Shop).
// Tìm theo mã / SĐT là duyệt danh sách O(n); thêm khách mới ở cuối là O(1).
// Phân quyền (ai được reset mật khẩu, khóa tài khoản...) do giao diện kiểm tra bằng NhanVien::laAdmin().
class QuanLyKhachHang {
private:
    LIST<KhachHang> ds;
    string file;
public:
    QuanLyKhachHang(const string &duong_dan = FILE_KHACH_HANG) : file(duong_dan) {}

    bool doc_file();                 // chưa có file thì danh sách rỗng (trả true); dòng hỏng bị bỏ qua
    bool ghi_file() const;           // ghi file tạm rồi đổi tên, lỗi giữa chừng không mất file cũ

    int  size() const { return ds.size(); }
    const KhachHang &lay(int i) const { return ds[i]; }
    int  tim_theo_ma(const string &ma) const;     // -1 nếu không có
    int  tim_theo_sdt(const string &sdt) const;

    // Tạo tài khoản. Mã KHxxxx tự sinh. email / địa chỉ có thể để trống.
    KetQuaDangKy dang_ky(const string &ten, const string &sdt, const string &mat_khau,
                         const string &email, const string &dia_chi, string *ma_moi = nullptr);

    int  dang_nhap(const string &sdt, const string &mat_khau) const;   // trả chỉ số khách, -1 nếu sai
    bool doi_mat_khau(const string &ma, const string &cu, const string &moi);
    // Admin cấp lại: moi rỗng -> đặt mật khẩu = số điện thoại
    bool dat_lai_mat_khau(const string &ma, const string &moi = "");
    bool dat_trang_thai(const string &ma, bool hoat_dong);   // khóa / mở tài khoản (không xóa, vì sổ doanh thu còn mã này)
};

#endif
