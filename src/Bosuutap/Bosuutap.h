#ifndef BOSUUTAP_H
#define BOSUUTAP_H
#include <string>
#include "../Cautrucdulieu/Vector.h"
using namespace std;

const char *const FILE_BO_SUU_TAP = "Data/bosuutap.txt";
const int RONG_BST_IDSP = 9;    // SPCM0001A
const int RONG_BST_TEN  = 28;   // tên bộ sưu tập, cũng là số ký tự tối đa khi nhập

enum KetQuaBST { BST_OK, BST_KY_TU_CAM, BST_QUA_DAI, BST_LOI_GHI_FILE };

// BỘ SƯU TẬP: mỗi sản phẩm thuộc nhiều nhất 1 bộ sưu tập ("Thu Đông 2026", "Thể thao"...).
// Lưu RIÊNG ở bosuutap.txt (| idsp | bosuutap |) nên không đụng gì tới item.txt.
// Sản phẩm không có dòng nào trong file = chưa thuộc bộ sưu tập nào.
class BoSuuTap {
private:
    struct Dong { string idsp, ten; };
    VECTOR<Dong> ds;
    string file;
public:
    BoSuuTap(const string &duong_dan = FILE_BO_SUU_TAP) : file(duong_dan) {}

    bool doc_file();                 // chưa có file thì rỗng (true); dòng hỏng bị bỏ qua
    bool ghi_file() const;           // ghi file tạm rồi đổi tên, lỗi giữa chừng không mất file cũ

    string cua(const string &idsp) const;                  // "" nếu không thuộc bộ sưu tập nào
    // Đưa sản phẩm vào bộ sưu tập; ten rỗng = gỡ khỏi bộ sưu tập. Lỗi ghi file thì hoàn lại.
    KetQuaBST dat(const string &idsp, const string &ten);
    VECTOR<string> ds_ten() const;                         // các tên bộ sưu tập (không trùng, theo thứ tự xuất hiện)
    int dem(const string &ten) const;                      // số sản phẩm trong bộ sưu tập
};

#endif
