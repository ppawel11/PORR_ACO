#ifndef PROJEKT_ACO_H
#define PROJEKT_ACO_H

#include "algorithm.h"
#include <functional>

using PheromoneTable = std::unordered_map<std::shared_ptr<Edge>, float>;

class Ant {
    int count_of_path_resets = 0;
    int max_steps = 0;

public:
    Ant(int max_steps): max_steps{max_steps} {};
    std::shared_ptr<Path> find_server(const std::shared_ptr<Graph> &graph, const std::shared_ptr<Vertex> &starting_point, int server_node_id, const PheromoneTable &pheromone_table);

private:
    static std::shared_ptr<Vertex> chooseNextVertex(
            const std::vector<std::shared_ptr<Edge>> &possible_edges,
            const PheromoneTable &pheromone_table);
};

class ACO: public Algorithm{
protected:
    int number_of_ants_per_cycle;
    int max_ant_steps;
    int number_of_cycles;
    float evaporation_param;
    float initial_pheromons_amount;

public:
    ACO(int number_of_ants_per_cycle, int max_ant_steps, int number_of_cycles, float evaporation_param, float initial_pheromones_amount):
        number_of_ants_per_cycle{number_of_ants_per_cycle},
        max_ant_steps{max_ant_steps},
        number_of_cycles{number_of_cycles},
        evaporation_param{evaporation_param},
        initial_pheromons_amount{initial_pheromones_amount} {};

    PathsToTarget computePaths(std::shared_ptr<Graph> graph, int server_id) override;

protected:
    virtual Path computePath(const std::shared_ptr<Graph>& graph, int server_id, int starting_point_id);
    PheromoneTable initPheromoneTable(const std::shared_ptr<Graph>& graph) const;
    void updatePheromoneTable(PheromoneTable &phermone_table, std::shared_ptr<Path> &best_path, const std::vector<std::shared_ptr<Path>> &found_paths);
};

#endif //PROJEKT_ACO_H
