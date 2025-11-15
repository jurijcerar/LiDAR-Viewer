#ifndef POINTCLOUD_CLASS_H
#define POINTCLOUD_CLASS_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

struct Vertice {
    float x, y, z;
    float r, g, b;
};

class PointCloud {
public:
    struct Point {
        int id;
        glm::vec3 position;
        float intensity;
    };
private:
	std::vector<Point> points;       // Original point cloud
    std::vector<Vertice> vertices;   // GPU-ready vertices
    std::vector<glm::vec3> colors;   // Color palette

public:
	PointCloud();

	void loadFromFile(const std::string& path, int n);

	// Accessors
    const std::vector<Point>& getPoints() const { return points; }
    const std::vector<Vertice>& getVertices() const { return vertices; }
    int getVerticesCount() const { return static_cast<int>(vertices.size()); }
};

#endif