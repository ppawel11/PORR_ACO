#include "graph.h"
#include <climits>
using namespace std;

Graph::Graph(const string &xml_file_path, const string &txt_file_path) {
    ifstream xml_stream(xml_file_path);
    ifstream txt_stream(txt_file_path);
    string line;
//   try{ //@TODO pewnie by wypadało zrobić obsługę błędów
        while(getline(xml_stream, line)){
            if(line.find("<link id=") != string::npos){
                int edge_id = stoi(line.substr(line.find('L') + 1, line.find('\"')));
                getline(xml_stream, line);
                int source = stoi(line.substr(line.find("N") + 1, line.find("/") - 1));
                getline(xml_stream, line);
                int target = stoi(line.substr(line.find('N') + 1, line.find("/") - 1));
                edges.emplace(edge_id, std::make_shared<Edge>(edge_id, 0, source, target));
                if(!nodes.count(source))
                    nodes.emplace(source, Vertex(source));
                nodes.at(source).edges.push_back(edges.at(edge_id));
                if(!nodes.count(target))
                    nodes.emplace(target, Vertex(target));
                nodes.at(target).edges.push_back(edges.at(edge_id));
            }
        }
    getline(txt_stream, line);
    if(line.find("current_load") != string::npos){
        currentLoad = stod(line.substr(line.find(",") + 1));
    } else ; //@todo throw error

    getline(txt_stream, line);
    if(line.find("capacity") != string::npos){
        capacity = stoi(line.substr(line.find(",") + 1));
    } else ; //@todo throw error

    int edgeId = -1;

    while(getline(txt_stream, line)){
        if(line.find("Lx") != string::npos){
            lX = stod(line.substr(line.find(",") + 1));
            continue;
        }
        else if(line.find("Ly") != string::npos){
            lY = stod(line.substr(line.find(",") + 1));
            continue;
        }
        else if(line.find("L") != string::npos){
            edgeId = stoi(line.substr(line.find("L") + 1, line.find(",")));
            edges.at(edgeId)->load = stoi(line.substr(line.find(",") + 1));
        }
        else if(line.find("source") != string::npos){
            source = stoi(line.substr(line.find("N") + 1));
        }
        else if(line.find("target") != string::npos){
            target = stoi(line.substr(line.find("N") + 1));
        }
        else if(line.find("value") != string::npos){
            value = stod(line.substr(line.find(",") + 1));
        }
        else if(line.find("S,") != string::npos){
            shortestPath = stoi(line.substr(line.find(",") + 1));
        }
        else if(line.find("bestY") != string::npos){
            bestY = stod(line.substr(line.find(",") + 1));
        }
        else
            ; //@TODO throw error
    }

//    }
//    catch(){
//        cout << "Error while parsing the graph!" << endl;
//           }
}


void Graph::dijkstra(vector<int> &path, vector<int> &xPath){
    int j;
    int u;
    int d[2*nodes.size()];
    int p[2*nodes.size()];
    bool alreadyChecked[2*nodes.size()];
    int commonEdgesTemp = 0;

    unordered_map<int, int> predecessors;
    predecessors.emplace(xPath[0], -99);
    for (int i = 1; i < xPath.size(); ++i) {
        predecessors.emplace(xPath[i], xPath[i-1]);
    }

    for(int i = 0; i < (int)2*nodes.size(); i++){
        auto t = nodes.find(i);
        if(t == nodes.end()){
            alreadyChecked[i] = true;
        }
        else{
            alreadyChecked[i] = false;
        }
        d[i] = INT_MAX;
        p[i] = -1;
    }

    d[source] = 0;

    for(int i = 0; i < (int)nodes.size(); i++) {
        for(j = 0; alreadyChecked[j]; j++);
        for(u = j++; j < (int)nodes.size()*2; j++){
            if(!alreadyChecked[j] && (d[j] < d[u]))
                u = j;
        }

        alreadyChecked[u] = true;
        int secondNode;


        for(int x = 0; x < (int)nodes.at(u).edges.size(); x++){
            if(nodes.at(u).edges[x]->load > lY * capacity){ //sprawdzanie warunku load
                continue;
            }


            if(nodes.at(u).edges[x]->node1 == u)
                secondNode = nodes.at(u).edges[x]->node2;
            else if(nodes.at(u).edges[x]->node2 == u)
                secondNode = nodes.at(u).edges[x]->node1;
            else{
                cout << "Error while searching for second node in Dijkstra algorithm." << endl;
                exit(1);
            }

            if(alreadyChecked[secondNode]){ // sprawdzanie czy sie nie zapetlamy
                continue;
            }

            if(d[secondNode] > d[u] + nodes.at(u).edges[x]->load) {
                auto t = predecessors.find(secondNode);
                if(t != predecessors.end()){
                    if(predecessors.at(secondNode) == u){
                        d[secondNode] = 1000000;
                        p[secondNode] = u;
                    }
                    else{
                        d[secondNode] = d[u] + nodes.at(u).edges[x]->load;
                        p[secondNode] = u;
                    }
                }
                else{
                    d[secondNode] = d[u] + nodes.at(u).edges[x]->load;
                    p[secondNode] = u;
                }
            }
        }
    }

    for(int i = 0; i < (int) 2*nodes.size(); i++){
        if(p[i] == -1)
            continue;
        auto t = predecessors.find(i);
        if(t != predecessors.end()){
            if(predecessors.at(i) == p[i])
                commonEdgesTemp++;
        }
    }

    commonEdges = commonEdgesTemp;

    long double yScoreTemp = 1;

    int print = target;
    for (int i = 0; i < 2*nodes.size(); ++i) {
        for(int x = 0; x < nodes.at(print).edges.size(); x++){
            if(p[print] == nodes.at(print).edges[x]->node1 || p[print] == nodes.at(print).edges[x]->node2){
                yScoreTemp *= (((long double)capacity - (long double)nodes.at(print).edges[x]->load)/(long double)capacity);
                calculatedBestY += log(((long double)capacity - (long double)nodes.at(print).edges[x]->load)/(long double)capacity);
            }
        }

        path.push_back(print);
        print = p[print];
        if(print == source)
            break;
        else if(print == -1){
            commonEdges = -1;
            return;
        }
    }
    scoreY = yScoreTemp;
    path.push_back(print);
}
