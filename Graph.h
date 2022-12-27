#ifndef GRAPH_CLASS_H
#define GRAPH_CLASS_H

#include"PointCloud.h"

struct Edge {
    Point p1, p2;
    float weight;
    bool operator<(Edge const& other) {
        return weight < other.weight;
    }
};

class Graph {
private:
    std::vector<Edge> startingGraph; 
    std::vector<Vertice> vertices;
    int* parent;
    int* rank;
    Color colors[5] =
    {
        Color(0.011f, 0.866f, 0.737f),
        Color(0.011f, 0.866f, 0.329f),
        Color(0.866f, 0.866f, 0.011f),
        Color(0.866f, 0.537f, 0.011f),
        Color(0.866f, 0.011f, 0.145f)
    };
public:
    Graph();
    void buildGraph(bool useEuclid, int n);
    int findSet(int i);
    void unionSet(int u, int v);
    void kruskal();
    Vertice* getVerticesData();
    int getVerticesCount();
    void clearGraph();
};

#endif
