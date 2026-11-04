#ifndef HEAP_H
#define HEAP_H

#include "VECTOR.h"

// Max-heap tự viết: phần tử "ưu tiên cao nhất" luôn nằm ở gốc.
// hon(x, y) trả true nếu x phải đứng TRƯỚC y (ví dụ doanh thu của x lớn hơn y).
template <typename T>
class HEAP {
private:
    VECTOR<T> a;
    bool (*hon)(const T&, const T&);

    void doi_cho(int i, int j) { T t = a[i]; a[i] = a[j]; a[j] = t; }

    void len(int i) {           // đẩy phần tử vừa thêm lên đúng chỗ
        while (i > 0) {
            int cha = (i - 1) / 2;
            if (!hon(a[i], a[cha])) break;
            doi_cho(i, cha);
            i = cha;
        }
    }

    void xuong(int i) {         // đẩy phần tử ở gốc xuống đúng chỗ
        int n = a.size();
        while (true) {
            int l = 2 * i + 1, r = l + 1, tot = i;
            if (l < n && hon(a[l], a[tot])) tot = l;
            if (r < n && hon(a[r], a[tot])) tot = r;
            if (tot == i) break;
            doi_cho(i, tot);
            i = tot;
        }
    }

public:
    HEAP(bool (*cmp)(const T&, const T&)) : hon(cmp) {}

    void push(const T &v) { a.push_back(v); len(a.size() - 1); }
    bool empty() const { return a.empty(); }
    int  size() const { return a.size(); }
    const T& top() const { return a[0]; }          // chỉ gọi khi !empty()

    T pop() {                                      // lấy phần tử ưu tiên cao nhất ra; chỉ gọi khi !empty()
        T res = a[0];
        a[0] = a[a.size() - 1];
        a.remove(a.size() - 1);
        if (!a.empty()) xuong(0);
        return res;
    }
};

#endif