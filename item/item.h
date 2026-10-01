#ifndef ITEM_H
#define ITEM_H
// các items bao gồm: áo, quần, giày, dép, các phụ kiện khác (mũ, bông tai, dây chuyền, đồng hồ,...)
#include <iostream>
#include <string>
using namespace std;
class product {
protected:
string idsp;
string namesp;
string brandsp;
string color;
string kieusp; //ví dụ áo hay quần hay giày hay dép
double price;
int sl;
public:
product() : price(0), sl(0) {};
product(string idsp, string namesp, string brandsp, string color, string kieusp, double price, int sl) : 
idsp(idsp), namesp(namesp), brandsp(brandsp), color(color), kieusp(kieusp), price(price), sl(sl) {}; 
};
class clothing : public product {
protected:
string size;
string material;
public:
clothing() : product() {};
clothing(string idsp, string namesp, string brandsp, string color, string kieusp, double price, int sl, string size, string material) : 
product(idsp, namesp, brandsp, color, kieusp, price, sl), size(size), material(material){};
};  
class accessory : public product {
protected:
string size;
string material;
string dimension;
public:
accessory(string idsp, string namesp, string brandsp, string color, string kieusp, double price, int sl, string size, string material, string dimension) : 
product(idsp, namesp, brandsp, color, kieusp, price, sl), size(size), material(material), dimension(dimension) {};
};
#endif