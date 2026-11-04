#include "doanhthu.h"
#include "../Vectorandheap/HEAP.h"
#include <fstream>
#include <sstream>
#include <ctime>

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
// Mỗi dòng: thoi_gian|idsp|ten|color|size|sl|don_gia
bool DoanhThu::doc_file(const string &duong_dan) {
    file = duong_dan;
    ds.clear();

    ifstream fin(duong_dan);
    if (!fin.is_open()) return true;   // chưa bán lần nào -> sổ trống, không phải lỗi

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string tok[7];
        int n = 0;
        while (n < 7 && getline(ss, tok[n], '|')) n++;
        if (n < 7) continue;           // dòng hỏng thì bỏ qua

        try {
            GiaoDich g;
            g.thoi_gian = stoll(tok[0]);
            g.idsp = tok[1];
            g.ten = tok[2];
            g.color = tok[3];
            g.size = tok[4];
            g.sl = stoi(tok[5]);
            g.don_gia = stoll(tok[6]);
            if (g.sl > 0 && g.don_gia >= 0) ds.push_back(g);
        } catch (...) {}
    }
    return true;
}

bool DoanhThu::them(const GiaoDich &g) {
    ofstream fout(file, ios::app);
    if (!fout.is_open()) return false;
    fout << g.thoi_gian << '|' << g.idsp << '|' << g.ten << '|' << g.color << '|'
         << g.size << '|' << g.sl << '|' << g.don_gia << '\n';
    fout.flush();
    if (!fout) return false;
    ds.push_back(g);
    return true;
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
    for (int i = 0; i < ds.size(); i++) {
        long long khoa = lay_khoa_ky(ds[i].thoi_gian, k);
        int vt = -1;
        for (int j = 0; j < gom.size(); j++) {
            if (gom[j].khoa == khoa) { vt = j; break; }
        }
        if (vt == -1) {
            DoanhThuKy m; m.khoa = khoa; m.tong = ds[i].thanh_tien();
            gom.push_back(m);
        } else {
            gom[vt].tong += ds[i].thanh_tien();
        }
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
    for (int i = 0; i < ds.size(); i++) {
        if (lay_khoa_ky(ds[i].thoi_gian, k) != khoa) continue;   // chỉ lấy giao dịch trong kỳ này

        int vt = -1;
        for (int j = 0; j < gom.size(); j++) {
            if (gom[j].idsp == ds[i].idsp) { vt = j; break; }
        }
        if (vt == -1) {
            DoanhThuSP m;
            m.idsp = ds[i].idsp; m.ten = ds[i].ten;
            m.tong = ds[i].thanh_tien(); m.so_luong = ds[i].sl;
            gom.push_back(m);
        } else {
            gom[vt].tong += ds[i].thanh_tien();
            gom[vt].so_luong += ds[i].sl;
        }
    }

    HEAP<DoanhThuSP> heap(sp_hon);
    for (int i = 0; i < gom.size(); i++) heap.push(gom[i]);

    VECTOR<DoanhThuSP> kq;
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