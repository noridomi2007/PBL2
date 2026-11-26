#include "Shop.h"
#include "../Tienich/Tiengviet.h"
#include "../Tienich/Tienich.h"
#include "../Customer/Hangthanhvien.h"
#include <fstream>
#include <filesystem>
#include <ctime> 
#include <iostream>
#include <iomanip>
#include <cctype>

// =========================================================================
// HÀM TIỆN ÍCH NỘI BỘ
// =========================================================================
static char to_upper_char(char c) {
    return static_cast<char>(toupper(static_cast<unsigned char>(c)));
}

static string to_upper_str(string s) {
    for (char &c : s) c = to_upper_char(c);
    return s;
}

// Kiểm tra 2 chuỗi có khớp nhau không (bỏ qua dấu tiếng Việt và chữ hoa/thường)
static bool khop_chuoi(const string& s1, const string& s2) {
    if (s1.empty() || s2.empty()) return false;
    return normalize(s1.c_str()) == normalize(s2.c_str());
}

// Kiểm tra danh sách string đã chứa phần tử chưa
static bool da_ton_tai(const VECTOR<string>& list, const string& item) {
    for (int i = 0; i < list.size(); i++) {
        if (khop_chuoi(list[i], item)) return true;
    }
    return false;
}

// co_ky_tu_cam() nằm trong Tienich/Tienich.h (dùng chung với customer, staff)

// =========================================================================
// CONSTRUCTOR & DESTRUCTOR
// =========================================================================

Shop::Shop() {}

Shop::~Shop() {
    clear();
}


// =========================================================================
// CÁC CHỨC NĂNG NGHIỆP VỤ CƠ BẢN
// =========================================================================

// Thêm sản phẩm mới vào danh sách (kiểm tra con trỏ, định dạng mã và chặn trùng ID)
bool Shop::add(product* p) {
    if (p == nullptr) return false;

    // 1. Kiểm tra định dạng mã sản phẩm có đúng chuẩn SKU không
    if (!kiem_tra_ma_sp(p->getid())) {
        cerr << "[Cảnh báo] Mã sản phẩm không đúng định dạng SKU (SP + 2 chữ cái + 4 số + 1 kiểu): " << p->getid() << endl;
        return false;
    }

    // 2. Chặn trùng mã sản phẩm đã tồn tại trong kho
    if (find(p->getid()) != -1) {
        cerr << "[Cảnh báo] Mã sản phẩm đã tồn tại trong kho: " << p->getid() << endl;
        return false;
    }

    ds.push_back(p);
    return true;
}

// Xóa sản phẩm theo idsp và giải phóng bộ nhớ an toàn (tránh memory leak)
bool Shop::remove(const string& id) {
    int vi_tri = find(id);
    if (vi_tri == -1) return false;

    string ma = ds[vi_tri]->getid();   // copy trước khi delete (id có thể đang tham chiếu vào chính sản phẩm này)
    delete ds[vi_tri];
    ds.remove(vi_tri);
    xoa_tat_ca_anh(ma);                // xóa hết ảnh _1, _2, _3...
    return true;
}

bool Shop::xoa_san_pham(const string& id) {
    int vi_tri = find(id);
    if (vi_tri == -1) return false;

    product* p = ds[vi_tri];
    string ma = p->getid();
    ds.remove(vi_tri);                       // gỡ khỏi danh sách nhưng CHƯA delete
    if (!ghi_file(file_hien_tai)) {
        ds.push_back(p);                     // ghi lỗi -> trả lại (vị trí trong danh sách có thể đổi, dữ liệu vẫn còn)
        return false;
    }
    delete p;
    xoa_tat_ca_anh(ma);
    return true;
}

// Tìm vị trí của sản phẩm trong danh sách theo mã idsp (trả về -1 nếu không có)
int Shop::find(const string& id) const {
    string id_check = to_upper_str(id);
    for (int i = 0; i < ds.size(); i++) {
        if (ds[i] != nullptr && to_upper_str(ds[i]->getid()) == id_check) {
            return i;
        }
    }
    return -1;
}

// Lấy tổng số lượng sản phẩm đang quản lý
int Shop::size() const {
    return ds.size();
}

// Lấy con trỏ sản phẩm tại vị trí thứ i (có kiểm tra chỉ số hợp lệ)
product* Shop::take(int i) const {
    if (i < 0 || i >= ds.size()) {
        return nullptr;
    }
    return ds[i];
}


// Thanh toán cả giỏ: được tất cả hoặc không gì (xem mô tả trong Shop.h)
KetQuaThanhToan Shop::thanh_toan(const GioHang& gio, const string& ma_kh, const string& ma_nv,
                                 int ngay, int thang, int nam, HoaDon& ra, int* dong_loi) {
    if (dong_loi) *dong_loi = -1;
    if (gio.empty()) return TT_GIO_TRONG;
    if (ma_kh.empty()) return TT_THIEU_KHACH;
    long long thoi_gian;
    if (!tao_thoi_gian(ngay, thang, nam, thoi_gian)) return TT_NGAY_SAI;

    // ---- Bước 1: kiểm tra mọi dòng, gom nhu cầu theo từng ô (sản phẩm, màu, size) ----
    // Hai dòng giỏ khác cách gõ ("Đen" / "den") nhưng cùng 1 ô vẫn được cộng chung để không bán lố kho
    struct Cho { int sp; int vr; int can; };
    VECTOR<Cho> cho;
    VECTOR<int> cho_cua_dong;
    for (int i = 0; i < gio.so_dong(); i++) {
        const DongGio& d = gio.lay(i);
        if (d.sl <= 0) { if (dong_loi) *dong_loi = i; return TT_SO_LUONG_SAI; }
        int sp = find(d.idsp);
        if (sp == -1) { if (dong_loi) *dong_loi = i; return TT_KHONG_CO_SP; }
        int vr = ds[sp]->tim_variant(d.color, d.size);
        if (vr == -1) { if (dong_loi) *dong_loi = i; return TT_KHONG_CO_BIEN_THE; }

        int k = -1;
        for (int j = 0; j < cho.size(); j++) if (cho[j].sp == sp && cho[j].vr == vr) { k = j; break; }
        if (k == -1) { Cho c; c.sp = sp; c.vr = vr; c.can = d.sl; cho.push_back(c); k = cho.size() - 1; }
        else cho[k].can += d.sl;
        cho_cua_dong.push_back(k);
    }
    for (int k = 0; k < cho.size(); k++) {
        if (ds[cho[k].sp]->get_variant(cho[k].vr).sl < cho[k].can) {
            if (dong_loi) { for (int i = 0; i < cho_cua_dong.size(); i++) if (cho_cua_dong[i] == k) { *dong_loi = i; break; } }
            return TT_THIEU_HANG;
        }
    }

    // ---- Bước 2: dựng các dòng hóa đơn với giảm giá theo hạng (hạng tính trên số đã chi TRƯỚC đơn này) ----
    const HangTV& hang = lay_hang(doanh_thu.tong_chi_tieu_khach(ma_kh));
    string ma_hd = doanh_thu.ma_hd_tiep_theo();
    VECTOR<GiaoDich> dong;
    for (int i = 0; i < gio.so_dong(); i++) {
        const DongGio& d = gio.lay(i);
        product* p = ds[cho[cho_cua_dong[i]].sp];
        const Variant& v = p->get_variant(cho[cho_cua_dong[i]].vr);
        GiaoDich g;
        g.thoi_gian = thoi_gian;
        g.idsp = p->getid();
        g.ten = p->getname();
        g.color = v.color;
        g.size = v.size;
        g.sl = d.sl;
        g.don_gia = p->getprice();
        g.ma_hd = ma_hd;
        g.ma_kh = ma_kh;
        g.ma_nv = ma_nv;
        g.giam_gia = tinh_giam_gia_dong(hang, g.don_gia, g.sl);
        dong.push_back(g);
    }

    // ---- Bước 3: trừ kho (nhớ lại để hoàn nếu có lỗi), ghi item.txt ----
    for (int k = 0; k < cho.size(); k++) {
        product* p = ds[cho[k].sp];
        string mau = p->get_variant(cho[k].vr).color, size = p->get_variant(cho[k].vr).size;
        p->tru_kho(mau, size, cho[k].can);
    }
    auto hoan_kho = [&]() {
        for (int k = 0; k < cho.size(); k++) {
            product* p = ds[cho[k].sp];
            string mau = p->get_variant(cho[k].vr).color, size = p->get_variant(cho[k].vr).size;
            p->them_variant(mau, size, cho[k].can);
        }
    };
    if (!ghi_file(file_hien_tai)) {
        hoan_kho();
        return TT_LOI_GHI_FILE;
    }

    // ---- Bước 4: ghi cả hóa đơn vào sổ doanh thu. Đây là "điểm chốt": ghi xong là đơn đã thành công ----
    if (!doanh_thu.them_nhieu(dong)) {
        hoan_kho();
        ghi_file(file_hien_tai);
        return TT_LOI_GHI_FILE;
    }

    ra = HoaDon();
    ra.ma_hd = ma_hd;
    ra.ma_kh = ma_kh;
    ra.ma_nv = ma_nv;
    ra.thoi_gian = thoi_gian;
    ra.dong = dong;
    return TT_OK;
}

    static bool qua_dai(const string &s, int toi_da) {
        return len_utf8(s.c_str()) > toi_da;   // đếm ký tự, không đếm byte
}
KetQuaThem Shop::them_quan_ao(const string &ten_hang, const string &ten_kieu,
                              const string &ten_sp, const string &kieu_chi_tiet,
                              long long gia, const string &chat_lieu,
                              const VECTOR<Variant> &bien_the,
                              const VECTOR<string> &ds_anh_nguon, string *ma_moi) {
    if (ten_hang.empty() || ten_sp.empty() || gia < 0 || bien_the.empty())
        return THEM_THIEU_THONG_TIN;
    if (!la_ma_kieu_quan_ao(kieu_sang_ma_kieu(ten_kieu)))
        return THEM_SAI_KIEU;

    if (co_ky_tu_cam(ten_hang) || co_ky_tu_cam(ten_sp) ||
        co_ky_tu_cam(kieu_chi_tiet) || co_ky_tu_cam(chat_lieu))
        return THEM_KY_TU_CAM;
    if (qua_dai(ten_sp, RONG_TEN) || qua_dai(ten_hang, RONG_HANG) ||
        qua_dai(kieu_chi_tiet, RONG_KIEU) || qua_dai(chat_lieu, RONG_CHAT_LIEU) ||
        gia > 99999999999LL)
        return THEM_QUA_DAI;
    for (int i = 0; i < bien_the.size(); i++) {
        if (qua_dai(bien_the[i].color, RONG_MAU) || qua_dai(bien_the[i].size, RONG_SIZE) ||
            bien_the[i].sl > 99999)
            return THEM_QUA_DAI;
    }
    for (int i = 0; i < bien_the.size(); i++) {
        if (co_ky_tu_cam(bien_the[i].color) || co_ky_tu_cam(bien_the[i].size))
            return THEM_KY_TU_CAM;
    }

    string id = sinh_ma_sp(ten_hang, ten_kieu);   // vd SPCM0003A
    if (id.empty() || find(id) != -1) return THEM_TRUNG_MA;

    if (!luu_nhieu_anh(id, ds_anh_nguon)) return THEM_LOI_ANH;

    clothing *c = new clothing(id, ten_sp, ten_hang, ten_kieu, kieu_chi_tiet, gia, chat_lieu);
    for (int i = 0; i < bien_the.size(); i++) {
        c->them_variant(bien_the[i].color, bien_the[i].size, bien_the[i].sl);
    }
    if (c->so_variant() == 0) {
        delete c;
        xoa_tat_ca_anh(id);
        return THEM_THIEU_THONG_TIN;
    }

    if (!add(c)) {
        delete c;
        xoa_tat_ca_anh(id);
        return THEM_TRUNG_MA;
    }

    if (!ghi_file(file_hien_tai)) {
        remove(id);            // remove() xóa luôn ảnh
        return THEM_LOI_GHI_FILE;
    }

    if (ma_moi) *ma_moi = id;
    return THEM_OK;
}

// =========================================================================
// QUẢN LÝ KHO (admin): nhập hàng, kiểm kê, sửa giá
// =========================================================================
static const int KHO_TOI_DA_MOI_O = 99999;   // bằng số chữ số của cột sl (RONG_SL = 5)

KetQuaKho Shop::nhap_kho(const string& id, const string& mau, const string& size, int sl) {
    int vt = find(id);
    if (vt == -1) return KHO_KHONG_CO_SP;
    string m = cat_khoang_trang(mau), sz = cat_khoang_trang(size);
    if (m.empty() || sz.empty()) return KHO_THIEU_THONG_TIN;
    if (co_ky_tu_cam(m) || co_ky_tu_cam(sz)) return KHO_KY_TU_CAM;
    if (len_utf8(m.c_str()) > RONG_MAU || len_utf8(sz.c_str()) > RONG_SIZE) return KHO_QUA_DAI;
    if (sl <= 0) return KHO_SAI_SO_LUONG;

    product* p = ds[vt];
    bool la_moi = (p->tim_variant(m, sz) == -1);
    int cu = p->getsl(m, sz);
    if ((long long)cu + sl > KHO_TOI_DA_MOI_O) return KHO_SAI_SO_LUONG;

    if (!p->them_variant(m, sz, sl)) return KHO_SAI_SO_LUONG;
    if (!ghi_file(file_hien_tai)) {                       // lỗi ghi -> trả về như cũ
        if (la_moi) p->xoa_variant(m, sz); else p->dat_sl_variant(m, sz, cu);
        return KHO_LOI_GHI_FILE;
    }
    return KHO_OK;
}

KetQuaKho Shop::kiem_ke(const string& id, const string& mau, const string& size, int sl) {
    int vt = find(id);
    if (vt == -1) return KHO_KHONG_CO_SP;
    if (sl < 0 || sl > KHO_TOI_DA_MOI_O) return KHO_SAI_SO_LUONG;
    product* p = ds[vt];
    if (p->tim_variant(mau, size) == -1) return KHO_KHONG_CO_BIEN_THE;
    int cu = p->getsl(mau, size);
    p->dat_sl_variant(mau, size, sl);
    if (!ghi_file(file_hien_tai)) { p->dat_sl_variant(mau, size, cu); return KHO_LOI_GHI_FILE; }
    return KHO_OK;
}

KetQuaKho Shop::dat_gia(const string& id, long long gia) {
    int vt = find(id);
    if (vt == -1) return KHO_KHONG_CO_SP;
    if (gia < 0 || gia > 99999999999LL) return KHO_SAI_SO_LUONG;
    product* p = ds[vt];
    long long cu = p->getprice();
    p->setprice(gia);
    if (!ghi_file(file_hien_tai)) { p->setprice(cu); return KHO_LOI_GHI_FILE; }
    return KHO_OK;
}

// =========================================================================
// QUẢN LÝ BỘ NHỚ VÀ ĐỌC / GHI FILE
// =========================================================================

// Giải phóng toàn bộ bộ nhớ con trỏ product* và làm rỗng vector
void Shop::clear() {
    for (int i = 0; i < ds.size(); i++) {
        delete ds[i];
    }
    ds.clear();
}

// Đọc toàn bộ danh sách sản phẩm từ file item.txt
bool Shop::doc_file(const string& duong_dan) {
    doanh_thu.doc_file();   // sổ doanh thu độc lập với item.txt nên luôn nạp
    ifstream fin(duong_dan);
    if (!fin.is_open()) {
        cerr << "[Lỗi] Không thể mở file: " << duong_dan << endl;
        return false;
    }

    // Mở file thành công -> dọn dẹp sạch danh sách cũ để nạp mới
    clear();

    string line;
    while (getline(fin, line)) {
        product* p = product::doc_tu_chuoi(line);
        if (p == nullptr) continue;

        // Mỗi dòng file là 1 variant. Mã đã có trong kho -> chuyển variant của dòng này sang sản phẩm đó
        int vi_tri = find(p->getid());
        if (vi_tri != -1) {
            product* da_co = ds[vi_tri];
            for (int i = 0; i < p->so_variant(); i++) {
                const Variant& v = p->get_variant(i);
                da_co->them_variant(v.color, v.size, v.sl);
            }
            delete p;
            continue;
        }

        // Mã mới: đi qua add() để kiểm tra mã SKU; dòng lỗi bị bỏ qua, tránh rò rỉ bộ nhớ
        if (!add(p)) delete p;
    }

    fin.close();
    file_hien_tai = duong_dan; // nhớ file này để thanh_toan() ghi lại đúng chỗ
    return true;
}

// Ghi toàn bộ danh sách sản phẩm ra file theo định dạng bảng chuẩn thẳng hàng
// Ghi toàn bộ danh sách sản phẩm ra file theo định dạng bảng chuẩn thẳng hàng
// Ghi ra file tạm trước, thành công hết mới đổi tên đè lên file thật
// -> lỗi giữa chừng (đầy ổ, tắt máy...) thì file cũ vẫn còn nguyên
bool Shop::ghi_file(const string& duong_dan) const {
    string tam = duong_dan + ".tmp";

    {   // dấu { } để fout tự đóng ở cuối khối, trước khi đổi tên file
        ofstream fout(tam);
        if (!fout.is_open()) {
            cerr << "[Lỗi] Không thể mở file để ghi: " << tam << endl;
            return false;
        }

        // Ghi dòng tiêu đề (Header) căn chuẩn xác độ rộng từng cột
        fout << "| " << setw(RONG_ID) << left << "idsp"
             << " | " << setw(RONG_TEN) << left << "namesp"
             << " | " << setw(RONG_HANG) << left << "brandsp"
             << " | " << setw(RONG_KIEU) << left << "Kieusp"
             << " | " << setw(RONG_MAU) << left << "color"
             << " | " << setw(RONG_GIA) << left << "gia"
             << " | " << setw(RONG_SL) << left << "sl"
             << " | " << setw(RONG_SIZE) << left << "size"
             << " | " << setw(RONG_CHAT_LIEU) << left << "material"
             << " | " << setw(RONG_KICH_THUOC) << left << "dimension"
             << " | " << setw(RONG_SUBTYPE) << left << "subtype"
             << " |\n";

        // Ghi từng sản phẩm
        for (int i = 0; i < ds.size(); i++) {
            if (ds[i] != nullptr) {
                ds[i]->ghi_file(fout);
            }
        }

        fout.close();
        if (fout.fail()) {
            cerr << "[Lỗi] Ghi file tạm thất bại: " << tam << endl;
            return false;
        }
    }

    error_code ec;
    filesystem::rename(tam, duong_dan, ec);   // thay file thật bằng file tạm
    if (ec) {
        cerr << "[Lỗi] Không thể thay file: " << duong_dan << endl;
        return false;
    }
    return true;
}
// Tự động sinh mã sản phẩm tiếp theo theo chuẩn Smart ID: SP + HÃNG(2) + STT(4) + KIỂU(1)
// Ví dụ: Đã có SPCM0001A -> Trả về SPCM0002A
string Shop::sinh_ma_sp(const string& ma_hang, char ma_kieu) const {
    if (ma_hang.length() != 2) return "";   // mã hãng phải đúng 2 ký tự

    string tien_to = "SP" + to_upper_str(ma_hang);   // Ví dụ: "SPCM"
    char kieu_hoa = to_upper_char(ma_kieu);

    int stt_max = 0;
    for (int i = 0; i < ds.size(); i++) {
        if (ds[i] == nullptr) continue;
        const string &id = ds[i]->getid();

        // Đúng mẫu SP + 2 chữ hãng + 4 số + 1 kiểu (9 ký tự), cùng hãng và cùng kiểu
        if (id.length() == 9 && id.compare(0, 4, tien_to) == 0 && to_upper_char(id[8]) == kieu_hoa) {
            try {
                int stt = stoi(id.substr(4, 4));
                if (stt > stt_max) stt_max = stt;
            } catch (...) {}
        }
    }

    if (stt_max >= 9999) return "";   // hết mã cho cặp hãng/kiểu này

    stringstream ss;
    ss << tien_to << setw(4) << setfill('0') << (stt_max + 1) << kieu_hoa;
    return ss.str();
}

// Tự động sinh mã từ tên Hãng và Kiểu tiếng Việt chọn trên giao diện UI
string Shop::sinh_ma_sp(const string& ten_hang, const string& ten_kieu) const {
    string ma_h = hang_sang_ma_hang(ten_hang);
    char ma_k = kieu_sang_ma_kieu(ten_kieu);
    return sinh_ma_sp(ma_h, ma_k);
}


// =========================================================================
// TÍNH NĂNG THƯƠNG MẠI ĐIỆN TỬ: LỌC PHÂN CẤP & TÌM KIẾM THEO % KHỚP
// =========================================================================

// 1. Lấy danh sách tất cả các Hãng hiện có trong kho (không trùng)
VECTOR<string> Shop::lay_ds_hang() const {
    VECTOR<string> res;
    for (int i = 0; i < ds.size(); i++) {
        if (ds[i] == nullptr) continue;
        string brand = ds[i]->getbrand();
        if (!brand.empty() && !da_ton_tai(res, brand)) {
            res.push_back(brand);
        }
    }
    return res;
}

// 2. Lấy danh sách các danh mục cha (Áo, Quần, Giày, Mũ...) hiện có
// Nếu truyền 'hang' thì chỉ lấy danh mục cha của riêng hãng đó
VECTOR<string> Shop::lay_ds_subtype(const string& hang) const {
    VECTOR<string> res;
    for (int i = 0; i < ds.size(); i++) {
        if (ds[i] == nullptr) continue;

        // Nếu có chỉ định hãng thì phải khớp hãng
        if (!hang.empty()) {
            string ma_h = hang_sang_ma_hang(hang);
            bool khop_ten_hang = khop_chuoi(ds[i]->getbrand(), hang);
            bool khop_ma_hang = (ds[i]->getid().length() >= 4 && ds[i]->getid().substr(2, 2) == to_upper_str(ma_h));
            if (!khop_ten_hang && !khop_ma_hang) continue;
        }

        string sub = ds[i]->getsubtype();
        if (!sub.empty() && sub != "-" && !da_ton_tai(res, sub)) {
            res.push_back(sub);
        }
    }
    return res;
}

// 3. Lấy danh sách các kiểu con cụ thể (Áo thun, Áo sơ mi, Quần jean...) thuộc 1 subtype
// Phục vụ UI: khi người dùng tick vào "Áo", gọi hàm này để bung ra các loại áo con để tick tiếp!
VECTOR<string> Shop::lay_ds_kieu_con(const string& subtype, const string& hang) const {
    VECTOR<string> res;
    for (int i = 0; i < ds.size(); i++) {
        if (ds[i] == nullptr) continue;

        // Khớp hãng nếu có chỉ định
        if (!hang.empty()) {
            string ma_h = hang_sang_ma_hang(hang);
            bool khop_ten_hang = khop_chuoi(ds[i]->getbrand(), hang);
            bool khop_ma_hang = (ds[i]->getid().length() >= 4 && ds[i]->getid().substr(2, 2) == to_upper_str(ma_h));
            if (!khop_ten_hang && !khop_ma_hang) continue;
        }

        // Khớp subtype danh mục cha (ví dụ "Áo")
        if (!subtype.empty()) {
            if (!khop_chuoi(ds[i]->getsubtype(), subtype)) continue;
        }

        string kieu = ds[i]->getkieu();
        if (!kieu.empty() && kieu != "-" && !da_ton_tai(res, kieu)) {
            res.push_back(kieu);
        }
    }
    return res;
}

// 4. Bộ lọc đa tiêu chí phân cấp:
// - hang: lọc theo hãng (rỗng = tất cả hãng)
// - subtype: lọc theo danh mục cha (Áo, Quần...) (rỗng = tất cả)
// - kieu_cu_the: lọc cụ thể theo kiểu con (Áo thun, Áo khoác...) (rỗng = tất cả kiểu của subtype đó)
// - gia_min, gia_max: khoảng giá (0 = không giới hạn)
// - sap_xep: 0: mã SP từ trên xuống (theo hãng), 1: giá tăng dần, 2: giá giảm dần, 3: tên A-Z
VECTOR<product*> Shop::loc_san_pham(const string& hang,
                                   const string& subtype,
                                   const string& kieu_cu_the,
                                   long long gia_min,
                                   long long gia_max,
                                   int sap_xep) const {
    VECTOR<product*> res;

    for (int i = 0; i < ds.size(); i++) {
        product* p = ds[i];
        if (p == nullptr) continue;

        // 1. Kiểm tra lọc Hãng
        if (!hang.empty()) {
            string ma_h = hang_sang_ma_hang(hang);
            bool khop_ten = khop_chuoi(p->getbrand(), hang);
            bool khop_ma = (p->getid().length() >= 4 && p->getid().substr(2, 2) == to_upper_str(ma_h));
            if (!khop_ten && !khop_ma) continue;
        }

        // 2. Kiểm tra lọc Danh mục cha (Subtype: Áo, Quần, Giày, Mũ...)
        if (!subtype.empty()) {
            if (!khop_chuoi(p->getsubtype(), subtype)) continue;
        }

        // 3. Kiểm tra lọc Kiểu cụ thể (Áo thun, Áo polo, Quần jean...)
        // Nếu kieu_cu_the rỗng -> hiển thị tất cả các kiểu thuộc subtype đó (như yêu cầu của bro)
        // Nếu kieu_cu_the được tick cụ thể -> chỉ hiện đúng kiểu đó!
        if (!kieu_cu_the.empty()) {
            if (!khop_chuoi(p->getkieu(), kieu_cu_the)) continue;
        }

        // 4. Kiểm tra khoảng giá
        if (gia_min > 0 && p->getprice() < gia_min) continue;
        if (gia_max > 0 && p->getprice() > gia_max) continue;

        res.push_back(p);
    }

    // Sắp xếp kết quả theo yêu cầu
    int n = res.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            bool can_doi = false;
            if (sap_xep == 0) {
                // Sắp xếp theo mã SP từ trên xuống dưới theo hãng (SPLV... -> SPNK...)
                if (res[i]->getid() > res[j]->getid()) can_doi = true;
            } else if (sap_xep == 1) {
                // Giá tăng dần
                if (res[i]->getprice() > res[j]->getprice()) can_doi = true;
            } else if (sap_xep == 2) {
                // Giá giảm dần
                if (res[i]->getprice() < res[j]->getprice()) can_doi = true;
            } else if (sap_xep == 3) {
                // Tên A-Z
                if (res[i]->getnamenorm() > res[j]->getnamenorm()) can_doi = true;
            }
            if (can_doi) {
                product* temp = res[i];
                res[i] = res[j];
                res[j] = temp;
            }
        }
    }

    return res;
}

// 5. Tìm kiếm thông minh theo % khớp từ khóa (Smart Keyword Match):
// Tách từ, so khớp độ phù hợp, sắp xếp kết quả từ % khớp cao nhất đến thấp nhất
VECTOR<KetQuaTimKiem> Shop::tim_kiem(const string& tu_khoa, double nguong_khop_min) const {
    VECTOR<KetQuaTimKiem> res;

    for (int i = 0; i < ds.size(); i++) {
        if (ds[i] == nullptr) continue;
        double ti_le = ds[i]->tinh_ti_le_khop(tu_khoa);

        if (ti_le >= nguong_khop_min) {
            KetQuaTimKiem kq;
            kq.sp = ds[i];
            kq.ti_le_khop = ti_le;
            res.push_back(kq);
        }
    }

    // Sắp xếp giảm dần theo tỷ lệ % khớp (sản phẩm khớp nhiều nhất đứng đầu)
    int n = res.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            // Ưu tiên 1: % khớp cao hơn đứng trước
            // Ưu tiên 2: Nếu cùng % khớp, sản phẩm có giá thấp hơn đứng trước
            if (res[i].ti_le_khop < res[j].ti_le_khop || 
               (res[i].ti_le_khop == res[j].ti_le_khop && res[i].sp->getprice() > res[j].sp->getprice())) {
                KetQuaTimKiem temp = res[i];
                res[i] = res[j];
                res[j] = temp;
            }
        }
    }

    return res;
}

// 6. Tìm kiếm kết hợp bộ lọc (cho Khách hàng vừa gõ từ khóa vừa lọc hãng/kiểu/giá):
VECTOR<KetQuaTimKiem> Shop::tim_kiem_va_loc(const string& tu_khoa,
                                           const string& hang,
                                           const string& subtype,
                                           const string& kieu_cu_the,
                                           long long gia_min,
                                           long long gia_max,
                                           int sap_xep,
                                           double nguong_khop_min) const {
    // 1. Lọc sản phẩm theo các tiêu chí cố định (Hãng, Subtype, Kiểu con, Khoảng giá)
    VECTOR<product*> ds_da_loc = loc_san_pham(hang, subtype, kieu_cu_the, gia_min, gia_max, sap_xep);

    VECTOR<KetQuaTimKiem> res;

    // 2. Nếu không có từ khóa tìm kiếm -> lấy toàn bộ sản phẩm đã lọc (100% khớp)
    if (tu_khoa.empty()) {
        for (int i = 0; i < ds_da_loc.size(); i++) {
            KetQuaTimKiem kq;
            kq.sp = ds_da_loc[i];
            kq.ti_le_khop = 100.0;
            res.push_back(kq);
        }
        return res;
    }

    // 3. Nếu có từ khóa -> tính % khớp cho từng sản phẩm trong danh sách đã lọc
    for (int i = 0; i < ds_da_loc.size(); i++) {
        double ti_le = ds_da_loc[i]->tinh_ti_le_khop(tu_khoa);
        if (ti_le >= nguong_khop_min) {
            KetQuaTimKiem kq;
            kq.sp = ds_da_loc[i];
            kq.ti_le_khop = ti_le;
            res.push_back(kq);
        }
    }

    // Sắp xếp kết quả tìm kiếm: ưu tiên % khớp cao nhất lên đầu
    int n = res.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (res[i].ti_le_khop < res[j].ti_le_khop) {
                KetQuaTimKiem temp = res[i];
                res[i] = res[j];
                res[j] = temp;
            }
        }
    }

    return res;
}