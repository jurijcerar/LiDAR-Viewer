#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Vertex.h"

class PointCloud {
public:
    struct Point {
        int       id;
        glm::vec3 position;
        float     intensity;
    };

    PointCloud() = default;

    // Load up to maxPoints from any LAS 1.0–1.4 file (all point formats).
    // Throws std::runtime_error on failure.
    void loadFromFile(const std::string& path, int maxPoints = 120000);

    const std::vector<Point>&  getPoints()      const { return points_; }
    const std::vector<Vertex>& getVertices()    const { return vertices_; }
    int                        getVertexCount() const { return static_cast<int>(vertices_.size()); }

private:
    std::vector<Point>  points_;
    std::vector<Vertex> vertices_;

    static const std::vector<glm::vec3> kPalette;
    static Vertex makeVertex(const Point& p);
};
