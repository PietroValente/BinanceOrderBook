#include "Parser.h"

std::vector<Parser::PriceLevel> Parser::parseOrderBookUpdate(std::string s){

    std::vector<Parser::PriceLevel> res;
    json j = json::parse(s);

    if (j.contains("bids")) {
        for (const auto& bid : j["bids"]) {
            if (bid.size() >= 2) {
                std::string tmp = bid[1];
                res.push_back(Parser::PriceLevel("bid", bid[0], stod(tmp)));
            }
        }
    }

    if (j.contains("asks")) {
        for (const auto& ask : j["asks"]) {
            if (ask.size() >= 2) {
                std::string tmp = ask[1];
                res.push_back(Parser::PriceLevel("ask", ask[0], stod(tmp)));
            }
        }
    }
    return res;
}