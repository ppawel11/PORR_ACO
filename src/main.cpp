#include <iostream>
#include "../include/RoutingTableGenerator/RoutingTableGenerator.h"
#include "../include/Algorithms/aco.h"

int main() {
    srand(0);

    auto graph = readGraphFromFile("../config/small_network.txt");
    std::shared_ptr<Algorithm> algorithm = std::make_shared<ACO>(10, 10, 0.5, 0.5, 0.5);

    RoutingTableGenerator routing_table_generator(graph, algorithm);

    auto routing_pairs = routing_table_generator.generateRoutingTableTo(3);

    for( const auto& routing_pair : routing_pairs)
    {
        std::cout<<routing_pair.first->getId()<<" -> "<<routing_pair.second->getId()<<std::endl;
    }

    return 0;
}
