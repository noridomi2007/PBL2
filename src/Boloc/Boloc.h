#ifndef BOLOC_H
#define BOLOC_H
// BỘ LỌC SẢN PHẨM CHO CỬA HÀNG: gom mọi tiêu chí (từ khóa, hãng, loại, kiểu cụ thể, khoảng giá,
// bộ sưu tập, lượt đã bán, còn hàng) và sắp xếp (liên quan, bán chạy, giá, tên) vào MỘT hàm.
// Phần hãng / loại / kiểu / giá / từ khóa dùng lại Shop::tim_kiem_va_loc; ở đây chỉ thêm 3 tiêu chí mới
// (bộ sưu tập, đã bán, còn hàng) và các kiểu sắp xếp có chiều tăng / giảm.
#include <string>
#include "../Shop/Shop.h"
#include "../Bosuutap/Bosuutap.h"
using namespace std;

enum KieuSapXep {
    SX_LIEN_QUAN,   // có từ khóa: % khớp cao trước; không có: theo mã sản phẩm
    SX_BAN_CHAY,    // đã bán nhiều -> ít
    SX_GIA_TANG,    // giá thấp -> cao
    SX_GIA_GIAM,    // giá cao -> thấp
    SX_TEN_AZ,
    SX_TEN_ZA
};

struct TieuChiLoc {
    string tu_khoa;
    string hang;             // "" = mọi hãng
    string subtype;          // "" = mọi loại (Áo, Quần...)
    string kieu;             // "" = mọi kiểu của loại đang chọn (Áo thun, Quần tây...)
    string bo_suu_tap;       // "" = mọi bộ sưu tập
    long long gia_min = 0;   // 0 = không giới hạn
    long long gia_max = 0;
    int ban_min = 0;         // chỉ lấy sản phẩm đã bán >= số này (0 = không lọc)
    bool chi_con_hang = false;
    KieuSapXep sap_xep = SX_LIEN_QUAN;

    // Có bộ lọc nào đang bật không (không tính từ khóa và kiểu sắp xếp)
    bool co_loc() const {
        return !hang.empty() || !subtype.empty() || !kieu.empty() || !bo_suu_tap.empty() ||
               gia_min > 0 || gia_max > 0 || ban_min > 0 || chi_con_hang;
    }
};

struct KetQuaLoc {
    product *sp = nullptr;
    double khop = 100.0;     // % khớp từ khóa
    int da_ban = 0;
    int ton = 0;             // tổng tồn kho
};

VECTOR<KetQuaLoc> loc_cua_hang(const Shop &shop, const BoSuuTap &bst, const TieuChiLoc &tc);

#endif
