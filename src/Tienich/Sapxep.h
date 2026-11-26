#ifndef SAPXEP_H
#define SAPXEP_H
#include "../Cautrucdulieu/Vector.h"

// Merge sort ổn định O(n log n): chỉ lấy phần tử bên phải khi NÓ đứng trước hẳn.
template <typename T, typename Truoc>
void sap_xep_tron(VECTOR<T> &a, Truoc truoc) {
    int n = a.size();
    if (n < 2) return;
    VECTOR<T> tam;
    for (int i = 0; i < n; i++) tam.push_back(a[i]);
    for (int rong = 1; rong < n; rong *= 2) {
        for (int lo = 0; lo < n; lo += 2 * rong) {
            int mid = lo + rong < n ? lo + rong : n;
            int hi = lo + 2 * rong < n ? lo + 2 * rong : n;
            int i = lo, j = mid, k = lo;
            while (i < mid && j < hi) tam[k++] = truoc(a[j], a[i]) ? a[j++] : a[i++];
            while (i < mid) tam[k++] = a[i++];
            while (j < hi) tam[k++] = a[j++];
        }
        for (int i = 0; i < n; i++) a[i] = tam[i];
    }
}
#endif