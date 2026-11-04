#ifndef SHOP_H
#define SHOP_H

#include "../item/item.h"
#include "../Vectorandheap/VECTOR.h"
#include <string>
#include "../doanhthu/doanhthu.h"
using namespace std;

// Kết quả của Shop::ban()
enum KetQuaBan {
    BAN_OK,
    BAN_KHONG_CO_SP,
    BAN_KHONG_CO_BIEN_THE,
    BAN_THIEU_HANG,
    BAN_SO_LUONG_SAI,
    BAN_LOI_GHI_FILE,
    BAN_NGAY_SAI           // ngày không có thật hoặc sau ngày hôm nay
};
enum KetQuaThem {
    THEM_OK, THEM_THIEU_THONG_TIN, THEM_SAI_KIEU, THEM_KY_TU_CAM,
    THEM_LOI_ANH, THEM_TRUNG_MA, THEM_LOI_GHI_FILE, THEM_QUA_DAI 
};

class Shop {
private:
    VECTOR<product*> ds; // Danh sách quản lý toàn bộ con trỏ sản phẩm đa hình
    string file_hien_tai = "Filetxt/item.txt"; // file đang dùng (do doc_file ghi nhớ), ban() tự lưu vào đây
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
    // Bán hàng: trừ kho đúng ô (màu, size). Đây là cổng DUY NHẤT để khách mua làm thay đổi tồn kho.
    // Bán thành công thì tự ghi file ngay (trừ kho trong file txt). Ghi file lỗi -> hoàn lại kho, trả BAN_LOI_GHI_FILE
    // (sau này gắn thêm tạo hóa đơn ở đây)
    KetQuaBan ban(const string& id, const string& color, const string& size, int so_luong,
              int ngay, int thang, int nam);

    // Quản lý bộ nhớ và file (Giữ nguyên)
    void clear();                           // Giải phóng toàn bộ bộ nhớ và xóa sạch danh sách
    bool doc_file(const string& duong_dan = "Filetxt/item.txt");       // Đọc file: nhiều dòng cùng idsp được gộp thành 1 sản phẩm nhiều variant
    bool ghi_file(const string& duong_dan = "Filetxt/item.txt") const; // Ghi toàn bộ ds ra file thẳng hàng

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
    KetQuaThem them_quan_ao(const string &ten_hang, const string &ten_kieu,
                        const string &ten_sp, const string &kieu_chi_tiet,
                        long long gia, const string &chat_lieu,
                        const VECTOR<Variant> &bien_the,
                        const VECTOR<string> &ds_anh_nguon,   // nhiều ảnh
                        string *ma_moi = nullptr);
};

#endif