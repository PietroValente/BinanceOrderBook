#include <iostream>
#include <string>

#include "Connection.h"
#include "Parser.h"
#include "Ask.h"
#include "Bid.h"

int main() {
    Bid b;
    Ask a;
    Parser p;
    std::string symbol = "BTCUSDT";
    Connection client("api.binance.com");

    if (!client.setup()) {
        std::cerr << "Setup failed\n";
        return 1;
    }

    while(1){
        std::string json = client.getJson("/api/v3/depth?symbol="+symbol+"&limit=1");
        std::vector<Parser::PriceLevel> req = p.parseOrderBookUpdate(json);

        if(req[0].quantity == 0){ //process bid
            b.remove(req[0].price);
        }
        else if(req[0].quantity != -1){
            b.insert(req[0].price, req[0].quantity);
        }
        if(req[1].quantity == 0){ //process ask
            a.remove(req[1].price);
        }
        else if(req[1].quantity != -1){
            a.insert(req[1].price, req[1].quantity);
        }

        //print
        std::cout << "\033[2J\033[1;1H";
        std::cout << "Price (USDT) | Amount (BTC)" << std::endl;
        a.showTop5();
        std::cout << "---------------------------" << std::endl;
        std::cout << "       MARKET PRICE        " << std::endl;
        std::cout << "---------------------------" << std::endl;
        b.showTop5();
        std::cout << "---------------------------" << std::endl;
        std::cout << req[1].price << " a " << req[1].quantity << std::endl;
        std::cout << req[0].price << " b " << req[0].quantity << std::endl;
    }
    return 0;
}
