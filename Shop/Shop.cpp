#include "Shop.h"
#include "../librarytucode/Libtiengviet.h"
#include <fstream>
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
    if (vi_tri == -1) {
        return false; // Không tìm thấy mã cần xóa
    }

    // 1. Giải phóng vùng nhớ heap của sản phẩm
    delete ds[vi_tri];

    // 2. Xóa con trỏ khỏi vector
    ds.remove(vi_tri);
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


// Bán hàng: kiểm tra rồi trừ kho đúng ô (màu, size)
KetQuaBan Shop::ban(const string& id, const string& color, const string& size, int so_luong) {
    if (so_luong <= 0) return BAN_SO_LUONG_SAI;

    int vi_tri = find(id);
    if (vi_tri == -1) return BAN_KHONG_CO_SP;

    product* p = ds[vi_tri];
    if (p->tim_variant(color, size) == -1) return BAN_KHONG_CO_BIEN_THE;
    if (!p->tru_kho(color, size, so_luong)) return BAN_THIEU_HANG;

    // Lưu ngay xuống file; nếu ghi lỗi thì hoàn lại kho để RAM và file không lệch nhau
    if (!ghi_file(file_hien_tai)) {
        p->them_variant(color, size, so_luong);
        return BAN_LOI_GHI_FILE;
    }
    return BAN_OK;
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
    file_hien_tai = duong_dan; // nhớ file này để ban() ghi lại đúng chỗ
    return true;
}

// Ghi toàn bộ danh sách sản phẩm ra file theo định dạng bảng chuẩn thẳng hàng
bool Shop::ghi_file(const string& duong_dan) const {
    ofstream fout(duong_dan);
    if (!fout.is_open()) {
        cerr << "[Lỗi] Không thể mở file để ghi: " << duong_dan << endl;
        return false;
    }

    // Ghi dòng tiêu đề (Header) căn chuẩn xác độ rộng từng cột
    fout << "| " << setw(12) << left << "idsp"
         << " | " << setw(26) << left << "namesp"
         << " | " << setw(14) << left << "brandsp"
         << " | " << setw(20) << left << "Kieusp"
         << " | " << setw(12) << left << "color"
         << " | " << setw(11) << left << "gia"
         << " | " << setw(5) << left << "sl"
         << " | " << setw(7) << left << "size"
         << " | " << setw(14) << left << "material"
         << " | " << setw(11) << left << "dimension"
         << " | " << setw(14) << left << "subtype"
         << " |\n";

    // Ghi từng sản phẩm
    for (int i = 0; i < ds.size(); i++) {
        if (ds[i] != nullptr) {
            ds[i]->ghi_file(fout);
        }
    }

    fout.close();
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