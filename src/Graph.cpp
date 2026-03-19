#include "Graph.h"
#include <nanoflann.hpp>
#include <algorithm>
#include <cmath>
#include <thread>
#include <vector>

using namespace nanoflann;

// ─── Colormap (same viridis-inspired as PointCloud) ───────────────────────────
static const glm::vec3 kColormap[] = {
    {0.267f, 0.005f, 0.329f},
    {0.283f, 0.141f, 0.458f},
    {0.254f, 0.265f, 0.530f},
    {0.164f, 0.471f, 0.558f},
    {0.128f, 0.566f, 0.551f},
    {0.180f, 0.663f, 0.490f},
    {0.475f, 0.762f, 0.330f},
    {0.993f, 0.906f, 0.144f},
};
static constexpr int kColormapSize = 8;

static glm::vec3 sampleColormap(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    float scaled = t * (kColormapSize - 1);
    int   lo     = static_cast<int>(scaled);
    int   hi     = std::min(lo + 1, kColormapSize - 1);
    float frac   = scaled - lo;
    return kColormap[lo] * (1.0f - frac) + kColormap[hi] * frac;
}

// ─── KD-tree adaptor ──────────────────────────────────────────────────────────
struct PointAdaptor {
    const std::vector<PointCloud::Point>& pts;
    explicit PointAdaptor(const std::vector<PointCloud::Point>& p) : pts(p) {}
    size_t kdtree_get_point_count() const { return pts.size(); }
    float kdtree_get_pt(size_t idx, size_t dim) const {
        return (&pts[idx].position.x)[dim];
    }
    template<class B> bool kdtree_get_bbox(B&) const { return false; }
};

using KDTree = KDTreeSingleIndexAdaptor<
    L2_Simple_Adaptor<float, PointAdaptor>,
    PointAdaptor, 3>;

// ─── Graph ────────────────────────────────────────────────────────────────────
void Graph::clear() {
    points_.clear();
    edges_.clear();
    vertices_.clear();
    parent_.clear();
    rank_.clear();
    intensityMin_ = 0.0f;
    intensityMax_ = 1.0f;
}

int Graph::findSet(int i) {
    if (parent_[i] != i) parent_[i] = findSet(parent_[i]);
    return parent_[i];
}

void Graph::unionSet(int u, int v) {
    u = findSet(u); v = findSet(v);
    if (u == v) return;
    if (rank_[u] < rank_[v]) std::swap(u, v);
    parent_[v] = u;
    if (rank_[u] == rank_[v]) rank_[u]++;
}

void Graph::buildGraph(const std::vector<PointCloud::Point>& pts, int k) {
    points_ = pts;
    edges_.clear();
    vertices_.clear();

    const int n = static_cast<int>(points_.size());
    parent_.resize(n);
    rank_.assign(n, 0);
    for (int i = 0; i < n; i++) parent_[i] = i;

    // Track intensity range from the copied points
    intensityMin_ =  1e30f;
    intensityMax_ = -1e30f;
    for (const auto& p : points_) {
        intensityMin_ = std::min(intensityMin_, p.intensity);
        intensityMax_ = std::max(intensityMax_, p.intensity);
    }
    if (intensityMax_ <= intensityMin_) intensityMax_ = intensityMin_ + 1.0f;

    PointAdaptor adaptor(points_);
    KDTree tree(3, adaptor, KDTreeSingleIndexAdaptorParams(10));
    tree.buildIndex();

    const int nThreads = std::max(1u, std::thread::hardware_concurrency());
    std::vector<std::vector<Edge>> localEdges(nThreads);

    auto worker = [&](int tid) {
        const int chunk = (n + nThreads - 1) / nThreads;
        const int start = tid * chunk;
        const int end   = std::min(start + chunk, n);

        std::vector<size_t> idx(k + 1);
        std::vector<float>  dist(k + 1);

        for (int i = start; i < end; i++) {
            nanoflann::KNNResultSet<float> rs(k + 1);
            rs.init(idx.data(), dist.data());
            float q[3] = { points_[i].position.x, points_[i].position.y, points_[i].position.z };
            tree.findNeighbors(rs, q, nanoflann::SearchParameters(10));

            for (size_t j = 0; j < rs.size(); j++) {
                if (static_cast<int>(idx[j]) == i) continue;
                localEdges[tid].push_back({ i, static_cast<int>(idx[j]), dist[j] });
            }
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(nThreads);
    for (int t = 0; t < nThreads; t++)
        threads.emplace_back(worker, t);
    for (auto& th : threads)
        th.join();

    size_t total = 0;
    for (auto& le : localEdges) total += le.size();
    edges_.reserve(total);
    for (auto& le : localEdges)
        edges_.insert(edges_.end(), le.begin(), le.end());
}

void Graph::kruskal() {
    std::sort(edges_.begin(), edges_.end());
    vertices_.clear();

    const float range = intensityMax_ - intensityMin_;

    for (const auto& e : edges_) {
        if (findSet(e.id1) != findSet(e.id2)) {
            const auto& p1 = points_[e.id1];
            const auto& p2 = points_[e.id2];

            float t1 = (p1.intensity - intensityMin_) / range;
            float t2 = (p2.intensity - intensityMin_) / range;

            glm::vec3 c1 = sampleColormap(t1);
            glm::vec3 c2 = sampleColormap(t2);

            vertices_.push_back({ p1.position.x, p1.position.y, p1.position.z, c1.r, c1.g, c1.b });
            vertices_.push_back({ p2.position.x, p2.position.y, p2.position.z, c2.r, c2.g, c2.b });

            unionSet(e.id1, e.id2);
        }
    }
}
