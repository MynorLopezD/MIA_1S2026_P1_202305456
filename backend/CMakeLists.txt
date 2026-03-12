cmake_minimum_required(VERSION 3.16)
project(ext2_backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(ext2_backend main.cpp)

target_link_libraries(ext2_backend
    Crow::Crow
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    ws2_32      # Windows sockets
    wsock32
)

# cmake_minimum_required(VERSION 3.10)
# project(CPPDisk)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include(FetchContent)

# FetchContent_Declare(
#     httplib
#     GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
#     GIT_TAG v0.18.0
# )

# set(HTTPLIB_USE_OPENSSL_IF_AVAILABLE OFF)
# set(HTTPLIB_USE_BROTLI_IF_AVAILABLE OFF)
# FetchContent_MakeAvailable(httplib)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# add_executable(cppdisk 
#     src/main.cpp
# )

# target_link_libraries(cppdisk httplib)
