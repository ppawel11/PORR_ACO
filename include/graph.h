class Graph{
public:
    std::unordered_map<int, Vertex> nodes;
    std::unordered_map<int, std::shared_ptr<Edge>> edges;
    double currentLoad;
    int capacity;
    int source;
    int target;
    double value; //(chyba) ne wczyt
    int shortestPath;
    double lX;
    double lY;
    long double scoreY;
    double bestY;
    long double calculatedBestY;
    int commonEdges;

    Graph(const string &xml_file_path, const string &txt_file_path);
    Graph(){};
    void dijkstra(vector<int> &path, vector<int> &xPath);
};
