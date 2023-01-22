#include "../../include/Algorithms/aco_omp.h"

#include <omp.h>


Path ACO_OMP::computePath(const std::shared_ptr<Graph> &graph, int server_id, int starting_point_id) {
    auto pheromone_table = initPheromoneTable(graph);
    auto starting_point = graph->getNode(starting_point_id);

    auto current_best_path = std::make_shared<Path>();

    for(int cycle_id = 0; cycle_id < number_of_cycles; ++cycle_id)
    {
        // initialize vector of Paths with size equal to the number of ants per cycle
        // during the cycle every ant will find up to one path
        std::vector<std::shared_ptr<Path>> found_paths(number_of_ants_per_cycle, std::make_shared<Path>());

        #pragma omp parallel num_threads(threads)
        {
            int path_id = omp_get_thread_num();
            while(path_id < number_of_ants_per_cycle )
            {
                auto ant = Ant(max_ant_steps);
//                auto best_found_path = ant.find_server(graph, starting_point, server_id, pheromone_table);

                // update the path stored in the vector
                // the id of updated path int he vector is: id_of_the_thread + id_of_an_ant_in_this_thread * number_of_all_threads
 //               found_paths[path_id] = best_found_path;

                path_id += omp_get_num_threads();
            }
        }

   //     updatePheromoneTable(pheromone_table, current_best_path, found_paths);
    }

    if(current_best_path->empty())
        throw std::runtime_error("No path found!");
    return *current_best_path;
}
