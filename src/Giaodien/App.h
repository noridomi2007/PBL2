#pragma once
// ỨNG DỤNG CỬA HÀNG: 3 vùng giao diện tách bạch
//   KHÁCH  : cửa hàng (bố cục kiểu LV + Shopee): duyệt, lọc, giỏ hàng, đặt hàng, tài khoản
//   NHÂN VIÊN: khu làm việc có thanh bên: bán tại quầy, khách hàng, xem tồn kho, doanh số của mình
//   ADMIN  : như nhân viên + doanh thu (biểu đồ), sản phẩm / nhập kho / bộ sưu tập, quản lý khách và nhân viên
#include "Khung/Ui.h"
#include "Cauhinh.h"
#include "../Shop/Shop.h"
#include "../Customer/Quanlykhachhang.h"
#include "../Customer/Hangthanhvien.h"
#include "../Staff/Quanlynhanvien.h"
#include "../Bosuutap/Bosuutap.h"
#include "../Boloc/Boloc.h"
#include "../Tienich/Tienich.h"
#include "../Tienich/Tiengviet.h"

enum Screen {
    S_LOGIN, S_REGISTER,
    S_HOME, S_DETAIL, S_CART, S_ACCOUNT,                   // khách
    S_POS, S_CUSTOMERS, S_STOCK, S_MYSALES,                // nhân viên (+ admin)
    S_REVENUE, S_PRODUCTS, S_STAFFS,                       // chỉ admin
    S_MATKHAU_NV                                           // đổi mật khẩu (nhân viên + admin)
};
enum Role { R_GUEST, R_CUSTOMER, R_STAFF, R_ADMIN };
enum Modal {
    M_NONE, M_INVOICE, M_NEED_LOGIN, M_PICK_VARIANT, M_ADD_PRODUCT, M_STOCK_IN, M_EDIT_PRODUCT, M_DELETE_PRODUCT,
    M_NEW_CUSTOMER, M_NEW_STAFF, M_CUSTOMER_INFO, M_CONFIRM_LOGOUT
};
enum ChartMode { CH_BAR, CH_PIE };

struct NavItem { Screen s; const char *label; ui::Icon ic; };

class App {
public:
    App();
    void run();

private:
    // ---------- dữ liệu ----------
    Shop shop;
    QuanLyKhachHang qk;
    QuanLyNhanVien qn;
    BoSuuTap bst;

    // ---------- phiên ----------
    Role role = R_GUEST;
    std::string ma, ten;               // mã + tên người đang đăng nhập
    Screen screen = S_HOME, quay_lai = S_HOME;
    Modal modal = M_NONE;
    bool menu_mo = false;

    // ---------- cuộn ----------
    float cuon = 0, cuon_dich = 0, cao_nd = 0;
    void cuon_cap_nhat(Rectangle vung);
    void doi_man(Screen s);

    // ---------- đăng nhập / đăng ký ----------
    int tab_dn = 0;                     // 0 khách, 1 nhân viên / admin
    ui::Field f_sdt, f_manv, f_mk;
    ui::Field f_dk_ten, f_dk_sdt, f_dk_mail, f_dk_dc, f_dk_mk, f_dk_mk2;
    std::string thong_bao_dn;

    // ---------- cửa hàng ----------
    std::string tim_nhap_cu;
    ui::Field f_tim, f_gia_min, f_gia_max;
    TieuChiLoc tc;
    VECTOR<KetQuaLoc> kq;
    bool can_loc = true;
    bool xem_them_hang = false;
    std::string ma_ct;                  // sản phẩm đang xem chi tiết
    int ct_anh = 0, ct_sl = 1;
    std::string ct_mau, ct_size;
    GioHang gio;
    HoaDon hd_cuoi;
    int tk_tab = 0;                     // tab trang tài khoản
    std::string hd_mo;                  // hóa đơn đang mở rộng
    ui::Field f_mk_cu, f_mk_moi, f_mk_moi2;

    void tinh_loc();
    void xoa_loc();
    void ve_cua_hang();
    void ve_dau_trang();
    void ve_trang_chu(Rectangle vung);
    float hero_y = 0;
    float ve_bo_loc(float x, float y, float w);
    float bl_cao = 700;
    float ve_luoi(float x, float y, float w);
    void ve_the_sp(Rectangle r, const KetQuaLoc &k);
    void ve_chi_tiet(Rectangle vung);
    void ve_gio_hang(Rectangle vung);
    void ve_tai_khoan(Rectangle vung);
    float ve_chan_trang(float y, float x, float w);
    void them_vao_gio(bool mua_ngay);
    void mo_chi_tiet(const std::string &id);
    void dat_hang();
    long long chi_tieu_khach();
    int chi_n = -1; long long chi_v = 0;

    // ---------- đăng nhập ----------
    void ve_dang_nhap();
    void ve_dang_ky();
    void dang_nhap_khach();
    void dang_nhap_nv();
    void dang_xuat();

    // ---------- khu làm việc ----------
    std::vector<NavItem> menu_lam_viec() const;
    void ve_khu_lam_viec();
    void ve_thanh_ben();
    std::string tieu_de_man() const;
    void ve_pos(Rectangle vung);
    void ve_khach_hang(Rectangle vung);
    void ve_kho(Rectangle vung, bool admin);
    void ve_doanh_so_cua_toi(Rectangle vung);
    void ve_nhan_vien(Rectangle vung);
    void ve_doanh_thu(Rectangle vung);
    void ve_mat_khau_nv(Rectangle vung);

    // POS
    ui::Field f_pos_tim, f_pos_sdt, f_pos_ngay;
    std::string pos_loai;
    int pos_kh = -1;
    GioHang gio_pos;
    std::string pk_ma, pk_mau, pk_size; int pk_sl = 1;      // hộp chọn biến thể
    std::string pos_q_cu, pos_loai_cu;
    VECTOR<KetQuaLoc> pos_kq;
    bool pos_dirty = true;
    ui::Field f_pw_cu, f_pw_moi, f_pw_moi2;

    // kho / sản phẩm
    ui::Field f_kho_tim;
    std::string kho_q_cu;
    VECTOR<KetQuaLoc> kho_kq;
    int kho_n_cu = -1;
    bool kho_dirty = true;
    std::string kho_mo;                                      // sản phẩm đang mở rộng
    std::string md_ma;                                       // sản phẩm đang thao tác trong hộp thoại
    int nk_che_do = 0;                                       // 0 nhập thêm, 1 kiểm kê
    ui::Field f_nk_mau, f_nk_size, f_nk_sl;
    ui::Field f_sp_gia, f_sp_bst;
    std::string cung_ma_cu; int cung_n_cu = -1;
    VECTOR<std::string> cung_ids;
    // thêm sản phẩm
    int ap_hang = 0, ap_kieu = 0;
    ui::Field f_ap_hang_khac, f_ap_ten, f_ap_kieu_ct, f_ap_gia, f_ap_chat_lieu, f_ap_bst, f_ap_kich_thuoc;
    ui::Field f_ap_mau[6], f_ap_size[6], f_ap_sl[6];
    std::vector<std::string> ap_anh;
    int ap_rows = 1;
    float md_cuon = 0, md_cao = 0;
    void mo_them_sp();
    void gui_them_sp();

    // khách hàng / nhân viên
    ui::Field f_kh_tim;
    std::string kh_xem;                                      // mã khách đang xem chi tiết
    ui::Field f_nk_ten, f_nk_sdt, f_nk_mail, f_nk_dc, f_nk_mk;    // đăng ký khách ở quầy
    ui::Field f_nv_ten, f_nv_sdt;                            // tạo nhân viên
    int nv_vai_tro = 0;

    // doanh thu
    KieuKy dt_ky = KY_THANG;
    long long dt_khoa = 0;
    ChartMode dt_kieu = CH_BAR;
    ui::Field f_dt_ngay;
    double dt_anim0 = 0;
    void dt_doi_ky(KieuKy k, long long khoa);

    // ---------- hộp thoại ----------
    void ve_hop_thoai();
    bool khung_hop_thoai(Rectangle &r, float w, float h, const std::string &tieu_de);
    void ve_hd_modal();
    void ve_chon_bien_the();
    void ve_them_sp_modal();
    void ve_nhap_kho_modal();
    void ve_sua_sp_modal();
    void ve_xoa_sp_modal();
    void ve_khach_moi_modal();
    void ve_nv_moi_modal();
    void ve_khach_info_modal();
    void ve_can_dang_nhap();

};
