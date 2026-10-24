#include "item.h"
#include "../librarytucode/Libtiengviet.h"
#include <iomanip>
#include <cctype>

// =========================================================================
// HÀM TIỆN ÍCH NỘI BỘ
// =========================================================================
static void in_o(ostream &os, const string &s, int visual_width) {
    int w = width_utf8(s.c_str(), visual_width);
    os << setw(w) << left << s;
}

static char to_upper_char(char c) {
    return static_cast<char>(toupper(static_cast<unsigned char>(c)));
}

// So khớp màu / size: bỏ qua hoa thường và dấu ("đen" == "Đen", "m" == "M")
static bool giong_nhau(const string &a, const string &b) {
    return normalize(a.c_str()) == normalize(b.c_str());
}

static bool da_co(const VECTOR<string> &list, const string &s) {
    for (int i = 0; i < list.size(); i++) {
        if (giong_nhau(list[i], s)) return true;
    }
    return false;
}

static string lam_sach_chuoi_so(const string &s) {
    string res = "";
    for (char c : s) {
        if (c != '.' && c != ',') {
            res += c;
        }
    }
    return res;
}


// =========================================================================
// 1. CÁC HÀM TIỆN ÍCH QUẢN LÝ MÃ SẢN PHẨM (SMART ID: SPCM0001A - 9 KÝ TỰ)
// =========================================================================

// Lấy ký tự kiểu sản phẩm ở cuối mã idsp (ví dụ "SPCM0001A" -> 'A')
char lay_ma_kieu_idsp(const string &idsp) {
    if (idsp.empty()) return '\0';
    return to_upper_char(idsp[idsp.length() - 1]);
}

// Kiểm tra ký tự kiểu có thuộc nhóm Quần Áo / Giày Dép (Clothing) không
bool la_ma_kieu_quan_ao(char ma_kieu) {
    char k = to_upper_char(ma_kieu);
    return (k == 'A' || k == 'Q' || k == 'V' || k == 'G' || k == 'S');
}

// Kiểm tra ký tự kiểu có thuộc nhóm Phụ Kiện (Accessory) không
bool la_ma_kieu_phu_kien(char ma_kieu) {
    char k = to_upper_char(ma_kieu);
    return (k == 'M' || k == 'D' || k == 'T' || k == 'K' || k == 'X' || k == 'P');
}

// Kiểm tra mã sản phẩm chuẩn SKU: đúng 9 ký tự (SP + 2 chữ hãng + 4 số STT + 1 kiểu hợp lệ)
// Ví dụ: SPCM0001A, SPLE0012Q, SPCS0005D...
bool kiem_tra_ma_sp(const string &idsp) {
    if (idsp.length() != 9) return false;

    // 2 ký tự đầu: "SP"
    if (to_upper_char(idsp[0]) != 'S' || to_upper_char(idsp[1]) != 'P') return false;

    // 2 ký tự tiếp theo: Mã hãng (chữ cái)
    if (!isalpha(static_cast<unsigned char>(idsp[2])) || !isalpha(static_cast<unsigned char>(idsp[3]))) return false;

    // 4 ký tự tiếp theo: Số thứ tự 0001 - 9999
    for (int i = 4; i <= 7; i++) {
        if (!isdigit(static_cast<unsigned char>(idsp[i]))) return false;
    }

    // Ký tự cuối cùng: Mã kiểu
    char k = to_upper_char(idsp[8]);
    return la_ma_kieu_quan_ao(k) || la_ma_kieu_phu_kien(k);
}

// --- BẢNG TRA CỨU KIỂU SẢN PHẨM (DÙNG CHO UI DROPDOWN) ---
// UI đọc ten từ bảng này cho người dùng chọn, rồi lấy luôn ma tương ứng.
// Không dùng đoán chuỗi con nên không bao giờ bị sai nhóm clothing/accessory.
const kieu_sp_entry DS_KIEU_SP[] = {
    {"Áo",           'A'},
    {"Quần",         'Q'},
    {"Váy/Đầm",      'V'},
    {"Giày/Dép",     'G'},
    {"Set đồ",       'S'},
    {"Mũ",           'M'},
    {"Đồng hồ",      'D'},
    {"Túi/Ví/Balo",  'T'},
    {"Kính",         'K'},
    {"Trang sức",    'X'},
    {"Phụ kiện khác",'P'}
};
const int SO_KIEU_SP = sizeof(DS_KIEU_SP) / sizeof(DS_KIEU_SP[0]);

// --- BẢNG TRA CỨU HÃNG (DÙNG CHO UI DROPDOWN) ---
const hang_sp_entry DS_HANG_SP[] = {
    {"Coolmate",      "CM"},
    {"Louis Vuitton", "LV"}, 
    {"Nike",          "NK"},
    {"Adidas",        "AD"},
    {"Casio",         "CS"},
    {"Levi's",        "LE"},
    {"Gucci",         "GC"},
    {"Zara",          "ZR"},
    {"Uniqlo",        "UQ"},
    {"Agarwood",      "AG"},
    {"Puma",          "PM"},
    {"Dior",          "DR"},
    {"Chanel",        "CH"},
};
const int SO_HANG_SP = sizeof(DS_HANG_SP) / sizeof(DS_HANG_SP[0]);

// Tra ngược: ký tự mã kiểu -> tên subtype trong DS_KIEU_SP ('A' -> "Áo", 'M' -> "Mũ")
// Dùng khi đọc file cũ chưa có cột subtype
string ma_kieu_sang_ten(char ma_kieu) {
    char k = to_upper_char(ma_kieu);
    for (int i = 0; i < SO_KIEU_SP; i++) {
        if (DS_KIEU_SP[i].ma == k) {
            return DS_KIEU_SP[i].ten;
        }
    }
    return "Phụ kiện khác";
}


// Chuyển tên kiểu (chọn từ UI, đúng với DS_KIEU_SP) -> ký tự mã kiểu
// So sánh chính xác (không fuzzy), nên tên phải khớp hoàn toàn với bảng ở trên.
// Nếu không tìm thấy -> trả 'P' (Phụ kiện khác) để an toàn
char kieu_sang_ma_kieu(const string &ten_kieu) {
    for (int i = 0; i < SO_KIEU_SP; i++) {
        if (ten_kieu == DS_KIEU_SP[i].ten) {
            return DS_KIEU_SP[i].ma;
        }
    }
    return 'P'; // Mặc định nếu tên không khớp bảng
}

// Chuyển tên hãng (chọn từ UI, đúng với DS_HANG_SP) -> mã 2 ký tự
// So sánh chính xác == (UI chọn sẵn nên tên luôn khớp bảng, không cần fuzzy)
// Nếu là hãng lạ không có trong bảng: normalize để bỏ dấu rồi lấy 2 chữ cái ASCII đầu viết hoa
string hang_sang_ma_hang(const string &ten_hang) {
    // Tìm trong bảng bằng so sánh chính xác (UI dropdown nên tên luôn khớp 100%)
    for (int i = 0; i < SO_HANG_SP; i++) {
        if (ten_hang == DS_HANG_SP[i].ten) {
            return DS_HANG_SP[i].ma;
        }
    }

    // Hãng lạ (Other): dùng normalize() để bỏ dấu tiếng Việt trước khi lấy 2 chữ cái đầu
    // Ví dụ: "Đông Hải" -> normalize -> "dong hai" -> "DO"
    string n = normalize(ten_hang.c_str());
    string res = "";
    for (char c : n) {
        if (isalpha(static_cast<unsigned char>(c))) {
            res += to_upper_char(c);
            if (res.length() == 2) break;
        }
    }
    while (res.length() < 2) res += 'X'; // Bù nếu tên quá ngắn
    return res;
}



// =========================================================================
// 2. CLASS PRODUCT (LỚP CHA CỦA SẢN PHẨM)
// =========================================================================

void product::capnhat_name_norm() {
    name_norm = normalize(namesp.c_str());
}

// ---------- VARIANT (MÀU + SIZE + SỐ LƯỢNG) ----------

int product::tim_variant(const string &color, const string &size) const {
    for (int i = 0; i < variants.size(); i++) {
        if (giong_nhau(variants[i].color, color) && giong_nhau(variants[i].size, size)) {
            return i;
        }
    }
    return -1;
}

bool product::them_variant(const string &color, const string &size, int sl) {
    string c = color, s = size;
    trim(c);
    trim(s);
    if (c.empty() || s.empty() || sl < 0) return false;

    int i = tim_variant(c, s);
    if (i != -1) {
        variants[i].sl += sl; // đã có ô này -> nhập thêm hàng
        return true;
    }
    variants.push_back(Variant(c, s, sl));
    return true;
}

bool product::dat_sl_variant(const string &color, const string &size, int sl) {
    if (sl < 0) return false;
    int i = tim_variant(color, size);
    if (i == -1) return false;
    variants[i].sl = sl;
    return true;
}

bool product::xoa_variant(const string &color, const string &size) {
    int i = tim_variant(color, size);
    if (i == -1) return false;
    variants.remove(i);
    return true;
}

int product::getsl() const {
    int tong = 0;
    for (int i = 0; i < variants.size(); i++) tong += variants[i].sl;
    return tong;
}

int product::getsl(const string &color, const string &size) const {
    int i = tim_variant(color, size);
    return (i == -1) ? 0 : variants[i].sl;
}

bool product::tru_kho(const string &color, const string &size, int sl) {
    if (sl <= 0) return false;
    int i = tim_variant(color, size);
    if (i == -1 || variants[i].sl < sl) return false;
    variants[i].sl -= sl;
    return true;
}

VECTOR<string> product::ds_mau() const {
    VECTOR<string> res;
    for (int i = 0; i < variants.size(); i++) {
        if (!da_co(res, variants[i].color)) res.push_back(variants[i].color);
    }
    return res;
}

VECTOR<string> product::ds_size(const string &color) const {
    VECTOR<string> res;
    for (int i = 0; i < variants.size(); i++) {
        if (!color.empty() && !giong_nhau(variants[i].color, color)) continue;
        if (!da_co(res, variants[i].size)) res.push_back(variants[i].size);
    }
    return res;
}

string product::ds_mau_chuoi() const {
    VECTOR<string> mau = ds_mau();
    string res = "";
    for (int i = 0; i < mau.size(); i++) {
        if (i > 0) res += ", ";
        res += mau[i];
    }
    return res;
}

// ---------- TÌM KIẾM ----------

// Tính tỷ lệ % khớp từ khóa tìm kiếm (từ 0.0 đến 100.0%)
// Phục vụ tính năng tìm kiếm thông minh như sàn TMĐT
double product::tinh_ti_le_khop(const string &tu_khoa_goc) const {
    string tk_norm = normalize(tu_khoa_goc.c_str());
    trim(tk_norm);
    if (tk_norm.empty()) return 0.0;

    // Chuỗi thông tin tổng hợp của sản phẩm đã bỏ dấu + chữ thường (gồm cả các màu đang có)
    string info = name_norm + " " + normalize(brandsp.c_str()) + " " +
                  normalize(subtype.c_str()) + " " + normalize(kieusp.c_str()) + " " +
                  normalize(ds_mau_chuoi().c_str()) + " " + normalize(idsp.c_str());

    // Nếu thông tin sản phẩm chứa trọn vẹn cả cụm từ tìm kiếm -> 100%
    if (info.find(tk_norm) != string::npos) {
        return 100.0;
    }

    // Tách từ khóa thành các từ đơn để kiểm tra độ khớp từng từ
    stringstream ss(tk_norm);
    string word;
    int tong_tu = 0;
    int so_tu_khop = 0;

    while (ss >> word) {
        if (word.empty()) continue;
        tong_tu++;
        if (info.find(word) != string::npos) {
            so_tu_khop++;
        }
    }

    if (tong_tu == 0) return 0.0;
    return ((double)so_tu_khop / tong_tu) * 100.0;
}

// ---------- IN / GHI FILE ----------

string product::xuat_thong_tin() const {
    stringstream ss;
    ss << nhan() << " Mã: " << idsp
       << " | Tên: " << namesp
       << " | Hãng: " << brandsp
       << " | Loại: " << subtype
       << " | Kiểu: " << kieusp
       << " | Giá: " << price << " VNĐ"
       << " | Tổng tồn: " << getsl();

    string mat = lay_material();
    if (mat != "-") ss << " | Chất liệu: " << mat;
    string dim = lay_dimension();
    if (dim != "-") ss << " | Kích cỡ/Đường kính: " << dim;

    ss << " | Biến thể: ";
    if (variants.empty()) {
        ss << "(chưa có)";
    }
    for (int i = 0; i < variants.size(); i++) {
        if (i > 0) ss << ", ";
        ss << variants[i].color << "-" << variants[i].size << ":" << variants[i].sl;
    }
    return ss.str();
}

// Mỗi variant 1 dòng, các dòng cùng idsp (thông tin chung lặp lại để file vẫn đọc được bằng mắt)
void product::ghi_file(ostream &os) const {
    string mat = lay_material();
    string dim = lay_dimension();
    string sub = subtype.empty() ? "-" : subtype;

    int n = variants.size();
    int so_dong = (n > 0) ? n : 1; // chưa có variant vẫn ghi 1 dòng để không mất sản phẩm

    for (int i = 0; i < so_dong; i++) {
        string c = "-", s = "-";
        int so = 0;
        if (n > 0) {
            c = variants[i].color;
            s = variants[i].size;
            so = variants[i].sl;
        }

        os << "| ";
        in_o(os, idsp, 12);
        os << " | ";
        in_o(os, namesp, 26);
        os << " | ";
        in_o(os, brandsp, 14);
        os << " | ";
        in_o(os, kieusp, 20);
        os << " | ";
        in_o(os, c, 12);
        os << " | ";
        in_o(os, to_string(price), 11);
        os << " | ";
        in_o(os, to_string(so), 5);
        os << " | ";
        in_o(os, s, 7);
        os << " | ";
        in_o(os, mat, 14);
        os << " | ";
        in_o(os, dim, 11);
        os << " | ";
        in_o(os, sub, 14);
        os << " |\n";
    }
}

// Đọc 1 dòng file -> 1 product chứa đúng 1 variant của dòng đó.
// Nhiều dòng cùng idsp sẽ được Shop::doc_file gộp lại thành 1 product nhiều variant.
product* product::doc_tu_chuoi(const string &line) {
    string raw_line = line;
    trim(raw_line);
    if (raw_line.empty()) return nullptr;

    if (raw_line.find("idsp") != string::npos || raw_line.find("namesp") != string::npos) {
        return nullptr;
    }

    stringstream ss(raw_line);
    string token;
    string tokens[14];
    int token_count = 0;

    while (getline(ss, token, '|') && token_count < 14) {
        trim(token);
        tokens[token_count++] = token;
    }

    int start_index = 0;
    while (start_index < token_count && tokens[start_index].empty()) {
        start_index++;
    }

    if (token_count - start_index < 7) {
        return nullptr;
    }

    string id    = tokens[start_index];
    // Tự động chuyển mã về chữ hoa
    for (char &c : id) c = to_upper_char(c);

    // Tự động nâng cấp mã cũ 7 ký tự (SPCM01A) -> mã mới 9 ký tự (SPCM0001A)
    if (id.length() == 7) id = id.substr(0, 4) + "00" + id.substr(4);

    string name  = tokens[start_index + 1];
    string brand = tokens[start_index + 2];
    string kieu  = tokens[start_index + 3];
    string col   = tokens[start_index + 4];
    if (col.empty()) col = "-";

    string gia_sach = lam_sach_chuoi_so(tokens[start_index + 5]);
    long long pr = 0;
    try {
        pr = stoll(gia_sach);
    } catch (...) { pr = 0; }
    if (pr < 0) pr = 0;

    int soluong = 0;
    try {
        soluong = stoi(tokens[start_index + 6]);
    } catch (...) { soluong = 0; }
    if (soluong < 0) soluong = 0;

    string sz  = (token_count - start_index > 7) ? tokens[start_index + 7] : "-";
    string mat = (token_count - start_index > 8) ? tokens[start_index + 8] : "-";
    string dim = (token_count - start_index > 9) ? tokens[start_index + 9] : "-";
    if (sz.empty()) sz = "-";

    bool is_acc = false;
    char ma_kieu = lay_ma_kieu_idsp(id);

    if (la_ma_kieu_phu_kien(ma_kieu)) {
        is_acc = true;
    } else if (la_ma_kieu_quan_ao(ma_kieu)) {
        is_acc = false;
    } else {
        is_acc = (dim != "-" && !dim.empty() && dim != "NA" && dim != "None");
    }

    // Cột subtype (cột thứ 11): file cũ không có hoặc ghi "-" thì suy ra từ ký tự cuối của mã
    string sub = (token_count - start_index > 10) ? tokens[start_index + 10] : "";
    if (sub.empty() || sub == "-") sub = ma_kieu_sang_ten(ma_kieu);

    product *p;
    if (is_acc) {
        p = new accessory(id, name, brand, sub, kieu, pr, mat, dim);
    } else {
        p = new clothing(id, name, brand, sub, kieu, pr, mat);
    }
    p->them_variant(col, sz, soluong);
    return p;
}

ostream& operator<<(ostream &os, const product &p) {
    os << p.xuat_thong_tin();
    return os;
}