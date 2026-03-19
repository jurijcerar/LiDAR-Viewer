#include "PointCloud.h"

#include <fstream>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <cmath>

// ─── Colormap ─────────────────────────────────────────────────────────────────
// Viridis-inspired perceptual gradient: dark purple → blue → teal → green → yellow
static const glm::vec3 kColormap[] = {
    {0.267f, 0.005f, 0.329f},   // 0.0  deep purple
    {0.283f, 0.141f, 0.458f},   // 0.14 purple-blue
    {0.254f, 0.265f, 0.530f},   // 0.28 blue
    {0.164f, 0.471f, 0.558f},   // 0.43 teal-blue
    {0.128f, 0.566f, 0.551f},   // 0.57 teal
    {0.180f, 0.663f, 0.490f},   // 0.71 green-teal
    {0.475f, 0.762f, 0.330f},   // 0.86 yellow-green
    {0.993f, 0.906f, 0.144f},   // 1.0  yellow
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

// ─── LAS public header (covers 1.0–1.4 common fields) ────────────────────────
#pragma pack(push, 1)
struct LasPublicHeader {
    char     signature[4];
    uint16_t fileSourceID;
    uint16_t globalEncoding;
    uint32_t guidData1;
    uint16_t guidData2;
    uint16_t guidData3;
    uint8_t  guidData4[8];
    uint8_t  versionMajor;
    uint8_t  versionMinor;
    char     systemIdentifier[32];
    char     generatingSoftware[32];
    uint16_t creationDOY;
    uint16_t creationYear;
    uint16_t headerSize;
    uint32_t pointDataOffset;
    uint32_t numVLRs;
    uint8_t  pointDataFormatID;
    uint16_t pointDataRecordLength;
    uint32_t legacyPointCount;
    uint32_t legacyPointsByReturn[5];
    double   scaleX, scaleY, scaleZ;
    double   offsetX, offsetY, offsetZ;
    double   maxX, minX;
    double   maxY, minY;
    double   maxZ, minZ;
};
#pragma pack(pop)

struct RawXYZ { int32_t x, y, z; };
static constexpr size_t kIntensityOffset = 12;

// ─── loadFromFile ─────────────────────────────────────────────────────────────
void PointCloud::loadFromFile(const std::string& path, int maxPoints) {
    points_.clear();
    vertices_.clear();

    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot open file: " + path);

    LasPublicHeader hdr{};
    f.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (!f) throw std::runtime_error("File too short to contain LAS header");

    if (std::strncmp(hdr.signature, "LASF", 4) != 0)
        throw std::runtime_error("Not a LAS file (bad signature)");
    if (hdr.versionMajor != 1 || hdr.versionMinor > 4)
        throw std::runtime_error("Unsupported LAS version");
    if (hdr.pointDataFormatID > 10)
        throw std::runtime_error("Unknown point data format ID");

    uint64_t totalPoints = hdr.legacyPointCount;
    if (hdr.versionMinor >= 4 && totalPoints == 0) {
        f.seekg(247, std::ios::beg);
        uint64_t ext = 0;
        f.read(reinterpret_cast<char*>(&ext), 8);
        totalPoints = ext;
    }

    f.seekg(hdr.pointDataOffset, std::ios::beg);

    const uint16_t recLen = hdr.pointDataRecordLength;
    if (recLen < sizeof(RawXYZ) + sizeof(uint16_t))
        throw std::runtime_error("Point record too short");

    const uint64_t limit = (totalPoints > 0)
        ? std::min(static_cast<uint64_t>(maxPoints), totalPoints)
        : static_cast<uint64_t>(maxPoints);

    const double safeRX = (hdr.maxX > hdr.minX) ? (hdr.maxX - hdr.minX) : 1.0;
    const double safeRY = (hdr.maxY > hdr.minY) ? (hdr.maxY - hdr.minY) : 1.0;
    const double safeRZ = (hdr.maxZ > hdr.minZ) ? (hdr.maxZ - hdr.minZ) : 1.0;

    points_.reserve(static_cast<size_t>(limit));
    std::vector<char> buf(recLen);

    // ── Pass 1: read points, track intensity range ────────────────────────────
    float intensityMin =  1e30f;
    float intensityMax = -1e30f;

    for (uint64_t i = 0; i < limit; ++i) {
        f.read(buf.data(), recLen);
        if (!f) break;

        RawXYZ raw;
        std::memcpy(&raw, buf.data(), sizeof(raw));

        uint16_t rawIntensity;
        std::memcpy(&rawIntensity, buf.data() + kIntensityOffset, sizeof(rawIntensity));

        Point p;
        p.id = static_cast<int>(i);

        double wx = raw.x * hdr.scaleX + hdr.offsetX;
        double wy = raw.y * hdr.scaleY + hdr.offsetY;
        double wz = raw.z * hdr.scaleZ + hdr.offsetZ;
        p.position.x = static_cast<float>((wx - hdr.minX) / safeRX * 2.0 - 1.0);
        p.position.y = static_cast<float>((wz - hdr.minZ) / safeRZ * 2.0 - 1.0);
        p.position.z = static_cast<float>((wy - hdr.minY) / safeRY * 2.0 - 1.0);
        p.intensity   = static_cast<float>(rawIntensity);

        intensityMin = std::min(intensityMin, p.intensity);
        intensityMax = std::max(intensityMax, p.intensity);

        points_.push_back(p);
    }

    // ── Pass 2: build vertices using actual intensity range ───────────────────
    const float intensityRange = (intensityMax > intensityMin)
                                 ? (intensityMax - intensityMin)
                                 : 1.0f;

    vertices_.reserve(points_.size());
    for (const auto& p : points_) {
        float t = (p.intensity - intensityMin) / intensityRange;
        glm::vec3 c = sampleColormap(t);
        vertices_.push_back({ p.position.x, p.position.y, p.position.z, c.r, c.g, c.b });
    }
}
