cmake_minimum_required(VERSION 3.16)
project(ext2_backend)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(ext2_backend ${SOURCES})

target_link_libraries(ext2_backend
    Crow::Crow
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    ws2_32
    wsock32
)