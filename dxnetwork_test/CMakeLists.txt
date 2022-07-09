cmake_minimum_required(VERSION 3.21)
project(dxnetwork_test)

set(CMAKE_CXX_STANDARD 20)

add_library(dxnetwork SHARED ../dxnetwork/dxnetwork_factory.cpp ../dxnetwork/dxnetwork_transfer.cpp)

add_executable(dxnetwork_test dxnetwork_test.cpp)
