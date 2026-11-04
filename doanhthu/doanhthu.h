#ifndef DOANHTHU_H
#define DOANHTHU_H

#include <string>
#include "../Vectorandheap/VECTOR.h"

using namespace std;

const char* const FILE_DOANH_THU = "Filetxt/doanhthu.txt";

// 1 dòng trong sổ bán hàng
struct GiaoDich {
    long long thoi_gian;   // Unix timestamp lúc bán
    string idsp, ten, color, size;
    int sl;
    long long don_gia;     // giá TẠI LÚC BÁN
    long long thanh_tien() const { return don_gia * sl; }
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
bool tao_thoi_gian(int ngay, int thang, int nam, long long &ket_qua); 
struct DoanhThuKy { long long khoa; long long tong; };                       // doanh thu của 1 kỳ
struct DoanhThuSP { string idsp, ten; long long tong; int so_luong; };       // doanh thu của 1 sản phẩm trong 1 kỳ

class DoanhThu {
private:
    VECTOR<GiaoDich> ds;
    string file;

public:
    DoanhThu() : file(FILE_DOANH_THU) {}

    bool doc_file(const string &duong_dan = FILE_DOANH_THU);  // chưa có file thì coi như sổ trống (trả true)
    bool them(const GiaoDich &g);                             // ghi nối vào file + RAM
    int  so_giao_dich() const { return ds.size(); }

    long long tong_doanh_thu(KieuKy k, long long khoa) const;     // tổng doanh thu 1 kỳ cụ thể
    VECTOR<DoanhThuKy> top_ky(KieuKy k, int top_n) const;         // top kỳ doanh thu cao nhất (HEAP)
    VECTOR<DoanhThuSP> top_san_pham(KieuKy k, long long khoa, int top_n) const; // top sản phẩm trong 1 kỳ (HEAP)
};

#endif