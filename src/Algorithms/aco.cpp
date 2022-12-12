#include "../../include/Algorithms/aco.h"

#include <utility>
#include <random>
#include <iostream>

PathsToTarget ACO::computePaths(std::shared_ptr<Graph> graph, int server_id) {
    PathsToTarget result = {};

    for (auto& [node_id, node]: graph->getNodes())
    {
        auto path_to_server = computePath(graph, server_id, node_id);
        result.push_back(VertexPathPair(node, path_to_server));
        std::cout<<"path found: "<<node_id<<" -> "<<server_id<<std::endl;
    }

    return result;
}

Path ACO::computePath(const std::shared_ptr<Graph> &graph, int server_id, int starting_point_id) {
    auto pheromone_table = initPheromoneTable(graph);
    auto starting_point = graph->getNode(starting_point_id);
    auto current_best_path = Path();

    for(int cycle_id = 0; cycle_id < number_of_cycles; ++cycle_id)
    {
        for(int ant_id = 0; ant_id < number_of_ants_per_cycle; ++ant_id)
        {
            auto ant = Ant(alpha, beta);
            ant.find_server(graph, starting_point, server_id, pheromone_table, [&current_best_path](const Path best_found_path)
                {
                    if(best_found_path.empty()) return;
                    if(current_best_path.empty() || best_found_path.size() < current_best_path.size()) current_best_path = best_found_path;
                }
            );
        }

        updatePheromoneTable(pheromone_table, current_best_path);
    }

    if(current_best_path.empty())
        throw std::runtime_error("No path found!");
    return current_best_path;
}

PheromoneTable ACO::initPheromoneTable(const std::shared_ptr<Graph> &graph) const {
    PheromoneTable result = {};

    for ( const auto& edge : graph->getEdges())
    {
        result[edge] = initial_pheromons_amount;
    }

    return result;
}

void ACO::updatePheromoneTable(PheromoneTable &phermone_table, const Path &path) {
    if(path.size() < 2) return;

    for(auto it = path.begin(); it < path.end()-1; ++it)
    {
        auto from = *it;
        auto to = *(it+1);

        for( auto& [edge, pheromones_count] : phermone_table)
        {
            if( edge->source == from && edge->target == to )
            {
                pheromones_count += 1.0/path.size();
            }
            else
            {
                // evaporate here?
            }
        }
    }
}

std::shared_ptr<Vertex> Ant::chooseNextVertex(const std::shared_ptr<Vertex> &vertex_of_origin,
                                              const std::vector<std::shared_ptr<Vertex>> &neighbours,
                                              const PheromoneTable &pheromone_table)
{
    //todo: legit selecting next vertex based on pheromones
    int neigh_id = rand() % neighbours.size();
    return neighbours[neigh_id];
}



void Ant::find_server(const std::shared_ptr<Graph> &graph, const std::shared_ptr<Vertex> &starting_point, int server_node_id,
                      const PheromoneTable &pheromone_table, std::function<void(const Path)> finished_callback) {
    auto path = Path{starting_point};

    if(starting_point->getId() == server_node_id)
    {
        finished_callback(path);
        return;
    }

    bool server_found = false;

    count_of_path_resets = 0;
    count_of_deleted_cycles = 0;

    while(count_of_path_resets < 10)
    {
        auto next_vertex = chooseNextVertex(path.back(), graph->getNeighbours(path.back()->getId()), pheromone_table);

        if(next_vertex->getId() == server_node_id)
        {
            path.push_back(graph->getNode(server_node_id));
            server_found = true;
            break;
        }

        auto same_vertex = std::find_if(path.begin(), path.end(), [&next_vertex](const auto& vertex_in_path){ return vertex_in_path->getId() == next_vertex->getId(); });
        if(same_vertex != std::end(path))
        {
            path.erase(same_vertex+1, path.end());
            count_of_deleted_cycles++;
            if( count_of_deleted_cycles >= 8)
            {
                path.resize(1);
                count_of_deleted_cycles = 0;
                count_of_path_resets++;
            }
        }
        else
        {
            path.push_back(next_vertex);
        }
    }

    server_found ? finished_callback(path) : finished_callback(Path());
}
