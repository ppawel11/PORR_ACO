#ifndef PROJEKT_ACO_OMP_H
#define PROJEKT_ACO_OMP_H

#include "aco.h"

class ACO_OMP: public ACO
{
    int threads;
public:
    ACO_OMP(int n, int s, int m, float a, float q, float p, int threads): ACO(n,s,m,a,q,p), threads{threads} {};
protected:
    Path computePath(const std::shared_ptr<Graph>& graph, int server_id, int starting_point_id) override;
};

#endif //PROJEKT_ACO_OMP_H
