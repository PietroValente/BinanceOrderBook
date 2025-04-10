# Binance Order Book
## Overview
The project consists in the local reconstruction of the BTC/USDT order book by leveraging Binance public APIs. The system retrieves real-time market data (depth snapshots and updates) and maintains an efficient local copy of the order book.

The goal of the project is to simulate the behavior of a trading platform's matching engine and provide a reliable and up-to-date view of the market for BTC/USDT.

&nbsp;
<p align="center">
    <a href="https://github.com/PietroValente/BinanceOrderBook/blob/main/images/Binance.png"><img src="https://github.com/PietroValente/BinanceOrderBook/blob/main/images/Binance.png" alt="" width="200px"></a>
    &nbsp;
    &nbsp;
    &nbsp;
        <a href="https://github.com/PietroValente/BinanceOrderBook/blob/main/images/ProgramExecution.png"><img src="https://github.com/PietroValente/BinanceOrderBook/blob/main/images/ProgramExecution.png" alt="" width="200px"></a>
</p>
&nbsp;

## Usability
Windows system

1. Install OpenSSL from https://slproweb.com/products/Win32OpenSSL.html (no light version)
2. BinanceOrderBook -> CMakeLists and set the path to OpenSSL folder
3. Open a terminal in the folder
4. In the build folder -> build and compile the files:

   cmake .. 
   cmake --build .  
  
5. Executive file will be in BinanceOrderBook->build->Debug:

   .\main 
  
