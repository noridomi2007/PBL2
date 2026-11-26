// DOANH THU (CHỈ ADMIN): biểu đồ cột / tròn, so sánh kỳ, top sản phẩm, top nhân viên
#include "App.h"
#include "../Doanhthu/Doanhthu.h"
#include <cmath>
#include <ctime>
using namespace ui;

namespace {
struct Cot { long long khoa; std::string nhan; long long v; };

long long ts_tu_khoa_tuan(long long khoa) {
    tm t = {};
    t.tm_year = (int)(khoa / 10000) - 1900;
    t.tm_mon = (int)((khoa / 100) % 100) - 1;
    t.tm_mday = (int)(khoa % 100);
    t.tm_hour = 12; t.tm_isdst = -1;
    return (long long)mktime(&t);
}

std::string nhan_tuan(long long khoa) {
    char b[16]; snprintf(b, sizeof(b), "%02d/%02d", (int)(khoa % 100), (int)((khoa / 100) % 100));
    return b;
}

std::vector<Cot> dung_cot(const DoanhThu &dt, KieuKy k, long long khoa) {
    std::vector<Cot> r;
    if (k == KY_THANG) { long long y = khoa / 100; for (int m = 1; m <= 12; m++) r.push_back({ y * 100 + m, "T" + std::to_string(m), 0 }); }
    else if (k == KY_QUY) { long long y = khoa / 10; for (int q = 1; q <= 4; q++) r.push_back({ y * 10 + q, "Quý " + std::to_string(q), 0 }); }
    else if (k == KY_NAM) { for (long long y = khoa - 5; y <= khoa; y++) r.push_back({ y, std::to_string(y), 0 }); }
    else {
        long long ts = ts_tu_khoa_tuan(khoa);
        for (int i = 7; i >= 0; i--) { long long key = lay_khoa_ky(ts - 7LL * 86400 * i, KY_TUAN); r.push_back({ key, nhan_tuan(key), 0 }); }
    }
    for (Cot &c : r) c.v = dt.tong_doanh_thu(k, c.khoa);
    return r;
}

long long lam_tron_tren(long long v) {   // 7.300.000 -> 8.000.000 (để trục dọc đẹp)
    if (v <= 0) return 1000000;
    long long mu = 1;
    while (mu * 10 <= v) mu *= 10;
    long long b = mu / 2 > 0 ? mu / 2 : 1;
    return ((v + b - 1) / b) * b;
}

const Color MAU_CHART[12] = { {66, 158, 228, 255}, {164, 156, 232, 255}, {86, 184, 146, 255}, {245, 176, 110, 255},
                              {240, 150, 176, 255}, {112, 205, 224, 255}, {236, 206, 96, 255}, {150, 170, 196, 255},
                              {120, 120, 214, 255}, {140, 210, 160, 255}, {230, 120, 120, 255}, {196, 170, 130, 255} };

struct BoNho { int dk = -1; long long khoa = -1; int n = -1; std::vector<Cot> cot; long long hd = 0, sl = 0; };
BoNho nho;
}

void App::dt_doi_ky(KieuKy k, long long khoa) { dt_ky = k; dt_khoa = khoa; dt_anim0 = GetTime(); }

void App::ve_doanh_thu(Rectangle view) {
    const DoanhThu &dt = shop.lay_doanh_thu();
    float x = view.x + 32, w = view.width - 64, y = view.y - cuon + 26;

    // tính lại số liệu khi đổi kỳ / có đơn mới
    if (nho.dk != (int)dt_ky || nho.khoa != dt_khoa || nho.n != dt.so_giao_dich()) {
        nho.dk = (int)dt_ky; nho.khoa = dt_khoa; nho.n = dt.so_giao_dich();
        nho.cot = dung_cot(dt, dt_ky, dt_khoa);
        nho.sl = 0;
        HASHTABLE<int> da;
        long long dem = 0;
        for (int i = 0; i < dt.so_giao_dich(); i++) {
            const GiaoDich &g = dt.lay(i);
            if (lay_khoa_ky(g.thoi_gian, dt_ky) != dt_khoa) continue;
            nho.sl += g.sl;
            if (g.ma_hd.empty()) dem++;
            else if (!da.contains(g.ma_hd)) { da.put(g.ma_hd, 1); dem++; }
        }
        nho.hd = dem;
    }
    std::vector<Cot> &cot = nho.cot;
    long long cur = 0, truoc = 0;
    int vt = -1;
    for (size_t i = 0; i < cot.size(); i++) if (cot[i].khoa == dt_khoa) vt = (int)i;
    if (vt >= 0) cur = cot[vt].v;
    if (vt > 0) truoc = cot[vt - 1].v;

    // ---- hàng điều khiển ----
    struct Ky { const char *t; KieuKy k; };
    Ky kys[] = { { "Tuần", KY_TUAN }, { "Tháng", KY_THANG }, { "Quý", KY_QUY }, { "Năm", KY_NAM } };
    float cx = x;
    for (Ky &k : kys) {
        if (chip(Rectangle{ cx, y, 84, 38 }, k.t, dt_ky == k.k, F.m15)) dt_doi_ky(k.k, khoa_ky_hien_tai(k.k));
        cx += 92;
    }
    // chuyển kỳ trước / sau
    float nx = cx + 14;
    Rectangle bl = { nx, y, 38, 38 }, br = { nx + 46 + 190, y, 38, 38 };
    std::string ten_k = ten_ky(dt_khoa, dt_ky);
    rrect(Rectangle{ nx + 44, y, 194, 38 }, 19, SKY100);
    text_c(F.m15, ten_k, Rectangle{ nx + 44, y, 194, 38 }, SKY600);
    long long hien = khoa_ky_hien_tai(dt_ky);
    bool bam_l = icon_button(bl, IC_LEFT, TEXT, SKY100, 18);
    bool co_sau = dt_khoa < hien;
    bool bam_r = icon_button(br, IC_RIGHT, co_sau ? TEXT : Color{ 190, 205, 220, 255 }, SKY100, 18);
    int huong = bam_l ? -1 : (bam_r && co_sau ? 1 : 0);
    if (huong != 0) {
        long long nk = dt_khoa;
        if (dt_ky == KY_THANG) { long long ny = dt_khoa / 100 + huong; nk = ny * 100 + dt_khoa % 100; }
        else if (dt_ky == KY_QUY) { long long ny = dt_khoa / 10 + huong; nk = ny * 10 + dt_khoa % 10; }
        else if (dt_ky == KY_NAM) nk = dt_khoa + huong;
        else nk = lay_khoa_ky(ts_tu_khoa_tuan(dt_khoa) + huong * 7LL * 86400, KY_TUAN);
        if (nk <= hien) dt_doi_ky(dt_ky, nk);
    }
    if (dt_ky == KY_TUAN) {
        float fx = br.x + 56;
        if (fx + 200 < x + w - 180) {
            bool enter = field(f_dt_ngay, Rectangle{ fx, y - 2, 134, 42 }, F.r15);
            if (button(Rectangle{ fx + 142, y, 60, 38 }, "Xem", BTN_SOFT, F.m13, true, 19) || enter) {
                int d, m, yy; long long ts;
                if (f_dt_ngay.date(d, m, yy) && tao_thoi_gian(d, m, yy, ts)) dt_doi_ky(KY_TUAN, lay_khoa_ky(ts, KY_TUAN));
                else toast("Ngày không hợp lệ hoặc sau hôm nay.", TOAST_ERR);
            }
        }
    }
    if (chip(Rectangle{ x + w - 172, y, 80, 38 }, "Cột", dt_kieu == CH_BAR, F.m15)) { dt_kieu = CH_BAR; dt_anim0 = GetTime(); }
    if (chip(Rectangle{ x + w - 84, y, 84, 38 }, "Tròn", dt_kieu == CH_PIE, F.m15)) { dt_kieu = CH_PIE; dt_anim0 = GetTime(); }
    y += 38 + 22;

    // ---- 4 thẻ số liệu ----
    float kw = (w - 3 * 16) / 4;
    std::string so_sanh = "-";
    Color c_ss = MUTED;
    if (vt > 0 && truoc > 0) {
        double pt = (double)(cur - truoc) * 100.0 / (double)truoc;
        char b[32]; snprintf(b, sizeof(b), "%+.1f%%", pt);
        so_sanh = b; c_ss = pt >= 0 ? OKC : DANGER;
    } else if (vt > 0 && truoc == 0 && cur > 0) { so_sanh = "Mới"; c_ss = OKC; }
    struct Tk { const char *nh; std::string gt; Color c; const char *phu; };
    Tk tk[] = { { "DOANH THU KỲ NÀY", tien(cur), SKY600, "sau giảm giá" }, { "SO VỚI KỲ TRƯỚC", so_sanh, c_ss, vt > 0 ? "kỳ liền trước trong biểu đồ" : "" },
                { "SỐ HÓA ĐƠN", std::to_string(nho.hd), INK, "trong kỳ" }, { "SẢN PHẨM ĐÃ BÁN", std::to_string(nho.sl), INK, "tổng số lượng" } };
    for (int i = 0; i < 4; i++) {
        Rectangle r = { x + i * (kw + 16), y, kw, 100 };
        panel(r, 18);
        text(F.m13, tk[i].nh, r.x + 20, r.y + 18, MUTED, 1.4f);
        text(F.b21, fit(F.b21, tk[i].gt, kw - 36), r.x + 20, r.y + 42, tk[i].c);
        text(F.r13, tk[i].phu, r.x + 20, r.y + 74, MUTED);
    }
    y += 100 + 22;

    // ---- biểu đồ chính ----
    float cw = w * 0.63f - 11, rw = w - cw - 22;
    Rectangle pr = { x, y, cw, 430 };
    panel(pr, 20);
    std::string tieu = dt_ky == KY_THANG ? "Doanh thu theo tháng, năm " + std::to_string(dt_khoa / 100)
                     : dt_ky == KY_QUY ? "Doanh thu theo quý, năm " + std::to_string(dt_khoa / 10)
                     : dt_ky == KY_NAM ? "Doanh thu 6 năm gần nhất" : "Doanh thu 8 tuần gần nhất (ngày đầu tuần)";
    text(F.s22, tieu, pr.x + 26, pr.y + 22, INK);
    text(F.r13, "Bấm vào cột / miếng để chọn kỳ", pr.x + 26, pr.y + 54, MUTED);
    float anim = ease_out((float)((GetTime() - dt_anim0) / 0.7));
    long long tong = 0, vmax = 0;
    for (Cot &c : cot) { tong += c.v; if (c.v > vmax) vmax = c.v; }
    int hov_cot = -1;

    if (dt_kieu == CH_BAR) {
        Rectangle pl = { pr.x + 74, pr.y + 96, pr.width - 74 - 26, pr.height - 96 - 52 };
        long long tran = lam_tron_tren(vmax);
        for (int g = 0; g <= 4; g++) {
            float gy = pl.y + pl.height - pl.height * g / 4.0f;
            hline(pl.x, gy, pl.width, g == 0 ? Color{ 190, 210, 228, 255 } : LINE);
            text_r(F.r13, tien_gon(tran * g / 4), Rectangle{ pr.x, gy - 10, 66, 20 }, MUTED, 0);
        }
        float slot = pl.width / cot.size(), bw = fminf(54.0f, slot * 0.58f);
        for (size_t i = 0; i < cot.size(); i++) {
            float bx = pl.x + slot * i + (slot - bw) / 2;
            float hh = pl.height * (float)cot[i].v / (float)tran * anim;
            Rectangle br_ = { bx, pl.y + pl.height - hh, bw, hh };
            Rectangle vung = { pl.x + slot * i, pl.y, slot, pl.height + 30 };
            bool h = hover(vung);
            bool chon = cot[i].khoa == dt_khoa;
            if (h) { hov_cot = (int)i; C.hand = true; if (C.pressed) dt_doi_ky(dt_ky, cot[i].khoa); }
            if (h) rrect(vung, 10, Fade(SKY100, 0.5f));
            if (hh >= 2) {
                rrect(Rectangle{ br_.x, br_.y, br_.width, br_.height < 8 ? 8 : br_.height }, 8, chon ? SKY500 : (h ? SKY400 : SKY300));
                if (hh > 8) DrawRectangle((int)br_.x, (int)(br_.y + br_.height - 6), (int)br_.width, 6, chon ? SKY500 : (h ? SKY400 : SKY300));
            }
            if (cot[i].v > 0 && anim > 0.9f) text_c(F.m13, tien_gon(cot[i].v), Rectangle{ bx - 14, br_.y - 24, bw + 28, 20 }, chon ? SKY600 : MUTED);
            text_c(chon ? F.m13 : F.r13, cot[i].nhan, Rectangle{ pl.x + slot * i, pl.y + pl.height + 8, slot, 22 }, chon ? SKY600 : TEXT);
        }
        if (tong == 0) text_c(F.r15, "Chưa có doanh thu trong khoảng này", Rectangle{ pl.x, pl.y + pl.height / 2 - 14, pl.width, 28 }, MUTED);
        if (hov_cot >= 0) {
            std::string t1 = cot[hov_cot].nhan, t2 = tien(cot[hov_cot].v);
            float tw_ = fmaxf(tw(F.m13, t1), tw(F.b15, t2)) + 28;
            Rectangle tr = { C.mouse.x + 14, C.mouse.y - 56, tw_, 48 };
            if (tr.x + tr.width > C.sw - 10) tr.x = C.mouse.x - 14 - tr.width;
            rrect(tr, 10, INK);
            text(F.r13, t1, tr.x + 14, tr.y + 7, Color{ 190, 215, 238, 255 });
            text(F.b15, t2, tr.x + 14, tr.y + 25, WHITE);
        }
    } else {
        Rectangle area = { pr.x, pr.y + 84, pr.width, pr.height - 84 };
        float rad = fminf(area.height * 0.5f - 18, area.width * 0.28f);
        Vector2 ctr = { area.x + 36 + rad + 10, area.y + area.height / 2 };
        std::vector<int> co;
        for (size_t i = 0; i < cot.size(); i++) if (cot[i].v > 0) co.push_back((int)i);
        float goc = -90.0f, sweep_max = 360.0f * anim;
        int hov_s = -1;
        if (tong > 0) {
            float dx = C.mouse.x - ctr.x, dy = C.mouse.y - ctr.y, d = sqrtf(dx * dx + dy * dy);
            if (hover(area) && d <= rad + 8 && d >= rad * 0.58f) {
                float a = fmodf(atan2f(dy, dx) * 57.2958f + 90.0f + 360.0f, 360.0f), acc = 0;
                for (int i : co) { float sp = 360.0f * (float)cot[i].v / (float)tong; if (a >= acc && a < acc + sp) { hov_s = i; break; } acc += sp; }
            }
            float acc = 0;
            for (int i : co) {
                float sp = 360.0f * (float)cot[i].v / (float)tong;
                float a0 = goc + acc, a1 = a0 + sp;
                float lim = goc + sweep_max;
                if (a0 < lim) {
                    Vector2 c2 = ctr;
                    if (i == hov_s || cot[i].khoa == dt_khoa) { float m = (a0 + a1) / 2 * 0.017453f; c2.x += cosf(m) * 7; c2.y += sinf(m) * 7; }
                    DrawCircleSector(c2, rad, a0, fminf(a1, lim), 64, MAU_CHART[(&cot[i] - &cot[0]) % 12]);
                }
                acc += sp;
            }
        } else DrawRing(ctr, rad * 0.58f, rad, 0, 360, 64, SKY100);
        DrawCircleV(ctr, rad * 0.58f, SURFACE);
        text_c(F.m13, "TỔNG", Rectangle{ ctr.x - 60, ctr.y - 26, 120, 18 }, MUTED, 1.4f);
        text_c(F.b17, tien_gon(tong), Rectangle{ ctr.x - 70, ctr.y - 6, 140, 28 }, INK);
        if (hov_s >= 0) { C.hand = true; if (C.pressed) dt_doi_ky(dt_ky, cot[hov_s].khoa); }
        // chú thích (nhiều hơn 8 mục thì chia 2 cột, chỉ ghi % cho gọn)
        float lx = ctr.x + rad + 40, ly0 = area.y + 16;
        float vung_rong = pr.x + pr.width - lx - 14;
        int cot_so = co.size() > 8 ? 2 : 1;
        float cot_w = vung_rong / cot_so;
        if (tong == 0) text(F.r15, "Chưa có doanh thu trong khoảng này", lx, area.y + area.height / 2 - 10, MUTED);
        for (size_t k = 0; k < co.size(); k++) {
            int i = co[k];
            int c = cot_so == 2 ? (int)(k / ((co.size() + 1) / 2)) : 0;
            int hang = cot_so == 2 ? (int)(k % ((co.size() + 1) / 2)) : (int)k;
            float lx2 = lx + c * cot_w;
            float ly = ly0 + hang * 34.0f;
            if (ly > area.y + area.height - 30) break;
            Rectangle lr = { lx2 - 8, ly - 4, cot_w - 10, 34 };
            bool hv = hover(lr) || hov_s == i;
            if (hover(lr)) { C.hand = true; if (C.pressed) dt_doi_ky(dt_ky, cot[i].khoa); }
            if (hv) rrect(lr, 10, SKY50);
            rrect(Rectangle{ lx2, ly + 6, 14, 14 }, 4, MAU_CHART[i % 12]);
            text(cot[i].khoa == dt_khoa ? F.m15 : F.r15, cot[i].nhan, lx2 + 24, ly + 3, INK);
            char pc[16]; snprintf(pc, sizeof(pc), "%.1f%%", 100.0 * cot[i].v / (double)tong);
            text_r(F.m13, cot_so == 2 ? std::string(pc) : std::string(pc) + "   " + tien_gon(cot[i].v), lr, MUTED, 8);
        }
        if (hov_s >= 0) {
            std::string t1 = cot[hov_s].nhan, t2 = tien(cot[hov_s].v);
            float tw_ = fmaxf(tw(F.m13, t1), tw(F.b15, t2)) + 28;
            Rectangle tr = { C.mouse.x + 14, C.mouse.y - 56, tw_, 48 };
            rrect(tr, 10, INK);
            text(F.r13, t1, tr.x + 14, tr.y + 7, Color{ 190, 215, 238, 255 });
            text(F.b15, t2, tr.x + 14, tr.y + 25, WHITE);
        }
    }

    // ---- bên phải: top sản phẩm + top nhân viên ----
    float rx = x + cw + 22;
    Rectangle tp = { rx, y, rw, 430 };
    panel(tp, 20);
    text(F.s22, "Top sản phẩm", tp.x + 24, tp.y + 22, INK);
    text(F.r13, ten_k, tp.x + 24, tp.y + 54, MUTED);
    VECTOR<DoanhThuSP> top = dt.top_san_pham(dt_ky, dt_khoa, 5);
    float ty = tp.y + 88;
    for (int i = 0; i < top.size(); i++) {
        float tl = top[0].tong > 0 ? (float)top[i].tong / (float)top[0].tong : 0;
        text(F.m15, std::to_string(i + 1) + ".  " + fit(F.m15, top[i].ten, rw - 150), tp.x + 24, ty, INK);
        text_r(F.m15, tien_gon(top[i].tong), Rectangle{ tp.x, ty - 2, rw, 22 }, SKY600, 24);
        rrect(Rectangle{ tp.x + 24, ty + 26, rw - 48, 8 }, 4, SKY100);
        rrect(Rectangle{ tp.x + 24, ty + 26, (rw - 48) * tl * anim, 8 }, 4, MAU_CHART[i % 12]);
        text(F.r13, std::to_string(top[i].so_luong) + " cái", tp.x + 24, ty + 40, MUTED);
        ty += 62;
    }
    if (top.size() == 0) text(F.r15, "Chưa có dữ liệu bán hàng.", tp.x + 24, tp.y + 100, MUTED);

    float y2 = y + 430 + 22;
    Rectangle np = { x, y2, w, 60 + 6 };
    VECTOR<DoanhThuNV> tnv = dt.top_nhan_vien(dt_ky, dt_khoa, 5);
    np.height = 78 + (tnv.size() > 0 ? tnv.size() * 46.0f : 50.0f) + 12;
    panel(np, 20);
    text(F.s22, "Doanh số nhân viên", np.x + 26, np.y + 22, INK);
    text(F.r13, ten_k + "  -  đơn khách tự mua online không tính cho nhân viên nào", np.x + 26 + 230, np.y + 30, MUTED);
    float ny = np.y + 72;
    for (int i = 0; i < tnv.size(); i++) {
        int in = qn.tim_theo_ma(tnv[i].ma_nv);
        std::string tn = in >= 0 ? qn.lay(in).getTenNV() : tnv[i].ma_nv;
        float tl = tnv[0].tong > 0 ? (float)tnv[i].tong / (float)tnv[0].tong : 0;
        text(F.m15, std::to_string(i + 1) + ".  " + tn + "  (" + tnv[i].ma_nv + ")", np.x + 26, ny + 6, INK);
        rrect(Rectangle{ np.x + 330, ny + 12, np.width - 330 - 190, 10 }, 5, SKY100);
        rrect(Rectangle{ np.x + 330, ny + 12, (np.width - 330 - 190) * tl * anim, 10 }, 5, MAU_CHART[(i + 2) % 12]);
        text_r(F.m15, tien(tnv[i].tong), Rectangle{ np.x, ny, np.width, 34 }, SKY600, 26);
        ny += 46;
    }
    if (tnv.size() == 0) text(F.r15, "Chưa có nhân viên nào bán hàng trong kỳ này.", np.x + 26, np.y + 84, MUTED);
    cao_nd = y2 + np.height + 40 - (view.y - cuon);
}
