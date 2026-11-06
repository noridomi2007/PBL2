#ifndef HANGTV_H
#define HANGTV_H

// BẢNG HẠNG THÀNH VIÊN: hạng suy ra từ TỔNG CHI TIÊU (tính từ sổ doanh thu, sau giảm giá).
// Muốn đổi hạng / mức giảm: chỉ sửa bảng BANG_HANG trong hangtv.cpp, không phải sửa chỗ nào khác.
struct HangTV {
    const char *ten;        // "Đồng", "Bạc", "Vàng", "Kim cương"
    long long nguong;       // tổng chi tiêu tối thiểu để đạt hạng này (VNĐ)
    int phan_tram;          // % giảm giá
    long long gia_min;      // chỉ giảm cho món có đơn giá >= gia_min (0 = không giới hạn dưới)
    long long gia_max;      // chỉ giảm cho món có đơn giá <= gia_max (0 = không giới hạn trên)
};

extern const HangTV BANG_HANG[];   // xếp theo nguong TĂNG DẦN, phần tử đầu có nguong = 0
extern const int SO_HANG_TV;

int           chi_so_hang(long long tong_chi_tieu);               // chỉ số hạng trong BANG_HANG
const HangTV& lay_hang(long long tong_chi_tieu);
// Còn thiếu bao nhiêu tiền để lên hạng kế tiếp; -1 nếu đã là hạng cao nhất
long long     con_thieu_de_len_hang(long long tong_chi_tieu);
// Số tiền giảm cho 1 dòng hàng (đơn giá x số lượng), làm tròn xuống đồng
long long     tinh_giam_gia_dong(const HangTV &h, long long don_gia, int sl);

#endif
