// file_dialog.h
#pragma once
#include <string>
#include <vector>

// Chọn 1 hoặc nhiều ảnh (giữ Ctrl / Shift trong hộp thoại). Trả về các đường dẫn UTF-8,
// rỗng nếu bấm Cancel. toi_da: số ảnh tối đa cho phép lấy.
std::vector<std::string> mo_hop_thoai_chon_anh(int toi_da = 10);