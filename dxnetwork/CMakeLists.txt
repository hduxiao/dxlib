cmake_minimum_required(VERSION 3.21)
project(dxnetwork)

set(CMAKE_CXX_STANDARD 20)

add_library(dxnetwork SHARED dxnetwork_factory.cpp dxnetwork_transfer.cpp)
