#pragma once
#include <map>
#include <string>

class Bid{
public:
    void insert(std::string price, double quantity);
    void remove(std::string price);
    void showTop5();
private:
    std::map<std::string, double, std::greater<>> bidOrderBook;
};