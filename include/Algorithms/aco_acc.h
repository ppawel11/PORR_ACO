#ifndef ACO_ACC_H
#define ACO_ACC_H

#include "aco.h"

class ACO_ACC : public ACO
{
public:
    ACO_ACC(int number_of_ants_per_cycle, int max_ant_steps, int number_of_cycles, float evaporation_param, float initial_pheromones_amount):
        ACO(number_of_ants_per_cycle, max_ant_steps, number_of_cycles, evaporation_param, initial_pheromones_amount){};
protected:
    Path computePath(const std::shared_ptr<Graph>& graph, int server_id, int starting_point_id) override;
};

#endif // ACO_ACC_H
