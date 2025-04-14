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
### Windows system

1. Install OpenSSL from https://slproweb.com/products/Win32OpenSSL.html (no light version)
2. Clone the folder

       git clone https://github.com/PietroValente/BinanceOrderBook.git
       cd BinanceOrderBook
   
3. BinanceOrderBook -> CMakeLists and set the path to OpenSSL folder

   ...  
   set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64") #set your directory  
   ...  
   
6. Create the build folder

       mkdir build
       cd build
   
7. Build & compile the files:

       cmake ..
       cmake --build .  
  
8. Run the main:

        cd Debug
        .\main  

### Linux/macOS system

1. Install Command Line Developer Tools

       sudo xcode-select --install  

2. Install Homebrew

       /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

3. Install CMake

       brew install cmake
  
4. Install OpenSSL

       brew install openssl
   
5. Clone the folder

       git clone https://github.com/PietroValente/BinanceOrderBook.git
  
3. BinanceOrderBook -> CMakeLists and set the path to OpenSSL folder

   Find the folder of OpenSSL running:

       brew --prefix openssl

    Modify the variable 
   ...  
   set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl@3") #set your directory  
   ...  
   
6. Create the build folder

       mkdir build
       cd build
   
7. Build & compile the files:

       cmake ..
       cmake --build .  
  
8. Run the main:

        ./main  
