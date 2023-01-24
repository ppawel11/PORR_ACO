#include "../../include/Graph/graph.h"
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>

void Graph::addNode(const std::shared_ptr<Vertex>& node) {
//    if(nodes.size() >= node->getId() && nodes[node->getId()].first == node->getId())
//    {
//        throw std::runtime_error("Node already exists!");
//    }

    for(const auto& n : nodes)
    {
        if(n.first == node->getId())
            throw std::runtime_error("Node already exists!");
    }

    nodes.push_back({node->getId(), node});
//    nodes.reserve(node->getId());
//    nodes[node->getId()] = {node->getId(), node};
}

void Graph::connectNodes(int source_id, int target_id) {
//    if(nodes.find(source_id) == nodes.end() || nodes.find(target_id) == nodes.end())
//    {
//        throw std::runtime_error("Invalid nodes.");
//    }
    if( std::find_if(edges.begin(), edges.end(), [&, source_id, target_id](const auto& edge)
        {
            return ((edge->source->getId() == source_id && edge->target->getId() == target_id) ||
                    (edge->target->getId() == source_id && edge->source->getId() == target_id));
        }) != edges.end())
    {
//        throw std::runtime_error("Edge already exists!");
        return;
    }

    std::shared_ptr<Vertex> first_node;
    std::shared_ptr<Vertex> second_node;

    for(const auto& n : nodes)
    {
        if(n.first == source_id)
            first_node = n.second;
        if(n.first == target_id)
            second_node = n.second;

    }

    int id_ = edges.size();
    edges.push_back(std::make_shared<Edge>(first_node, second_node, id_));
    edges.push_back(std::make_shared<Edge>(second_node, first_node, id_+1));
}

bool Graph::hasNode(int node_id) const {
//    return nodes.find(node_id) != nodes.end();
    for(const auto& n : nodes)
    {
        if(n.first == node_id)
            return true;
        }
    return false;
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
    return nodes[node_id].second;
}

const std::vector<std::pair<int, std::shared_ptr<Vertex>>> & Graph::getNodes() const {
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

std::vector<std::shared_ptr<Edge>> Graph::getEdgesFromNode(int node_id, int banned_id) const {
    std::vector<std::shared_ptr<Edge>> result = {};
    for(const auto& edge : edges)
    {
        if(edge->source->getId() == node_id && edge->target->getId() != banned_id)
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
                    bool has_node = false;
                    for(const auto& node_id_node : graph->nodes)
                    {
                        if(node_id_node.first == node_id) {
                            has_node = true;
                            break;
                        }
                    }
                    if(!has_node)
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

                    bool has_node = false;
                    for(const auto& node_id_node : graph->nodes)
                    {
                        if(node_id_node.first == node_id) {
                            has_node = true;
                            break;
                        }
                    }
                    if(!has_node) {
                        graph->addNode(std::make_shared<Vertex>(node_id));
                    }
                    graph->connectNodes(node, node_id);

                    buffer.clear();
                }
            }
        }
    }

    std::sort(graph->nodes.begin(), graph->nodes.end(), [](auto &left, auto &right) {
        return left.first < right.first;
    });

    return graph;
}
