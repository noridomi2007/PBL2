#include "Boloc.h"
#include "../Tienich/Tiengviet.h"
#include "../Tienich/Sapxep.h"


VECTOR<KetQuaLoc> loc_cua_hang(const Shop &shop, const BoSuuTap &bst, const TieuChiLoc &tc) {
    long long gmin = tc.gia_min, gmax = tc.gia_max;
    if (gmin > 0 && gmax > 0 && gmin > gmax) { long long t = gmin; gmin = gmax; gmax = t; }   // gõ ngược thì tự đảo

    // 1. Từ khóa + hãng + loại + kiểu + giá: dùng lại hàm có sẵn của Shop
    string tk = tc.tu_khoa;
    trim(tk);
    VECTOR<KetQuaTimKiem> co = shop.tim_kiem_va_loc(tk, tc.hang, tc.subtype, tc.kieu, gmin, gmax, 0);

    // 2. Ba tiêu chí mới
    const DoanhThu &sodt = shop.lay_doanh_thu();
    VECTOR<KetQuaLoc> kq;
    for (int i = 0; i < co.size(); i++) {
        product *p = co[i].sp;
        if (!tc.bo_suu_tap.empty() && bst.cua(p->getid()) != tc.bo_suu_tap) continue;
        int ban = sodt.da_ban(p->getid());
        if (tc.ban_min > 0 && ban < tc.ban_min) continue;
        int ton = p->getsl();
        if (tc.chi_con_hang && ton <= 0) continue;
        KetQuaLoc r;
        r.sp = p; r.khop = co[i].ti_le_khop; r.da_ban = ban; r.ton = ton;
        kq.push_back(r);
    }

    // 3. Sắp xếp. Luôn xếp theo mã trước để các phần tử bằng nhau có thứ tự cố định, rồi mới xếp theo tiêu chí chính.
    sap_xep_tron(kq, [](const KetQuaLoc &a, const KetQuaLoc &b) { return a.sp->getid() < b.sp->getid(); });
    bool co_tu_khoa = !tk.empty();
    switch (tc.sap_xep) {
        case SX_LIEN_QUAN:
            if (co_tu_khoa) sap_xep_tron(kq, [](const KetQuaLoc &a, const KetQuaLoc &b) { return a.khop > b.khop; });
            break;
        case SX_BAN_CHAY:
            sap_xep_tron(kq, [](const KetQuaLoc &a, const KetQuaLoc &b) { return a.da_ban > b.da_ban; });
            break;
        case SX_GIA_TANG:
            sap_xep_tron(kq, [](const KetQuaLoc &a, const KetQuaLoc &b) { return a.sp->getprice() < b.sp->getprice(); });
            break;
        case SX_GIA_GIAM:
            sap_xep_tron(kq, [](const KetQuaLoc &a, const KetQuaLoc &b) { return a.sp->getprice() > b.sp->getprice(); });
            break;
        case SX_TEN_AZ:
            sap_xep_tron(kq, [](const KetQuaLoc &a, const KetQuaLoc &b) { return a.sp->getnamenorm() < b.sp->getnamenorm(); });
            break;
        case SX_TEN_ZA:
            sap_xep_tron(kq, [](const KetQuaLoc &a, const KetQuaLoc &b) { return a.sp->getnamenorm() > b.sp->getnamenorm(); });
            break;
    }
    return kq;
}
