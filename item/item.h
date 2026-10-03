#ifndef ITEM_H
#define ITEM_H
// các items bao gồm: áo, quần, giày, dép, các phụ kiện khác (mũ, bông tai, dây chuyền, đồng hồ,...)
#include <iostream>
#include <string>
using namespace std;
class product {
protected:
string subtype;
string idsp;
string namesp;
string brandsp;
string color;
string kieusp; //ví dụ áo hay quần hay giày hay dép
double price;
int sl;
public:
product() : price(0), sl(0) {}; // Hàm dựng mặc định
product(string subtype, string idsp, string namesp, string brandsp, string color, string kieusp, double price, int sl) : 
subtype(subtype), idsp(idsp), namesp(namesp), brandsp(brandsp), color(color), kieusp(kieusp), price(price), sl(sl) {}; // Hàm dựng full tham số 
product(const product &a) : subtype(a.subtype), idsp(a.idsp), namesp(a.namesp), brandsp(a.brandsp), color(a.color), kieusp(a.kieusp), price(a.price), sl(a.sl){}; //Hàm dựng sao chép
virtual ~product(){}; // Hàm hủy
// Mấy cái hàm get
const string& getid() const {return idsp;};
const string& getname() const {return namesp;};
const string& getbrand() const {return brandsp;};
const string& getcolor() const {return color;};
const string& getkieu() const {return kieusp;}; 
const double& getprice() const{return price;};
int getsl() const {return sl;};
// Mấy cái hàm set
bool setname(const string &a) {
    if(a.empty()) return false;
    namesp = a;
    return true;
}
bool setbrand(const string &a) {
    if(a.empty()) return false;
    brandsp = a;
    return true;
}
bool setcolor(const string &a) {
    if(a.empty()) return false;
    color = a;
    return true;
}
bool setprice(const double a) {
    if(a<0) return false;
    price = a;
    return true;
}
bool setsl(const int a) {
    if(a<0) return false;
    sl = a;
    return true;
}
};


class clothing : public product {
protected:
string size;
string material;
public:
clothing() : product() {};
clothing(string idsp, string namesp, string brandsp, string color, string kieusp, double price, int sl, string size, string material) : 
product(subtype, idsp, namesp, brandsp, color, kieusp, price, sl), size(size), material(material){};
clothing(const clothing &a) : product(a), size(a.size), material(a.material){};
~clothing(){};
const string& getsize() const {return size;};
const string& getmaterial() const {return material;};
bool setsize(const string &a) {
    if(a.empty()) return false;
    size = a;
    return true;
}
bool setmaterial(const string &a) {
    if(a.empty()) return false;
    material = a;
    return true;
}
};  


class accessory : public product {
protected:
string size;
string material;
string dimension;
public:
accessory() : product(){};
accessory(string idsp, string namesp, string brandsp, string color, string kieusp, double price, int sl, string size, string material, string dimension) : 
product(subtype, idsp, namesp, brandsp, color, kieusp, price, sl), size(size), material(material), dimension(dimension) {};
accessory(const accessory &a) : product(a), size(a.size), material(a.material), dimension(a.dimension){};
~accessory(){};
const string& getsize() const {return size;};
const string& getmaterial() const {return material;};
const string& getdimension() const {return dimension;};
bool setsize(const string &a) {
    if(a.empty()) return false;
    size = a;
    return true;
}
bool setmaterial(const string &a) {
    if(a.empty()) return false;
    material = a;
    return true;
}
bool setdimension(const string &a) {
    if(a.empty()) return false;
    dimension = a;
    return true;
}
};
#endif