add_library(src_functions
    Connection.cpp
    Parser.cpp
    Ask.cpp
    Bid.cpp
    )

target_include_directories(src_functions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(src_functions PRIVATE OpenSSL::SSL OpenSSL::Crypto)