#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <vector>

#include <okami/custommodelpkg.hpp>
#include <okami/itemtype.hpp>
#include <wolf_framework.hpp>

namespace okami::custommodelpkg
{
namespace
{

#define ALIGN_UP_BY(value, alignment) (((value) + (alignment) - 1) & ~((alignment) - 1))

constexpr uint32_t kAlignment = 0x10;
constexpr uint32_t kScrId = 0x00726373;
constexpr uint32_t kMdbId = 0x0062646D;
constexpr uint32_t kMeshType = 0x14; // static entity

constexpr uint32_t kSubMeshCount = 6;
constexpr uint16_t kVerticesPerFace = 4;
constexpr float kHalfExtent = 5.0f; // half-width of the cube (±5 units = 10×10×10)

#pragma pack(push, 1)

struct ScrHeader
{
    uint32_t scrId;
    uint32_t fileType;
    uint32_t meshCount;
    uint32_t padding;
};

struct MdbHeader
{
    uint32_t mdbId;
    uint32_t meshType;
    uint16_t meshId;
    uint16_t subMeshCount;
    uint8_t padding[20];
};

struct MdHeader
{
    uint32_t vertexOffset;
    uint32_t unknown1;
    uint32_t textureMapOffset;
    uint32_t colorWeightOffset;
    uint32_t textureUvOffset;
    uint16_t vertexCount;
    uint16_t textureIndex;
};

struct MdVertex
{
    float x, y, z;
    uint16_t connection;
    uint16_t pad;
};

struct TextureMap
{
    int16_t u, v;
};

struct ColorWeight
{
    uint8_t r, g, b, a;
};

struct MdTransform
{
    int16_t unk1;
    int16_t unk2;
    uint32_t unk3;
    float scaleX, scaleY, scaleZ;
    float rotX, rotY, rotZ;
    float posX, posY, posZ;
    uint8_t padding[20];
};

#pragma pack(pop)

// Face vertex definitions for a box (±15.0 units), 4 vertices per face as triangle strips.
struct FaceVerts
{
    float v[4][3];
};

static constexpr FaceVerts kFaces[kSubMeshCount] = {
    // Front  (+Z)
    {{{-kHalfExtent, -kHalfExtent, +kHalfExtent},
      {+kHalfExtent, -kHalfExtent, +kHalfExtent},
      {-kHalfExtent, +kHalfExtent, +kHalfExtent},
      {+kHalfExtent, +kHalfExtent, +kHalfExtent}}},
    // Back   (-Z)
    {{{+kHalfExtent, -kHalfExtent, -kHalfExtent},
      {-kHalfExtent, -kHalfExtent, -kHalfExtent},
      {+kHalfExtent, +kHalfExtent, -kHalfExtent},
      {-kHalfExtent, +kHalfExtent, -kHalfExtent}}},
    // Top    (+Y)
    {{{-kHalfExtent, +kHalfExtent, +kHalfExtent},
      {+kHalfExtent, +kHalfExtent, +kHalfExtent},
      {-kHalfExtent, +kHalfExtent, -kHalfExtent},
      {+kHalfExtent, +kHalfExtent, -kHalfExtent}}},
    // Bottom (-Y)
    {{{-kHalfExtent, -kHalfExtent, -kHalfExtent},
      {+kHalfExtent, -kHalfExtent, -kHalfExtent},
      {-kHalfExtent, -kHalfExtent, +kHalfExtent},
      {+kHalfExtent, -kHalfExtent, +kHalfExtent}}},
    // Right  (+X)
    {{{+kHalfExtent, -kHalfExtent, +kHalfExtent},
      {+kHalfExtent, -kHalfExtent, -kHalfExtent},
      {+kHalfExtent, +kHalfExtent, +kHalfExtent},
      {+kHalfExtent, +kHalfExtent, -kHalfExtent}}},
    // Left   (-X)
    {{{-kHalfExtent, -kHalfExtent, -kHalfExtent},
      {-kHalfExtent, -kHalfExtent, +kHalfExtent},
      {-kHalfExtent, +kHalfExtent, -kHalfExtent},
      {-kHalfExtent, +kHalfExtent, +kHalfExtent}}},
};

/// Helper: pad buffer to the next multiple of `alignment`.
static void alignBuffer(std::vector<uint8_t> &buf, size_t alignment)
{
    size_t aligned = ALIGN_UP_BY(buf.size(), alignment);
    buf.resize(aligned, 0);
}

/// Helper: append raw bytes of a POD struct.
template <typename T> static void writeStruct(std::vector<uint8_t> &buf, const T &val)
{
    const auto *p = reinterpret_cast<const uint8_t *>(&val);
    buf.insert(buf.end(), p, p + sizeof(T));
}

/// Helper: append a uint32_t in little-endian (native on x86).
static void writeU32(std::vector<uint8_t> &buf, uint32_t val)
{
    writeStruct(buf, val);
}

/// Generate an MD binary blob for a single-color box with 6 faces.
static std::vector<uint8_t> generateBoxMD(uint8_t r, uint8_t g, uint8_t b)
{
    std::vector<uint8_t> buf;
    buf.reserve(1024);

    // ------ Compute sizes/offsets before writing ------

    const size_t mdHeaderAligned = ALIGN_UP_BY(sizeof(MdHeader), kAlignment);
    const size_t verticesSize = ALIGN_UP_BY(sizeof(MdVertex) * kVerticesPerFace, kAlignment);
    const size_t texMapSize = ALIGN_UP_BY(sizeof(TextureMap) * kVerticesPerFace, kAlignment);
    const size_t colorsSize = ALIGN_UP_BY(sizeof(ColorWeight) * kVerticesPerFace, kAlignment);
    const size_t subMeshSize = ALIGN_UP_BY(mdHeaderAligned + verticesSize + texMapSize + colorsSize, kAlignment);

    // MdHeader offsets are relative to MdHeader start position.
    const uint32_t vertexOffset = static_cast<uint32_t>(mdHeaderAligned);
    const uint32_t textureMapOffset = static_cast<uint32_t>(mdHeaderAligned + verticesSize);
    const uint32_t colorWeightOffset = static_cast<uint32_t>(mdHeaderAligned + verticesSize + texMapSize);

    // SubMesh offsets are relative to MdbHeader start.
    // subMeshOffset[0] = sizeof(MdbHeader) + 6*sizeof(uint32_t), aligned to 0x10
    const size_t subMeshOffsetBase = ALIGN_UP_BY(sizeof(MdbHeader) + kSubMeshCount * sizeof(uint32_t), kAlignment);

    // MdMeshSize = subMeshOffsetBase + 6 * subMeshSize, aligned to MESH_ALIGNMENT
    const size_t mdMeshSize = ALIGN_UP_BY(subMeshOffsetBase + kSubMeshCount * subMeshSize, kAlignment);

    // Transform offset from file start:
    //   ScrHeader(16) + 1*uint32_t(4) = 20, aligned to 0x10 = 32
    //   + mdMeshSize
    //   aligned to TRANSFORM_ALIGNMENT
    const size_t afterScrHeader = ALIGN_UP_BY(sizeof(ScrHeader) + sizeof(uint32_t), kAlignment);
    const size_t transformOffset = ALIGN_UP_BY(afterScrHeader + mdMeshSize, kAlignment);

    // ------ 1. ScrHeader ------
    ScrHeader scr{};
    scr.scrId = kScrId;
    scr.fileType = 0; // entity type
    scr.meshCount = 1;
    scr.padding = 0;
    writeStruct(buf, scr);

    // ------ 2. Transform offset (1 mesh = 1 offset) ------
    writeU32(buf, static_cast<uint32_t>(transformOffset));

    // ------ 3. Align to 0x10 ------
    alignBuffer(buf, kAlignment);

    // ------ 4. MdbHeader ------
    MdbHeader mdb{};
    mdb.mdbId = kMdbId;
    mdb.meshType = kMeshType;
    mdb.meshId = 0;
    mdb.subMeshCount = kSubMeshCount;
    std::memset(mdb.padding, 0, sizeof(mdb.padding));
    writeStruct(buf, mdb);

    // ------ 5. SubMesh offsets ------
    for (uint32_t i = 0; i < kSubMeshCount; i++)
    {
        uint32_t offset = static_cast<uint32_t>(subMeshOffsetBase + i * subMeshSize);
        writeU32(buf, offset);
    }

    // ------ 6. Align to 0x10 (SUBMESH_OFFSET_ALIGNMENT) ------
    alignBuffer(buf, kAlignment);

    // ------ 7. Write 6 submeshes ------
    for (uint32_t face = 0; face < kSubMeshCount; face++)
    {
        // MdHeader
        MdHeader hdr{};
        hdr.vertexOffset = vertexOffset;
        hdr.unknown1 = 0;
        hdr.textureMapOffset = textureMapOffset;
        hdr.colorWeightOffset = colorWeightOffset;
        hdr.textureUvOffset = 0;
        hdr.vertexCount = kVerticesPerFace;
        hdr.textureIndex = 0;
        writeStruct(buf, hdr);

        alignBuffer(buf, kAlignment);

        // Vertices
        for (uint16_t v = 0; v < kVerticesPerFace; v++)
        {
            MdVertex vert{};
            vert.x = kFaces[face].v[v][0];
            vert.y = kFaces[face].v[v][1];
            vert.z = kFaces[face].v[v][2];
            vert.connection = (v == 0) ? 0x8000 : 0x0000;
            vert.pad = 0;
            writeStruct(buf, vert);
        }

        alignBuffer(buf, kAlignment);

        // Texture map UVs (zeroed — no texture, vertex colors only)
        for (uint16_t v = 0; v < kVerticesPerFace; v++)
        {
            TextureMap tm{};
            tm.u = 0;
            tm.v = 0;
            writeStruct(buf, tm);
        }

        alignBuffer(buf, kAlignment);

        // Color weights
        for (uint16_t v = 0; v < kVerticesPerFace; v++)
        {
            ColorWeight cw{};
            cw.r = r;
            cw.g = g;
            cw.b = b;
            cw.a = 255;
            writeStruct(buf, cw);
        }

        alignBuffer(buf, kAlignment);

        // SUBMESH_ALIGNMENT — already aligned
    }

    // ------ 8. Mesh alignment ------
    alignBuffer(buf, kAlignment);

    // ------ 9. Transform alignment ------
    alignBuffer(buf, kAlignment);

    // ------ 10. MdTransform ------
    // The first 4 bytes of MdTransform are a SIGNED RELATIVE OFFSET from the
    // MdTransform position back to the MdbHeader. The game engine computes:
    //   MdbHeader_ptr = MdTransform_ptr + *(int32_t*)MdTransform_ptr
    // (see FUN_18020d770 which reads *piVar11 as an offset adjustment)
    const int32_t backOffset = static_cast<int32_t>(afterScrHeader) - static_cast<int32_t>(transformOffset);

    MdTransform xform{};
    std::memcpy(&xform.unk1, &backOffset, sizeof(backOffset)); // store as first 4 bytes
    xform.unk3 = 0;
    xform.scaleX = 1.0f;
    xform.scaleY = 1.0f;
    xform.scaleZ = 1.0f;
    xform.rotX = 0.0f;
    xform.rotY = 0.0f;
    xform.rotZ = 0.0f;
    xform.posX = 0.0f;
    xform.posY = 0.0f;
    xform.posZ = 0.0f;
    std::memset(xform.padding, 0, sizeof(xform.padding));
    writeStruct(buf, xform);

    return buf;
}

/// Build the in-memory resource package format expected by thunk_FUN_1801b1900.
/// The MD data must start at a 16-byte aligned offset because the renderer uses
/// movaps (SSE aligned load) to read vertex data from the MD blob.
static std::vector<uint8_t> buildInMemoryPackage(const std::vector<uint8_t> &mdData)
{
    // Layout:
    //   [uint32_t 1]           - entry count       (4 bytes)
    //   [uint32_t offset_md]   - byte offset       (4 bytes)
    //   ["MD\0\0"]             - type tag           (4 bytes)
    //   [4 bytes padding]      - align MD data to 16
    //   [MD binary data...]

    constexpr uint32_t entryCount = 1;
    constexpr uint32_t headerSize = sizeof(uint32_t)   // count
                                    + sizeof(uint32_t) // one offset
                                    + 4;               // one type tag
    // headerSize = 12; align to 16 so MD data starts at a 16-byte boundary.
    constexpr uint32_t offsetMd = ALIGN_UP_BY(headerSize, kAlignment);

    std::vector<uint8_t> pkg;
    pkg.reserve(offsetMd + mdData.size());

    // Entry count
    writeU32(pkg, entryCount);

    // Offset to MD data
    writeU32(pkg, offsetMd);

    // Type tag "MD\0\0"
    const uint8_t tag[4] = {'M', 'D', '\0', '\0'};
    pkg.insert(pkg.end(), tag, tag + 4);

    // Pad to 16-byte alignment
    alignBuffer(pkg, kAlignment);

    // MD binary data
    pkg.insert(pkg.end(), mdData.begin(), mdData.end());

    return pkg;
}

// Static storage for generated packages, keyed by entity type ID (0x0Axx).
static std::unordered_map<uint32_t, std::vector<uint8_t>> s_packages;

} // anonymous namespace

void initialize()
{
    struct DummyDef
    {
        uint8_t itemId;
        uint8_t r, g, b;
    };

    constexpr DummyDef defs[] = {
        {ItemTypes::ForeignStandardItem, 80, 120, 255},    // Blue
        {ItemTypes::ForeignProgressionItem, 180, 80, 255}, // Purple
        {ItemTypes::ForeignTrapItem, 255, 80, 80},         // Red
        {ItemTypes::OkamiStandardItem, 80, 200, 200},      // Cyan
        {ItemTypes::OkamiProgressionItem, 255, 200, 50},   // Gold
        {ItemTypes::OkamiTrapItem, 255, 140, 50},          // Orange
    };

    for (const auto &d : defs)
    {
        uint32_t entityType = 0x0A00u | d.itemId;
        auto md = generateBoxMD(d.r, d.g, d.b);
        s_packages[entityType] = buildInMemoryPackage(md);
        wolf::logInfo("[custommodelpkg] Built model package for entity type 0x%04X (%zu bytes)", entityType, s_packages[entityType].size());
    }
}

bool isApDummyEntity(uint32_t entityTypeId)
{
    return s_packages.count(entityTypeId) > 0;
}

void *getPackageForEntity(uint32_t entityTypeId)
{
    auto it = s_packages.find(entityTypeId);
    return it != s_packages.end() ? it->second.data() : nullptr;
}

} // namespace okami::custommodelpkg
