#include "../../include/Graph/graph.h"
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>

void Graph::addNode(std::shared_ptr<Vertex> node) {
    if(nodes.contains(node->getId()))
    {
        throw std::runtime_error("Node already exists!");
    }
    nodes[node->getId()] = node;
}

void Graph::connectNodes(int source_id, int target_id) {
    if(!nodes.contains(source_id) || !nodes.contains(target_id))
    {
        throw std::runtime_error("Invalid nodes.");
    }
    if( std::find_if(edges.begin(), edges.end(), [&, source_id, target_id](const auto& edge)
        {
            return ((edge->source->getId() == source_id && edge->target->getId() == target_id) ||
                    (edge->target->getId() == source_id && edge->source->getId() == target_id));
        }) != edges.end())
    {
//        throw std::runtime_error("Edge already exists!");
        return;
    }

    edges.push_back(std::make_shared<Edge>(nodes[source_id], nodes[target_id]));
    edges.push_back(std::make_shared<Edge>(nodes[target_id], nodes[source_id]));
}

bool Graph::hasNode(int node_id) const {
    return nodes.contains(node_id);
}

std::vector<std::shared_ptr<Vertex>> Graph::getNeighbours(int node_id) const{
    if( !hasNode(node_id))
    {
        throw std::runtime_error("Node does not exists!");
    }

    std::vector<std::shared_ptr<Vertex>> result  {};

    for(const auto& edge : edges)
    {
        if(edge->source->getId() ==  node_id)
        {
            result.push_back(edge->target);
        }
    }
    return result;
}

std::shared_ptr<Vertex> Graph::getNode(int node_id) {
    return nodes[node_id];
}

const std::unordered_map<int, std::shared_ptr<Vertex>> &Graph::getNodes() const {
    return nodes;
}

const std::vector<std::shared_ptr<Edge>> &Graph::getEdges() const {
    return edges;
}

std::vector<std::shared_ptr<Edge>> Graph::getEdgesFromNode(int node_id) const {
    std::vector<std::shared_ptr<Edge>> result = {};
    for(const auto& edge : edges)
    {
        if(edge->source->getId() == node_id)
        {
            result.push_back(edge);
        }
    }
    return result;
}

std::shared_ptr<Graph> readGraphFromFile(const std::string& file_name)
{
    std::ifstream infile(file_name);
    std::string line;
    std::string buffer;

    auto graph = std::make_shared<Graph>();

    bool new_node_added;
    int node;

    while (std::getline(infile, line))
    {
        if(!buffer.empty())
        {
            int node_id = std::stoi(buffer);
            if(!graph->hasNode(node_id))
                graph->addNode(std::make_shared<Vertex>(node_id));
            graph->connectNodes(node, node_id);
            buffer.clear();
        }
        new_node_added = false;
        buffer.clear();

        for(const auto& ch : line)
        {
            if(!new_node_added)
            {
                if(ch != ':')
                    buffer.push_back(ch);
                else
                {
                    int node_id = std::stoi(buffer);
                    if(!graph->hasNode(node_id))
                        graph->addNode(std::make_shared<Vertex>(node_id));

                    node = node_id;
                    buffer.clear();
                    new_node_added = true;
                }
            }
            else
            {
                if(ch != ',' && ch != '\n')
                    buffer.push_back(ch);
                else
                {
                    int node_id = std::stoi(buffer);
                    if(!graph->hasNode(node_id))
                        graph->addNode(std::make_shared<Vertex>(node_id));
                    graph->connectNodes(node, node_id);
                    buffer.clear();
                }
            }
        }
    }

    return graph;
}
