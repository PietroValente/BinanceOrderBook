#include <iostream>
#include <iomanip>

#include "Bid.h"

void Bid::insert(std::string price, double quantity){
    bidOrderBook[price] = quantity;
}
void Bid::remove(std::string price){
    bidOrderBook.erase(price);
}
void Bid::showTop5(){
    auto it = bidOrderBook.begin();
    for(int i = 0; it != bidOrderBook.end() && i<5; ++i, ++it){
        double price = stod(it->first);
        std::cout << "  \033[32m" << std::fixed << std::setprecision(2) << price
        << "\033[0m   |   " << std::fixed << std::setprecision(5) << it->second << std::endl;
    }
}