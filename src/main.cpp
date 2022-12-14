#include <iostream>
#include <cstring>
#include "../include/RoutingTableGenerator/RoutingTableGenerator.h"
#include "../include/Algorithms/aco_omp.h"

int main(int argc, char** argv) {
    /**
     * arg[0] -> program
     * arg[1] -> algorithm choice:
     *  - basic -> no parallelization
     *  - omp -> OpenMP
     *
     * arg[2] -> threads number
     */
    srand(0);

    auto graph = readGraphFromFile("../config/grid20x20.txt");

    std::shared_ptr<Algorithm> algorithm = nullptr;
    if(std::strcmp(argv[1], "basic") == 0){
        algorithm = std::make_shared<ACO>(8, 1000, 16, 0.5, 0.5, 0.5);
    }
    else if(std::strcmp(argv[1], "omp") == 0){
        algorithm = std::make_shared<ACO_OMP>(8, 1000, 16, 0.5, 0.5, 0.5);
    }
    else{
        std::cout << "Provided wrong algorithm version!" << std::endl;
    }

    RoutingTableGenerator routing_table_generator(graph, algorithm);

    auto routing_pairs = routing_table_generator.generateRoutingTableTo(3);

    for( const auto& routing_pair : routing_pairs)
    {
        std::cout<<routing_pair.first->getId()<<" -> "<<routing_pair.second->getId()<<std::endl;
    }

    return 0;
}
