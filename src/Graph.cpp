#include "Graph.h"
#include <iostream>
#include <chrono>
#include <cstdint>

Graph::Graph() {
    startingGraph.clear();
    vertices.clear();
}

void buildEdges(std::vector<Point> points, std::vector<Edge>& startingGraph, bool useEuclid, Point p) {
    for (int m = 0; m < points.size(); m++) {
        float weight;
        if (useEuclid) {
            weight = (float)(pow((p.x - points[m].x), 2) + pow((p.y - points[m].y), 2) + pow((p.z - points[m].z), 2));
        }
        else {
            weight = std::abs(p.intensity - points[m].intensity);
        }
        Edge e = {
            e.p1 = p,
            e.p2 = points[m],
            e.weight = weight
        };
        startingGraph.push_back(e);
    }
}

void Graph::buildGraph(bool useEuclid, int n) {
    parent = new int[n];
    rank = new int[n];

    startingGraph.clear();
    vertices.clear();

    for (int i = 0; i < n; i++) {
        parent[i] = i;
        rank[i] = 0;
    }

    int temp;

    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            for (int k = 0; k < dim; k++) {
                for (int l = 0; l < matrix[i][j][k].size(); l++) {
                    buildEdges(matrix[i][j][k], startingGraph, useEuclid, matrix[i][j][k][l]);
                    temp = 1;
                    if (useEuclid) {
                        while (matrix[i + temp][j][k].size() == 0 && matrix[i + temp][j + temp][k].size() == 0 && matrix[i + temp][j + temp][k + temp].size() == 0
                            && matrix[i][j + temp][k].size() == 0 && matrix[i][j + temp][k + temp].size() == 0 && matrix[i][j][k + temp].size() == 0
                            && matrix[i + temp][j][k + temp].size() == 0 && temp + i < dim - 1 && temp + j < dim - 1 && temp + k < dim - 1) {
                            temp++;
                        }
                    }
                    if (i < dim - 1) {
                        buildEdges(matrix[i + temp][j][k], startingGraph, useEuclid, matrix[i][j][k][l]);
                        if (j < dim - 1) {
                            buildEdges(matrix[i + temp][j + temp][k], startingGraph, useEuclid, matrix[i][j][k][l]);
                            if (k < dim - 1) {
                                buildEdges(matrix[i + temp][j + temp][k + temp], startingGraph, useEuclid, matrix[i][j][k][l]);
                            }
                        }
                        if (k < dim - 1) {
                            buildEdges(matrix[i + temp][j][k + temp], startingGraph, useEuclid, matrix[i][j][k][l]);
                        }
                    }
                    if (j < dim - 1) {
                        buildEdges(matrix[i][j + temp][k], startingGraph, useEuclid, matrix[i][j][k][l]);
                        if (k < dim - 1) {
                            buildEdges(matrix[i][j + temp][k + temp], startingGraph, useEuclid, matrix[i][j][k][l]);
                        }
                    }
                    if (k < dim - 1) {
                        buildEdges(matrix[i][j][k + temp], startingGraph, useEuclid, matrix[i][j][k][l]);
                    }
                }
            }
        }
    }
}


int Graph::findSet(int i) {
    if (i == parent[i])
        return i;
    else
        return parent[i] = findSet(parent[i]);
}

void Graph::unionSet(int u, int v) {
    u = findSet(u);
    v = findSet(v);
    if (u != v) {
        if (rank[u] < rank[v]) {
            std::swap(u, v);
        }
        parent[v] = u;
        if (rank[u] == rank[v]) {
            rank[u]++;
        }
    }
}

void Graph::kruskal() {
    std::cout << startingGraph.size()<<"\n";
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    std::sort(startingGraph.begin(), startingGraph.end());
    end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    std::cout << "elapsed time sort: " << elapsed_seconds.count() << "s\n";
    for (auto& g : startingGraph) {
        if (findSet(g.p1.id) != findSet(g.p2.id)) {
            int colorIndex = std::ceil((g.p1.intensity / 100.f) * 5) - 1;
            if (colorIndex > 4) {
                colorIndex = 4;
            }
            Vertice v1 = {
                v1.x = g.p1.x,
                v1.y = g.p1.y,
                v1.z = g.p1.z,
                v1.color = colors[colorIndex]
            };

            colorIndex = std::ceil((g.p2.intensity / 100.f) * 5) - 1;
            if (colorIndex > 4) {
                colorIndex = 4;
            }
            Vertice v2 = {
                v2.x = g.p2.x,
                v2.y = g.p2.y,
                v2.z = g.p2.z,
                v2.color = colors[colorIndex]
            };
            vertices.push_back(v1);
            vertices.push_back(v2);
            unionSet(g.p1.id, g.p2.id);
        }
    }
}

Vertice* Graph::getVerticesData() {
    return vertices.data();
}

int Graph::getVerticesCount() {
    return vertices.size();
}

void Graph::clearGraph() {
    vertices.clear();
    startingGraph.clear();
}
