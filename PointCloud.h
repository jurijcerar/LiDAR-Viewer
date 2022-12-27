#ifndef POINTCLOUD_CLASS_H
#define POINTCLOUD_CLASS_H

#include<fstream>
#include<vector>
#include<algorithm>
#include<cassert>
#include<cmath>

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

struct Point {
	int id;
	float x, y, z;
	int intensity;
};

const int dim = 256;

extern std::vector<Point> matrix[dim][dim][dim];

struct Color {
	float r, g, b;
	Color(float r, float g, float b);
};

struct Vertice {
	float x, y, z;
	Color color;
};

class PointCloud {
private:
	std::vector<Vertice> vertices;

	Color colors[5] =
	{ 
		Color(0.011f, 0.866f, 0.737f),
		Color(0.011f, 0.866f, 0.329f),
		Color(0.866f, 0.866f, 0.011f),
		Color(0.866f, 0.537f, 0.011f),
		Color(0.866f, 0.011f, 0.145f)
	};
public:
	PointCloud();
	void buildPointCloud(const std::string& path, int n);
	int getVerticesCount();
	Vertice* getVerticesData();
};

#endif