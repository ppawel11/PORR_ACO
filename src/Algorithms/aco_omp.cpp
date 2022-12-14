#include "../../include/Algorithms/aco_omp.h"

#include <omp.h>
#include <iostream>
#include <mutex>


Path ACO_OMP::computePath(const std::shared_ptr<Graph> &graph, int server_id, int starting_point_id) {
    auto pheromone_table = initPheromoneTable(graph);
    auto starting_point = graph->getNode(starting_point_id);

    auto current_best_path = Path();
    std::vector<std::shared_ptr<Path>> found_paths(number_of_ants_per_cycle, std::make_shared<Path>());
    std::mutex mtx;

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

                if(!best_found_path.empty())
                {
                    if(current_best_path.empty() || best_found_path.size() < current_best_path.size())
                    {
                        if(!current_best_path.empty())
                        {
                            found_paths[path_id] = std::make_shared<Path>(best_found_path);
                        }

                        std::scoped_lock lock(mtx);
                        current_best_path = best_found_path;
                    }
                    else
                    {
                        found_paths[path_id] = std::make_shared<Path>(best_found_path);
                    }
                }

                path_id += omp_get_num_threads();
            }
        }

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        // ugly hack
        // todo: let Ant remember its best path, change found_paths to std::vector<Ant> and pass it to updatePheromoneTable after each cycle
        std::vector<Path> found_paths_deptr = {};
        for(const auto& path_ptr : found_paths)
        {
            if(!path_ptr->empty()) found_paths_deptr.push_back(*path_ptr);
        }

        std::chrono::steady_clock::time_point rewrite = std::chrono::steady_clock::now();



        updatePheromoneTable(pheromone_table, current_best_path, found_paths_deptr);

        std::chrono::steady_clock::time_point update = std::chrono::steady_clock::now();

        auto time_ants = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
        auto time_rewrite = std::chrono::duration_cast<std::chrono::milliseconds> (rewrite - end).count();
        auto time_update = std::chrono::duration_cast<std::chrono::milliseconds> (update - rewrite).count();

        std::cout<<"cycle "<<cycle_id<<" ants time: "<<time_ants<<" rewrite time: "<<time_rewrite<<" pheromone update time: "<<time_update<<std::endl;
    }

    if(current_best_path.empty())
        throw std::runtime_error("No path found!");
    return current_best_path;
}
