#include "../../include/Algorithms/aco_omp.h"

#include <omp.h>
#include <iostream>
#include <mutex>


Path ACO_OMP::computePath(const std::shared_ptr<Graph> &graph, int server_id, int starting_point_id) {
    auto pheromone_table = initPheromoneTable(graph);
    auto starting_point = graph->getNode(starting_point_id);

    auto current_best_path = std::make_shared<Path>();
    std::vector<std::shared_ptr<Path>> found_paths(number_of_ants_per_cycle, std::make_shared<Path>());

    for(int cycle_id = 0; cycle_id < number_of_cycles; ++cycle_id)
    {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        //todo: user defined threads number
        #pragma omp parallel num_threads(4)
        {
            int path_id = omp_get_thread_num();
            while(path_id < number_of_ants_per_cycle )
            {
                auto ant = Ant(max_ant_steps, alpha, beta);
                auto best_found_path = ant.find_server(graph, starting_point, server_id, pheromone_table);

                found_paths[path_id] = best_found_path;

                path_id += omp_get_num_threads();
            }
        }

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        updatePheromoneTable(pheromone_table, current_best_path, found_paths);

        std::chrono::steady_clock::time_point update = std::chrono::steady_clock::now();

        auto time_ants = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
        auto time_update = std::chrono::duration_cast<std::chrono::milliseconds> (update - end).count();

        std::cout<<"cycle "<<cycle_id<<" ants time: "<<time_ants<<" pheromone update time: "<<time_update<<std::endl;
    }

    if(current_best_path->empty())
        throw std::runtime_error("No path found!");
    return *current_best_path;
}
