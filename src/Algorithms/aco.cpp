#include "../../include/Algorithms/aco.h"

#include <utility>
#include <random>
#include <iostream>
#include <chrono>

PathsToTarget ACO::computePaths(std::shared_ptr<Graph> graph, int server_id) {
    PathsToTarget result = {};

    for (auto& [node_id, node]: graph->getNodes())
    {
        if(node_id == server_id)
        {
            // no need to look for the path from the server node
            result.push_back(VertexPathPair{graph->getNode(server_id),
                                            Path{graph->getNode(server_id)}});
            continue;
        }

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        auto path_to_server = computePath(graph, server_id, node_id);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        auto time_s = std::chrono::duration_cast<std::chrono::microseconds> (end - begin).count();

        result.push_back(VertexPathPair(node, path_to_server));
//        std::cout<<"path found: "<<node_id<<" -> "<<server_id<<" path size: "<<path_to_server.size()<<" in time: {"<<time_s<<"} [s]"<<std::endl;
    }

    return result;
}

Path ACO::computePath(const std::shared_ptr<Graph> &graph, int server_id, int starting_point_id) {
    auto pheromone_table = initPheromoneTable(graph);
    auto starting_point = graph->getNode(starting_point_id);
    auto current_best_path = std::make_shared<Path>();

    for(int cycle_id = 0; cycle_id < number_of_cycles; ++cycle_id)
    {
        // initialize vector of Paths with size equal to the number of ants per cycle
        // during the cycle every ant will find up to one path
        std::vector<std::shared_ptr<Path>> found_paths(number_of_ants_per_cycle, std::make_shared<Path>());

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        for(int ant_id = 0; ant_id < number_of_ants_per_cycle; ++ant_id)
        {
            auto ant = Ant(max_ant_steps);
            auto best_found_path = ant.find_server(graph, starting_point, server_id, pheromone_table);

            // remember path found by ant in the vector
            found_paths[ant_id] = best_found_path;
        }

        updatePheromoneTable(pheromone_table, current_best_path, found_paths);
    }

    if(current_best_path->empty())
        throw std::runtime_error("No path found!");
    return *current_best_path;
}

PheromoneTable ACO::initPheromoneTable(const std::shared_ptr<Graph> &graph) const {
    PheromoneTable result = {};

    for ( const auto& edge : graph->getEdges())
    {
        result[edge] = initial_pheromons_amount;
    }

    return result;
}

void ACO::updatePheromoneTable(PheromoneTable &phermone_table, std::shared_ptr<Path> &best_path, const std::vector<std::shared_ptr<Path>> &found_paths) {
    auto best_path_buffer = std::make_shared<Path>();

    for(auto& path : found_paths)
    {
        if(path->size() < 2) continue;
        // if there is no path in the best path buffer or this one is shorter - replace best path buffer with this path
        if(best_path_buffer->empty() || path->size() < best_path_buffer->size()) best_path_buffer = path;

        for(auto it = path->begin(); it < path->end() - 1; ++it)
        {
            auto from = *it;
            auto to = *(it+1);

            for( auto& [edge, pheromones_count] : phermone_table)
            {
                // for every edge in the path increase the pheromone amount by 1/path length
                if( edge->source == from && edge->target == to )
                {
                    pheromones_count += 1.0 / path->size();
                }
            }
        }
    }

    for( auto& [edge, pheromones_count] : phermone_table)
    {
        // multiply pheromones amount on every edge by evaporation_param
        pheromones_count *= evaporation_param;
        if( pheromones_count < initial_pheromons_amount ) pheromones_count = initial_pheromons_amount;
    }

    // if best path found in this cycle (best path buffer) is better than global best path (best_path) replace the global best path with the one found in this cycle
    if(best_path->empty() || (!best_path_buffer->empty() && best_path_buffer->size() < best_path->size())) best_path = best_path_buffer;
}

std::shared_ptr<Vertex> Ant::chooseNextVertex(const std::vector<std::shared_ptr<Edge>> &possible_edges, const PheromoneTable &pheromone_table)
{
    // weighted random select of next Vertex where pheromone amount on the edge leading to the Vertex is the weight
    int sum_of_weight = 0;

    for(const std::shared_ptr<Edge>& edge : possible_edges)
    {
        sum_of_weight += pheromone_table.at(edge) * 100.0;
    }

    int random_number = rand() % sum_of_weight;

    for(const std::shared_ptr<Edge>& edge : possible_edges)
    {
        if(random_number < pheromone_table.at(edge)*100.0)
        {
            return edge->target;
        }
        random_number -= pheromone_table.at(edge) * 100.0;
    }

    throw std::runtime_error("Random next vertex choice failure.");
}



std::shared_ptr<Path> Ant::find_server(const std::shared_ptr<Graph> &graph, const std::shared_ptr<Vertex> &starting_point, int server_node_id, const PheromoneTable &pheromone_table) {
    auto path = std::make_shared<Path>();
    path->push_back(starting_point);

    if(starting_point->getId() == server_node_id)
    {
        return path;
    }

    bool server_found = false;
    count_of_path_resets = 0;
    int steps = 0;

    while(count_of_path_resets < 10)
    {
        if( ++steps > max_steps )
        {
            // if number of steps made by ant exceeds max steps finish searching
            break;
        }

        auto next_possible_edges = graph->getEdgesFromNode(path->back()->getId());
        if(path->size() > 2)
        {
            // from next possible edges remove the edge which was used in the last move
            next_possible_edges.erase(std::remove_if(next_possible_edges.begin(), next_possible_edges.end(),
                                                     [&path](const auto &edge) {
                                                         return edge->target->getId() == path->rbegin()[1]->getId();
                                                     }), next_possible_edges.end());
        }

        if(next_possible_edges.empty())
        {
            // if there are no other possible edges left return to the starting point and increment resets counter
            path->resize(1);
            count_of_path_resets++;
            continue;
        }

        auto next_vertex = chooseNextVertex(next_possible_edges, pheromone_table);

        if(next_vertex->getId() == server_node_id)
        {
            // Ant found the server!
            path->push_back(graph->getNode(server_node_id));
            server_found = true;
            break;
        }

        // find out if the ant has already been in the new visited Vertex
        auto same_vertex = std::find_if(path->begin(), path->end(), [&next_vertex](const auto& vertex_in_path){ return vertex_in_path->getId() == next_vertex->getId(); });

        if(same_vertex != path->end())
        {
            // if the Ant has already been in this Vertex - remove the cycle from the path
            path->erase(same_vertex+1, path->end());
        }
        else
        {
            path->push_back(next_vertex);
        }
    }

    if(server_found)
        return path;
    return std::make_shared<Path>();
}
