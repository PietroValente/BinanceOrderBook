#include <iostream>
#include <string>
#include <thread>
#include <chrono>

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
        std::string payload = client.getPayload("/api/v3/depth?symbol="+symbol+"&limit=5000");
        std::vector<Parser::PriceLevel> req = p.parseOrderBookUpdate(payload);

        for(int i=0; i<req.size(); ++i){
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
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); //avoid http 418 I'm teapot
    }
    return 0;
}
