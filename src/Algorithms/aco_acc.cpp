#include "../../include/Algorithms/aco_acc.h"


Path ACO_ACC::computePath(const std::shared_ptr<Graph> &graph, int server_id, int starting_point_id) {
    auto pheromone_table = std::make_shared<PheromoneTable>(initPheromoneTable(graph));
    auto starting_point = graph->getNode(starting_point_id);

    auto current_best_path = std::make_shared<Path>();

    for(int cycle_id = 0; cycle_id < number_of_cycles; ++cycle_id)
    {
        // initialize vector of Paths with size equal to the number of ants per cycle
        // during the cycle every ant will find up to one path
        std::vector<std::shared_ptr<Path>> found_paths(number_of_ants_per_cycle, std::make_shared<Path>());

//        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

#pragma acc data copy(pheromone_table, found_paths, current_best_path), copyin(server_id, graph, starting_point)
{
    #pragma acc parallel loop present(found_paths, pheromone_table, server_id, graph, starting_point)
        for(int ant_id = 0; ant_id < number_of_ants_per_cycle; ++ant_id)
        {
            auto ant = Ant(max_ant_steps);
            auto best_found_path = ant.find_server(graph, starting_point, server_id, pheromone_table);

            // remember path found by ant in the vector
            found_paths[ant_id] = best_found_path;
        }
}

        updatePheromoneTable(pheromone_table, current_best_path, found_paths);
    }

    if(current_best_path->empty())
        throw std::runtime_error("No path found!");
    return *current_best_path;
}
