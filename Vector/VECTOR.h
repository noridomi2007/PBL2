#ifndef VECTOR_H
#define VECTOR_H

template <typename T>
class VECTOR {
    private:
    T* data;
    int n, cap;
    void reserve(int newcap) {
        T* t = new T[newcap];
        for (int i = 0; i < n; i++) t[i] = data[i];
        delete[] data;
        data = t;
        cap =newcap;
    }
public:
    VECTOR() : data(nullptr), n(0), cap(0) {}

    VECTOR(const VECTOR& o) : data(nullptr), n(0), cap(0) {
        if (o.n > 0) {
            reserve(o.n);
            for (int i = 0; i < o.n; i++) data[i] = o.data[i];
            n = o.n;
        }
    }

VECTOR& operator=(const VECTOR& o) {
    if (this != &o) {
        delete[] data;                    
        n = o.n;
        cap = o.cap;
        data = (cap > 0) ? new T[cap] : nullptr; 
        for (int i = 0; i < n; i++) data[i] = o.data[i];
    }
    return *this;
}

    ~VECTOR() { delete[] data; }

    void push_back(const T& v) {
        if (n == cap) reserve(cap == 0 ? 4 : cap * 2);
        data[n++] = v;
    }

    void remove(int i) {                         
        if (i < 0 || i >= n) return;
        for (int j = i; j < n - 1; j++) data[j] = data[j + 1];
        n--;
    }

    T& operator[](int i)             { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    int size() const                 { return n; }
    int maxsize() const              {return max; };
    bool empty() const               { return n == 0; }
    void clear()                     { n = 0; }   
};
#endif