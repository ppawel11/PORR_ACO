#ifndef PROJEKT_ALGORITHM_H
#define PROJEKT_ALGORITHM_H

#include "../Graph/graph.h"

using Path = std::vector<std::shared_ptr<Vertex>>;
using VertexPathPair = std::pair<std::shared_ptr<Vertex>, Path>;
using PathsToTarget = std::vector<VertexPathPair>;

class Algorithm{
public:
    virtual PathsToTarget computePaths(std::shared_ptr<Graph> graph, int server_id) = 0;
};

#endif //PROJEKT_ALGORITHM_H
