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
    int id;
    std::shared_ptr<Vertex> source;
    std::shared_ptr<Vertex> target;

    Edge(std::shared_ptr<Vertex> source, std::shared_ptr<Vertex> target, int id): source{source}, target{target}, id{id} {}
};

class Graph{
public:
//    std::unordered_map<int, std::shared_ptr<Vertex>> nodes;
    std::vector<std::pair<int, std::shared_ptr<Vertex>>> nodes;

    std::vector<std::shared_ptr<Edge>> edges;
    Graph() {};

    void addNode(const std::shared_ptr<Vertex>& node);
    void connectNodes(int source_id, int target_id);
    bool hasNode(int node_id) const;
    std::shared_ptr<Vertex> getNode(int node_id);
    std::vector<std::shared_ptr<Vertex>> getNeighbours(int node_id) const;
    std::vector<std::shared_ptr<Edge>> getEdgesFromNode(int node_id) const;
    std::vector<std::shared_ptr<Edge>> getEdgesFromNode(int node_id, int banned_id) const;
    const std::vector<std::pair<int, std::shared_ptr<Vertex>>> & getNodes() const;
    const std::vector<std::shared_ptr<Edge>>& getEdges() const;
};

std::shared_ptr<Graph> readGraphFromFile(const std::string& file_name);

#endif
