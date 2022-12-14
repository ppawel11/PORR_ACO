#include "../../include/Algorithms/aco.h"

#include <utility>
#include <random>
#include <iostream>
#include <chrono>

PathsToTarget ACO::computePaths(std::shared_ptr<Graph> graph, int server_id) {
    PathsToTarget result = {};

    for (auto& [node_id, node]: graph->getNodes())
    {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        auto path_to_server = computePath(graph, server_id, node_id);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        auto time_s = std::chrono::duration_cast<std::chrono::seconds> (end - begin).count();

        result.push_back(VertexPathPair(node, path_to_server));
        std::cout<<"path found: "<<node_id<<" -> "<<server_id<<" path size: "<<path_to_server.size()<<" in time: "<<time_s<<" [s]"<<std::endl;
    }

    return result;
}

Path ACO::computePath(const std::shared_ptr<Graph> &graph, int server_id, int starting_point_id) {
    auto pheromone_table = initPheromoneTable(graph);
    auto starting_point = graph->getNode(starting_point_id);

    auto current_best_path = std::make_shared<Path>();

    for(int cycle_id = 0; cycle_id < number_of_cycles; ++cycle_id)
    {
        std::vector<std::shared_ptr<Path>> found_paths(number_of_ants_per_cycle, std::make_shared<Path>());

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        for(int ant_id = 0; ant_id < number_of_ants_per_cycle; ++ant_id)
        {
            auto ant = Ant(max_ant_steps, alpha, beta);
            auto best_found_path = ant.find_server(graph, starting_point, server_id, pheromone_table);

            found_paths[ant_id] = best_found_path;
        }

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        updatePheromoneTable(pheromone_table, current_best_path, found_paths);
        std::chrono::steady_clock::time_point update = std::chrono::steady_clock::now();

        auto time_s = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
        auto time_s_update = std::chrono::duration_cast<std::chrono::milliseconds> (update - end).count();
        std::cout<<"cycle "<<cycle_id<<" ants time: "<<time_s<<" pheromone update time: "<<time_s_update<<std::endl;
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

void ACO::updatePheromoneTable(PheromoneTable &phermone_table, std::shared_ptr<Path> &best_path, const std::vector<std::shared_ptr<Path>> &other_found_paths) {
    auto best_path_buffer = std::make_shared<Path>();

    for(auto& path : other_found_paths)
    {
        if(path->size() < 2) continue;
        if(best_path_buffer->empty() || path->size() < best_path_buffer->size()) best_path_buffer = path;

        for(auto it = path->begin(); it < path->end() - 1; ++it)
        {
            auto from = *it;
            auto to = *(it+1);

            for( auto& [edge, pheromones_count] : phermone_table)
            {
                if( edge->source == from && edge->target == to )
                {
                    pheromones_count += 1.0 / path->size();
                }
            }
        }
    }

    for(auto it = best_path_buffer->begin(); it < best_path_buffer->end() - 1; ++it)
    {
        auto from = *it;
        auto to = *(it+1);

        for( auto& [edge, pheromones_count] : phermone_table)
        {
            if( edge->source == from && edge->target == to )
            {
                pheromones_count -= 1.0 / best_path_buffer->size();
                pheromones_count += evaporation_param * (1.0 / best_path_buffer->size());
            }
        }
    }

    for( auto& [edge, pheromones_count] : phermone_table)
    {
        pheromones_count *= 1 - evaporation_param;
        if( pheromones_count < initial_pheromons_amount ) pheromones_count = initial_pheromons_amount;
    }

    if(best_path->empty() || (!best_path_buffer->empty() && best_path_buffer->size() < best_path->size())) best_path = best_path_buffer;
}

std::shared_ptr<Vertex> Ant::chooseNextVertex(const std::vector<std::shared_ptr<Edge>> &possible_edges, const PheromoneTable &pheromone_table)
{
    int sum_of_weight = 0;

    for(const std::shared_ptr<Edge>& edge : possible_edges)
    {
        sum_of_weight += pheromone_table.at(edge) * 100.0;
    }

    //todo: better random number generation
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
    count_of_deleted_cycles = 0;

    int steps = 0;
    while(count_of_path_resets < 10)
    {
        if( ++steps > max_steps )
        {
            break;
        }

        auto next_possible_edges = graph->getEdgesFromNode(path->back()->getId());
        if(path->size() > 2)
            next_possible_edges.erase(std::remove_if(next_possible_edges.begin(), next_possible_edges.end(), [&path](const auto& edge){ return edge->target->getId() == path->rbegin()[1]->getId(); }), next_possible_edges.end() );
        if(next_possible_edges.empty())
        {
            path->resize(1);
            count_of_deleted_cycles = 0;
            count_of_path_resets++;
            continue;
        }

        auto next_vertex = chooseNextVertex(next_possible_edges, pheromone_table);

        if(next_vertex->getId() == server_node_id)
        {
            path->push_back(graph->getNode(server_node_id));
            server_found = true;
            break;
        }

        auto same_vertex = std::find_if(path->begin(), path->end(), [&next_vertex](const auto& vertex_in_path){ return vertex_in_path->getId() == next_vertex->getId(); });

        if(same_vertex != path->end())
        {
            path->erase(same_vertex+1, path->end());
//            count_of_deleted_cycles++;
//            if( count_of_deleted_cycles >= 10)
//            {
//                path.resize(1);
//                count_of_deleted_cycles = 0;
//                count_of_path_resets++;
//                continue;
//            }
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
