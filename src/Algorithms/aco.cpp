#include "../../include/Algorithms/aco.h"

#include <utility>
#include <random>
#include <iostream>
#include <chrono>

PathsToTarget ACO::computePaths(std::shared_ptr<Graph> graph, int server_id) {
    PathsToTarget result = {};


//        #pragma acc data copy(result)
//    {
//        #pragma acc kernels
        for (auto& node_id_node: graph->getNodes()) {
            if (node_id_node.first == server_id) {
                // no need to look for the path from the server node
                result.push_back(VertexPathPair{graph->getNode(server_id),
                                                Path{graph->getNode(server_id)}});
                continue;
            }

            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            auto path_to_server = computePath(graph, server_id, node_id_node.first);
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

            auto time_s = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

            if( !path_to_server.empty())
                result.push_back(VertexPathPair(node_id_node.second, path_to_server));

            std::cout << "path found: " << node_id_node.first << " -> " << server_id << " path size: " << path_to_server.size()
                      << " in time: {" << time_s << "} [s]" << std::endl;
            for( const auto& v : path_to_server )
            {
                std::cout<<v->id<<" -> ";
            }
            std::cout<<std::endl;
//        }
    }

    return result;
}

//#pragma acc routine seq
Path ACO::computePath(const std::shared_ptr<Graph> &graph, int server_id, int starting_point_id) {
    auto pheromone_table = std::make_shared<PheromoneTable>(initPheromoneTable(graph));
    auto starting_point = graph->getNode(starting_point_id);
    auto current_best_path = std::make_shared<Path>();

    for(int cycle_id = 0; cycle_id < number_of_cycles; ++cycle_id)
    {
        // initialize vector of Paths with size equal to the number of ants per cycle
        // during the cycle every ant will find up to one path
        std::vector<std::shared_ptr<Path>> found_paths(number_of_ants_per_cycle, std::make_shared<Path>());
        std::vector<std::shared_ptr<Ant>> ants(number_of_ants_per_cycle, std::make_shared<Ant>(max_ant_steps));

//        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        for(int ant_id = 0; ant_id < number_of_ants_per_cycle; ++ant_id)
        {
//            auto ant = Ant(max_ant_steps);
            auto best_found_path = ants[ant_id]->find_server(graph, starting_point, server_id, pheromone_table);

            // remember path found by ant in the vector
            found_paths[ant_id] = best_found_path;
        }

        updatePheromoneTable(pheromone_table, current_best_path, found_paths);
    }

//    if(current_best_path->empty())
//        throw std::runtime_error("No path found!");
    return *current_best_path;
}

PheromoneTable ACO::initPheromoneTable(const std::shared_ptr<Graph> &graph) const {
//    PheromoneTable result = {};

    PheromoneTable result_ = {};
    int size = graph->getEdges().size();
    result_.resize(size);

    for ( const auto& edge : graph->getEdges())
    {
//        result[edge] = initial_pheromons_amount;
        result_[edge->id] = { edge, initial_pheromons_amount };
    }

    return result_;
}

void ACO::updatePheromoneTable(std::shared_ptr<PheromoneTable> phermone_table, std::shared_ptr<Path> &best_path, const std::vector<std::shared_ptr<Path>> &found_paths) {
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

            for( auto& edge_pheromones : *phermone_table)
            {
                // for every edge in the path increase the pheromone amount by 1/path length
                if( edge_pheromones.first->source == from && edge_pheromones.first->target == to )
                {
                    edge_pheromones.second += 1.0 / path->size();
                }
            }
        }
    }

    for( auto& edge_pheromones : *phermone_table)
    {
        // multiply pheromones amount on every edge by evaporation_param
        edge_pheromones.second *= evaporation_param;
        if( edge_pheromones.second < initial_pheromons_amount ) edge_pheromones.second = initial_pheromons_amount;
    }

    // if best path found in this cycle (best path buffer) is better than global best path (best_path) replace the global best path with the one found in this cycle
    if(best_path->empty() || (!best_path_buffer->empty() && best_path_buffer->size() < best_path->size())) best_path = best_path_buffer;
}

std::shared_ptr<Vertex> Ant::chooseNextVertex(const std::vector<std::shared_ptr<Edge>> &possible_edges, std::shared_ptr<PheromoneTable> pheromone_table)
{
    // weighted random select of next Vertex where pheromone amount on the edge leading to the Vertex is the weight
    int sum_of_weight = 0;

    for(const std::shared_ptr<Edge>& edge : possible_edges)
    {
        sum_of_weight += (*pheromone_table)[edge->id].second * 100.0;
    }

    int random_number = rand() % sum_of_weight;

    for(const std::shared_ptr<Edge>& edge : possible_edges)
    {
        if(random_number < (*pheromone_table)[edge->id].second *100.0)
        {
            return edge->target;
        }
        random_number -= (*pheromone_table)[edge->id].second * 100.0;
    }

//    throw std::runtime_error("Random next vertex choice failure.");
}



std::shared_ptr<Path> Ant::find_server(const std::shared_ptr<Graph> &graph, const std::shared_ptr<Vertex> &starting_point, int server_node_id, std::shared_ptr<PheromoneTable> pheromone_table) {
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

        auto next_possible_edges = graph->getEdgesFromNode(path->back()->getId(), path->rbegin()[1]->getId());
//        if(path->size() > 2)
//        {
//             from next possible edges remove the edge which was used in the last move
//            next_possible_edges.erase(std::remove_if(next_possible_edges.begin(), next_possible_edges.end(),
//                                                     [&path](const auto &edge) {
//                                                         return edge->target->getId() == path->rbegin()[1]->getId();
//                                                     }), next_possible_edges.end());
//            auto rbeg_id = (*path)[path->size() - 2]->id;
//            if (edge_id == rbeg_id)
//                continue;
//        }

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
        auto it = path->begin();
        for (it = path->begin(); it != path->end(); ++it) {
            if (it->get()->id == next_vertex->id) {
                break;
            }
        }

        if (it != path->end()) {
            // if the Ant has already been in this Vertex - remove the cycle from the path
            path->erase(it + 1, path->end());
        } else {
            path->push_back(next_vertex);
        }
    }

    if(server_found)
        return path;
    return std::make_shared<Path>();
}
