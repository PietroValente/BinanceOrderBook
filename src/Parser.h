#pragma once
#include <string>

#include "json.hpp"

using json = nlohmann::json;

class Parser{
public:
    struct PriceLevel {
        std::string price;
        double quantity;
        PriceLevel(): price(""), quantity(-1){}
    };
    std::vector<PriceLevel> parseOrderBookUpdate(std::string s);
};