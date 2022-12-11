//
// Created by ml72486 on 16.06.2021.
//

#include "vertex.h"
#include <algorithm>
#include <random>

Vertex::Vertex(int id): id(id){

}

bool Vertex::findPath(int target, vector<int> &path, std::unordered_map<int, Vertex> &map, double lx, std::unordered_set<int> &visited)
{
    vector<int> queue;
    path.push_back(id);
    visited.insert(id);
    if(id == target)
        return true;
    for(int i = 0; i < (int)edges.size(); i++){
        if((double)edges[i]->load < lx)
            queue.push_back(i);
    }
    auto rd = std::random_device {};
    auto rng = std::default_random_engine { rd() };
    std::shuffle(std::begin(queue), std::end(queue), rng);
    for(int edgeid: queue){
        if((edges[edgeid]->node1 != id) && visited.find(edges[edgeid]->node1) == visited.end()){
            if(map.at(edges[edgeid]->node1).findPath(target, path, map, lx, visited))
                return true;
        }
        else if(visited.find(edges[edgeid]->node2) == visited.end()){
            if(map.at(edges[edgeid]->node2).findPath(target, path, map, lx, visited))
                return true;
        }
    }
    path.pop_back();
    return false;
}

Edge::Edge(int id, int load, int node1, int node2): id(id), load(load), node1(node1), node2(node2){}
