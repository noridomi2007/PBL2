#include "Doanhthu.h"
#include "../Cautrucdulieu/Heap.h"
#include "../Tienich/Tienich.h"
#include <fstream>
#include <cstdio>
#include <sstream>
#include <ctime>
#include <filesystem>

// ---------- KỲ THỐNG KÊ ----------
long long lay_khoa_ky(long long thoi_gian, KieuKy k) {
    time_t t = static_cast<time_t>(thoi_gian);
    tm *p = localtime(&t);
    if (!p) return 0;
    tm d = *p;
    long long nam = d.tm_year + 1900;

    switch (k) {
        case KY_NAM:   return nam;
        case KY_QUY:   return nam * 10 + (d.tm_mon / 3 + 1);
        case KY_THANG: return nam * 100 + (d.tm_mon + 1);
        case KY_TUAN: {
            int lui = (d.tm_wday + 6) % 7;   // Thứ 2 -> 0 ... Chủ nhật -> 6
            d.tm_mday -= lui;                // lùi về Thứ 2 (có thể lùi qua tháng/năm)
            d.tm_isdst = -1;
            mktime(&d);                      // mktime tự chuẩn hóa lại ngày/tháng/năm
            return (long long)(d.tm_year + 1900) * 10000 + (d.tm_mon + 1) * 100 + d.tm_mday;
        }
    }
    return 0;
}

long long khoa_ky_hien_tai(KieuKy k) {
    return lay_khoa_ky(static_cast<long long>(time(nullptr)), k);
}

string ten_ky(long long khoa, KieuKy k) {
    switch (k) {
        case KY_NAM:   return "Năm " + to_string(khoa);
        case KY_QUY:   return "Quý " + to_string(khoa % 10) + "/" + to_string(khoa / 10);
        case KY_THANG: return "Tháng " + to_string(khoa % 100) + "/" + to_string(khoa / 100);
        case KY_TUAN:  return "Tuần từ " + to_string(khoa % 100) + "/" +
                              to_string((khoa / 100) % 100) + "/" + to_string(khoa / 10000);
    }
    return "";
}

// ---------- ĐỌC / GHI SỔ ----------
// File dạng bảng thẳng hàng, mỗi dòng hàng 11 cột:
//   | thoi_gian | idsp | ten | color | size | sl | don_gia | ma_hd | ma_kh | ma_nv | giam_gia |
// Dòng cũ dạng a|b|c (7 hoặc 11 cột) vẫn đọc được (các cột thiếu coi như rỗng / 0).
static const int RONG_COT_DT[11] = { RONG_DT_THOI_GIAN, RONG_DT_IDSP, RONG_DT_TEN, RONG_DT_MAU, RONG_DT_SIZE,
                                     RONG_DT_SL, RONG_DT_DON_GIA, RONG_DT_MA_HD, RONG_DT_MA_KH, RONG_DT_MA_NV,
                                     RONG_DT_GIAM_GIA };

static string tieu_de_so() {
    const string cot[11] = { "Time", "idsp", "name", "Color", "Size", "Số lượng", "Đơn giá",
                             "idHĐ", "maKH", "maNV", "giảm giá" };
    return dong_bang(cot, RONG_COT_DT, 11);
}

static string dong_giao_dich(const GiaoDich &g) {
    const string cot[11] = { to_string(g.thoi_gian), g.idsp, g.ten, g.color, g.size, to_string(g.sl),
                             to_string(g.don_gia), g.ma_hd, g.ma_kh, g.ma_nv, to_string(g.giam_gia) };
    return dong_bang(cot, RONG_COT_DT, 11) + "\n";
}

static bool file_trong(const string &duong_dan) {
    ifstream f(duong_dan);
    return !f.is_open() || f.peek() == ifstream::traits_type::eof();
}

// ---------- CHỈ MỤC BẢNG BĂM ----------
void DoanhThu::lap_chi_muc(int vi_tri) {
    const GiaoDich &g = ds[vi_tri];
    da_ban_sp[g.idsp] += g.sl;                   // tính cả dòng cũ chưa có hóa đơn
    long long tt = g.thanh_tien();
    if (!g.ma_kh.empty()) chi_tieu_kh[g.ma_kh] += tt;
    if (!g.ma_nv.empty()) {
        doanh_so_nv[g.ma_nv] += tt;
        for (int kk = 0; kk < 4; kk++)
            doanh_so_nv_ky[g.ma_nv + "|" + to_string(kk) + "|" + to_string(lay_khoa_ky(g.thoi_gian, (KieuKy)kk))] += tt;
    }
    if (g.ma_hd.empty()) return;                 // dòng cũ chưa có hóa đơn: không đưa vào chỉ mục
    VECTOR<int> &cac_dong = chi_muc_hd[g.ma_hd];
    if (cac_dong.empty() && !g.ma_kh.empty()) hd_cua_khach[g.ma_kh].push_front(g.ma_hd);  // lần đầu thấy hóa đơn này: chèn ĐẦU danh sách liên kết
    cac_dong.push_back(vi_tri);
    if (g.ma_hd.size() > 2 && g.ma_hd.compare(0, 2, "HD") == 0) {
        try { so_hd_lon_nhat = max(so_hd_lon_nhat, stoll(g.ma_hd.substr(2))); } catch (...) {}
    }
}

void DoanhThu::xay_lai_chi_muc() {
    chi_muc_hd.clear();
    hd_cua_khach.clear();
    da_ban_sp.clear();     
    chi_tieu_kh.clear(); 
    doanh_so_nv.clear(); 
    doanh_so_nv_ky.clear();
    so_hd_lon_nhat = 0;
    for (int i = 0; i < ds.size(); i++) lap_chi_muc(i);
}

bool DoanhThu::doc_file(const string &duong_dan) {
    file = duong_dan;
    ds.clear();
    xay_lai_chi_muc();

    ifstream fin(duong_dan);
    if (!fin.is_open()) return true;   // chưa bán lần nào -> sổ trống, không phải lỗi

    string line;
    bool dong_dau = true, co_tieu_de = true;
    while (getline(fin, line)) {
        if (cat_khoang_trang(line).empty()) continue;
        string tok[11];
        int n = tach_dong_bang(line, tok, 11);
        bool la_so = !tok[0].empty() && tok[0].find_first_not_of("0123456789") == string::npos;
        if (dong_dau) { co_tieu_de = !la_so; dong_dau = false; }
        if (!la_so) continue;                    // dòng tiêu đề (tên cột nào cũng được)
        if (n < 7) continue;           // dòng hỏng thì bỏ qua
        if (n > 11) n = 11;

        try {
            GiaoDich g;
            g.thoi_gian = stoll(tok[0]);
            g.idsp = tok[1];
            g.ten = tok[2];
            g.color = tok[3];
            g.size = tok[4];
            g.sl = stoi(tok[5]);
            g.don_gia = stoll(tok[6]);
            if (n > 7)  g.ma_hd = tok[7];
            if (n > 8)  g.ma_kh = tok[8];
            if (n > 9)  g.ma_nv = tok[9];
            if (n > 10) g.giam_gia = stoll(tok[10]);
            if (g.sl > 0 && g.don_gia >= 0 && g.giam_gia >= 0 && g.giam_gia <= g.tam_tinh()) ds.push_back(g);
        } catch (...) {}
    }
    fin.close();
    xay_lai_chi_muc();
    if (!co_tieu_de) chuan_hoa_file();   // file cũ chưa thẳng hàng: chuyển sang bảng một lần (không báo lỗi nếu thất bại)
    return true;
}

// Giữ bản sao file cũ (đuôi .truoc_chuan_hoa, chỉ tạo một lần) rồi ghi cả sổ ra file tạm và đổi tên
bool DoanhThu::chuan_hoa_file() const {
    error_code ec;
    string ban_sao = file + ".truoc_chuan_hoa";
    if (!filesystem::exists(ban_sao, ec)) filesystem::copy_file(file, ban_sao, ec);
    string tam = file + ".tmp";
    {
        ofstream fout(tam);
        if (!fout.is_open()) return false;
        fout << tieu_de_so() << '\n';
        for (int i = 0; i < ds.size(); i++) fout << dong_giao_dich(ds[i]);
        fout.close();
        if (fout.fail()) return false;
    }
    filesystem::rename(tam, file, ec);
    return !ec;
}

bool DoanhThu::them(const GiaoDich &g) {
    bool can_tieu_de = file_trong(file);
    ofstream fout(file, ios::app);
    if (!fout.is_open()) return false;
    if (can_tieu_de) fout << tieu_de_so() << '\n';
    fout << dong_giao_dich(g);
    fout.flush();
    if (!fout) return false;
    ds.push_back(g);
    lap_chi_muc(ds.size() - 1);
    return true;
}

bool DoanhThu::them_nhieu(const VECTOR<GiaoDich> &dong) {
    if (dong.empty()) return true;
    string noi_dung;
    for (int i = 0; i < dong.size(); i++) noi_dung += dong_giao_dich(dong[i]);

    bool can_tieu_de = file_trong(file);
    ofstream fout(file, ios::app);
    if (!fout.is_open()) return false;
    if (can_tieu_de) fout << tieu_de_so() << '\n';
    fout << noi_dung;               // ghi MỘT lần cả hóa đơn
    fout.flush();
    if (!fout) return false;
    for (int i = 0; i < dong.size(); i++) { ds.push_back(dong[i]); lap_chi_muc(ds.size() - 1); }
    return true;
}

string DoanhThu::ma_hd_tiep_theo() const {
    char buf[24];
    snprintf(buf, sizeof(buf), "HD%06lld", so_hd_lon_nhat + 1);
    return buf;
}

// ---------- THỐNG KÊ ----------
long long DoanhThu::tong_doanh_thu(KieuKy k, long long khoa) const {
    long long tong = 0;
    for (int i = 0; i < ds.size(); i++) {
        if (lay_khoa_ky(ds[i].thoi_gian, k) == khoa) tong += ds[i].thanh_tien();
    }
    return tong;
}

// Kỳ nào doanh thu cao hơn thì đứng trước; bằng nhau thì kỳ mới hơn đứng trước
static bool ky_hon(const DoanhThuKy &a, const DoanhThuKy &b) {
    if (a.tong != b.tong) return a.tong > b.tong;
    return a.khoa > b.khoa;
}

VECTOR<DoanhThuKy> DoanhThu::top_ky(KieuKy k, int top_n) const {
    // Bước 1: gom doanh thu theo từng kỳ
    VECTOR<DoanhThuKy> gom;
    HASHTABLE<int> vt_cua;                                   // khóa kỳ -> vị trí trong gom
    for (int i = 0; i < ds.size(); i++) {
        long long khoa = lay_khoa_ky(ds[i].thoi_gian, k);
        int *vt = vt_cua.find(to_string(khoa));
        if (!vt) {
            DoanhThuKy m; m.khoa = khoa; m.tong = ds[i].thanh_tien();
            vt_cua.put(to_string(khoa), gom.size());
            gom.push_back(m);
        } else gom[*vt].tong += ds[i].thanh_tien();
    }
    

    // Bước 2: đưa vào heap, lấy top_n phần tử ở gốc
    HEAP<DoanhThuKy> heap(ky_hon);
    for (int i = 0; i < gom.size(); i++) heap.push(gom[i]);

    VECTOR<DoanhThuKy> kq;
    while (!heap.empty() && kq.size() < top_n) kq.push_back(heap.pop());
    return kq;
}

// Doanh thu cao hơn đứng trước; bằng nhau thì bán nhiều cái hơn đứng trước
static bool sp_hon(const DoanhThuSP &a, const DoanhThuSP &b) {
    if (a.tong != b.tong) return a.tong > b.tong;
    return a.so_luong > b.so_luong;
}

VECTOR<DoanhThuSP> DoanhThu::top_san_pham(KieuKy k, long long khoa, int top_n) const {
    VECTOR<DoanhThuSP> gom;
    HASHTABLE<int> vt_cua;                                   // idsp -> vị trí trong gom
    for (int i = 0; i < ds.size(); i++) {
        if (lay_khoa_ky(ds[i].thoi_gian, k) != khoa) continue;
        int *vt = vt_cua.find(ds[i].idsp);
        if (!vt) {
            DoanhThuSP m;
            m.idsp = ds[i].idsp; m.ten = ds[i].ten;
            m.tong = ds[i].thanh_tien(); m.so_luong = ds[i].sl;
            vt_cua.put(ds[i].idsp, gom.size());
            gom.push_back(m);
        } else {
            gom[*vt].tong += ds[i].thanh_tien();
            gom[*vt].so_luong += ds[i].sl;
        }
    }

    HEAP<DoanhThuSP> heap(sp_hon);
    for (int i = 0; i < gom.size(); i++) heap.push(gom[i]);

    VECTOR<DoanhThuSP> kq;
    while (!heap.empty() && kq.size() < top_n) kq.push_back(heap.pop());
    return kq;
}
// ---------- KHÁCH & NHÂN VIÊN ----------
long long DoanhThu::tong_chi_tieu_khach(const string &ma_kh) const {
    const long long *p = chi_tieu_kh.find(ma_kh);
    return p ? *p : 0;
}
long long DoanhThu::doanh_so_nhan_vien(const string &ma_nv) const {
    const long long *p = doanh_so_nv.find(ma_nv);
    return p ? *p : 0;
}
long long DoanhThu::doanh_so_nhan_vien(const string &ma_nv, KieuKy k, long long khoa) const {
    const long long *p = doanh_so_nv_ky.find(ma_nv + "|" + to_string((int)k) + "|" + to_string(khoa));
    return p ? *p : 0;
}

// Doanh số cao hơn đứng trước; bằng nhau thì mã nhỏ hơn đứng trước
static bool nv_hon(const DoanhThuNV &a, const DoanhThuNV &b) {
    if (a.tong != b.tong) return a.tong > b.tong;
    return a.ma_nv < b.ma_nv;
}

VECTOR<DoanhThuNV> DoanhThu::top_nhan_vien(KieuKy k, long long khoa, int top_n) const {
    VECTOR<DoanhThuNV> gom;
    HASHTABLE<int> vt_cua;
    for (int i = 0; i < ds.size(); i++) {
        if (ds[i].ma_nv.empty()) continue;
        if (lay_khoa_ky(ds[i].thoi_gian, k) != khoa) continue;
        int *vt = vt_cua.find(ds[i].ma_nv);
        if (!vt) {
            DoanhThuNV m; m.ma_nv = ds[i].ma_nv; m.tong = ds[i].thanh_tien();
            vt_cua.put(ds[i].ma_nv, gom.size());
            gom.push_back(m);
        } else gom[*vt].tong += ds[i].thanh_tien();
    }
    HEAP<DoanhThuNV> heap(nv_hon);
    for (int i = 0; i < gom.size(); i++) heap.push(gom[i]);

    VECTOR<DoanhThuNV> kq;
    while (!heap.empty() && kq.size() < top_n) kq.push_back(heap.pop());
    return kq;
}

// ---------- NHẬP NGÀY ----------
bool tao_thoi_gian(int ngay, int thang, int nam, long long &ket_qua) {
    if (nam < 2000 || nam > 2100) return false;

    tm t = {};
    t.tm_year = nam - 1900;
    t.tm_mon = thang - 1;
    t.tm_mday = ngay;
    t.tm_hour = 12;
    t.tm_isdst = -1;
    time_t x = mktime(&t);
    if (x == static_cast<time_t>(-1)) return false;

    // mktime tự "sửa" ngày sai (31/2 -> 3/3, tháng 13 -> tháng 1 năm sau) nên so lại để bắt
    if (t.tm_year != nam - 1900 || t.tm_mon != thang - 1 || t.tm_mday != ngay) return false;

    // Không cho nhập ngày sau hôm nay (cùng ngày thì được)
    time_t bay_gio = time(nullptr);
    tm *p = localtime(&bay_gio);
    if (!p) return false;
    tm h = *p;
    h.tm_hour = 12; h.tm_min = 0; h.tm_sec = 0; h.tm_isdst = -1;
    if (x > mktime(&h)) return false;

    ket_qua = static_cast<long long>(x);
    return true;
}