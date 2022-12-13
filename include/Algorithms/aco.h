#ifndef PROJEKT_ACO_H
#define PROJEKT_ACO_H

#include "algorithm.h"
#include <functional>

using PheromoneTable = std::unordered_map<std::shared_ptr<Edge>, float>;

class Ant {
    int count_of_deleted_cycles = 0;
    int count_of_path_resets = 0;
    int max_steps = 0;
    float alpha = 0.0;
    float beta = 0.0;

public:
    Ant(int max_steps, float alpha, float beta): max_steps{max_steps}, alpha{alpha}, beta{beta} {};
    Path find_server(const std::shared_ptr<Graph> &graph, const std::shared_ptr<Vertex> &starting_point, int server_node_id, const PheromoneTable &pheromone_table);

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
    float initial_pheromons_amount;
    float alpha;
    float evaporation_param;
    float beta; // probably not useful if every edge is the same length

public:
    ACO(int n, int s, int m, float a, float q, float p): number_of_ants_per_cycle{n}, max_ant_steps{s}, number_of_cycles{m}, initial_pheromons_amount{p}, alpha{a}, evaporation_param{q} {};
    PathsToTarget computePaths(std::shared_ptr<Graph> graph, int server_id) override;

protected:
    virtual Path computePath(const std::shared_ptr<Graph>& graph, int server_id, int starting_point_id);
    PheromoneTable initPheromoneTable(const std::shared_ptr<Graph>& graph) const;
    void
    updatePheromoneTable(PheromoneTable &phermone_table, const Path &best_path, const std::vector<Path> &other_found_paths);
};

#endif //PROJEKT_ACO_H
