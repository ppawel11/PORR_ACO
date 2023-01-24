#include "../../include/RoutingTableGenerator/RoutingTableGenerator.h"

std::vector<RoutingPair> RoutingTableGenerator::generateRoutingTableTo(const int server_id) {
    auto shortest_paths = algorithm->computePaths(graph, server_id);

    std::vector<RoutingPair> result = {};
    result.reserve(shortest_paths.size());

    for(const auto& vertex_path : shortest_paths)
    {
//        if(vertex_path.second.empty())
//            throw std::runtime_error("Empty path");

        if(vertex_path.second[0]->getId() == server_id)
            result.emplace_back(vertex_path.first, vertex_path.second[0]);
        else
        {
//            if(vertex_path.second.size() < 2 )
//                throw std::runtime_error("Path too short");
            result.emplace_back(vertex_path.first, vertex_path.second[1]);
        }
    }

    return result;
}
