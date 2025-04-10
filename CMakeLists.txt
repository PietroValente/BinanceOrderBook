cmake_minimum_required(VERSION 3.10)
project(Straylight-Research)

set(CMAKE_CXX_STANDARD 20)
set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")

find_package(OpenSSL REQUIRED)

add_subdirectory(src)

add_executable(main main.cpp)

target_include_directories(main PRIVATE ${OPENSSL_INCLUDE_DIR})

target_link_libraries(main PRIVATE src_functions)
target_link_libraries(main PRIVATE OpenSSL::SSL OpenSSL::Crypto)