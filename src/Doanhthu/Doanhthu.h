#ifndef DOANHTHU_H
#define DOANHTHU_H

#include <string>
#include "../Cautrucdulieu/Vector.h"
#include "../Cautrucdulieu/Hash.h"
#include "../Cautrucdulieu/Slist.h"

using namespace std;

const char* const FILE_DOANH_THU = "Data/doanhthu.txt";

// Độ rộng cột trong doanhthu.txt (file dạng bảng thẳng hàng).
const int RONG_DT_THOI_GIAN = 10;   // timestamp 10 chữ số
const int RONG_DT_IDSP      = 9;    // SPCM0001A
const int RONG_DT_TEN       = 64;   // bằng RONG_TEN (độ dài tối đa tên sản phẩm) trong item.h
const int RONG_DT_MAU       = 16;
const int RONG_DT_SIZE      = 8;
const int RONG_DT_SL        = 5;
const int RONG_DT_DON_GIA   = 11;
const int RONG_DT_MA_HD     = 8;    // HD000001
const int RONG_DT_MA_KH     = 8;    // KH0001
const int RONG_DT_MA_NV     = 8;    // NV0001
const int RONG_DT_GIAM_GIA  = 11;

// 1 dòng hàng trong sổ bán hàng. Một hóa đơn = các dòng cùng ma_hd.
// SỔ NÀY LÀ NGUỒN SỰ THẬT DUY NHẤT: doanh thu, chi tiêu của khách (và hạng thành viên),
// doanh số của nhân viên, hóa đơn... đều tính ra từ đây, không lưu số riêng ở nơi khác.
struct GiaoDich {
    long long thoi_gian = 0;   // Unix timestamp lúc bán
    string idsp, ten, color, size;
    int sl = 0;
    long long don_gia = 0;     // giá TẠI LÚC BÁN
    string ma_hd;              // mã hóa đơn, vd HD000001 (rỗng với dòng cũ chưa có hóa đơn)
    string ma_kh;              // khách mua
    string ma_nv;              // nhân viên bán; rỗng nếu khách tự mua
    long long giam_gia = 0;    // tiền giảm của RIÊNG dòng này (giảm giá hạng thành viên)

    long long tam_tinh() const { return don_gia * sl; }               // trước giảm giá
    long long thanh_tien() const { return don_gia * sl - giam_gia; }  // SAU giảm giá = tiền thực thu
};

// Loại kỳ thống kê
enum KieuKy { KY_TUAN, KY_THANG, KY_QUY, KY_NAM };

// Khóa của 1 kỳ (dùng để so sánh / gom nhóm):
//   KY_NAM   -> 2026
//   KY_QUY   -> 20264      (năm*10 + quý)
//   KY_THANG -> 202610     (năm*100 + tháng)
//   KY_TUAN  -> 20261026   (ngày THỨ HAI đầu tuần, dạng yyyymmdd; tuần tính từ Thứ 2 đến Chủ nhật)
long long lay_khoa_ky(long long thoi_gian, KieuKy k);
long long khoa_ky_hien_tai(KieuKy k);
string    ten_ky(long long khoa, KieuKy k);   // "Tháng 10/2026", "Quý 4/2026"...
// Đổi ngày/tháng/năm người dùng nhập -> timestamp. false nếu ngày không có thật hoặc SAU hôm nay.
bool tao_thoi_gian(int ngay, int thang, int nam, long long &ket_qua);

struct DoanhThuKy { long long khoa; long long tong; };                      // doanh thu của 1 kỳ
struct DoanhThuSP { string idsp, ten; long long tong; int so_luong; };      // doanh thu của 1 sản phẩm trong 1 kỳ
struct DoanhThuNV { string ma_nv; long long tong; };                        // doanh số của 1 nhân viên trong 1 kỳ

class DoanhThu {
private:
    VECTOR<GiaoDich> ds;     // sổ theo thứ tự ghi: duyệt tuần tự cho thống kê
    string file;

    // BẢNG BĂM tra hóa đơn: ~O(1) thay vì quét cả sổ. Chỉ là chỉ mục dựng lại từ `ds` mỗi lần mở file.
    HASHTABLE<VECTOR<int>>    chi_muc_hd;    // mã hóa đơn -> chỉ số các dòng hàng trong ds
    HASHTABLE<DSLK_DON<string>> hd_cua_khach; // mã khách  -> DANH SÁCH LIÊN KẾT ĐƠN các mã hóa đơn của khách (mới ghi nhất đứng đầu)
    long long so_hd_lon_nhat = 0;            // số lớn nhất trong các mã HDxxxxxx, để sinh mã kế tiếp O(1)
    HASHTABLE<int> da_ban_sp;                // mã sản phẩm -> tổng số lượng đã bán (tính từ sổ, dùng cho "Đã bán" + lọc bán chạy)
    HASHTABLE<long long> chi_tieu_kh;        // mã khách -> tổng chi tiêu
    HASHTABLE<long long> doanh_so_nv;        // mã nhân viên -> doanh số mọi thời gian
    HASHTABLE<long long> doanh_so_nv_ky;     // "mã_nv|kiểu kỳ|khóa kỳ" -> doanh số trong kỳ

    void lap_chi_muc(int vi_tri);            // đưa dòng thứ vi_tri vào các chỉ mục
    void xay_lai_chi_muc();
    bool chuan_hoa_file() const;             // ghi lại cả sổ dạng bảng thẳng hàng (giữ bản sao file cũ)

public:
    DoanhThu() : file(FILE_DOANH_THU) {}

    bool doc_file(const string &duong_dan = FILE_DOANH_THU);  // chưa có file thì sổ trống (true). Đọc được cả dòng cũ 7 cột; file cũ chưa thẳng hàng tự được chuyển sang bảng một lần.
    bool them(const GiaoDich &g);                             // ghi nối vào file + RAM
    bool them_nhieu(const VECTOR<GiaoDich> &dong);            // ghi MỘT LẦN cả hóa đơn: lỗi thì RAM không thêm gì
    int  so_giao_dich() const { return ds.size(); }
    const GiaoDich &lay(int i) const { return ds[i]; }
    string ma_hd_tiep_theo() const;                           // HD000001, HD000002...

    // Tra hóa đơn bằng bảng băm (nullptr nếu không có). Dùng qua lay_hoa_don() / ds_hoa_don_cua_khach() trong Hoadon.h
    const VECTOR<int>    *dong_cua_hoa_don(const string &ma_hd) const { return chi_muc_hd.find(ma_hd); }
    const DSLK_DON<string> *hoa_don_cua_khach(const string &ma_kh) const { return hd_cua_khach.find(ma_kh); }
    int so_hoa_don() const { return chi_muc_hd.size(); }
    int da_ban(const string &idsp) const { const int *p = da_ban_sp.find(idsp); return p ? *p : 0; }   // O(1)

    // Tất cả số liệu dưới đây tính theo thành tiền SAU giảm giá
    long long tong_doanh_thu(KieuKy k, long long khoa) const;
    VECTOR<DoanhThuKy> top_ky(KieuKy k, int top_n) const;                                   // HEAP
    VECTOR<DoanhThuSP> top_san_pham(KieuKy k, long long khoa, int top_n) const;             // HEAP

    long long tong_chi_tieu_khach(const string &ma_kh) const;                               // quyết định hạng thành viên
    long long doanh_so_nhan_vien(const string &ma_nv) const;                                // mọi thời gian
    long long doanh_so_nhan_vien(const string &ma_nv, KieuKy k, long long khoa) const;      // trong 1 kỳ
    VECTOR<DoanhThuNV> top_nhan_vien(KieuKy k, long long khoa, int top_n) const;            // HEAP
};

#endif
