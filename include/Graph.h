#ifndef GRAPH_CLASS_H
#define GRAPH_CLASS_H

#include "PointCloud.h"
#include <vector>
#include <glm/glm.hpp>

struct Edge {
    int id1, id2;
    float weight;
    bool operator<(const Edge& other) const { return weight < other.weight; }
};

class Graph {
private:
    std::vector<PointCloud::Point> points;  // points to build the graph
    std::vector<Edge> edges;                // edges for Kruskal
    std::vector<Vertice> vertices;          // GPU-ready vertices
    std::vector<int> parent;
    std::vector<int> rank;

    std::vector<glm::vec3> colors;

    int findSet(int i);
    void unionSet(int u, int v);

public:
    Graph();

    void clearGraph();
    void buildGraph(const std::vector<PointCloud::Point>& pts, int k, bool useEuclid);
    void kruskal();

    Vertice* getVerticesData();
    int getVerticesCount();
};

#endif
