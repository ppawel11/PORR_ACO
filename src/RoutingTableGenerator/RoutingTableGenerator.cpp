#include "../../include/RoutingTableGenerator/RoutingTableGenerator.h"

std::vector<RoutingPair> RoutingTableGenerator::generateRoutingTableTo(const int server_id) {
    auto shortest_paths = algorithm->computePaths(graph, server_id);

    std::vector<RoutingPair> result = {};
    result.reserve(shortest_paths.size());

    for(const auto& [vertex,path] : shortest_paths)
    {
        if(path.empty())
            throw std::runtime_error("Empty path");

        if(path[0]->getId() == server_id)
            result.emplace_back(vertex, path[0]);
        else
        {
            if(path.size() < 2 )
                throw std::runtime_error("Path too short");
            result.emplace_back(vertex, path[1]);
        }
    }

    return result;
}
