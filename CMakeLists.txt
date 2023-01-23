cmake_minimum_required(VERSION 3.17)
project(Projekt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fopenacc -fopt-info-optimized-omp -fcf-protection=none -fno-stack-protector")
#set(CMAKE_CXX_FLAGS -fopt-info-optimized-omp)

#set(CMAKE_CXX_STANDARD 11)

#SET(CMAKE_CXX_COMPILER /opt/nvidia/hpc_sdk/Linux_x86_64/22.11/compilers/bin/pgc++)
#set(CMAKE_CXX_FLAGS
#   "${SSE_FLAGS} -std=c++11 -g -O3 -acc -Minfo=acc -Mcudalib=curand -ta=tesla,nollvm"
#)

add_executable(Projekt
        src/main.cpp
        src/Graph/graph.cpp
        src/RoutingTableGenerator/RoutingTableGenerator.cpp
        src/Algorithms/aco.cpp
        src/Algorithms/aco_omp.cpp
        src/Algorithms/aco_acc.cpp
        include/Algorithms/aco.h
        include/Algorithms/aco_omp.h
        include/Algorithms/algorithm.h
        include/Graph/graph.h
        include/RoutingTableGenerator/RoutingTableGenerator.h
        include/Algorithms/aco_acc.h
        include/Algorithms/aco_changed.h
        src/Algorithms/aco_changed.cpp)
