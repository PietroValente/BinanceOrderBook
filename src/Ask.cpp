#include <iostream>
#include <iomanip>

#include "Ask.h"

void Ask::insert(std::string price, double quantity){
    askOrderBook[price] = quantity;
}
void Ask::remove(std::string price){
    askOrderBook.erase(price);
}
void Ask::showTop5(){
    int s = askOrderBook.size()>5? static_cast<int>(askOrderBook.size())-5: 0;
    auto it = askOrderBook.rbegin();
    advance(it, s);
    for(int i = 0; it != askOrderBook.rend() && i<5; ++i, ++it){
        double price = stod(it->first);
        std::cout << "  \033[31m" << std::fixed << std::setprecision(2) << price 
        << "\033[0m   |   " << std::fixed << std::setprecision(5) << it->second << std::endl;
    }
}