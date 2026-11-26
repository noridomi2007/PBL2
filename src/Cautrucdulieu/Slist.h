#ifndef SLIST_H
#define SLIST_H

// Danh sách liên kết ĐƠN tự cài đặt: mỗi nút giữ 1 giá trị + 1 con trỏ next (chỉ đi được một chiều).
//   - push_front: thêm vào ĐẦU danh sách, O(1)  -> phần tử thêm sau cùng đứng đầu (mới nhất trước).
//   - duyệt từ đầu đến cuối: for (const auto &x : ds) { ... }   (O(n), n = số phần tử)
// Dùng trong dự án: DoanhThu giữ cho mỗi khách 1 danh sách MÃ HÓA ĐƠN (chỉ lưu mã, không lưu cả hóa đơn);
// có mã thì tra hóa đơn bằng bảng băm ~O(1).
template <typename T>
class DSLK_DON {
private:
    struct Node {
        T val;
        Node *next;
        Node(const T &v, Node *n) : val(v), next(n) {}
    };

    Node *head;
    int n;

    void sao_chep(const DSLK_DON &o) {     // giữ nguyên thứ tự
        head = nullptr;
        n = 0;
        Node *cuoi = nullptr;
        for (Node *p = o.head; p; p = p->next) {
            Node *m = new Node(p->val, nullptr);
            if (cuoi) cuoi->next = m; else head = m;
            cuoi = m;
            n++;
        }
    }

public:
    DSLK_DON() : head(nullptr), n(0) {}
    DSLK_DON(const DSLK_DON &o) : head(nullptr), n(0) { sao_chep(o); }
    DSLK_DON &operator=(const DSLK_DON &o) {
        if (this != &o) { clear(); sao_chep(o); }
        return *this;
    }
    ~DSLK_DON() { clear(); }

    void push_front(const T &v) { head = new Node(v, head); n++; }

    const T &front() const { return head->val; }      // chỉ gọi khi !empty()

    bool pop_front() {
        if (!head) return false;
        Node *k = head->next;
        delete head;
        head = k;
        n--;
        return true;
    }

    int  size() const  { return n; }
    bool empty() const { return n == 0; }

    void clear() {
        while (head) {
            Node *k = head->next;
            delete head;
            head = k;
        }
        n = 0;
    }

    // Bộ duyệt tối thiểu để dùng được range-for
    class const_iterator {
        const Node *p;
    public:
        explicit const_iterator(const Node *x) : p(x) {}
        const T &operator*() const { return p->val; }
        const_iterator &operator++() { p = p->next; return *this; }
        bool operator!=(const const_iterator &o) const { return p != o.p; }
    };
    const_iterator begin() const { return const_iterator(head); }
    const_iterator end() const   { return const_iterator(nullptr); }
};

#endif
