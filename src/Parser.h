#pragma once
#include <string>

#include "json.hpp"

using json = nlohmann::json;

class Parser{
public:
    struct PriceLevel {
        std::string type;
        std::string price;
        double quantity;
        PriceLevel(std::string type, std::string price, double quantity): type(type), price(price), quantity(quantity){}
    };
    std::vector<PriceLevel> parseOrderBookUpdate(std::string s);
};