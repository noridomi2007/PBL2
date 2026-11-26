#include "Bosuutap.h"
#include "../Tienich/Tienich.h"
#include "../Tienich/Tiengviet.h"
#include <fstream>
#include <filesystem>

static bool trung_ten(const string &a, const string &b) {   // không phân biệt hoa thường và dấu
    return normalize(a.c_str()) == normalize(b.c_str());
}

bool BoSuuTap::doc_file() {
    ds.clear();
    ifstream fin(file);
    if (!fin.is_open()) return true;   // chưa có bộ sưu tập nào, không phải lỗi
    string line;
    while (getline(fin, line)) {
        string tok[2];
        int n = tach_dong_bang(line, tok, 2);
        if (n < 2 || tok[0].empty() || tok[1].empty() || tok[0] == "idsp") continue;
        bool da_co = false;
        for (int i = 0; i < ds.size(); i++) if (ds[i].idsp == tok[0]) { da_co = true; break; }
        if (da_co) continue;
        Dong d; d.idsp = tok[0]; d.ten = tok[1];
        ds.push_back(d);
    }
    return true;
}

bool BoSuuTap::ghi_file() const {
    string tam = file + ".tmp";
    {
        ofstream fout(tam);
        if (!fout.is_open()) return false;
        const int rong[2] = { RONG_BST_IDSP, RONG_BST_TEN };
        const string tieu_de[2] = { "idsp", "bosuutap" };
        fout << dong_bang(tieu_de, rong, 2) << "\n";
        for (int i = 0; i < ds.size(); i++) {
            const string cot[2] = { ds[i].idsp, ds[i].ten };
            fout << dong_bang(cot, rong, 2) << "\n";
        }
        fout.close();
        if (fout.fail()) return false;
    }
    error_code ec;
    filesystem::rename(tam, file, ec);
    return !ec;
}

string BoSuuTap::cua(const string &idsp) const {
    for (int i = 0; i < ds.size(); i++) if (ds[i].idsp == idsp) return ds[i].ten;
    return "";
}

KetQuaBST BoSuuTap::dat(const string &idsp, const string &ten_vao) {
    string ten = cat_khoang_trang(ten_vao);
    if (co_ky_tu_cam(ten)) return BST_KY_TU_CAM;
    if (len_utf8(ten.c_str()) > RONG_BST_TEN) return BST_QUA_DAI;

    // Gõ lại tên đã có (khác hoa thường / dấu) thì dùng đúng tên đang có, để không đẻ ra 2 bộ sưu tập gần giống nhau
    for (int i = 0; i < ds.size() && !ten.empty(); i++)
        if (trung_ten(ds[i].ten, ten)) { ten = ds[i].ten; break; }

    VECTOR<Dong> truoc = ds;           // để hoàn lại nếu ghi file lỗi
    int vt = -1;
    for (int i = 0; i < ds.size(); i++) if (ds[i].idsp == idsp) { vt = i; break; }
    if (ten.empty()) {
        if (vt != -1) ds.remove(vt);
    } else if (vt != -1) {
        ds[vt].ten = ten;
    } else {
        Dong d; d.idsp = idsp; d.ten = ten;
        ds.push_back(d);
    }
    if (!ghi_file()) { ds = truoc; return BST_LOI_GHI_FILE; }
    return BST_OK;
}

VECTOR<string> BoSuuTap::ds_ten() const {
    VECTOR<string> kq;
    for (int i = 0; i < ds.size(); i++) {
        bool co = false;
        for (int j = 0; j < kq.size(); j++) if (kq[j] == ds[i].ten) { co = true; break; }
        if (!co) kq.push_back(ds[i].ten);
    }
    return kq;
}

int BoSuuTap::dem(const string &ten) const {
    int n = 0;
    for (int i = 0; i < ds.size(); i++) if (ds[i].ten == ten) n++;
    return n;
}
