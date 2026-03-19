#pragma once

#include "PointCloud.h"
#include <vector>

struct Edge {
    int   id1, id2;
    float weight;
    bool operator<(const Edge& o) const { return weight < o.weight; }
};

class Graph {
public:
    Graph() = default;

    void clear();

    // Build k-NN graph using Euclidean distance (threaded).
    void buildGraph(const std::vector<PointCloud::Point>& pts, int k);

    // Kruskal MST — populates GPU vertex buffer.
    void kruskal();

    Vertex* getVertexData()  { return vertices_.data(); }
    int     getVertexCount() { return static_cast<int>(vertices_.size()); }

private:
    std::vector<PointCloud::Point> points_;
    std::vector<Edge>              edges_;
    std::vector<Vertex>            vertices_;
    std::vector<int>               parent_;
    std::vector<int>               rank_;

    float intensityMin_ = 0.0f;
    float intensityMax_ = 1.0f;

    int  findSet(int i);
    void unionSet(int u, int v);
};
