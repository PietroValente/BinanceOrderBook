#include "Parser.h"

std::vector<Parser::PriceLevel> Parser::parseOrderBookUpdate(std::string s){

    std::vector<Parser::PriceLevel> res(2,Parser::PriceLevel());
    json j = json::parse(s);

    if (j.contains("bids")) {
        for (const auto& bid : j["bids"]) {
            if (bid.size() >= 2) {
                res[0].price = bid[0];
                std::string tmp = bid[1];
                res[0].quantity = stod(tmp);
            }
        }
    }

    if (j.contains("asks")) {
        for (const auto& ask : j["asks"]) {
            if (ask.size() >= 2) {
                res[1].price = ask[0];
                std::string tmp = ask[1];
                res[1].quantity = stod(tmp);
            }
        }
    }
    return res;
}