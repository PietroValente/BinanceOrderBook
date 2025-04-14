#include <iostream>

#include "Connection.h"
#include "Parser.h"
#include "Ask.h"
#include "Bid.h"

int main() {
    Bid b;
    Ask a;
    Parser p;
    const std::string symbol = "btcusdt";
    Connection client("stream.binance.com", "9443", symbol);

    if (!client.setup()) {
        std::cerr << "Setup failed\n";
        return 1;
    }

    client.sendWebSocketMessage();

    while(1){
        std::string payload = client.receiveWebSocketMessage();
        std::vector<Parser::PriceLevel> req = p.parseOrderBookUpdate(payload);

        for(int i=0; i<req.size(); ++i){
            //assume all price has same length and use first one as sample
            if(req[i].price.size() != req[0].price.size()) continue;
            if(req[i].type == "ask"){
                if(req[i].quantity == 0){
                    a.remove(req[i].price);
                }
                else{
                    a.insert(req[i].price, req[i].quantity);
                }
            }
            else if(req[i].type == "bid"){
                if(req[i].quantity == 0){
                    b.remove(req[i].price);
                }
                else{
                    b.insert(req[i].price, req[i].quantity);
                }
            }
        }

        //print
        std::cout << "\033[2J\033[1;1H";
        std::cout << "Price (USDT) | Amount (BTC)" << std::endl;
        a.showTop5();
        std::cout << "---------------------------" << std::endl;
        std::cout << "       MARKET PRICE        " << std::endl;
        std::cout << "---------------------------" << std::endl;
        b.showTop5();
    }
    return 0;
}
