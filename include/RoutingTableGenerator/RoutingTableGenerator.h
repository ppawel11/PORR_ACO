#ifndef PROJEKT_ROUTINGTABLEGENERATOR_H
#define PROJEKT_ROUTINGTABLEGENERATOR_H

#include "../Algorithms/algorithm.h"

using RoutingPair = std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>;

class RoutingTableGenerator {
    std::shared_ptr<Graph> graph;
    std::shared_ptr<Algorithm> algorithm;
public:
    RoutingTableGenerator(std::shared_ptr<Graph> graph, std::shared_ptr<Algorithm> algorithm): graph{graph}, algorithm{algorithm} {};

    std::vector<RoutingPair> generateRoutingTableTo(const int server_id);
};

#endif //PROJEKT_ROUTINGTABLEGENERATOR_H
