# CẤU TRÚC PROJECT PBL2

## 1. Build và chạy
```
mingw32-make clean
mingw32-make          ->  app.exe   (giao diện đồ họa)
.\app.exe             (chạy ở thư mục PBL2 này, để app tìm thấy Data/ và Assets/)

mingw32-make test     ->  test.exe  (bản terminal, chỉ để kiểm thử logic)
```

## 2. Sơ đồ thư mục
```
PBL2/
├─ Makefile
├─ src/                     <- TOÀN BỘ MÃ NGUỒN
│  ├─ Main.cpp              điểm vào chương trình
│  ├─ Cautrucdulieu/        cấu trúc dữ liệu tự cài: Vector, Hash, Heap, List, Slist
│  ├─ Tienich/              hàm dùng chung: xử lý tiếng Việt, ngày giờ, định dạng bảng, stb_image
│  ├─ Item/                 sản phẩm (quần áo, phụ kiện) + biến thể màu/size
│  ├─ Shop/                 cửa hàng: danh sách sản phẩm, tìm kiếm, thanh toán, nhập kho
│  ├─ Hoadon/               hóa đơn
│  ├─ Doanhthu/             sổ doanh thu: thống kê theo tuần/tháng/quý/năm, top sản phẩm/nhân viên
│  ├─ Customer/             khách hàng, hạng thành viên, quản lý khách hàng
│  ├─ Staff/                nhân viên + admin, quản lý nhân viên
│  ├─ Bosuutap/             bộ sưu tập (mỗi sản phẩm thuộc tối đa 1 bộ sưu tập)
│  ├─ Boloc/                bộ lọc + sắp xếp sản phẩm cho cửa hàng
│  ├─ Dialog/               hộp thoại chọn file ảnh của Windows
│  └─ Giaodien/             GIAO DIỆN RAYLIB
│     ├─ Khung/             khung dùng chung: Ui.h, Chu.cpp, Nhaplieu.cpp, Ve.cpp
│     ├─ App.h / App.cpp    lớp App, vòng lặp chính, đăng nhập / đăng ký
│     ├─ Cuahang.cpp        khu của khách
│     ├─ Khulamviec.cpp     khu nhân viên / admin
│     ├─ Bieudo.cpp         doanh thu: biểu đồ cột / tròn (chỉ admin)
│     ├─ Hopthoai.cpp       các hộp thoại
│     └─ Cauhinh.h          tên cửa hàng, khẩu hiệu
├─ Test/                    bản kiểm thử trên terminal
├─ Data/                    file dữ liệu .txt (sản phẩm, khách, nhân viên, doanh thu, bộ sưu tập)
├─ Assets/
│  ├─ Images/               ảnh sản phẩm (SPCM0002A_1.png ...)
│  └─ Fonts/                font chữ (giao diện dùng Roboto và Times)
├─ Raylib/
│  ├─ Include/              raylib.h, rlgl.h ...
│  └─ Lib/                  libraylib.a ...
└─ Docs/                    tài liệu (file này, nm_out.txt)
```

## 3. Đã đổi tên gì
| Cũ | Mới | Lý do |
|---|---|---|
| `Vectorandheap/` | `src/Cautrucdulieu/` | tên nói đúng nội dung: các cấu trúc dữ liệu |
| `librarytucode/` | `src/Tienich/` | là các hàm tiện ích dùng chung |
| `loc/` | `src/Boloc/` | bộ lọc |
| `main/` | `src/Giaodien/` | chứa giao diện, không chỉ hàm main |
| `Hoandon.cpp` | `Hoadon.cpp` | sửa lỗi chính tả, khớp với `Hoadon.h` |
| `hangtv.*` | `Hangthanhvien.*` | "tv" khó hiểu |
| `ui_text / ui_input / ui_draw` | `Chu / Nhaplieu / Ve` | nói đúng việc: chữ / nhập liệu / vẽ |
| `app_core` | `App.cpp` | |
| `app_shop` | `Cuahang.cpp` | khu của khách |
| `app_work` | `Khulamviec.cpp` | khu nhân viên / admin |
| `app_revenue` | `Bieudo.cpp` | biểu đồ doanh thu |
| `app_modal` | `Hopthoai.cpp` | các hộp thoại |
| `config.h` | `Cauhinh.h` | |
| `Filetxt/` | `Data/` | |
| `Images/`, `font/` | `Assets/Images/`, `Assets/Fonts/` | gom tài nguyên |
| `include/`, `lib/` | `Raylib/Include/`, `Raylib/Lib/` | gom thư viện ngoài |
| `test/test_main.cpp` | `Test/Test.cpp` | |

Quy ước: tên thư mục và file mã nguồn chỉ viết hoa chữ cái đầu. Tên class, hàm, biến trong code KHÔNG đổi.
Các đường dẫn `#include` và đường dẫn dữ liệu trong code đã được sửa theo.
Tên class / biến vẫn viết hoa như cũ (VECTOR, HASHTABLE...) nên file `Vector.h` vẫn chứa class `VECTOR`.

## 4. Những thay đổi so với bản cũ (logic, không phải đổi tên)
- `Doanhthu`: thêm đếm "đã bán" theo sản phẩm; sửa lỗi đọc tràn mảng ở `tieu_de_so()`; nhận dòng tiêu đề tên cột nào cũng được.
- `Shop`: thêm `nhap_kho`, `kiem_ke`, `dat_gia`.
- Mới hoàn toàn: `Bosuutap/`, `Boloc/`, `Giaodien/`.

## 5. Phân quyền
- **Khách:** cửa hàng, giỏ, đặt hàng, tài khoản của mình.
- **Nhân viên:** bán tại quầy, tra / đăng ký khách, xem tồn kho, xem doanh số cá nhân.
- **Admin:** mọi thứ của nhân viên + doanh thu (biểu đồ), thêm / xóa / nhập kho sản phẩm, bộ sưu tập, khóa khách, quản lý nhân viên.
- Admin mặc định: `NV0001` / `admin123`. Nhân viên mới có mật khẩu mặc định là số điện thoại.

## 6. Phóng to cửa sổ
Thiết kế gốc 1360x840. `begin_frame()` trong `src/Giaodien/Khung/Nhaplieu.cpp` tính
`scale = min(rộng/1360, cao/840)` và nhân toàn bộ hình vẽ theo tỉ lệ đó.
