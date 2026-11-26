#ifndef HOADON_H
#define HOADON_H

#include <string>
#include "../Cautrucdulieu/Vector.h"
#include "../Doanhthu/Doanhthu.h"

using namespace std;

// ---------------- GIỎ HÀNG (nằm trong RAM, mất khi thoát) ----------------
struct DongGio {
    string idsp, color, size;
    int sl = 0;
};

class GioHang {
private:
    VECTOR<DongGio> ds;
public:
    // Thêm món. Cùng (mã, màu, size) đã có thì CỘNG DỒN số lượng. false nếu sl <= 0.
    bool them(const string &idsp, const string &color, const string &size, int sl);
    bool doi_sl(int i, int sl);      // sl <= 0 thì xóa dòng đó
    bool xoa(int i);
    void lam_rong() { ds.clear(); }
    int  so_dong() const { return ds.size(); }
    bool empty() const { return ds.empty(); }
    const DongGio &lay(int i) const { return ds[i]; }
};

// ---------------- HÓA ĐƠN ----------------
// Không có file riêng: hóa đơn là các dòng cùng ma_hd trong sổ doanh thu (nguồn sự thật duy nhất),
// nên in lại hóa đơn cũ lúc nào cũng khớp với doanh thu.
struct HoaDon {
    string ma_hd, ma_kh, ma_nv;
    long long thoi_gian = 0;
    VECTOR<GiaoDich> dong;

    long long tam_tinh() const;     // tổng trước giảm giá
    long long giam_gia() const;     // tổng tiền giảm
    long long thanh_toan() const;   // khách phải trả = tam_tinh - giam_gia
};

HoaDon lay_hoa_don(const DoanhThu &sodt, const string &ma_hd);                // ma_hd rỗng nếu không tìm thấy
VECTOR<HoaDon> ds_hoa_don_cua_khach(const DoanhThu &sodt, const string &ma_kh); // mới nhất trước
string in_hoa_don(const HoaDon &hd);                                           // nhiều dòng, để in ra màn hình / file

#endif
