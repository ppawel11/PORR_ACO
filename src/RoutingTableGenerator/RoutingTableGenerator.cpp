#include "../../include/RoutingTableGenerator/RoutingTableGenerator.h"

std::vector<RoutingPair> RoutingTableGenerator::generateRoutingTableTo(const int server_id) {
    auto shortest_paths = algorithm->computePaths(graph, server_id);

    std::vector<RoutingPair> result = {};
    result.reserve(shortest_paths.size());

    for(const auto& vertex_and_path : shortest_paths)
    {
        if(vertex_and_path.second.empty())
            throw std::runtime_error("Empty path");
        
        result.emplace_back(vertex_and_path.first, vertex_and_path.second[0]);
    }

    return result;
}
