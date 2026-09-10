cmake_minimum_required(VERSION 3.15)
project(OpenSkyADSB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(opensky main.cpp)

target_link_libraries(opensky PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)