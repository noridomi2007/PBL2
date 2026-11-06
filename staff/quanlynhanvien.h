#ifndef QUANLYNHANVIEN_H
#define QUANLYNHANVIEN_H
#include "staff.h"
#include "../Vectorandheap/VECTOR.h"

const char *const FILE_NHAN_VIEN = "Filetxt/nhanvien.txt";

enum KetQuaTaoNV {
    NV_OK, NV_THIEU_THONG_TIN, NV_KY_TU_CAM, NV_SDT_SAI, NV_SDT_TRUNG, NV_VAI_TRO_SAI,
    NV_MAT_KHAU_NGAN, NV_LOI_GHI_FILE,
    NV_QUA_DAI                  // tên / email / địa chỉ dài hơn độ rộng cột (RONG_NV_...)
};

// Nhân viên không bị xóa (sổ doanh thu còn lưu mã), chỉ chuyển sang "đã nghỉ".
// Phân quyền do giao diện kiểm tra bằng NhanVien::laAdmin().
class QuanLyNhanVien {
private:
    VECTOR<NhanVien> ds;
    string file;
    int so_admin_dang_lam() const;
public:
    QuanLyNhanVien(const string &duong_dan = FILE_NHAN_VIEN) : file(duong_dan) {}

    bool doc_file();
    bool ghi_file() const;

    int  size() const { return ds.size(); }
    const NhanVien &lay(int i) const { return ds[i]; }
    int  tim_theo_ma(const string &ma) const;
    int  tim_theo_sdt(const string &sdt) const;

    // Lần chạy đầu chưa có ai: tạo admin NV0001, mật khẩu "admin123" (PHẢI đổi ngay). Trả true nếu vừa tạo.
    bool khoi_tao_admin_neu_trong();

    // Admin tạo nhân viên. mat_khau rỗng -> mật khẩu mặc định = số điện thoại.
    KetQuaTaoNV tao_nhan_vien(const string &ten, const string &sdt, const string &vai_tro,
                              const string &email = "", const string &dia_chi = "",
                              const string &mat_khau = "", string *ma_moi = nullptr);

    int  dang_nhap(const string &ma, const string &mat_khau) const;   // chỉ số nhân viên, -1 nếu sai
    bool doi_mat_khau(const string &ma, const string &cu, const string &moi);
    bool dat_lai_mat_khau(const string &ma, const string &moi = "");  // rỗng -> mật khẩu = SĐT
    // Cho nghỉ / đi làm lại. Không cho vô hiệu hóa admin cuối cùng (sẽ không ai vào quản lý được nữa).
    bool dat_trang_thai(const string &ma, bool dang_lam);
    bool doi_vai_tro(const string &ma, const string &vai_tro);        // cũng không được hạ admin cuối cùng
};

#endif
