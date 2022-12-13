#include <iostream>
#include "../include/RoutingTableGenerator/RoutingTableGenerator.h"
#include "../include/Algorithms/aco_omp.h"

int main() {
    srand(0);

    auto graph = readGraphFromFile("../config/grid20x20.txt");
    std::shared_ptr<Algorithm> algorithm = std::make_shared<ACO_OMP>(8, 1000, 16, 0.5, 0.5, 0.5);

    RoutingTableGenerator routing_table_generator(graph, algorithm);

    auto routing_pairs = routing_table_generator.generateRoutingTableTo(3);

    for( const auto& routing_pair : routing_pairs)
    {
        std::cout<<routing_pair.first->getId()<<" -> "<<routing_pair.second->getId()<<std::endl;
    }

    return 0;
}
