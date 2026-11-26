#include "Hangthanhvien.h"

// ===== GIÁ TRỊ TẠM, CHỜ BẢNG CHÍNH THỨC CỦA BẠN: chỉ cần sửa các dòng dưới đây =====
const HangTV BANG_HANG[] = {
    // tên          ngưỡng       %   giá_min  giá_max
    {"Đồng",              0,      0,   0,   0},
    {"Bạc",         2000000,      5,   0,   0},
    {"Vàng",        5000000,     10,   0,   0},
    {"Kim cương",  10000000,     30,   0,   0},
};
const int SO_HANG_TV = sizeof(BANG_HANG) / sizeof(BANG_HANG[0]);

int chi_so_hang(long long tong) {
    for (int i = SO_HANG_TV - 1; i > 0; i--) {
        if (tong >= BANG_HANG[i].nguong) return i;
    }
    return 0;
}

const HangTV &lay_hang(long long tong) { return BANG_HANG[chi_so_hang(tong)]; }

long long con_thieu_de_len_hang(long long tong) {
    int i = chi_so_hang(tong);
    if (i + 1 >= SO_HANG_TV) return -1;
    return BANG_HANG[i + 1].nguong - tong;
}

long long tinh_giam_gia_dong(const HangTV &h, long long don_gia, int sl) {
    if (h.phan_tram <= 0 || sl <= 0 || don_gia <= 0) return 0;
    if (h.gia_min > 0 && don_gia < h.gia_min) return 0;
    if (h.gia_max > 0 && don_gia > h.gia_max) return 0;
    return don_gia * sl * h.phan_tram / 100;
}
