cmake_minimum_required(VERSION 3.16)
project(ext2_backend)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ASIO_INCLUDE_DIR "C:/msys64/ucrt64/include")
set(OPENSSL_ROOT_DIR "C:/msys64/ucrt64")

find_package(Crow REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(ext2_backend ${SOURCES})

target_compile_definitions(ext2_backend PRIVATE
    DBL_DECIMAL_DIG=17
)

target_include_directories(ext2_backend PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${ASIO_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(ext2_backend
    Crow::Crow
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    ws2_32
    wsock32
)