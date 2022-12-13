#ifndef PROJEKT_ACO_H
#define PROJEKT_ACO_H

#include "algorithm.h"
#include <functional>

using PheromoneTable = std::unordered_map<std::shared_ptr<Edge>, float>;

class Ant {
    int count_of_deleted_cycles = 0;
    int count_of_path_resets = 0;
    float alpha = 0.0;
    float beta = 0.0;

public:
    Ant(float alpha, float beta): alpha{alpha}, beta{beta} {};
    void find_server(
            const std::shared_ptr<Graph> &graph,
            const std::shared_ptr<Vertex> &starting_point,
            int server_node_id,
            const PheromoneTable& pheromone_table,
            std::function<void(const Path)> finished_callback);

private:
    std::shared_ptr<Vertex> chooseNextVertex(
            const std::vector<std::shared_ptr<Edge>> &possible_edges,
            const PheromoneTable &pheromone_table);
};

class ACO: public Algorithm{
    int number_of_ants_per_cycle;
    int number_of_cycles;
    float initial_pheromons_amount;
    float alpha;
    float evaporation_param;
    float beta; // probably not useful if every edge is the same length

public:
    ACO(int n, int m, float a, float q, float p): number_of_ants_per_cycle{n}, number_of_cycles{m}, initial_pheromons_amount{p}, alpha{a}, evaporation_param{q} {};
    PathsToTarget computePaths(std::shared_ptr<Graph> graph, int server_id) override;

private:
    Path computePath(const std::shared_ptr<Graph>& graph, int server_id, int starting_point_id);
    PheromoneTable initPheromoneTable(const std::shared_ptr<Graph>& graph) const;
    void updatePheromoneTable(PheromoneTable& phermone_table, const Path& path);
};

#endif //PROJEKT_ACO_H
