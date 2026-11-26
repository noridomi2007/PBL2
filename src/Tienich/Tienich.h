#ifndef TIENICH_H
#define TIENICH_H

// Tiện ích dùng chung cho mọi module. Chỉ dùng thư viện chuẩn (KHÔNG cần raylib).
#include <string>
#include <cstdio>
#include <ctime>
#include <chrono>
#include <random>
using namespace std;

// '|' và xuống dòng làm hỏng cấu trúc file txt (mỗi dòng 1 bản ghi, cột cách nhau bởi '|')
inline bool co_ky_tu_cam(const string &s) {
    return s.find_first_of("|\r\n") != string::npos;
}

// ======================== MẬT KHẨU ========================
// Mật khẩu KHÔNG lưu nguyên văn. Trong file lưu 1 chuỗi duy nhất dạng  "muoi$bam"  (8 + 1 + 16 ký tự):
//   muoi = 8 ký tự hex ngẫu nhiên sinh lúc đặt mật khẩu; bam = hàm băm djb2 (xem bam_chuoi) của (muoi + mật khẩu).
// Chuỗi này nằm ngay trong cột "matKhau" của file, không cần file hay bảng riêng.
// LƯU Ý: đây chỉ để người mở file txt không đọc thẳng được mật khẩu, KHÔNG phải bảo mật thực sự.
const int MAT_KHAU_TOI_THIEU = 6;

// HÀM BĂM djb2 (Daniel J. Bernstein): bắt đầu từ 5381, với mỗi ký tự c thì h = h * 33 + c.
// Số 64 bit tự "tràn" vòng lại nên không cần lấy dư. Cùng chuỗi vào -> luôn ra cùng số; không suy ngược lại được chuỗi.
inline unsigned long long bam_chuoi(const string &s) {
    unsigned long long h = 5381;
    for (unsigned char c : s) h = h * 33 + c;
    return h;
}

inline string so_sang_hex(unsigned long long v, int so_chu_so) {
    const char *hex = "0123456789abcdef";
    string r(so_chu_so, '0');
    for (int i = so_chu_so - 1; i >= 0; i--) { r[i] = hex[v & 0xF]; v >>= 4; }
    return r;
}

inline bool la_mat_khau_da_ma_hoa(const string &s) {
    if (s.size() != 25 || s[8] != '$') return false;
    for (size_t i = 0; i < s.size(); i++) {
        if (i == 8) continue;
        char c = s[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return false;
    }
    return true;
}

inline string ma_hoa_mat_khau(const string &mat_khau) {
    static mt19937_64 rng(random_device{}() ^
        static_cast<unsigned long long>(chrono::high_resolution_clock::now().time_since_epoch().count()));
    string muoi = so_sang_hex(rng(), 8);
    return muoi + "$" + so_sang_hex(bam_chuoi(muoi + mat_khau), 16);
}

// da_luu có thể là chuỗi đã mã hóa, hoặc mật khẩu thô của file cũ (cho phép đăng nhập được, rồi đổi mật khẩu để mã hóa)
inline bool kiem_tra_mat_khau(const string &nhap, const string &da_luu) {
    if (!la_mat_khau_da_ma_hoa(da_luu)) return !da_luu.empty() && nhap == da_luu;
    string muoi = da_luu.substr(0, 8);
    return so_sang_hex(bam_chuoi(muoi + nhap), 16) == da_luu.substr(9);
}

// ======================== FILE DẠNG BẢNG (CỘT THẲNG HÀNG) ========================
// Mọi file txt của dự án ghi dạng bảng:   | cột1 | cột2 | ... |   mỗi cột có độ rộng cố định (hằng RONG_... ở từng module).
// Khi ĐỌC thì bỏ khoảng trắng đầu/cuối từng ô, nên đọc được cả file cũ không canh cột (a|b|c).
// Độ rộng tính theo KÝ TỰ chứ không theo byte, vì chữ có dấu chiếm 2-3 byte.
inline int do_dai_utf8(const string &s) {
    int n = 0;
    for (unsigned char c : s) if ((c & 0xC0) != 0x80) n++;
    return n;
}

inline string cat_khoang_trang(const string &s) {
    size_t b = 0, e = s.size();
    while (b < e && (s[b] == ' ' || s[b] == '\t' || s[b] == '\r' || s[b] == '\n')) b++;
    while (e > b && (s[e - 1] == ' ' || s[e - 1] == '\t' || s[e - 1] == '\r' || s[e - 1] == '\n')) e--;
    return s.substr(b, e - b);
}

// Thêm dấu cách cho đủ rộng (chuỗi dài hơn thì để nguyên, không cắt)
inline string can_trai(const string &s, int rong) {
    int d = do_dai_utf8(s);
    return d >= rong ? s : s + string(rong - d, ' ');
}
inline string can_phai(const string &s, int rong) {
    int d = do_dai_utf8(s);
    return d >= rong ? s : string(rong - d, ' ') + s;
}

// Ghép các ô thành 1 dòng bảng:  | a    | b  | c      |
inline string dong_bang(const string *cot, const int *rong, int so_cot) {
    string r = "|";
    for (int i = 0; i < so_cot; i++) r += " " + can_trai(cot[i], rong[i]) + " |";
    return r;
}

// Tách 1 dòng thành các ô (đã bỏ khoảng trắng). Trả về SỐ Ô tìm thấy; chỉ lưu tối đa toi_da ô vào tok.
// Chấp nhận cả dạng "| a | b |" lẫn dạng cũ "a|b|c". Ô rỗng ở giữa vẫn được giữ lại.
inline int tach_dong_bang(const string &dong, string *tok, int toi_da) {
    string t = cat_khoang_trang(dong);
    if (!t.empty() && t.front() == '|') t.erase(0, 1);
    if (!t.empty() && t.back() == '|') t.pop_back();
    int n = 0;
    size_t bat_dau = 0;
    while (true) {
        size_t vt = t.find('|', bat_dau);
        string o = cat_khoang_trang(t.substr(bat_dau, vt == string::npos ? string::npos : vt - bat_dau));
        if (n < toi_da) tok[n] = o;
        n++;
        if (vt == string::npos) break;
        bat_dau = vt + 1;
    }
    return n;
}

// ======================== SỐ ĐIỆN THOẠI ========================
// Chấp nhận đúng 10 chữ số, bắt đầu bằng 0 (vd 0912345678)
inline bool sdt_hop_le(const string &s) {
    if (s.size() != 10 || s[0] != '0') return false;
    for (char c : s) if (c < '0' || c > '9') return false;
    return true;
}

// ======================== TIỀN & NGÀY ========================
// 1234567 -> "1.234.567"
inline string dinh_dang_tien(long long v) {
    bool am = v < 0;
    string s = to_string(am ? -v : v), r;
    int dem = 0;
    for (int i = (int)s.size() - 1; i >= 0; i--) {
        r.insert(r.begin(), s[i]);
        if (++dem % 3 == 0 && i > 0) r.insert(r.begin(), '.');
    }
    return (am ? "-" : "") + r;
}

inline void ngay_hom_nay(int &d, int &m, int &y) {
    time_t t = time(nullptr);
    tm *p = localtime(&t);
    d = p->tm_mday; m = p->tm_mon + 1; y = p->tm_year + 1900;
}

// timestamp -> "dd/mm/yyyy"
inline string dinh_dang_ngay(long long ts) {
    time_t t = static_cast<time_t>(ts);
    tm *p = localtime(&t);
    if (!p) return "?";
    char buf[40];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", p->tm_mday, p->tm_mon + 1, p->tm_year + 1900);
    return buf;
}

#endif
