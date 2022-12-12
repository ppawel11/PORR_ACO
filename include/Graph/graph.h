#ifndef PROJEKT_GRAPH_H
#define PROJEKT_GRAPH_H

#include <unordered_map>
#include <vector>
#include <memory>

struct Vertex{
    int id;

    Vertex() = default;
    Vertex(int id): id{id}  {};

    int getId() const { return id; };
};

struct Edge{
    std::shared_ptr<Vertex> source;
    std::shared_ptr<Vertex> target;

    Edge(std::shared_ptr<Vertex> source, std::shared_ptr<Vertex> target): source{source}, target{target} {}
};

class Graph{
    std::unordered_map<int, std::shared_ptr<Vertex>> nodes;
    std::vector<std::shared_ptr<Edge>> edges;

public:
    Graph() {};

    void addNode(std::shared_ptr<Vertex> node);
    void connectNodes(int source_id, int target_id);
    bool hasNode(int node_id) const;
    std::shared_ptr<Vertex> getNode(int node_id);
    std::vector<std::shared_ptr<Vertex>> getNeighbours(int node_id) const;
};

std::shared_ptr<Graph> readGraphFromFile(const std::string& file_name);

#endif
