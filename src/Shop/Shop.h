#ifndef SHOP_H
#define SHOP_H

#include "../Item/Item.h"
#include "../Cautrucdulieu/Vector.h"
#include <string>
#include "../Doanhthu/Doanhthu.h"
#include "../Hoadon/Hoadon.h"
using namespace std;

// Kết quả của Shop::thanh_toan()
enum KetQuaThanhToan {
    TT_OK,
    TT_GIO_TRONG,
    TT_THIEU_KHACH,          // chưa có mã khách (khách phải có tài khoản mới mua được)
    TT_NGAY_SAI,             // ngày không có thật hoặc sau ngày hôm nay
    TT_SO_LUONG_SAI,         // có dòng số lượng <= 0
    TT_KHONG_CO_SP,
    TT_KHONG_CO_BIEN_THE,
    TT_THIEU_HANG,
    TT_LOI_GHI_FILE          // không ghi được file -> đã hoàn lại kho, đơn không thành công
};
enum KetQuaThem {
    THEM_OK, THEM_THIEU_THONG_TIN, THEM_SAI_KIEU, THEM_KY_TU_CAM,
    THEM_LOI_ANH, THEM_TRUNG_MA, THEM_LOI_GHI_FILE, THEM_QUA_DAI 
};

// Kết quả của nhập kho / kiểm kê / sửa giá (chỉ admin dùng)
enum KetQuaKho {
    KHO_OK, KHO_KHONG_CO_SP, KHO_KHONG_CO_BIEN_THE, KHO_SAI_SO_LUONG,
    KHO_THIEU_THONG_TIN, KHO_KY_TU_CAM, KHO_QUA_DAI, KHO_LOI_GHI_FILE
};

class Shop {
private:
    VECTOR<product*> ds; // Danh sách quản lý toàn bộ con trỏ sản phẩm đa hình
    string file_hien_tai = "Data/item.txt"; // file đang dùng (do doc_file ghi nhớ), ban() tự lưu vào đây
    DoanhThu doanh_thu;
public:
    // Constructors & Destructor
    Shop();
    ~Shop(); // Tự động giải phóng toàn bộ bộ nhớ con trỏ product*

    // Cấm copy: Shop giữ con trỏ thô, copy sẽ làm 2 Shop cùng delete 1 con trỏ (double free)
    Shop(const Shop&) = delete;
    Shop& operator=(const Shop&) = delete;

    // Các chức năng nghiệp vụ cơ bản (Giữ nguyên)
    bool add(product* p);                   // Thêm sản phẩm mới (chặn trùng mã idsp)
    bool remove(const string& id);          // Xóa sản phẩm theo idsp (thu hồi bộ nhớ an toàn)
    bool xoa_san_pham(const string& id);
    int  find(const string& id) const;      // Tìm vị trí sản phẩm theo idsp, trả về -1 nếu không có
    int  size() const;                      // Số lượng sản phẩm hiện có trong kho
    product* take(int i) const;             // Lấy con trỏ sản phẩm thứ i
    const DoanhThu& lay_doanh_thu() const { return doanh_thu; }
    // THANH TOÁN: cổng DUY NHẤT để bán hàng. Nhận cả giỏ, được TẤT CẢ hoặc KHÔNG GÌ:
    //   1. kiểm tra mọi dòng đủ hàng (chưa trừ gì)   2. tính giảm giá theo HẠNG của khách (hạng tính từ sổ doanh thu)
    //   3. trừ kho, ghi item.txt (lỗi -> hoàn kho)   4. ghi cả hóa đơn vào sổ doanh thu (lỗi -> hoàn kho, lưu lại item.txt)
    // ma_kh: bắt buộc. ma_nv: rỗng nếu khách tự mua online. Ngày: khách tự mua thì truyền ngày hôm nay.
    // Lỗi theo dòng thì *dong_loi là chỉ số dòng trong giỏ (còn lại là -1). Thành công thì hóa đơn ở `ra`.
    KetQuaThanhToan thanh_toan(const GioHang& gio, const string& ma_kh, const string& ma_nv,
                               int ngay, int thang, int nam, HoaDon& ra, int* dong_loi = nullptr);

    // Quản lý bộ nhớ và file (Giữ nguyên)
    void clear();                           // Giải phóng toàn bộ bộ nhớ và xóa sạch danh sách
    bool doc_file(const string& duong_dan = "Data/item.txt");       // Đọc file: nhiều dòng cùng idsp được gộp thành 1 sản phẩm nhiều variant
    bool ghi_file(const string& duong_dan = "Data/item.txt") const; // Ghi toàn bộ ds ra file thẳng hàng

    // Tiện ích tự động sinh mã sản phẩm theo quy tắc Smart ID (Giữ nguyên)
    string sinh_ma_sp(const string& ma_hang, char ma_kieu) const;
    string sinh_ma_sp(const string& ten_hang, const string& ten_kieu) const;

    // =========================================================================
    // TÍNH NĂNG THƯƠNG MẠI ĐIỆN TỬ: LỌC PHÂN CẤP & TÌM KIẾM THEO % KHỚP
    // =========================================================================

    // 1. Các hàm hỗ trợ lấy danh mục cho Giao diện UI (Bộ lọc phân cấp - Cascading Filter):
    // Lấy danh sách tất cả các Hãng hiện có trong kho (không trùng)
    VECTOR<string> lay_ds_hang() const;

    // Lấy danh sách các loại danh mục cha (Áo, Quần, Giày, Mũ...) hiện có
    // Nếu truyền 'hang' thì chỉ lấy subtype của riêng hãng đó
    VECTOR<string> lay_ds_subtype(const string& hang = "") const;

    // Lấy danh sách các kiểu con cụ thể (Áo thun, Áo polo, Quần jean...) thuộc 1 subtype
    // Dùng để UI khi tick vào "Áo" sẽ bung tiếp các kiểu áo con ra cho người dùng tick!
    VECTOR<string> lay_ds_kieu_con(const string& subtype, const string& hang = "") const;

    // 2. Bộ lọc đa tiêu chí phân cấp (Multi-criteria Filter):
    // - hang: tên hoặc mã hãng (rỗng = tất cả)
    // - subtype: danh mục loại "Áo", "Quần", "Mũ"... (rỗng = tất cả)
    // - kieu_cu_the: kiểu chi tiết "Áo thun", "Áo khoác"... (rỗng = tất cả)
    // - gia_min, gia_max: khoảng giá (0 = không giới hạn)
    // - sap_xep: 0: mã SP từ trên xuống (theo hãng), 1: giá tăng dần, 2: giá giảm dần, 3: tên A-Z
    VECTOR<product*> loc_san_pham(const string& hang = "",
                                 const string& subtype = "",
                                 const string& kieu_cu_the = "",
                                 long long gia_min = 0,
                                 long long gia_max = 0,
                                 int sap_xep = 0) const;

    // 3. Tìm kiếm thông minh theo % khớp từ khóa (Smart Keyword Match):
    // Tách từ, tính % khớp từ khóa, sắp xếp sản phẩm từ % khớp cao nhất đến thấp nhất
    // - nguong_khop_min: % khớp tối thiểu để lấy (mặc định 1.0% = chỉ cần khớp ít nhất 1 từ)
    VECTOR<KetQuaTimKiem> tim_kiem(const string& tu_khoa, double nguong_khop_min = 1.0) const;

    // 4. Tìm kiếm kết hợp bộ lọc (cho Khách hàng vừa gõ tìm kiếm vừa lọc hãng/kiểu/giá):
    VECTOR<KetQuaTimKiem> tim_kiem_va_loc(const string& tu_khoa,
                                         const string& hang = "",
                                         const string& subtype = "",
                                         const string& kieu_cu_the = "",
                                         long long gia_min = 0,
                                         long long gia_max = 0,
                                         int sap_xep = 0,
                                         double nguong_khop_min = 1.0) const;
    // QUẢN LÝ KHO (giao diện chỉ cho admin gọi). Cả ba hàm tự ghi file, lỗi ghi thì hoàn lại như cũ.
    // Nhập hàng: ô (màu, size) chưa có thì tạo mới, đã có thì CỘNG DỒN. sl phải > 0.
    KetQuaKho nhap_kho(const string& id, const string& mau, const string& size, int sl);
    // Kiểm kê: đặt thẳng số lượng của 1 ô ĐÃ CÓ (có thể về 0). Dùng khi hàng hỏng / thất lạc / đếm lại.
    KetQuaKho kiem_ke(const string& id, const string& mau, const string& size, int sl);
    KetQuaKho dat_gia(const string& id, long long gia);

    KetQuaThem them_san_pham(const string &ten_hang, const string &ten_kieu,
                        const string &ten_sp, const string &kieu_chi_tiet,
                        long long gia, const string &chat_lieu,
                        const string &kich_thuoc,            
                        const VECTOR<Variant> &bien_the,
                        const VECTOR<string> &ds_anh_nguon,
                        string *ma_moi = nullptr);
};

#endif