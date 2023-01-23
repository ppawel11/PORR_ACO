#include "../../include/Algorithms/aco_acc.h"
#include <iostream>
#include <openacc.h>
#include "openacc_curand.h"

Path ACO_ACC::computePath(const std::shared_ptr<Graph> &graph, int server_id, int starting_point_id) {
    auto pheromone_table = std::make_shared<PheromoneTable>(initPheromoneTable(graph));
    auto starting_point = graph->getNode(starting_point_id);

    auto current_best_path = std::make_shared<Path>();

    for (int cycle_id = 0; cycle_id < number_of_cycles; ++cycle_id) {
        // initialize vector of Paths with size equal to the number of ants per cycle
        // during the cycle every ant will find up to one path
        std::vector<std::shared_ptr<Path>> found_paths(number_of_ants_per_cycle, std::make_shared<Path>());
        std::vector<std::shared_ptr<Ant>> ants(number_of_ants_per_cycle, std::make_shared<Ant>(max_ant_steps));
        std::vector<int> seeds = {};
        seeds.reserve(number_of_ants_per_cycle);
        for(int i = 0; i < number_of_ants_per_cycle; ++i)
        {
            seeds.emplace_back(rand());
        }
//        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        #pragma acc data copy(pheromone_table, ants, found_paths, seeds, current_best_path), copyin(server_id, graph, starting_point)
        {
            #pragma acc parallel loop present(found_paths, ants, seeds, pheromone_table, server_id, graph, starting_point)
            for (int ant_id = 0; ant_id < number_of_ants_per_cycle; ++ant_id) {
                //                auto ant = Ant(max_ant_steps);
                curandState_t state;
                int gangidx = __pgi_gangidx();
                curand_init(seeds[gangidx], gangidx, 0, &state);

                int count_of_path_resets = 0;
                int max_steps = max_ant_steps;
                bool server_found = false;

                auto path = std::make_shared<Path>();
                path->push_back(starting_point);

                if (starting_point->getId() != server_id) {
                    server_found = false;
                    count_of_path_resets = 0;
                    int steps = 0;

                    while (count_of_path_resets < 10) {
                        if (++steps > max_steps) {
                            // if number of steps made by ant exceeds max steps finish searching
                            break;
                        }

                        //                        auto next_possible_edges = graph->getEdgesFromNode(path->back()->getId());
                        std::vector<std::shared_ptr<Edge>> next_possible_edges = {};
                        for (const auto &edge : graph->edges) {
                            if (edge->source->getId() == path->back()->getId()) {
                                next_possible_edges.push_back(edge);
                            }
                        }

                        // weighted random select of next Vertex where pheromone amount on the edge leading to the Vertex is the weight
                        std::shared_ptr<Vertex> next_vertex;
                        int sum_of_weight = 0;

                        for (const std::shared_ptr<Edge> &edge : next_possible_edges) {
                            auto edge_id = edge->target->id;
                            if( path->size() > 1)
                            {
                                auto rbeg_id = (*path)[path->size() - 2]->id;
                                if (edge_id == rbeg_id)
                                    continue;
                            }
                            sum_of_weight += pheromone_table->at(edge) * 100.0;
                        }


//                        int random_number = rand() % sum_of_weight;
                        int random_number = curand_uniform(&state) * sum_of_weight;

                        for (const std::shared_ptr<Edge> &edge : next_possible_edges) {
                            if( path->size() > 1) {
                                auto rbeg_id = (*path)[path->size() - 2]->id;
                                if (edge->target->id == rbeg_id) {
                                    continue;
                                }
                            }

                            auto pheromones = pheromone_table->at(edge)*100.0;

                            if (random_number < pheromone_table->at(edge) * 100.0) {
                                next_vertex = edge->target;
    //                            std::cout<<"wybrany nastepny -> "<<next_vertex->id<<std::endl;
                                break;
                            }
                            random_number -= pheromone_table->at(edge) * 100.0;
                        }

                        if (next_vertex->id == server_id) {
                            // Ant found the server!
                            //                            path->push_back(graph->getNode(server_id));
                            path->push_back(graph->nodes[server_id]);
                            server_found = true;
                            break;
                        }

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
                }

                std::shared_ptr<Path> best_found_path;
                if (server_found)
                    best_found_path = path;
                else
                    best_found_path = std::make_shared<Path>();

                // remember path found by ant in the vector
                found_paths[ant_id] = best_found_path;
            }
        } // ACC LOOP

        updatePheromoneTable(pheromone_table, current_best_path, found_paths);
    }

//    if(current_best_path->empty())
//        throw std::runtime_error("No path found!");
    return *current_best_path;
}
