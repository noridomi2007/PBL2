#ifndef ITEM_H
#define ITEM_H

// Các mặt hàng gồm: quần áo, giày dép (clothing) và các phụ kiện thời trang (accessory)
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "../Vector/VECTOR.h"

using namespace std;

// Forward declaration
class product;

// Struct lưu kết quả tìm kiếm kèm theo % khớp từ khóa cho sàn TMĐT
struct KetQuaTimKiem {
    product* sp;
    double ti_le_khop; // Tỷ lệ % khớp từ khóa (0.0% -> 100.0%)
};

// =========================================================================
// 1. CÁC HÀM TIỆN ÍCH QUẢN LÝ MÃ SẢN PHẨM (SMART ID: SPCM0001A - 9 KÝ TỰ)
// =========================================================================
// Lấy ký tự kiểu sản phẩm ở cuối mã idsp (ví dụ 'SPCM0001A' -> 'A')
char lay_ma_kieu_idsp(const string &idsp);

// Kiểm tra ký tự kiểu có thuộc nhóm Quần Áo / Giày Dép (Clothing) không
// 'A' (Áo), 'Q' (Quần), 'V' (Váy), 'G' (Giày/Dép), 'S' (Set đồ)
bool la_ma_kieu_quan_ao(char ma_kieu);

// Kiểm tra ký tự kiểu có thuộc nhóm Phụ Kiện (Accessory) không
// 'M' (Mũ), 'D' (Đồng hồ), 'T' (Túi/Ví/Balo), 'K' (Kính), 'X' (Trang sức), 'P' (Phụ kiện khác...)
bool la_ma_kieu_phu_kien(char ma_kieu);

// Kiểm tra mã sản phẩm chuẩn SKU (9 ký tự): SP + HÃNG(2 chữ cái) + STT(4 chữ số) + KIỂU(1 ký tự)
bool kiem_tra_ma_sp(const string &idsp);

// Tra ngược từ ký tự mã kiểu ra tên danh mục (ví dụ 'A' -> "Áo", 'M' -> "Mũ")
// Dùng để tự động điền subtype khi đọc file cũ không có cột subtype
string ma_kieu_sang_ten(char ma_kieu);

// Chuyển đổi tên kiểu tiếng Việt chọn trên giao diện -> ký tự mã kiểu (ví dụ "Áo" -> 'A', "Mũ" -> 'M')
char kieu_sang_ma_kieu(const string &ten_kieu);

// Chuyển đổi tên hãng chọn trên giao diện -> mã viết tắt 2 ký tự (ví dụ "Coolmate" -> "CM", "Nike" -> "NK")
string hang_sang_ma_hang(const string &ten_hang);

// --- BẢNG TRA CỨU DÙNG CHO UI DROPDOWN ---
// subtype của sản phẩm chọn từ DS_KIEU_SP, sau đó ma được dùng để sinh SKU
struct kieu_sp_entry { const char* ten; char ma; };
extern const kieu_sp_entry DS_KIEU_SP[];
extern const int SO_KIEU_SP;

// Hãng sản phẩm chọn từ DS_HANG_SP
struct hang_sp_entry { const char* ten; const char* ma; };
extern const hang_sp_entry DS_HANG_SP[];
extern const int SO_HANG_SP;


// =========================================================================
// 2. VARIANT: MỖI Ô (MÀU + SIZE) CÓ SỐ LƯỢNG TỒN KHO RIÊNG
// =========================================================================
// Ví dụ: Áo thun SPCM0001A có 4 variant: Đen-M:5, Đen-L:0, Trắng-M:12, Trắng-L:3
struct Variant {
    string color;
    string size;   // "M", "L", "42", "16cm"... Hàng không chia size thì ghi "Free"
    int sl;        // số lượng tồn của đúng cặp màu + size này

    Variant() : sl(0) {}
    Variant(const string &c, const string &s, int n) : color(c), size(s), sl(n) {}
};


// =========================================================================
// 3. CLASS PRODUCT (LỚP CHA CỦA SẢN PHẨM)
// =========================================================================
class product {
protected:
    string idsp;     // Định dạng chuẩn 9 ký tự: SP + HÃNG(2) + STT(4) + KIỂU(1), ví dụ: SPCM0001A
    string namesp;
    string brandsp;
    string subtype;  // Danh mục rộng: "Áo", "Quần", "Mũ", "Đồng hồ"...
    string kieusp;   // Mô tả chi tiết: "Áo thun oversize", "Áo sơ mi oxford", "Giày sneaker"...
    long long price; // VNĐ không có số lẻ nên dùng số nguyên, cộng dồn doanh thu không sai số
    VECTOR<Variant> variants; // Các cặp màu + size, mỗi cặp có số lượng riêng
    string name_norm; // namesp đã bỏ dấu + chữ thường, cache sẵn để tìm kiếm nhanh

    void capnhat_name_norm(); // tính lại name_norm (gọi khi namesp đổi)

    // Các "móc" để lớp con khai báo thông tin riêng, dùng chung cho xuat_thong_tin() và ghi_file()
    virtual const char* nhan() const { return "[SP]"; }
    virtual string lay_material() const { return "-"; }
    virtual string lay_dimension() const { return "-"; }

public:
    // Constructors & Destructor (màu / size / số lượng thêm sau bằng them_variant)
    product() : price(0) {}
    product(string idsp, string namesp, string brandsp, string subtype, string kieusp, long long price)
        : idsp(idsp), namesp(namesp), brandsp(brandsp), subtype(subtype), kieusp(kieusp), price(price) { capnhat_name_norm(); }
    product(const product &a)
        : idsp(a.idsp), namesp(a.namesp), brandsp(a.brandsp), subtype(a.subtype), kieusp(a.kieusp), price(a.price),
          variants(a.variants), name_norm(a.name_norm) {}
    virtual ~product() {}

    // --- CÁC HÀM GET (INLINE TRONG HEADER) ---
    const string& getid() const { return idsp; }
    const string& getname() const { return namesp; }
    const string& getbrand() const { return brandsp; }
    const string& getsubtype() const { return subtype; }
    const string& getkieu() const { return kieusp; }
    long long getprice() const { return price; }
    const string& getnamenorm() const { return name_norm; }

    // needle_norm phải được normalize() 1 lần bên ngoài, không normalize lại mỗi sản phẩm
    bool khop_ten(const string &needle_norm) const { return name_norm.find(needle_norm) != string::npos; }

    // --- TÍNH % KHỚP TỪ KHÓA TÌM KIẾM THEO CHUẨN SÀN THƯƠNG MẠI ĐIỆN TỬ ---
    // Trả về từ 0.0 đến 100.0 (%)
    double tinh_ti_le_khop(const string &tu_khoa_goc) const;

    // --- CÁC HÀM SET (KIỂM TRA DỮ LIỆU ĐẦU VÀO) ---
    bool setid(const string &a) {
        if (!kiem_tra_ma_sp(a)) return false;
        idsp = a;
        return true;
    }
    bool setname(const string &a) {
        if (a.empty()) return false;
        namesp = a;
        capnhat_name_norm();
        return true;
    }
    bool setbrand(const string &a) {
        if (a.empty()) return false;
        brandsp = a;
        return true;
    }
    bool setsubtype(const string &a) {
        if (a.empty()) return false;
        subtype = a;
        return true;
    }
    bool setkieu(const string &a) {
        if (a.empty()) return false;
        kieusp = a;
        return true;
    }
    bool setprice(long long a) {
        if (a < 0) return false;
        price = a;
        return true;
    }

    // --- QUẢN LÝ VARIANT (MÀU + SIZE + SỐ LƯỢNG) ---
    // So khớp màu/size không phân biệt hoa thường và dấu ("đen" == "Đen", "m" == "M")
    int  so_variant() const { return variants.size(); }
    const Variant& get_variant(int i) const { return variants[i]; } // i trong [0, so_variant())
    int  tim_variant(const string &color, const string &size) const; // -1 nếu không có

    // Nhập hàng: chưa có ô (màu, size) thì tạo mới, đã có thì CỘNG DỒN số lượng
    bool them_variant(const string &color, const string &size, int sl);
    // Đặt thẳng số lượng của 1 ô (dùng khi sửa kho / kiểm kê)
    bool dat_sl_variant(const string &color, const string &size, int sl);
    bool xoa_variant(const string &color, const string &size);

    int  getsl() const;                                             // Tổng tồn kho mọi màu mọi size
    int  getsl(const string &color, const string &size) const;      // Tồn kho của đúng 1 ô (0 nếu không có)
    // Bán hàng: trừ kho đúng ô; false nếu không có ô đó hoặc không đủ hàng (không trừ gì cả)
    bool tru_kho(const string &color, const string &size, int sl);

    // Phục vụ UI: khách chọn màu trước, rồi hiện các size của màu đó
    VECTOR<string> ds_mau() const;                                  // Các màu không trùng
    VECTOR<string> ds_size(const string &color = "") const;         // Size của 1 màu (rỗng = mọi màu)
    string ds_mau_chuoi() const;                                    // "Đen, Trắng" để hiển thị / tìm kiếm

    // --- CÁC HÀM NGHIỆP VỤ & ĐA HÌNH CỦA TỪNG SẢN PHẨM ---
    virtual bool la_phu_kien() const { return false; }
    string xuat_thong_tin() const;
    // Ghi mỗi variant 1 dòng (cùng idsp). Sản phẩm chưa có variant vẫn ghi 1 dòng "-" để không bị mất
    void ghi_file(ostream &os) const;

    // Phân tích 1 dòng text và khởi tạo đúng đối tượng (clothing hoặc accessory) chứa 1 variant của dòng đó
    static product* doc_tu_chuoi(const string &line);

    // Toán tử so sánh và in
    bool operator==(const product &other) const { return idsp == other.idsp; }
    friend ostream& operator<<(ostream &os, const product &p);
};


// =========================================================================
// 4. CLASS CLOTHING (LỚP CON: QUẦN ÁO, GIÀY DÉP)
// =========================================================================
class clothing : public product {
protected:
    string material;

    const char* nhan() const override { return "[Quần Áo]"; }
    string lay_material() const override { return material.empty() ? "-" : material; }

public:
    clothing() : product() {}
    clothing(string idsp, string namesp, string brandsp, string subtype, string kieusp, long long price, string material)
        : product(idsp, namesp, brandsp, subtype, kieusp, price), material(material) {}
    clothing(const clothing &a)
        : product(a), material(a.material) {}
    virtual ~clothing() {}

    const string& getmaterial() const { return material; }
    bool setmaterial(const string &a) {
        if (a.empty()) return false;
        material = a;
        return true;
    }

    bool la_phu_kien() const override { return false; }
};


// =========================================================================
// 5. CLASS ACCESSORY (LỚP CON: PHỤ KIỆN THỜI TRANG)
// =========================================================================
class accessory : public product {
protected:
    string material;
    string dimension; // Kích thước chung của món: "40mm" (mặt đồng hồ), "10mm" (hạt vòng)...

    const char* nhan() const override { return "[Phụ Kiện]"; }
    string lay_material() const override { return material.empty() ? "-" : material; }
    string lay_dimension() const override { return dimension.empty() ? "-" : dimension; }

public:
    accessory() : product() {}
    accessory(string idsp, string namesp, string brandsp, string subtype, string kieusp, long long price, string material, string dimension)
        : product(idsp, namesp, brandsp, subtype, kieusp, price), material(material), dimension(dimension) {}
    accessory(const accessory &a)
        : product(a), material(a.material), dimension(a.dimension) {}
    virtual ~accessory() {}

    const string& getmaterial() const { return material; }
    const string& getdimension() const { return dimension; }

    bool setmaterial(const string &a) {
        if (a.empty()) return false;
        material = a;
        return true;
    }
    bool setdimension(const string &a) {
        if (a.empty()) return false;
        dimension = a;
        return true;
    }

    bool la_phu_kien() const override { return true; }
};

#endif