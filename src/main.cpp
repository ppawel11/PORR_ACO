#include <iostream>
#include "../include/Graph/graph.h"

int main() {
    auto graph = readGraphFromFile("../config/small_network.txt");

    for( int i = 1; i<7; ++i)
    {
        auto neigh = graph->getNeighbours(i);
        std::cout<<i<<": ";
        for( const auto& n : neigh )
        {
            std::cout<<n->getId()<<" ";
        }
        std::cout<<std::endl;
    }

    return 0;
}
