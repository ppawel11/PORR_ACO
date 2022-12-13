#include "../../include/Algorithms/dumb_algorithm.h"

PathsToTarget DumbAlgorithm::computePaths(std::shared_ptr<Graph> graph, int server_id) {
    if(!graph->hasNode(server_id))
        throw std::runtime_error("Server id incorrect");

    return PathsToTarget
            {
                VertexPathPair(graph->getNode(1), Path{graph->getNode(3)}),
                VertexPathPair(graph->getNode(2), Path{graph->getNode(3)}),
                VertexPathPair(graph->getNode(3), Path{graph->getNode(3)}),
                VertexPathPair(graph->getNode(4), Path{graph->getNode(3)}),
                VertexPathPair(graph->getNode(5), Path{graph->getNode(4), graph->getNode(3)}),
                VertexPathPair(graph->getNode(6), Path{graph->getNode(4), graph->getNode(3)}),
            };
}
