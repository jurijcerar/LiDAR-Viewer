#include "PointCloud.h"
#include <fstream>
#include <cassert>
#include <cmath>
#include <iostream>


#pragma pack(push, 1)
struct Header {
	char magic[4];
	uint16_t fileSourceId;
	uint16_t globalEncoding;
	uint32_t guidData1;
	uint16_t guidData2;
	uint16_t guidData3;
	uint8_t guidData4[8];
	uint8_t versionMajor;
	uint8_t versionMinor;
	char systemIdentifier[32];
	char generatingSoftware[32];
	uint16_t creationDay;
	uint16_t creationYear;
	uint16_t headerSize;
	uint32_t pointDataOffset;
	uint32_t numberOfVariableLengthRecords;
	uint8_t pointDataRecordID;
	uint16_t pointDataRecordLength;
	uint32_t numberOfPointRecords;
	uint32_t numberOfPointsByReturn[5];
	double scaleX, scaleY, scaleZ;
	double offsetX, offsetY, offsetZ;
	double maxX, minX;
	double maxY, minY;
	double maxZ, minZ;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PointData {
	uint32_t x, y, z;
	uint16_t intensity;
	uint8_t flags;
	uint8_t classification;
	uint8_t scanAngleRank;
	uint8_t userData;
	uint16_t pointSourceID;
	double gpsTime;
};
#pragma pack(pop)


PointCloud::PointCloud() {
    colors = {
        {0.011f, 0.866f, 0.737f},
        {0.011f, 0.866f, 0.329f},
        {0.866f, 0.866f, 0.011f},
        {0.866f, 0.537f, 0.011f},
        {0.866f, 0.011f, 0.145f}
    };
}


void PointCloud::loadFromFile(const std::string &path, int n) {
    points.clear();
    vertices.clear();

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("File can't be read");

    Header header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    assert(header.versionMajor == 1 && header.versionMinor == 2);
    assert(header.pointDataRecordID == 1);

    file.seekg(header.pointDataOffset);

    for (uint32_t i = 0; i < n; i++) {
        PointData pd;
        file.read(reinterpret_cast<char*>(&pd), sizeof(PointData));

        Point p;
        p.id = i;
        p.position.x = (float)(((pd.x * header.scaleX) - header.minX) / (header.maxX - header.minX) * 2 - 1);
        p.position.y = (float)(((pd.z * header.scaleZ) - header.minZ) / (header.maxZ - header.minZ) * 2 - 1);
        p.position.z = (float)(((pd.y * header.scaleY) - header.minY) / (header.maxY - header.minY) * 2 - 1);
        p.intensity = pd.intensity;

        points.push_back(p);

        // Map intensity to color
        int colorIndex = std::min(4, static_cast<int>(std::ceil((p.intensity / 100.f) * 5) - 1));

        Vertice v = {
            p.position.x, p.position.y, p.position.z,
            colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b
        };
        vertices.push_back(v);
    }
}