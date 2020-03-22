project(srep)
cmake_minimum_required(VERSION 3.1.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTING "build unit tests" OFF)

find_package(nlohmann_json 3.7.3 REQUIRED)
include_directories(nlohmann_json_INCLUDE_DIRS)
link_directories(nlohmann_json_LIBRARY_DIRS)

find_package(Boost 1.72 REQUIRED COMPONENTS system)
include_directories(Boost_INCLUDE_DIRS)
link_directories(Boost_LIBRARY_DIRS)

find_package(OpenSSL REQUIRED COMPONENTS SSL)

add_subdirectory(src)

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
