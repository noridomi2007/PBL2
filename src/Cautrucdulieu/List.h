#ifndef LIST_H
#define LIST_H

// Danh sách liên kết ĐÔI tự cài đặt: mỗi nút giữ giá trị + con trỏ prev / next.
// head và tail cho push/pop ở HAI đầu đều O(1); chèn/xóa giữa chừng chỉ đổi con trỏ, không dồn phần tử.
// operator[] vẫn dùng được nhưng là O(n) vì phải đi từ đầu/cuối/vị trí vừa truy cập (nút gần nhất).
// Có "con trỏ nhớ" (cur) nên duyệt tuần tự for (i = 0..size) vẫn gần O(1) mỗi bước.
// Muốn duyệt đúng kiểu danh sách liên kết thì dùng range-for:  for (auto &x : ds) { ... }
template <typename T>
class LIST {
private:
    struct Node {
        T val;
        Node *prev, *next;
        Node(const T &v) : val(v), prev(nullptr), next(nullptr) {}
    };

    Node *head, *tail;
    int n;
    mutable Node *cur;     // nút truy cập gần nhất (để duyệt tuần tự nhanh)
    mutable int cur_i;

    // Tìm nút thứ i: đi từ điểm gần nhất trong 3 điểm đầu / cuối / vị trí vừa truy cập
    Node *nut_tai(int i) const {
        if (n == 0) return nullptr;
        if (i < 0) i = 0;
        if (i >= n) i = n - 1;
        Node *p;
        int vi;
        int d_dau = i, d_cuoi = n - 1 - i;
        int d_cur = cur ? (i > cur_i ? i - cur_i : cur_i - i) : n + 1;
        if (d_cur <= d_dau && d_cur <= d_cuoi) { p = cur; vi = cur_i; }
        else if (d_dau <= d_cuoi) { p = head; vi = 0; }
        else { p = tail; vi = n - 1; }
        while (vi < i) { p = p->next; vi++; }
        while (vi > i) { p = p->prev; vi--; }
        cur = p; cur_i = i;
        return p;
    }

    void go_nut(Node *p) {           // tháo nút p khỏi danh sách rồi giải phóng
        if (p->prev) p->prev->next = p->next; else head = p->next;
        if (p->next) p->next->prev = p->prev; else tail = p->prev;
        if (cur == p) { cur = nullptr; cur_i = 0; }
        delete p;
        n--;
    }

public:
    LIST() : head(nullptr), tail(nullptr), n(0), cur(nullptr), cur_i(0) {}

    LIST(const LIST &o) : head(nullptr), tail(nullptr), n(0), cur(nullptr), cur_i(0) {
        for (Node *p = o.head; p; p = p->next) push_back(p->val);
    }

    LIST &operator=(const LIST &o) {
        if (this != &o) {
            clear();
            for (Node *p = o.head; p; p = p->next) push_back(p->val);
        }
        return *this;
    }

    ~LIST() { clear(); }

    void push_back(const T &v) {
        Node *p = new Node(v);
        p->prev = tail;
        if (tail) tail->next = p; else head = p;
        tail = p;
        n++;
    }

    void push_front(const T &v) {
        Node *p = new Node(v);
        p->next = head;
        if (head) head->prev = p; else tail = p;
        head = p;
        n++;
        cur = nullptr;               // chỉ số của mọi nút đã dịch đi 1
    }

    void pop_back()  { if (tail) go_nut(tail); }
    void pop_front() { if (head) { go_nut(head); cur = nullptr; } }

    void remove(int i) {             // xóa phần tử thứ i (như VECTOR::remove)
        if (i < 0 || i >= n) return;
        Node *p = nut_tai(i);
        go_nut(p);
        cur = nullptr;               // chỉ số các nút phía sau đã đổi
    }

    T &operator[](int i)             { return nut_tai(i)->val; }
    const T &operator[](int i) const { return nut_tai(i)->val; }

    int  size() const  { return n; }
    bool empty() const { return n == 0; }

    void clear() {
        Node *p = head;
        while (p) { Node *k = p->next; delete p; p = k; }
        head = tail = nullptr; n = 0; cur = nullptr; cur_i = 0;
    }

    // ---- duyệt bằng range-for ----
    class iterator {
        Node *p;
    public:
        iterator(Node *p) : p(p) {}
        T &operator*() const { return p->val; }
        iterator &operator++() { p = p->next; return *this; }
        bool operator!=(const iterator &o) const { return p != o.p; }
    };
    class const_iterator {
        const Node *p;
    public:
        const_iterator(const Node *p) : p(p) {}
        const T &operator*() const { return p->val; }
        const_iterator &operator++() { p = p->next; return *this; }
        bool operator!=(const const_iterator &o) const { return p != o.p; }
    };
    iterator begin() { return iterator(head); }
    iterator end() { return iterator(nullptr); }
    const_iterator begin() const { return const_iterator(head); }
    const_iterator end() const { return const_iterator(nullptr); }
};

#endif
