#ifndef PROJEKT_DUMB_ALGORITHM_H
#define PROJEKT_DUMB_ALGORITHM_H

#include "algorithm.h"

// Dumb algorithm uses known nerwork structure. Returns paths to node 3 in small_network.txt.
class DumbAlgorithm: public Algorithm
{
public:
    virtual PathsToTarget computePaths(std::shared_ptr<Graph>, int server_id);
};

#endif //PROJEKT_DUMB_ALGORITHM_H
