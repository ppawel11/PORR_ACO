//
// Created by ml72486 on 16.06.2021.
//

#ifndef ALHE_PROJEKT_VERTEX_H
#define ALHE_PROJEKT_VERTEX_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct Edge{
    int id;
    int load;
    int node1;
    int node2;

    Edge(int id, int load, int node1, int node2);
};

class Vertex{
public:
    int id;
    vector<shared_ptr<Edge>> edges;

    Vertex();
    Vertex(int id);
    bool findPath(int target, vector<int> &path, std::unordered_map<int, Vertex> &map, double lx, std::unordered_set<int> &visited);
};

#endif //ALHE_PROJEKT_VERTEX_H
