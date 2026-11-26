// ĐIỂM VÀO CHƯƠNG TRÌNH
//   Biên dịch:  mingw32-make      Chạy:  .\app.exe   (đứng ở thư mục gốc PBL2 để đọc Data, Assets)
#include "raylib.h"
#include "Giaodien/Khung/Ui.h"
#include "Giaodien/App.h"
#include "Giaodien/Cauhinh.h"

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1360, 840, (std::string(TEN_SHOP) + " - Cửa hàng thời trang").c_str());
    SetWindowMinSize(1100, 700);
    SetTargetFPS(120);
    ui::load_fonts();
    ui::init_input();          // móc GLFW: nhận phím / chữ đúng thứ tự (Unikey)
    {
        App app;
        app.run();
    }
    ui::anh_giai_phong();
    ui::unload_fonts();
    CloseWindow();
    return 0;
}
