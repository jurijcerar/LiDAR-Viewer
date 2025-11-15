#include "Graph.h"
#include <nanoflann.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace nanoflann;

// KD-tree adapter for PointCloud::Point
struct PointCloudAdaptor {
    const std::vector<PointCloud::Point>& pts;
    PointCloudAdaptor(const std::vector<PointCloud::Point>& points) : pts(points) {}
    inline size_t kdtree_get_point_count() const { return pts.size(); }
    inline float kdtree_get_pt(const size_t idx, const size_t dim) const {
        if (dim == 0) return pts[idx].position.x;
        if (dim == 1) return pts[idx].position.y;
        return pts[idx].position.z;
    }
    template<class BBOX> bool kdtree_get_bbox(BBOX&) const { return false; }
};

Graph::Graph() {
    colors = {
        {0.011f, 0.866f, 0.737f},
        {0.011f, 0.866f, 0.329f},
        {0.866f, 0.866f, 0.011f},
        {0.866f, 0.537f, 0.011f},
        {0.866f, 0.011f, 0.145f}
    };
}

void Graph::clearGraph() {
    points.clear();
    edges.clear();
    vertices.clear();
    parent.clear();
    rank.clear();
}

int Graph::findSet(int i) {
    if (parent[i] != i) parent[i] = findSet(parent[i]);
    return parent[i];
}

void Graph::unionSet(int u, int v) {
    u = findSet(u);
    v = findSet(v);
    if (u == v) return;
    if (rank[u] < rank[v]) std::swap(u, v);
    parent[v] = u;
    if (rank[u] == rank[v]) rank[u]++;
}

void Graph::buildGraph(const std::vector<PointCloud::Point>& pts, int k, bool useEuclid) {
    points = pts;
    edges.clear();
    vertices.clear();

    int n = points.size();
    parent.resize(n);
    rank.resize(n, 0);
    for (int i = 0; i < n; i++) parent[i] = i;

    // Build KD-tree
    PointCloudAdaptor adaptor(points);
    typedef KDTreeSingleIndexAdaptor<
        L2_Simple_Adaptor<float, PointCloudAdaptor>,
        PointCloudAdaptor, 3
    > KDTree;

    KDTree tree(3, adaptor, KDTreeSingleIndexAdaptorParams(10));
    tree.buildIndex();

    // For each point, find k nearest neighbors
    for (int i = 0; i < n; i++) {
        std::vector<size_t> ret_index(k + 1);
        std::vector<float> out_dist_sqr(k + 1);
        nanoflann::KNNResultSet<float> resultSet(k + 1);
        resultSet.init(&ret_index[0], &out_dist_sqr[0]);
        float query[3] = { points[i].position.x, points[i].position.y, points[i].position.z };
        tree.findNeighbors(resultSet, query, nanoflann::SearchParameters(10));

        for (size_t j = 0; j < ret_index.size(); j++) {
            if (ret_index[j] == i) continue; // skip self
            float weight;
            if (useEuclid) {
                weight = out_dist_sqr[j]; // squared Euclidean distance
            } else {
                weight = std::abs(points[i].intensity - points[ret_index[j]].intensity);
            }
            edges.push_back({ i, static_cast<int>(ret_index[j]), weight });
        }
    }
}

void Graph::kruskal() {
    std::sort(edges.begin(), edges.end());
    vertices.clear();

    for (auto& e : edges) {
        if (findSet(e.id1) != findSet(e.id2)) {
            int idx1 = std::min(4, static_cast<int>(std::ceil(points[e.id1].intensity / 100.f * 5) - 1));
            int idx2 = std::min(4, static_cast<int>(std::ceil(points[e.id2].intensity / 100.f * 5) - 1));

            Vertice v1 = { points[e.id1].position.x, points[e.id1].position.y, points[e.id1].position.z,
                            colors[idx1].r, colors[idx1].g, colors[idx1].b };
            Vertice v2 = { points[e.id2].position.x, points[e.id2].position.y, points[e.id2].position.z,
                            colors[idx2].r, colors[idx2].g, colors[idx2].b };

            vertices.push_back(v1);
            vertices.push_back(v2);

            unionSet(e.id1, e.id2);
        }
    }
}

Vertice* Graph::getVerticesData() { return vertices.data(); }
int Graph::getVerticesCount() { return vertices.size(); }
