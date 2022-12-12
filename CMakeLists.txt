cmake_minimum_required(VERSION 3.17)
project(Projekt)

set(CMAKE_CXX_STANDARD 20)

add_executable(Projekt
        src/main.cpp
        src/Graph/graph.cpp
        src/RoutingTableGenerator/RoutingTableGenerator.cpp
        src/Algorithms/dumb_algorithm.cpp
        )