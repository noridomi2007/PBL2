#ifndef HASH_H
#define HASH_H

#include <string>

// Bảng băm tự cài đặt, khóa là chuỗi, giải quyết đụng độ bằng NỐI CHUỖI (mỗi ô là 1 danh sách liên kết).
//   - Hàm băm: djb2 (h = h * 33 + ký tự, bắt đầu từ 5381), lấy dư theo số ô.
//   - Tự tăng số ô gấp đôi và băm lại khi hệ số tải (n / cap) vượt 0.75, nên tra cứu luôn ~O(1).
// Dùng trong dự án: tra hóa đơn theo mã HDxxxxxx, và tìm các hóa đơn của một khách mà không phải quét cả sổ.
template <typename V>
class HASHTABLE {
private:
    struct Node {
        std::string key;
        V val;
        Node *next;
        Node(const std::string &k, const V &v, Node *n) : key(k), val(v), next(n) {}
    };

    Node **bucket;
    int cap, n;

    static unsigned long long bam(const std::string &s) {
        unsigned long long h = 5381;                       // djb2
        for (unsigned char c : s) h = h * 33 + c;
        return h;
    }
    int o_cua(const std::string &k) const { return (int)(bam(k) % (unsigned long long)cap); }

    void khoi_tao(int c) {
        cap = c < 8 ? 8 : c;
        bucket = new Node *[cap];
        for (int i = 0; i < cap; i++) bucket[i] = nullptr;
        n = 0;
    }

    void giai_phong() {
        for (int i = 0; i < cap; i++) {
            Node *p = bucket[i];
            while (p) { Node *k = p->next; delete p; p = k; }
        }
        delete[] bucket;
        bucket = nullptr; cap = 0; n = 0;
    }

    void bam_lai(int cap_moi) {       // chuyển các nút sang mảng ô mới (không cấp phát lại nút)
        Node **cu = bucket;
        int cap_cu = cap;
        cap = cap_moi;
        bucket = new Node *[cap];
        for (int i = 0; i < cap; i++) bucket[i] = nullptr;
        for (int i = 0; i < cap_cu; i++) {
            Node *p = cu[i];
            while (p) {
                Node *k = p->next;
                int o = o_cua(p->key);
                p->next = bucket[o];
                bucket[o] = p;
                p = k;
            }
        }
        delete[] cu;
    }

    Node *tim_nut(const std::string &k) const {
        for (Node *p = bucket[o_cua(k)]; p; p = p->next) if (p->key == k) return p;
        return nullptr;
    }

public:
    HASHTABLE(int cap_ban_dau = 16) : bucket(nullptr), cap(0), n(0) { khoi_tao(cap_ban_dau); }

    HASHTABLE(const HASHTABLE &o) : bucket(nullptr), cap(0), n(0) {
        khoi_tao(o.cap);
        for (int i = 0; i < o.cap; i++)
            for (Node *p = o.bucket[i]; p; p = p->next) put(p->key, p->val);
    }

    HASHTABLE &operator=(const HASHTABLE &o) {
        if (this != &o) {
            giai_phong();
            khoi_tao(o.cap);
            for (int i = 0; i < o.cap; i++)
                for (Node *p = o.bucket[i]; p; p = p->next) put(p->key, p->val);
        }
        return *this;
    }

    ~HASHTABLE() { giai_phong(); }

    // Thêm mới, hoặc ghi đè nếu khóa đã có
    void put(const std::string &k, const V &v) {
        Node *p = tim_nut(k);
        if (p) { p->val = v; return; }
        if ((n + 1) * 4 > cap * 3) bam_lai(cap * 2);
        int o = o_cua(k);
        bucket[o] = new Node(k, v, bucket[o]);
        n++;
    }

    V *find(const std::string &k)             { Node *p = tim_nut(k); return p ? &p->val : nullptr; }
    const V *find(const std::string &k) const { Node *p = tim_nut(k); return p ? &p->val : nullptr; }
    bool contains(const std::string &k) const { return tim_nut(k) != nullptr; }

    // Lấy giá trị theo khóa; chưa có thì tạo mới với giá trị mặc định (tiện cho V là danh sách)
    V &operator[](const std::string &k) {
        Node *p = tim_nut(k);
        if (p) return p->val;
        put(k, V());
        return tim_nut(k)->val;
    }

    bool erase(const std::string &k) {
        int o = o_cua(k);
        Node *truoc = nullptr;
        for (Node *p = bucket[o]; p; truoc = p, p = p->next) {
            if (p->key == k) {
                if (truoc) truoc->next = p->next; else bucket[o] = p->next;
                delete p;
                n--;
                return true;
            }
        }
        return false;
    }

    int  size() const  { return n; }
    bool empty() const { return n == 0; }
    int  so_o() const  { return cap; }
    void clear() { int c = cap; giai_phong(); khoi_tao(c); }

    // Độ dài chuỗi nối dài nhất (để đánh giá hàm băm có phân bố đều không)
    int chuoi_dai_nhat() const {
        int best = 0;
        for (int i = 0; i < cap; i++) {
            int d = 0;
            for (Node *p = bucket[i]; p; p = p->next) d++;
            if (d > best) best = d;
        }
        return best;
    }
};

#endif
