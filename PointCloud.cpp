#include "PointCloud.h"
#include <iostream>

PointCloud::PointCloud() {
}

std::vector<Point> matrix[dim][dim][dim];

int getCoordinate(double n, double min, double max) {
	
	double a = (max - min) / dim;

	for (int i = 1; i < dim; i++) {
		if (n < min + i * a) {
			return i-1;
		}
	}

	return dim - 1;
}

void PointCloud::buildPointCloud(const std::string &path, int n) {

	vertices.clear();

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			for (int k = 0; k < dim; k++) {
				matrix[i][j][k].clear();
			}
		}
	}

	std::ifstream file(path, std::ios::binary);

	if (file.is_open()) {
		Header header;

		file.read((char*)&header, sizeof(header));

		assert(header.versionMajor == 1 && header.versionMinor == 2);
		assert(header.headerSize == sizeof(header));
		assert(header.pointDataRecordID == 1);

		file.seekg(header.pointDataOffset);

		for (uint32_t i = 0; i < n; i++) {
			PointData pointDataBuffer;
			file.read((char*)&pointDataBuffer, sizeof(PointData));

			Point point = {
				point.id = i,
				point.x = (float)((((float)(pointDataBuffer.x * header.scaleX)) - header.minX) / (header.maxX - header.minX)) * 2 - 1,
				point.y = (float)((((float)(pointDataBuffer.z * header.scaleZ)) - header.minZ) / (header.maxZ - header.minZ)) * 2 - 1,
				point.z = (float)((((float)(pointDataBuffer.y * header.scaleY)) - header.minY) / (header.maxY - header.minY)) * 2 - 1,
				point.intensity = pointDataBuffer.intensity
			};

			int colorIndex = std::ceil(((float)pointDataBuffer.intensity / 100.f)*5)-1;
			if (colorIndex > 4) {
				colorIndex = 4;
			}

			Vertice vertice = {
				vertice.x = point.x,
				vertice.y = point.y,
				vertice.z = point.z,
				vertice.color = colors[colorIndex]
			};

			//std::cout << vertice.x  <<" " << vertice.y << " " << vertice.z<<"\n";

			int x = getCoordinate(point.x, -1, 1);
			int y = getCoordinate(point.y, -1, 1);
			int z = getCoordinate(point.z, -1, 1);
			
			matrix[x][y][z].push_back(point);
			vertices.push_back(vertice);

		}

		if (!file.good()) {
			throw std::runtime_error::runtime_error("File not good");
		}

	}
	else {
		throw std::runtime_error::runtime_error("File can't be read");
	}

}

int PointCloud::getVerticesCount() {
	return vertices.size();
}

Vertice* PointCloud::getVerticesData() {
	return vertices.data();
}

Color::Color(float r, float g, float b) {
	this->r = r;
	this->g = g;
	this->b = b;
}
