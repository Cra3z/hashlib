cmake_minimum_required(VERSION 3.22)
project(PackageTest CXX)

find_package(hashlib CONFIG REQUIRED)

add_executable(example src/example.cpp)

target_link_libraries(example hashlib::hashlib)